/*
    Realm of Aesir
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

#include "random_helper.h"
#include <random>
#include <spdlog/spdlog.h>

using namespace std;
using namespace lotr;

//#define EXTREME_RANDOM_LOGGING

random_helper::random_helper() : _rng64(pcg_extras::seed_seq_from<random_device>()), _rng32(pcg_extras::seed_seq_from<random_device>()) { }

uint64_t random_helper::generate_single_fast(uint64_t end) {
    return _rng64(end);
}

uint32_t random_helper::generate_single_fast(uint32_t end) {
    return _rng32(end);
}

uint64_t random_helper::generate_single(uint64_t from, uint64_t end) {
    uniform_int_distribution<decltype(from)> uniform_dist(from, end);
    decltype(from) ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

uint64_t random_helper::generate_single_uint64() {
    uniform_int_distribution<decltype(random_helper::generate_single_uint64())> uniform_dist(
            numeric_limits<decltype(random_helper::generate_single_uint64())>::min(),
            numeric_limits<decltype(random_helper::generate_single_uint64())>::max());
    decltype(random_helper::generate_single_uint64()) ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}
int64_t random_helper::generate_single(int64_t from, int64_t end) {
    uniform_int_distribution<decltype(from)> uniform_dist(from, end);
    decltype(from) ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

int64_t random_helper::generate_single_int64() {
    uniform_int_distribution<decltype(random_helper::generate_single_int64())> uniform_dist(
            numeric_limits<decltype(random_helper::generate_single_int64())>::min(),
            numeric_limits<decltype(random_helper::generate_single_int64())>::max());
    decltype(random_helper::generate_single_int64()) ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

float random_helper::generate_single(float from, float end) {
    uniform_real_distribution<decltype(from)> uniform_dist(from, end);
    decltype(from) ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

double random_helper::generate_single(double from, double end) {
    uniform_real_distribution<decltype(from)> uniform_dist(from, end);
    decltype(from) ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

bool random_helper::one_in_x(uint32_t x) {
    uniform_int_distribution<uint32_t> uniform_dist(0, x);
    bool ret = uniform_dist(_rng64) == 0;
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

thread_local random_helper lotr::random;
