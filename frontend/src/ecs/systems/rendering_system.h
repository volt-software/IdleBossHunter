/*
    IdleBossHunter
    Copyright (C) 2018 Michael de Lang

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

#include <SDL_video.h>
#include "../../config.h"
#include "ecs_system.h"

namespace ibh {
    class rendering_system : public ecs_system  {
    public:
        explicit rendering_system(config *config, SDL_Window *window)
        : _config(config), _window(window) {}

        ~rendering_system() override = default;

        void update(entt::registry &es, TimeDelta dt) override;
        void end_rendering();
    private:
        config *_config;
        SDL_Window *_window;
    };
}
