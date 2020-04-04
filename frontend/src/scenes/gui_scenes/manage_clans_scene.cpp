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

#include "manage_clans_scene.h"
#include <rendering/imgui/imgui.h>
#include <rendering/imgui/imgui_internal.h>
#include "spdlog/spdlog.h"
#include <messages/generic_error_response.h>
#include <messages/clan/get_clan_listing_request.h>
#include <messages/clan/get_clan_listing_response.h>
#include <messages/clan/join_clan_request.h>
#include <messages/clan/join_clan_response.h>
#include <messages/clan/leave_clan_request.h>
#include <messages/clan/leave_clan_response.h>

using namespace std;
using namespace ibh;

manage_clans_scene::manage_clans_scene(iscene_manager *manager) : scene(generate_type<manage_clans_scene>()), _error(), _waiting_for_reply(true), _waiting_for_clans(true), _selected_clan(), _clans() {
    send_message<get_clan_listing_request>(manager);
}

void manage_clans_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(ImGui::Begin("Clans Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        if(!_error.empty()) {
            ImGui::Text("%s", _error.c_str());
        }

        if(_waiting_for_clans) {
            ImGui::Text("Waiting for server answer...");
        } else {
            ImGui::ListBoxHeader("Clans", _clans.size(), 4);
            for (auto &clan : _clans) {
                if (ImGui::Selectable(fmt::format("{}", clan.name).c_str(), _selected_clan == clan.name)) {
                    _selected_clan = clan.name;
                }
            }
            ImGui::ListBoxFooter();

            ImGui::SameLine();

            ImGui::ListBoxHeader("Clan Members", _clans.size(), 4);
            for (auto &clan : _clans) {
                if (clan.name != _selected_clan) {
                    break;
                }

                for (auto &member : clan.members) {
                    ImGui::Selectable(fmt::format("{}", member).c_str());
                }
            }
            ImGui::ListBoxFooter();

            ImGui::SameLine();

            //TODO refactor this block into generic function in scene
            if (_waiting_for_reply || _selected_clan.empty())
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button("Join")) {
                send_message<join_clan_request>(manager, _selected_clan);
                _waiting_for_reply = true;
            }

            if (ImGui::Button("Leave")) {
                send_message<leave_clan_request>(manager);
                _waiting_for_reply = true;
            }

            if (_waiting_for_reply || _selected_clan.empty())
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            if (ImGui::Button("Done")) {
                _closed = true;
            }
        }
    }
    ImGui::End();
}

void manage_clans_scene::handle_message(iscene_manager *manager, uint64_t type, message const *msg) {
    spdlog::trace("[{}] received message {}", __FUNCTION__, type);
    switch (type) {
        case get_clan_listing_response::type: {
            auto resp_msg = dynamic_cast<get_clan_listing_response const*>(msg);

            if(!resp_msg) {
                return;
            }

            _waiting_for_reply = false;
            _waiting_for_clans = false;

            if(!resp_msg->error.empty()) {
                _error = resp_msg->error;
                return;
            }

            _clans = resp_msg->clans;
            break;
        }

        case generic_error_response::type: {
            auto resp_msg = dynamic_cast<generic_error_response const*>(msg);

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
