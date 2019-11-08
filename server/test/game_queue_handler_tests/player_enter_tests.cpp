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
#include "../test_helpers/startup_helper.h"
#include <game_queue_message_handlers/player_enter_handler.h>
#include <ecs/components.h>

using namespace std;
using namespace lotr;

// TODO

/*

TEST_CASE("character enter tests") {
    outward_queues q;
    SECTION( "character enters world" ) {
        entt::registry registry;

        auto new_entity = registry.create();
        {
            map_component test_map(10, 10, "test", {}, {}, {});

            registry.assign<map_component>(new_entity, move(test_map));
        }

        player_enter_message msg("test_player", "gender", "race", "class", "test", {}, 1, 2, 3, 4, 5);
        handle_player_enter_message(&msg, registry, q);

        auto &test_map = registry.get<map_component>(new_entity);

        REQUIRE(test_map.players.size() == 1);
        REQUIRE(test_map.players[0].name == "test_player");
        REQUIRE(test_map.players[0].gender == "gender");
        REQUIRE(test_map.players[0].race == "race");
        REQUIRE(test_map.players[0].character_class == "class");
        REQUIRE(test_map.players[0].connection_id == 1);
        REQUIRE(test_map.players[0].level == 2);
        REQUIRE(test_map.players[0].gold == 3);
        REQUIRE(get<0>(test_map.players[0].loc) == 4);
        REQUIRE(get<1>(test_map.players[0].loc) == 5);
    }

    SECTION( "character does not enter non-existing world" ) {
        entt::registry registry;

        auto new_entity = registry.create();
        {
            map_component test_map(10, 10, "test", {}, {}, {});

            registry.assign<map_component>(new_entity, move(test_map));
        }

        player_enter_message msg("test_player", "gender", "race", "class", "wrong_map", {}, 1, 2, 3, 4, 5);
        handle_player_enter_message(&msg, registry, q);

        auto &test_map = registry.get<map_component>(new_entity);

        REQUIRE(test_map.players.size() == 0);
    }

    SECTION( "character does not enter world with wrong coordinates" ) {
        entt::registry registry;

        auto new_entity = registry.create();
        {
            map_component test_map(10, 10, "test", {}, {}, {});

            registry.assign<map_component>(new_entity, move(test_map));
        }
        player_enter_message msg("test_player", "gender", "race", "class", "test", {}, 1, 20, 30, 40, 50);
        handle_player_enter_message(&msg, registry, q);

        auto &test_map = registry.get<map_component>(new_entity);

        REQUIRE(test_map.players.size() == 0);
    }
}
*/
