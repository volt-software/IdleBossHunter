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
#include "ibh_containers.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <SDL.h>
#include <algorithm>

using namespace std;
using namespace ibh;

show_characters_scene::show_characters_scene(iscene_manager *manager, vector<character_object> characters) : _characters(move(characters)), _races(), _classes(), _show_create(false), _waiting_for_select(true), _error(), _selected_race(), _selected_class() {
    send_message<character_select_request>(manager);
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

        if (ImGui::Button("Play") && selected_char_slot >= 0) {
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

        if (ImGui::BeginCombo("Race", _selected_race.c_str()))
        {
            for(auto &race : _races) {
                if (ImGui::Selectable(race.name.c_str(), _selected_race == race.name)) {
                    _selected_race = race.name;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Class", _selected_class.c_str()))
        {
            for(auto &c : _classes) {
                if (ImGui::Selectable(c.name.c_str(), _selected_class == c.name)) {
                    _selected_class = c.name;
                }
            }
            ImGui::EndCombo();
        }

        if(!_selected_class.empty() && !_selected_race.empty()) {
            ibh_flat_map<string, int64_t> combined_stats;

            vector<character_race>::iterator r = find_if(begin(_races), end(_races), [&_selected_race = _selected_race](const character_race &race){return race.name == _selected_race;});
            vector<character_class>::iterator c = find_if(begin(_classes), end(_classes), [&_selected_class = _selected_class](const character_class &_class){return _class.name == _selected_class;});

            if(r == end(_races) || c == end(_classes)) {
                spdlog::error("[{}] something went wrong with calculating stats. Please report this as a bug.", __FUNCTION__);
                ImGui::End();
                return;
            }

            for(auto &stat : r->level_stat_mods) {
                ImGui::Text("%s %s: %lli", r->name.c_str(), stat.name.c_str(), stat.value);
                combined_stats[stat.name] += stat.value;
            }

            for(auto &stat : c->stat_mods) {
                ImGui::Text("%s %s: %lli", c->name.c_str(), stat.name.c_str(), stat.value);
                combined_stats[stat.name] += stat.value;
            }

            for(auto &stat : c->stat_mods) {
                ImGui::Text("%s: %lli", stat.name.c_str(), combined_stats[stat.name]);
            }
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
