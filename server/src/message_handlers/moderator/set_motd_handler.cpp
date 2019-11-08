/*
    Realm of Aesir
    Copyright (C) 2019 Michael de Lang

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


#include "set_motd_handler.h"

#include <spdlog/spdlog.h>

#include <messages/moderator/set_motd_request.h>
#include <uws_thread.h>
#include <messages/moderator/update_motd_response.h>
#include "message_handlers/handler_macros.h"

using namespace std;

namespace lotr {
    template <class Server, class WebSocket>
    void set_motd_handler(Server *s, rapidjson::Document const &d, shared_ptr<database_pool> pool, per_socket_data<WebSocket> *user_data,
                          moodycamel::ConcurrentQueue<unique_ptr<queue_message>> &q, lotr_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        if(!user_data->is_game_master) {
            spdlog::warn("[{}] user {} tried to set motd but is not a game master!", __FUNCTION__, user_data->username);
            return;
        }

        MEASURE_TIME_OF_FUNCTION()
        DESERIALIZE_WITH_PLAYING_CHECK(set_motd_request)

        spdlog::info("[{}] motd set to \"{}\" by user {}", __FUNCTION__, msg->motd, user_data->username);
        motd = msg->motd;

        update_motd_response motd_msg(motd);
        auto motd_msg_str = motd_msg.serialize();
        {
            shared_lock lock(user_connections_mutex);
            for (auto &[conn_id, other_user_data] : user_connections) {
                try {
                    if (other_user_data.ws.expired()) {
                        continue;
                    }
                    s->send(other_user_data.ws, motd_msg_str, websocketpp::frame::opcode::value::TEXT);
                } catch (...) {
                    continue;
                }
            }
        }
    }

    template void set_motd_handler<server, websocketpp::connection_hdl>(server *s, rapidjson::Document const &d, shared_ptr<database_pool> pool,
                                                               per_socket_data<websocketpp::connection_hdl> *user_data,
                                                               moodycamel::ConcurrentQueue<unique_ptr<queue_message>> &q, lotr_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &user_connections);
}
