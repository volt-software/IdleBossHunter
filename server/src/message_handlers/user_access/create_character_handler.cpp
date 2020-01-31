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


#include "create_character_handler.h"

#include <spdlog/spdlog.h>

#include <messages/user_access/create_character_request.h>
#include <messages/user_access/create_character_response.h>
#include <repositories/characters_repository.h>
#include <repositories/character_stats_repository.h>
#include <game_logic/censor_sensor.h>
#include "message_handlers/handler_macros.h"
#include <ecs/components.h>
#include <utf.h>
#include <websocket_thread.h>
#include <messages/user_access/character_select_response.h>
#include "macros.h"

using namespace std;

namespace ibh {
    template <class Server, class WebSocket>
    void handle_create_character(Server *s, rapidjson::Document const &d,
                                 shared_ptr<database_pool> pool, per_socket_data<WebSocket> *user_data, moodycamel::ConcurrentQueue<unique_ptr<queue_message>> &q, ibh_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        MEASURE_TIME_OF_FUNCTION(trace);
        DESERIALIZE_WITH_NOT_PLAYING_CHECK(create_character_request);

        characters_repository<database_pool, database_transaction> player_repo(pool);
        character_stats_repository<database_pool, database_transaction> stats_repo(pool);

        if(msg->slot > 4) {
            SEND_ERROR("Slot has to be at least 0 and at most 3.", "", "", true);
            return;
        }

        auto transaction = player_repo.create_transaction();
        auto existing_character = player_repo.get_character_by_slot(msg->slot, user_data->user_id, transaction);

        if(existing_character) {
            SEND_ERROR("Character already exists in slot", "", "", true);
            return;
        }

        if(sensor.is_profane_ish(msg->name)) {
            SEND_ERROR("invalid_char_name", "Invalid Character Name", "That character name is not valid", true);
            return;
        }

        auto utf_name = To_UTF32(msg->name);
        if(utf_name.size() < 2 || utf_name.size() > 20) {
            SEND_ERROR("Character names needs to be at least 2 characters and at most 20 characters", "", "", true);
            return;
        }

        if(any_of(begin(msg->name), end(msg->name), ::isdigit)) {
            SEND_ERROR("Character names cannot contain digits", "", "", true);
            return;
        }

        if(any_of(begin(msg->name), end(msg->name), ::isspace)) {
            SEND_ERROR("Character names cannot contain spaces", "", "", true);
            return;
        }

        if(To_UTF32(utf_to_upper_copy(msg->name))[0] != To_UTF32(msg->name)[0]) {
            SEND_ERROR("Character names must start with a capital", "", "", true);
            return;
        }

        auto class_it = find_if(begin(select_response.classes), end(select_response.classes), [&baseclass = as_const(msg->baseclass)](character_class const &c){ return c.name == baseclass;});
        if(class_it == end(select_response.classes)) {
            SEND_ERROR("Selected class does not exist", "", "", true);
            spdlog::error("[{}] Attempted to create character with class {}", __FUNCTION__, msg->baseclass);
            return;
        }

        db_character new_player;
        new_player.name = msg->name;
        new_player.user_id = user_data->user_id;
        new_player.race = msg->race;
        new_player._class = msg->baseclass;
        new_player.level = 1;
        new_player.slot = msg->slot;

        auto race_it = find_if(begin(select_response.races), end(select_response.races), [&race = as_const(msg->race)](character_race const &r){ return r.name == race;});
        if(race_it == end(select_response.races)) {
            spdlog::error("[{}] character {} slot {} wrong race {}", __FUNCTION__, msg->name, msg->slot, msg->race);
            SEND_ERROR("Chosen race does not exist", "", "", true);
            return;
        }

        auto classes_it = find_if(begin(select_response.classes), end(select_response.classes), [&baseclass = as_const(msg->baseclass)](character_class const &c){ return c.name == baseclass; });
        if(classes_it == end(select_response.classes)) {
            spdlog::error("[{}] character {} slot {} wrong class {}", __FUNCTION__, msg->name, msg->slot, msg->baseclass);
            SEND_ERROR("Chosen class does not exist", "", "", true);
            return;
        }

        if(!player_repo.insert(new_player, transaction)) {
            SEND_ERROR("Player with name already exists", "", "", true);
            spdlog::error("[{}] Player with slot {} already exists, but this code path should never be hit.", __FUNCTION__, new_player.slot);
            return;
        }

        vector<stat_component> player_stats;
        for(auto const &stat : stat_names) {
            auto value = 0;
            auto race_value_it = find_if(begin(race_it->level_stat_mods), end(race_it->level_stat_mods), [&stat](stat_component const &sc){ return sc.name == stat; });
            auto class_value_it = find_if(begin(classes_it->stat_mods), end(classes_it->stat_mods), [&stat](stat_component const &sc){ return sc.name == stat; });

            if(race_value_it != end(race_it->level_stat_mods)) {
                value += race_value_it->value;
            } else {
                spdlog::error("[{}] couldn't find stat {} for race {}", __FUNCTION__, stat, race_it->name);
            }

            if(class_value_it != end(classes_it->stat_mods)) {
                value += class_value_it->value;
            } else {
                spdlog::error("[{}] couldn't find stat {} for class {}", __FUNCTION__, stat, classes_it->name);
            }

            player_stats.emplace_back(stat, value);
        }

        for(auto const &stat : player_stats) {
            db_character_stat char_stat{0, new_player.id, stat.name, stat.value};
            stats_repo.insert(char_stat, transaction);
            player_stats.emplace_back(stat.name, stat.value);
        }

        transaction->commit();


        create_character_response response{character_object{new_player.name, new_player.race, new_player._class,
                                                            new_player.level, new_player.slot, new_player.gold, new_player.xp, new_player.skill_points, move(player_stats), {}, {}}};
        auto response_msg = response.serialize();
        s->send(user_data->ws, response_msg, websocketpp::frame::opcode::value::TEXT);
    }

    template void handle_create_character<server, websocketpp::connection_hdl>(server *s, rapidjson::Document const &d, shared_ptr<database_pool> pool,
                                                                               per_socket_data<websocketpp::connection_hdl> *user_data, moodycamel::ConcurrentQueue<unique_ptr<queue_message>> &q, ibh_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &user_connections);
}
