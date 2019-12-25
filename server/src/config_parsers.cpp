/*
    IdleBossHunter
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

#include "config_parsers.h"
#include "working_directory_manipulation.h"

#include <spdlog/spdlog.h>
#include <rapidjson/document.h>

using namespace ibh;
using namespace rapidjson;

optional<config> ibh::parse_env_file() {
    auto env_contents = read_whole_file("config.json");
    if(!env_contents) {
        spdlog::error("[{}] no config.json file found. Please make one.", __FUNCTION__);
        return {};
    }

    spdlog::trace(R"([{}] config.json file contents: {})", __FUNCTION__, env_contents.value());

    Document d;
    d.Parse(env_contents->c_str(), env_contents->size());

    if (d.HasParseError() || !d.IsObject()) {
        spdlog::error("[{}] deserialize config.json failed", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("DEBUG_LEVEL")) {
        spdlog::error("[{}] deserialize config.json missing DEBUG_LEVEL", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("ADDRESS")) {
        spdlog::error("[{}] deserialize config.json missing ADDRESS", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("PORT")) {
        spdlog::error("[{}] deserialize config.json missing PORT", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("CONNECTION_STRING")) {
        spdlog::error("[{}] deserialize config.json missing CONNECTION_STRING", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("TICK_LENGTH")) {
        spdlog::error("[{}] deserialize config.json missing TICK_LENGTH", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("LOG_TICK_TIMES")) {
        spdlog::error("[{}] deserialize config.json missing LOG_TICK_TIMES", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("CERTIFICATE_PASSWORD")) {
        spdlog::error("[{}] deserialize config.json missing CERTIFICATE_PASSWORD", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("CERTIFICATE_FILE")) {
        spdlog::error("[{}] deserialize config.json missing CERTIFICATE_FILE", __FUNCTION__);
        return {};
    }

    if(!d.HasMember("PRIVATE_KEY_FILE")) {
        spdlog::error("[{}] deserialize config.json missing PRIVATE_KEY_FILE", __FUNCTION__);
        return {};
    }

    config config;
    config.debug_level = d["DEBUG_LEVEL"].GetString();
    config.address = d["ADDRESS"].GetString();
    config.port = d["PORT"].GetUint();
    config.connection_string = d["CONNECTION_STRING"].GetString();
    config.certificate_password = d["CERTIFICATE_PASSWORD"].GetString();
    config.certificate_file = d["CERTIFICATE_FILE"].GetString();
    config.private_key_file = d["PRIVATE_KEY_FILE"].GetString();
    config.tick_length = d["TICK_LENGTH"].GetUint();
    config.log_tick_times = d["LOG_TICK_TIMES"].GetBool();

    return config;
}
