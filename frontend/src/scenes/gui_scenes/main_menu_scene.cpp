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
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <SDL.h>

using namespace std;
using namespace fresh;

void main_menu_scene::update(iscene_manager *manager, entt::registry &es, TimeDelta dt) {
    if(!_closed) {
        if(ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoTitleBar)) {
            if (ImGui::Button("New game")) {
                _closed = true;
            }

            if (ImGui::Button("Load game")) {
                _closed = true;
            }

            if (ImGui::Button("Options")) {
                _closed = true;
            }

            if (ImGui::Button("Quit")) {
                SDL_Event event;
                SDL_zero(event);
                event.type = SDL_QUIT;
                SDL_PushEvent(&event);
                _closed = true;
            }
        }
        ImGui::End();
    }
}
