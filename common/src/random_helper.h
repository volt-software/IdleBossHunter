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

#pragma once

#include <pcg_random.hpp>

namespace ibh {
    class random_helper {
    public:
        random_helper();

        template<typename T>
        T generate_single(T from, T end);

        template<typename T>
        T generate_single();

        bool one_in_x(uint32_t x);
    private:
        pcg64 _rng64;
    };

    extern thread_local random_helper random;
}
