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
#include "repositories/company_member_applications_repository.h"
#include "repositories/companies_repository.h"
#include "repositories/characters_repository.h"
#include "repositories/users_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("company member applications repository tests") {
    company_member_applications_repository<database_transaction> member_repo{};
    companies_repository<database_transaction> company_repo{};
    characters_repository<database_transaction> char_repo{};
    users_repository<database_transaction> user_repo{};

    SECTION("company member applications inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);
        db_company company{0, "company", 0, 2};
        company_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_member member{company.id, player.id, 1, 0};
        REQUIRE(member_repo.insert(member, transaction) == true);

        auto member2 = member_repo.get(company.id, player.id, transaction);
        REQUIRE(member2->company_id == member.company_id);
        REQUIRE(member2->character_id == member.character_id);
        REQUIRE(member2->member_level == 0);
    }

    SECTION("company member applications no double insertion" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);
        db_company company{0, "company", 0, 2};
        company_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_member member{company.id, player.id, 1, 0};
        REQUIRE(member_repo.insert(member, transaction) == true);
        REQUIRE(member_repo.insert(member, transaction) == false);
    }

    SECTION("company member applications deleted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);
        db_company company{0, "company", 0, 2};
        company_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_member member{company.id, player.id, 1, 0};
        member_repo.insert(member, transaction);

        auto member2 = member_repo.get(company.id, player.id, transaction);
        REQUIRE(member2->company_id == member.company_id);
        REQUIRE(member2->character_id == member.character_id);
        REQUIRE(member2->member_level == 0);

        member_repo.remove(member, transaction);
        member2 = member_repo.get(company.id, player.id, transaction);
        REQUIRE(!member2);
    }

    SECTION( "get all character member applications by company" ) {
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
        db_company company{0, "company", 0, 2};
        company_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_member member{company.id, player.id, 1, 0};
        db_company_member member2{company.id, player2.id, 2, 0};
        member_repo.insert(member, transaction);
        member_repo.insert(member2, transaction);

        auto members = member_repo.get_by_company_id(company.id, transaction);
        REQUIRE(members.size() == 2);
        REQUIRE(members[0].company_id == member.company_id);
        REQUIRE(members[0].character_id == member.character_id);
        REQUIRE(members[0].member_level == 0);
        REQUIRE(members[1].company_id == member2.company_id);
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
        db_company company{0, "company", 0, 2};
        company_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_member member{company.id, player.id, 1, 0};
        db_company_member member2{company.id, player2.id, 2, 0};
        member_repo.insert(member, transaction);
        member_repo.insert(member2, transaction);

        auto members = member_repo.get_by_character_id(player.id, transaction);
        REQUIRE(members.size() == 1);
        REQUIRE(members[0].company_id == member.company_id);
        REQUIRE(members[0].character_id == member.character_id);
        REQUIRE(members[0].member_level == 0);
    }
}

#endif
