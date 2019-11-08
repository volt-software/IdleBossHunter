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

#include "logger_init.h"

#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

void lotr::reconfigure_logger(config const& config) noexcept {
    auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto time_since_epoch = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    auto file_sink = make_shared<spdlog::sinks::basic_file_sink_mt>(fmt::format("logs/log-{}.txt", time_since_epoch.count()), true);

    auto logger = make_shared<spdlog::logger>("multi_sink"s, spdlog::sinks_init_list{console_sink, file_sink});

    if(!config.debug_level.empty()) {
        if(config.debug_level == "error") {
            logger->set_level(spdlog::level::err);
        } else if(config.debug_level == "warning") {
            logger->set_level(spdlog::level::warn);
        } else if(config.debug_level == "info") {
            logger->set_level(spdlog::level::info);
        } else if(config.debug_level == "debug") {
            logger->set_level(spdlog::level::debug);
        } else if(config.debug_level == "trace") {
            logger->set_level(spdlog::level::trace);
        } else {
            spdlog::warn(R"([{}] No debug level set, couldn't match "{}" to anything)", __FUNCTION__, config.debug_level);
        }
    }

    spdlog::set_default_logger(logger);

    spdlog::info("[{}] debug level: {}", __FUNCTION__, config.debug_level);
}
