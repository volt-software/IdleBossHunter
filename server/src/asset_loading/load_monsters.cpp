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

    if(!d.IsObject() || !d.HasMember("name") ||
            !d.HasMember("stats") || !d["stats"].IsObject()) {
        spdlog::trace("[{}] couldn't load monster due to missing members {}!", __FUNCTION__, file);
        return {};
    }

    ibh_flat_map<uint32_t, int64_t> stats;
    stats.reserve(d["stats"].MemberCount());
    string name = d["name"].GetString();

    for (auto const &stat : stat_names) {
        if(d["stats"].HasMember(stat.c_str())) {
            auto mapper_it = stat_name_to_id_mapper.find(stat);
            if(mapper_it == end(stat_name_to_id_mapper)) {
                spdlog::error("[{}] monster {} could not map stat {}", __FUNCTION__, name, stat);
                continue;
            }
            stats.insert(decltype(stats)::value_type{mapper_it->second, d["stats"][stat.c_str()].GetInt64()});
            spdlog::trace("[{}] monster {} found stat {}", __FUNCTION__, name, stat);
        } else {
            spdlog::trace("[{}] monster {} missing stat {}", __FUNCTION__, name, stat);
        }
    }

    return monster_definition_component(name, move(stats));
}
