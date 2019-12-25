/*
    IdleBossHunter
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


#pragma once

#include "../scene.h"

#include <messages/objects/character_object.h>

namespace ibh {
    class show_characters_scene : public scene  {
    public:
        explicit show_characters_scene(vector<character_object> characters);
        ~show_characters_scene() override = default;

        void update(iscene_manager *manager, entt::registry &es, TimeDelta dt) override;
        void handle_message(uint32_t type, message* msg) override;

    private:
        vector<character_object> _characters;
        bool _show_create;
        string _error;
    };
}
