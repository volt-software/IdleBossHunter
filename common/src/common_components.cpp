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

#include "common_components.h"
#include <spdlog/spdlog.h>

namespace ibh {
    string const stat_str = "str";
    string const stat_dex = "dex";
    string const stat_agi = "agi";
    string const stat_int = "int";
    string const stat_spd = "spd";
    string const stat_vit = "vit";
    string const stat_luk = "luk";
    string const stat_cha = "cha";
    string const stat_con = "con";
    string const stat_move = "move";
    string const stat_hpregen = "hpregen";
    string const stat_mpregen = "mpregen";
    string const stat_hp = "hp";
    string const stat_max_hp = "maxhp";
    string const stat_mp = "mp";
    string const stat_max_mp = "maxmp";
    string const stat_xp = "xp";
    string const stat_gold = "gold";
    string const stat_hweapon_damage_rolls = "weaponDamageRolls";
    string const stat_weapon_armor_class = "weaponArmorClass";
    string const stat_armor_class = "armorClass";
    string const stat_accuracy = "accuracy";
    string const stat_offense = "offense";
    string const stat_defense = "defense";
    string const stat_stealth = "stealth";
    string const stat_perception = "perception";
    string const stat_physical_damage_boost = "physicalDamageBoost";
    string const stat_magical_damage_boost = "magicalDamageBoost";
    string const stat_healing_boost = "healingBoost";
    string const stat_physical_damage_reflect = "physicalDamageReflect";
    string const stat_magical_damage_reflect = "magicalDamageReflect";
    string const stat_mitigation = "mitigation";
    string const stat_magical_resist = "magicalResist";
    string const stat_physical_resist = "physicalResist";
    string const stat_necrotic_resist = "necroticResist";
    string const stat_energy_resist = "energyResist";
    string const stat_water_resist = "waterResist";
    string const stat_fire_resist = "fireResist";
    string const stat_ice_resist = "iceResist";
    string const stat_poison_resist = "poisonResist";
    string const stat_disease_resist = "diseaseResist";
    string const stat_action_speed = "actionSpeed";
    /*, "damageFactor"s TODO damage factor is a double :< */

    string const gear_slot_right_hand = "rightHand";
    string const gear_slot_left_hand = "leftHand";
    string const gear_slot_armor = "armor";
    string const gear_slot_robe1 = "robe1";
    string const gear_slot_robe2 = "robe2";
    string const gear_slot_ring1 = "ring1";
    string const gear_slot_ring2 = "ring2";
    string const gear_slot_head = "head";
    string const gear_slot_next = "next";
    string const gear_slot_waist = "waist";
    string const gear_slot_wrist = "wrist";
    string const gear_slot_hands = "hands";
    string const gear_slot_feet = "feet";
    string const gear_slot_ear = "ear";

    array<string const, 42> const stat_names = {stat_str, stat_dex, stat_agi, stat_int, stat_spd, stat_vit, stat_luk, stat_cha, stat_con, stat_move,
                                                stat_hpregen, stat_mpregen, stat_hp, stat_mp, stat_max_hp, stat_max_mp, stat_xp, stat_gold, stat_hweapon_damage_rolls, stat_weapon_armor_class, stat_armor_class,
                                                stat_accuracy, stat_offense, stat_defense, stat_stealth, stat_perception, stat_physical_damage_boost, stat_magical_damage_boost,
                                                stat_healing_boost, stat_physical_damage_reflect, stat_magical_damage_reflect, stat_mitigation, stat_magical_resist,
                                                stat_physical_resist, stat_necrotic_resist, stat_energy_resist, stat_water_resist, stat_fire_resist, stat_ice_resist,
                                                stat_poison_resist, stat_disease_resist, stat_action_speed };

    array<string const, 14> const slot_names = {gear_slot_right_hand, gear_slot_left_hand, gear_slot_armor, gear_slot_robe1, gear_slot_robe2, gear_slot_ring1,
                                                gear_slot_ring2, gear_slot_head, gear_slot_next, gear_slot_waist, gear_slot_wrist, gear_slot_hands,
                                                gear_slot_feet, gear_slot_ear};

    void test() {
        for(auto &stat: stat_names) {
            spdlog::info("{}", stat);
        }
    }
}
