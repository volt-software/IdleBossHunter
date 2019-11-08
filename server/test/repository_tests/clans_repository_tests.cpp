/*
    Realm of Aesir
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
#include "repositories/clans_repository.h"

using namespace std;
using namespace lotr;

TEST_CASE("clans repository tests") {
    clans_repository<database_pool, database_transaction> clans_repo(db_pool);

    SECTION("clan inserted correctly" ) {
        auto transaction = clans_repo.create_transaction();
        db_clan clan{0, "clan", {}, {}};
        clans_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);

        auto clan2 = clans_repo.get(clan.id, transaction);
        REQUIRE(clan2->id == clan.id);
        REQUIRE(clan2->name == clan.name);
    }

    SECTION( "update clan" ) {
        auto transaction = clans_repo.create_transaction();
        db_clan clan{0, "clan", {}, {}};
        clans_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);

        clan.name = "notclan";
        clans_repo.update(clan, transaction);

        auto clan2 = clans_repo.get(clan.id, transaction);
        REQUIRE(clan2->id == clan.id);
        REQUIRE(clan2->name == clan.name);
    }
}

#endif
