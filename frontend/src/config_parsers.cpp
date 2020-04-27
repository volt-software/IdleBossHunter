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

#ifndef __EMSCRIPTEN__

#include "config_parsers.h"
#include "working_directory_manipulation.h"

#include <spdlog/spdlog.h>
#include <rapidjson/document.h>

using namespace ibh;
using namespace rapidjson;

#define PARSE_MEMBER(name, var, method) \
    if(!d.HasMember(name)) { \
        spdlog::error("[{}] deserialize config.json missing " name, __FUNCTION__); \
        return {}; \
    } \
    config.var = d[name].method;

optional<config> ibh::parse_env_file() {
    auto env_contents = read_whole_file("config.json");
    if(!env_contents) {
        spdlog::error("[{}] no config.json file found. Please make one.", __FUNCTION__);
        return {};
    }

    spdlog::trace(R"([{}] config.json file contents: {})", __FUNCTION__, env_contents.value());

    config config;
    Document d;
    d.Parse(env_contents->c_str(), env_contents->size());

    if (d.HasParseError() || !d.IsObject()) {
        spdlog::error("[{}] deserialize config.json failed", __FUNCTION__);
        return {};
    }

    PARSE_MEMBER("DEBUG_LEVEL", debug_level, GetString());
    PARSE_MEMBER("SERVER_URL", server_url, GetString());
    PARSE_MEMBER("TICK_LENGTH", tick_length, GetUint());
    PARSE_MEMBER("SCREEN_WIDTH", screen_width, GetUint());
    PARSE_MEMBER("SCREEN_HEIGHT", screen_height, GetUint());
    PARSE_MEMBER("THREADS", threads, GetInt());
    PARSE_MEMBER("VOLUME", volume, GetUint());
    PARSE_MEMBER("LOG_FPS", log_fps, GetBool());
    PARSE_MEMBER("DISABLE_VSYNC", disable_vsync, GetBool());
    PARSE_MEMBER("AUTO_FULLSCREEN", auto_fullscreen, GetBool());

    return config;
}

#endif
