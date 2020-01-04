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

#include "character_select_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

character_class::character_class(string name, string description, vector<stat_component> stat_mods, vector<item_object> items, vector<skill_object> skills) noexcept :
name(move(name)), description(move(description)), stat_mods(move(stat_mods)), items(move(items)), skills(move(skills)) {

}

character_race::character_race(string name, string description, vector<stat_component> level_stat_mods) noexcept :
name(move(name)), description(move(description)), level_stat_mods(move(level_stat_mods)) {

}

character_select_response::character_select_response(vector<character_race> races, vector<character_class> classes) noexcept
        : races(move(races)), classes(move(classes)) {

}

string character_select_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("classes"));
    writer.StartArray();
    for(auto const &_class : classes) {
        writer.StartObject();

        writer.String(KEY_STRING("name"));
        writer.String(_class.name.c_str(), _class.name.size());

        writer.String(KEY_STRING("description"));
        writer.String(_class.description.c_str(), _class.description.size());

        writer.String(KEY_STRING("stat_mods"));
        writer.StartArray();
        for(auto const &stat : _class.stat_mods) {
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
        for(auto const &item : _class.items) {
            writer.StartObject();

            write_item_object(writer, item);

            writer.EndObject();
        }
        writer.EndArray();

        writer.String(KEY_STRING("skills"));
        writer.StartArray();
        for(auto const &skill : _class.skills) {
            writer.StartObject();

            write_skill_object(writer, skill);

            writer.EndObject();
        }
        writer.EndArray();

        writer.EndObject();
    }
    writer.EndArray();

    writer.String(KEY_STRING("races"));
    writer.StartArray();
    for(auto const &race : races) {
        writer.StartObject();

        writer.String(KEY_STRING("name"));
        writer.String(race.name.c_str(), race.name.size());

        writer.String(KEY_STRING("description"));
        writer.String(race.description.c_str(), race.description.size());

        writer.String(KEY_STRING("level_stat_mods"));
        writer.StartArray();
        for(auto const &stat : race.level_stat_mods) {
            writer.StartObject();
            writer.String(KEY_STRING("name"));
            writer.String(stat.name.c_str(), stat.name.size());

            writer.String(KEY_STRING("value"));
            writer.Int64(stat.value);
            writer.EndObject();
        }
        writer.EndArray();

        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<character_select_response> character_select_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("classes") || ! d.HasMember("races")) {
        spdlog::warn("[character_select_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[character_select_response] deserialize failed wrong type");
        return nullptr;
    }

    vector<character_class> classes;
    {
        auto &class_array = d["classes"];
        if (!class_array.IsArray()) {
            spdlog::warn("[character_select_response] deserialize failed3");
            return nullptr;
        }

        for (SizeType i = 0; i < class_array.Size(); i++) {
            if (!class_array[i].IsObject() ||
                !class_array[i].HasMember("name") ||
                !class_array[i].HasMember("description") ||
                !class_array[i].HasMember("stat_mods") ||
                !class_array[i].HasMember("items") ||
                !class_array[i].HasMember("skills")) {
                spdlog::warn("[character_select_response] deserialize failed4");
                return nullptr;
            }

            vector<stat_component> class_base_stats;
            {
                auto &race_stat_mods_array = class_array[i]["stat_mods"];
                if (!race_stat_mods_array.IsArray()) {
                    spdlog::warn("[character_select_response] deserialize failed5");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < race_stat_mods_array.Size(); i2++) {
                    if (!race_stat_mods_array[i2].IsObject() ||
                        !race_stat_mods_array[i2].HasMember("name") ||
                        !race_stat_mods_array[i2].HasMember("value")) {
                        spdlog::warn("[character_select_response] deserialize failed6");
                        return nullptr;
                    }
                    class_base_stats.emplace_back(race_stat_mods_array[i2]["name"].GetString(), race_stat_mods_array[i2]["value"].GetInt64());
                }
            }

            vector<item_object> class_items;
            {
                auto &race_items_array = class_array[i]["items"];
                if (!race_items_array.IsArray()) {
                    spdlog::warn("[character_select_response] deserialize failed7");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < race_items_array.Size(); i2++) {
                    if (!race_items_array[i2].IsObject() ||
                        !race_items_array[i2].HasMember("name")) {
                        spdlog::warn("[character_select_response] deserialize failed8");
                        return nullptr;
                    }

                    class_items.emplace_back(0, race_items_array[i2]["name"].GetString(), "", "", vector<stat_component>{});
                }
            }

            vector<skill_object> class_skills;
            {
                auto &race_skills_array = class_array[i]["skills"];
                if (!race_skills_array.IsArray()) {
                    spdlog::warn("[character_select_response] deserialize failed9");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < race_skills_array.Size(); i2++) {
                    if (!race_skills_array[i2].IsObject() ||
                        !race_skills_array[i2].HasMember("name") ||
                        !race_skills_array[i2].HasMember("value")) {
                        spdlog::warn("[character_select_response] deserialize failed10");
                        return nullptr;
                    }

                    class_skills.emplace_back(race_skills_array[i2]["name"].GetString(), race_skills_array[i2]["value"].GetInt64());
                }
            }

            classes.emplace_back(class_array[i]["name"].GetString(), class_array[i]["description"].GetString(), move(class_base_stats),
                                 move(class_items), move(class_skills));
        }
    }

    vector<character_race> races;
    {
        auto &classes_array = d["races"];
        if (!classes_array.IsArray()) {
            spdlog::warn("[character_select_response] deserialize failed11");
            return nullptr;
        }

        for (SizeType i = 0; i < classes_array.Size(); i++) {
            if (!classes_array[i].IsObject() ||
                !classes_array[i].HasMember("name") ||
                !classes_array[i].HasMember("description") ||
                !classes_array[i].HasMember("level_stat_mods")) {
                spdlog::warn("[character_select_response] deserialize failed12");
                return nullptr;
            }

            vector<stat_component> race_stat_mods;
            {
                auto &class_stat_mods_array = classes_array[i]["level_stat_mods"];
                if (!class_stat_mods_array.IsArray()) {
                    spdlog::warn("[character_select_response] deserialize failed13");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < class_stat_mods_array.Size(); i2++) {
                    if (!class_stat_mods_array[i2].IsObject() ||
                        !class_stat_mods_array[i2].HasMember("name") ||
                        !class_stat_mods_array[i2].HasMember("value")) {
                        spdlog::warn("[character_select_response] deserialize failed14");
                        return nullptr;
                    }
                    race_stat_mods.emplace_back(class_stat_mods_array[i2]["name"].GetString(), class_stat_mods_array[i2]["value"].GetInt64());
                }
            }

            races.emplace_back(classes_array[i]["name"].GetString(), classes_array[i]["description"].GetString(), move(race_stat_mods));
        }
    }

    return make_unique<character_select_response>(move(races), move(classes));
}
