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
#include <rapidjson/document.h>
#include "messages/message.h"

using namespace std;

namespace ibh {
    struct member {
        uint64_t id;
        uint64_t level;
        string name;

        member(uint64_t id, uint64_t level, string name) noexcept : id(id), level(level), name(move(name)) {}
    };

    struct get_company_applications_response : message {
        explicit get_company_applications_response(string error, vector<member> members) noexcept;

        ~get_company_applications_response() noexcept override = default;

        [[nodiscard]]
        string serialize() const override;

        [[nodiscard]]
        static unique_ptr<get_company_applications_response> deserialize(rapidjson::Document const &d);

        string error;
        vector<member> members;

        static constexpr uint64_t type = generate_type<get_company_applications_response>();
    };
}
