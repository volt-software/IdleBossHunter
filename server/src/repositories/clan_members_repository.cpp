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

#include "clan_members_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;

template class ibh::clan_members_repository<database_transaction>;
template class ibh::clan_members_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
void clan_members_repository<transaction_T>::insert(db_clan_member &member, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("INSERT INTO clan_members (clan_id, character_id, member_level) VALUES ({}, {}, {})", member.clan_id, member.character_id, member.member_level));

    if(result.empty()) {
        spdlog::error("[{}] contains {} entries", __FUNCTION__, result.size());
        return;
    }

    spdlog::debug("[{}] inserted member {}-{}", __FUNCTION__, member.clan_id, member.character_id);
}

template<DatabaseTransaction transaction_T>
void clan_members_repository<transaction_T>::update(db_clan_member const &member, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE clan_members SET member_level = {} WHERE clan_id = {} AND character_id = {}", member.member_level, member.clan_id, member.character_id));

    spdlog::debug("[{}] updated member {}-{}", __FUNCTION__, member.clan_id, member.character_id);
}

template<DatabaseTransaction transaction_T>
optional<db_clan_member> clan_members_repository<transaction_T>::get(uint64_t clan_id, uint64_t character_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.clan_id, m.character_id, m.member_level FROM clan_members m WHERE m.clan_id = {} AND m.character_id = {}" , clan_id, character_id));

    if(result.empty()) {
        spdlog::error("[{}] found no member by clan_id {} character_id {}", __FUNCTION__, clan_id, character_id);
        return {};
    }

    auto ret = make_optional<db_clan_member>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}), result[0][2].as(uint16_t{}));

    spdlog::trace("[{}] found member by clan_id {} character_id {}", __FUNCTION__, clan_id, character_id);

    return ret;
}

template<DatabaseTransaction transaction_T>
vector<db_clan_member> clan_members_repository<transaction_T>::get_by_clan_id(uint64_t clan_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.clan_id, m.character_id, m.member_level FROM clan_members m WHERE m.clan_id = {}", clan_id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_clan_member> members;
    members.reserve(result.size());

    for(auto const & res : result) {
        members.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}), res[2].as(uint16_t{}));
    }

    return members;
}
