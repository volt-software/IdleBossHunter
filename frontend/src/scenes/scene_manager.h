/*
    IdleBossHunter
    Copyright (C) 2019  Michael de Lang

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

#include <config.h>
#include <ecs/ecs.h>

namespace ibh {
    class scene;

    class iscene_manager {
    public:
        virtual ~iscene_manager() = default;

        virtual void remove(scene *scene) = 0;
        virtual void add(unique_ptr<scene> scene) = 0;
        virtual void force_goto_scene(unique_ptr<scene> new_scene) = 0;
        virtual config * get_config() = 0;
        virtual entt::registry& get_entity_registry() = 0;
        virtual int get_socket() = 0;
    };
}
