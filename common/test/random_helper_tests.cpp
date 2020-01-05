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
#include "random_helper.h"

using namespace std;
using namespace ibh;

TEST_CASE("random_helper tests") {
    SECTION("quickcheck generate within bounds") {
        for(uint32_t i = 0; i < 1'000; i++) {
            auto ret = ibh::random.generate_single(0UL, 3UL);
            REQUIRE(ret <= 3);

            auto ret2 = ibh::random.generate_single(-5L, 5L);
            REQUIRE(ret2 >= -5);
            REQUIRE(ret2 <= 5);

            auto ret3 = ibh::random.generate_single(-0.5F, .5F);
            REQUIRE(ret3 >= -0.5F);
            REQUIRE(ret3 <= .5F);

            auto ret4 = ibh::random.generate_single(-0.5, .5);
            REQUIRE(ret4 >= -0.5);
            REQUIRE(ret4 <= .5);
        }
    }
}
