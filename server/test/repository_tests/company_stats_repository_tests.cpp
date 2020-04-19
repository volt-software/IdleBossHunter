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
#include "repositories/company_stats_repository.h"
#include "repositories/companies_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("company stats repository tests") {
    company_stats_repository<database_transaction> stat_repo{};
    companies_repository<database_transaction> companies_repo{};

    SECTION("company stats inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company", 0, 2};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_stat stat{0, company.id, 121, 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->company_id == stat.company_id);
        REQUIRE(stat2->stat_id == stat.stat_id);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "update stats" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company", 0, 2};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_stat stat{0, company.id, 122, 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);

        stat.value = 12;
        stat_repo.update(stat, transaction);

        auto stat2 = stat_repo.get(stat.id, transaction);
        REQUIRE(stat2->id == stat.id);
        REQUIRE(stat2->company_id == stat.company_id);
        REQUIRE(stat2->stat_id == stat.stat_id);
        REQUIRE(stat2->value == stat.value);
    }

    SECTION( "get one for company stats" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company", 0, 2};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_stat stat{0, company.id, 123, 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);
        db_company_stat stat2{0, company.id, 124, 20};
        stat_repo.insert(stat2, transaction);
        REQUIRE(stat2.id > 0);

        auto retrieved_stat = stat_repo.get_by_stat(company.id, stat.stat_id, transaction);
        REQUIRE(retrieved_stat);
        REQUIRE(retrieved_stat->id == stat.id);
        REQUIRE(retrieved_stat->company_id == stat.company_id);
        REQUIRE(retrieved_stat->stat_id == stat.stat_id);
        REQUIRE(retrieved_stat->value == stat.value);
    }

    SECTION( "get all for company stats" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company", 0, 2};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);
        db_company_stat stat{0, company.id, 125, 2};
        stat_repo.insert(stat, transaction);
        REQUIRE(stat.id > 0);
        db_company_stat stat2{0, company.id, 126, 20};
        stat_repo.insert(stat2, transaction);
        REQUIRE(stat2.id > 0);

        auto stats = stat_repo.get_by_company_id(company.id, transaction);
        REQUIRE(stats.size() == 2);
        REQUIRE(stats[0].id == stat.id);
        REQUIRE(stats[0].company_id == stat.company_id);
        REQUIRE(stats[0].stat_id == stat.stat_id);
        REQUIRE(stats[0].value == stat.value);
        REQUIRE(stats[1].id == stat2.id);
        REQUIRE(stats[1].company_id == stat2.company_id);
        REQUIRE(stats[1].stat_id == stat2.stat_id);
        REQUIRE(stats[1].value == stat2.value);
    }
}

#endif
