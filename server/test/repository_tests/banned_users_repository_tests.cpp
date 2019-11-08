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
#include "repositories/users_repository.h"
#include "repositories/banned_users_repository.h"

using namespace std;
using namespace lotr;

TEST_CASE("banned users repository tests") {
    users_repository<database_pool, database_transaction> user_repo(db_pool);
    banned_users_repository<database_pool, database_transaction> banned_user_repo(db_pool);

    SECTION( "banned user inserted correctly" ) {
        auto transaction = user_repo.create_transaction();
        user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id != 0);

        auto now = chrono::system_clock::now();
        banned_user busr{0, "ip", usr, now};
        
        auto inserted = banned_user_repo.insert_if_not_exists(busr, transaction);
        REQUIRE(inserted);
        REQUIRE(busr.id != 0);

        auto busr2 = banned_user_repo.get(busr.id, transaction);
        REQUIRE(busr2->id == busr.id);
        REQUIRE(busr2->ip == busr.ip);
        REQUIRE(busr2->_user->id == usr.id);
        REQUIRE(busr2->until == busr.until);
    }

    SECTION( "update banned user" ) {
        auto transaction = user_repo.create_transaction();
        user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id != 0);

        user usr2{0, "user2", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr2, transaction);
        REQUIRE(usr2.id != 0);

        auto now = chrono::system_clock::now();
        banned_user busr{0, "ip", usr, now};

        auto inserted = banned_user_repo.insert_if_not_exists(busr, transaction);
        REQUIRE(inserted);
        REQUIRE(busr.id != 0);

        now += 2us;
        busr.ip = "ip2";
        busr._user = usr2;
        busr.until = now;
        banned_user_repo.update(busr, transaction);

        auto busr2 = banned_user_repo.get(busr.id, transaction);
        REQUIRE(busr2->id == busr.id);
        REQUIRE(busr2->ip == busr.ip);
        REQUIRE(busr2->_user->id == usr2.id);
        REQUIRE(busr2->until == busr.until);
    }

    SECTION( "banned user is banned" ) {
        auto transaction = user_repo.create_transaction();
        user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id != 0);

        auto now = chrono::system_clock::now() + 200s;
        banned_user busr{0, "ip", usr, now};

        auto inserted = banned_user_repo.insert_if_not_exists(busr, transaction);
        REQUIRE(inserted);
        REQUIRE(busr.id != 0);

        auto busr2 = banned_user_repo.is_username_or_ip_banned("wrong_ip", "wrong_user", transaction);
        REQUIRE(!busr2);

        busr2 = banned_user_repo.is_username_or_ip_banned("user", {}, transaction);
        REQUIRE(busr2);
        REQUIRE(busr2->id == busr.id);

        busr2 = banned_user_repo.is_username_or_ip_banned({}, "ip", transaction);
        REQUIRE(busr2);
        REQUIRE(busr2->id == busr.id);

        busr2 = banned_user_repo.is_username_or_ip_banned("user", "ip", transaction);
        REQUIRE(busr2);
        REQUIRE(busr2->id == busr.id);
    }
}

#endif
