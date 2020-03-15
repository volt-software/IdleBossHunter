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

#include "clans_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;
using namespace chrono;

template class ibh::clans_repository<database_transaction>;
template class ibh::clans_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
bool clans_repository<transaction_T>::insert(db_clan &clan, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO clans (name) VALUES ('{}') ON CONFLICT DO NOTHING RETURNING id", transaction->escape(clan.name)));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        //already exists
        return false;
    }

    clan.id = result[0][0].as(uint64_t{});

    return true;
}

template<DatabaseTransaction transaction_T>
void clans_repository<transaction_T>::update(db_clan const &clan, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("UPDATE clans SET name = '{}' WHERE id = {}", transaction->escape(clan.name), clan.id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());
}

template<DatabaseTransaction transaction_T>
void clans_repository<transaction_T>::remove(db_clan const &clan, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("DELETE FROM clans WHERE id = {}", clan.id));

    spdlog::debug("[{}] removed {} entries", __FUNCTION__, result.size());
}

template<DatabaseTransaction transaction_T>
optional<db_clan> clans_repository<transaction_T>::get(int id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT id, name FROM clans WHERE id = {}", id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        return {};
    }

    return make_optional<db_clan>(result[0]["id"].as(uint64_t{}), result[0]["name"].as(string{}), vector<db_clan_stat>{}, vector<db_clan_building>{});
}

template<DatabaseTransaction transaction_T>
optional<db_clan> clans_repository<transaction_T>::get(string const &name, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT id, name FROM clans WHERE name = '{}'", transaction->escape(name)));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        return {};
    }

    return make_optional<db_clan>(result[0]["id"].as(uint64_t{}), result[0]["name"].as(string{}), vector<db_clan_stat>{}, vector<db_clan_building>{});
}

template<DatabaseTransaction transaction_T>
vector<db_clan> clans_repository<transaction_T>::get_all(const unique_ptr<transaction_T> &transaction) const {
    auto result = transaction->execute("SELECT id, name FROM clans");

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_clan> clans;
    clans.reserve(result.size());

    for(auto const & res : result) {
        clans.emplace_back(res["id"].as(uint64_t{}), res["name"].as(string{}), vector<db_clan_stat>{}, vector<db_clan_building>{});
    }

    return clans;
}
