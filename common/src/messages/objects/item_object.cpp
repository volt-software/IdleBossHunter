/*
    IdleBossHunter
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

#include "item_object.h"

ibh::item_object::item_object(uint32_t value, string name, string description, string item_type, vector<stat_component> stats) noexcept :
value(value), name(move(name)), description(move(description)), item_type(move(item_type)), stats(move(stats)) {}

void ibh::write_item_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, item_object const &obj) {
    writer.String(KEY_STRING("value"));
    writer.Uint(obj.value);

    writer.String(KEY_STRING("name"));
    writer.String(obj.name.c_str(), obj.name.size());

    writer.String(KEY_STRING("description"));
    writer.String(obj.description.c_str(), obj.description.size());

    writer.String(KEY_STRING("item_type"));
    writer.String(obj.item_type.c_str(), obj.item_type.size());

    writer.String(KEY_STRING("stats"));
    writer.StartArray();
    for(auto const &stat : obj.stats) {
        writer.StartObject();
        writer.String(KEY_STRING("name"));
        writer.String(stat.name.c_str(), stat.name.size());

        writer.String(KEY_STRING("value"));
        writer.Int64(stat.value);
        writer.EndObject();
    }
    writer.EndArray();
}

bool ibh::read_item_object_into_vector(rapidjson::Value const &value, vector<item_object> &objs) {
    if(!value.IsObject() || !value.HasMember("value") ||
            !value.HasMember("name") || !value.HasMember("stats") ||
            !value.HasMember("description") || !value.HasMember("item_type")) {
        return false;
    }

    auto &stats_array = value["stats"];
    if(!stats_array.IsArray()) {
        return false;
    }

    vector<stat_component> stats;
    for(rapidjson::SizeType i = 0; i < stats_array.Size(); i++) {
        if(!stats_array[i].IsObject() || !stats_array[i].HasMember("name") || !stats_array[i].HasMember("value")) {
            return false;
        }

        stats.emplace_back(stats_array[i]["name"].GetString(), stats_array[i]["value"].GetUint());
    }

    objs.emplace_back(value["value"].GetUint(), value["name"].GetString(), value["description"].GetString(),
            value["item_type"].GetString(), move(stats));
    return true;
}
