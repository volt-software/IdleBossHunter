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
#include <ibh_containers.h>
#include <rapidjson/document.h>
#include <functional>
#include <messages/message.h>
#include <optional>

namespace ibh {
    class scene_system : public ecs_system, iscene_manager  {
    public:
        explicit scene_system(config *config, entt::registry &es);

        ~scene_system() override = default;

        // ecs_system
        void update(entt::registry &es, TimeDelta dt) override;

        // iscene_manager
        void remove(scene *old_scene) override;
        void add(unique_ptr<scene> scene) override;
        void force_goto_scene(unique_ptr<scene> new_scene) override;
        config * get_config() override;
        entt::registry& get_entity_registry() override;
        int get_socket() override;

        // scene_system
        void init_main_menu();

        // message handling
        void handle_message(rapidjson::Document &d);
    private:
        config *_config;
        vector<unique_ptr<scene>> _scenes;
        vector<unsigned int> _scenes_to_erase;
        vector<unique_ptr<scene>> _scenes_to_add;
        unique_ptr<scene> _force_goto_scene;
        entt::registry &es;

        unsigned int _id_counter;
    };
}
