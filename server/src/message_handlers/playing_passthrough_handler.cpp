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


#include "playing_passthrough_handler.h"

#include <spdlog/spdlog.h>

#include <messages/company/accept_application_request.h>
#include <messages/company/create_company_request.h>
#include <messages/company/increase_bonus_request.h>
#include <messages/company/join_company_request.h>
#include <messages/company/leave_company_request.h>
#include <messages/company/reject_application_request.h>
#include <messages/company/set_tax_request.h>
#include <messages/resources/set_action_request.h>
#include "message_handlers/handler_macros.h"
#include <websocket_thread.h>
#include "macros.h"

#ifdef TEST_CODE
#include "../../test/custom_server.h"
#endif

using namespace std;
using namespace chrono;

namespace ibh {

    template <class WebSocketMsgT>
    auto convert_msg(uint64_t connection_id, const unique_ptr<WebSocketMsgT> &msg);

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<set_action_request> &msg) {
        return make_unique<set_action_message>(connection_id, msg->resource_id);
    }

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<accept_application_request> &msg) {
        return make_unique<accept_application_message>(connection_id, msg->applicant_id);
    }

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<create_company_request> &msg) {
        return make_unique<create_company_message>(connection_id, msg->name, msg->company_type);
    }

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<increase_bonus_request> &msg) {
        return make_unique<increase_bonus_message>(connection_id, msg->bonus_type);
    }

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<join_company_request> &msg) {
        return make_unique<join_company_message>(connection_id, msg->company_name);
    }

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<leave_company_request> &msg) {
        return make_unique<leave_company_message>(connection_id);
    }

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<reject_application_request> &msg) {
        return make_unique<reject_application_message>(connection_id, msg->applicant_id);
    }

    template <>
    auto convert_msg(uint64_t connection_id, const unique_ptr<set_tax_request> &msg) {
        return make_unique<set_tax_message>(connection_id, msg->rate);
    }

    // accept_application_request

    template <class Server, class WebSocket, class WebSocketMsgT>
    void playing_passthrough_handler(Server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction, per_socket_data<WebSocket> *user_data,
                              queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        MEASURE_TIME_OF_FUNCTION(trace);
        DESERIALIZE_WITH_PLAYING_CHECK(WebSocketMsgT);

        q->enqueue(convert_msg<WebSocketMsgT>(user_data->connection_id, msg));
    }

#define TEMPLATE_SPECIALIZE(server, hdl, type) template void playing_passthrough_handler<server, hdl, type>(server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction, \
    per_socket_data<hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<hdl>> &user_connections);

    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, set_action_request)
    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, accept_application_request)
    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, create_company_request)
    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, increase_bonus_request)
    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, join_company_request)
    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, leave_company_request)
    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, reject_application_request)
    TEMPLATE_SPECIALIZE(server, websocketpp::connection_hdl, set_tax_request)

#ifdef TEST_CODE
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, set_action_request)
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, accept_application_request)
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, create_company_request)
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, increase_bonus_request)
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, join_company_request)
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, leave_company_request)
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, reject_application_request)
    TEMPLATE_SPECIALIZE(custom_server, custom_hdl, set_tax_request)
#endif
}
