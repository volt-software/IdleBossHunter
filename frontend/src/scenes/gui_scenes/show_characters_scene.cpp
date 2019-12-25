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

#include "show_characters_scene.h"
#include "messages/user_access/play_character_request.h"
#include "messages/generic_error_response.h"
#include "messages/update_response.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <SDL.h>

using namespace std;
using namespace ibh;

show_characters_scene::show_characters_scene(vector<character_object> characters) : _characters(move(characters)), _show_create(false), _error() {

}

void show_characters_scene::update(iscene_manager *manager, entt::registry &es, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(ImGui::Begin("Characters Menu", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        if(_error.size() > 0) {
            ImGui::Text("%s", _error.c_str());
        }

        int selected_char_slot = -1;
        ImGui::ListBoxHeader("Characters", _characters.size(), 4);
        for(auto& character : _characters) {
            if(ImGui::Selectable(fmt::format("{} {}", character.name, character.level).c_str())) {
                selected_char_slot = character.slot;
            }
        }
        ImGui::ListBoxFooter();

        if (ImGui::Button("Create Character")) {
            _show_create = true;
        }

        if (ImGui::Button("Play")) {
            send_message<play_character_request>(es, static_cast<uint32_t>(selected_char_slot));
        }
    }
    ImGui::End();

    if(!_show_create) {
        return;
    }

    if(ImGui::Begin("Characters Menu", nullptr, ImGuiWindowFlags_NoTitleBar)) {

    }
    ImGui::End();
}

void show_characters_scene::handle_message(uint32_t type, message *msg) {
    switch (type) {
        case update_response::type: {
            auto resp_msg = dynamic_cast<update_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _waiting_for_reply = false;
            _closed = true;
        }
        case generic_error_response::type: {
            auto resp_msg = dynamic_cast<generic_error_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _waiting_for_reply = false;
            _error = resp_msg->error;
        }
    }
}
