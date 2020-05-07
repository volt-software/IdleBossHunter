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

#include "battle_update_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

battle_update_response::battle_update_response(uint64_t mob_turns, uint64_t player_turns, uint64_t mob_hits, uint64_t player_hits, uint64_t mob_damage, uint64_t player_damage) noexcept :
        mob_turns(mob_turns), player_turns(player_turns), mob_hits(mob_hits), player_hits(player_hits), mob_damage(mob_damage), player_damage(player_damage) {

}

string battle_update_response::serialize() const {
    spdlog::trace("[battle_update_response] type {}", type);

    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("mob_turns"));
    writer.Uint64(mob_turns);

    writer.String(KEY_STRING("player_turns"));
    writer.Uint64(player_turns);

    writer.String(KEY_STRING("mob_hits"));
    writer.Uint64(mob_hits);

    writer.String(KEY_STRING("player_hits"));
    writer.Uint64(player_hits);

    writer.String(KEY_STRING("mob_damage"));
    writer.Uint64(mob_damage);

    writer.String(KEY_STRING("player_damage"));
    writer.Uint64(player_damage);

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<battle_update_response> battle_update_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("mob_turns") || !d.HasMember("player_turns") || !d.HasMember("mob_hits")
    || !d.HasMember("player_hits") || !d.HasMember("mob_damage") || !d.HasMember("player_damage")) {
        spdlog::warn("[battle_update_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[battle_update_response] deserialize failed wrong type");
        return nullptr;
    }

    return make_unique<battle_update_response>(d["mob_turns"].GetUint64(), d["player_turns"].GetUint64(), d["mob_hits"].GetUint64(),
            d["player_hits"].GetUint64(), d["mob_damage"].GetUint64(), d["player_damage"].GetUint64());
}
