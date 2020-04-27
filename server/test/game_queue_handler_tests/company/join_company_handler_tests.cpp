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
#include <game_queue_message_handlers/company/join_company_handler.h>
#include <ecs/components.h>
#include <repositories/companies_repository.h>
#include <repositories/company_members_repository.h>
#include <repositories/company_member_applications_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/users_repository.h>
#include <messages/company/join_company_response.h>

using namespace std;
using namespace ibh;

TEST_CASE("join company handler tests") {
    SECTION( "joins company" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        companies_repository<database_transaction> company_repo{};
        company_members_repository<database_transaction> company_members_repo{};
        company_member_applications_repository<database_transaction> company_applications_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character company_applicant{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(company_applicant, transaction);
        REQUIRE(company_applicant.id > 0);

        db_company existing_company{0, "test_company", 0, 2};
        company_repo.insert(existing_company, transaction);
        REQUIRE(existing_company.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = company_applicant.id;
            pc.connection_id = 1;
            registry.emplace<pc_component>(entt, move(pc));
        }

        join_company_message msg(1, existing_company.name);

        auto ret = handle_join_company(&msg, registry, q, transaction);
        REQUIRE(ret == true);

        test_outmsg<join_company_response>(q, true);

        auto all_applicants = company_applications_repo.get_by_company_id(existing_company.id, transaction);
        auto applicant_it = find_if(begin(all_applicants), end(all_applicants), [&](const auto &a){ return a.character_id == company_applicant.id; });
        REQUIRE(applicant_it != end(all_applicants));

        auto all_members = company_members_repo.get_by_company_id(existing_company.id, transaction);
        REQUIRE(all_members.empty());
    }

    SECTION( "cannot join company when already member of another" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        companies_repository<database_transaction> company_repo{};
        company_members_repository<database_transaction> company_members_repo{};
        company_member_applications_repository<database_transaction> company_applications_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character company_applicant{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(company_applicant, transaction);
        REQUIRE(company_applicant.id > 0);

        db_company existing_company{0, "test_company", 0, 2};
        company_repo.insert(existing_company, transaction);
        REQUIRE(existing_company.id > 0);

        db_company second_existing_company{0, "test_company2", 0, 2};
        company_repo.insert(second_existing_company, transaction);
        REQUIRE(second_existing_company.id > 0);

        db_company_member existing_member{existing_company.id, company_applicant.id, 0, 0};
        REQUIRE(company_members_repo.insert(existing_member, transaction) == true);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = company_applicant.id;
            pc.connection_id = 1;
            registry.emplace<pc_component>(entt, move(pc));
        }

        join_company_message msg(1, second_existing_company.name);

        auto ret = handle_join_company(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<join_company_response>(q, false);

        auto all_applicants = company_applications_repo.get_by_company_id(existing_company.id, transaction);
        REQUIRE(all_applicants.empty());

        auto all_members = company_members_repo.get_by_company_id(second_existing_company.id, transaction);
        REQUIRE(all_members.empty());
    }

    SECTION( "cannot join company when already applied" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        companies_repository<database_transaction> company_repo{};
        company_members_repository<database_transaction> company_members_repo{};
        company_member_applications_repository<database_transaction> company_applications_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character company_applicant{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(company_applicant, transaction);
        REQUIRE(company_applicant.id > 0);

        db_company existing_company{0, "test_company", 0, 2};
        company_repo.insert(existing_company, transaction);
        REQUIRE(existing_company.id > 0);

        db_company_member existing_member{existing_company.id, company_applicant.id, 0, 0};
        REQUIRE(company_applications_repo.insert(existing_member, transaction) == true);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = company_applicant.id;
            pc.connection_id = 1;
            registry.emplace<pc_component>(entt, move(pc));
        }

        join_company_message msg(1, existing_company.name);

        auto ret = handle_join_company(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<join_company_response>(q, false);

        auto all_applicants = company_applications_repo.get_by_company_id(existing_company.id, transaction);
        auto applicant_it = find_if(begin(all_applicants), end(all_applicants), [&](const auto &a){ return a.character_id == company_applicant.id; });
        REQUIRE(applicant_it != end(all_applicants));

        auto all_members = company_members_repo.get_by_company_id(existing_company.id, transaction);
        REQUIRE(all_members.empty());
    }
}
