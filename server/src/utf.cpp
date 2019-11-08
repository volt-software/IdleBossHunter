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

#include "utf.h"
#include <algorithm>

u32string lotr::To_UTF32(const string &s) {
    wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(s);
}

void lotr::string_tolower(string &str) {
    transform(str.begin(), str.end(), str.begin(),
              [](unsigned char c){ return tolower(c); });
}

string lotr::string_tolower_copy(string str) {
    transform(str.begin(), str.end(), str.begin(),
              [](unsigned char c){ return tolower(c); });
    return str;
}

string lotr::utf_to_upper_copy(string const &str)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t>> to_wstring_convertor;
    auto wstr = to_wstring_convertor.from_bytes(str);

    auto& f = use_facet<ctype<wchar_t>>(locale());

    f.toupper(&wstr[0], &wstr[0] + wstr.size());

    return to_wstring_convertor.to_bytes(wstr);
}

string lotr::utf_to_lower_copy(string const &str)
{
    wstring_convert<codecvt_utf8<wchar_t>> to_wstring_convertor;
    auto wstr = to_wstring_convertor.from_bytes(str);

    auto& f = use_facet<ctype<wchar_t>>(locale());

    f.tolower(&wstr[0], &wstr[0] + wstr.size());

    return to_wstring_convertor.to_bytes(wstr);
}
