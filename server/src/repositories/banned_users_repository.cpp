/*
    Realm of Aesir
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

#include "banned_users_repository.h"
#include <spdlog/spdlog.h>

using namespace lotr;
using namespace chrono;

template class lotr::banned_users_repository<database_pool, database_transaction>;

template<typename pool_T, typename transaction_T>
banned_users_repository<pool_T, transaction_T>::banned_users_repository(shared_ptr<pool_T> database_pool) : _database_pool(move(database_pool)) {

}

template<typename pool_T, typename transaction_T>
unique_ptr<transaction_T> banned_users_repository<pool_T, transaction_T>::create_transaction() {
    return _database_pool->create_transaction();
}

template<typename pool_T, typename transaction_T>
bool banned_users_repository<pool_T, transaction_T>::insert_if_not_exists(banned_user &usr, unique_ptr<transaction_T> const &transaction) const {
    string ip = !usr.ip.empty() ? "'" + transaction->escape(usr.ip) + "'" : "NULL";
    string user_id = usr._user ? to_string(usr._user->id) : "NULL";
    string until = usr.until ? to_string(usr.until->time_since_epoch().count()) : "NULL";

    auto result = transaction->execute(fmt::format("INSERT INTO banned_users (ip, user_id, until) VALUES ({}, {}, {}) RETURNING id", ip, user_id, until));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        //already exists
        return false;
    }

    usr.id = result[0][0].as(uint64_t{});

    return true;
}

template<typename pool_T, typename transaction_T>
void banned_users_repository<pool_T, transaction_T>::update(banned_user const &usr, unique_ptr<transaction_T> const &transaction) const {
    string ip = !usr.ip.empty() ? "'" + transaction->escape(usr.ip) + "'" : "NULL";
    string user_id = usr._user ? to_string(usr._user->id) : "NULL";
    string until = usr.until ? to_string(usr.until->time_since_epoch().count()) : "NULL";

    auto result = transaction->execute(fmt::format("UPDATE banned_users SET ip = {}, user_id = {}, until = {}", ip, user_id, until));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());
}

template<typename pool_T, typename transaction_T>
optional<banned_user> banned_users_repository<pool_T, transaction_T>::get(int id, unique_ptr<transaction_T> const &transaction) const {
    auto result = transaction->execute(fmt::format("SELECT id, ip, user_id, until FROM banned_users WHERE id = {}", id));

    spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

    if(result.empty()) {
        return {};
    }

    string ip;
    optional<user> _user;
    optional<system_clock::time_point> until;

    if(!result[0]["ip"].is_null()) {
        ip = result[0]["ip"].as(string{});
    }

    if(!result[0]["user_id"].is_null()) {
        _user = make_optional<user>({result[0]["user_id"].as(uint64_t{}), {}, {}, {}, 0, {}, 0, 0});
    }

    if(!result[0]["until"].is_null()) {
        until = system_clock::time_point(nanoseconds(result[0]["until"].as(int64_t{})));
    }

    return make_optional<banned_user>(result[0]["id"].as(uint64_t{}), ip, _user, until);
}

template<typename pool_T, typename transaction_T>
optional<banned_user> banned_users_repository<pool_T, transaction_T>::is_username_or_ip_banned(optional<string> username, optional<string> ip, unique_ptr<transaction_T> const &transaction) const {
    if(!username && !ip) {
        spdlog::error("[{}] called without arguments", __FUNCTION__);
        return {};
    }

    uint64_t usr_id;
    string ip_ret;
    optional<system_clock::time_point> until;
    auto now = system_clock::now().time_since_epoch().count();

    if(username && ip) {
        auto result = transaction->execute(fmt::format("SELECT bu.id as id, bu.ip, until FROM banned_users bu "
                                           "LEFT JOIN users u ON bu.user_id = u.id AND u.username = '{}' "
                                           "WHERE bu.until >= {} AND (u.id IS NOT NULL OR bu.ip = '{}')",
                                           transaction->escape(username.value()), now, transaction->escape(ip.value())));

        spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

        if(result.empty()) {
            return {};
        }

        if(!result[0]["ip"].is_null()) {
            ip_ret = result[0]["ip"].as(string{});
        }

        if(!result[0]["until"].is_null()) {
            until = system_clock::time_point(nanoseconds(result[0]["until"].as(int64_t{})));
        }

        usr_id = result[0]["id"].as(uint64_t{});
    } else if(username) {
        auto result = transaction->execute(fmt::format("SELECT bu.id as id, bu.ip, until FROM banned_users bu "
                                           "LEFT JOIN users u ON bu.user_id = u.id AND u.username = '{}' "
                                           "WHERE bu.until >= {} AND u.id IS NOT NULL", transaction->escape(username.value()), now));

        spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

        if(result.empty()) {
            return {};
        }

        if(!result[0]["ip"].is_null()) {
            ip_ret = result[0]["ip"].as(string{});
        }

        if(!result[0]["until"].is_null()) {
            until = system_clock::time_point(nanoseconds(result[0]["until"].as(int64_t{})));
        }

        usr_id = result[0]["id"].as(uint64_t{});
    } else {
        auto result = transaction->execute(fmt::format("SELECT bu.id as id, bu.ip, until FROM banned_users bu "
                                           "WHERE bu.until >= {} AND bu.ip = '{}'", now, transaction->escape(ip.value())));

        spdlog::debug("[{}] contains {} entries", __FUNCTION__, result.size());

        if(result.empty()) {
            return {};
        }

        if(!result[0]["ip"].is_null()) {
            ip_ret = result[0]["ip"].as(string{});
        }

        if(!result[0]["until"].is_null()) {
            until = system_clock::time_point(nanoseconds(result[0]["until"].as(int64_t{})));
        }

        usr_id = result[0]["id"].as(uint64_t{});
    }

    return make_optional<banned_user>(usr_id, ip_ret, user{}, until);
}
