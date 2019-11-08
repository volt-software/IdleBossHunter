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

#include "skill_object.h"
#include <ecs/components.h>

void lotr::write_skill_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, skill_object const &obj) {
    writer.String(KEY_STRING("value"));
    writer.Uint(obj.value);

    writer.String(KEY_STRING("name"));
    writer.String(obj.name.c_str(), obj.name.size());
}

bool lotr::read_skill_object_into_vector(rapidjson::Value const &value, vector<skill_object> &objs) {
    if(!value.IsObject() || !value.HasMember("value") || !value.HasMember("name")) {
        return false;
    }

    objs.emplace_back(value["name"].GetString(), value["value"].GetUint());
    return true;
}
