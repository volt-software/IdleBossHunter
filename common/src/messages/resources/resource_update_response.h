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

using namespace std;

namespace ibh {
    struct resource {
        uint32_t resource_id;
        uint64_t resource_amt;
        uint64_t resource_xp;
        uint64_t resource_level;

        resource(uint32_t resource_id, uint64_t resource_amt, uint64_t resource_xp, uint64_t resource_level) noexcept : resource_id(resource_id),  resource_amt(resource_amt),  resource_xp(resource_xp),  resource_level(resource_level){}
        resource(resource&&) noexcept = default;
        resource(const resource&) noexcept = default;
        resource& operator=(resource&&) noexcept = default;
        resource& operator=(const resource&) noexcept = default;
    };

    struct resource_update_response : message {
        explicit resource_update_response(vector<resource> resources) noexcept;

        ~resource_update_response() noexcept override = default;

        [[nodiscard]]
        string serialize() const override;

        [[nodiscard]]
        static unique_ptr<resource_update_response> deserialize(rapidjson::Document const &d);

        vector<resource> resources;

        static constexpr uint64_t type = generate_type<resource_update_response>();
    };
}
