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

#pragma once

#include <string>

using namespace std;

namespace ibh {
    struct config {
        // to be filled by .env
        string debug_level{};
        string server_url{};
        uint32_t tick_length{};
        bool log_fps{};
        uint32_t screen_width{};
        uint32_t screen_height{};
        bool adaptive_vsync{};
        bool disable_vsync{};
        bool auto_fullscreen{};
        int threads{};
        uint32_t volume{};

        //to be filled by code
        uint32_t refresh_rate{};
        uint32_t user_event_type{};
        uint32_t music_to_play{};
        bool show_demo_window{};
        bool borderless{};
    };
}
