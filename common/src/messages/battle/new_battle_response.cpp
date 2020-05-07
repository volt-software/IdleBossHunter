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

#include "new_battle_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

new_battle_response::new_battle_response(string mob_name, uint64_t mob_level, uint64_t mob_hp, uint64_t mob_max_hp, uint64_t player_hp, uint64_t player_max_hp) noexcept : mob_name(move(mob_name)), mob_level(mob_level), mob_hp(mob_hp), mob_max_hp(mob_max_hp), player_hp(player_hp), player_max_hp(player_max_hp) {

}

string new_battle_response::serialize() const {
    spdlog::trace("[new_battle_response] type {}", type);

    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("mob_name"));
    writer.String(mob_name.c_str(), mob_name.size());

    writer.String(KEY_STRING("mob_level"));
    writer.Uint64(mob_level);

    writer.String(KEY_STRING("mob_hp"));
    writer.Uint64(mob_hp);

    writer.String(KEY_STRING("mob_max_hp"));
    writer.Uint64(mob_max_hp);

    writer.String(KEY_STRING("player_hp"));
    writer.Uint64(player_hp);

    writer.String(KEY_STRING("player_max_hp"));
    writer.Uint64(player_max_hp);

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<new_battle_response> new_battle_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("mob_name") || !d.HasMember("mob_level") || !d.HasMember("mob_hp") || !d.HasMember("mob_max_hp") || !d.HasMember("player_hp") || !d.HasMember("player_max_hp")) {
        spdlog::warn("[new_battle_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[new_battle_response] deserialize failed wrong type");
        return nullptr;
    }

    return make_unique<new_battle_response>(d["mob_name"].GetString(), d["mob_level"].GetUint64(), d["mob_hp"].GetUint64(), d["mob_max_hp"].GetUint64(), d["player_hp"].GetUint64(), d["player_max_hp"].GetUint64());
}
