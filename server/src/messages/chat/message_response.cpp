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

#include "message_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace lotr;
using namespace rapidjson;

string const message_response::type = "Chat:receive";

message_response::message_response(string user, string content, string source, uint64_t unix_timestamp) noexcept : user(move(user)), content(move(content)), source(move(source)), unix_timestamp(unix_timestamp) {

}

string message_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String("type");
    writer.String(type.c_str(), type.size());

    writer.String("user");
    writer.String(user.c_str(), user.size());

    writer.String("content");
    writer.String(content.c_str(), content.size());

    writer.String("source");
    writer.String(source.c_str(), source.size());

    writer.String("unix_timestamp");
    writer.Uint64(unix_timestamp);

    writer.EndObject();
    return sb.GetString();
}

optional<message_response> message_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("user") || !d.HasMember("content") || !d.HasMember("source") || !d.HasMember("unix_timestamp")) {
        spdlog::warn("[message_response] deserialize failed");
        return nullopt;
    }

    if(d["type"].GetString() != type) {
        spdlog::warn("[message_response] deserialize failed wrong type");
        return nullopt;
    }

    return message_response(d["user"].GetString(), d["content"].GetString(), d["source"].GetString(), d["unix_timestamp"].GetUint64());
}
