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

#include "company_stats_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;

template class ibh::company_stats_repository<database_transaction>;
template class ibh::company_stats_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
void company_stats_repository<transaction_T>::insert(db_company_stat &stat, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO company_stats (company_id, stat_id, value) VALUES ({}, {}, {}) RETURNING id", stat.company_id, stat.stat_id, stat.value));

    if(result.empty()) {
        spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());
        return;
    }

    stat.id = result[0][0].as(uint32_t{});

    spdlog::trace("[{}] inserted stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
void company_stats_repository<transaction_T>::update(db_company_stat const &stat, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE company_stats SET value = {} WHERE id = {}", stat.value, stat.id));

    spdlog::trace("[{}] updated stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
void company_stats_repository<transaction_T>::update_by_stat_id(db_company_stat const &stat, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE company_stats SET value = {} WHERE company_id = {} AND stat_id = {}", stat.value, stat.company_id, stat.stat_id));

    spdlog::trace("[{}] updated stat {}", __FUNCTION__, stat.id);
}

template<DatabaseTransaction transaction_T>
optional<db_company_stat> company_stats_repository<transaction_T>::get(uint64_t id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.company_id, s.stat_id, s.value FROM company_stats s WHERE s.id = {}" , id));

    if(result.empty()) {
        spdlog::trace("[{}] found no stat by id {}", __FUNCTION__, id);
        return {};
    }

    auto ret = make_optional<db_company_stat>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                          result[0][2].as(uint64_t{}), result[0][3].as(int64_t{}));

    spdlog::trace("[{}] found stat by id {}", __FUNCTION__, id);

    return ret;
}

template<DatabaseTransaction transaction_T>
optional<db_company_stat> company_stats_repository<transaction_T>::get_by_stat(uint64_t company_id, uint64_t stat_id, const unique_ptr<transaction_T> &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.company_id, s.stat_id, s.value FROM company_stats s WHERE s.company_id = {} AND s.stat_id = {}" , company_id, stat_id));

    if(result.empty()) {
        spdlog::error("[{}] found no stat {} for company {}", __FUNCTION__, stat_id, company_id);
        return {};
    }

    auto ret = make_optional<db_company_stat>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                           result[0][2].as(uint64_t{}), result[0][3].as(int64_t{}));

    spdlog::trace("[{}] found stat {} for company {}", __FUNCTION__, stat_id, company_id);

    return ret;
}

template<DatabaseTransaction transaction_T>
vector<db_company_stat> company_stats_repository<transaction_T>::get_by_company_id(uint64_t company_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT s.id, s.company_id, s.stat_id, s.value FROM company_stats s WHERE s.company_id = {}", company_id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_company_stat> stats;
    stats.reserve(result.size());

    for(auto const & res : result) {
        stats.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}),
                           res[2].as(uint64_t{}), res[3].as(int64_t{}));
    }

    return stats;
}
