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

#include "items_repository.h"
#include <spdlog/spdlog.h>

using namespace lotr;

template class lotr::items_repository<database_pool, database_transaction>;

template<typename pool_T, typename transaction_T>
items_repository<pool_T, transaction_T>::items_repository(shared_ptr<pool_T> database_pool) : _database_pool(move(database_pool)) {

}

template<typename pool_T, typename transaction_T>
unique_ptr<transaction_T> items_repository<pool_T, transaction_T>::create_transaction() {
    return _database_pool->create_transaction();
}

template<typename pool_T, typename transaction_T>
bool items_repository<pool_T, transaction_T>::insert(db_item &item, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format(
            "INSERT INTO items (character_id, item_name, item_slot, equip_slot) VALUES ({}, '{}', '{}', '{}') "
            "RETURNING xmax, id",
            item.character_id, transaction->escape(item.name), transaction->escape(item.slot), transaction->escape(item.equip_slot)));

    if(result.empty()) {
        spdlog::error("[{}] contains {} entries", __FUNCTION__, result.size());
        return false;
    }

    item.id = result[0][1].as(uint64_t{});

    if(result[0][0].as(uint64_t{}) == 0) {
        spdlog::debug("[{}] inserted db_item {}", __FUNCTION__, item.id);
        return true;
    }

    spdlog::debug("[{}] could not insert db_item {} {}", __FUNCTION__, item.id, item.name);
    return false;
}

template<typename pool_T, typename transaction_T>
void items_repository<pool_T, transaction_T>::update_item(db_item const &item, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("UPDATE items SET character_id = {}, equip_slot = '{}' WHERE id = {}", item.character_id, transaction->escape(item.equip_slot), item.id));

    spdlog::debug("[{}] updated db_item {}", __FUNCTION__, item.id);
}

template<typename pool_T, typename transaction_T>
void items_repository<pool_T, transaction_T>::delete_item(db_item const &item, unique_ptr<transaction_T> const &transaction) const {
    transaction->execute(fmt::format("DELETE FROM items WHERE id = {}", item.id));

    spdlog::debug("[{}] deleted db_item {}", __FUNCTION__, item.id);
}

template<typename pool_T, typename transaction_T>
optional<db_item> items_repository<pool_T, transaction_T>::get_item(uint64_t id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT p.id, p.character_id, p.item_name, p.item_slot, p.equip_slot FROM items p WHERE id = {}", id));

    if(result.empty()) {
        spdlog::debug("[{}] found no db_item by id {}", __FUNCTION__, id);
        return {};
    }

    auto ret = make_optional<db_item>(result[0][0].as(uint64_t{}), result[0][1].as(uint64_t{}),
                                      result[0][2].as(string{}),
                                      result[0][3].as(string{}), result[0][4].as(string{}));

    spdlog::debug("[{}] found db_item by id {}", __FUNCTION__, id);

    return ret;
}

template<typename pool_T, typename transaction_T>
vector<db_item> items_repository<pool_T, transaction_T>::get_by_character_id(uint64_t character_id, unique_ptr<transaction_T> const &transaction) const {
    pqxx::result result = transaction->execute(fmt::format("SELECT p.id, p.character_id, p.item_name, p.item_slot, p.equip_slot FROM items p WHERE character_id = {}", character_id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    vector<db_item> items;
    items.reserve(result.size());

    for(auto const & res : result) {
        items.emplace_back(res[0].as(uint64_t{}), res[1].as(uint64_t{}),
                           res[2].as(string{}),
                           res[3].as(string{}), res[4].as(string{}));
    }

    return items;
}
