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

#include "get_clan_applicants_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

get_clan_applicants_response::get_clan_applicants_response(string error, vector<member> members) noexcept : error(move(error)), members(move(members)) {

}

string get_clan_applicants_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("error"));
    writer.String(error.c_str(), error.size());

    writer.String(KEY_STRING("members"));
    writer.StartArray();
    for(auto &member : members) {
        writer.StartObject();

        writer.String(KEY_STRING("id"));
        writer.Uint64(member.id);

        writer.String(KEY_STRING("level"));
        writer.Uint64(member.level);

        writer.String(KEY_STRING("name"));
        writer.String(member.name.c_str(), member.name.size());

        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<get_clan_applicants_response> get_clan_applicants_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("error") || !d.HasMember("members")) {
        spdlog::warn("[get_clan_applicants_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[get_clan_applicants_response] deserialize failed wrong type");
        return nullptr;
    }

    vector<member> members;
    {
        auto &members_array = d["members"];
        if(!members_array.IsArray()){
            spdlog::warn("[get_clan_applicants_response] deserialize failed11");
            return nullptr;
        }

        for (SizeType i = 0; i < members_array.Size(); i++) {
            if (!members_array[i].IsObject() ||
                !members_array[i].HasMember("id") ||
                !members_array[i].HasMember("level") ||
                !members_array[i].HasMember("name")) {
                spdlog::warn("[get_clan_applicants_response] deserialize failed12");
                return nullptr;
            }


            members.emplace_back(members_array[i]["id"].GetUint64(), members_array[i]["level"].GetUint64(), members_array[i]["name"].GetString());
        }
    }

    return make_unique<get_clan_applicants_response>(d["error"].GetString(), move(members));
}
