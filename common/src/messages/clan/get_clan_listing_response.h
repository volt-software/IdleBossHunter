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

using namespace std;

namespace ibh {
    struct bonus {
        string name;
        uint64_t amount;

        bonus(string name, uint64_t amount) noexcept : name(move(name)), amount(amount) {}
    };
    struct clan {
        uint64_t id;
        string name;
        vector<string> members;
        vector<bonus> bonuses;

        clan(uint64_t id, string name, vector<string> members, vector<bonus> bonuses) noexcept : id(id), name(move(name)), members(move(members)), bonuses(move(bonuses)) {}
    };

    struct get_clan_listing_response : message {
        explicit get_clan_listing_response(string error, vector<clan> clans) noexcept;

        ~get_clan_listing_response() noexcept override = default;

        [[nodiscard]]
        string serialize() const override;

        [[nodiscard]]
        static unique_ptr<get_clan_listing_response> deserialize(rapidjson::Document const &d);

        string error;
        vector<clan> clans;

        inline static constexpr uint64_t type = generate_type<get_clan_listing_response>();
    };
}
