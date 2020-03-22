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
#include <vector>
#include <optional>
#include <rapidjson/document.h>
#include <common_components.h>
#include "../message.h"
#include "../../ibh_containers.h"

using namespace std;

namespace ibh {
    struct character_component;

    struct level_up_response : message {
        level_up_response(ibh_flat_map<uint64_t, stat_component> added_stats, uint64_t new_xp_goal, uint64_t current_xp) noexcept;

        ~level_up_response() noexcept override = default;

        [[nodiscard]]
        string serialize() const override;

        [[nodiscard]]
        static unique_ptr<level_up_response> deserialize(rapidjson::Document const &d);

        ibh_flat_map<uint64_t, stat_component> added_stats;
        uint64_t new_xp_goal;
        uint64_t current_xp;

        static constexpr uint64_t type = generate_type<level_up_response>();
    };
}
