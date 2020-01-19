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

#include "random_helper.h"
#include <random>
#include <spdlog/spdlog.h>

using namespace std;
using namespace ibh;

//#define EXTREME_RANDOM_LOGGING

random_helper::random_helper() : _rng64(pcg_extras::seed_seq_from<random_device>()) { }

template<typename T>
T random_helper::generate_single(T from, T end) {
    static_assert(is_arithmetic_v<T>);

    if(from == end) {
        return from;
    }

    typedef typename conditional<is_floating_point<T>::value, uniform_real_distribution<T>, uniform_int_distribution<T>>::type dist_type;
    dist_type uniform_dist(from, end);
    T ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

template<typename T>
T random_helper::generate_single() {
    static_assert(is_arithmetic_v<T>);

    typedef typename conditional<is_floating_point<T>::value, uniform_real_distribution<T>, uniform_int_distribution<T>>::type dist_type;
    dist_type uniform_dist(numeric_limits<T>::min(), numeric_limits<T>::max());
    T ret = uniform_dist(_rng64);
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

bool random_helper::one_in_x(uint32_t x) {
    if(x == 0) {
        return true;
    }

    uniform_int_distribution<uint32_t> uniform_dist(0, x);
    bool ret = uniform_dist(_rng64) == 0;
#ifdef EXTREME_RANDOM_LOGGING
    spdlog::trace("[{}] ret {}", __FUNCTION__, ret);
#endif
    return ret;
}

thread_local random_helper ibh::random;
template int64_t random_helper::generate_single();
template uint64_t random_helper::generate_single();
template float random_helper::generate_single();
template double random_helper::generate_single();
template int64_t random_helper::generate_single(int64_t from, int64_t end);
template uint64_t random_helper::generate_single(uint64_t from, uint64_t end);
template float random_helper::generate_single(float from, float end);
template double random_helper::generate_single(double from, double end);


