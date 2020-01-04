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

#include "load_assets.h"
#include <filesystem>
#include "spdlog/spdlog.h"
#include "load_monsters.h"
#include "load_monster_specials.h"
#include <random_helper.h>
#include <game_logic/logic_helpers.h>

using namespace std;
using namespace ibh;

void ibh::load_assets(entt::registry &registry, atomic<bool> const &quit) {
    uint32_t monster_count = 0;
    uint32_t monster_specials_count = 0;
    auto loading_start = chrono::system_clock::now();

    for(auto& p: filesystem::recursive_directory_iterator("assets/monsters")) {
        if(!p.is_regular_file() || quit) {
            continue;
        }

        auto monster = load_monsters(p.path().string());

        if(!monster) {
            continue;
        }

        auto new_entity = registry.create();
        registry.assign<monster_definition_component>(new_entity, move(monster.value()));

        monster_count++;
    }

    auto specials_loading_start = chrono::system_clock::now();

    for(auto& p: filesystem::recursive_directory_iterator("assets/monster_specials")) {
        if(!p.is_regular_file() || quit) {
            continue;
        }

        auto special = load_monster_specials(p.path().string());

        if(!special) {
            continue;
        }

        auto new_entity = registry.create();
        registry.assign<monster_special_definition_component>(new_entity, move(special.value()));

        monster_specials_count++;
    }


    auto loading_end = chrono::system_clock::now();
    spdlog::info("[{}] {:n} monsters loaded in {:n} µs", __FUNCTION__, monster_count, chrono::duration_cast<chrono::microseconds>(specials_loading_start - loading_start).count());
    spdlog::info("[{}] {:n} monster specials loaded in {:n} µs", __FUNCTION__, monster_specials_count, chrono::duration_cast<chrono::microseconds>(loading_end - specials_loading_start).count());
//    spdlog::info("[{}] {:n} maps loaded in {:n} µs", __FUNCTION__, map_count, chrono::duration_cast<chrono::microseconds>(entity_spawning_start - spawners_loading_start).count());
//    spdlog::info("[{}] {:n} entities spawned in {:n} µs", __FUNCTION__, entity_count, chrono::duration_cast<chrono::microseconds>(loading_end - entity_spawning_start).count());
    spdlog::info("[{}] everything loaded in {:n} µs", __FUNCTION__, chrono::duration_cast<chrono::microseconds>(loading_end - loading_start).count());
}

