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

#include "company_members_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;

template class ibh::company_members_repository<database_transaction>;
template class ibh::company_members_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
bool company_members_repository<transaction_T>::insert(db_company_member const &member, unique_ptr<transaction_T> const &transaction) const {
    try {
        transaction->execute(fmt::format("INSERT INTO company_members (company_id, character_id, member_level, wage) VALUES ({}, {}, {}, {})",
                            member.company_id, member.character_id, member.member_level, member.wage));
    } catch(pqxx::unique_violation const &e) {
        return false;
    }

    spdlog::trace("[{}] inserted member {}-{}", __FUNCTION__, member.company_id, member.character_id);
    return true;
}

template<DatabaseTransaction transaction_T>
void company_members_repository<transaction_T>::update(db_company_member const &member, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE company_members SET member_level = {}, wage = {} WHERE company_id = {} AND character_id = {}", member.member_level, member.wage, member.company_id, member.character_id));

    spdlog::trace("[{}] updated member {}-{}", __FUNCTION__, member.company_id, member.character_id);
}

template<DatabaseTransaction transaction_T>
void company_members_repository<transaction_T>::remove(db_company_member const &member, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("DELETE FROM company_members WHERE company_id = {} AND character_id = {}", member.company_id, member.character_id));

    spdlog::trace("[{}] deleted member {}-{}", __FUNCTION__, member.company_id, member.character_id);
}

template<DatabaseTransaction transaction_T>
optional<db_company_member> company_members_repository<transaction_T>::get(uint64_t company_id, uint64_t character_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.company_id, m.character_id, m.member_level, m.wage FROM company_members m WHERE m.company_id = {} AND m.character_id = {}" , company_id, character_id));

    if(result.empty()) {
        spdlog::trace("[{}] found no member by company_id {} character_id {}", __FUNCTION__, company_id, character_id);
        return {};
    }

    auto ret = make_optional<db_company_member>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}), result[0][2].as(uint16_t{}), result[0][3].as(uint64_t{}));

    spdlog::trace("[{}] found member by company_id {} character_id {}", __FUNCTION__, company_id, character_id);

    return ret;
}

template<DatabaseTransaction transaction_T>
vector<db_company_member> company_members_repository<transaction_T>::get_by_company_id(uint64_t company_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.company_id, m.character_id, m.member_level, m.wage FROM company_members m WHERE m.company_id = {}", company_id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_company_member> members;
    members.reserve(result.size());

    for(auto const & res : result) {
        members.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}), res[2].as(uint16_t{}), res[3].as(uint16_t{}));
    }

    return members;
}

template<DatabaseTransaction transaction_T>
optional<db_company_member> company_members_repository<transaction_T>::get_by_character_id(uint64_t character_id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT m.company_id, m.character_id, m.member_level, m.wage FROM company_members m WHERE m.character_id = {} LIMIT 1", character_id));

    if(result.empty()) {
        spdlog::trace("[{}] found no member character_id {}", __FUNCTION__, character_id);
        return {};
    }

    auto ret = make_optional<db_company_member>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}), result[0][2].as(uint16_t{}), result[0][3].as(uint64_t{}));

    spdlog::trace("[{}] found member character_id {}", __FUNCTION__, character_id);

    return ret;
}
