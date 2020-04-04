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

#include "get_company_listing_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

get_company_listing_response::get_company_listing_response(string error, vector<company> companies) noexcept : error(move(error)), companies(move(companies)) {

}

string get_company_listing_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("error"));
    writer.String(error.c_str(), error.size());

    writer.String(KEY_STRING("companies"));
    writer.StartArray();
    for(auto &company : companies) {
        writer.StartObject();

        writer.String(KEY_STRING("name"));
        writer.String(company.name.c_str(), company.name.size());

        writer.String(KEY_STRING("members"));
        writer.StartArray();
        for(auto &member : company.members) {
            writer.String(member.c_str(), member.size());
        }
        writer.EndArray();

        writer.String(KEY_STRING("bonuses"));
        writer.StartArray();
        for(auto &bonus : company.bonuses) {
            writer.StartObject();

            writer.String(KEY_STRING("stat_id"));
            writer.Uint64(bonus.stat_id);

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

unique_ptr<get_company_listing_response> get_company_listing_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("error") || !d.HasMember("companies")) {
        spdlog::warn("[get_company_listing_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[get_company_listing_response] deserialize failed wrong type");
        return nullptr;
    }

    vector<company> companies;
    {
        auto &company_array = d["companies"];
        if(!company_array.IsArray()){
            spdlog::warn("[get_company_listing_response] deserialize failed11");
            return nullptr;
        }

        for (SizeType i = 0; i < company_array.Size(); i++) {
            if (!company_array[i].IsObject() ||
                !company_array[i].HasMember("name") ||
                !company_array[i].HasMember("members") ||
                !company_array[i].HasMember("bonuses")) {
                spdlog::warn("[get_company_listing_response] deserialize failed12");
                return nullptr;
            }

            vector<string> members;
            {
                auto &company_member_array = company_array[i]["members"];
                if (!company_member_array.IsArray()) {
                    spdlog::warn("[get_company_listing_response] deserialize failed13");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < company_member_array.Size(); i2++) {
                    if (!company_member_array[i2].IsString()) {
                        spdlog::warn("[get_company_listing_response] deserialize failed14");
                        return nullptr;
                    }
                    members.emplace_back(company_member_array[i2].GetString());
                }
            }

            vector<bonus> bonuses;
            {
                auto &bonus_array = company_array[i]["bonuses"];
                if (!bonus_array.IsArray()) {
                    spdlog::warn("[get_company_listing_response] deserialize failed13");
                    return nullptr;
                }

                for (SizeType i2 = 0; i2 < bonus_array.Size(); i2++) {
                    if (!bonus_array[i2].IsObject() ||
                        !bonus_array[i2].HasMember("stat_id") ||
                        !bonus_array[i2].HasMember("amount")) {
                        spdlog::warn("[get_company_listing_response] deserialize failed14");
                        return nullptr;
                    }
                    bonuses.emplace_back(bonus_array[i2]["stat_id"].GetUint64(), bonus_array[i2]["amount"].GetUint64());
                }
            }

            companies.emplace_back(company_array[i]["name"].GetString(), move(members), move(bonuses));
        }
    }

    return make_unique<get_company_listing_response>(d["error"].GetString(), move(companies));
}
