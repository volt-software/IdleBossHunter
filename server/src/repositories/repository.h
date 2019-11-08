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

namespace lotr {

    template<typename T, typename DatabaseType, DatabaseTransaction DBT>
    concept bool Repository = requires(T a, DatabaseType& ref_type, unique_ptr<DBT> transaction, int id) {
        { a.create_transaction() } -> unique_ptr<DatabaseTransaction>;
        { a.insert_if_not_exists(ref_type, transaction) } -> bool;
        { a.update(ref_type, transaction) } -> void;
        { a.get(id, transaction) } -> optional<DatabaseType>;
    };
}

#include "users_repository.h"
