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

#include "generic_error_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

generic_error_response::generic_error_response(string error, string pretty_error_name, string pretty_error_description, bool clear_login_data) noexcept
        : error(move(error)), pretty_error_name(move(pretty_error_name)),  pretty_error_description(move(pretty_error_description)), clear_login_data(clear_login_data) {

}

string generic_error_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("error"));
    writer.String(error.c_str(), error.size());

    writer.String(KEY_STRING("prettyErrorName"));
    writer.String(pretty_error_name.c_str(), pretty_error_name.size());

    writer.String(KEY_STRING("prettyErrorDesc"));
    writer.String(pretty_error_description.c_str(), pretty_error_description.size());

    writer.String(KEY_STRING("clearLoginData"));
    writer.Bool(clear_login_data);

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<generic_error_response> generic_error_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") ||
        !d.HasMember("error") ||
        !d.HasMember("prettyErrorName") ||
        !d.HasMember("prettyErrorDesc") ||
        !d.HasMember("clearLoginData")) {
        spdlog::warn("[generic_error_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[generic_error_response] deserialize failed wrong type");
        return nullptr;
    }

    return make_unique<generic_error_response>(
            d["error"].GetString(),
            d["prettyErrorName"].GetString(),
            d["prettyErrorDesc"].GetString(),
            d["clearLoginData"].GetBool()
            );
}
