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

#include "company_buildings_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;
using namespace chrono;

template class ibh::company_buildings_repository<database_transaction>;
template class ibh::company_buildings_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
bool company_buildings_repository<transaction_T>::insert(db_company_building &company, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO company_buildings (name, company_id) VALUES ('{}', {}) ON CONFLICT DO NOTHING RETURNING id", transaction->escape(company.name), company.company_id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        //already exists
        return false;
    }

    company.id = result[0][0].as(uint64_t{});

    return true;
}

template<DatabaseTransaction transaction_T>
void company_buildings_repository<transaction_T>::update(db_company_building const &company, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("UPDATE company_buildings SET name = '{}' WHERE id = {}", transaction->escape(company.name), company.id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());
}

template<DatabaseTransaction transaction_T>
optional<db_company_building> company_buildings_repository<transaction_T>::get(int id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT id, company_id, name FROM company_buildings WHERE id = {}", id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        return {};
    }

    return make_optional<db_company_building>(result[0]["id"].as(uint64_t{}), result[0]["company_id"].as(uint64_t{}), result[0]["name"].as(string{}));
}
