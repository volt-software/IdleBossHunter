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

#include <config.h>
#include <ecs/ecs.h>
#include <messages/objects/character_object.h>

namespace ibh {
    class scene;
    struct socket_component;

    class iscene_manager {
    public:
        virtual ~iscene_manager() = default;

        virtual void remove(scene *scene) = 0;
        virtual void remove_by_type(uint64_t type) = 0;
        virtual void add(unique_ptr<scene> scene) = 0;
        virtual void open_or_close(unique_ptr<scene> scene) = 0;
        virtual void force_goto_scene(unique_ptr<scene> new_scene) = 0;
        virtual config * get_config() = 0;
        virtual entt::registry& get_entity_registry() = 0;
        [[nodiscard]] virtual socket_component& get_socket()  const = 0;
        virtual void set_logged_in(bool logged_in) = 0;
        [[nodiscard]] virtual bool get_logged_in() const = 0;
        virtual void set_connected(bool connected) = 0;
        [[nodiscard]] virtual bool get_connected() const = 0;
        virtual optional<character_object>& get_character() = 0;
    };
}
