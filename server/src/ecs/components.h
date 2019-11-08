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

#pragma once

#include <string>
#include <variant>
#include <array>
#include <vector>
#include <optional>
#include <lotr_flat_map.h>
#include <entt/entity/registry.hpp>

using namespace std;

namespace lotr {
    extern array<string const, 42> const stat_names;
    extern array<string const, 14> const slot_names;

    // enums

    // components

    struct stat_component {
        string name;
        int64_t value;

        stat_component(string name, int64_t value) : name(move(name)), value(value) {}
    };

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

        vector<stat_component> stats;
        vector<random_stat_component> random_stats;
        //vector<item_component> items;
        //vector<skill_component> skills;

        monster_definition_component(string name, uint64_t min_level, uint64_t max_level, vector<stat_component> stats, vector<random_stat_component> random_stats) :
        name(move(name)), min_level(min_level), max_level(max_level), stats(move(stats)), random_stats(move(random_stats)) {}
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

        lotr_flat_map<string, uint64_t> stats;
        lotr_flat_map<string, item_component> items;
        lotr_flat_map<string, skill_component> skills;

        pc_component() : id(), name(), race(), dir(), _class(), spawn_message(),
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

    extern string const stat_str;
    extern string const stat_dex;
    extern string const stat_agi;
    extern string const stat_int;
    extern string const stat_wis;
    extern string const stat_wil;
    extern string const stat_luk;
    extern string const stat_cha;
    extern string const stat_con;
    extern string const stat_move;
    extern string const stat_hpregen;
    extern string const stat_mpregen;
    extern string const stat_hp;
    extern string const stat_max_hp;
    extern string const stat_mp;
    extern string const stat_max_mp;
    extern string const stat_xp;
    extern string const stat_gold;
    extern string const stat_hweapon_damage_rolls;
    extern string const stat_weapon_armor_class;
    extern string const stat_armor_class;
    extern string const stat_accuracy;
    extern string const stat_offense;
    extern string const stat_defense;
    extern string const stat_stealth;
    extern string const stat_perception;
    extern string const stat_physical_damage_boost;
    extern string const stat_magical_damage_boost;
    extern string const stat_healing_boost;
    extern string const stat_physical_damage_reflect;
    extern string const stat_magical_damage_reflect;
    extern string const stat_mitigation;
    extern string const stat_magical_resist;
    extern string const stat_physical_resist;
    extern string const stat_necrotic_resist;
    extern string const stat_energy_resist;
    extern string const stat_water_resist;
    extern string const stat_fire_resist;
    extern string const stat_ice_resist;
    extern string const stat_poison_resist;
    extern string const stat_disease_resist;
    extern string const stat_action_speed;

    extern string const gear_slot_right_hand;
    extern string const gear_slot_left_hand;
    extern string const gear_slot_armor;
    extern string const gear_slot_robe1;
    extern string const gear_slot_robe2;
    extern string const gear_slot_ring1;
    extern string const gear_slot_ring2;
    extern string const gear_slot_head;
    extern string const gear_slot_next;
    extern string const gear_slot_waist;
    extern string const gear_slot_wrist;
    extern string const gear_slot_hands;
    extern string const gear_slot_feet;
    extern string const gear_slot_ear;
}
