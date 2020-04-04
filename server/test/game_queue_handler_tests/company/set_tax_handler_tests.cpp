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
#include "../game_queue_helpers.h"
#include <game_queue_message_handlers/company/set_tax_handler.h>
#include <ecs/components.h>
#include <repositories/companies_repository.h>
#include <repositories/company_members_repository.h>
#include <repositories/company_stats_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/users_repository.h>
#include <messages/company/set_tax_response.h>

using namespace std;
using namespace ibh;

TEST_CASE("set tax handler tests") {
    SECTION( "rejects application" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        companies_repository<database_transaction> company_repo{};
        company_members_repository<database_transaction> company_members_repo{};
        company_stats_repository<database_transaction> company_stats_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character company_admin{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(company_admin, transaction);
        REQUIRE(company_admin.id > 0);

        db_company existing_company{0, "test_company"};
        company_repo.insert(existing_company, transaction);
        REQUIRE(existing_company.id > 0);

        db_company_member existing_member{existing_company.id, company_admin.id, COMPANY_ADMIN};
        REQUIRE(company_members_repo.insert(existing_member, transaction) == true);

        db_company_stat existing_stat{0, existing_company.id, company_stat_tax_id, 5};
        company_stats_repo.insert(existing_stat, transaction);
        REQUIRE(existing_stat.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = company_admin.id;
            pc.connection_id = 1;
            pc.company_id = existing_company.id;
            registry.assign<pc_component>(entt, move(pc));
        }

        auto company_entt = registry.create();
        {
            company_component company{existing_company.id, existing_company.name, ibh_flat_map<uint64_t, uint16_t>{{existing_member.character_id, existing_member.member_level}}, ibh_flat_map<uint32_t, int64_t>{{existing_stat.stat_id, existing_stat.value}}};
            registry.assign<company_component>(company_entt, move(company));
        }

        set_tax_message msg(1, 50);

        auto ret = handle_set_tax(&msg, registry, q, transaction);
        REQUIRE(ret == true);

        test_outmsg<set_tax_response>(q, true);

        auto retrieved_stat = company_stats_repo.get_by_stat(existing_company.id, company_stat_tax_id, transaction);
        REQUIRE(retrieved_stat);
        REQUIRE(retrieved_stat->value == 50);

        auto &company = registry.get<company_component>(company_entt);
        REQUIRE(company.stats.find(company_stat_tax_id) != end(company.stats));
        REQUIRE(company.stats.find(company_stat_tax_id)->second == retrieved_stat->value);
    }

    SECTION( "rejects attempt when missing admin rights" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        companies_repository<database_transaction> company_repo{};
        company_members_repository<database_transaction> company_members_repo{};
        company_stats_repository<database_transaction> company_stats_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character company_admin{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(company_admin, transaction);
        REQUIRE(company_admin.id > 0);

        db_company existing_company{0, "test_company"};
        company_repo.insert(existing_company, transaction);
        REQUIRE(existing_company.id > 0);

        db_company_member existing_member{existing_company.id, company_admin.id, COMPANY_MEMBER};
        REQUIRE(company_members_repo.insert(existing_member, transaction) == true);

        db_company_stat existing_stat{0, existing_company.id, company_stat_tax_id, 5};
        company_stats_repo.insert(existing_stat, transaction);
        REQUIRE(existing_stat.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = company_admin.id;
            pc.connection_id = 1;
            pc.company_id = existing_company.id;
            registry.assign<pc_component>(entt, move(pc));
        }

        auto company_entt = registry.create();
        {
            company_component company{existing_company.id, existing_company.name, ibh_flat_map<uint64_t, uint16_t>{{existing_member.character_id, existing_member.member_level}}, ibh_flat_map<uint32_t, int64_t>{{existing_stat.stat_id, existing_stat.value}}};
            registry.assign<company_component>(company_entt, move(company));
        }

        set_tax_message msg(1, 50);

        auto ret = handle_set_tax(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<set_tax_response>(q, false);

        auto retrieved_stat = company_stats_repo.get_by_stat(existing_company.id, company_stat_tax_id, transaction);
        REQUIRE(retrieved_stat);
        REQUIRE(retrieved_stat->value == 5);

        auto &company = registry.get<company_component>(company_entt);
        REQUIRE(company.stats.find(company_stat_tax_id) != end(company.stats));
        REQUIRE(company.stats.find(company_stat_tax_id)->second == retrieved_stat->value);
    }
}
