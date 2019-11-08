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


#include "public_chat_handler.h"

#include <spdlog/spdlog.h>

#include <messages/chat/message_request.h>
#include <messages/chat/message_response.h>
#include "message_handlers/handler_macros.h"
#include <game_logic/censor_sensor.h>
#include <uws_thread.h>

using namespace std;
using namespace chrono;

namespace lotr {
    template <class Server, class WebSocket>
    void handle_public_chat(Server *s, rapidjson::Document const &d, shared_ptr<database_pool> pool, per_socket_data<WebSocket> *user_data,
            moodycamel::ConcurrentQueue<unique_ptr<queue_message>> &q, lotr_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        MEASURE_TIME_OF_FUNCTION()
        DESERIALIZE_WITH_LOGIN_CHECK(message_request)

        auto now = system_clock::now();
        auto chat_msg = message_response(user_data->username, sensor.clean_profanity_ish(msg->content), "game", duration_cast<milliseconds>(now.time_since_epoch()).count()).serialize();

        {
            shared_lock lock(user_connections_mutex);
            for (auto &[conn_id, other_user_data] : user_connections) {
                try {
                    if (other_user_data.ws.expired()) {
                        continue;
                    }
                    s->send(other_user_data.ws, chat_msg, websocketpp::frame::opcode::value::TEXT);
                } catch (...) {
                    continue;
                }
            }
        }
    }

    template void handle_public_chat<server, websocketpp::connection_hdl>(server *s, rapidjson::Document const &d, shared_ptr<database_pool> pool,
            per_socket_data<websocketpp::connection_hdl> *user_data, moodycamel::ConcurrentQueue<unique_ptr<queue_message>> &q, lotr_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &user_connections);
}
