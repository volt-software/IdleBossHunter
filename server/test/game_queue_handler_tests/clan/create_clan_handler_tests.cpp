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
#include <game_queue_message_handlers/clan/create_clan_handler.h>
#include <ecs/components.h>
#include <repositories/clans_repository.h>
#include <repositories/characters_repository.h>
#include <repositories/users_repository.h>

using namespace std;
using namespace ibh;

TEST_CASE("create clan handler tests") {
    SECTION( "character creates clan" ) {
        entt::registry registry;
        outward_queues q;
        create_clan_message msg(1, "clan_name");
        clans_repository<database_transaction> clan_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();
        auto existing_clans = clan_repo.get_all(transaction);

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
            pc.stats.insert({stat_gold_id, 10'001});
            registry.assign<pc_component>(entt, move(pc));
        }

        auto ret = handle_create_clan(&msg, registry, q, transaction);
        REQUIRE(ret == true);

        auto &pc = registry.get<pc_component>(entt);
        auto gold_it = pc.stats.find(stat_gold_id);
        REQUIRE(gold_it != end(pc.stats));
        REQUIRE(gold_it->second == 1);
        auto current_clans = clan_repo.get_all(transaction);
        REQUIRE(current_clans.size() == existing_clans.size() + 1);
    }

    SECTION( "not enough gold" ) {
        entt::registry registry;
        outward_queues q;
        create_clan_message msg(1, "clan_name");
        clans_repository<database_transaction> clan_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();
        auto existing_clans = clan_repo.get_all(transaction);

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
            pc.stats.insert({stat_gold_id, 9'999});
            registry.assign<pc_component>(entt, move(pc));
        }

        auto ret = handle_create_clan(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        auto &pc = registry.get<pc_component>(entt);
        auto gold_it = pc.stats.find(stat_gold_id);
        REQUIRE(gold_it != end(pc.stats));
        REQUIRE(gold_it->second == 9'999);
        auto current_clans = clan_repo.get_all(transaction);
        REQUIRE(current_clans.size() == existing_clans.size());
    }

    SECTION( "already exists" ) {
        entt::registry registry;
        outward_queues q;
        create_clan_message msg(1, "clan_name_exists");
        clans_repository<database_transaction> clan_repo{};
        characters_repository<database_transaction> char_repo{};
        users_repository<database_transaction> user_repo{};
        auto transaction = db_pool->create_transaction();
        db_clan new_clan{};
        new_clan.name = msg.clan_name;
        REQUIRE(clan_repo.insert(new_clan, transaction) == true);
        auto existing_clans = clan_repo.get_all(transaction);

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
            pc.stats.insert({stat_gold_id, 10'001});
            registry.assign<pc_component>(entt, move(pc));
        }

        auto ret = handle_create_clan(&msg, registry, q, transaction);
        REQUIRE(ret == false);

        auto &pc = registry.get<pc_component>(entt);
        auto gold_it = pc.stats.find(stat_gold_id);
        REQUIRE(gold_it != end(pc.stats));
        REQUIRE(gold_it->second == 10'001);
        auto current_clans = clan_repo.get_all(transaction);
        REQUIRE(current_clans.size() == existing_clans.size());
    }
}
