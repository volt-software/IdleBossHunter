/*
    Realm of Aesir
    Copyright (C) 2019  Michael de Lang

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
#include "test_helpers/startup_helper.h"
#include "game_logic/random_helper.h"

using namespace std;
using namespace lotr;

TEST_CASE("random_helper tests") {
    SECTION("quickcheck generate within bounds") {
        for(uint32_t i = 0; i < 1'000; i++) {
            auto ret = lotr::random.generate_single((uint64_t)0, 3);
            REQUIRE(ret <= 3);

            auto ret2 = lotr::random.generate_single((int64_t)-5, 5);
            REQUIRE(ret2 >= -5);
            REQUIRE(ret2 <= 5);

            auto ret3 = lotr::random.generate_single(-0.5f, .5f);
            REQUIRE(ret3 >= -0.5f);
            REQUIRE(ret3 <= .5f);

            auto ret4 = lotr::random.generate_single(-0.5, .5);
            REQUIRE(ret4 >= -0.5);
            REQUIRE(ret4 <= .5);
        }
    }
}
