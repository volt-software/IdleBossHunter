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

#include "characters_repository.h"
#include <spdlog/spdlog.h>

using namespace ibh;

template class ibh::characters_repository<database_transaction>;
template class ibh::characters_repository<database_subtransaction>;

template<DatabaseTransaction transaction_T>
bool characters_repository<transaction_T>::insert(db_character &character, unique_ptr<transaction_T> const &transaction) const {

    auto result = transaction->execute(fmt::format(
            "INSERT INTO characters (user_id, slot, level, gold, xp, skill_points, x, y, character_name, race, class, map) VALUES ({}, {}, {}, {}, {}, {}, {}, {}, '{}', '{}', '{}', '{}') "
            "ON CONFLICT (user_id, slot) DO NOTHING RETURNING xmax, id",
            character.user_id, character.slot, character.level, character.gold, character.xp, character.skill_points, character.x, character.y, transaction->escape(character.name), transaction->escape(character.race),
            transaction->escape(character._class), transaction->escape(character.map)));

    if(result.empty()) {
        spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());
        return false;
    }

    character.id = result[0][1].as(uint64_t{});

    if(result[0][0].as(uint64_t{}) == 0) {
        spdlog::trace("[{}] inserted db_character {}", __FUNCTION__, character.id);
        return true;
    }

    spdlog::trace("[{}] could not insert db_character {} {}", __FUNCTION__, character.id, character.name);
    return false;
}

template<DatabaseTransaction transaction_T>
bool characters_repository<transaction_T>::insert_or_update_character(db_character &character, unique_ptr<transaction_T> const &transaction) const {

    auto result = transaction->execute(fmt::format(
            "INSERT INTO characters (user_id, slot, level, gold, xp, skill_points, x, y, character_name, race, class, map) VALUES ({}, {}, {}, {}, {}, {}, {}, {}, '{}', '{}', '{}', '{}') "
            "ON CONFLICT (user_id, slot) DO UPDATE SET user_id = {}, level = {}, gold = {}, xp = {}, skill_points = {}, x = {}, y = {}, race = '{}', class = '{}', map = '{}' RETURNING xmax, id",
            character.user_id, character.slot, character.level, character.gold, character.xp, character.skill_points, character.x, character.y, transaction->escape(character.name), transaction->escape(character.race),
            transaction->escape(character._class), transaction->escape(character.map),
            character.user_id, character.level, character.gold, character.xp, character.skill_points, character.x, character.y,
            transaction->escape(character.race), transaction->escape(character._class), transaction->escape(character.map)));

    if(result.empty()) {
        spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());
        return false;
    }

    character.id = result[0][1].as(uint64_t{});

    if(result[0][0].as(uint64_t{}) == 0) {
        spdlog::trace("[{}] inserted db_character {}", __FUNCTION__, character.id);
        return true;
    }

    spdlog::trace("[{}] updated db_character {}", __FUNCTION__, character.id);
    return false;
}

template<DatabaseTransaction transaction_T>
void characters_repository<transaction_T>::update_character(db_character const &character, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE characters SET user_id = {}, level = {}, gold = {}, xp = {}, skill_points = {}, x = {}, y = {}, race = '{}', class = '{}', map = '{}' WHERE id = {}",
            character.user_id, character.level, character.gold, character.xp, character.skill_points, character.x, character.y,
            transaction->escape(character.race), transaction->escape(character._class), transaction->escape(character.map), character.id));

    spdlog::trace("[{}] updated db_character {}", __FUNCTION__, character.id);
}

template<DatabaseTransaction transaction_T>
void characters_repository<transaction_T>::delete_character_by_slot(uint32_t slot, uint64_t user_id, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("DELETE FROM character_stats s USING characters c WHERE s.character_id = c.id AND c.slot = {} AND c.user_id = {}", slot, user_id));
    transaction->execute(fmt::format("DELETE FROM characters WHERE slot = {} AND user_id = {}", slot, user_id));

    spdlog::trace("[{}] deleted db_character {} for user {}", __FUNCTION__, slot, user_id);
}

