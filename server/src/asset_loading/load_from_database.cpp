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

#include "load_from_database.h"
#include "spdlog/spdlog.h"

#include <repositories/item_stats_repository.h>
#include <repositories/items_repository.h>
#include <repositories/item_stats_repository.h>
#include <repositories/users_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/character_stats_repository.h>
#include <repositories/companies_repository.h>
#include <repositories/company_buildings_repository.h>
#include <repositories/company_stats_repository.h>
#include <iterator>
#include <ecs/components.h>

using namespace std;
using namespace ibh;

void ibh::load_from_database(entt::registry &registry, const shared_ptr<database_pool> &db_pool, atomic<bool> const &quit) {
    item_stats_repository<database_transaction> stat_repo{};
    items_repository<database_transaction> items_repo{};
    item_stats_repository<database_transaction> item_stats_repo{};
    users_repository<database_transaction> user_repo{};
    characters_repository<database_transaction> char_repo{};
    character_stats_repository<database_transaction> char_stats_repo{};
    auto loading_start = chrono::system_clock::now();

    auto transaction = db_pool->create_transaction();
    auto users = user_repo.get_all(transaction);
    vector<db_character> all_characters;

    for(auto &user : users) {
        auto characters = char_repo.get_by_user_id(user.id, transaction);
        all_characters.reserve(all_characters.size() + characters.size());

        for(auto &character : characters) {
            character.stats = char_stats_repo.get_by_character_id(character.id, transaction);
            character.items = items_repo.get_by_character_id(character.id, transaction);
            for(auto &item : character.items) {
                item.stats = item_stats_repo.get_by_item_id(item.id, transaction);
            }
        }

        all_characters.insert(end(all_characters), make_move_iterator(begin(characters)), make_move_iterator(end(characters)));
    }

    for(auto &character : all_characters) {
        ibh_flat_map<uint32_t , int64_t> stats;
        vector<item_component> items;

        for(auto &stat : character.stats) {
            stats.emplace(stat.stat_id, stat.value);
        }

        for(auto &item : character.items) {
            vector<stat_component> item_stats;
            item_stats.reserve(item.stats.size());
            for(auto &stat : item.stats) {
                item_stats.emplace_back(stat.stat_id, stat.value);
            }
            items.emplace_back(item.name, "", item.slot, 0, 0, 0, 0, 0, false, false, move(item_stats));
        }

        spdlog::trace("[{}] loaded character id {} name {} no. of items {} no. of stats {}", __FUNCTION__, character.id, character.name, items.size(), stats.size());
        auto new_entity = registry.create();
        registry.emplace<pc_component>(new_entity, pc_component{character.id, 0, character.name, character.race, "",
                                                               character._class, "", character.level,
                                                               character.skill_points, stats,
                                                               ibh_flat_map<uint32_t, item_component>{}, (items),
                                                               ibh_flat_map<string, skill_component>{}});
    }

    auto loading_end = chrono::system_clock::now();
    spdlog::info("[{}] database to game loaded in {:n} µs", __FUNCTION__, chrono::duration_cast<chrono::microseconds>(loading_end - loading_start).count());
}

