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

#include <spdlog/spdlog.h>
#include "battle_system.h"
#include "components.h"
#include "random_helper.h"

using namespace std;
using namespace ibh;

#define GET_STAT(stats, var_name, stat_name) \
    auto var_name = stats.find(stat_name); \
    if(var_name == end(stats)) { \
        spdlog::error("[{}] missing " #var_name " for player {} battle {}", __FUNCTION__, pc.id, pc.battle->monster_name); \
        return; \
    }

void battle_turn(pc_component &pc, ibh_flat_map<string, stat_component> &attacker, ibh_flat_map<string, stat_component> &defender, bool &attacker_dead, bool &defender_dead) {
    GET_STAT(attacker, attacker_str, stat_str);
    GET_STAT(attacker, attacker_agi, stat_agi);
    GET_STAT(defender, defender_str, stat_str);
    GET_STAT(defender, defender_agi, stat_agi);
    GET_STAT(defender, defender_hp, stat_hp);

    auto dmg = ibh::random.generate_single(0L, max(attacker_str->second.value - defender_str->second.value, 0L));
    auto attacker_hit = ibh::random.generate_single(0L, attacker_agi->second.value);
    auto defender_hit = ibh::random.generate_single(0L, defender_agi->second.value);

    if(attacker_hit >= defender_hit) {
        defender_hp->second.value -= dmg;
    }

    if(defender_hp->second.value <= 0) {
        defender_dead = true;
    }
}

void set_hp_mp(pc_component &pc, ibh_flat_map<string, stat_component> &stats) {
    GET_STAT(stats, hp, stat_hp);
    GET_STAT(stats, max_hp, stat_max_hp);
    GET_STAT(stats, mp, stat_mp);
    GET_STAT(stats, max_mp, stat_max_mp);
    GET_STAT(stats, str, stat_str);
    GET_STAT(stats, vit, stat_vit);

    hp->second.value = str->second.value * 10 + vit->second.value * 2;
    max_hp->second.value = hp->second.value;
    mp->second.value = vit->second.value * 10;
    max_mp->second.value = mp->second.value;
}

void ibh::battle_system::do_tick(entt::registry &es) {
    _tick_count++;

    if(_tick_count < 10) {
        return;
    }

    auto pc_view = es.view<pc_component>();
    for(auto pc_entity : pc_view) {
        pc_component &pc = pc_view.get<pc_component>(pc_entity);
        if(!pc.battle) {
            auto monster_definition_view = es.view<monster_definition_component>();
            for(auto mob_def_entity : monster_definition_view) {
                monster_definition_component &mob_def = monster_definition_view.get<monster_definition_component>(mob_def_entity);
                if(pc.level >= mob_def.min_level && pc.level <= mob_def.max_level) {
                    pc.battle = make_optional<battle_component>(mob_def.name, mob_def.min_level, mob_def.stats);

                    // mob setup
                    set_hp_mp(pc, pc.battle->monster_stats);

                    // player setup
                    for(auto &stat_name : stat_names) {
                        auto stat = pc.stats.find(stat_name);

                        if(stat == end(pc.stats)) {
                            continue;
                        }

                        pc.battle->total_player_stats.insert(ibh_flat_map<string, stat_component>::value_type{stat_name, stat_component{stat_name, stat->second}});
                    }

                    for(auto &slot_name : slot_names) {
                        auto item = pc.equipped_items.find(slot_name);

                        if(item == end(pc.equipped_items)) {
                            continue;
                        }

                        for(auto &stat : item->second.stats) {
                            GET_STAT(pc.battle->total_player_stats, stat_iter, stat.name);
                            stat_iter->second.value += stat.value;
                        }
                    }
                    set_hp_mp(pc, pc.battle->total_player_stats);
                    break;
                }
            }
        }

        if(!pc.battle) {
            spdlog::warn("[{}] Couldn't find appropriate monster to battle for player {} level {}", __FUNCTION__, pc.id, pc.level);
            continue;
        }

        GET_STAT(pc.battle->monster_stats, mob_spd_iter, stat_spd);
        GET_STAT(pc.battle->total_player_stats, plyr_spd_iter, stat_spd);
        int64_t mob_spd = mob_spd_iter->second.value;
        int64_t plyr_spd = plyr_spd_iter->second.value;
        uint32_t turns = 0;
        bool mob_dead = false;
        bool plyr_dead = false;
        if(mob_spd > plyr_spd) {
            while(mob_spd > plyr_spd) {
                turns++;
                mob_spd -= plyr_spd;

                battle_turn(pc, pc.battle->monster_stats, pc.battle->total_player_stats, mob_dead, plyr_dead);

                if(mob_dead || plyr_dead) {
                    break;
                }
            }

            if(!mob_dead && !plyr_dead) {
                battle_turn(pc, pc.battle->total_player_stats, pc.battle->monster_stats, plyr_dead, mob_dead);
            }
        } else {
            while(plyr_spd > mob_spd) {
                turns++;
                plyr_spd -= mob_spd;

                battle_turn(pc, pc.battle->total_player_stats, pc.battle->monster_stats, plyr_dead, mob_dead);

                if(mob_dead || plyr_dead) {
                    break;
                }
            }

            if(!mob_dead && !plyr_dead) {
                battle_turn(pc, pc.battle->monster_stats, pc.battle->total_player_stats, mob_dead, plyr_dead);
            }
        }

        if(mob_dead) {
            // send message that player won
            GET_STAT(pc.battle->monster_stats, mob_xp, stat_xp);
            GET_STAT(pc.battle->monster_stats, mob_gold, stat_gold);
            GET_STAT(pc.stats, plyr_xp, stat_xp);
            GET_STAT(pc.stats, plyr_gold, stat_gold);
            plyr_xp->second += mob_xp->second.value;
            plyr_gold->second += mob_gold->second.value;

            pc.battle = {};
        } else if (plyr_dead) {
            // send message that player lost
            pc.battle = {};
        } else {
            // send message of battle update
        }
    }
}
