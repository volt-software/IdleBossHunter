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
#include "repositories/users_repository.h"
#include "repositories/banned_users_repository.h"

using namespace std;
using namespace ibh;

TEST_CASE("users repository tests") {
    users_repository<database_transaction> user_repo{};
    banned_users_repository<database_transaction> banned_user_repo{};

    SECTION( "user inserted correctly" ) {
        auto transaction = db_pool->create_transaction();
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id != 0);
        
        auto usr2 = user_repo.get(usr.id, transaction);
        REQUIRE(usr2);
        REQUIRE(usr2->id == usr.id);
        REQUIRE(usr2->username == usr.username);
        REQUIRE(usr2->password == usr.password);
        REQUIRE(usr2->email == usr.email);
        REQUIRE(usr2->login_attempts == usr.login_attempts);
        REQUIRE(usr2->is_game_master == usr.is_game_master);
        REQUIRE(usr2->max_characters == usr.max_characters);
        REQUIRE(usr2->verification_code == usr.verification_code);
        
        uint64_t old_id = usr.id;
        user_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id == old_id);
    }
    
    SECTION( "update user" ) {
        auto transaction = db_pool->create_transaction();
        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr, transaction);
        REQUIRE(usr.id != 0);
        
        usr.username = "user2";
        usr.password = "pass2";
        usr.email = "email2";
        usr.login_attempts = 5;
        usr.is_game_master = 6;
        usr.max_characters = 7;
        user_repo.update(usr, transaction);
        
        auto usr2 = user_repo.get(usr.username, transaction);
        REQUIRE(usr2);
        REQUIRE(usr2->id == usr.id);
        REQUIRE(usr2->username == usr.username);
        REQUIRE(usr2->password == usr.password);
        REQUIRE(usr2->email == usr.email);
        REQUIRE(usr2->login_attempts == usr.login_attempts);
        REQUIRE(usr2->is_game_master == usr.is_game_master);
        REQUIRE(usr2->max_characters == usr.max_characters);
    }

    SECTION( "get all users" ) {
        auto transaction = db_pool->create_transaction();
        auto existing_usrs = user_repo.get_all(transaction);

        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        db_user usr2{0, "user2", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr, transaction);
        user_repo.insert_if_not_exists(usr2, transaction);
        REQUIRE(usr.id != 0);
        REQUIRE(usr2.id != 0);

        auto usrs = user_repo.get_all(transaction);
        REQUIRE(usrs.size() == existing_usrs.size() + 2);
    }

    SECTION( "get all users without banned" ) {
        auto transaction = db_pool->create_transaction();
        auto existing_usrs = user_repo.get_all(transaction);

        db_user usr{0, "user", "pass", "email", 0, "code", 0, 0};
        db_user usr2{0, "user2", "pass", "email", 0, "code", 0, 0};
        user_repo.insert_if_not_exists(usr, transaction);
        user_repo.insert_if_not_exists(usr2, transaction);
        REQUIRE(usr.id != 0);
        REQUIRE(usr2.id != 0);
        db_banned_user busr{0, "", usr, {}};
        banned_user_repo.insert_if_not_exists(busr, transaction);
        REQUIRE(busr.id != 0);

        auto usrs = user_repo.get_all(transaction);
        REQUIRE(usrs.size() == existing_usrs.size() + 1);
    }
}

#endif
