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
#include <messages/user_access/login_request.h>
#include <messages/user_access/login_response.h>
#include <messages/user_access/register_request.h>
#include <messages/user_access/play_character_request.h>
#include <messages/user_access/play_character_response.h>
#include <messages/user_access/create_character_request.h>
#include <messages/user_access/create_character_response.h>
#include <messages/user_access/character_select_request.h>
#include <messages/user_access/character_select_response.h>
#include <messages/user_access/delete_character_request.h>
#include <messages/user_access/user_entered_game_response.h>
#include <messages/user_access/user_left_game_response.h>
#include <messages/chat/message_request.h>
#include <messages/chat/message_response.h>
#include <messages/moderator/set_motd_request.h>
#include <messages/moderator/update_motd_response.h>
#include <messages/update_response.h>
#include <messages/generic_error_response.h>
#include <messages/generic_ok_response.h>

#include <ecs/components.h>

using namespace std;
using namespace ibh;

#define SERDE_SINGLE(type) type msg{}; \
            rapidjson::Document d; \
            auto ser = msg.serialize(); \
            d.Parse(ser.c_str(), ser.size()); \
            auto msg2 = type::deserialize(d);

#define SERDE(type, ...) type msg(__VA_ARGS__); \
            rapidjson::Document d; \
            auto ser = msg.serialize(); \
            d.Parse(ser.c_str(), ser.size()); \
            auto msg2 = type::deserialize(d);

