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

#include <common_components.h>
#include "company_object.h"

void ibh::write_company_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, company_object const &obj) {
    writer.String(KEY_STRING("name"));
    writer.String(obj.name.c_str(), obj.name.size());

    writer.String(KEY_STRING("members"));
    writer.StartArray();
    for(auto &member : obj.members) {
        writer.String(member.c_str(), member.size());
    }
    writer.EndArray();

    writer.String(KEY_STRING("bonuses"));
    writer.StartArray();
    for(stat_component const &bonus : obj.bonuses) {
        writer.StartObject();

        writer.String(KEY_STRING("stat_id"));
        writer.Uint64(bonus.stat_id);

        writer.String(KEY_STRING("value"));
        writer.Uint64(bonus.value);

        writer.EndObject();
    }
    writer.EndArray();
}

bool ibh::read_company_object_into_vector(rapidjson::Value const &value, vector<company_object> &objs) {
    if(!value.IsObject() || !value.HasMember("name") ||
       !value.HasMember("members") || !value.HasMember("bonuses")) {
        return false;
    }

    auto &stats_array = value["bonuses"];
    if(!stats_array.IsArray()) {
        return false;
    }

    auto &members_array = value["members"];
    if(!members_array.IsArray()) {
        return false;
    }

    vector<stat_component> bonuses;
    bonuses.reserve(stats_array.Size());
    for(rapidjson::SizeType i = 0; i < stats_array.Size(); i++) {
        if(!stats_array[i].IsObject() || !stats_array[i].HasMember("stat_id") || !stats_array[i].HasMember("value")) {
            return false;
        }

        bonuses.emplace_back(stats_array[i]["stat_id"].GetUint64(), stats_array[i]["value"].GetUint());
    }

    vector<string> members;
    members.reserve(members_array.Size());
    for(rapidjson::SizeType i = 0; i < members_array.Size(); i++) {
        if(!members_array[i].IsString()) {
            return false;
        }

        members.emplace_back(members_array[i].GetString());
    }

    objs.emplace_back(value["name"].GetString(), move(members), move(bonuses));
    return true;
}
