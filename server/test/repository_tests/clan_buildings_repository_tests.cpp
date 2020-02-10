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

#ifndef EXCLUDE_PSQL_TESTS

#include <catch2/catch.hpp>
#include "../test_helpers/startup_helper.h"
#include "repositories/clans_repository.h"
#include "repositories/clan_buildings_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("clan buildings repository tests") {
    clans_repository<database_transaction> clans_repo{};
    clan_buildings_repository<database_transaction> clan_buildings_repo{};

    SECTION("clan building inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_clan clan{0, "clan", {}, {}};
        clans_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);

        db_clan_building building{0, clan.id, "building"};
        clan_buildings_repo.insert(building, transaction);
        REQUIRE(building.id > 0);

        auto building2 = clan_buildings_repo.get(building.id, transaction);
        REQUIRE(building2->id == building.id);
        REQUIRE(building2->clan_id == building.clan_id);
        REQUIRE(building2->name == building.name);
    }

    SECTION( "update clan building" ) {
        auto transaction = db_pool->create_transaction();
        db_clan clan{0, "clan", {}, {}};
        clans_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);

        db_clan_building building{0, clan.id, "building"};
        clan_buildings_repo.insert(building, transaction);
        REQUIRE(building.id > 0);

        building.name = "notclan";
        clan_buildings_repo.update(building, transaction);

        auto building2 = clan_buildings_repo.get(building.id, transaction);
        REQUIRE(building2->id == building.id);
        REQUIRE(building2->clan_id == building.clan_id);
        REQUIRE(building2->name == building.name);
    }
}

#endif
