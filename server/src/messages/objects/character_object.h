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
#include "item_object.h"
#include "skill_object.h"

using namespace std;

namespace lotr {
    struct stat_component;
    struct character_object {
        string name;
        string race;
        string baseclass;
        uint64_t level;
        uint32_t slot;
        uint64_t gold;
        uint64_t xp;
        uint64_t skill_points;
        vector<stat_component> stats;
        vector<item_object> items;
        vector<skill_object> skills;

        character_object(string name, string race, string baseclass, uint64_t level, uint32_t slot, uint64_t gold, uint64_t xp, uint64_t skill_points, vector<stat_component> stats, vector<item_object> items, vector<skill_object> skills) noexcept;
    };

    void write_character_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, character_object const &obj);
    bool read_character_object_into_vector(rapidjson::Value const &value, vector<character_object> &objs);
}
