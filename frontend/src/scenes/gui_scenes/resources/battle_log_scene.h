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

#include "scenes/scene.h"
#include <mutex>

namespace ibh {
    class battle_log_scene : public scene  {
    public:
        battle_log_scene() : scene(generate_type<battle_log_scene>()) {}
        ~battle_log_scene() override = default;

        void update(iscene_manager *manager, TimeDelta dt) override;
        void handle_message(iscene_manager *manager, uint64_t type, message const* msg) override;

    private:
        vector<string> _current_battle_log{};
        vector<string> _battle_log{};
        string _mob_name{};
        bool _first_frame{};
        uint64_t _mob_max_hp{};
        uint64_t _player_max_hp{};
        uint64_t _mob_current_hp{};
        uint64_t _player_current_hp{};
    };
}
