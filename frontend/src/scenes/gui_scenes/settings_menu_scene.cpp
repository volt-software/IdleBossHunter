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

#include "settings_menu_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"

using namespace std;
using namespace ibh;

void settings_menu_scene::update(iscene_manager *manager, entt::registry &es, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(ImGui::Begin("Settings Menu", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        auto *config = manager->get_config();
        string resolution_label = fmt::format("{}x{}", config->screen_width, config->screen_height);
        if (ImGui::BeginCombo("Resolution", resolution_label.c_str()))
        {
            if (ImGui::Selectable("1280x720", config->screen_width == 1280)) {
                send_event(config->user_event_type, 0);
            }
            if (ImGui::Selectable("1600x900", config->screen_width == 1600)) {
                send_event(config->user_event_type, 1);
            }
            if (ImGui::Selectable("1920x1080", config->screen_width == 1920)) {
                send_event(config->user_event_type, 2);
            }
            ImGui::EndCombo();
        }

        string music_label = fmt::format("{}", config->music_to_play);

        if (ImGui::BeginCombo("Music", music_label.c_str())) {
            for(uint32_t i = 1; i <= 8; i++) {
                if (ImGui::Selectable(fmt::format("{}", i).c_str(), config->music_to_play == i)) {
                    send_event(config->user_event_type, 3, new int(i));
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Show IMGUI demo window", &config->show_demo_window);

        if (ImGui::Button("Done")) {
            _closed = true;
        }
    }
    ImGui::End();
}

void settings_menu_scene::handle_message(uint32_t type, message *msg) {

}
