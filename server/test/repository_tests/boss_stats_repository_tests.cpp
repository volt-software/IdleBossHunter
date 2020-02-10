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
#include "repositories/boss_stats_repository.h"
#include "repositories/bosses_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("boss stats repository tests") {
    boss_stats_repository<database_transaction> stat_repo{};
    bosses_repository<database_transaction> bosses_repo{};

    SECTION("boss stats inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_boss boss{0, "boss", {}};
        bosses_repo.insert(boss, transaction);
        REQUIRE(boss.id > 0);
        db_boss_stat stat{0, boss.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->boss_id == stat.boss_id);
        REQUIRE(stat2->name == stat.name);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "update stats" ) {
        auto transaction = db_pool->create_transaction();
        db_boss boss{0, "boss", {}};
        bosses_repo.insert(boss, transaction);
        REQUIRE(boss.id > 0);
        db_boss_stat stat{0, boss.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        stat.value = 12;
        stat_repo.update(stat, transaction);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->boss_id == stat.boss_id);
        REQUIRE(stat2->name == stat.name);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "get all for character stats" ) {
        auto transaction = db_pool->create_transaction();
        db_boss boss{0, "boss", {}};
        bosses_repo.insert(boss, transaction);
        REQUIRE(boss.id > 0);
        db_boss_stat stat{0, boss.id, "test", 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);
        db_boss_stat stat2{0, boss.id, "test2", 20};
        stat_repo.insert(stat2, transaction);
        REQUIRE(stat2.id > 0);

        auto stats = stat_repo.get_by_boss_id(boss.id, transaction);
        REQUIRE(stats.size() == 2);
        REQUIRE(stats[0].id == stat.id);
        REQUIRE(stats[0].boss_id == stat.boss_id);
        REQUIRE(stats[0].name == stat.name);
        REQUIRE(stats[0].value == stat.value);
        REQUIRE(stats[1].id == stat2.id);
        REQUIRE(stats[1].boss_id == stat2.boss_id);
        REQUIRE(stats[1].name == stat2.name);
        REQUIRE(stats[1].value == stat2.value);
    }
}

#endif
