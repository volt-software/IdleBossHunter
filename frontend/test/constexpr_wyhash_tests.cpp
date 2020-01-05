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
#include "constexpr_wyhash.h"

using namespace std;
using namespace ibh;

namespace ibh {
    class test_class {

    };
}

// Also do this test on the frontend, since using a different compiler might lead to a different typename
// which would then lead to different message types and an inability to communicate with the server.

TEST_CASE("constexpr_wyhash tests") {
    SECTION("typename should return correct name") {
        auto name = type_name<test_class>();
        REQUIRE(name == "ibh::test_class");
    }
}
