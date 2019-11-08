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
    struct stat_component;
    struct item_object {
        uint32_t tier;
        uint32_t value;
        uint32_t sprite;
        string name;
        string description;
        string item_type;
        vector<stat_component> stats;


        item_object(uint32_t tier, uint32_t value, uint32_t sprite, string name, string description, string item_type, vector<stat_component> stats) noexcept;
    };

    void write_item_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, item_object const &obj);
    bool read_item_object_into_vector(rapidjson::Value const &value, vector<item_object> &objs);
}
