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

#include "resource_update_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

resource_update_response::resource_update_response(vector<resource> resources) noexcept :
        resources(move(resources)) {

}

string resource_update_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("resources"));

    writer.StartArray();
    for(auto &resource : resources) {
        writer.StartObject();

        writer.String(KEY_STRING("resource_id"));
        writer.Uint(resource.resource_id);

        writer.String(KEY_STRING("resource_amt"));
        writer.Uint64(resource.resource_amt);

        writer.String(KEY_STRING("resource_xp"));
        writer.Uint64(resource.resource_xp);

        writer.String(KEY_STRING("resource_level"));
        writer.Uint64(resource.resource_level);

        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<resource_update_response> resource_update_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("resources")) {
        spdlog::warn("[resource_update_response] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[resource_update_response] deserialize failed wrong type");
        return nullptr;
    }

    vector<resource> resources;
    {
        auto &resources_array = d["resources"];
        if (!resources_array.IsArray()) {
            spdlog::warn("[resource_update_response] deserialize failed1");
            return nullptr;
        }

        resources.reserve(resources_array.Size());
        for (SizeType i = 0; i < resources_array.Size(); i++) {
            if (!resources_array[i].IsObject() ||
                !resources_array[i].HasMember("resource_id") ||
                !resources_array[i].HasMember("resource_amt") ||
                !resources_array[i].HasMember("resource_xp") ||
                !resources_array[i].HasMember("resource_level")) {
                spdlog::warn("[resource_update_response] deserialize failed12");
                return nullptr;
            }

            resources.emplace_back(resources_array[i]["resource_id"].GetUint(), resources_array[i]["resource_amt"].GetUint64(), resources_array[i]["resource_xp"].GetUint64(), resources_array[i]["resource_level"].GetUint64());
        }
    }

    return make_unique<resource_update_response>(move(resources));
}