TEST_CASE("message serialization tests") {

    // user access control

    SECTION("login request") {
        SERDE(login_request, "user", "pass");
        REQUIRE(msg.username == msg2->username);
        REQUIRE(msg.password == msg2->password);
    }

    SECTION("empty login response") {
        vector<character_object> players;
        vector<account_object> users;
        SERDE(login_response, move(players), move(users), "username", "email", "motd");
        REQUIRE(msg.characters.size() == msg2->characters.size());
        REQUIRE(msg.username == msg2->username);
        REQUIRE(msg.email == msg2->email);
        REQUIRE(msg.motd == msg2->motd);
    }

    SECTION("login response") {
        vector<character_object> players;
        players.emplace_back("name", "race", "baseclass", 1, 9, 2, 5, 213, vector<stat_component>{}, vector<item_object>{}, vector<skill_object>{});
        players.emplace_back("name2", "race2", "baseclass2", 3, 19, 4, 6, 343, vector<stat_component>{}, vector<item_object>{}, vector<skill_object>{});
        vector<account_object> users;
        users.emplace_back(false, true, false, 123, 456, "user1");
        users.emplace_back(true, false, true, 890, 342, "user2");
        SERDE(login_response, move(players), move(users), "username", "email", "motd");
        REQUIRE(msg.characters.size() == msg2->characters.size());
        REQUIRE(msg.online_users.size() == msg2->online_users.size());
        REQUIRE(msg.username == msg2->username);
        REQUIRE(msg.email == msg2->email);
        REQUIRE(msg.motd == msg2->motd);

        for(uint32_t i = 0; i < msg.characters.size(); i++) {
            REQUIRE(msg.characters[i].name == msg2->characters[i].name);
            REQUIRE(msg.characters[i].race == msg2->characters[i].race);
            REQUIRE(msg.characters[i].baseclass == msg2->characters[i].baseclass);
            REQUIRE(msg.characters[i].level == msg2->characters[i].level);
            REQUIRE(msg.characters[i].slot == msg2->characters[i].slot);
            REQUIRE(msg.characters[i].gold == msg2->characters[i].gold);
            REQUIRE(msg.characters[i].xp == msg2->characters[i].xp);
            REQUIRE(msg.characters[i].skill_points == msg2->characters[i].skill_points);
        }

        for(uint32_t i = 0; i < msg.online_users.size(); i++) {
            REQUIRE(msg.online_users[i].is_game_master == msg2->online_users[i].is_game_master);
            REQUIRE(msg.online_users[i].is_tester == msg2->online_users[i].is_tester);
            REQUIRE(msg.online_users[i].has_done_trial == msg2->online_users[i].has_done_trial);
            REQUIRE(msg.online_users[i].trial_ends_unix_timestamp == msg2->online_users[i].trial_ends_unix_timestamp);
            REQUIRE(msg.online_users[i].subscription_tier == msg2->online_users[i].subscription_tier);
            REQUIRE(msg.online_users[i].username == msg2->online_users[i].username);
        }
    }

    SECTION("register request") {
        SERDE(register_request, "user", "pass", "email");
        REQUIRE(msg.username == msg2->username);
        REQUIRE(msg.password == msg2->password);
        REQUIRE(msg.email == msg2->email);
    }

    SECTION("play character request") {
        SERDE(play_character_request, 1);
        REQUIRE(msg.slot == msg2->slot);
    }

    SECTION("play character response") {
        SERDE(play_character_response, 1);
        REQUIRE(msg.slot == msg2->slot);
    }

    SECTION("create character request") {
        SERDE(create_character_request, 1, "name", "race", "baseclass");
        REQUIRE(msg.slot == msg2->slot);
        REQUIRE(msg.name == msg2->name);
        REQUIRE(msg.race == msg2->race);
        REQUIRE(msg.baseclass == msg2->baseclass);
    }

    SECTION("create character response") {
        SERDE(create_character_response, character_object("name", "race", "baseclass", 1, 9, 2, 5, 213, vector<stat_component>{}, vector<item_object>{}, vector<skill_object>{}));
        REQUIRE(msg.character.name == msg2->character.name);
        REQUIRE(msg.character.race == msg2->character.race);
        REQUIRE(msg.character.baseclass == msg2->character.baseclass);
        REQUIRE(msg.character.level == msg2->character.level);
        REQUIRE(msg.character.slot == msg2->character.slot);
        REQUIRE(msg.character.gold == msg2->character.gold);
        REQUIRE(msg.character.xp == msg2->character.xp);
        REQUIRE(msg.character.skill_points == msg2->character.skill_points);
    }

    SECTION("character select request") {
        SERDE_SINGLE(character_select_request);
        REQUIRE(msg2);
    }

    SECTION("character select response") {
        vector<character_class> classes;
        {
            vector<stat_component> stat_mods;
            vector<item_object> items;
            vector<skill_object> skills;

            stat_mods.emplace_back("test2", 234);
            items.emplace_back(1, "test3", "4", "5", vector<stat_component>{});
            skills.emplace_back("test4", 345);
            classes.emplace_back("test5", "test6", stat_mods, items, skills);
        }

        vector<character_race> races;
        {
            vector<stat_component> stat_mods;
            stat_mods.emplace_back("test7", 456);
            races.emplace_back("test8", "test9", stat_mods);
        }
        SERDE(character_select_response, races, classes);
        REQUIRE(msg.races.size() == 1);
        REQUIRE(msg.races.size() == msg2->races.size());
        REQUIRE(msg.classes.size() == 1);
        REQUIRE(msg.classes.size() == msg2->classes.size());

        REQUIRE(msg.races[0].name == msg2->races[0].name);
        REQUIRE(msg.races[0].description == msg2->races[0].description);
        REQUIRE(msg.races[0].level_stat_mods.size() == 1);
        REQUIRE(msg.races[0].level_stat_mods.size() == msg2->races[0].level_stat_mods.size());
        REQUIRE(msg.races[0].level_stat_mods[0].name == msg2->races[0].level_stat_mods[0].name);
        REQUIRE(msg.races[0].level_stat_mods[0].value == msg2->races[0].level_stat_mods[0].value);

        REQUIRE(msg.classes[0].name == msg2->classes[0].name);
        REQUIRE(msg.classes[0].description == msg2->classes[0].description);
        REQUIRE(msg.classes[0].stat_mods.size() == 1);
        REQUIRE(msg.classes[0].stat_mods.size() == msg2->classes[0].stat_mods.size());
        REQUIRE(msg.classes[0].stat_mods[0].name == msg2->classes[0].stat_mods[0].name);
        REQUIRE(msg.classes[0].stat_mods[0].value == msg2->classes[0].stat_mods[0].value);
        REQUIRE(msg.classes[0].items.size() == 1);
        REQUIRE(msg.classes[0].items.size() == msg2->classes[0].items.size());
        REQUIRE(msg.classes[0].items[0].name == msg2->classes[0].items[0].name);
        REQUIRE(msg.classes[0].skills.size() == 1);
        REQUIRE(msg.classes[0].skills.size() == msg2->classes[0].skills.size());
        REQUIRE(msg.classes[0].skills[0].name == msg2->classes[0].skills[0].name);
        REQUIRE(msg.classes[0].skills[0].value == msg2->classes[0].skills[0].value);
    }

    SECTION("delete character request") {
        SERDE(delete_character_request, 1);
        REQUIRE(msg.slot == msg2->slot);
    }

    SECTION("user joined response") {
        SERDE(user_entered_game_response, account_object(true, false, true, 123, 345, "username"));
        REQUIRE(msg.user.is_game_master == msg2->user.is_game_master);
        REQUIRE(msg.user.is_tester == msg2->user.is_tester);
        REQUIRE(msg.user.has_done_trial == msg2->user.has_done_trial);
        REQUIRE(msg.user.trial_ends_unix_timestamp == msg2->user.trial_ends_unix_timestamp);
        REQUIRE(msg2->user.trial_ends_unix_timestamp == 123);
        REQUIRE(msg.user.subscription_tier == msg2->user.subscription_tier);
        REQUIRE(msg2->user.subscription_tier == 345);
        REQUIRE(msg.user.username == msg2->user.username);
    }

    SECTION("user left game response") {
        SERDE(user_left_game_response, "username");
        REQUIRE(msg.username == msg2->username);
    }

    // commands

    // chat

    SECTION("message request") {
        SERDE(message_request, "content");
        REQUIRE(msg.content == msg2->content);
    }

    SECTION("message response") {
        SERDE(message_response, "user", "content", "source", 1234);
        REQUIRE(msg.user == msg2->user);
        REQUIRE(msg.content == msg2->content);
        REQUIRE(msg.source == msg2->source);
        REQUIRE(msg.unix_timestamp == msg2->unix_timestamp);
    }

    // moderator

    SECTION("set motd request") {
        SERDE(set_motd_request, "motd");
        REQUIRE(msg.motd == msg2->motd);
    }

    SECTION("update motd response") {
        SERDE(update_motd_response, "motd");
        REQUIRE(msg.motd == msg2->motd);
    }

    // misc

    SECTION("update response") {
        SERDE_SINGLE(update_response)
        REQUIRE(msg2);
    }

    SECTION("generic error response") {
        SERDE(generic_error_response, "err", "name", "desc", true);
        REQUIRE(msg.error == msg2->error);
        REQUIRE(msg.pretty_error_name == msg2->pretty_error_name);
        REQUIRE(msg.pretty_error_name == msg2->pretty_error_name);
        REQUIRE(msg.clear_login_data == msg2->clear_login_data);
    }

    SECTION("generic ok response") {
        SERDE(generic_ok_response, "msg");
        REQUIRE(msg.message == msg2->message);
    }
}
