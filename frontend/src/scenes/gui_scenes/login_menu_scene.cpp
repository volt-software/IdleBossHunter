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
#include "chat_scene.h"
#include "show_characters_scene.h"
#include <rendering/imgui/imgui.h>
#include <rendering/imgui/imgui_internal.h>
#include "spdlog/spdlog.h"
#include <SDL.h>
#include <messages/user_access/login_request.h>
#include <messages/user_access/login_response.h>
#include <messages/generic_error_response.h>
#include <messages/user_access/register_request.h>

using namespace std;
using namespace ibh;

void login_menu_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(ImGui::Begin("Login/Register", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if(_error.size() > 0) {
            ImGui::Text("%s", _error.c_str());
        }

        static char bufpass[64];
        static char bufuser[64];
        static char bufmail[128];
        ImGui::InputTextWithHint("username", "<username>", bufuser, 64, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll);
        ImGui::InputTextWithHint("password", "<password>", bufpass, 64, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);

        if(_show_register) {
            ImGui::InputTextWithHint("email", "<email>", bufmail, 128, ImGuiInputTextFlags_CharsNoBlank);
        }

        if (_waiting_for_reply)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        if (ImGui::Button("Register")) {
            if(_show_register) {
                if(strlen(bufpass) >= 4 && strlen(bufuser) >= 2 && strlen(bufmail) >= 4) {
                    send_message<register_request>(manager, bufuser, bufpass, bufmail);
                } else {
                    _error = "Input not correct";
                }
            } else {
                _show_register = true;
            }
        }

        if (ImGui::Button("Login")) {
            _show_register = false;
            if(strlen(bufpass) >= 4 && strlen(bufuser) >= 2) {
                send_message<login_request>(manager, bufuser, bufpass);
            } else {
                _error = "Input not correct";
            }
        }

        if (_waiting_for_reply)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
    }
    ImGui::End();
}

void login_menu_scene::handle_message(iscene_manager *manager, uint64_t type, message *msg) {
    spdlog::trace("[{}] received message {}", __FUNCTION__, type);
    switch (type) {
        case login_response::type: {
            auto resp_msg = dynamic_cast<login_response*>(msg);

            if(!resp_msg) {
                return;
            }

            manager->add(make_unique<show_characters_scene>(manager, resp_msg->characters));
            manager->add(make_unique<chat_scene>(resp_msg->online_users));

            _waiting_for_reply = false;
            _closed = true;
            break;
        }
        case generic_error_response::type: {
            auto resp_msg = dynamic_cast<generic_error_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _waiting_for_reply = false;
            _error = resp_msg->error;
            spdlog::warn("[{}] received error {}", __FUNCTION__, resp_msg->error);
            break;
        }
        default: {
            break;
        }
    }
}
