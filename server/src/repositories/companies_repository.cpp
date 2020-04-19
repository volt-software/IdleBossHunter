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

#include "companies_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;
using namespace chrono;

template class ibh::companies_repository<database_transaction>;
template class ibh::companies_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
bool companies_repository<transaction_T>::insert(db_company &company, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO companies (name, no_of_shares, company_type) VALUES ('{}', {}, {}) ON CONFLICT DO NOTHING RETURNING id", transaction->escape(company.name), company.no_of_shares, company.company_type));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        //already exists
        return false;
    }

    company.id = result[0][0].as(uint64_t{});

    return true;
}

template<DatabaseTransaction transaction_T>
void companies_repository<transaction_T>::update(db_company const &company, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("UPDATE companies SET name = '{}', no_of_shares = {} WHERE id = {}", transaction->escape(company.name), company.no_of_shares, company.id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());
}

template<DatabaseTransaction transaction_T>
void companies_repository<transaction_T>::remove(db_company const &company, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("DELETE FROM companies WHERE id = {}", company.id));

    spdlog::trace("[{}] removed {} entries", __FUNCTION__, result.size());
}

template<DatabaseTransaction transaction_T>
optional<db_company> companies_repository<transaction_T>::get(int id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT id, name, no_of_shares, company_type FROM companies WHERE id = {}", id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        return {};
    }

    return make_optional<db_company>(result[0]["id"].as(uint64_t{}), result[0]["name"].as(string{}), result[0]["no_of_shares"].as(uint64_t{}), result[0]["company_type"].as(uint16_t{}));
}

template<DatabaseTransaction transaction_T>
optional<db_company> companies_repository<transaction_T>::get(string const &name, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT id, name, no_of_shares, company_type FROM companies WHERE name = '{}'", transaction->escape(name)));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        return {};
    }

    return make_optional<db_company>(result[0]["id"].as(uint64_t{}), result[0]["name"].as(string{}), result[0]["no_of_shares"].as(uint64_t{}), result[0]["company_type"].as(uint16_t{}));
}

template<DatabaseTransaction transaction_T>
vector<db_company> companies_repository<transaction_T>::get_all(const unique_ptr<transaction_T> &transaction) const {
    auto result = transaction->execute("SELECT id, name, no_of_shares, company_type FROM companies");

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_company> companies;
    companies.reserve(result.size());

    for(auto const & res : result) {
        companies.emplace_back(res["id"].as(uint64_t{}), res["name"].as(string{}), res["no_of_shares"].as(uint64_t{}), res["company_type"].as(uint16_t{}));
    }

    return companies;
}
