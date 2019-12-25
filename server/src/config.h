/*
    IdleBossHunter
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

#pragma once

#include <string>

using namespace std;

namespace ibh {
    struct config {
        string address;
        uint16_t port;
        string debug_level;
        string connection_string;
        string certificate_file;
        string private_key_file;
        string certificate_password;
        uint32_t tick_length;
        bool log_tick_times;
    };
}
