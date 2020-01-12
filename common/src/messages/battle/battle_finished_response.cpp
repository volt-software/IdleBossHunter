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

#include "battle_finished_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

battle_finished_response::battle_finished_response(bool mob_died, bool player_died, uint64_t xp_gained, uint64_t money_gained) noexcept :
        mob_died(mob_died), player_died(player_died), xp_gained(xp_gained), money_gained(money_gained) {

}

string battle_finished_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("mob_died"));
    writer.Bool(mob_died);

    writer.String(KEY_STRING("player_died"));
    writer.Bool(player_died);

    writer.String(KEY_STRING("xp_gained"));
    writer.Uint64(xp_gained);

    writer.String(KEY_STRING("money_gained"));
    writer.Uint64(money_gained);

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<battle_finished_response> battle_finished_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("mob_died") || !d.HasMember("player_died") || !d.HasMember("xp_gained")
        || !d.HasMember("money_gained")) {
        spdlog::warn("[battle_finished_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[battle_finished_response] deserialize failed wrong type");
        return nullptr;
    }

    return make_unique<battle_finished_response>(d["mob_died"].GetBool(), d["player_died"].GetBool(), d["xp_gained"].GetUint64(),
                                               d["money_gained"].GetUint64());
}
