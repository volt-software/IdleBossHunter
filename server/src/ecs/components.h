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

#pragma once

#include <string>
#include <variant>
#include <array>
#include <vector>
#include <optional>
#include <ibh_containers.h>
#include <entt/entity/registry.hpp>
#include "common_components.h"

using namespace std;

namespace ibh {

    // enums

    // components

    struct random_stat_component {
        string name;
        int64_t min;
        int64_t max;

        random_stat_component(string name, int64_t min, int64_t max) : name(move(name)), min(min), max(max) {}
    };

    struct skill_component {
        string name;
        int64_t level;
    };

    struct item_effect_component {
        string name;
        string tooltip;
        string message;
        uint32_t potency;
        uint32_t duration;
        uint32_t uses;
        uint32_t range;
        uint32_t chance;
        bool autocast;
        bool can_apply;
        vector<stat_component> stats;
    };

    struct item_required_skill_component {
        string name;
        uint32_t level;
    };

    struct item_component {
        string name;
        string desc;
        string type;
        uint64_t value;
        uint64_t sprite;
        uint64_t quality;
        uint64_t enchant_level;
        uint64_t required_level;
        uint64_t tier;
        bool binds;
        bool tells_bind;
        optional<item_effect_component> effect;
        //optional<random_stat_component> random_trait_levels;
        //vector<string> random_trait_names;
        vector<string> required_professions;
        //vector<item_required_skill_component> required_skills;
        vector<stat_component> stats;
        //vector<random_stat_component> random_stats;
    };

    struct monster_definition_component {
        string name;

        uint64_t min_level;
        uint64_t max_level;

        ibh_flat_map<string, stat_component> stats;
        //vector<random_stat_component> random_stats;
        //vector<item_component> items;
        //vector<skill_component> skills;

        monster_definition_component(string name, uint64_t min_level, uint64_t max_level, ibh_flat_map<string, stat_component> stats) :
        name(move(name)), min_level(min_level), max_level(max_level), stats(move(stats)) {}
    };

    struct monster_special_definition_component {
        string name;
        vector<stat_component> stat_multipliers;
        bool teleport_when_beat;

        monster_special_definition_component(string name, vector<stat_component> stats, bool teleport) : name(move(name)), stat_multipliers(move(stats)), teleport_when_beat(teleport) {}
    };

    struct monster_component {
        string name;
        string special_name;
        uint32_t level;
        vector<stat_component> stats;
        bool teleport_when_beat;
    };

    struct clan_member_component {
        uint64_t character_id;
        uint16_t member_level;
    };

    struct clan_building_definition_component {
        string name;
        vector<stat_component> bonuses;
        uint64_t cost;
    };

    struct clan_component {
        string name;
        vector<clan_member_component> members;
        vector<stat_component> stats;
    };

    struct battle_component {
        string monster_name;
        uint32_t monster_level;
        ibh_flat_map<string, stat_component> monster_stats;
        ibh_flat_map<string, stat_component> total_player_stats;

        battle_component(string monster_name, uint32_t monster_level, ibh_flat_map<string, stat_component> monster_stats) : monster_name(move(monster_name)), monster_level(monster_level), monster_stats(move(monster_stats)) {}
    };

    struct pc_component {
        uint64_t id;
        uint64_t connection_id;
        string name;
        string race;
        string dir;
        string _class;
        string spawn_message;

        uint64_t level;
        uint64_t skill_points;

        optional<battle_component> battle;

        ibh_flat_map<string, int64_t> stats;
        ibh_flat_map<string, item_component> items;
        ibh_flat_map<string, skill_component> skills;

        pc_component() : id(), connection_id(), name(), race(), dir(), _class(), spawn_message(),
                          level(), skill_points(), stats(), items(), skills() {}
    };

    struct user_component {
        string name;
        string email;
        uint16_t subscription_tier;
        bool is_tester;
        bool is_game_master;
        uint16_t trial_ends;
        bool has_done_trial;
        string discord_tag;
        bool discord_online;
        vector<pc_component> characters;
    };

    // helper functions


    // constants
}
