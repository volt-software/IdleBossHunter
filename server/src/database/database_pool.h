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

#include "database_transaction.h"
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>

using namespace std;

namespace lotr {
    /*template<typename T>
    concept bool DatabasePool = requires(T a, uint32_t id) {
        { a.create_transaction() } -> unique_ptr<DatabaseTransaction>;
        { a.release_connection(id) } -> void;
    };*/

    class database_transaction;

    class database_pool {
    public:
        database_pool() noexcept;
        ~database_pool();

        void create_connections(const string& connection_string, uint32_t min_connections = 5);

        unique_ptr<database_transaction> create_transaction();

        /**
         * Marks connection as available again
         * @param id
         */
        void release_connection(uint32_t id);
    private:
        string _connection_string;
        uint32_t _min_connections;
        vector<tuple<bool, uint32_t, shared_ptr<pqxx::connection>>> _connections;
        mutex _connections_mutex;
    };
}
