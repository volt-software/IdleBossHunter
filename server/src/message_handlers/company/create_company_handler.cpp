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


#include "create_company_handler.h"

#include <spdlog/spdlog.h>

#include <messages/company/create_company_request.h>
#include "message_handlers/handler_macros.h"
#include "game_queue_messages/messages.h"
#include <websocket_thread.h>
#include "macros.h"

#ifdef TEST_CODE
#include "../../../test/custom_server.h"
#endif

using namespace std;
using namespace chrono;

namespace ibh {
    template <class Server, class WebSocket>
    void handle_create_company(Server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction, per_socket_data<WebSocket> *user_data,
                            queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        MEASURE_TIME_OF_FUNCTION(trace);
        DESERIALIZE_WITH_PLAYING_CHECK(create_company_request);

        q->enqueue(make_unique<create_company_message>(user_data->connection_id, msg->name, msg->company_type));
    }

    template void handle_create_company<server, websocketpp::connection_hdl>(server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                                          per_socket_data<websocketpp::connection_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &user_connections);

#ifdef TEST_CODE
    template void handle_create_company<custom_server, custom_hdl>(custom_server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                           per_socket_data<custom_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> &user_connections);
#endif
}
