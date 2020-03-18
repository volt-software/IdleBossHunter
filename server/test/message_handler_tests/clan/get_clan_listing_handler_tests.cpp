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
#include <message_handlers/clan/get_clan_listing_handler.h>
#include <messages/clan/get_clan_listing_request.h>
#include <messages/clan/get_clan_listing_response.h>
#include <repositories/clans_repository.h>
#include "../../custom_server.h"

using namespace std;
using namespace ibh;

TEST_CASE("get clan listing handler tests") {
    SECTION("Should return clan") {
        string message = get_clan_listing_request().serialize();
        per_socket_data<custom_hdl> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> q;
        ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> user_connections;
        custom_server s;
        clans_repository<database_transaction> clans_repo{};
        user_data.ws = 1;
        user_data.username = "test_user";

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        auto transaction = db_pool->create_transaction();
        db_clan new_clan{0, "test"};
        clans_repo.insert(new_clan, transaction);
        REQUIRE(new_clan.id > 0);

        handle_get_clan_listing(&s, d, transaction, &user_data, q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = get_clan_listing_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error.empty());
        REQUIRE(!new_msg->clans.empty());
        auto inserted_clan = find_if(begin(new_msg->clans), end(new_msg->clans), [clan_name = new_clan.name](clan const &c){ return c.name == clan_name; });
        REQUIRE(inserted_clan != end(new_msg->clans));
        REQUIRE(inserted_clan->name == new_clan.name);
    }
}
