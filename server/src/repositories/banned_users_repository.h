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

#pragma once

#include <string>
#include <memory>
#include <optional>
#include <database/database_pool.h>
#include <database/database_transaction.h>
#include "models.h"

using namespace std;

namespace lotr {
    template<typename pool_T, typename transaction_T>
    class banned_users_repository {
    public:
        explicit banned_users_repository(shared_ptr<pool_T> database_pool);

        unique_ptr<transaction_T> create_transaction();
        bool insert_if_not_exists(banned_user& usr, unique_ptr<transaction_T> const &transaction) const;
        void update(banned_user const &usr, unique_ptr<transaction_T> const &transaction) const;
        optional<banned_user> get(int id, unique_ptr<transaction_T> const &transaction) const;
        optional<banned_user> is_username_or_ip_banned(optional<string> username, optional<string> ip, unique_ptr<transaction_T> const &transaction) const;
    private:
        shared_ptr<pool_T> _database_pool;
    };
}
