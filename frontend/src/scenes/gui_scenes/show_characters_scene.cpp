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
#include "messages/user_access/character_select_request.h"
#include "messages/user_access/character_select_response.h"
#include "messages/generic_error_response.h"
#include "messages/update_response.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <SDL.h>

using namespace std;
using namespace ibh;

show_characters_scene::show_characters_scene(iscene_manager *manager, vector<character_object> characters) : _characters(move(characters)), _races(), _classes(), _show_create(false), _waiting_for_select(true), _error() {
    character_select_request req{};
#ifdef __EMSCRIPTEN__
    emscripten_websocket_send_utf8_text(manager->get_socket(), req.serialize().c_str());
#endif
}

void show_characters_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(_waiting_for_select) {
        if(ImGui::Begin("Waiting for server", nullptr, ImGuiWindowFlags_NoTitleBar)) {

        }
        ImGui::End();
        return;
    }

    if(ImGui::Begin("Characters List", nullptr, ImGuiWindowFlags_NoTitleBar)) {
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
            send_message<play_character_request>(manager, static_cast<uint32_t>(selected_char_slot));
        }
    }
    ImGui::End();

    if(!_show_create) {
        return;
    }

    if(ImGui::Begin("Create Character", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        if(_error.size() > 0) {
            ImGui::Text("%s", _error.c_str());
        }
    }
    ImGui::End();
}

void show_characters_scene::handle_message(iscene_manager *manager, uint32_t type, message *msg) {
    switch (type) {
        case update_response::type: {
            auto resp_msg = dynamic_cast<update_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _closed = true;
        }
        case character_select_response::type: {
            auto resp_msg = dynamic_cast<character_select_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _races = resp_msg->races;
            _classes = resp_msg->classes;
            _waiting_for_select = false;
        }
        case generic_error_response::type: {
            auto resp_msg = dynamic_cast<generic_error_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _waiting_for_select = false;
            _error = resp_msg->error;
        }
    }
}
