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


#include "play_character_handler.h"

#include <spdlog/spdlog.h>

#include <messages/user_access/play_character_request.h>
#include <repositories/characters_repository.h>
#include <repositories/character_stats_repository.h>
#include "message_handlers/handler_macros.h"
#include <ecs/components.h>
#include <websocket_thread.h>
#include <utf.h>
#include <messages/user_access/character_select_response.h>
#include <messages/user_access/play_character_response.h>
#include "macros.h"

#ifdef TEST_CODE
#include "../../../test/custom_server.h"
#endif

using namespace std;

namespace ibh {
    template <class Server, class WebSocket>
    void handle_play_character(Server *s, rapidjson::Document const &d,
                               unique_ptr<database_transaction> const &transaction, per_socket_data<WebSocket> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        MEASURE_TIME_OF_FUNCTION(trace);
        DESERIALIZE_WITH_NOT_PLAYING_CHECK(play_character_request);

        characters_repository<database_transaction> character_repo{};
        character_stats_repository<database_transaction> stats_repo{};

        auto character = character_repo.get_character_by_slot(msg->slot, user_data->user_id, transaction);

        if(!character) {
            SEND_ERROR("Couldn't find character in that slot", "", "", true);
            return;
        }

        {
            shared_lock lock(user_connections_mutex);
            for (auto &[conn_id, other_user_data] : user_connections) {
                if (other_user_data.connection_id != user_data->connection_id && other_user_data.user_id == user_data->user_id &&
                    other_user_data.playing_character_slot == static_cast<int32_t>(msg->slot)) {
                    SEND_ERROR("Already playing that slot on another connection", "", "", true);
                    return;
                }
            }
        }

        user_data->playing_character_slot = msg->slot;
        auto db_stats = stats_repo.get_by_character_id(character->id, transaction);

        play_character_response play_resp{msg->slot};
        s->send(user_data->ws, play_resp.serialize(), websocketpp::frame::opcode::value::TEXT);

        // TODO move this calculation somewhere global
        auto race_it = find_if(begin(select_response.races), end(select_response.races), [&race = as_const(character->race)](character_race const &r){ return r.name == race;});
        if(race_it == end(select_response.races)) {
            spdlog::error("[{}] character {} slot {} wrong race {}", __FUNCTION__, character->name, character->slot, character->race);
            SEND_ERROR("Chosen race does not exist", "", "", true);
            return;
        }

        auto classes_it = find_if(begin(select_response.classes), end(select_response.classes), [&baseclass = as_const(character->_class)](character_class const &c){ return c.name == baseclass; });
        if(classes_it == end(select_response.classes)) {
            spdlog::error("[{}] character {} slot {} wrong class {}", __FUNCTION__, character->name, character->slot, character->_class);
            SEND_ERROR("Chosen class does not exist", "", "", true);
            return;
        }

/*        vector<stat_component> player_stats_mods;
        player_stats_mods.reserve(stat_names.size());
        for(auto const &stat : stat_names) {
            auto value = 0;
            auto race_value_it = find_if(begin(race_it->stat_mods), end(race_it->stat_mods), [&stat](stat_component const &sc){ return sc.name == stat; });
            auto class_value_it = find_if(begin(classes_it->stat_mods), end(classes_it->stat_mods), [&stat](stat_component const &sc){ return sc.name == stat; });

            if(race_value_it != end(race_it->stat_mods)) {
                value += race_value_it->value;
            }

            if(class_value_it != end(classes_it->stat_mods)) {
                value += class_value_it->value;
            }

            player_stats_mods.emplace_back(stat, value);
        }*/

        vector<stat_component> player_stats;
        player_stats.reserve(db_stats.size());
        for(auto const &stat : db_stats) {
            player_stats.emplace_back(stat.stat_id, stat.value);
        }
        spdlog::debug("[{}] enqueing character {} slot {}", __FUNCTION__, character->name, character->slot);
        q->enqueue(make_unique<player_enter_message>(character->id, character->name, character->race, character->_class, move(player_stats),
                user_data->connection_id, character->level, character->gold, character->xp, character->skill_points));
    }

    template void handle_play_character<server, websocketpp::connection_hdl>(server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                                             per_socket_data<websocketpp::connection_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &user_connections);

#ifdef TEST_CODE
    template void handle_play_character<custom_server, custom_hdl>(custom_server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                           per_socket_data<custom_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> &user_connections);
#endif
}
