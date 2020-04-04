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

using namespace std;
using namespace ibh;

TEST_CASE("companies repository tests") {
    companies_repository<database_transaction> companies_repo{};

    SECTION("company inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company"};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);

        auto company2 = companies_repo.get(company.id, transaction);
        REQUIRE(company2->id == company.id);
        REQUIRE(company2->name == company.name);
    }

    SECTION( "update company" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company"};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);

        company.name = "notcompany";
        companies_repo.update(company, transaction);

        auto company2 = companies_repo.get(company.id, transaction);
        REQUIRE(company2->id == company.id);
        REQUIRE(company2->name == company.name);
    }

    SECTION( "remove company" ) {
        auto transaction = db_pool->create_transaction();
        db_company company{0, "company"};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);

        companies_repo.remove(company, transaction);

        auto company2 = companies_repo.get(company.id, transaction);
        REQUIRE(!company2);
    }

    SECTION( "get all companies" ) {
        auto transaction = db_pool->create_transaction();
        auto companies_existing = companies_repo.get_all(transaction);

        db_company company{0, "company"};
        companies_repo.insert(company, transaction);
        REQUIRE(company.id > 0);

        auto companies = companies_repo.get_all(transaction);
        REQUIRE(companies.size() == companies_existing.size() + 1);
    }
}

#endif
