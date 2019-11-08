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
#include <rapidjson/writer.h>
#include <rapidjson/pointer.h>
#include "messages/message.h"

using namespace std;

namespace lotr {
    struct account_object {
        bool is_game_master;
        bool is_tester;
        bool has_done_trial;
        uint64_t trial_ends_unix_timestamp;
        uint32_t subscription_tier;
        string username;

        account_object(bool is_game_master, bool is_tester, bool has_done_trial, uint64_t trial_ends_unix_timestamp, uint32_t subscription_tier, string username) noexcept :
                is_game_master(is_game_master), is_tester(is_tester), has_done_trial(has_done_trial), trial_ends_unix_timestamp(trial_ends_unix_timestamp), subscription_tier(subscription_tier), username(move(username)) {}
    };

    void write_account_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, account_object const &obj);
    bool read_account_object_into_vector(rapidjson::Value const &value, vector<account_object> &objs);
}
