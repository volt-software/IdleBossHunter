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
#include <utf.h>

using namespace std;
using namespace lotr;

TEST_CASE("utf character count test") {
    string input = "ńößôб漢";
    auto output = To_UTF32(input);
    REQUIRE(output.size() == 6);
}

TEST_CASE("utf_to_upper_copy test") {
    string input = "ńößôб漢";
    auto output = utf_to_upper_copy(input);
    REQUIRE(output == "ŃÖßÔБ漢");
}

TEST_CASE("utf_to_lower_copy test") {
    string input = "ŃÖßÔБ漢";
    auto output = utf_to_lower_copy(input);
    REQUIRE(output == "ńößôб漢");
}
