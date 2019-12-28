/*
    IdleBossHunter
    Copyright (C) 2019 Michael de Lang

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

using namespace std;

namespace ibh {
    struct stat_component {
        string name;
        int64_t value;

        stat_component(string name, int64_t value) : name(move(name)), value(value) {}
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

    extern array<string const, 42> const stat_names;
    extern array<string const, 14> const slot_names;
}