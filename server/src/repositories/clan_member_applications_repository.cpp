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

#include "clan_member_applications_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;

template class ibh::clan_member_applications_repository<database_transaction>;
template class ibh::clan_member_applications_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
void clan_member_applications_repository<transaction_T>::insert(db_clan_member &member, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO clan_member_applications (clan_id, character_id) VALUES ({}, {})", member.clan_id, member.character_id));

    if(result.empty()) {
        spdlog::error("[{}] contains {} entries", __FUNCTION__, result.size());
        return;
    }

    spdlog::debug("[{}] inserted member {}-{}", __FUNCTION__, member.clan_id, member.character_id);
}

template<DatabaseTransaction transaction_T>
void clan_member_applications_repository<transaction_T>::remove(db_clan_member const &member, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("DELETE FROM clan_member_applications WHERE clan_id = {} AND character_id = {}", member.clan_id, member.character_id));

    spdlog::debug("[{}] updated member {}-{}", __FUNCTION__, member.clan_id, member.character_id);
}

template<DatabaseTransaction transaction_T>
optional<db_clan_member> clan_member_applications_repository<transaction_T>::get(uint64_t clan_id, uint64_t character_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.clan_id, m.character_id FROM clan_member_applications m WHERE m.clan_id = {} AND m.character_id = {}" , clan_id, character_id));

    if(result.empty()) {
        spdlog::error("[{}] found no member by clan_id {} character_id {}", __FUNCTION__, clan_id, character_id);
        return {};
    }

    auto ret = make_optional<db_clan_member>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}), 0);

    spdlog::trace("[{}] found member by clan_id {} character_id {}", __FUNCTION__, clan_id, character_id);

    return ret;
}

template<DatabaseTransaction transaction_T>
vector<db_clan_member> clan_member_applications_repository<transaction_T>::get_by_clan_id(uint64_t clan_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.clan_id, m.character_id FROM clan_member_applications m WHERE m.clan_id = {}", clan_id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_clan_member> members;
    members.reserve(result.size());

    for(auto const & res : result) {
        members.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}), 0);
    }

    return members;
}

template<DatabaseTransaction transaction_T>
vector<db_clan_member> clan_member_applications_repository<transaction_T>::get_by_character_id(uint64_t character_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.clan_id, m.character_id FROM clan_member_applications m WHERE m.character_id = {}", character_id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_clan_member> members;
    members.reserve(result.size());

    for(auto const & res : result) {
        members.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}), 0);
    }

    return members;
}
