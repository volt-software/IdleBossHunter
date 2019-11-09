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
#include <scenes/scene.h>
#include "../../config.h"
#include "ecs_system.h"
#include "../../scenes/scene_manager.h"

namespace fresh {
    class scene_system : public ecs_system, iscene_manager  {
    public:
        explicit scene_system(config config)
                : _config(std::move(config)), _scenes(), _scenes_to_erase(), _scenes_to_add(), _id_counter(0) {}

        ~scene_system() override = default;

        // ecs_system
        void update(entt::registry &es, TimeDelta dt) override;

        // iscene_manager
        void remove(scene *old_scene) override;
        void add(scene *scene) override;
        void force_goto_scene(scene *new_scene) override;

        // scene_system
        void init_main_menu();
    private:
        config _config;
        std::vector<std::unique_ptr<scene>> _scenes;
        std::vector<unsigned int> _scenes_to_erase;
        std::vector<std::unique_ptr<scene>> _scenes_to_add;
        std::unique_ptr<scene> _force_goto_scene;

        unsigned int _id_counter;
    };
}
