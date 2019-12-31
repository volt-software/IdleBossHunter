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

#include "main_menu_scene.h"
#include "settings_menu_scene.h"
#include "login_menu_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <SDL.h>

using namespace std;
using namespace ibh;

void main_menu_scene::update(iscene_manager *manager, TimeDelta dt) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Login")) {
            manager->add(make_unique<login_menu_scene>());
        }

        if (ImGui::MenuItem("Settings")) {
            manager->add(make_unique<settings_menu_scene>());
        }

        ImGui::EndMainMenuBar();
    }
}

void main_menu_scene::handle_message(iscene_manager *manager, uint32_t type, message *msg) {

}
