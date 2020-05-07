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
#include <messages/objects/company_object.h>
#include <common_components.h>

using namespace ibh;
using namespace rapidjson;

get_company_listing_response::get_company_listing_response(string error, vector<company_object> companies) noexcept : error(move(error)), companies(move(companies)) {

}

string get_company_listing_response::serialize() const {
    spdlog::trace("[get_company_listing_response] type {}", type);

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

        write_company_object(writer, company);

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

    vector<company_object> companies;
    auto &companies_array = d["companies"];
    if(!companies_array.IsArray()) {
        spdlog::warn("[get_company_listing_response] deserialize failed");
        return nullptr;
    }

    companies.reserve(companies_array.Size());
    for(SizeType i = 0; i < companies_array.Size(); i++) {
        if(!read_company_object_into_vector(companies_array[i], companies)) {
            spdlog::warn("[get_company_listing_response] deserialize failed");
            return nullptr;
        }
    }

    return make_unique<get_company_listing_response>(d["error"].GetString(), move(companies));
}
