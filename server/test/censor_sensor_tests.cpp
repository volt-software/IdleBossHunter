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
#include "game_logic/censor_sensor.h"

using namespace std;
using namespace lotr;

TEST_CASE("censor sensor tests") {
    SECTION( "marks words as profane" ) {
        censor_sensor s("assets/profanity_locales/en.json");

        REQUIRE(s.is_profane("bollocks"));
        REQUIRE(s.is_profane("BoLloCKs"));
        REQUIRE(s.is_profane("anal"));
        REQUIRE(s.is_profane("arsehole"));
        REQUIRE(s.is_profane("beaners"));
        REQUIRE(!s.is_profane("hello"));
        REQUIRE(s.is_profane("this is bollocks"));
        REQUIRE(s.is_profane("this is BoLloCKs"));

        REQUIRE(s.is_profane_ish("this is bollocks"));
        REQUIRE(s.is_profane_ish("this is BoLloCKs"));
        REQUIRE(s.is_profane_ish("hello"));
        REQUIRE(s.is_profane_ish("HELLO"));
    }

    SECTION( "marks words as profane only when tier is enabled" ) {
        censor_sensor s("assets/profanity_locales/en.json");

        s.disable_tier(4);
        REQUIRE(!s.is_profane("bollocks"));
        REQUIRE(s.is_profane("anal"));
        REQUIRE(s.is_profane("arsehole"));
        REQUIRE(s.is_profane("beaners"));

        REQUIRE(!s.is_profane_ish("this is bollocks"));

        s.enable_tier(4);
        REQUIRE(s.is_profane("bollocks"));
        REQUIRE(s.is_profane("anal"));
        REQUIRE(s.is_profane("arsehole"));
        REQUIRE(s.is_profane("beaners"));

        REQUIRE(s.is_profane_ish("this is bollocks"));
    }

    SECTION( "clean profanity" ) {
        censor_sensor s("assets/profanity_locales/en.json");

        REQUIRE(s.clean_profanity("this is bollocks") == "this is ********");
    }
}
