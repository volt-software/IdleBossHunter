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

#include <memory>
#include <string>
#include <pqxx/pqxx>

using namespace std;

namespace lotr {
    /*template<typename T>
    concept bool DatabaseTransaction = requires(T a, string& a2) {
        { a.execute(a2) } -> pqxx::result;
        { a.escape(a2) } -> string;
        { a.commit() } -> void;
    };*/

    class database_pool;

    class database_transaction {
    public:
        explicit database_transaction(database_pool *pool, uint32_t connection_id, shared_ptr<pqxx::connection> connection) noexcept;

        ~database_transaction();

        pqxx::result execute(string const & query);
        string escape(string const & element);
        void commit();

    private:
        database_pool *_pool;
        uint32_t _connection_id;
        pqxx::work _transaction;
    };
}
