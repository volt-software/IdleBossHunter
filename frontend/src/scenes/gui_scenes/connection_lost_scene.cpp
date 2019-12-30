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

#include "connection_lost_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"

using namespace std;
using namespace ibh;

void connection_lost_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(ImGui::Begin("Connection lost", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Connection lost with the server. Please refresh the page.");
    }
    ImGui::End();
}

void connection_lost_scene::handle_message(iscene_manager *manager, uint32_t type, message *msg) {

}
