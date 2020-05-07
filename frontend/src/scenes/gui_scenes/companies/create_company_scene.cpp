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

#include "create_company_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <messages/generic_error_response.h>
#include <messages/company/create_company_request.h>
#include <messages/company/create_company_response.h>

using namespace std;
using namespace ibh;

create_company_scene::create_company_scene() : scene(generate_type<create_company_scene>()), _error(), _waiting_for_reply() {
}

void create_company_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(ImGui::Begin("Companies Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        if(!_error.empty()) {
            ImGui::Text("%s", _error.c_str());
        }

        static char bufname[64];

        disable_buttons_when(_waiting_for_reply);

        ImGui::InputTextWithHint("Name", "<company name>", bufname, 64, ImGuiInputTextFlags_AutoSelectAll);

        if (ImGui::Button("Create")) {
            _company_name = bufname;
            send_message<create_company_request>(manager, _company_name, static_cast<uint16_t>(0));
            _waiting_for_reply = true;
        }

        reenable_buttons();
    }
    ImGui::End();
}

void create_company_scene::handle_message(iscene_manager *manager, uint64_t type, message const *msg) {
    spdlog::trace("[{}] received message {}", __FUNCTION__, type);
    switch (type) {
        case create_company_response::type: {
            auto resp_msg = dynamic_cast<create_company_response const*>(msg);

            if(!resp_msg) {
                return;
            }

            _waiting_for_reply = false;

            if(!resp_msg->error.empty()) {
                _error = resp_msg->error;
                return;
            }

            manager->get_character()->company = _company_name;
            _closed = true;

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
