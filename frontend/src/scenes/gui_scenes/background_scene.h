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

#include "../scene.h"
#include <memory>

namespace ibh {
    class texture_atlas;
    class sprite;

    class background_scene : public scene  {
    public:
        background_scene();
        ~background_scene() override;

        void update(iscene_manager *manager, TimeDelta dt) override;
        void handle_message(iscene_manager *manager, uint64_t type, message const* msg) override;

    private:
        std::vector<std::shared_ptr<texture_atlas>> _atlasses;
        std::vector<std::unique_ptr<sprite>> _sprites;
        uint32_t _microseconds_between_switch;
        uint32_t _microsecond_count;
        uint32_t _bg_to_render;
    };
}
