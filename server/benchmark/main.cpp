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
#include <filesystem>
#include <chrono>
#include <game_logic/censor_sensor.h>
#include <sodium.h>
#include <csignal>
#include <rapidjson/writer.h>
#define CEREAL_THREAD_SAFE 1
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

#include "../src/config.h"
#include "../src/config_parsers.h"
#include "benchmark_helpers/startup_helper.h"
#include "../src/working_directory_manipulation.h"
#include <asset_loading/load_assets.h>
#include <messages/generic_error_response.h>
#include <random_helper.h>
#include <random>
#include <macros.h>
#include <on_leaving_scope.h>
#include <ecs/battle_system.h>

using namespace std;
using namespace ibh;
using namespace rapidjson;

atomic<bool> quit{false};
namespace ibh {
    template<class Archive>
    void serialize(Archive &archive,
                   generic_error_response &m) {
        archive((uint32_t)ibh::generic_error_response::type, m.error, m.pretty_error_description, m.pretty_error_name, m.clear_login_data);
    }
}

void on_sigint(int sig) {
    quit = true;
}

void bench_censor_sensor() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);

    censor_sensor s{};
    s.add_dictionary("assets/profanity_locales/en.json");

    for(int i = 0; i < 1'000'000; i++) {
        s.is_profane("this is bollocks");
    }
}

char hashed_password[crypto_pwhash_STRBYTES];
string test_pass = "very_secure_password";
#define crypto_pwhash_argon2id_MEMLIMIT_rair 33554432U

void bench_hashing() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);

    if (crypto_pwhash_str(hashed_password,
                          test_pass.c_str(),
                          test_pass.length(),
                          crypto_pwhash_argon2id_OPSLIMIT_SENSITIVE,
                          crypto_pwhash_argon2id_MEMLIMIT_rair) != 0) {
        spdlog::error("out of memory?");
        return;
    }
}

void bench_hash_verify() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);

    if (crypto_pwhash_str_verify(hashed_password, test_pass.c_str(), test_pass.length()) != 0) {
        spdlog::error("Hash should verify");
    }
}

void bench_serialization() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);
    generic_error_response resp{"err", "pretty err", "desc", true};

    for(int i = 0; i < 1'000'000; i++) {
        auto msg = resp.serialize();
        rapidjson::Document d;
        d.Parse(&msg[0], msg.size());
        auto resp2 = generic_error_response::deserialize(d);
        if(resp.clear_login_data != resp2->clear_login_data) {
            spdlog::error("[{}] err in serialization", __FUNCTION__);
        }
    }
}

void bench_serialization_cereal() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);
    generic_error_response resp{"err", "pretty err", "desc", true};

    for(int i = 0; i < 1'000'000; i++) {
        stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(resp);
        }
        {
            cereal::BinaryInputArchive ar(ss);
            generic_error_response resp2{"", "", "", false};
            ar(resp2);
            if(resp.clear_login_data != resp2.clear_login_data) {
                spdlog::error("[{}] err in serialization", __FUNCTION__);
            }
        }
    }
}

void bench_rapidjson_without_strlen() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    for(int i = 0; i < 1'000'000; i++) {
        writer.String("some strsome str");
    }

    writer.EndObject();
}

void bench_rapidjson_with_strlen() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    for(int i = 0; i < 1'000'000; i++) {
        writer.String("some strsome str", string_length("some strsome str"));
    }

    writer.EndObject();
}

void bench_random_helper() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);

    for(int64_t i = 0; i < 1'000'000; i++) {
        ibh::random.generate_single(0L, 1'000'000L);
    }
}

void bench_pcg() {
    if(quit) {
        return;
    }

    MEASURE_TIME_OF_FUNCTION(info);

    pcg64 rng64{pcg_extras::seed_seq_from<random_device>()};
    uniform_int_distribution<int64_t> dist;
    for(int64_t i = 0; i < 1'000'000; i++) {
        dist(rng64);
    }
}

void bench_battle() {
    if(quit) {
        return;
    }

    outward_queues q;
    entt::registry es;

    for(int64_t i = 0; i < 1'000; i++) {
        auto entt = es.create();
        ibh_flat_map<uint32_t, stat_component> stats;
        for(auto &stat : stat_name_ids) {
            stats.insert(ibh_flat_map<uint32_t, stat_component>::value_type{stat, stat_component{"t", i+1}});
        }
        es.assign<monster_definition_component>(entt, fmt::format("{}", i), stats);
    }

    for(int64_t i = 0; i < 1'000; i++) {
        auto entt = es.create();
        ibh_flat_map<uint32_t, int64_t> stats;
        for(auto &stat : stat_name_ids) {
            stats.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat, i+1});
        }
        es.assign<monster_special_definition_component>(entt, fmt::format("{}", i), stats, false);
    }

    for(int64_t i = 0; i < 1'000; i++) {
        auto entt = es.create();
        ibh_flat_map<uint32_t, int64_t> stats;
        for(auto &stat : stat_name_ids) {
            stats.insert(ibh_flat_map<uint32_t, int64_t>::value_type{stat, i+1});
        }
        es.assign<pc_component>(entt, i, i,  "name", "race", "dir", "class", "spawn", i, i, stats, ibh_flat_map<uint32_t, item_component> {}, vector<item_component>{}, ibh_flat_map<string, skill_component>{});
    }

    battle_system s{1, &q};

    {
        MEASURE_TIME_OF_FUNCTION(info);
        for (int64_t i = 0; i < 1'000 && !quit; i++) {
            s.do_tick(es);
        }
    }
}

int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%C-%m-%d %H:%M:%S.%e] [%L] %v");
    locale::global(locale("en_US.UTF-8"));
    set_cwd(get_selfpath());
    ::signal(SIGINT, on_sigint);

    try {
        auto config_opt = parse_env_file();
        if(!config_opt) {
            return 1;
        }
        config = config_opt.value();
    } catch (const exception& e) {
        spdlog::error("[{}] config.json file is malformed json.", __FUNCTION__);
        return 1;
    }
    db_pool = make_shared<database_pool>();
    db_pool->create_connections(config.connection_string, 2);

    if(sodium_init() != 0) {
        spdlog::error("[{}] sodium init failure", __FUNCTION__);
        return 1;
    }

    entt::registry registry;
    //load_assets(registry, quit);

    bench_censor_sensor();
    bench_hashing();
    bench_hash_verify();
    bench_serialization();
    bench_serialization_cereal();
    bench_rapidjson_without_strlen();
    bench_rapidjson_with_strlen();
    bench_random_helper();
    bench_pcg();
    //bench_battle();
}
