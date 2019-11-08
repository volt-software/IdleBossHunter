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

#pragma once

#include <pcg_random.hpp>

namespace lotr {
    class random_helper {
    public:
        random_helper();

        uint64_t generate_single_fast(uint64_t end);
        uint32_t generate_single_fast(uint32_t end);

        uint64_t generate_single(uint64_t from, uint64_t end);
        uint64_t generate_single_uint64();
        int64_t generate_single(int64_t from, int64_t end);
        int64_t generate_single_int64();
        float generate_single(float from, float end);
        double generate_single(double from, double end);
        bool one_in_x(uint32_t x);
    private:
        pcg64 _rng64;
        pcg32 _rng32;
    };

    extern thread_local random_helper random;
}
