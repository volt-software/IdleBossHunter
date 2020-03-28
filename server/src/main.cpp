/*
    IdleBossHunter
    Copyright (C) 2020 Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <spdlog/spdlog.h>
#include <atomic>
#include <functional>
#include <csignal>
#include <chrono>
#include <filesystem>

#include <entt/entt.hpp>
#include <asset_loading/load_assets.h>
#include <asset_loading/load_from_database.h>
#include <game_logic/logic_helpers.h>
#include <game_logic/censor_sensor.h>
#include <sodium.h>
#include <messages/update_response.h>
#include <game_queue_message_handlers/player_enter_handler.h>
#include <game_queue_message_handlers/player_leave_handler.h>
#include <game_queue_message_handlers/clan/accept_application_handler.h>
#include <game_queue_message_handlers/clan/create_clan_handler.h>
#include <game_queue_message_handlers/clan/increase_bonus_handler.h>
#include <game_queue_message_handlers/clan/join_clan_handler.h>
#include <game_queue_message_handlers/clan/leave_clan_handler.h>
#include <game_queue_message_handlers/clan/reject_application_handler.h>
#include <game_queue_message_handlers/clan/set_tax_handler.h>
#include <range/v3/all.hpp>
#include <asset_loading/load_character_select.h>

#include "config.h"
#include "logger_init.h"
#include "config_parsers.h"

#include "working_directory_manipulation.h"

#include "ecs/battle_system.h"

#include "websocket_thread.h"
#include "discord/discord_thread.h"
#include "discord/discord_rest.h"

using namespace std;
using namespace ibh;

atomic<bool> quit{false};

void on_sigint([[maybe_unused]] int sig) {
    quit.store(true, memory_order_release);
    spdlog::info("received sigint");
}

int main() {
    set_cwd(get_selfpath());
    ::signal(SIGINT, on_sigint);
    locale::global(locale("en_US.UTF-8"));

    sensor.add_dictionary("assets/profanity_locales/en.json");
    fill_mappers();

    config config;
    try {
        auto config_opt = parse_env_file();
        if(!config_opt) {
            return 1;
        }
        config = config_opt.value();
    } catch (const exception& e) {
        spdlog::error("[{}] config.json file is malformed json.", __FUNCTION__);
        return 1;
    }

    if(!filesystem::exists("logs") && !filesystem::create_directory("logs")) {
        spdlog::error("[{}] Fatal error creating logs directory", __FUNCTION__);
        return 1;
    }

    reconfigure_logger(config);

    if(sodium_init() != 0) {
        spdlog::error("[{}] sodium init failure", __FUNCTION__);
        return 1;
    }

    auto pool = make_shared<database_pool>();
    pool->create_connections(config.connection_string, 1);

    server_handle s_handle{};
    client_handle c_handle{};

    entt::registry es;

    load_assets(es, quit);
    load_from_database(es, pool, quit);
    auto char_sel = load_character_select("assets/charselect.json");

    if(!char_sel) {
        exit(1);
    }

    select_response = char_sel.value();

    auto mob_def_view = es.view<monster_definition_component>();
    auto special_def_view = es.view<monster_special_definition_component>();

    if(mob_def_view.empty() || special_def_view.empty()) {
        spdlog::error("[{}] monster init failure", __FUNCTION__);
        return 1;
    }

    outward_queues outward_queue;
    battle_system bs{config.battle_system_each_n_ticks, &outward_queue};

    if(quit.load(memory_order_acquire)) {
        spdlog::warn("[{}] quitting program", __FUNCTION__);
        return 0;
    }

    auto websocket_thread = run_websocket(config, pool, s_handle, quit);
    vector<thread> discord_threads;
    if(!config.discord_channel_id.empty() && !config.discord_token.empty()) {
        discord_threads.emplace_back(run_discord(config, c_handle, outward_queue, quit));
        auto discord_rest_threads = run_discord_rest(quit);
        discord_threads.insert(end(discord_threads), make_move_iterator(begin(discord_rest_threads)), make_move_iterator(end(discord_rest_threads)));
        assert(discord_threads.size() == 3);
    } else {
        spdlog::warn("[{}] not starting discord threads due to missing channel id or missing token", __FUNCTION__);
    }

    vector<uint64_t> frame_times;
    auto next_tick = chrono::system_clock::now() + chrono::milliseconds(config.tick_length);
    auto next_log_tick_times = chrono::system_clock::now() + chrono::seconds(1);
    uint32_t tick_counter = 0;

    ibh_flat_map<uint32_t, function<bool(queue_message*, entt::registry&, outward_queues&, unique_ptr<database_transaction> const &)>> game_queue_message_router;
    game_queue_message_router.emplace(player_enter_message::_type, handle_player_enter_message);
    game_queue_message_router.emplace(player_leave_message::_type, handle_player_leave_message);

    // clans
    game_queue_message_router.emplace(accept_application_message::_type, handle_accept_application);
    game_queue_message_router.emplace(create_clan_message::_type, handle_create_clan);
    game_queue_message_router.emplace(increase_bonus_message::_type, handle_increase_bonus);
    game_queue_message_router.emplace(join_clan_message::_type, handle_join_clan);
    game_queue_message_router.emplace(leave_clan_message::_type, handle_leave_clan);
    game_queue_message_router.emplace(reject_application_message::_type, handle_reject_application);
    game_queue_message_router.emplace(set_tax_message::_type, handle_set_tax);

    while (!quit.load(memory_order_acquire)) {
        auto now = chrono::system_clock::now();
        if(now < next_tick) {
            this_thread::sleep_until(next_tick);
        }
        auto tick_start = chrono::system_clock::now();

        {
            unique_ptr<queue_message> msg(nullptr);
            while (game_loop_queue.try_dequeue(msg)) {
                spdlog::trace("[{}] got game loop msg with type {}", __FUNCTION__, msg->type);
                auto transaction = pool->create_transaction();
                if(game_queue_message_router[msg->type](msg.get(), es, outward_queue, transaction)) {
                    transaction->commit();
                }
            }
        }

        bs.do_tick(es);

        auto tick_end = chrono::system_clock::now();
        frame_times.push_back(chrono::duration_cast<chrono::microseconds>(tick_end - tick_start).count());
        next_tick += chrono::milliseconds(config.tick_length);
        tick_counter++;

        {
            outward_message msg{{}, nullptr};
            while (outward_queue.try_dequeue(msg)) {
                shared_lock lock(user_connections_mutex);

                if(msg.conn_id == 0) {
                    auto serializedMsg = msg.msg->serialize();
                    for(auto &conn : user_connections) {
                        try {
                            s_handle.s->send(conn.second.ws, serializedMsg, websocketpp::frame::opcode::value::TEXT);
                        } catch (...) {
                            spdlog::warn("[{}] socket expired, wanted to send outward message to all", __FUNCTION__, msg.conn_id);
                            continue;
                        }
                    }
                    continue;
                }

                auto user_data = user_connections.find(msg.conn_id);
                if (user_data != end(user_connections) && !user_data->second.ws.expired()) {
                    try {
                        s_handle.s->send(user_data->second.ws, msg.msg->serialize(), websocketpp::frame::opcode::value::TEXT);
                    } catch (...) {
                        spdlog::warn("[{}] socket expired, wanted to send outward message", __FUNCTION__, msg.conn_id);
                        continue;
                    }
                } else {
                    spdlog::warn("[{}] couldn't find connection id {}, wanted to send outward message", __FUNCTION__, msg.conn_id);
                    game_loop_queue.enqueue(make_unique<player_leave_message>(msg.conn_id));
                }
            }
        }

        if(config.log_tick_times && tick_end > next_log_tick_times) {
            spdlog::info("[{}] ticks {} - frame times max/avg/min: {} / {} / {} µs", __FUNCTION__, tick_counter,
                         *max_element(begin(frame_times), end(frame_times)), accumulate(begin(frame_times), end(frame_times), 0UL) / frame_times.size(),
                         *min_element(begin(frame_times), end(frame_times)));
            frame_times.clear();
            next_log_tick_times += chrono::seconds(1);
            tick_counter = 0;
        }
    }

    spdlog::warn("[{}] quitting program", __FUNCTION__);
    s_handle.s->stop();
    if(!config.discord_channel_id.empty() && !config.discord_token.empty()) {
        c_handle.c->stop();
    }
    websocket_thread.join();
    spdlog::warn("[{}] websocket_thread stopped", __FUNCTION__);
    for(auto &t : discord_threads) {
        t.join();
        spdlog::warn("[{}] discord_thread stopped", __FUNCTION__);
    }
    spdlog::warn("[{}] all discord_threads stopped", __FUNCTION__);

    return 0;
}
