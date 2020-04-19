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
#include <array>
#include <ibh_containers.h>

using namespace std;

namespace ibh {
    struct stat_component {
        uint64_t stat_id;
        int64_t value;

        stat_component(uint64_t stat_id, int64_t value) : stat_id(stat_id), value(value) {}
    };

    extern string const stat_str;
    extern string const stat_dex;
    extern string const stat_agi;
    extern string const stat_int;
    extern string const stat_spd;
    extern string const stat_vit;
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

    extern string const gear_slot_armor;
    extern string const gear_slot_robe1;
    extern string const gear_slot_robe2;
    extern string const gear_slot_ring1;
    extern string const gear_slot_ring2;
    extern string const gear_slot_head;
    extern string const gear_slot_neck;
    extern string const gear_slot_waist;
    extern string const gear_slot_wrist;
    extern string const gear_slot_hands;
    extern string const gear_slot_feet;
    extern string const gear_slot_ear;

    extern string const company_stat_tax;
    extern string const company_stat_str_bonus;
    extern string const company_stat_agi_bonus;
    extern string const company_stat_int_bonus;
    extern string const company_stat_vit_bonus;
    extern string const company_stat_spd_bonus;
    extern string const company_stat_gold_bonus;
    extern string const company_stat_gold;
    extern string const company_stat_xp_bonus;

    extern array<string const, 42> const stat_names;
    extern array<string const, 12> const slot_names;
    extern array<string const, 9> const company_stat_names;

    uint32_t constexpr stat_str_id = 1;
    uint32_t constexpr stat_dex_id = 2;
    uint32_t constexpr stat_agi_id = 3;
    uint32_t constexpr stat_int_id = 4;
    uint32_t constexpr stat_spd_id = 5;
    uint32_t constexpr stat_vit_id = 6;
    uint32_t constexpr stat_luk_id = 7;
    uint32_t constexpr stat_cha_id = 8;
    uint32_t constexpr stat_con_id = 9;
    uint32_t constexpr stat_move_id = 10;
    uint32_t constexpr stat_hpregen_id = 11;
    uint32_t constexpr stat_mpregen_id = 12;
    uint32_t constexpr stat_hp_id = 13;
    uint32_t constexpr stat_max_hp_id = 14;
    uint32_t constexpr stat_mp_id = 15;
    uint32_t constexpr stat_max_mp_id = 16;
    uint32_t constexpr stat_xp_id = 17;
    uint32_t constexpr stat_gold_id = 18;
    uint32_t constexpr stat_hweapon_damage_rolls_id = 19;
    uint32_t constexpr stat_weapon_armor_class_id = 20;
    uint32_t constexpr stat_armor_class_id = 21;
    uint32_t constexpr stat_accuracy_id = 22;
    uint32_t constexpr stat_offense_id = 23;
    uint32_t constexpr stat_defense_id = 24;
    uint32_t constexpr stat_stealth_id = 25;
    uint32_t constexpr stat_perception_id = 26;
    uint32_t constexpr stat_physical_damage_boost_id = 26;
    uint32_t constexpr stat_magical_damage_boost_id = 27;
    uint32_t constexpr stat_healing_boost_id = 28;
    uint32_t constexpr stat_physical_damage_reflect_id = 29;
    uint32_t constexpr stat_magical_damage_reflect_id = 30;
    uint32_t constexpr stat_mitigation_id = 31;
    uint32_t constexpr stat_magical_resist_id = 32;
    uint32_t constexpr stat_physical_resist_id = 33;
    uint32_t constexpr stat_necrotic_resist_id = 34;
    uint32_t constexpr stat_energy_resist_id = 35;
    uint32_t constexpr stat_water_resist_id = 36;
    uint32_t constexpr stat_fire_resist_id = 37;
    uint32_t constexpr stat_ice_resist_id = 38;
    uint32_t constexpr stat_poison_resist_id = 39;
    uint32_t constexpr stat_disease_resist_id = 40;
    uint32_t constexpr stat_action_speed_id = 41;

