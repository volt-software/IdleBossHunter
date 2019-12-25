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

#include "bosses_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;
using namespace chrono;

template class ibh::bosses_repository<database_pool, database_transaction>;

template<typename pool_T, typename transaction_T>
bosses_repository<pool_T, transaction_T>::bosses_repository(shared_ptr<pool_T> database_pool) : _database_pool(move(database_pool)) {

}

template<typename pool_T, typename transaction_T>
unique_ptr<transaction_T> bosses_repository<pool_T, transaction_T>::create_transaction() {
    return _database_pool->create_transaction();
}

template<typename pool_T, typename transaction_T>
bool bosses_repository<pool_T, transaction_T>::insert(db_boss &boss, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO bosses (name) VALUES ('{}') ON CONFLICT DO NOTHING RETURNING id", transaction->escape(boss.name)));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        //already exists
        return false;
    }

    boss.id = result[0][0].as(uint64_t{});

    return true;
}

template<typename pool_T, typename transaction_T>
void bosses_repository<pool_T, transaction_T>::update(db_boss const &boss, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("UPDATE bosses SET name = '{}' WHERE id = {}", transaction->escape(boss.name), boss.id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());
}

template<typename pool_T, typename transaction_T>
optional<db_boss> bosses_repository<pool_T, transaction_T>::get(int id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT id, name FROM bosses WHERE id = {}", id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        return {};
    }

    return make_optional<db_boss>(result[0]["id"].as(uint64_t{}), result[0]["name"].as(string{}), vector<db_boss_stat>{});
}
