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

#include "set_action_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/resources/set_action_response.h>
#include <game_queue_message_handlers/handler_helpers.h>
#include <magic_enum.hpp>

using namespace std;

namespace ibh {
    bool handle_set_action(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *set_action_msg = dynamic_cast<set_action_message*>(msg);

        if(set_action_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_group = es.view<pc_component>();
        for(auto entity : pc_group) {
            auto &pc = pc_group.get<pc_component>(entity);

            if(pc.connection_id != set_action_msg->connection_id) {
                continue;
            }


            if(!magic_enum::enum_contains<selectable_actions>(set_action_msg->action_id)) {
                auto new_err_msg = make_unique<set_action_response>("Wrong action id");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            es.remove_if_exists<battle_component, wood_gathering_component, ore_gathering_component, water_gathering_component, plants_gathering_component, clay_gathering_component,
                    paper_gathering_component, ink_gathering_component, metal_gathering_component, bricks_gathering_component, gems_gathering_component, timber_gathering_component,
                    item_gathering_component, working_component>(entity);

            switch(set_action_msg->action_id) {
                case magic_enum::enum_integer(selectable_actions::COMBAT):
                    es.emplace<battle_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::WOOD_GATHERING):
                    es.emplace<wood_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::ORE_GATHERING):
                    es.emplace<ore_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::WATER_GATHERING):
                    es.emplace<water_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::PLANTS_GATHERING):
                    es.emplace<plants_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::CLAY_GATHERING):
                    es.emplace<clay_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::PAPER_CRAFTING):
                    es.emplace<paper_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::INK_CRAFTING):
                    es.emplace<ink_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::METAL_FORGING):
                    es.emplace<metal_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::BRICK_FIRING):
                    es.emplace<bricks_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::GEM_CRAFTING):
                    es.emplace<gems_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::WOOD_WORKING):
                    es.emplace<timber_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::ITEM_CRAFTING):
                    es.emplace<item_gathering_component>(entity);
                    break;
                case magic_enum::enum_integer(selectable_actions::WORKING):
                    es.emplace<working_component>(entity);
                    break;
                default:
                    auto new_err_msg = make_unique<set_action_response>("Wrong action id");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
            }

            auto new_err_msg = make_unique<set_action_response>("");
            outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});

            return true;
        }

        auto new_err_msg = make_unique<set_action_response>("Couldn't find you");
        outward_queue.enqueue(outward_message{set_action_msg->connection_id, move(new_err_msg)});
        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, set_action_msg->connection_id);

        return false;
    }
}
