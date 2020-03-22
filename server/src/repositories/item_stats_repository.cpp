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

#include "item_stats_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;

template class ibh::item_stats_repository<database_transaction>;
template class ibh::item_stats_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
void item_stats_repository<transaction_T>::insert(db_item_stat &stat, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO item_stats (item_id, stat_id, value) VALUES ({}, '{}', {}) RETURNING id", stat.item_id, stat.stat_id, stat.value));

    if(result.empty()) {
        spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());
        return;
    }

    stat.id = result[0][0].as(uint32_t{});

    spdlog::trace("[{}] inserted stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
void item_stats_repository<transaction_T>::update(db_item_stat const &stat, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE item_stats SET value = {} WHERE id = {}", stat.value, stat.id));

    spdlog::trace("[{}] updated stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
void item_stats_repository<transaction_T>::update_by_stat_id(db_item_stat const &stat, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE item_stats SET value = {} WHERE item_id = {} AND stat_id = {}", stat.value, stat.item_id, stat.stat_id));

    spdlog::trace("[{}] updated stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
optional<db_item_stat> item_stats_repository<transaction_T>::get(uint64_t id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.item_id, s.stat_id, s.value FROM item_stats s WHERE s.id = {}" , id));

    if(result.empty()) {
        spdlog::trace("[{}] found no stat by id {}", __FUNCTION__, id);
        return {};
    }

    auto ret = make_optional<db_item_stat>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                          result[0][2].as(uint64_t{}), result[0][3].as(int64_t{}));

    spdlog::trace("[{}] found stat by id {}", __FUNCTION__, id);

    return ret;
}

template<DatabaseTransaction transaction_T>
vector<db_item_stat> item_stats_repository<transaction_T>::get_by_item_id(uint64_t item_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.item_id, s.stat_id, s.value FROM item_stats s WHERE s.item_id = {}", item_id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_item_stat> stats;
    stats.reserve(result.size());

    for(auto const & res : result) {
        stats.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}),
                           res[2].as(uint64_t{}), res[3].as(int64_t{}));
    }

    return stats;
}
