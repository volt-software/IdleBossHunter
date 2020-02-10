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
#include "repositories/items_repository.h"
#include "repositories/characters_repository.h"
#include "repositories/users_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("items repository tests") {
    items_repository<database_transaction> items_repo{};
    characters_repository<database_transaction> char_repo{};
    users_repository<database_transaction> user_repo{};

    SECTION("item inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_user u{};
        user_repo.insert_if_not_exists(u, transaction);
        REQUIRE(u.id > 0);
        db_character c{};
        c.user_id = u.id;
        char_repo.insert(c, transaction);
        REQUIRE(c.id > 0);
        db_item item{0, c.id, "item", "slot", "equip"};
        items_repo.insert(item, transaction);
        REQUIRE(item.id > 0);

        auto item2 = items_repo.get_item(item.id, transaction);
        REQUIRE(item2->id == item.id);
        REQUIRE(item2->character_id == item.character_id);
        REQUIRE(item2->name == item.name);
        REQUIRE(item2->slot == item.slot);
        REQUIRE(item2->equip_slot == item.equip_slot);
    }

    SECTION( "update item equip_slot" ) {
        auto transaction = db_pool->create_transaction();
        db_user u{};
        user_repo.insert_if_not_exists(u, transaction);
        REQUIRE(u.id > 0);
        db_character c{};
        c.user_id = u.id;
        char_repo.insert(c, transaction);
        REQUIRE(c.id > 0);
        db_item item{0, c.id, "item", "slot", "equip"};
        items_repo.insert(item, transaction);
        REQUIRE(item.id > 0);

        item.equip_slot = "second_equip";
        items_repo.update_item(item, transaction);

        auto item2 = items_repo.get_item(item.id, transaction);
        REQUIRE(item2->id == item.id);
        REQUIRE(item2->character_id == item.character_id);
        REQUIRE(item2->name == item.name);
        REQUIRE(item2->slot == item.slot);
        REQUIRE(item2->equip_slot == item.equip_slot);
    }

    SECTION( "update item npc/character ids" ) {
        auto transaction = db_pool->create_transaction();
        db_user u{};
        user_repo.insert_if_not_exists(u, transaction);
        REQUIRE(u.id > 0);
        db_character c{};
        c.user_id = u.id;
        c.slot = 0;
        char_repo.insert(c, transaction);
        REQUIRE(c.id > 0);
        db_character c2{};
        c2.user_id = u.id;
        c2.slot = 1;
        char_repo.insert(c2, transaction);
        REQUIRE(c2.id > 0);
        db_item item{0, c.id, "item", "slot", "equip"};
        items_repo.insert(item, transaction);
        REQUIRE(item.id > 0);

        item.character_id = c2.id;
        items_repo.update_item(item, transaction);

        auto item2 = items_repo.get_item(item.id, transaction);
        REQUIRE(item2->id == item.id);
        REQUIRE(item2->character_id == item.character_id);
        REQUIRE(item2->name == item.name);
        REQUIRE(item2->slot == item.slot);
        REQUIRE(item2->equip_slot == item.equip_slot);
    }

    SECTION( "delete item" ) {
        auto transaction = db_pool->create_transaction();
        db_user u{};
        user_repo.insert_if_not_exists(u, transaction);
        REQUIRE(u.id > 0);
        db_character c{};
        c.user_id = u.id;
        char_repo.insert(c, transaction);
        REQUIRE(c.id > 0);
        db_item item{0, c.id, "item", "slot", "equip"};
        items_repo.insert(item, transaction);
        REQUIRE(item.id > 0);

        items_repo.delete_item(item, transaction);

        auto item2 = items_repo.get_item(item.id, transaction);
        REQUIRE(!item2);
    }

    SECTION( "get items by character id" ) {
        auto transaction = db_pool->create_transaction();
        db_user u{};
        user_repo.insert_if_not_exists(u, transaction);
        REQUIRE(u.id > 0);
        db_character c{};
        c.user_id = u.id;
        char_repo.insert(c, transaction);
        REQUIRE(c.id > 0);
        db_item item{0, c.id, "item", "slot", "equip"};
        db_item item2{0, c.id, "item2", "slot2", "equip2"};
        items_repo.insert(item, transaction);
        REQUIRE(item.id > 0);
        items_repo.insert(item2, transaction);
        REQUIRE(item2.id > 0);

        auto items = items_repo.get_by_character_id(c.id, transaction);
        REQUIRE(items.size() == 2);
    }
}

#endif
