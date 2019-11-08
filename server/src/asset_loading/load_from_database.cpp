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

#include "load_from_database.h"
#include "spdlog/spdlog.h"

#include <repositories/item_stats_repository.h>
#include <repositories/items_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_buildings_repository.h>
#include <repositories/clan_stats_repository.h>

using namespace std;
using namespace lotr;

void lotr::load_from_database(entt::registry &registry, shared_ptr<database_pool> db_pool, atomic<bool> const &quit) {
    item_stats_repository<database_pool, database_transaction> stat_repo(db_pool);
    items_repository<database_pool, database_transaction> items_repo(db_pool);
    characters_repository<database_pool, database_transaction> char_repo(db_pool);
    auto loading_start = chrono::system_clock::now();

    auto loading_end = chrono::system_clock::now();
    spdlog::info("[{}] loaded in {:n} µs", __FUNCTION__, chrono::duration_cast<chrono::microseconds>(loading_end - loading_start).count());
}

