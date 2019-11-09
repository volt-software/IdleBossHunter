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

#include "character_object.h"
#include <ecs/components.h>

lotr::character_object::character_object(string name, string race, string baseclass, uint64_t level, uint32_t slot, uint64_t gold, uint64_t xp, uint64_t skill_points, vector<stat_component> stats, vector<item_object> items, vector<skill_object> skills) noexcept :
name(move(name)), race(move(race)), baseclass(move(baseclass)), level(level), slot(slot), gold(gold), xp(xp), skill_points(skill_points), stats(move(stats)), items(move(items)), skills(move(skills)) {}

void lotr::write_character_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, character_object const &obj) {
    writer.String(KEY_STRING("name"));
    writer.String(obj.name.c_str(), obj.name.size());

    writer.String(KEY_STRING("race"));
    writer.String(obj.race.c_str(), obj.race.size());

    writer.String(KEY_STRING("baseclass"));
    writer.String(obj.baseclass.c_str(), obj.baseclass.size());

    writer.String(KEY_STRING("level"));
    writer.Uint64(obj.level);

    writer.String(KEY_STRING("slot"));
    writer.Uint(obj.slot);

    writer.String(KEY_STRING("gold"));
    writer.Uint64(obj.gold);

    writer.String(KEY_STRING("xp"));
    writer.Uint64(obj.xp);

    writer.String(KEY_STRING("skill_points"));
    writer.Uint64(obj.skill_points);

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

    writer.String(KEY_STRING("items"));
    writer.StartArray();
    for(auto const &item : obj.items) {
        writer.StartObject();
        write_item_object(writer, item);
        writer.EndObject();
    }
    writer.EndArray();

    writer.String(KEY_STRING("skills"));
    writer.StartArray();
    for(auto const &skill : obj.skills) {
        writer.StartObject();
        write_skill_object(writer, skill);
        writer.EndObject();
    }
    writer.EndArray();
}

bool lotr::read_character_object_into_vector(rapidjson::Value const &value, vector<character_object> &objs) {
    if(!value.IsObject() || !value.HasMember("name") ||
       !value.HasMember("race") || !value.HasMember("baseclass") ||
       !value.HasMember("level") || !value.HasMember("gold") ||
       !value.HasMember("slot") || !value.HasMember("stats") ||
       !value.HasMember("xp") || !value.HasMember("skill_points") ||
       !value.HasMember("items") || !value.HasMember("skills")) {
        return false;
    }

    auto &stats_array = value["stats"];
    if(!stats_array.IsArray()) {
        return false;
    }

    auto &items_array = value["items"];
    if(!items_array.IsArray()) {
        return false;
    }

    auto &skills_array = value["skills"];
    if(!skills_array.IsArray()) {
        return false;
    }

    vector<stat_component> stats;
    for(rapidjson::SizeType i = 0; i < stats_array.Size(); i++) {
        if(!stats_array[i].IsObject() || !stats_array[i].HasMember("name") || !stats_array[i].HasMember("value")) {
            return false;
        }

        stats.emplace_back(stats_array[i]["name"].GetString(), stats_array[i]["value"].GetUint());
    }

    vector<item_object> items;
    for(rapidjson::SizeType i = 0; i < items_array.Size(); i++) {
        if(!read_item_object_into_vector(items_array[i], items)) {
            return false;
        }
    }

    vector<skill_object> skills;
    for(rapidjson::SizeType i = 0; i < skills_array.Size(); i++) {
        if(!read_skill_object_into_vector(skills_array[i], skills)) {
            return false;
        }
    }

    objs.emplace_back(value["name"].GetString(), value["race"].GetString(), value["baseclass"].GetString(),
                      value["level"].GetUint64(), value["slot"].GetUint(), value["gold"].GetUint64(), value["xp"].GetUint64(), value["skill_points"].GetUint64(), move(stats), move(items), move(skills));
    return true;
}
