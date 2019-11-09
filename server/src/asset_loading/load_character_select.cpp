/*
    IdleBossHunter
    Copyright (C) 2019  Michael de Lang

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

#include "load_character_select.h"
#include <working_directory_manipulation.h>
#include "spdlog/spdlog.h"
#include <ecs/components.h>

using namespace std;
using namespace lotr;
using namespace rapidjson;

void emplace_maxhp_maxmp(vector<stat_component> &stat_mods) {
    auto hp_it = find_if(begin(stat_mods), end(stat_mods), [](stat_component const &sc){ return sc.name == stat_hp;});
    if(hp_it != end(stat_mods)) {
        stat_mods.emplace_back(stat_max_hp, hp_it->value);
    }

    auto mp_it = find_if(begin(stat_mods), end(stat_mods), [](stat_component const &sc){ return sc.name == stat_mp;});
    if(mp_it != end(stat_mods)) {
        stat_mods.emplace_back(stat_max_mp, mp_it->value);
    }
}

void read_items(Value const &item_array, vector<item_object> &items) {
    if(!item_array.IsArray()) {
        spdlog::warn("[{}] deserialize failed", __FUNCTION__);
        return;
    }

    for(SizeType i = 0; i < item_array.Size(); i++) {
        if(!read_item_object_into_vector(item_array[i], items)) {
            spdlog::warn("[{}] deserialize failed", __FUNCTION__);
            continue;
        }
    }
}

void read_skills(Value const &skill_array, vector<skill_object> &skills) {
    if(!skill_array.IsArray()) {
        spdlog::warn("[{}] deserialize failed", __FUNCTION__);
        return;
    }

    for(SizeType i = 0; i < skill_array.Size(); i++) {
        if(!read_skill_object_into_vector(skill_array[i], skills)) {
            spdlog::warn("[{}] deserialize failed", __FUNCTION__);
            continue;
        }
    }
}

void read_classes(Value const &class_array, vector<character_class> &classes) {
    if(!class_array.IsArray()) {
        spdlog::warn("[{}] deserialize failed", __FUNCTION__);
        return;
    }

    for(SizeType i = 0; i < class_array.Size(); i++) {
        auto &_class = class_array[i];
        spdlog::trace("[{}] loading class {}", __FUNCTION__, _class["name"].GetString());

        vector<stat_component> stat_mods;
        vector<item_object> items;
        vector<skill_object> skills;
        for (auto const &stat : stat_names) {
            if(_class.HasMember(stat.c_str())) {
                stat_mods.emplace_back(stat, _class[stat.c_str()].GetInt64());
            }
        }

        read_items(_class["items"], items);
        read_skills(_class["skills"], skills);

        classes.emplace_back(_class["name"].GetString(), _class["description"].GetString(), move(stat_mods), move(items), move(skills));
    }
}

void read_races(Value const &race_array, vector<character_race> &races) {
    if(!race_array.IsArray()) {
        spdlog::warn("[{}] deserialize failed", __FUNCTION__);
        return;
    }

    for(SizeType i = 0; i < race_array.Size(); i++) {
        auto &race = race_array[i];
        spdlog::trace("[{}] loading class {}", __FUNCTION__, race["name"].GetString());

        vector<stat_component> stat_mods;
        for (auto const &stat : stat_names) {
            if(race.HasMember(stat.c_str())) {
                stat_mods.emplace_back(stat, race[stat.c_str()].GetInt64());
            }
        }

        races.emplace_back(race["name"].GetString(), race["description"].GetString(), move(stat_mods));
    }
}

optional<character_select_response> lotr::load_character_select(string const &file) {
    auto env_contents = read_whole_file(file);

    if(!env_contents) {
        spdlog::trace("[{}] couldn't load character select!", __FUNCTION__);
        return {};
    }

    Document d;
    d.Parse(env_contents->c_str(), env_contents->size());

    if(!d.IsObject() || !d.HasMember("races") || !d.HasMember("races")) {
        spdlog::trace("[{}] couldn't load character select!", __FUNCTION__);
        return {};
    }

    vector<character_class> classes;
    read_classes(d["classes"], classes);

    vector<character_race> races;
    read_races(d["races"], races);

    return character_select_response(move(races), move(classes));
}