    uint32_t constexpr gear_slot_armor_id = 1003;
    uint32_t constexpr gear_slot_robe1_id = 1004;
    uint32_t constexpr gear_slot_robe2_id = 1005;
    uint32_t constexpr gear_slot_ring1_id = 1006;
    uint32_t constexpr gear_slot_ring2_id = 1007;
    uint32_t constexpr gear_slot_head_id = 1008;
    uint32_t constexpr gear_slot_neck_id = 1009;
    uint32_t constexpr gear_slot_waist_id = 1010;
    uint32_t constexpr gear_slot_wrist_id = 1011;
    uint32_t constexpr gear_slot_hands_id = 1012;
    uint32_t constexpr gear_slot_feet_id = 1013;
    uint32_t constexpr gear_slot_ear_id = 1014;

    uint32_t constexpr company_stat_tax_id = 2001;
    uint32_t constexpr company_stat_str_bonus_id = 2002;
    uint32_t constexpr company_stat_agi_bonus_id = 2003;
    uint32_t constexpr company_stat_int_bonus_id = 2004;
    uint32_t constexpr company_stat_vit_bonus_id = 2005;
    uint32_t constexpr company_stat_spd_bonus_id = 2006;
    uint32_t constexpr company_stat_gold_bonus_id = 2007;
    uint32_t constexpr company_stat_gold_id = 2008;
    uint32_t constexpr company_stat_xp_bonus_id = 2009;

    uint32_t constexpr resource_wood_id = 3001;
    uint32_t constexpr resource_ore_id = 3002;
    uint32_t constexpr resource_water_id = 3003;
    uint32_t constexpr resource_plants_id = 3004;
    uint32_t constexpr resource_clay_id = 3005;
    uint32_t constexpr resource_gems_id = 3006;
    uint32_t constexpr resource_paper_id = 3007;
    uint32_t constexpr resource_ink_id = 3008;
    uint32_t constexpr resource_metal_id = 3009;
    uint32_t constexpr resource_bricks_id = 3010;
    uint32_t constexpr resource_timber_id = 3011;


    extern array<uint32_t const, 42> const stat_name_ids;
    extern array<uint32_t const, 12> const slot_name_ids;
    extern array<uint32_t const, 9> const company_stat_name_ids;
    extern array<uint32_t const, 11> const resource_ids;

    extern ibh_flat_map<string, uint32_t> stat_name_to_id_mapper;
    extern ibh_flat_map<uint32_t, string> stat_id_to_name_mapper;
    extern ibh_flat_map<string, uint32_t> slot_name_to_id_mapper;
    extern ibh_flat_map<uint32_t, string> slot_id_to_name_mapper;
    extern ibh_flat_map<string, uint32_t> company_stat_name_to_id_mapper;
    extern ibh_flat_map<uint32_t, string> company_stat_id_to_name_mapper;

    void fill_mappers();

    // constants
    enum class company_member_level : uint16_t {
        COMPANY_MEMBER = 1,
        COMPANY_SAGE,
        COMPANY_ADMIN
    };

    enum class selectable_actions : uint16_t {
        COMBAT = 1,
        WOOD_GATHERING,
        ORE_GATHERING,
        WATER_GATHERING,
        PLANTS_GATHERING,
        CLAY_GATHERING,
        PAPER_CRAFTING,
        INK_CRAFTING,
        METAL_FORGING,
        BRICK_FIRING,
        GEM_CRAFTING,
        WOOD_WORKING,
        ITEM_CRAFTING,
        WORKING,
    };

    enum class resource_types : uint16_t {
        // basic
        WOOD = 1,
        ORE,
        WATER,
        PLANTS,
        CLAY,
        GEMS,


        // compounds from basic
        PAPER, // wood + water
        INK, // water + plants
        METAL, // ore + water
        BRICKS, // water + clay
        TIMBER, // wood
    };

    enum class item_types : uint16_t {
        WEAPON = 1,
        DUAL_HANDED_WEAPON = 1,
        BODY,
        LEGS,
        BOOTS,
        SHIELD,
        CAPE,
        RING,
        NECKLACE,
        POTION,
    };
}