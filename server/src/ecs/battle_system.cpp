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
#include <uws_thread.h>
#include <messages/user_access/character_select_response.h>
#include <messages/battle/new_battle_response.h>
#include <messages/battle/level_up_response.h>
#include <messages/battle/battle_update_response.h>
#include <messages/battle/battle_finished_response.h>
#include "battle_system.h"
#include "random_helper.h"
#include "on_leaving_scope.h"
#include "macros.h"

using namespace std;
using namespace ibh;

#define GET_STAT(stats, var_name, stat_name) \
    auto var_name = stats.find(stat_name); \
    if(var_name == end(stats)) { \
        spdlog::error("[{}] missing " #var_name " for pc {} battle {}", __FUNCTION__, pc.id, pc.battle->monster_name); \
        throw std::runtime_error("missing " #var_name); \
    }

#define GET_STAT_DEFAULT(stats, var_name, stat_name, def) \
    auto var_name = stats.find(stat_name); \
    if(var_name == end(stats)) { \
        /*spdlog::warn("[{}] missing " #var_name " for pc {} battle {}, inserted default {}", __FUNCTION__, pc.id, pc.battle->monster_name, def);*/ \
        stats.insert(ibh_flat_map<string, stat_component>::value_type{stat_name, stat_component{stat_name, def}}); \
        var_name = stats.find(stat_name); \
    }

int64_t battle_turn(pc_component &pc, ibh_flat_map<string, stat_component> &attacker, ibh_flat_map<string, stat_component> &defender, bool &attacker_dead, bool &defender_dead, string const &attacker_name, string const &defender_name) {
    GET_STAT(attacker, attacker_str, stat_str);
    GET_STAT(attacker, attacker_agi, stat_agi);
    GET_STAT(defender, defender_str, stat_str);
    GET_STAT(defender, defender_agi, stat_agi);
    GET_STAT(defender, defender_hp, stat_hp);

    auto attacker_dmg = ibh::random.generate_single(attacker_str->second.value * 0.9, attacker_str->second.value*1.1);
    auto defender_def = ibh::random.generate_single(defender_str->second.value * 0.9, defender_str->second.value*1.1);
    int64_t dmg = round(max(attacker_dmg * attacker_dmg / (attacker_dmg + defender_def), 0.));
    auto attacker_hit = ibh::random.generate_single(0L, attacker_agi->second.value);
    auto defender_hit = ibh::random.generate_single(0L, defender_agi->second.value);

    if(attacker_hit >= defender_hit) {
        defender_hp->second.value -= dmg;
        spdlog::trace("[{}] {} attacked {} for {} dmg. {} has {} health left. {} {} {} {}", __FUNCTION__, attacker_name, defender_name, dmg, defender_name, defender_hp->second.value, attacker_dmg, defender_def, attacker_str->second.value, defender_str->second.value);
    } else {
        spdlog::trace("[{}] {} tried to attack {} but missed. {} has {} health left.", __FUNCTION__, attacker_name, defender_name, defender_name, defender_hp->second.value);
        dmg = -1;
    }

    if(defender_hp->second.value <= 0) {
        spdlog::trace("[{}] {} died", __FUNCTION__, defender_name);
        defender_dead = true;
    }

    return dmg;
}

void set_hp_mp(pc_component &pc, ibh_flat_map<string, stat_component> &stats) {
    GET_STAT(stats, hp, stat_hp);
    GET_STAT_DEFAULT(stats, max_hp, stat_max_hp, 1);
    GET_STAT(stats, mp, stat_mp);
    GET_STAT_DEFAULT(stats, max_mp, stat_max_mp, 1);
    GET_STAT(stats, str, stat_str);
    GET_STAT(stats, vit, stat_vit);

    hp->second.value = str->second.value * 10 + vit->second.value * 2;
    max_hp->second.value = hp->second.value;
    mp->second.value = vit->second.value * 10;
    max_mp->second.value = mp->second.value;
}

void simulate_battle(pc_component &pc, entt::registry &es, vector<monster_definition_component> monsters, vector<monster_special_definition_component> monster_specials, outward_queues *outward_queue) {
    if(!pc.battle) {
        auto definition = ibh::random.generate_single(0UL, monsters.size()-1UL);
        auto special = ibh::random.generate_single(-static_cast<int64_t>(monster_specials.size()), static_cast<int64_t>(monster_specials.size())-1L);
        auto level = ibh::random.generate_single(max(static_cast<int64_t>(pc.level)-2L, 0L), static_cast<int64_t>(pc.level)+2L);
        monster_definition_component &mob_def = monsters[definition];
        monster_special_definition_component &special_def = monster_specials[abs(special)];
        ibh_flat_map<string, stat_component> mob_stats;

        for(auto& stat_name : stat_names) {
            auto stat_it = mob_def.stats.find(stat_name);
            if(stat_it == end(mob_def.stats)) {
                //spdlog::error("[{}] couldn't find stat {}", __FUNCTION__, stat_name);
                continue;
            }
            double value = level * 5 * stat_it->second.value / 100.;
            if(special >= 0) {
                auto special_stat_it = special_def.stats.find(stat_name);
                if(special_stat_it != end(special_def.stats)) {
                    value *= special_stat_it->second.value / 100.;
                }
            }

            value = ibh::random.generate_single(max(value*0.95, 0.), value*1.05);
            mob_stats.insert(ibh_flat_map<string, stat_component>::value_type{stat_name, stat_component{stat_name, static_cast<int64_t>(round(value))}});
        }

        string name = mob_def.name;
        if(special >= 0) {
            name += " " + special_def.name;
        }
        pc.battle = make_optional<battle_component>(mob_def.name, level, move(mob_stats));

        // mob setup
        set_hp_mp(pc, pc.battle->monster_stats);

        // pc setup
        for(auto &stat_name : stat_names) {
            auto stat = pc.stats.find(stat_name);

            if(stat == end(pc.stats)) {
                continue;
            }

            pc.battle->total_player_stats.insert(ibh_flat_map<string, stat_component>::value_type{stat_name, stat->second});
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

        if(pc.connection_id > 0) {
            auto mob_hp = pc.battle->monster_stats.find(stat_hp);
            auto new_battle_msg = make_unique<new_battle_response>(name, level, mob_hp->second.value);
            outward_queue->enqueue({pc.connection_id, move(new_battle_msg)});
        }
    }

    if(!pc.battle) {
        spdlog::warn("[{}] Couldn't find appropriate monster to battle for pc {} level {}", __FUNCTION__, pc.id, pc.level);
        return;
    }

#ifdef BATTLE_EXTREME_LOGGING
        for(auto &mob_stat : pc.battle->monster_stats) {
            spdlog::info("[{}] available stat for monster: {} - {} - {}", __FUNCTION__, mob_stat.first, mob_stat.second.name, mob_stat.second.value);
        }
#endif

    GET_STAT(pc.battle->monster_stats, mob_spd_iter, stat_spd);
    GET_STAT(pc.battle->total_player_stats, plyr_spd_iter, stat_spd);
    int64_t mob_spd = mob_spd_iter->second.value;
    int64_t plyr_spd = plyr_spd_iter->second.value;
    uint64_t player_turns = 0;
    uint64_t mob_turns = 0;
    uint64_t player_dmg = 0;
    uint64_t mob_dmg = 0;
    uint64_t player_hits = 0;
    uint64_t mob_hits = 0;
    bool mob_dead = false;
    bool plyr_dead = false;

    if(mob_spd > plyr_spd) {
        while(mob_spd > plyr_spd) {
            mob_turns++;
            mob_spd -= plyr_spd;

            auto dmg = battle_turn(pc, pc.battle->monster_stats, pc.battle->total_player_stats, mob_dead, plyr_dead, pc.battle->monster_name, pc.name);
            if(dmg >= 0) {
                mob_hits++;
                mob_dmg += dmg;
            }

            if(mob_dead || plyr_dead) {
                break;
            }
        }

        if(!mob_dead && !plyr_dead) {
            player_turns++;
            auto dmg = battle_turn(pc, pc.battle->total_player_stats, pc.battle->monster_stats, plyr_dead, mob_dead, pc.name, pc.battle->monster_name);
            if(dmg >= 0) {
                player_hits++;
                player_dmg += dmg;
            }
        }
    } else {
        while(plyr_spd > mob_spd) {
            player_turns++;
            plyr_spd -= mob_spd;

            auto dmg = battle_turn(pc, pc.battle->total_player_stats, pc.battle->monster_stats, plyr_dead, mob_dead, pc.name, pc.battle->monster_name);
            if(dmg >= 0) {
                player_hits++;
                player_dmg += dmg;
            }

            if(mob_dead || plyr_dead) {
                break;
            }
        }

        if(!mob_dead && !plyr_dead) {
            mob_turns++;
            auto dmg = battle_turn(pc, pc.battle->monster_stats, pc.battle->total_player_stats, mob_dead, plyr_dead, pc.battle->monster_name, pc.name);
            if(dmg >= 0) {
                mob_hits++;
                mob_dmg += dmg;
            }
        }
    }

    if(mob_dead) {
        spdlog::trace("[{}] pc killed mob {}", __FUNCTION__, pc.name, pc.battle->monster_name);
        GET_STAT(pc.battle->monster_stats, mob_xp, stat_xp);
        GET_STAT(pc.battle->monster_stats, mob_gold, stat_gold);
        GET_STAT(pc.stats, plyr_xp, stat_xp);
        GET_STAT(pc.stats, plyr_gold, stat_gold);
        plyr_xp->second.value += mob_xp->second.value;
        plyr_gold->second.value += mob_gold->second.value;

        auto level_calc = [](uint64_t level) { return 100+pow(level, 2); };
        auto level_threshold = level_calc(pc.level);
        if(plyr_xp->second.value >= level_threshold) {
            plyr_xp->second.value -= level_threshold;
            pc.level++;
            ibh_flat_map<string, stat_component> stats;

            for(auto &race : select_response.races) {
                if(race.name != pc.race) {
                    continue;
                }
                if(pc.connection_id > 0) {
                    stats.reserve(race.level_stat_mods.size());
                }

                for(auto &extra_stat : race.level_stat_mods) {
                    auto stat_it = pc.stats.find(extra_stat.name);
                    
                    if(stat_it == end(pc.stats)) {
                        spdlog::error("[{}] missing stat {} for pc {} - {}", __FUNCTION__, extra_stat.name, pc.name, pc.id);
                        continue;
                    }
                    stat_it->second.value += extra_stat.value;
                    if(pc.connection_id > 0) {
                        stats.insert(ibh_flat_map<string, stat_component>::value_type{extra_stat.name,
                                                                                      stat_component{extra_stat.name,
                                                                                                     extra_stat.value}});
                    }
                }

                break;
            }

            for(auto &c : select_response.classes) {
                if(c.name != pc._class) {
                    continue;
                }

                for(auto &extra_stat : c.stat_mods) {
                    auto stat_it = pc.stats.find(extra_stat.name);

                    if(stat_it == end(pc.stats)) {
                        spdlog::error("[{}] missing stat {} for pc {} - {}", __FUNCTION__, extra_stat.name, pc.name, pc.id);
                        continue;
                    }
                    stat_it->second.value += extra_stat.value;
                    if(pc.connection_id > 0) {
                        auto msg_stats_it = stats.find(extra_stat.name);
                        msg_stats_it->second.value += extra_stat.value;
                    }
                }

                break;
            }
            if(pc.connection_id > 0) {
                auto level_up_msg = make_unique<level_up_response>(move(stats), level_calc(pc.level),
                                                                   level_calc(pc.level) - plyr_xp->second.value);
                outward_queue->enqueue({pc.connection_id, move(level_up_msg)});
            }
            spdlog::trace("[{}] pc {} level up", __FUNCTION__, pc.name);
        }
        if(pc.connection_id > 0) {
            auto finished_msg = make_unique<battle_finished_response>(true, false, mob_xp->second.value, mob_gold->second.value);
            outward_queue->enqueue({pc.connection_id, move(finished_msg)});
        }

        pc.battle.reset();
    } else if (plyr_dead) {
        spdlog::trace("[{}] pc {} died against mob {}", __FUNCTION__, pc.name, pc.battle->monster_name);
        if(pc.connection_id > 0) {
            auto finished_msg = make_unique<battle_finished_response>(false, true, 0, 0);
            outward_queue->enqueue({pc.connection_id, move(finished_msg)});
        }
        pc.battle.reset();
    } else {
        spdlog::trace("[{}] pc fought against mob {}", __FUNCTION__, pc.name, pc.battle->monster_name);
        if(pc.connection_id > 0) {
            auto update_msg = make_unique<battle_update_response>(mob_turns, player_turns, mob_hits, player_hits, mob_dmg, player_dmg);
            outward_queue->enqueue({pc.connection_id, move(update_msg)});
        }
    }
}

void ibh::battle_system::do_tick(entt::registry &es) {
    _tick_count++;

    if(_tick_count < _every_n_ticks) {
        return;
    }

    _tick_count = 0;

    MEASURE_TIME_OF_FUNCTION()
    auto pc_view = es.view<pc_component>();
    for(auto pc_entity : pc_view) {
        pc_component &pc = pc_view.get<pc_component>(pc_entity);
        simulate_battle(pc, es, _monsters, _monster_specials, _outward_queue);
    }
}
