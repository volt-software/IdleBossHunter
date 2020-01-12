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

#include "level_up_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

level_up_response::level_up_response(ibh_flat_map<string, stat_component> added_stats, uint64_t new_xp_goal, uint64_t current_xp) noexcept : added_stats(move(added_stats)), new_xp_goal(new_xp_goal), current_xp(current_xp) {

}

string level_up_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("added_stats"));
    writer.StartObject();
    for(auto &stat : added_stats) {
        writer.String(stat.first.c_str(), stat.first.size());
        writer.Int64(stat.second.value);
    }
    writer.EndObject();

    writer.String(KEY_STRING("new_xp_goal"));
    writer.Uint64(new_xp_goal);

    writer.String(KEY_STRING("current_xp"));
    writer.Uint64(current_xp);

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<level_up_response> level_up_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("added_stats") || !d.HasMember("new_xp_goal") || !d.HasMember("current_xp")) {
        spdlog::warn("[level_up_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[level_up_response] deserialize failed wrong type");
        return nullptr;
    }

    ibh_flat_map<string, stat_component> stats;
    stats.reserve(d["added_stats"].MemberCount());
    for(auto &it : d["added_stats"].GetObject()) {
        stats.insert(ibh_flat_map<string, stat_component>::value_type{it.name.GetString(), stat_component{it.name.GetString(), it.value.GetInt64()}});
    }

    return make_unique<level_up_response>(move(stats), d["new_xp_goal"].GetUint64(), d["current_xp"].GetUint64());
}
