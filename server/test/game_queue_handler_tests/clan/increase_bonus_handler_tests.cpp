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
#include <game_queue_message_handlers/clan/increase_bonus_handler.h>
#include <ecs/components.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_members_repository.h>
#include <repositories/clan_stats_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/users_repository.h>
#include <messages/clan/increase_bonus_response.h>

using namespace std;
using namespace ibh;

TEST_CASE("increase bonus handler tests") {
    SECTION( "rejects application" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        clans_repository<database_transaction> clan_repo{};
        clan_members_repository<database_transaction> clan_members_repo{};
        clan_stats_repository<database_transaction> clan_stats_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character clan_admin{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(clan_admin, transaction);
        REQUIRE(clan_admin.id > 0);

        db_clan existing_clan{0, "test_clan"};
        clan_repo.insert(existing_clan, transaction);
        REQUIRE(existing_clan.id > 0);

        db_clan_member existing_member{existing_clan.id, clan_admin.id, CLAN_ADMIN};
        REQUIRE(clan_members_repo.insert(existing_member, transaction) == true);

        db_clan_stat existing_str_stat{0, existing_clan.id, clan_stat_str_bonus_id, 5};
        clan_stats_repo.insert(existing_str_stat, transaction);
        REQUIRE(existing_str_stat.id > 0);

        db_clan_stat existing_gold_stat{0, existing_clan.id, clan_stat_gold_id, 20'000'000};
        clan_stats_repo.insert(existing_gold_stat, transaction);
        REQUIRE(existing_gold_stat.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = clan_admin.id;
            pc.connection_id = 1;
            pc.clan_id = existing_clan.id;
            registry.assign<pc_component>(entt, move(pc));
        }

        auto clan_entt = registry.create();
        {
            clan_component clan{existing_clan.id, existing_clan.name, ibh_flat_map<uint64_t, uint16_t>{{existing_member.character_id, existing_member.member_level}},
                                ibh_flat_map<uint32_t, int64_t>{{existing_str_stat.stat_id, existing_str_stat.value}, {existing_gold_stat.stat_id, existing_gold_stat.value}}};
            registry.assign<clan_component>(clan_entt, move(clan));
        }

        increase_bonus_message msg(1, clan_stat_str_bonus_id);

        auto ret = handle_increase_bonus(&msg, registry, q, transaction);
        REQUIRE(ret == true);

        test_outmsg<increase_bonus_response>(q, true);

        auto retrieved_stat = clan_stats_repo.get_by_stat(existing_clan.id, clan_stat_str_bonus_id, transaction);
        REQUIRE(retrieved_stat);
        REQUIRE(retrieved_stat->value == 6);

        auto &clan = registry.get<clan_component>(clan_entt);
        REQUIRE(clan.stats.find(msg.bonus_type) != end(clan.stats));
        REQUIRE(clan.stats.find(msg.bonus_type)->second == retrieved_stat->value);
        REQUIRE(clan.stats.find(clan_stat_gold_id) != end(clan.stats));
        REQUIRE(clan.stats.find(clan_stat_gold_id)->second < existing_gold_stat.value);
    }

    SECTION( "rejects attempt when missing admin rights" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        clans_repository<database_transaction> clan_repo{};
        clan_members_repository<database_transaction> clan_members_repo{};
        clan_stats_repository<database_transaction> clan_stats_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character clan_admin{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(clan_admin, transaction);
        REQUIRE(clan_admin.id > 0);

        db_clan existing_clan{0, "test_clan"};
        clan_repo.insert(existing_clan, transaction);
        REQUIRE(existing_clan.id > 0);

        db_clan_member existing_member{existing_clan.id, clan_admin.id, CLAN_MEMBER};
        REQUIRE(clan_members_repo.insert(existing_member, transaction) == true);

        db_clan_stat existing_str_stat{0, existing_clan.id, clan_stat_str_bonus_id, 5};
        clan_stats_repo.insert(existing_str_stat, transaction);
        REQUIRE(existing_str_stat.id > 0);

        db_clan_stat existing_gold_stat{0, existing_clan.id, clan_stat_gold_id, 2'000'000};
        clan_stats_repo.insert(existing_gold_stat, transaction);
        REQUIRE(existing_gold_stat.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = clan_admin.id;
            pc.connection_id = 1;
            pc.clan_id = existing_clan.id;
            registry.assign<pc_component>(entt, move(pc));
        }

        auto clan_entt = registry.create();
        {
            clan_component clan{existing_clan.id, existing_clan.name, ibh_flat_map<uint64_t, uint16_t>{{existing_member.character_id, existing_member.member_level}},
                                ibh_flat_map<uint32_t, int64_t>{{existing_str_stat.stat_id, existing_str_stat.value}, {existing_gold_stat.stat_id, existing_gold_stat.value}}};
            registry.assign<clan_component>(clan_entt, move(clan));
        }

        increase_bonus_message msg(1, clan_stat_str_bonus_id);

        auto ret = handle_increase_bonus(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<increase_bonus_response>(q, false);

        auto retrieved_stat = clan_stats_repo.get_by_stat(existing_clan.id, clan_stat_str_bonus_id, transaction);
        REQUIRE(retrieved_stat);
        REQUIRE(retrieved_stat->value == 5);

        auto &clan = registry.get<clan_component>(clan_entt);
        REQUIRE(clan.stats.find(msg.bonus_type) != end(clan.stats));
        REQUIRE(clan.stats.find(msg.bonus_type)->second == retrieved_stat->value);
        REQUIRE(clan.stats.find(clan_stat_gold_id) != end(clan.stats));
        REQUIRE(clan.stats.find(clan_stat_gold_id)->second == existing_gold_stat.value);
    }

    SECTION( "rejects attempt when not enough clan gold" ) {
        entt::registry registry;
        moodycamel::ConcurrentQueue<outward_message> cq;
        outward_queues q(&cq);
        clans_repository<database_transaction> clan_repo{};
        clan_members_repository<database_transaction> clan_members_repo{};
        clan_stats_repository<database_transaction> clan_stats_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();

        db_user user{};
        user_repo.insert_if_not_exists(user, transaction);
        REQUIRE(user.id > 0);
        db_character clan_admin{0, user.id, 0, 0, 0, 0, 0, 0, 0, "", "", "", "", vector<db_character_stat> {}, vector<db_item> {}};
        char_repo.insert(clan_admin, transaction);
        REQUIRE(clan_admin.id > 0);

        db_clan existing_clan{0, "test_clan"};
        clan_repo.insert(existing_clan, transaction);
        REQUIRE(existing_clan.id > 0);

        db_clan_member existing_member{existing_clan.id, clan_admin.id, CLAN_MEMBER};
        REQUIRE(clan_members_repo.insert(existing_member, transaction) == true);

        db_clan_stat existing_str_stat{0, existing_clan.id, clan_stat_str_bonus_id, 5};
        clan_stats_repo.insert(existing_str_stat, transaction);
        REQUIRE(existing_str_stat.id > 0);

        db_clan_stat existing_gold_stat{0, existing_clan.id, clan_stat_gold_id, 2'00'000};
        clan_stats_repo.insert(existing_gold_stat, transaction);
        REQUIRE(existing_gold_stat.id > 0);

        auto entt = registry.create();
        {
            pc_component pc{};
            pc.id = clan_admin.id;
            pc.connection_id = 1;
            pc.clan_id = existing_clan.id;
            registry.assign<pc_component>(entt, move(pc));
        }

        auto clan_entt = registry.create();
        {
            clan_component clan{existing_clan.id, existing_clan.name, ibh_flat_map<uint64_t, uint16_t>{{existing_member.character_id, existing_member.member_level}},
                                ibh_flat_map<uint32_t, int64_t>{{existing_str_stat.stat_id, existing_str_stat.value}, {existing_gold_stat.stat_id, existing_gold_stat.value}}};
            registry.assign<clan_component>(clan_entt, move(clan));
        }

        increase_bonus_message msg(1, clan_stat_str_bonus_id);

        auto ret = handle_increase_bonus(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        test_outmsg<increase_bonus_response>(q, false);

        auto retrieved_stat = clan_stats_repo.get_by_stat(existing_clan.id, clan_stat_str_bonus_id, transaction);
        REQUIRE(retrieved_stat);
        REQUIRE(retrieved_stat->value == 5);

        auto &clan = registry.get<clan_component>(clan_entt);
        REQUIRE(clan.stats.find(msg.bonus_type) != end(clan.stats));
        REQUIRE(clan.stats.find(msg.bonus_type)->second == retrieved_stat->value);
        REQUIRE(clan.stats.find(clan_stat_gold_id) != end(clan.stats));
        REQUIRE(clan.stats.find(clan_stat_gold_id)->second == existing_gold_stat.value);
    }
}
