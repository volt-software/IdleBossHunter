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
#include <game_queue_message_handlers/clan/accept_application_handler.h>
#include <ecs/components.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_members_repository.h>
#include <repositories/clan_member_applications_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/users_repository.h>
#include <messages/clan/accept_application_response.h>

using namespace std;
using namespace ibh;

TEST_CASE("accept application handler tests") {
    SECTION( "accepts application" ) {
        entt::registry registry;
        outward_queues q;
        clans_repository<database_transaction> clan_repo{};
        clan_members_repository<database_transaction> clan_members_repo{};
        clan_member_applications_repository<database_transaction> clan_applications_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character clan_admin{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(clan_admin, transaction);
        REQUIRE(clan_admin.id > 0);
        db_character clan_applicant{0, user.id, 1, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(clan_applicant, transaction);
        REQUIRE(clan_applicant.id > 0);

        db_clan existing_clan{0, "test_clan"};
        clan_repo.insert(existing_clan, transaction);
        REQUIRE(existing_clan.id > 0);

        db_clan_member existing_member{existing_clan.id, clan_admin.id, CLAN_ADMIN};
        REQUIRE(clan_members_repo.insert(existing_member, transaction) == true);

        db_clan_member new_member{existing_clan.id, clan_applicant.id, 0};
        REQUIRE(clan_applications_repo.insert(new_member, transaction) == true);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = clan_admin.id;
            pc.connection_id = 1;
            registry.assign<pc_component>(entt, move(pc));
        }

        accept_application_message msg(1, clan_applicant.id);

        auto ret = handle_accept_application(&msg, registry, q, transaction);
        REQUIRE(ret == true);

        test_outmsg<accept_application_response>(q, true);

        auto all_applicants = clan_applications_repo.get_by_clan_id(existing_clan.id, transaction);
        auto applicant_it = find_if(begin(all_applicants), end(all_applicants), [&](const auto &a){ return a.character_id == clan_applicant.id; });
        REQUIRE(applicant_it == end(all_applicants));

        auto all_members = clan_members_repo.get_by_clan_id(existing_clan.id, transaction);
        auto member_it = find_if(begin(all_members), end(all_members), [&](const auto &m){ return m.character_id == clan_applicant.id; });
        REQUIRE(member_it != end(all_members));
    }

    SECTION( "rejects attempt when missing admin rights" ) {
        entt::registry registry;
        outward_queues q;
        clans_repository<database_transaction> clan_repo{};
        clan_members_repository<database_transaction> clan_members_repo{};
        clan_member_applications_repository<database_transaction> clan_applications_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character clan_admin{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(clan_admin, transaction);
        REQUIRE(clan_admin.id > 0);
        db_character clan_applicant{0, user.id, 1, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(clan_applicant, transaction);
        REQUIRE(clan_applicant.id > 0);

        db_clan existing_clan{0, "test_clan"};
        clan_repo.insert(existing_clan, transaction);
        REQUIRE(existing_clan.id > 0);

        db_clan_member existing_member{existing_clan.id, clan_admin.id, CLAN_MEMBER};
        REQUIRE(clan_members_repo.insert(existing_member, transaction) == true);

        db_clan_member new_member{existing_clan.id, clan_applicant.id, 0};
        REQUIRE(clan_applications_repo.insert(new_member, transaction) == true);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = clan_admin.id;
            pc.connection_id = 1;
            registry.assign<pc_component>(entt, move(pc));
        }

        accept_application_message msg(1, clan_applicant.id);

        auto ret = handle_accept_application(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<accept_application_response>(q, false);

        auto all_applicants = clan_applications_repo.get_by_clan_id(existing_clan.id, transaction);
        auto applicant_it = find_if(begin(all_applicants), end(all_applicants), [&](const auto &a){ return a.character_id == clan_applicant.id; });
        REQUIRE(applicant_it != end(all_applicants));

        auto all_members = clan_members_repo.get_by_clan_id(existing_clan.id, transaction);
        auto member_it = find_if(begin(all_members), end(all_members), [&](const auto &m){ return m.character_id == clan_applicant.id; });
        REQUIRE(member_it == end(all_members));
    }
}
