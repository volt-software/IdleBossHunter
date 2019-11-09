/*
    IdleBossHunter
    Copyright (C) 2019  Michael de Lang

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

#ifndef EXCLUDE_PSQL_TESTS

#include <catch2/catch.hpp>
#include "../test_helpers/startup_helper.h"
#include "repositories/bosses_repository.h"

using namespace std;
using namespace lotr;

TEST_CASE("bosses repository tests") {
    bosses_repository<database_pool, database_transaction> bosses_repo(db_pool);

    SECTION("boss inserted correctly" ) {
        auto transaction = bosses_repo.create_transaction();
        db_boss boss{0, "boss", {}};
        bosses_repo.insert(boss, transaction);
        REQUIRE(boss.id > 0);

        auto boss2 = bosses_repo.get(boss.id, transaction);
        REQUIRE(boss2->id == boss.id);
        REQUIRE(boss2->name == boss.name);
    }

    SECTION( "update boss" ) {
        auto transaction = bosses_repo.create_transaction();
        db_boss boss{0, "boss", {}};
        bosses_repo.insert(boss, transaction);
        REQUIRE(boss.id > 0);

        boss.name = "notboss";
        bosses_repo.update(boss, transaction);

        auto boss2 = bosses_repo.get(boss.id, transaction);
        REQUIRE(boss2->id == boss.id);
        REQUIRE(boss2->name == boss.name);
    }
}

#endif
