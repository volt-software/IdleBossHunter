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
#include <range/v3/all.hpp>
#include <asset_loading/load_character_select.h>

#include "config.h"
#include "logger_init.h"
#include "config_parsers.h"

#include "repositories/users_repository.h"
#include "repositories/banned_users_repository.h"
#include "repositories/characters_repository.h"
#include "working_directory_manipulation.h"

#include "ecs/battle_system.h"

#include "uws_thread.h"

using namespace std;
using namespace ibh;

atomic<bool> quit{false};

void on_sigint([[maybe_unused]] int sig) {
    quit = true;
    spdlog::info("received sigint");
}

int main() {
    set_cwd(get_selfpath());
    ::signal(SIGINT, on_sigint);
    locale::global(locale("en_US.UTF-8"));

    sensor.add_dictionary("assets/profanity_locales/en.json");

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

    users_repository<database_pool, database_transaction> user_repo(pool);
    banned_users_repository<database_pool, database_transaction> banned_user_repo(pool);
    characters_repository<database_pool, database_transaction> player_repo(pool);
    server_handle s_handle{};

    entt::registry es;

    load_assets(es, quit);
    load_from_database(es, pool, quit);
    auto char_sel = load_character_select("assets/charselect.json");

    if(!char_sel) {
        exit(1);
    }

    select_response = char_sel.value();

    vector<monster_definition_component> mob_defs;
    vector<monster_special_definition_component> special_defs;
    auto mob_def_view = es.view<monster_definition_component>();
    auto special_def_view = es.view<monster_special_definition_component>();
    for(auto entity : mob_def_view) {
        auto const &c = mob_def_view.get<monster_definition_component>(entity);

        for(auto &stat_name :stat_names) {
            auto stat_it = c.stats.find(stat_name);

            if(stat_it == end(c.stats)) {
                continue;
            }

            if(stat_it->second.value >= 300) {
                spdlog::error("[{}] mob {} has stat {} more than 300", __FUNCTION__, c.name, stat_name);
            }
        }

        mob_defs.push_back(c);
    }
    for(auto entity : special_def_view) {
        auto const &c = special_def_view.get<monster_special_definition_component>(entity);

        for(auto &stat_name :stat_names) {
            auto stat_it = c.stats.find(stat_name);

            if(stat_it == end(c.stats)) {
                continue;
            }

            if(stat_it->second.value >= 500) {
                spdlog::error("[{}] mob special {} has stat {} more than 300", __FUNCTION__, c.name, stat_name);
            }
        }

        special_defs.push_back(c);
    }

    if(mob_defs.empty() || special_defs.empty()) {
        spdlog::error("[{}] monster init failure", __FUNCTION__);
        return 1;
    }

    outward_queues outward_queue;
    battle_system bs{config.battle_system_each_n_ticks, &outward_queue, move(mob_defs), move(special_defs)};

    if(quit) {
        spdlog::warn("[{}] quitting program", __FUNCTION__);
        return 0;
    }

    auto uws_thread = run_uws(config, pool, s_handle, quit);


    vector<uint64_t> frame_times;
    auto next_tick = chrono::system_clock::now() + chrono::milliseconds(config.tick_length);
    auto next_log_tick_times = chrono::system_clock::now() + chrono::seconds(1);
    uint32_t tick_counter = 0;

    ibh_flat_map<uint32_t, function<void(queue_message*, entt::registry&, outward_queues&)>> game_queue_message_router;
    game_queue_message_router.emplace(player_enter_message::_type, handle_player_enter_message);
    game_queue_message_router.emplace(player_leave_message::_type, handle_player_leave_message);

    while (!quit) {
        auto now = chrono::system_clock::now();
        if(now < next_tick) {
            this_thread::sleep_until(next_tick);
        }
        auto tick_start = chrono::system_clock::now();

        {
            unique_ptr<queue_message> msg(nullptr);
            while (game_loop_queue.try_dequeue(msg)) {
                spdlog::trace("[{}] got game loop msg with type {}", __FUNCTION__, msg->type);
                game_queue_message_router[msg->type](msg.get(), es, outward_queue);
            }
        }

        bs.do_tick(es);

/*        for(auto m_entity : map_viewc) {
            map_component &m = map_view.get(m_entity);
            lotr_player_location_map player_location_map;

            for (auto &player : m.players) {
                auto existing_players = player_location_map.find(player.loc);

                if (existing_players == end(player_location_map)) {
                    player_location_map[player.loc] = vector<pc_component *>{&player};
                } else {
                    existing_players->second.push_back(&player);
                }

                player.fov = compute_fov_restrictive_shadowcasting(m, player.loc, true);

                auto min_x = max(0U, get<0>(player.loc) - fov_max_distance);
                auto min_y = max(0U, get<1>(player.loc) - fov_max_distance);
                auto max_x = min(m.width, get<0>(player.loc) + fov_max_distance);
                auto max_y = min(m.height, get<1>(player.loc) + fov_max_distance);

                auto visible_npcs = m.npcs | ranges::views::filter([&](npc_component const &npc){ return is_visible(player.loc, npc.loc, player.fov, min_x, max_x, min_y, max_y); });
                auto visible_pcs = m.players | ranges::views::filter([&](pc_component const &pc){ return is_visible(player.loc, pc.loc, player.fov, min_x, max_x, min_y, max_y) && player.connection_id != pc.connection_id; });
                vector<character_component> cs;

                for(auto &npc : visible_npcs) {
                    cs.push_back(npc);
                }

                for(auto &pc : visible_pcs) {
                    cs.push_back(pc);
                }

                outward_queue.enqueue(outward_message{player.connection_id, make_unique<update_response>(cs)});
            }

            remove_dead_npcs(m.npcs);
            fill_spawners(m, m.npcs, es);

            for(auto &npc : m.npcs) {
                run_ai_on(npc, m, player_location_map);
            }
        }*/

        auto tick_end = chrono::system_clock::now();
        frame_times.push_back(chrono::duration_cast<chrono::microseconds>(tick_end - tick_start).count());
        next_tick += chrono::milliseconds(config.tick_length);
        tick_counter++;

        {
            outward_message msg{{}, nullptr};
            while (outward_queue.try_dequeue(msg)) {
                shared_lock lock(user_connections_mutex);
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
    uws_thread.join();
    spdlog::warn("[{}] uws thread stopped", __FUNCTION__);

    return 0;
}
