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

#include <catch2/catch.hpp>
#include <ecs/battle_system.h>
#include <game_queue_messages/messages.h>

using namespace std;
using namespace ibh;
int64_t battle_turn(pc_component &pc, ibh_flat_map<uint32_t, int64_t> &attacker, ibh_flat_map<uint32_t, int64_t> &defender, bool &attacker_dead, bool &defender_dead, string const &attacker_name, string const &defender_name);
void set_hp_mp(pc_component &pc, ibh_flat_map<uint32_t, int64_t> &stats);
void simulate_battle(pc_component &pc, entt::registry &es, ibh::outward_queues *outward_queue);

TEST_CASE("set hp/mp test") {
    pc_component pc{};
    ibh_flat_map<uint32_t, int64_t> stats;
    stats.emplace(stat_hp_id, 10);
    stats.emplace(stat_mp_id, 10);
    stats.emplace(stat_str_id, 10);
    stats.emplace(stat_vit_id, 10);
    set_hp_mp(pc, stats);
    auto hp = stats.find(stat_hp_id);
    auto max_hp = stats.find(stat_max_hp_id);
    auto mp = stats.find(stat_mp_id);
    auto max_mp = stats.find(stat_max_mp_id);
    REQUIRE(hp->second == 120);
    REQUIRE(max_hp->second == 120);
    REQUIRE(mp->second == 100);
    REQUIRE(max_mp->second == 100);
}

TEST_CASE("battle_turn dmg test") {
    pc_component pc{};
    bool attacker_dead = false;
    bool defender_dead = false;
    string attacker_name = "a";
    string defender_name = "b";
    for(uint32_t i = 0; i < 100; i++) {
        ibh_flat_map<uint32_t, int64_t> stats_attacker;
        ibh_flat_map<uint32_t, int64_t> stats_defender;
        stats_attacker.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_hp_id, 100'000'000});
        stats_attacker.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_str_id, i*10});
        stats_attacker.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_agi_id, 1'000});
        stats_attacker.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_vit_id, 10});
        stats_defender.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_hp_id, 100'000'000});
        stats_defender.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_str_id, 10});
        stats_defender.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_agi_id, 10});
        stats_defender.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat_vit_id, 10});
        for(uint32_t x = 0; x < 100; x++) {
            auto dmg = battle_turn(pc, stats_attacker, stats_defender, attacker_dead, defender_dead, attacker_name,
                                   defender_name);
            if(dmg != -1) {
                REQUIRE(dmg >= floor(i * 9. * i * 9. / (i * 9. + 11.)));
                REQUIRE(dmg <= i * 11);
            }
            REQUIRE(attacker_dead == false);
            REQUIRE(defender_dead == false);
        }
    }
}