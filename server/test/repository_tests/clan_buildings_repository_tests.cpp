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
#include "repositories/companies_repository.h"
#include "repositories/company_buildings_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("company buildings repository tests") {
    companies_repository<database_transaction> companies_repo{};
    company_buildings_repository<database_transaction> company_buildings_repo{};

    SECTION("company building inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company"};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);

        db_company_building building{0, company.id, "building"};
        company_buildings_repo.insert(building, transaction);
        REQUIRE(building.id > 0);

        auto building2 = company_buildings_repo.get(building.id, transaction);
        REQUIRE(building2->id == building.id);
        REQUIRE(building2->company_id == building.company_id);
        REQUIRE(building2->name == building.name);
    }

    SECTION( "update company building" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company"};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);

        db_company_building building{0, company.id, "building"};
        company_buildings_repo.insert(building, transaction);
        REQUIRE(building.id > 0);

        building.name = "notcompany";
        company_buildings_repo.update(building, transaction);

        auto building2 = company_buildings_repo.get(building.id, transaction);
        REQUIRE(building2->id == building.id);
        REQUIRE(building2->company_id == building.company_id);
        REQUIRE(building2->name == building.name);
    }
}

#endif
