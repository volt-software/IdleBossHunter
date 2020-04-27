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

#include "connecting_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"

using namespace std;
using namespace ibh;

void connecting_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(ImGui::Begin("Connecting...", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Connecting to server, please wait... %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
    }
    ImGui::End();
}

void connecting_scene::handle_message(iscene_manager *manager, uint64_t type, message const *msg) {

}
