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
#include "../message.h"

using namespace std;

namespace ibh {
    struct new_battle_response : message {
        new_battle_response(string mob_name, uint64_t mob_level, uint64_t mob_hp, uint64_t mob_max_hp, uint64_t player_hp, uint64_t player_max_hp) noexcept;

        ~new_battle_response() noexcept override = default;

        [[nodiscard]]
        string serialize() const override;

        [[nodiscard]]
        static unique_ptr<new_battle_response> deserialize(rapidjson::Document const &d);

        string mob_name;
        uint64_t mob_level;
        uint64_t mob_hp;
        uint64_t mob_max_hp;
        uint64_t player_hp;
        uint64_t player_max_hp;

        static constexpr uint64_t type = generate_type<new_battle_response>();
    };
}
