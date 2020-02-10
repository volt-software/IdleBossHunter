/*
    IdleBossHunter
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

#include "boss_stats_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;

template class ibh::boss_stats_repository<database_transaction>;
template class ibh::boss_stats_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
void boss_stats_repository<transaction_T>::insert(db_boss_stat &stat, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO boss_stats (boss_id, stat_name, value) VALUES ({}, '{}', {}) RETURNING id", stat.boss_id, transaction->escape(stat.name), stat.value));

    if(result.empty()) {
        spdlog::error("[{}] contains {} entries", __FUNCTION__, result.size());
        return;
    }

    stat.id = result[0][0].as(uint32_t{});

    spdlog::debug("[{}] inserted stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
void boss_stats_repository<transaction_T>::update(db_boss_stat const &stat, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE boss_stats SET value = {} WHERE id = {}", stat.value, stat.id));

    spdlog::debug("[{}] updated stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
optional<db_boss_stat> boss_stats_repository<transaction_T>::get(uint64_t id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.boss_id, s.stat_name, s.value FROM boss_stats s WHERE s.id = {}" , id));

    if(result.empty()) {
        spdlog::error("[{}] found no stat by id {}", __FUNCTION__, id);
        return {};
    }

    auto ret = make_optional<db_boss_stat>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                          result[0][2].as(string{}), result[0][3].as(int64_t{}));

    spdlog::trace("[{}] found stat by id {}", __FUNCTION__, id);

    return ret;
}

template<DatabaseTransaction transaction_T>
vector<db_boss_stat> boss_stats_repository<transaction_T>::get_by_boss_id(uint64_t boss_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.boss_id, s.stat_name, s.value FROM boss_stats s WHERE s.boss_id = {}", boss_id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_boss_stat> stats;
    stats.reserve(result.size());

    for(auto const & res : result) {
        stats.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}),
                           res[2].as(string{}), res[3].as(int64_t{}));
    }

    return stats;
}
