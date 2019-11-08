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

#include "account_object.h"
#include <ecs/components.h>

void lotr::write_account_object(rapidjson::Writer<rapidjson::StringBuffer> &writer, account_object const &obj) {
    writer.String(KEY_STRING("is_game_master"));
    writer.Bool(obj.is_game_master);

    writer.String(KEY_STRING("is_tester"));
    writer.Bool(obj.is_tester);

    writer.String(KEY_STRING("has_done_trial"));
    writer.Bool(obj.has_done_trial);

    writer.String(KEY_STRING("trial_ends_unix_timestamp"));
    writer.Uint64(obj.trial_ends_unix_timestamp);

    writer.String(KEY_STRING("subscription_tier"));
    writer.Uint(obj.subscription_tier);

    writer.String(KEY_STRING("username"));
    writer.String(obj.username.c_str(), obj.username.size());
}

bool lotr::read_account_object_into_vector(rapidjson::Value const &value, vector<account_object> &objs) {
    if(!value.IsObject() || !value.HasMember("is_game_master") || !value.HasMember("is_tester") ||
       !value.HasMember("has_done_trial") || !value.HasMember("trial_ends_unix_timestamp") ||
       !value.HasMember("subscription_tier") || !value.HasMember("username")) {
        return false;
    }

    objs.emplace_back(value["is_game_master"].GetBool(), value["is_tester"].GetBool(), value["has_done_trial"].GetBool(), value["trial_ends_unix_timestamp"].GetUint64(),
            value["subscription_tier"].GetUint(), value["username"].GetString());
    return true;
}
