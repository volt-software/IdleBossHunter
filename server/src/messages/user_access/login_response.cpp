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

#include "login_response.h"
#include <spdlog/spdlog.h>
#include <ecs/components.h>

using namespace lotr;
using namespace rapidjson;

string const login_response::type = "Auth:login_response";

login_response::login_response(vector<character_object> players, vector<account_object> online_users, string username, string email, string motd) noexcept :
        players(move(players)), online_users(move(online_users)), username(move(username)), email(move(email)), motd(move(motd)) {
}

string login_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.String(type.c_str(), type.size());

    writer.String(KEY_STRING("username"));
    writer.String(username.c_str(), username.size());

    writer.String(KEY_STRING("email"));
    writer.String(email.c_str(), email.size());

    writer.String(KEY_STRING("motd"));
    writer.String(motd.c_str(), motd.size());

    writer.String(KEY_STRING("players"));
    writer.StartArray();

    for(auto& player : players) {
        writer.StartObject();

        write_character_object(writer, player);

        writer.EndObject();
    }

    writer.EndArray();

    writer.String(KEY_STRING("online_users"));
    writer.StartArray();

    for(auto& user : online_users) {
        writer.StartObject();

        write_account_object(writer, user);

        writer.EndObject();
    }

    writer.EndArray();

    writer.EndObject();
    return sb.GetString();
}

optional<login_response> login_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("players") || !d.HasMember("username") || !d.HasMember("email") || !d.HasMember("motd")) {
        spdlog::warn("[login_response] deserialize failed");
        return nullopt;
    }

    if(d["type"].GetString() != type) {
        spdlog::warn("[login_response] deserialize failed wrong type");
        return nullopt;
    }

    vector<character_object> players;
    auto &players_array = d["players"];
    if(!players_array.IsArray()) {
        spdlog::warn("[login_response] deserialize failed");
        return nullopt;
    }

    vector<account_object> online_users;
    auto &online_users_array = d["online_users"];
    if(!online_users_array.IsArray()) {
        spdlog::warn("[login_response] deserialize failed");
        return nullopt;
    }

    for(SizeType i = 0; i < online_users_array.Size(); i++) {
        if(!read_account_object_into_vector(online_users_array[i], online_users)) {
            spdlog::warn("[login_response] deserialize failed");
            return nullopt;
        }
    }

    for(SizeType i = 0; i < players_array.Size(); i++) {
        if(!read_character_object_into_vector(players_array[i], players)) {
            spdlog::warn("[login_response] deserialize failed");
            return nullopt;
        }
    }

    return login_response(move(players), move(online_users), d["username"].GetString(), d["email"].GetString(), d["motd"].GetString());
}
