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

#include "user_joined_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace lotr;
using namespace rapidjson;

string const user_joined_response::type = "Auth:join";

user_joined_response::user_joined_response(account_object user) noexcept
        : user(move(user)) {

}

string user_joined_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.String(type.c_str(), type.size());

    write_account_object(writer, user);

    writer.EndObject();
    return sb.GetString();
}

optional<user_joined_response> user_joined_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("is_game_master") || !d.HasMember("is_tester") || !d.HasMember("has_done_trial") ||
        !d.HasMember("trial_ends_unix_timestamp") || !d.HasMember("subscription_tier") || !d.HasMember("username")) {
        spdlog::warn("[user_joined_response] deserialize failed");
        return nullopt;
    }

    if(d["type"].GetString() != type) {
        spdlog::warn("[user_joined_response] deserialize failed wrong type");
        return nullopt;
    }

    return user_joined_response(account_object(d["is_game_master"].GetBool(), d["is_tester"].GetBool(), d["has_done_trial"].GetBool(),
            d["trial_ends_unix_timestamp"].GetUint64(), d["subscription_tier"].GetUint(), d["username"].GetString()));
}
