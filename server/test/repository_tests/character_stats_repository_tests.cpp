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
#include "repositories/character_stats_repository.h"
#include "repositories/users_repository.h"
#include "repositories/characters_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("character stats repository tests") {
    character_stats_repository<database_transaction> stat_repo{};
    users_repository<database_transaction> users_repo{};
    characters_repository<database_transaction> characters_repo{};

    SECTION("character stats inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_user usr{0, "test", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id > 0);
        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        characters_repo.insert(character, transaction);
        REQUIRE(character.id > 0);
        db_character_stat stat{0, character.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->character_id == stat.character_id);
        REQUIRE(stat2->name == stat.name);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "update stats" ) {
        auto transaction = db_pool->create_transaction();
        db_user usr{0, "test", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id > 0);
        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        characters_repo.insert(character, transaction);
        REQUIRE(character.id > 0);
        db_character_stat stat{0, character.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        stat.value = 12;
        stat_repo.update(stat, transaction);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->character_id == stat.character_id);
        REQUIRE(stat2->name == stat.name);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "get all for character stats" ) {
        auto transaction = db_pool->create_transaction();
        db_user usr{0, "test", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id > 0);
        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        characters_repo.insert(character, transaction);
        REQUIRE(character.id > 0);
        db_character_stat stat{0, character.id, "test", 20};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);
        db_character_stat stat2{0, character.id, "test2", 30};
        stat_repo.insert(stat2, transaction);
        REQUIRE(stat2.id > 0);

        auto stats = stat_repo.get_by_character_id(character.id, transaction);
        REQUIRE(stats.size() == 2);
        REQUIRE(stats[0].id == stat.id);
        REQUIRE(stats[0].character_id == stat.character_id);
        REQUIRE(stats[0].name == stat.name);
        REQUIRE(stats[0].value == stat.value);
        REQUIRE(stats[1].id == stat2.id);
        REQUIRE(stats[1].character_id == stat2.character_id);
        REQUIRE(stats[1].name == stat2.name);
        REQUIRE(stats[1].value == stat2.value);
    }
}

#endif
