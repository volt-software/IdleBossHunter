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

    string const gear_slot_armor = "armor";
    string const gear_slot_robe1 = "robe1";
    string const gear_slot_robe2 = "robe2";
    string const gear_slot_ring1 = "ring1";
    string const gear_slot_ring2 = "ring2";
    string const gear_slot_head = "head";
    string const gear_slot_neck = "neck";
    string const gear_slot_waist = "waist";
    string const gear_slot_wrist = "wrist";
    string const gear_slot_hands = "hands";
    string const gear_slot_feet = "feet";
    string const gear_slot_ear = "ear";

    string const company_stat_tax = "tax";
    string const company_stat_str_bonus = "str_bonus";
    string const company_stat_agi_bonus = "agi_bonus";
    string const company_stat_int_bonus = "int_bonus";
    string const company_stat_vit_bonus = "vit_bonus";
    string const company_stat_spd_bonus = "spd_bonus";
    string const company_stat_gold_bonus = "gold_bonus";
    string const company_stat_gold = "gold";
    string const company_stat_xp_bonus = "xp_bonus";

    array<string const, 42> const stat_names = {stat_str, stat_dex, stat_agi, stat_int, stat_spd, stat_vit, stat_luk, stat_cha, stat_con, stat_move,
                                                stat_hpregen, stat_mpregen, stat_hp, stat_mp, stat_max_hp, stat_max_mp, stat_xp, stat_gold, stat_hweapon_damage_rolls, stat_weapon_armor_class, stat_armor_class,
                                                stat_accuracy, stat_offense, stat_defense, stat_stealth, stat_perception, stat_physical_damage_boost, stat_magical_damage_boost,
                                                stat_healing_boost, stat_physical_damage_reflect, stat_magical_damage_reflect, stat_mitigation, stat_magical_resist,
                                                stat_physical_resist, stat_necrotic_resist, stat_energy_resist, stat_water_resist, stat_fire_resist, stat_ice_resist,
                                                stat_poison_resist, stat_disease_resist, stat_action_speed };

    array<string const, 12> const slot_names = {gear_slot_armor, gear_slot_robe1, gear_slot_robe2, gear_slot_ring1,
                                                gear_slot_ring2, gear_slot_head, gear_slot_neck, gear_slot_waist, gear_slot_wrist, gear_slot_hands,
                                                gear_slot_feet, gear_slot_ear};

    array<string const, 9> const company_stat_names = {company_stat_tax, company_stat_str_bonus, company_stat_agi_bonus, company_stat_int_bonus,
                                                     company_stat_vit_bonus, company_stat_spd_bonus, company_stat_gold_bonus, company_stat_gold, company_stat_xp_bonus};


    /*, "damageFactor"s TODO damage factor is a double :< */



    array<uint32_t const, 42> const stat_name_ids = {stat_str_id, stat_dex_id, stat_agi_id, stat_int_id, stat_spd_id, stat_vit_id, stat_luk_id, stat_cha_id, stat_con_id, stat_move_id,
                                                stat_hpregen_id, stat_mpregen_id, stat_hp_id, stat_mp_id, stat_max_hp_id, stat_max_mp_id, stat_xp_id, stat_gold_id, stat_hweapon_damage_rolls_id, stat_weapon_armor_class_id, stat_armor_class_id,
                                                stat_accuracy_id, stat_offense_id, stat_defense_id, stat_stealth_id, stat_perception_id, stat_physical_damage_boost_id, stat_magical_damage_boost_id,
                                                stat_healing_boost_id, stat_physical_damage_reflect_id, stat_magical_damage_reflect_id, stat_mitigation_id, stat_magical_resist_id,
                                                stat_physical_resist_id, stat_necrotic_resist_id, stat_energy_resist_id, stat_water_resist_id, stat_fire_resist_id, stat_ice_resist_id,
                                                stat_poison_resist_id, stat_disease_resist_id, stat_action_speed_id };

    array<uint32_t const, 12> const slot_name_ids = {gear_slot_armor_id, gear_slot_robe1_id, gear_slot_robe2_id, gear_slot_ring1_id,
                                                     gear_slot_ring2_id, gear_slot_head_id, gear_slot_neck_id, gear_slot_waist_id, gear_slot_wrist_id, gear_slot_hands_id,
                                                     gear_slot_feet_id, gear_slot_ear_id };

    array<uint32_t const, 9> const company_stat_ids = {company_stat_tax_id, company_stat_str_bonus_id, company_stat_agi_bonus_id, company_stat_int_bonus_id,
                                                       company_stat_vit_bonus_id, company_stat_spd_bonus_id, company_stat_gold_id, company_stat_xp_bonus_id};

    array<uint32_t const, 11> const resource_ids = {resource_wood_id, resource_ore_id, resource_water_id, resource_plants_id, resource_clay_id, resource_gems_id,
                                                    resource_paper_id, resource_ink_id, resource_metal_id, resource_bricks_id, resource_timber_id};

    ibh_flat_map<string, uint32_t> stat_name_to_id_mapper;
    ibh_flat_map<uint32_t, string> stat_id_to_name_mapper;
    ibh_flat_map<string, uint32_t> slot_name_to_id_mapper;
    ibh_flat_map<uint32_t, string> slot_id_to_name_mapper;
    ibh_flat_map<string, uint32_t> company_stat_name_to_id_mapper;
    ibh_flat_map<uint32_t, string> company_stat_id_to_name_mapper;

    void fill_mappers() {
        for(uint32_t i = 0; i < stat_names.size(); i++) {
            stat_name_to_id_mapper.insert({stat_names[i], stat_name_ids[i]});
            stat_id_to_name_mapper.insert({stat_name_ids[i], stat_names[i]});
        }

        for(uint32_t i = 0; i < slot_names.size(); i++) {
            slot_name_to_id_mapper.insert({slot_names[i], slot_name_ids[i]});
            slot_id_to_name_mapper.insert({slot_name_ids[i], slot_names[i]});
        }

        for(uint32_t i = 0; i < company_stat_names.size(); i++) {
            company_stat_name_to_id_mapper.insert({company_stat_names[i], company_stat_ids[i]});
            company_stat_id_to_name_mapper.insert({company_stat_ids[i], company_stat_names[i]});
        }
    }
}
