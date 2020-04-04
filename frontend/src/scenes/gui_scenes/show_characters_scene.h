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

#include <messages/objects/character_object.h>
#include <messages/user_access/character_select_response.h>
#include <mutex>

namespace ibh {
    class show_characters_scene : public scene  {
    public:
        explicit show_characters_scene(iscene_manager *manager, vector<character_object> characters);
        ~show_characters_scene() override = default;

        void update(iscene_manager *manager, TimeDelta dt) override;
        void handle_message(iscene_manager *manager, uint64_t type, message const* msg) override;

    private:
        vector<character_object> _characters;
        vector<character_race> _races;
        vector<character_class> _classes;
        bool _show_create;
        bool _show_delete;
        bool _waiting_for_select;
        bool _waiting_for_reply;
        string _error;
        string _selected_race;
        string _selected_class;
        uint32_t _selected_slot;
        int32_t _selected_play_slot;
    };
}
