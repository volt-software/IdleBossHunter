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

#include "database_transaction.h"
#include "database_pool.h"
#include <spdlog/spdlog.h>

using namespace std;
using namespace lotr;

database_transaction::database_transaction(database_pool *pool, uint32_t connection_id, shared_ptr<pqxx::connection> connection) noexcept
        : _pool(pool), _connection_id(connection_id), _transaction(*connection) {

}

database_transaction::~database_transaction() {
    _pool->release_connection(_connection_id);
}

pqxx::result database_transaction::execute(string const &query) {
    spdlog::trace("[database_transaction] executing query {}", query);
    return _transaction.exec(query);
}

string database_transaction::escape(string const &element) {
    return _transaction.esc(element);
}

void database_transaction::commit() {
    _transaction.commit();
}
