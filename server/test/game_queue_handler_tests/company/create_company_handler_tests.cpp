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

#include <catch2/catch.hpp>
#include "../../test_helpers/startup_helper.h"
#include <game_queue_message_handlers/company/create_company_handler.h>
#include <ecs/components.h>
#include <repositories/companies_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/users_repository.h>
#include <messages/company/create_company_response.h>
#include "../game_queue_helpers.h"

using namespace std;
using namespace ibh;

TEST_CASE("create company handler tests") {
    SECTION( "character creates company" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        create_company_message msg(1, "company_name", 2);
        companies_repository<database_transaction> company_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();
        auto existing_companies = company_repo.get_all(transaction);

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = player.id;
            pc.connection_id = 1;
            pc.stats.emplace(stat_gold_id, 10'001);
            registry.assign<pc_component>(entt, move(pc));
        }

        auto ret = handle_create_company(&msg, registry, q, transaction);
        REQUIRE(ret == true);

        test_outmsg<create_company_response>(q, true);

        auto &pc = registry.get<pc_component>(entt);
        auto gold_it = pc.stats.find(stat_gold_id);
        REQUIRE(gold_it != end(pc.stats));
        REQUIRE(gold_it->second == 1);
        auto current_companies = company_repo.get_all(transaction);
        REQUIRE(current_companies.size() == existing_companies.size() + 1);
    }

    SECTION( "not enough gold" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        create_company_message msg(1, "company_name", 2);
        companies_repository<database_transaction> company_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();
        auto existing_companies = company_repo.get_all(transaction);

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = player.id;
            pc.connection_id = 1;
            pc.stats.emplace(stat_gold_id, 9'999);
            registry.assign<pc_component>(entt, move(pc));
        }

        auto ret = handle_create_company(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<create_company_response>(q, false);

        auto &pc = registry.get<pc_component>(entt);
        auto gold_it = pc.stats.find(stat_gold_id);
        REQUIRE(gold_it != end(pc.stats));
        REQUIRE(gold_it->second == 9'999);
        auto current_companies = company_repo.get_all(transaction);
        REQUIRE(current_companies.size() == existing_companies.size());
    }

    SECTION( "already exists" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        create_company_message msg(1, "company_name_exists", 2);
        companies_repository<database_transaction> company_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();
        db_company new_company{};
        new_company.name = msg.company_name;
        REQUIRE(company_repo.insert(new_company, transaction) == true);
        auto existing_companies = company_repo.get_all(transaction);

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character player{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(player, transaction);
        REQUIRE(player.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = player.id;
            pc.connection_id = 1;
            pc.stats.emplace(stat_gold_id, 10'001);
            registry.assign<pc_component>(entt, move(pc));
        }

        auto ret = handle_create_company(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<create_company_response>(q, false);

        auto &pc = registry.get<pc_component>(entt);
        auto gold_it = pc.stats.find(stat_gold_id);
        REQUIRE(gold_it != end(pc.stats));
        REQUIRE(gold_it->second == 10'001);
        auto current_companies = company_repo.get_all(transaction);
        REQUIRE(current_companies.size() == existing_companies.size());
    }
}
