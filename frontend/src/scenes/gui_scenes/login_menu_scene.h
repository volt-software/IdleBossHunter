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

#include <string>

using namespace std;

namespace ibh {
    class login_menu_scene : public scene  {
    public:
        login_menu_scene() = default;
        ~login_menu_scene() override = default;

        void update(iscene_manager *manager, TimeDelta dt) override;
        void handle_message(iscene_manager *manager, uint32_t type, message* msg) override;

    private:
        string _error;
        bool _show_register;
        bool _waiting_for_reply;
    };
}