template<DatabaseTransaction transaction_T>
optional<db_character> characters_repository<transaction_T>::get_character(string const &name, uint64_t user_id,
                                                                                   unique_ptr<transaction_T> const &transaction) const {
    pqxx::result result = transaction->execute(fmt::format("SELECT p.id, p.user_id, p.slot, p.level, p.gold, p.xp, p.skill_points, p.x, p.y, p.character_name, p.race, p.class, p.map FROM characters p WHERE character_name = '{}' and p.user_id = {}", transaction->escape(name), user_id));

    if(result.empty()) {
        spdlog::trace("[{}] found no db_character by name {}", __FUNCTION__, name);
        return {};
    }

    auto ret = make_optional<db_character>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                           result[0][2].as(uint32_t{}), result[0][3].as(uint64_t{}),
                                           result[0][4].as(uint64_t{}), result[0][5].as(uint64_t{}),
                                           result[0][6].as(uint64_t{}), result[0][7].as(uint32_t{}),
                                           result[0][8].as(uint32_t{}),
                                           result[0][9].as(string{}), result[0][10].as(string{}),
                                           result[0][11].as(string{}),
                                           result[0][12].as(string{}), vector<db_character_stat>{}, vector<db_item>{});

    spdlog::trace("[{}] found db_character by name {} with id {}", __FUNCTION__, name, ret->id);

    return ret;
}

template<DatabaseTransaction transaction_T>
optional<db_character> characters_repository<transaction_T>::get_character(uint64_t id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT p.id, p.user_id, p.slot, p.level, p.gold, p.xp, p.skill_points, p.x, p.y, p.character_name, p.race, p.class, p.map FROM characters p WHERE id = {}", id));

    if(result.empty()) {
        spdlog::trace("[{}] found no db_character by id {}", __FUNCTION__, id);
        return {};
    }

    auto ret = make_optional<db_character>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                           result[0][2].as(uint32_t{}), result[0][3].as(uint64_t{}),
                                           result[0][4].as(uint64_t{}), result[0][5].as(uint64_t{}),
                                           result[0][6].as(uint64_t{}), result[0][7].as(uint32_t{}),
                                           result[0][8].as(uint32_t{}),
                                           result[0][9].as(string{}), result[0][10].as(string{}),
                                           result[0][11].as(string{}),
                                           result[0][12].as(string{}), vector<db_character_stat>{}, vector<db_item>{});

    spdlog::trace("[{}] found db_character by id {}", __FUNCTION__, id);

    return ret;
}

template<DatabaseTransaction transaction_T>
optional<db_character> characters_repository<transaction_T>::get_character_by_slot(uint32_t slot, uint64_t user_id,
                                                                                   unique_ptr<transaction_T> const &transaction) const {
    pqxx::result result = transaction->execute(fmt::format("SELECT p.id, p.user_id, p.slot, p.level, p.gold, p.xp, p.skill_points, p.x, p.y, p.character_name, p.race, p.class, p.map FROM characters p WHERE slot = {} and user_id = {}", slot, user_id));


    if(result.empty()) {
        spdlog::trace("[{}] found no db_character by slot {}", __FUNCTION__, slot);
        return {};
    }

    auto ret = make_optional<db_character>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                           result[0][2].as(uint32_t{}), result[0][3].as(uint64_t{}),
                                           result[0][4].as(uint64_t{}), result[0][5].as(uint64_t{}),
                                           result[0][6].as(uint64_t{}), result[0][7].as(uint32_t{}),
                                           result[0][8].as(uint32_t{}),
                                           result[0][9].as(string{}), result[0][10].as(string{}),
                                           result[0][11].as(string{}),
                                           result[0][12].as(string{}), vector<db_character_stat>{}, vector<db_item>{});

    spdlog::trace("[{}] found db_character by slot {} for user {}", __FUNCTION__, slot, user_id);

    return ret;
}

template<DatabaseTransaction transaction_T>
vector<db_character> characters_repository<transaction_T>::get_by_user_id(uint64_t user_id,
                                                                                  unique_ptr<transaction_T> const &transaction) const {
    pqxx::result result = transaction->execute(fmt::format("SELECT p.id, p.user_id, p.slot, p.level, p.gold, p.xp, p.skill_points, p.x, p.y, p.character_name, p.race, p.class, p.map FROM characters p WHERE user_id = {}", user_id));

    spdlog::trace("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_character> characters;
    characters.reserve(result.size());

    for(auto const & res : result) {
        characters.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}), res[2].as(uint32_t{}), res[3].as(uint64_t{}),
                                res[4].as(uint64_t{}),res[5].as(uint64_t{}), res[6].as(uint64_t{}), res[7].as(uint32_t{}),
                                res[8].as(uint32_t{}), res[9].as(string{}), res[10].as(string{}), res[11].as(string{}),
                                res[12].as(string{}), vector<db_character_stat>{}, vector<db_item>{});
    }

    return characters;
}
