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

#include "set_tax_request.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace ibh;
using namespace rapidjson;

set_tax_request::set_tax_request(uint32_t rate) noexcept : rate(rate) {

}

string set_tax_request::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String(KEY_STRING("type"));
    writer.Uint64(type);

    writer.String(KEY_STRING("rate"));
    writer.Uint(rate);

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<set_tax_request> set_tax_request::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("type") || !d.HasMember("rate")) {
        spdlog::warn("[set_tax_request] deserialize failed");
        return nullptr;
    }

    if(d["type"].GetUint64() != type) {
        spdlog::warn("[set_tax_request] deserialize failed wrong type");
        return nullptr;
    }

    return make_unique<set_tax_request>(d["rate"].GetUint());
}
