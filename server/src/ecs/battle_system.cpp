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

#include <execution>
#include <spdlog/spdlog.h>
#include <magic_enum.hpp>
#include <websocket_thread.h>
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

[[nodiscard]]
int64_t battle_turn(pc_component &pc, ibh_flat_map<uint32_t, int64_t> &attacker, ibh_flat_map<uint32_t, int64_t> &defender, bool &attacker_dead, bool &defender_dead, string const &attacker_name, string const &defender_name) {
    auto &attacker_str = get_stat(attacker, stat_str_id);
    auto &attacker_agi = get_stat(attacker, stat_agi_id);
    auto &defender_str = get_stat(defender, stat_str_id);
    auto &defender_agi = get_stat(defender, stat_agi_id);
    auto &defender_hp = get_stat(defender, stat_hp_id);

    auto attacker_dmg = ibh::random.generate_single(attacker_str * 0.9, attacker_str*1.1);
    auto defender_def = ibh::random.generate_single(defender_str * 0.9, defender_str*1.1);
    int64_t dmg = round(max(attacker_dmg * attacker_dmg / (attacker_dmg + defender_def), 0.));
    auto attacker_hit = ibh::random.generate_single(0L, attacker_agi);
    auto defender_hit = ibh::random.generate_single(0L, defender_agi);

    if(dmg <= -922337203685477580L){
        spdlog::error("[{}] something went wrong?", __FUNCTION__);
    }

    if(attacker_hit >= defender_hit) {
        defender_hp -= dmg;
        spdlog::trace("[{}] {} attacked {} for {} dmg. {} has {} health left. {} {} {} {}", __FUNCTION__, attacker_name, defender_name, dmg, defender_name, defender_hp, attacker_dmg, defender_def, attacker_str, defender_str);
    } else {
        spdlog::trace("[{}] {} tried to attack {} but missed. {} has {} health left.", __FUNCTION__, attacker_name, defender_name, defender_name, defender_hp);
        dmg = -1;
    }

    if(defender_hp <= 0) {
        spdlog::trace("[{}] {} died", __FUNCTION__, defender_name);
        defender_dead = true;
    }

    return dmg;
}

void set_hp_mp(pc_component &pc, ibh_flat_map<uint32_t, int64_t> &stats) {
    auto &hp = get_stat(stats, stat_hp_id);
    auto &mp = get_stat(stats, stat_mp_id);
    auto &str = get_stat(stats, stat_str_id);
    auto &vit = get_stat(stats, stat_vit_id);
    auto &max_hp = get_stat_or_initialize_default(stats, stat_max_hp_id, hp);
    auto &max_mp = get_stat_or_initialize_default(stats, stat_max_mp_id, mp);

    hp = str * 10 + vit * 2;
    max_hp = hp;
    mp = vit * 10;
    max_mp = mp;
}

