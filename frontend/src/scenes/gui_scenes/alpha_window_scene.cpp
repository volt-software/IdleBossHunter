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

#include "alpha_window_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"

using namespace std;
using namespace ibh;

void alpha_window_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(ImGui::Begin("Alpha quality", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This build is alpha quality, things may be broken or otherwise.");
        if (ImGui::Button("Gotcha")) {
            _closed = true;
        }
    }
    ImGui::End();
}

void alpha_window_scene::handle_message(iscene_manager *manager, uint64_t type, message *msg) {

}
