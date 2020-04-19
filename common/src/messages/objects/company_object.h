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

#pragma once

#include <string>
#include <rapidjson/writer.h>
#include <rapidjson/pointer.h>
#include "messages/message.h"

using namespace std;

namespace ibh {
    struct stat_component;

    struct company_object {
        string name;
        vector<string> members;
        vector<stat_component> bonuses;

        company_object(string name, vector<string> members, vector<stat_component> bonuses) noexcept : name(move(name)), members(move(members)), bonuses(move(bonuses)) {}
    };
    
    void write_company_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, company_object const &obj);
    bool read_company_object_into_vector(rapidjson::Value const &value, vector<company_object> &objs);
}