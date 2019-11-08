/*
    Realm of Aesir
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

#pragma once

#include <string>
#include <optional>
#include <vector>
#include <rapidjson/document.h>
#include "../message.h"
#include "messages/objects/account_object.h"
#include "messages/objects/character_object.h"

using namespace std;

namespace lotr {
    struct login_response : message {
        login_response(vector<character_object> players, vector<account_object> online_users, string username, string email, string motd) noexcept;

        ~login_response() noexcept = default;

        [[nodiscard]]
        string serialize() const override;

        [[nodiscard]]
        static optional<login_response> deserialize(rapidjson::Document const &d);

        vector<character_object> players;
        vector<account_object> online_users;
        string username;
        string email;
        string motd;

        static string const type;
    };
}
