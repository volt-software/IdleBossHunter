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
#include <rapidjson/writer.h>
#include <rapidjson/pointer.h>
#include "messages/message.h"

using namespace std;

namespace lotr {
    struct skill_object {
        uint32_t value;
        string name;

        skill_object(string name, uint32_t value) noexcept :
                value(value), name(move(name)) {}
    };

    void write_skill_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, skill_object const &obj);
    bool read_skill_object_into_vector(rapidjson::Value const &value, vector<skill_object> &objs);
}
