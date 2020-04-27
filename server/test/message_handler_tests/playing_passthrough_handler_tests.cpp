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

#include <catch2/catch.hpp>
#include "../test_helpers/startup_helper.h"
#include <messages/company/accept_application_request.h>
#include <messages/company/create_company_request.h>
#include <messages/company/increase_bonus_request.h>
#include <messages/company/join_company_request.h>
#include <messages/company/leave_company_request.h>
#include <messages/company/reject_application_request.h>
#include <messages/company/set_tax_request.h>
#include <messages/resources/set_action_request.h>
#include <message_handlers/playing_passthrough_handler.h>
#include <common_components.h>
#include "../custom_server.h"

using namespace std;
using namespace ibh;

template<class MsgT, class convertT, class ...Args>
unique_ptr<queue_message> do_passthrough_test_for(Args... args) {
    string message = MsgT(args...).serialize();
    per_socket_data<custom_hdl> user_data;
    moodycamel::ConcurrentQueue<unique_ptr<queue_message>> cq;
    queue_abstraction<unique_ptr<queue_message>> q(&cq);
    ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> user_connections;
    custom_server s;
    user_data.ws = 1;
    user_data.connection_id = 1;
    user_data.username = "test_user";

    rapidjson::Document d;
    d.Parse(&message[0], message.size());
    auto transaction = db_pool->create_transaction();

    playing_passthrough_handler<custom_server, custom_hdl, MsgT>(&s, d, transaction, &user_data, &q, user_connections);

    unique_ptr<queue_message> msg(nullptr);
    REQUIRE(q.try_dequeue(msg));
    REQUIRE(msg->connection_id == 1);
    REQUIRE(msg->type == convertT::_type);
    return msg;
}

TEST_CASE("playing passthrough handler tests") {
    SECTION("Should passthrough select_action_message") {
        auto msg = do_passthrough_test_for<set_action_request, set_action_message>(1);
        auto *conv_msg = dynamic_cast<set_action_message*>(msg.get());
        REQUIRE(conv_msg);
        REQUIRE(conv_msg->action_id == 1);
    }

    SECTION("Should passthrough accept_application_request") {
        auto msg = do_passthrough_test_for<accept_application_request, accept_application_message>(1);
        auto *conv_msg = dynamic_cast<accept_application_message*>(msg.get());
        REQUIRE(conv_msg);
        REQUIRE(conv_msg->applicant_id == 1);
    }

    SECTION("Should passthrough create_company_request") {
        auto msg = do_passthrough_test_for<create_company_request, create_company_message>("name", 1);
        auto *conv_msg = dynamic_cast<create_company_message*>(msg.get());
        REQUIRE(conv_msg);
        REQUIRE(conv_msg->company_name == "name");
        REQUIRE(conv_msg->company_type == 1);
    }

    SECTION("Should passthrough increase_bonus_request") {
        auto msg = do_passthrough_test_for<increase_bonus_request, increase_bonus_message>(1);
        auto *conv_msg = dynamic_cast<increase_bonus_message*>(msg.get());
        REQUIRE(conv_msg);
        REQUIRE(conv_msg->bonus_type == 1);
    }

    SECTION("Should passthrough join_company_request") {
        auto msg = do_passthrough_test_for<join_company_request, join_company_message>("name");
        auto *conv_msg = dynamic_cast<join_company_message*>(msg.get());
        REQUIRE(conv_msg);
        REQUIRE(conv_msg->company_name == "name");
    }

    SECTION("Should passthrough leave_company_request") {
        auto msg = do_passthrough_test_for<leave_company_request, leave_company_message>();
        auto *conv_msg = dynamic_cast<leave_company_message*>(msg.get());
        REQUIRE(conv_msg);
    }

    SECTION("Should passthrough reject_application_request") {
        auto msg = do_passthrough_test_for<reject_application_request, reject_application_message>(1);
        auto *conv_msg = dynamic_cast<reject_application_message*>(msg.get());
        REQUIRE(conv_msg);
        REQUIRE(conv_msg->applicant_id == 1);
    }

    SECTION("Should passthrough set_tax_request") {
        auto msg = do_passthrough_test_for<set_tax_request, set_tax_message>(1);
        auto *conv_msg = dynamic_cast<set_tax_message*>(msg.get());
        REQUIRE(conv_msg);
        REQUIRE(conv_msg->tax_percentage == 1);
    }
}
