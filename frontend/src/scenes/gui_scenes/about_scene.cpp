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

#include "about_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"

using namespace std;
using namespace ibh;

void about_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(ImGui::Begin("About")) {
        ImGui::Text("IdleBossHunter v0.0.1");
        ImGui::Text("Copyright 2020© by sole proprietorship Volt Software, situated in the Netherlands");
        ImGui::Text("");
        if(ImGui::Button("Discord")) {
            emscripten_run_script("window.open('https://discord.gg/r9BtesB', '_blank');");
        }
        ImGui::SameLine();
        if(ImGui::Button("Github")) {
            emscripten_run_script("window.open('https://github.com/Oipo/IdleBossHunter', '_blank');");
        }
        ImGui::SameLine(ImGui::GetWindowWidth()-67);
        if (ImGui::Button("Close")) {
            _closed = true;
        }
    }
    ImGui::End();
}

void about_scene::handle_message(iscene_manager *manager, uint64_t type, message *msg) {

}
