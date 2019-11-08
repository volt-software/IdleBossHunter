/*
    Realm of Aesir
    Copyright (C) 2019  Michael de Lang

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
#include <spdlog/spdlog.h>
#include "../test_helpers/startup_helper.h"
#include <message_handlers/user_access/register_handler.h>
#include <messages/user_access/register_request.h>
#include <messages/generic_error_response.h>
#include "../custom_server.h"

using namespace std;
using namespace lotr;

TEST_CASE("register handler tests") {
    SECTION("Prohibit too short usernames") {
        string message = register_request("a", "okay_password", "an_email").serialize();
        per_socket_data<uint64_t> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        lotr_flat_map<uint64_t, per_socket_data<uint64_t>> user_connections;
        custom_server s;
        user_data.ws = 1;

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        handle_register(&s, d, db_pool, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = generic_error_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error == "Usernames needs to be at least 2 characters and at most 20 characters");
    }

    SECTION("Prohibit too short usernames utf8") {
        string message = register_request("漢", "okay_password", "an_email").serialize();
        per_socket_data<uint64_t> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        lotr_flat_map<uint64_t, per_socket_data<uint64_t>> user_connections;
        custom_server s;
        user_data.ws = 1;

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        handle_register(&s, d, db_pool, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = generic_error_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error == "Usernames needs to be at least 2 characters and at most 20 characters");
    }

    SECTION("Prohibit too long usernames") {
        string message = register_request("aalishdiquwhgebilugfhkjsdhasdasd", "okay_password", "an_email").serialize();
        per_socket_data<uint64_t> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        lotr_flat_map<uint64_t, per_socket_data<uint64_t>> user_connections;
        custom_server s;
        user_data.ws = 1;

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        handle_register(&s, d, db_pool, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = generic_error_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error == "Usernames needs to be at least 2 characters and at most 20 characters");
    }

    SECTION("Prohibit too short password") {
        string message = register_request("ab", "shortpw", "an_email").serialize();
        per_socket_data<uint64_t> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        lotr_flat_map<uint64_t, per_socket_data<uint64_t>> user_connections;
        custom_server s;
        user_data.ws = 1;

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        handle_register(&s, d, db_pool, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = generic_error_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error == "Password needs to be at least 8 characters");
    }

    SECTION("Prohibit too short password utf8") {
        string message = register_request("ab", "漢字漢字漢字", "an_email").serialize();
        per_socket_data<uint64_t> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        lotr_flat_map<uint64_t, per_socket_data<uint64_t>> user_connections;
        custom_server s;
        user_data.ws = 1;

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        handle_register(&s, d, db_pool, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = generic_error_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error == "Password needs to be at least 8 characters");
    }

    SECTION("Prohibit password equal to username") {
        string message = register_request("okay_p$ssword", "okay_p$ssword", "an_email").serialize();
        per_socket_data<uint64_t> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        lotr_flat_map<uint64_t, per_socket_data<uint64_t>> user_connections;
        custom_server s;
        user_data.ws = 1;

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        handle_register(&s, d, db_pool, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = generic_error_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error == "Password cannot equal username");
    }

    SECTION("Prohibit password equal to email") {
        string message = register_request("ab", "an_email", "an_email").serialize();
        per_socket_data<uint64_t> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        lotr_flat_map<uint64_t, per_socket_data<uint64_t>> user_connections;
        custom_server s;
        user_data.ws = 1;

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        handle_register(&s, d, db_pool, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = generic_error_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error == "Password cannot equal email");
    }
}
