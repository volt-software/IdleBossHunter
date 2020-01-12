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

#include "battle_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"

#include <messages/battle/battle_finished_response.h>
#include <messages/battle/battle_update_response.h>
#include <messages/battle/level_up_response.h>
#include <messages/battle/new_battle_response.h>

using namespace std;
using namespace ibh;

void battle_scene::update(iscene_manager *manager, TimeDelta dt) {
    if(_closed) {
        return;
    }

    if(_first_frame) {
        ImGui::SetNextWindowSize(ImVec2{550.0F, 280.0F}, ImGuiCond_Once);
    }
    if(ImGui::Begin("Battle Log")) {
        ImGui::BeginChild("Battle Log", ImVec2(0.0F, 250.0F), true, ImGuiWindowFlags_None);
        ImGui::Columns(1);
        for(auto const &msg : _battle_log) {
            ImGui::Text("%s", msg.c_str());
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void battle_scene::handle_message(iscene_manager *manager, uint64_t type, message *msg) {
    switch (type) {
        case battle_finished_response::type: {
            auto resp_msg = dynamic_cast<battle_finished_response*>(msg);

            if(!resp_msg) {
                return;
            }

            if(resp_msg->mob_died) {
                _battle_log.emplace_back(fmt::format("You killed the {}. You gained {} xp and {} gold.", _mob_name, resp_msg->xp_gained, resp_msg->money_gained));
            }

            break;
        }
        case battle_update_response::type: {
            auto resp_msg = dynamic_cast<battle_update_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _battle_log.emplace_back(fmt::format("{} hit you {} out of {} times for {} dmg!", _mob_name, resp_msg->mob_hits, resp_msg->mob_turns, resp_msg->mob_damage));
            _battle_log.emplace_back(fmt::format("You hit {} out of {} times for {} dmg!", _mob_name, resp_msg->player_hits, resp_msg->player_turns, resp_msg->player_damage));
            break;
        }
        case level_up_response::type: {
            auto resp_msg = dynamic_cast<level_up_response*>(msg);

            if(!resp_msg) {
                return;
            }

            fmt::memory_buffer out;
            format_to(out, "You leveled up! New stats: ");
            for(auto &stat : resp_msg->added_stats) {
                format_to(out, "{}: +{}, ", stat.first, stat.second.value);
            }

            _battle_log.emplace_back(to_string(out));
            break;
        }
        case new_battle_response::type: {
            auto resp_msg = dynamic_cast<new_battle_response*>(msg);

            if(!resp_msg) {
                return;
            }

            _mob_name = resp_msg->mob_name;
            _battle_log.emplace_back(fmt::format("You enter battle with a {}", _mob_name));
            break;
        }
        default: {
            break;
        }
    }
}
