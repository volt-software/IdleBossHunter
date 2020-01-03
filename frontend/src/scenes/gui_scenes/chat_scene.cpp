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

#include "chat_scene.h"
#include "messages/chat/message_request.h"
#include "messages/chat/message_response.h"
#include "messages/user_access/user_entered_game_response.h"
#include "messages/user_access/user_left_game_response.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <cstring>

using namespace std;
using namespace ibh;

chat_scene::chat_scene(vector<account_object> online_users) : scene(1), _messages(), _online_users(move(online_users)), _first_frame(true) {

}

void chat_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(_first_frame) {
        ImGui::SetNextWindowSize(ImVec2{610.0F, 280.0F}, ImGuiCond_Once);
    }
    if(ImGui::Begin("Chat Window")) {

        static char bufmsg[100];

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
            ImGui::SetKeyboardFocusHere(0);
        }
        ImGui::PushItemWidth(-1);
        if(ImGui::InputTextWithHint("|chat message", "<message>", bufmsg, 100, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
            send_message<message_request>(manager, string{bufmsg});
            memset(bufmsg, 0, sizeof(bufmsg));
        }
        ImGui::PopItemWidth();

        ImGui::BeginChild("Chat Messages", ImVec2(0.0F, 200.0F), true, ImGuiWindowFlags_None);
        ImGui::Columns(2);

        if(_first_frame) {
            ImGui::SetColumnWidth(-1, 500.0F);
        }
        for(auto const &msg : _messages) {
            ImGui::Text("%s", msg.c_str());
        }
        ImGui::NextColumn();

        if(_first_frame) {
            ImGui::SetColumnWidth(-1, 100.0F);
            _first_frame = false;
        }
        for(auto const &user : _online_users) {
            if(user.is_game_master) {
                ImGui::PushStyleColor(ImGuiCol_Text,  ImVec4(1.0F, 0.4F, 0.4F, 1.0F));
            }
            if(user.is_tester) {
                ImGui::PushStyleColor(ImGuiCol_Text,  ImVec4(0.4F, 1.0F, 0.4F, 1.0F));
            }

            ImGui::Text("%s", user.username.c_str());

            if(user.is_game_master) {
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Game master");
                }

                ImGui::PopStyleColor(1);
            }

            if(user.is_tester) {
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Tester");
                }

                ImGui::PopStyleColor(1);
            }
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

void chat_scene::handle_message(iscene_manager *manager, uint64_t type, message *msg) {
    switch (type) {
        case message_response::type: {
            auto resp_msg = dynamic_cast<message_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _messages.emplace_back(fmt::format("{}: {}", resp_msg->user, resp_msg->content));
            break;
        }
        case user_entered_game_response::type: {
            auto resp_msg = dynamic_cast<user_entered_game_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _messages.emplace_back(fmt::format("{} entered the game", resp_msg->user.username));
            _online_users.push_back(resp_msg->user);
            break;
        }
        case user_left_game_response::type: {
            auto resp_msg = dynamic_cast<user_left_game_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _messages.emplace_back(fmt::format("{} left the game", resp_msg->username));
            _online_users.erase(remove_if(begin(_online_users), end(_online_users), [&](const account_object &user_obj) noexcept { return user_obj.username == resp_msg->username; }), end(_online_users));
            break;
        }
        default: {
            break;
        }
    }
}