void simulate_battle(pc_component &pc, battle_component &bc, entt::registry &es, outward_queues &outward_queue) {
    if(bc.done) {
        auto mob_view = es.view<monster_definition_component>();
        auto mob_special_view = es.view<monster_special_definition_component>();

        if(mob_view.empty() || mob_special_view.empty()) {
            throw std::runtime_error("missing mobs/specials"); \
        }

        auto definition = ibh::random.generate_single(0UL, mob_view.size()-1UL);
        auto special = ibh::random.generate_single(-static_cast<int64_t>(mob_special_view.size()), static_cast<int64_t>(mob_special_view.size())-1L);
        auto level = ibh::random.generate_single(max(static_cast<int64_t>(pc.level)-2L, 0L), static_cast<int64_t>(pc.level)+2L);
        monster_definition_component &mob_def = es.get<monster_definition_component>(*(mob_view.begin() + definition));
        ibh_flat_map<uint32_t, int64_t> mob_stats;
        mob_stats.reserve(stat_name_ids.size());
        string name = mob_def.name;

        for(auto& mob_stat_id : stat_name_ids) {
            auto stat_it = mob_def.stats.find(mob_stat_id);
            if(stat_it == end(mob_def.stats)) {
                //spdlog::error("[{}] couldn't find stat {}", __FUNCTION__, stat_name);
                continue;
            }

            double value = level * 6 * stat_it->second / 100.;
            if(special >= 0) {
                monster_special_definition_component &special_def = es.get<monster_special_definition_component>(*(mob_special_view.begin() + special));
                auto special_stat_it = special_def.stats.find(mob_stat_id);
                if(special_stat_it != end(special_def.stats)) {
                    value *= special_stat_it->second / 100.;
                }
            }

            value = ibh::random.generate_single(max(value*0.95, 0.), value*1.05);
            mob_stats.emplace(mob_stat_id, static_cast<int64_t>(round(value)));
        }
        if(special >= 0) {
            monster_special_definition_component &special_def = es.get<monster_special_definition_component>(*(mob_special_view.begin() + special));
            name += " " + special_def.name;
        }
        bc = battle_component(name, level, move(mob_stats));

        // mob setup
        set_hp_mp(pc, bc.monster_stats);

        // pc setup
        bc.total_player_stats.reserve(stat_name_ids.size());
        for(auto &stat_id : stat_name_ids) {
            auto stat = pc.stats.find(stat_id);

            if(stat == end(pc.stats)) {
                continue;
            }

            bc.total_player_stats.emplace(stat_id, stat->second);
        }

        for(auto &slot_id : slot_name_ids) {
            auto item = pc.equipped_items.find(slot_id);

            if(item == end(pc.equipped_items)) {
                continue;
            }

            for(auto &stat : item->second.stats) {
                auto &pc_stat = get_stat(bc.total_player_stats, stat.stat_id);
                pc_stat += stat.value;
            }
        }
        set_hp_mp(pc, bc.total_player_stats);
        bc.done = false;

        if(pc.connection_id > 0) {
            auto mob_hp = bc.monster_stats.find(stat_hp_id);
            auto mob_max_hp = bc.monster_stats.find(stat_max_hp_id);
            auto player_hp = bc.total_player_stats.find(stat_hp_id);
            auto player_max_hp = bc.total_player_stats.find(stat_max_hp_id);
            auto new_battle_msg = make_unique<new_battle_response>(name, level, mob_hp->second, mob_max_hp->second, player_hp->second, player_max_hp->second);
            outward_queue.enqueue_tokenless(outward_message{pc.connection_id, move(new_battle_msg)});
        }
    }

    if(bc.done) {
        spdlog::warn("[{}] Couldn't find appropriate monster to battle for pc {} level {}", __FUNCTION__, pc.id, pc.level);
        return;
    }

#ifdef BATTLE_EXTREME_LOGGING
        for(auto &mob_stat : bc.monster_stats) {
            spdlog::info("[{}] available stat for monster: {} - {} - {}", __FUNCTION__, mob_stat.first, mob_stat.second.name, mob_stat.second.value);
        }
#endif

    auto mob_spd = get_stat(bc.monster_stats, stat_spd_id);
    auto plyr_spd = get_stat(bc.total_player_stats, stat_spd_id);
    uint64_t player_turns = 0;
    uint64_t mob_turns = 0;
    uint64_t player_dmg_to_mob = 0;
    uint64_t mob_dmg_to_player = 0;
    uint64_t player_hits = 0;
    uint64_t mob_hits = 0;
    bool mob_dead = false;
    bool plyr_dead = false;

    if(mob_spd > plyr_spd) {
        while(mob_spd > plyr_spd) {
            mob_turns++;
            mob_spd -= plyr_spd;

            auto dmg = battle_turn(pc, bc.monster_stats, bc.total_player_stats, mob_dead, plyr_dead, bc.monster_name, pc.name);
            if(dmg >= 0) {
                mob_hits++;
                mob_dmg_to_player += dmg;
            }

            if(mob_dead || plyr_dead) {
                break;
            }
        }

        if(!mob_dead && !plyr_dead) {
            player_turns++;
            auto dmg = battle_turn(pc, bc.total_player_stats, bc.monster_stats, plyr_dead, mob_dead, pc.name, bc.monster_name);
            if(dmg >= 0) {
                player_hits++;
                player_dmg_to_mob += dmg;
            }
        }
    } else {
        while(plyr_spd > mob_spd) {
            player_turns++;
            plyr_spd -= mob_spd;

            auto dmg = battle_turn(pc, bc.total_player_stats, bc.monster_stats, plyr_dead, mob_dead, pc.name, bc.monster_name);
            if(dmg >= 0) {
                player_hits++;
                player_dmg_to_mob += dmg;
            }

            if(mob_dead || plyr_dead) {
                break;
            }
        }

        if(!mob_dead && !plyr_dead) {
            mob_turns++;
            auto dmg = battle_turn(pc, bc.monster_stats, bc.total_player_stats, mob_dead, plyr_dead, bc.monster_name, pc.name);
            if(dmg >= 0) {
                mob_hits++;
                mob_dmg_to_player += dmg;
            }
        }
    }

    if(mob_dead) {
        spdlog::trace("[{}] pc {} killed mob {}", __FUNCTION__, pc.name, bc.monster_name);
        auto &mob_xp = get_stat(bc.monster_stats, stat_xp_id);
        auto &mob_gold = get_stat(bc.monster_stats, stat_gold_id);
        auto &plyr_xp = get_stat(pc.stats, stat_xp_id);
        auto &plyr_gold = get_stat(pc.stats, stat_gold_id);
        plyr_xp += mob_xp;
        plyr_gold += mob_gold;

        auto level_calc = [](uint64_t level) { return 50*pow(2, level); };
        auto level_threshold = level_calc(pc.level);
        if(plyr_xp >= level_threshold) {
            plyr_xp -= level_threshold;
            pc.level++;
            ibh_flat_map<uint64_t, stat_component> stats;

            for(auto &race : select_response.races) {
                if(race.name != pc.race) {
                    continue;
                }
                if(pc.connection_id > 0) {
                    stats.reserve(race.level_stat_mods.size());
                }

                for(auto &extra_stat : race.level_stat_mods) {
                    auto stat_it = pc.stats.find(extra_stat.stat_id);

                    if(stat_it == end(pc.stats)) {
                        spdlog::error("[{}] missing stat {} for pc {} - {}", __FUNCTION__, extra_stat.stat_id, pc.name, pc.id);
                        continue;
                    }
                    stat_it->second += extra_stat.value;
                    if(pc.connection_id > 0) {
                        stats.emplace(extra_stat.stat_id, stat_component{extra_stat.stat_id, extra_stat.value});
                    }
                }

                break;
            }

            for(auto &c : select_response.classes) {
                if(c.name != pc._class) {
                    continue;
                }

                for(auto &extra_stat : c.stat_mods) {
                    auto stat_it = pc.stats.find(extra_stat.stat_id);

                    if(stat_it == end(pc.stats)) {
                        spdlog::error("[{}] missing stat {} for pc {} - {}", __FUNCTION__, extra_stat.stat_id, pc.name, pc.id);
                        continue;
                    }
                    stat_it->second += extra_stat.value;
                    if(pc.connection_id > 0) {
                        auto msg_stats_it = stats.find(extra_stat.stat_id);
                        msg_stats_it->second.value += extra_stat.value;
                    }
                }

                break;
            }
            if(pc.connection_id > 0) {
                auto level_up_msg = make_unique<level_up_response>(move(stats), level_calc(pc.level),
                                                                   level_calc(pc.level) - plyr_xp);
                outward_queue.enqueue_tokenless(outward_message{pc.connection_id, move(level_up_msg)});
            }
            spdlog::trace("[{}] pc {} level up", __FUNCTION__, pc.name);
        }
        if(pc.connection_id > 0) {
            auto finished_msg = make_unique<battle_finished_response>(true, false, mob_xp, mob_gold);
            outward_queue.enqueue_tokenless(outward_message{pc.connection_id, move(finished_msg)});
        }

        bc.done = true;
    } else if (plyr_dead) {
        spdlog::trace("[{}] pc {} died against mob {}", __FUNCTION__, pc.name, bc.monster_name);
        if(pc.connection_id > 0) {
            auto finished_msg = make_unique<battle_finished_response>(false, true, 0, 0);
            outward_queue.enqueue_tokenless(outward_message{pc.connection_id, move(finished_msg)});
        }
        bc.done = true;
    } else {
        spdlog::trace("[{}] pc {} fought against mob {}", __FUNCTION__, pc.name, bc.monster_name);
        if(pc.connection_id > 0) {
            auto update_msg = make_unique<battle_update_response>(mob_turns, player_turns, mob_hits, player_hits, mob_dmg_to_player, player_dmg_to_mob);
            outward_queue.enqueue_tokenless(outward_message{pc.connection_id, move(update_msg)});
        }
    }
}

void battle_system::do_tick(entt::registry &es) {
    _tick_count++;

    if(_tick_count < _every_n_ticks) {
        return;
    }

    _tick_count = 0;

    MEASURE_TIME_OF_FUNCTION(info);
    auto pc_group = es.group<battle_component>(entt::get<pc_component>);
    for_each(execution::par_unseq, begin(pc_group), end(pc_group), [&es, &outward_queue = _outward_queue, &pc_group](auto entity){
        auto [pc, bc] = pc_group.template get<pc_component, battle_component>(entity);
        simulate_battle(pc, bc, es, outward_queue);
    });
}
