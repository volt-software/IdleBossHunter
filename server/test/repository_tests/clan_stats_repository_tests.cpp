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
#include "repositories/clan_stats_repository.h"
#include "repositories/clans_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("clan stats repository tests") {
    clan_stats_repository<database_pool, database_transaction> stat_repo(db_pool);
    clans_repository<database_pool, database_transaction> clans_repo(db_pool);

    SECTION("clan stats inserted correctly" ) {
        auto transaction = stat_repo.create_transaction();
        db_clan clan{0, "clan", {}, {}};
        clans_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_stat stat{0, clan.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->clan_id == stat.clan_id);
        REQUIRE(stat2->name == stat.name);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "update stats" ) {
        auto transaction = stat_repo.create_transaction();
        db_clan clan{0, "clan", {}, {}};
        clans_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_stat stat{0, clan.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        stat.value = 12;
        stat_repo.update(stat, transaction);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->clan_id == stat.clan_id);
        REQUIRE(stat2->name == stat.name);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "get all for character stats" ) {
        auto transaction = stat_repo.create_transaction();
        db_clan clan{0, "clan", {}, {}};
        clans_repo.insert(clan, transaction);
        REQUIRE(clan.id > 0);
        db_clan_stat stat{0, clan.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);
        db_clan_stat stat2{0, clan.id, "test2", 20};
        stat_repo.insert(stat2, transaction);
        REQUIRE(stat2.id > 0);

        auto stats = stat_repo.get_by_clan_id(clan.id, transaction);
        REQUIRE(stats.size() == 2);
        REQUIRE(stats[0].id == stat.id);
        REQUIRE(stats[0].clan_id == stat.clan_id);
        REQUIRE(stats[0].name == stat.name);
        REQUIRE(stats[0].value == stat.value);
        REQUIRE(stats[1].id == stat2.id);
        REQUIRE(stats[1].clan_id == stat2.clan_id);
        REQUIRE(stats[1].name == stat2.name);
        REQUIRE(stats[1].value == stat2.value);
    }
}

#endif
