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
#include "../game_queue_helpers.h"
#include <game_queue_message_handlers/resources/set_action_handler.h>
#include <ecs/components.h>
#include <messages/resources/set_action_response.h>
#include <magic_enum.hpp>

using namespace std;
using namespace ibh;

template <class NewComponentT>
void test_set_action(entt::entity existing_entt, selectable_actions action, entt::registry &registry) {
    auto transaction = db_pool->create_transaction();
    moodycamel::ConcurrentQueue<outward_message> cq;
    outward_queues q(&cq);
    set_action_message msg(1, magic_enum::enum_integer(action));

    auto ret = handle_set_action(&msg, registry, q, transaction);
    REQUIRE(ret == true);

    test_outmsg<set_action_response>(q, true);
    REQUIRE((is_same_v<NewComponentT, battle_component> || !registry.has<battle_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, wood_gathering_component> || !registry.has<wood_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, ore_gathering_component> || !registry.has<ore_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, water_gathering_component> || !registry.has<water_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, plants_gathering_component> || !registry.has<plants_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, clay_gathering_component> || !registry.has<clay_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, paper_gathering_component> || !registry.has<paper_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, ink_gathering_component> || !registry.has<ink_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, metal_gathering_component> || !registry.has<metal_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, bricks_gathering_component> || !registry.has<bricks_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, gems_gathering_component> || !registry.has<gems_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, timber_gathering_component> || !registry.has<timber_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, item_gathering_component> || !registry.has<item_gathering_component>(existing_entt)));
    REQUIRE((is_same_v<NewComponentT, working_component> || !registry.has<working_component>(existing_entt)));
    REQUIRE(registry.has<NewComponentT>(existing_entt));
}

TEST_CASE("set action handler tests") {
    SECTION( "sets action" ) {
        entt::registry registry;

        auto existing_entt = registry.create();
        {
            pc_component pc{};
            pc.id = 1;
            pc.connection_id = 1;
            registry.emplace<pc_component>(existing_entt, move(pc));
            registry.emplace<battle_component>(existing_entt);
        }

        test_set_action<wood_gathering_component>(existing_entt, selectable_actions::WOOD_GATHERING, registry);
        test_set_action<ore_gathering_component>(existing_entt, selectable_actions::ORE_GATHERING, registry);
        test_set_action<water_gathering_component>(existing_entt, selectable_actions::WATER_GATHERING, registry);
        test_set_action<plants_gathering_component>(existing_entt, selectable_actions::PLANTS_GATHERING, registry);
        test_set_action<clay_gathering_component>(existing_entt, selectable_actions::CLAY_GATHERING, registry);
        test_set_action<paper_gathering_component>(existing_entt, selectable_actions::PAPER_CRAFTING, registry);
        test_set_action<ink_gathering_component>(existing_entt, selectable_actions::INK_CRAFTING, registry);
        test_set_action<metal_gathering_component>(existing_entt, selectable_actions::METAL_FORGING, registry);
        test_set_action<bricks_gathering_component>(existing_entt, selectable_actions::BRICK_FIRING, registry);
        test_set_action<gems_gathering_component>(existing_entt, selectable_actions::GEM_CRAFTING, registry);
        test_set_action<timber_gathering_component>(existing_entt, selectable_actions::WOOD_WORKING, registry);
        test_set_action<item_gathering_component>(existing_entt, selectable_actions::ITEM_CRAFTING, registry);
        test_set_action<working_component>(existing_entt, selectable_actions::WORKING, registry);
        test_set_action<battle_component>(existing_entt, selectable_actions::COMBAT, registry);
    }
}
