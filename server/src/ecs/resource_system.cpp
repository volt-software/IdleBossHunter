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

#include <execution>
#include <spdlog/spdlog.h>
#include <magic_enum.hpp>
#include <websocket_thread.h>
#include "resource_system.h"
#include "random_helper.h"
#include "on_leaving_scope.h"
#include "macros.h"
#include <messages/resources/resource_update_response.h>

using namespace std;
using namespace ibh;

void simulate_resource(uint32_t resource_id, pc_component &pc, outward_queues &outward_queue) {
    auto resource_level = pc.stats.find(resource_id + 600u);
    if(resource_level == end(pc.stats)) {
        pc.stats.emplace(resource_id + 600u, 1);
        resource_level = pc.stats.find(resource_id + 600u);
    }

    auto resource_amt = pc.stats.find(resource_id);
    if(resource_amt == end(pc.stats)) {
        pc.stats.emplace(resource_id, 1);
        resource_amt = pc.stats.find(resource_id);
    } else {
        resource_amt->second++;
    }

    auto resource_xp = pc.stats.find(resource_id + 300u);
    if(resource_xp == end(pc.stats)) {
        pc.stats.emplace(resource_id + 300u, 1);
        resource_xp = pc.stats.find(resource_id + 300u);
    } else {
        resource_xp->second++;

        auto xp_threshold = 50*pow(2, resource_level->second);
        if(resource_xp->second > xp_threshold) {
            resource_xp->second -= xp_threshold;
            resource_level->second++;
        }
    }

    auto update_msg = make_unique<resource_update_response>(vector<resource>{
        {resource_id, static_cast<uint64_t>(resource_amt->second), static_cast<uint64_t>(resource_xp->second), static_cast<uint64_t>(resource_level->second)}
    });
    outward_queue.enqueue_tokenless(outward_message{pc.connection_id, move(update_msg)});
}
template <typename T>
void tick_for(entt::registry &es, uint32_t resource_id, queue_abstraction<outward_message> &outward_queue) {
    auto pc_group = es.group<T>(entt::get<pc_component>);
    for_each(execution::par_unseq, begin(pc_group), end(pc_group), [resource_id, &outward_queue, &pc_group](auto entity){
        auto &pc = pc_group.template get<pc_component>(entity);
        simulate_resource(resource_id, pc, outward_queue);
    });
}

void ibh::resource_system::do_tick(entt::registry &es) {
    _tick_count++;

    if(_tick_count < _every_n_ticks) {
        return;
    }

    _tick_count = 0;

    MEASURE_TIME_OF_FUNCTION(info);
    tick_for<wood_gathering_component>(es, resource_wood_id, _outward_queue);
    tick_for<ore_gathering_component>(es, resource_ore_id, _outward_queue);
    tick_for<water_gathering_component>(es, resource_water_id, _outward_queue);
    tick_for<plants_gathering_component>(es, resource_plants_id, _outward_queue);
    tick_for<clay_gathering_component>(es, resource_clay_id, _outward_queue);
    tick_for<paper_gathering_component>(es, resource_paper_id, _outward_queue);
    tick_for<ink_gathering_component>(es, resource_ink_id, _outward_queue);
    tick_for<metal_gathering_component>(es, resource_metal_id, _outward_queue);
    tick_for<bricks_gathering_component>(es, resource_bricks_id, _outward_queue);
    tick_for<gems_gathering_component>(es, resource_gems_id, _outward_queue);
    tick_for<timber_gathering_component>(es, resource_timber_id, _outward_queue);
}
