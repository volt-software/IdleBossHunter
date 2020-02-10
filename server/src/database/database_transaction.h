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

#pragma once

#include <memory>
#include <string>
#include <pqxx/pqxx>

using namespace std;

namespace ibh {
    template<typename T>
    concept DatabaseTransaction = requires(T a, string const& a2) {
        { a.execute(a2) } -> pqxx::result;
        { a.escape(a2) } -> string;
        { a.commit() } -> void;
    };

    class database_pool;
    class database_transaction;

    class database_subtransaction {
    public:
        explicit database_subtransaction(pqxx::work &transaction, string const &name) noexcept;

        pqxx::result execute(string const & query);
        [[nodiscard]] string escape(string const & element);
        void commit();
    private:

        pqxx::subtransaction _subtransaction;
    };

    class database_transaction {
    public:
        explicit database_transaction(database_pool *pool, uint32_t connection_id, shared_ptr<pqxx::connection> connection) noexcept;

        ~database_transaction();
        database_transaction(database_transaction const &o) = delete;
        database_transaction(database_transaction &&o) = delete;
        database_transaction& operator=(database_transaction const &o) = delete;

        [[nodiscard]] unique_ptr<database_subtransaction> create_subtransaction(string const &name = string{});
        pqxx::result execute(string const & query);
        [[nodiscard]] string escape(string const & element);
        void commit();

    private:
        database_pool *_pool;
        uint32_t _connection_id;
        pqxx::work _transaction;
    };
}
