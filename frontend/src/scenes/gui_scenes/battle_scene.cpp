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
    if(_first_frame) {
        ImGui::SetNextWindowSize(ImVec2{550.0F, 280.0F}, ImGuiCond_Once);
    }
    if(ImGui::Begin("Battle Log")) {
        float progress = _mob_max_hp == 0 ? 0 : _mob_current_hp/static_cast<double>(_mob_max_hp);
        ImGui::ProgressBar(progress, ImVec2(0.0F,0.0F), fmt::format("{}/{}", _mob_current_hp, _mob_max_hp).c_str());
        ImGui::SameLine(0.0F, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text(fmt::format("{}", _mob_name).c_str());

        progress = _player_max_hp == 0 ? 0 : _player_current_hp/static_cast<double>(_player_max_hp);
        ImGui::ProgressBar(progress, ImVec2(0.0F,0.0F), fmt::format("{}/{}", _player_current_hp, _player_max_hp).c_str());
        ImGui::SameLine(0.0F, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Player hp");

        ImGui::BeginChild("Battle Log", ImVec2(0.0F, 0.0F), true, ImGuiWindowFlags_None);
        ImGui::Columns(1);
        for(auto const &msg : _battle_log) {
            ImGui::Text("%s", msg.c_str());
        }
        for(auto const &msg : _current_battle_log) {
            ImGui::Text("%s", msg.c_str());
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void battle_scene::handle_message(iscene_manager *manager, uint64_t type, message const *msg) {
    switch (type) {
        case battle_finished_response::type: {
            auto resp_msg = dynamic_cast<battle_finished_response const*>(msg);

            if(!resp_msg) {
                return;
            }

            if(resp_msg->mob_died) {
                _battle_log.emplace_back(fmt::format("You killed the {}. You gained {} xp and {} gold.", _mob_name, resp_msg->xp_gained, resp_msg->money_gained));
            } else {
                _battle_log.emplace_back(fmt::format("The {} killed you. You magically respawn.", _mob_name));
            }

            break;
        }
        case battle_update_response::type: {
            auto resp_msg = dynamic_cast<battle_update_response const*>(msg);

            if(!resp_msg) {
                return;
            }

            _current_battle_log.emplace_back(fmt::format("{} hit you {} out of {} times for {} dmg!", _mob_name, resp_msg->mob_hits, resp_msg->mob_turns, resp_msg->mob_damage));
            _current_battle_log.emplace_back(fmt::format("You hit {} {} out of {} times for {} dmg!", _mob_name, resp_msg->player_hits, resp_msg->player_turns, resp_msg->player_damage));
            _mob_current_hp -= resp_msg->player_damage;
            _player_current_hp -= resp_msg->mob_damage;
            break;
        }
        case level_up_response::type: {
            auto resp_msg = dynamic_cast<level_up_response const*>(msg);

            if(!resp_msg) {
                return;
            }

            fmt::memory_buffer out{};
            format_to(out, "You leveled up! New stats: ");
            for(auto &stat : resp_msg->added_stats) {
                format_to(out, "{}: +{}, ", stat.first, stat.second.value);
            }

            _battle_log.emplace_back(to_string(out));
            break;
        }
        case new_battle_response::type: {
            auto resp_msg = dynamic_cast<new_battle_response const*>(msg);

            if(!resp_msg) {
                return;
            }

            _mob_name = resp_msg->mob_name;
            _battle_log.emplace_back(fmt::format("You enter battle with a level {} {}", resp_msg->mob_level, _mob_name));
            _current_battle_log.clear();
            _mob_current_hp = resp_msg->mob_hp;
            _mob_max_hp = resp_msg->mob_max_hp;
            _player_current_hp = resp_msg->player_hp;
            _player_max_hp = resp_msg->player_max_hp;
            break;
        }
        default: {
            break;
        }
    }
}
