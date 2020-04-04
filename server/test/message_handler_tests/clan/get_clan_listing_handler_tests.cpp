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
#include "../../test_helpers/startup_helper.h"
#include <message_handlers/company/get_company_listing_handler.h>
#include <messages/company/get_company_listing_request.h>
#include <messages/company/get_company_listing_response.h>
#include <repositories/companies_repository.h>
#include "../../custom_server.h"

using namespace std;
using namespace ibh;

TEST_CASE("get company listing handler tests") {
    SECTION("Should return company") {
        string message = get_company_listing_request().serialize();
        per_socket_data<custom_hdl> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> cq;
        queue_abstraction<unique_ptr<queue_message>> q(&cq);
        ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> user_connections;
        custom_server s;
        companies_repository<database_transaction> companies_repo{};
        user_data.ws = 1;
        user_data.username = "test_user";

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        auto transaction = db_pool->create_transaction();
        db_company new_company{0, "test"};
        companies_repo.insert(new_company, transaction);
        REQUIRE(new_company.id > 0);

        handle_get_company_listing(&s, d, transaction, &user_data, &q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = get_company_listing_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error.empty());
        REQUIRE(!new_msg->companies.empty());
        auto inserted_company = find_if(begin(new_msg->companies), end(new_msg->companies), [company_name = new_company.name](company const &c){ return c.name == company_name; });
        REQUIRE(inserted_company != end(new_msg->companies));
        REQUIRE(inserted_company->name == new_company.name);
    }
}
