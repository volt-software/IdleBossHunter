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

#include "get_clan_listing_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

get_clan_listing_response::get_clan_listing_response(string error, vector<clan> clans) noexcept : error(move(error)), clans(move(clans)) {

}

string get_clan_listing_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("error"));
    writer.String(error.c_str(), error.size());

    writer.String(KEY_STRING("clans"));
    writer.StartArray();
    for(auto &clan : clans) {
        writer.StartObject();

        writer.String(KEY_STRING("id"));
        writer.Uint64(clan.id);

        writer.String(KEY_STRING("name"));
        writer.String(clan.name.c_str(), clan.name.size());

        writer.String(KEY_STRING("members"));
        writer.StartArray();
        for(auto &member : clan.members) {
            writer.String(member.c_str(), member.size());
        }
        writer.EndArray();

        writer.String(KEY_STRING("bonuses"));
        writer.StartArray();
        for(auto &bonus : clan.bonuses) {
            writer.StartObject();

            writer.String(KEY_STRING("name"));
            writer.String(bonus.name.c_str(), bonus.name.size());

            writer.String(KEY_STRING("amount"));
            writer.Uint64(bonus.amount);

            writer.EndObject();
        }
        writer.EndArray();

        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<get_clan_listing_response> get_clan_listing_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("error") || !d.HasMember("clans")) {
        spdlog::warn("[get_clan_listing_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[get_clan_listing_response] deserialize failed wrong type");
        return nullptr;
    }

    vector<clan> clans;
    {
        auto &clan_array = d["clans"];
        if(!clan_array.IsArray()){
            spdlog::warn("[get_clan_listing_response] deserialize failed11");
            return nullptr;
        }

        for (SizeType i = 0; i < clan_array.Size(); i++) {
            if (!clan_array[i].IsObject() ||
                !clan_array[i].HasMember("id") ||
                !clan_array[i].HasMember("name") ||
                !clan_array[i].HasMember("members") ||
                !clan_array[i].HasMember("bonuses")) {
                spdlog::warn("[get_clan_listing_response] deserialize failed12");
                return nullptr;
            }

            vector<string> members;
            {
                auto &clan_member_array = clan_array[i]["members"];
                if (!clan_member_array.IsArray()) {
                    spdlog::warn("[get_clan_listing_response] deserialize failed13");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < clan_member_array.Size(); i2++) {
                    if (!clan_member_array[i2].IsString()) {
                        spdlog::warn("[get_clan_listing_response] deserialize failed14");
                        return nullptr;
                    }
                    members.emplace_back(clan_member_array[i2].GetString());
                }
            }

            vector<bonus> bonuses;
            {
                auto &bonus_array = clan_array[i]["bonuses"];
                if (!bonus_array.IsArray()) {
                    spdlog::warn("[get_clan_listing_response] deserialize failed13");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < bonus_array.Size(); i2++) {
                    if (!bonus_array[i2].IsObject() ||
                        !bonus_array[i2].HasMember("name") ||
                        !bonus_array[i2].HasMember("amount")) {
                        spdlog::warn("[get_clan_listing_response] deserialize failed14");
                        return nullptr;
                    }
                    bonuses.emplace_back(bonus_array[i2]["name"].GetString(), bonus_array[i2]["amount"].GetUint64());
                }
            }

            clans.emplace_back(clan_array[i]["id"].GetUint64(), clan_array[i]["name"].GetString(), move(members), move(bonuses));
        }
    }

    return make_unique<get_clan_listing_response>(d["error"].GetString(), move(clans));
}
