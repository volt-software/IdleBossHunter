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

#include "manage_company_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <messages/generic_error_response.h>
#include <messages/company/get_company_listing_request.h>
#include <messages/company/get_company_listing_response.h>
#include <messages/company/join_company_request.h>
#include <messages/company/join_company_response.h>
#include <messages/company/leave_company_request.h>
#include <messages/company/leave_company_response.h>

using namespace std;
using namespace ibh;

manage_company_scene::manage_company_scene() : scene(generate_type<manage_company_scene>()), _error(), _waiting_for_reply(true), _waiting_for_companies(true), _selected_company(), _companies() {
}

void manage_company_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(ImGui::Begin("Companies Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        if(!_error.empty()) {
            ImGui::Text("%s", _error.c_str());
        }

        if(_waiting_for_companies) {
            ImGui::Text("Waiting for server answer...");
        } else {
            if(ImGui::ListBoxHeader("Companies", _companies.size(), 4)) {
                for (auto &company : _companies) {
                    if (ImGui::Selectable(fmt::format("{}", company.name).c_str(), _selected_company == company.name)) {
                        _selected_company = company.name;
                    }
                }
                ImGui::ListBoxFooter();
            }

            ImGui::SameLine();

            if(ImGui::ListBoxHeader("Company Members", _companies.size(), 4)) {
                for (auto &company : _companies) {
                    if (company.name != _selected_company) {
                        break;
                    }

                    for (auto &member : company.members) {
                        ImGui::Selectable(fmt::format("{}", member).c_str());
                    }
                }
                ImGui::ListBoxFooter();
            }

            ImGui::SameLine();

            disable_buttons_when(_waiting_for_reply || _selected_company.empty());

            if (ImGui::Button("Join")) {
                send_message<join_company_request>(manager, _selected_company);
                _waiting_for_reply = true;
            }

            if (ImGui::Button("Leave")) {
                send_message<leave_company_request>(manager);
                _waiting_for_reply = true;
            }

            if (ImGui::Button("Create")) {
                send_message<leave_company_request>(manager);
                _waiting_for_reply = true;
            }

            reenable_buttons();

            if (ImGui::Button("Done")) {
                _closed = true;
            }
        }
    }
    ImGui::End();
}

void manage_company_scene::handle_message(iscene_manager *manager, uint64_t type, message const *msg) {
    spdlog::trace("[{}] received message {}", __FUNCTION__, type);
    switch (type) {
        case get_company_listing_response::type: {
            auto resp_msg = dynamic_cast<get_company_listing_response const*>(msg);

            if(!resp_msg) {
                return;
            }

            _waiting_for_reply = false;
            _waiting_for_companies = false;

            if(!resp_msg->error.empty()) {
                _error = resp_msg->error;
                return;
            }

            _companies = resp_msg->companies;
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
