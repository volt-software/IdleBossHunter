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
#include <message_handlers/clan/get_clan_applications_handler.h>
#include <messages/clan/get_clan_applications_request.h>
#include <messages/clan/get_clan_applications_response.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_members_repository.h>
#include <repositories/clan_member_applications_repository.h>
#include <repositories/users_repository.h>
#include <repositories/characters_repository.h>
#include <ecs/components.h>
#include "../../custom_server.h"

using namespace std;
using namespace ibh;

TEST_CASE("get clan applications handler tests") {
    SECTION("Should return applicant") {
        string message = get_clan_applications_request().serialize();
        per_socket_data<custom_hdl> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> cq;
        queue_abstraction<unique_ptr<queue_message>> q(&cq);
        ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> user_connections;
        custom_server s;
        clans_repository<database_transaction> clans_repo{};
        clan_members_repository<database_transaction> clan_members_repo{};
        clan_member_applications_repository<database_transaction> clan_applications_repo{};
        users_repository<database_transaction> users_repo{};
        characters_repository<database_transaction> chars_repo{};
        user_data.ws = 1;
        user_data.username = "test_user";

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        auto transaction = db_pool->create_transaction();
        db_clan new_clan{0, "test"};
        clans_repo.insert(new_clan, transaction);
        REQUIRE(new_clan.id > 0);
        db_user new_user{};
        users_repo.insert_if_not_exists(new_user, transaction);
        REQUIRE(new_user.id > 0);
        db_character new_char{0, new_user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        new_char.name = "new_char_name";
        chars_repo.insert(new_char, transaction);
        REQUIRE(new_char.id > 0);
        db_character requesting_char{0, new_user.id, 1, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        requesting_char.name = "requesting_char_name";
        chars_repo.insert(requesting_char, transaction);
        REQUIRE(requesting_char.id > 0);
        db_clan_member requesting_member{new_clan.id, requesting_char.id, CLAN_SAGE};
        REQUIRE(clan_members_repo.insert(requesting_member, transaction) == true);
        db_clan_member new_member{new_clan.id, new_char.id, CLAN_MEMBER};
        REQUIRE(clan_applications_repo.insert(new_member, transaction) == true);

        user_data.playing_character_slot = 0;
        user_data.playing_character_id = requesting_char.id;

        handle_get_clan_applications(&s, d, transaction, &user_data, &q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = get_clan_applications_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(new_msg->error.empty());
        REQUIRE(!new_msg->members.empty());
        auto inserted_member = find_if(begin(new_msg->members), end(new_msg->members), [char_name = new_char.name](member const &m){ return m.name == char_name; });
        REQUIRE(inserted_member != end(new_msg->members));
        REQUIRE(inserted_member->name == new_char.name);
    }

    SECTION("Only sages and admins can retrieve applications") {
        string message = get_clan_applications_request().serialize();
        per_socket_data<custom_hdl> user_data;
        moodycamel::ConcurrentQueue<unique_ptr<queue_message>> cq;
        queue_abstraction<unique_ptr<queue_message>> q(&cq);
        ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> user_connections;
        custom_server s;
        clans_repository<database_transaction> clans_repo{};
        clan_members_repository<database_transaction> clan_members_repo{};
        clan_member_applications_repository<database_transaction> clan_applications_repo{};
        users_repository<database_transaction> users_repo{};
        characters_repository<database_transaction> chars_repo{};
        user_data.ws = 1;
        user_data.username = "test_user";

        rapidjson::Document d;
        d.Parse(&message[0], message.size());

        auto transaction = db_pool->create_transaction();
        db_clan new_clan{0, "test"};
        clans_repo.insert(new_clan, transaction);
        REQUIRE(new_clan.id > 0);
        db_user new_user{};
        users_repo.insert_if_not_exists(new_user, transaction);
        REQUIRE(new_user.id > 0);
        db_character new_char{0, new_user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        new_char.name = "new_char_name";
        chars_repo.insert(new_char, transaction);
        REQUIRE(new_char.id > 0);
        db_character requesting_char{0, new_user.id, 1, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        requesting_char.name = "requesting_char_name";
        chars_repo.insert(requesting_char, transaction);
        REQUIRE(requesting_char.id > 0);
        db_clan_member requesting_member{new_clan.id, requesting_char.id, CLAN_MEMBER};
        REQUIRE(clan_members_repo.insert(requesting_member, transaction) == true);
        db_clan_member new_member{new_clan.id, new_char.id, CLAN_MEMBER};
        REQUIRE(clan_applications_repo.insert(new_member, transaction) == true);

        user_data.playing_character_slot = 0;
        user_data.playing_character_id = requesting_char.id;

        handle_get_clan_applications(&s, d, transaction, &user_data, &q, user_connections);

        d.Parse(&s.sent_message[0], s.sent_message.size());
        auto new_msg = get_clan_applications_response::deserialize(d);
        REQUIRE(new_msg);
        REQUIRE(!new_msg->error.empty());
    }
}
