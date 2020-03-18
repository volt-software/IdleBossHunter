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

#ifndef EXCLUDE_PSQL_TESTS

#include <catch2/catch.hpp>
#include "../test_helpers/startup_helper.h"
#include "repositories/clan_member_applications_repository.h"
#include "repositories/clans_repository.h"
#include "repositories/characters_repository.h"
#include "repositories/users_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("clan member applications repository tests") {
    clan_member_applications_repository<database_transaction> member_repo{};
    clans_repository<database_transaction> clan_repo{};
    characters_repository<database_transaction> char_repo{};
    users_repository<database_transaction> user_repo{};

    SECTION("clan member applications inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);
        db_clan clan{0, "clan"};
        clan_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_member member{clan.id, player.id, 1};
        REQUIRE(member_repo.insert(member, transaction) == true);

        auto member2 = member_repo.get(clan.id, player.id, transaction);
        REQUIRE(member2->clan_id == member.clan_id);
        REQUIRE(member2->character_id == member.character_id);
        REQUIRE(member2->member_level == 0);
    }

    SECTION("clan member applications no double insertion" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);
        db_clan clan{0, "clan"};
        clan_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_member member{clan.id, player.id, 1};
        REQUIRE(member_repo.insert(member, transaction) == true);
        REQUIRE(member_repo.insert(member, transaction) == false);
    }

    SECTION("clan member applications deleted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);
        db_clan clan{0, "clan"};
        clan_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_member member{clan.id, player.id, 1};
        member_repo.insert(member, transaction);

        auto member2 = member_repo.get(clan.id, player.id, transaction);
        REQUIRE(member2->clan_id == member.clan_id);
        REQUIRE(member2->character_id == member.character_id);
        REQUIRE(member2->member_level == 0);

        member_repo.remove(member, transaction);
        member2 = member_repo.get(clan.id, player.id, transaction);
        REQUIRE(!member2);
    }

    SECTION( "get all character member applications by clan" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        db_character player2{0, user.id, 1, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        char_repo.insert(player2, transaction);
        REQUIRE(player.id > 0);
        REQUIRE(player2.id > 0);
        db_clan clan{0, "clan"};
        clan_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_member member{clan.id, player.id, 1};
        db_clan_member member2{clan.id, player2.id, 2};
        member_repo.insert(member, transaction);
        member_repo.insert(member2, transaction);

        auto members = member_repo.get_by_clan_id(clan.id, transaction);
        REQUIRE(members.size() == 2);
        REQUIRE(members[0].clan_id == member.clan_id);
        REQUIRE(members[0].character_id == member.character_id);
        REQUIRE(members[0].member_level == 0);
        REQUIRE(members[1].clan_id == member2.clan_id);
        REQUIRE(members[1].character_id == member2.character_id);
        REQUIRE(members[1].member_level == 0);
    }

    SECTION( "get all character member applications by char id" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        db_character player2{0, user.id, 1, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        char_repo.insert(player2, transaction);
        REQUIRE(player.id > 0);
        REQUIRE(player2.id > 0);
        db_clan clan{0, "clan"};
        clan_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_member member{clan.id, player.id, 1};
        db_clan_member member2{clan.id, player2.id, 2};
        member_repo.insert(member, transaction);
        member_repo.insert(member2, transaction);

        auto members = member_repo.get_by_character_id(player.id, transaction);
        REQUIRE(members.size() == 1);
        REQUIRE(members[0].clan_id == member.clan_id);
        REQUIRE(members[0].character_id == member.character_id);
        REQUIRE(members[0].member_level == 0);
    }
}

#endif
