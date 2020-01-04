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

#include "load_monsters.h"
#include <working_directory_manipulation.h>
#include "spdlog/spdlog.h"
#include <ecs/components.h>
#include <rapidjson/document.h>

using namespace std;
using namespace ibh;
using namespace rapidjson;

optional<monster_definition_component> ibh::load_monsters(string const &file) {
    auto env_contents = read_whole_file(file);

    if(!env_contents) {
        spdlog::trace("[{}] couldn't load monster file {}!", __FUNCTION__, file);
        return {};
    }

    Document d;
    d.Parse(env_contents->c_str(), env_contents->size());

    if(!d.IsObject() || !d.HasMember("name") || !d.HasMember("min_level") || !d.HasMember("max_level") ||
            !d.HasMember("stats") || !d["stats"].IsObject()) {
        spdlog::trace("[{}] couldn't load monster due to missing members {}!", __FUNCTION__, file);
        return {};
    }

    vector<stat_component> stats;
    stats.reserve(d["stats"].MemberCount());

    for (auto const &stat : stat_names) {
        if(d["stats"].HasMember(stat.c_str())) {
            stats.emplace_back(stat, d["stats"][stat.c_str()].GetInt64());
        }
    }

    return monster_definition_component(d["name"].GetString(), d["min_level"].GetUint64(), d["max_level"].GetUint64(), move(stats), {});
}
