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
        string name;
        int64_t value;

        stat_component(string name, int64_t value) : name(move(name)), value(value) {}
    };

    struct stat_id_component {
        uint32_t id;
        int64_t value;

        stat_id_component(uint32_t id, int64_t value) : id(id), value(value) {}
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

    extern array<string const, 42> const stat_names;
    extern array<string const, 12> const slot_names;

    extern uint32_t const stat_str_id;
    extern uint32_t const stat_dex_id;
    extern uint32_t const stat_agi_id;
    extern uint32_t const stat_int_id;
    extern uint32_t const stat_spd_id;
    extern uint32_t const stat_vit_id;
    extern uint32_t const stat_luk_id;
    extern uint32_t const stat_cha_id;
    extern uint32_t const stat_con_id;
    extern uint32_t const stat_move_id;
    extern uint32_t const stat_hpregen_id;
    extern uint32_t const stat_mpregen_id;
    extern uint32_t const stat_hp_id;
    extern uint32_t const stat_max_hp_id;
    extern uint32_t const stat_mp_id;
    extern uint32_t const stat_max_mp_id;
    extern uint32_t const stat_xp_id;
    extern uint32_t const stat_gold_id;
    extern uint32_t const stat_hweapon_damage_rolls_id;
    extern uint32_t const stat_weapon_armor_class_id;
    extern uint32_t const stat_armor_class_id;
    extern uint32_t const stat_accuracy_id;
    extern uint32_t const stat_offense_id;
    extern uint32_t const stat_defense_id;
    extern uint32_t const stat_stealth_id;
    extern uint32_t const stat_perception_id;
    extern uint32_t const stat_physical_damage_boost_id;
    extern uint32_t const stat_magical_damage_boost_id;
    extern uint32_t const stat_healing_boost_id;
    extern uint32_t const stat_physical_damage_reflect_id;
    extern uint32_t const stat_magical_damage_reflect_id;
    extern uint32_t const stat_mitigation_id;
    extern uint32_t const stat_magical_resist_id;
    extern uint32_t const stat_physical_resist_id;
    extern uint32_t const stat_necrotic_resist_id;
    extern uint32_t const stat_energy_resist_id;
    extern uint32_t const stat_water_resist_id;
    extern uint32_t const stat_fire_resist_id;
    extern uint32_t const stat_ice_resist_id;
    extern uint32_t const stat_poison_resist_id;
    extern uint32_t const stat_disease_resist_id;
    extern uint32_t const stat_action_speed_id;

    extern uint32_t const gear_slot_armor_id;
    extern uint32_t const gear_slot_robe1_id;
    extern uint32_t const gear_slot_robe2_id;
    extern uint32_t const gear_slot_ring1_id;
    extern uint32_t const gear_slot_ring2_id;
    extern uint32_t const gear_slot_head_id;
    extern uint32_t const gear_slot_neck_id;
    extern uint32_t const gear_slot_waist_id;
    extern uint32_t const gear_slot_wrist_id;
    extern uint32_t const gear_slot_hands_id;
    extern uint32_t const gear_slot_feet_id;
    extern uint32_t const gear_slot_ear_id;

    extern array<uint32_t const, 42> const stat_name_ids;
    extern array<uint32_t const, 12> const slot_name_ids;

    extern ibh_flat_map<string, uint32_t> stat_name_to_id_mapper;
    extern ibh_flat_map<string, uint32_t> slot_name_to_id_mapper;
}