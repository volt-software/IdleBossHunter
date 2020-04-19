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

#include <spdlog/spdlog.h>
#include <magic_enum.hpp>
#include <websocket_thread.h>
#include "resource_system.h"
#include "random_helper.h"
#include "on_leaving_scope.h"
#include "macros.h"

using namespace std;
using namespace ibh;

void simulate_resource(uint32_t resource_id, pc_component &pc, entt::registry &es, outward_queues &outward_queue) {
    auto resource = pc.stats.find(resource_id);
    auto resource_xp = pc.stats.find(resource_id + 300u);
    auto resource_level = pc.stats.find(resource_id + 600u);

    if(resource == end(pc.stats)) {
        pc.stats.emplace(resource_id, 1);
    } else {
        resource->second++;
    }

    if(resource_xp == end(pc.stats)) {
        pc.stats.emplace(resource_id + 300u, 1);
    } else {
        resource_xp->second++;

        if(resource_level == end(pc.stats)) {
            pc.stats.emplace(resource_id + 600u, 1);
            resource_level = pc.stats.find(resource_id + 600u);
        }

        auto xp_threshold = 50*pow(2, resource_level->second);
        if(resource_xp->second > xp_threshold) {
            resource_xp->second -= xp_threshold;
            resource_level->second++;
        }
        auto update_msg = make_unique<battle_update_response>(mob_turns, player_turns, mob_hits, player_hits, mob_dmg_to_player, player_dmg_to_mob);
        outward_queue.enqueue_tokenless(outward_message{pc.connection_id, move(update_msg)});
    }
}

void ibh::resource_system::do_tick(entt::registry &es) {
    _tick_count++;

    if(_tick_count < _every_n_ticks) {
        return;
    }

    _tick_count = 0;

    MEASURE_TIME_OF_FUNCTION(info);
    {
        auto pc_group = es.group<pc_component>(entt::get<wood_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_wood_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<ore_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_ore_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<water_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_water_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<plants_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_plants_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<clay_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_clay_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<gems_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_gems_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<paper_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_paper_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<ink_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_ink_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<metal_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_metal_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<bricks_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_bricks_id, pc, es, _outward_queue);
        }
    }

    {
        auto pc_group = es.group<pc_component>(entt::get<timber_gathering_component>);
        for (auto pc_entity : pc_group) {
            pc_component &pc = pc_group.get<pc_component>(pc_entity);
            simulate_resource(resource_timber_id, pc, es, _outward_queue);
        }
    }
}
