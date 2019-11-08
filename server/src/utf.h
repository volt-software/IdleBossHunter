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

#include <string>
#include <locale>
#include <codecvt>

using namespace std;

namespace lotr {
    [[nodiscard]]
    u32string To_UTF32(const string &s);
    void string_tolower(string &str);
    [[nodiscard]]
    string string_tolower_copy(string str);

    [[nodiscard]]
    string utf_to_upper_copy(string const &str);

    [[nodiscard]]
    string utf_to_lower_copy(string const &str);
}
