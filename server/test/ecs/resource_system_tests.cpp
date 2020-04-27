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
#include <ecs/resource_system.h>
#include <game_queue_messages/messages.h>

using namespace std;
using namespace ibh;

void simulate_resource(uint32_t resource_id, pc_component &pc, outward_queues &outward_queue);

TEST_CASE("simulate_resource test") {
    pc_component pc{};
    moodycamel::ConcurrentQueue<outward_message> cq;
    outward_queues q{&cq};
    for(uint32_t i = 0; i < 10; i++) {
        simulate_resource(resource_wood_id, pc, q);
        auto resource = pc.stats.find(resource_wood_id);
        auto resource_xp = pc.stats.find(resource_wood_id + 300u);
        auto resource_level = pc.stats.find(resource_wood_id + 600u);

        REQUIRE(resource != end(pc.stats));
        REQUIRE(resource_xp != end(pc.stats));
        REQUIRE(resource_level != end(pc.stats));

        REQUIRE(resource->second == i + 1);
        REQUIRE(resource_xp->second == i + 1);
        REQUIRE(resource_level->second == 1);
    }

    for(uint32_t i = 0; i < 91; i++) {
        simulate_resource(resource_wood_id, pc, q);
    }

    auto resource_level = pc.stats.find(resource_wood_id + 600u);
    REQUIRE(resource_level->second == 2);
}