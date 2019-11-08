/*
    RealmOfAesirWorld
    Copyright (C) 2017  Michael de Lang

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

#include "clan_stats_repository.h"
#include <spdlog/spdlog.h>

using namespace lotr;

template class lotr::clan_stats_repository<database_pool, database_transaction>;

template<typename pool_T, typename transaction_T>
clan_stats_repository<pool_T, transaction_T>::clan_stats_repository(shared_ptr<pool_T> database_pool) : _database_pool(move(database_pool)) {

}

template<typename pool_T, typename transaction_T>
unique_ptr<transaction_T> clan_stats_repository<pool_T, transaction_T>::create_transaction() {
    return _database_pool->create_transaction();
}

template<typename pool_T, typename transaction_T>
void clan_stats_repository<pool_T, transaction_T>::insert(db_clan_stat &stat, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO clan_stats (clan_id, stat_name, value) VALUES ({}, '{}', {}) RETURNING id", stat.clan_id, transaction->escape(stat.name), stat.value));

    if(result.empty()) {
        spdlog::error("[{}] contains {} entries", __FUNCTION__, result.size());
        return;
    }

    stat.id = result[0][0].as(uint32_t{});

    spdlog::debug("[{}] inserted stat {}", __FUNCTION__, stat.id);
}

template<typename pool_T, typename transaction_T>
void clan_stats_repository<pool_T, transaction_T>::update(db_clan_stat const &stat, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE clan_stats SET value = {} WHERE id = {}", stat.value, stat.id));

    spdlog::debug("[{}] updated stat {}", __FUNCTION__, stat.id);
}

template<typename pool_T, typename transaction_T>
optional<db_clan_stat> clan_stats_repository<pool_T, transaction_T>::get(uint64_t id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.clan_id, s.stat_name, s.value FROM clan_stats s WHERE s.id = {}" , id));

    if(result.empty()) {
        spdlog::error("[{}] found no stat by id {}", __FUNCTION__, id);
        return {};
    }

    auto ret = make_optional<db_clan_stat>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                          result[0][2].as(string{}), result[0][3].as(int64_t{}));

    spdlog::trace("[{}] found stat by id {}", __FUNCTION__, id);

    return ret;
}

template<typename pool_T, typename transaction_T>
vector<db_clan_stat> clan_stats_repository<pool_T, transaction_T>::get_by_clan_id(uint64_t clan_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.clan_id, s.stat_name, s.value FROM clan_stats s WHERE s.clan_id = {}", clan_id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_clan_stat> stats;
    stats.reserve(result.size());

    for(auto const & res : result) {
        stats.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}),
                           res[2].as(string{}), res[3].as(int64_t{}));
    }

    return stats;
}
