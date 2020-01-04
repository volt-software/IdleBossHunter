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

#pragma once

#include <string>
#include <optional>
#include <rapidjson/document.h>
#include "messages/message.h"
#include "messages/objects/item_object.h"
#include "messages/objects/skill_object.h"

using namespace std;

namespace ibh {
    struct stat_component;

    struct character_class {
        string name;
        string description;
        vector<stat_component> stat_mods;
        vector<item_object> items;
        vector<skill_object> skills;

        character_class(string name, string description, vector<stat_component> stat_mods, vector<item_object> items, vector<skill_object> skills) noexcept;
    };

    struct character_race {
        string name;
        string description;
        vector<stat_component> level_stat_mods;

        character_race(string name, string description, vector<stat_component> level_stat_mods) noexcept;
    };

    struct character_select_response : message {
        character_select_response(vector<character_race> races, vector<character_class> classes) noexcept;

        ~character_select_response() noexcept override = default;

        [[nodiscard]]
        string serialize() const override;

        [[nodiscard]]
        static unique_ptr<character_select_response> deserialize(rapidjson::Document const &d);

        vector<character_race> races;
        vector<character_class> classes;

        inline static constexpr uint64_t type = generate_type<character_select_response>();
    };
}
