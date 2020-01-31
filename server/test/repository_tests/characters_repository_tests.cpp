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
#include <spdlog/spdlog.h>
#include "../test_helpers/startup_helper.h"
#include "repositories/users_repository.h"
#include "repositories/characters_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("characters repository tests") {
    users_repository<database_pool, database_transaction> users_repo(db_pool);
    characters_repository<database_pool, database_transaction> characters_repo(db_pool);
    auto transaction = characters_repo.create_transaction();


    SECTION( "db_character inserted correctly" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        auto inserted = characters_repo.insert_or_update_character(character, transaction);
        REQUIRE(inserted == true);

        auto character2 = characters_repo.get_character(character.id, transaction);
        REQUIRE(character2);
        REQUIRE(character2->user_id == character.user_id);
        REQUIRE(character2->slot == character.slot);
        REQUIRE(character2->level == character.level);
        REQUIRE(character2->gold == character.gold);
        REQUIRE(character2->xp == character.xp);
        REQUIRE(character2->skill_points == character.skill_points);
        REQUIRE(character2->x == character.x);
        REQUIRE(character2->y == character.y);
        REQUIRE(character2->name == character.name);
        REQUIRE(character2->race == character.race);
        REQUIRE(character2->_class == character._class);
        REQUIRE(character2->map == character.map);
    }

    SECTION( "db_character only for user" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        auto inserted = characters_repo.insert_or_update_character(character, transaction);
        REQUIRE(inserted == true);

        auto character2 = characters_repo.get_character(character.name, usr.id + 1, transaction);
        REQUIRE(!character2);
    }

    SECTION( "db_character updated correctly" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        auto inserted = characters_repo.insert_or_update_character(character, transaction);
        REQUIRE(inserted == true);

        character.race = "race2";
        character._class = "class2";
        character.level = 3;
        character.gold = 4;
        character.xp = 5;
        character.skill_points = 6;
        inserted = characters_repo.insert_or_update_character(character, transaction);
        REQUIRE(inserted == false);

        auto character2 = characters_repo.get_character(character.id, transaction);
        REQUIRE(character2);
        REQUIRE(character2->user_id == character.user_id);
        REQUIRE(character2->name == character.name);
        REQUIRE(character2->level == character.level);
        REQUIRE(character2->gold == character.gold);
        REQUIRE(character2->xp == character.xp);
        REQUIRE(character2->skill_points == character.skill_points);
        REQUIRE(character2->x == character.x);
        REQUIRE(character2->y == character.y);
        REQUIRE(character2->race == character.race);
        REQUIRE(character2->_class == character._class);
        REQUIRE(character2->map == character.map);
    }
    
    SECTION( "db_character get by name with location working correctly" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        auto inserted = characters_repo.insert_or_update_character(character, transaction);
        REQUIRE(inserted == true);

        character.race = "race2";
        character._class = "class2";
        character.level = 3;
        character.gold = 4;
        character.xp = 5;
        character.skill_points = 6;
        inserted = characters_repo.insert_or_update_character(character, transaction);
        REQUIRE(inserted == false);

        auto character2 = characters_repo.get_character(character.name, usr.id, transaction);
        REQUIRE(character2);
        REQUIRE(character2->user_id == character.user_id);
        REQUIRE(character2->name == character.name);
        REQUIRE(character2->level == character.level);
        REQUIRE(character2->gold == character.gold);
        REQUIRE(character2->xp == character.xp);
        REQUIRE(character2->skill_points == character.skill_points);
        REQUIRE(character2->x == character.x);
        REQUIRE(character2->y == character.y);
        REQUIRE(character2->race == character.race);
        REQUIRE(character2->_class == character._class);
        REQUIRE(character2->map == character.map);
    }

    SECTION( "Can't insert db_character twice" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        auto ret = characters_repo.insert(character, transaction);
        REQUIRE(ret == true);

        ret = characters_repo.insert(character, transaction);
        REQUIRE(ret == false);

        auto character2 = characters_repo.get_character(character.id, transaction);
        REQUIRE(character2);
        REQUIRE(character2->user_id == character.user_id);
        REQUIRE(character2->slot == character.slot);
        REQUIRE(character2->level == character.level);
        REQUIRE(character2->gold == character.gold);
        REQUIRE(character2->xp == character.xp);
        REQUIRE(character2->skill_points == character.skill_points);
        REQUIRE(character2->x == character.x);
        REQUIRE(character2->y == character.y);
        REQUIRE(character2->name == character.name);
        REQUIRE(character2->race == character.race);
        REQUIRE(character2->_class == character._class);
        REQUIRE(character2->map == character.map);
    }

    SECTION( "multiple characters retrieved correctly" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        db_character character2{0, usr.id, 8, 9, 10, 11, 12, 13, 14, "john doe2"s, "race2", "class2", "map2", {}, {}};
        characters_repo.insert_or_update_character(character, transaction);
        characters_repo.insert_or_update_character(character2, transaction);

        auto characters = characters_repo.get_by_user_id(character.user_id, transaction);
        REQUIRE(characters.size() == 2);
        REQUIRE(characters[0].items.empty());
        REQUIRE(characters[0].stats.empty());

        characters = characters_repo.get_by_user_id(character.user_id, transaction);
        REQUIRE(characters.size() == 2);
        REQUIRE(characters[0].items.empty());
        REQUIRE(characters[0].stats.empty());
    }

    SECTION( "Get character by slot" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        db_character character2{0, usr.id, 8, 9, 10, 11, 12, 13, 14, "john doe2"s, "race2", "class2", "map2", {}, {}};
        characters_repo.insert_or_update_character(character, transaction);
        characters_repo.insert_or_update_character(character2, transaction);

        auto character_by_slot = characters_repo.get_character_by_slot(1, usr.id, transaction);
        REQUIRE(character_by_slot);
        REQUIRE(character_by_slot->user_id == character.user_id);
        REQUIRE(character_by_slot->slot == character.slot);
        REQUIRE(character_by_slot->level == character.level);
        REQUIRE(character_by_slot->gold == character.gold);
        REQUIRE(character_by_slot->xp == character.xp);
        REQUIRE(character_by_slot->skill_points == character.skill_points);
        REQUIRE(character_by_slot->x == character.x);
        REQUIRE(character_by_slot->y == character.y);
        REQUIRE(character_by_slot->name == character.name);
        REQUIRE(character_by_slot->race == character.race);
        REQUIRE(character_by_slot->_class == character._class);
        REQUIRE(character_by_slot->map == character.map);

        character_by_slot = characters_repo.get_character_by_slot(8, usr.id, transaction);
        REQUIRE(character_by_slot);
        REQUIRE(character_by_slot->user_id == character2.user_id);
        REQUIRE(character_by_slot->slot == character2.slot);
        REQUIRE(character_by_slot->level == character2.level);
        REQUIRE(character_by_slot->gold == character2.gold);
        REQUIRE(character_by_slot->xp == character2.xp);
        REQUIRE(character_by_slot->skill_points == character2.skill_points);
        REQUIRE(character_by_slot->x == character2.x);
        REQUIRE(character_by_slot->y == character2.y);
        REQUIRE(character_by_slot->name == character2.name);
        REQUIRE(character_by_slot->race == character2.race);
        REQUIRE(character_by_slot->_class == character2._class);
        REQUIRE(character_by_slot->map == character2.map);
    }

    SECTION( "Get character by slot with location" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        db_character character2{0, usr.id, 8, 9, 10, 11, 12, 13, 14, "john doe2"s, "race2", "class2", "map2", {}, {}};
        characters_repo.insert_or_update_character(character, transaction);
        characters_repo.insert_or_update_character(character2, transaction);

        auto character_by_slot = characters_repo.get_character_by_slot(1, usr.id, transaction);
        REQUIRE(character_by_slot);
        REQUIRE(character_by_slot->user_id == character.user_id);
        REQUIRE(character_by_slot->slot == character.slot);
        REQUIRE(character_by_slot->level == character.level);
        REQUIRE(character_by_slot->gold == character.gold);
        REQUIRE(character_by_slot->xp == character.xp);
        REQUIRE(character_by_slot->skill_points == character.skill_points);
        REQUIRE(character_by_slot->x == character.x);
        REQUIRE(character_by_slot->y == character.y);
        REQUIRE(character_by_slot->name == character.name);
        REQUIRE(character_by_slot->race == character.race);
        REQUIRE(character_by_slot->_class == character._class);
        REQUIRE(character_by_slot->map == character.map);

        character_by_slot = characters_repo.get_character_by_slot(8, usr.id, transaction);
        REQUIRE(character_by_slot);
        REQUIRE(character_by_slot->user_id == character2.user_id);
        REQUIRE(character_by_slot->slot == character2.slot);
        REQUIRE(character_by_slot->level == character2.level);
        REQUIRE(character_by_slot->gold == character2.gold);
        REQUIRE(character_by_slot->xp == character2.xp);
        REQUIRE(character_by_slot->skill_points == character2.skill_points);
        REQUIRE(character_by_slot->x == character2.x);
        REQUIRE(character_by_slot->y == character2.y);
        REQUIRE(character_by_slot->name == character2.name);
        REQUIRE(character_by_slot->race == character2.race);
        REQUIRE(character_by_slot->_class == character2._class);
        REQUIRE(character_by_slot->map == character2.map);
    }

    SECTION( "Delete character by slot" ) {
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        users_repo.insert_if_not_exists(usr, transaction);

        db_character character{0, usr.id, 1, 2, 3, 4, 5, 6, 7, "john doe"s, "race", "class", "map", {}, {}};
        db_character character2{0, usr.id, 8, 9, 10, 11, 12, 13, 14, "john doe2"s, "race2", "class2", "map2", {}, {}};
        characters_repo.insert_or_update_character(character, transaction);
        characters_repo.insert_or_update_character(character2, transaction);
        characters_repo.delete_character_by_slot(1, usr.id, transaction);

        auto character_by_slot = characters_repo.get_character_by_slot(1, usr.id, transaction);
        REQUIRE(!character_by_slot);

        character_by_slot = characters_repo.get_character_by_slot(8, usr.id, transaction);
        REQUIRE(character_by_slot);
        REQUIRE(character_by_slot->user_id == character2.user_id);
        REQUIRE(character_by_slot->slot == character2.slot);
        REQUIRE(character_by_slot->level == character2.level);
        REQUIRE(character_by_slot->gold == character2.gold);
        REQUIRE(character_by_slot->xp == character2.xp);
        REQUIRE(character_by_slot->skill_points == character2.skill_points);
        REQUIRE(character_by_slot->x == character2.x);
        REQUIRE(character_by_slot->y == character2.y);
        REQUIRE(character_by_slot->name == character2.name);
        REQUIRE(character_by_slot->race == character2.race);
        REQUIRE(character_by_slot->_class == character2._class);
        REQUIRE(character_by_slot->map == character2.map);
    }
}

#endif
