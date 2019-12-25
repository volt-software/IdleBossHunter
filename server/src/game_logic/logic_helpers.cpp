/*
    IdleBossHunter
    Copyright (C) 2019 Michael de Lang

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

#include "logic_helpers.h"

#include <spdlog/spdlog.h>
#include <random_helper.h>

using namespace std;
using namespace ibh;

void ibh::remove_dead_monsters(entt::registry &registry) noexcept {
    // TODO
    //npcs.erase(remove_if(begin(npcs), end(npcs), [&](npc_component &npc) noexcept { return npc.stats[stat_hp] <= 0; }), end(npcs));
}

void ibh::fill_spawners(entt::registry &registry) {
    // TODO
}
