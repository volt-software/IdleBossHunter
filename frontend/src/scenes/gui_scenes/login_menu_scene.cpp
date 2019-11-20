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

#include "login_menu_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <SDL.h>

using namespace std;
using namespace ibh;

void login_menu_scene::update(iscene_manager *manager, entt::registry &es, TimeDelta dt) {
    if(!_closed) {
        if(ImGui::Begin("Settings Menu", nullptr, ImGuiWindowFlags_NoTitleBar)) {

            string label = fmt::format("{}x{}", manager->get_config()->screen_width, manager->get_config()->screen_height);
            if (ImGui::BeginCombo("Resolution", label.c_str()))
            {
                if (ImGui::Selectable("1280x720", manager->get_config()->screen_width == 1280)) {
                    spdlog::info("1280x720 {}", manager->get_config()->user_event_type);
                    SDL_Event event{0};
                    event.type = manager->get_config()->user_event_type;
                    event.user.code = 0;
                    auto ret = SDL_PushEvent(&event);
                    if(ret != 1) {
                        spdlog::error("error pushing event {}", ret);
                    }
                }
                if (ImGui::Selectable("1600x900", manager->get_config()->screen_width == 1600)) {
                    spdlog::info("1600x900");
                    SDL_Event event{0};
                    event.type = manager->get_config()->user_event_type;
                    event.user.code = 1;
                    auto ret = SDL_PushEvent(&event);
                    if(ret != 1) {
                        spdlog::error("error pushing event {}", ret);
                    }
                }
                if (ImGui::Selectable("1920x1080", manager->get_config()->screen_width == 1920)) {
                    spdlog::info("1920x1080");
                    SDL_Event event{0};
                    event.type = manager->get_config()->user_event_type;
                    event.user.code = 2;
                    auto ret = SDL_PushEvent(&event);
                    if(ret != 1) {
                        spdlog::error("error pushing event {}", ret);
                    } else {
                        spdlog::info("Pushed succesfully {}", ret);
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Done")) {
                _closed = true;
            }
        }
        ImGui::End();
    }
}
