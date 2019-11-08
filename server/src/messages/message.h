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
#include <spdlog/spdlog.h>

using namespace std;

namespace lotr {
    int constexpr string_length(const char* str)
    {
        return *str ? 1 + string_length(str + 1) : 0;
    }

#define KEY_STRING(str) str, string_length(str)

    struct message {
        virtual ~message() = default;
        [[nodiscard]]
        virtual string serialize() const = 0;
    };

    struct outward_message {
        outward_message(uint64_t conn_id, unique_ptr<message> msg) : conn_id(conn_id), msg(move(msg)) {}

        uint64_t conn_id;
        unique_ptr<message> msg;
    };
}
