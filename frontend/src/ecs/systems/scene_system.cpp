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

#include "scene_system.h"
#include "spdlog/spdlog.h"
#include <scenes/gui_scenes/alpha_window_scene.h>
#include <scenes/gui_scenes/main_menu_scene.h>
#include <messages/chat/message_response.h>
#include <messages/user_access/login_response.h>
#include <messages/user_access/character_select_response.h>
#include <messages/user_access/create_character_response.h>
#include <messages/user_access/user_entered_game_response.h>
#include <messages/user_access/user_joined_response.h>
#include <messages/user_access/user_left_game_response.h>
#include <messages/user_access/user_left_response.h>
#include <messages/generic_error_response.h>
#include <messages/generic_ok_response.h>

using namespace std;
using namespace ibh;

scene_system::scene_system(config *config, entt::registry &es)
        : _config(config), _scenes(), _scenes_to_erase(), _scenes_to_add(), es(es), _id_counter(0) {
}

void scene_system::update(entt::registry &es, TimeDelta dt) {
    for(auto& scene : _scenes) {
        scene->update(this, dt);
        if(scene->_closed) {
            remove(scene.get());
        }
    }

    if(_force_goto_scene) {
        spdlog::info("[{}] force goto new scene", __FUNCTION__);
        _scenes.clear();
        _scenes_to_add.clear();
        _scenes_to_erase.clear();

        _scenes.emplace_back(move(_force_goto_scene));
        _force_goto_scene = nullptr;
        return;
    }

    for(auto &scene_id : _scenes_to_erase) {
        _scenes.erase(remove_if(begin(_scenes), end(_scenes), [=](const unique_ptr<scene> &sc) { return sc->_id == scene_id; }), end(_scenes));
    }

    _scenes.reserve(_scenes.size() + _scenes_to_add.size());
    move(begin(_scenes_to_add), end(_scenes_to_add), back_inserter(_scenes));

    _scenes_to_add.clear();
    _scenes_to_erase.clear();
}

void scene_system::remove(scene *old_scene) {
    _scenes_to_erase.push_back(old_scene->_id);
}

void scene_system::add(scene *scene) {
    scene->_id = _id_counter++;
    _scenes_to_add.emplace_back(scene);
}

void scene_system::force_goto_scene(scene *new_scene) {
    if(_force_goto_scene) {
        spdlog::error("[{}] force goto scene wasn't empty", __FUNCTION__);
        throw runtime_error("force goto scene wasn't empty");
    }

    _force_goto_scene.reset(new_scene);
}

config * scene_system::get_config() {
    return _config;
}

void scene_system::init_main_menu() {
    auto main_menu = make_unique<main_menu_scene>();
    main_menu->_id = _id_counter++;
    spdlog::info("main menu id {}", main_menu->_id);
    auto alpha_window = make_unique<alpha_window_scene>();
    alpha_window->_id = _id_counter++;
    spdlog::info("alpha window id {}", alpha_window->_id);
    _scenes.push_back(move(main_menu));
    _scenes.push_back(move(alpha_window));
}

unique_ptr<message> deserialize_message(uint32_t &type, rapidjson::Document &d) {
    switch(type){
        case login_response::type: {
            return login_response::deserialize(d);
        }
        case generic_error_response::type: {
            return generic_error_response::deserialize(d);
        }
        case generic_ok_response::type: {
            return generic_ok_response::deserialize(d);
        }
        case character_select_response::type: {
            return character_select_response::deserialize(d);
        }
        case create_character_response::type: {
            return create_character_response::deserialize(d);
        }
        case user_entered_game_response::type: {
            return user_entered_game_response::deserialize(d);
        }
        case user_joined_response::type: {
            return user_joined_response::deserialize(d);
        }
        case user_left_game_response::type: {
            return user_left_game_response::deserialize(d);
        }
        case user_left_response::type: {
            return user_left_response::deserialize(d);
        }
        case message_response::type: {
            return message_response::deserialize(d);
        }
        default:
            return nullptr;
    }
}

void scene_system::handle_message(rapidjson::Document &d) {
    auto type = d["type"].GetUint();
    auto msg = deserialize_message(type, d);

    if(!msg) {
        spdlog::error("[{}] No message for type {}", __FUNCTION__, type);
        return;
    }

    spdlog::trace("[{}] Handling message type {} for {} scenes", __FUNCTION__, type, _scenes.size());

    for(auto& scene : _scenes) {
        scene->handle_message(this, type, msg.get());
    }
}

entt::registry &scene_system::get_entity_registry() {
    return es;
}

int scene_system::get_socket() {
    auto view = es.view<socket_component>();
    for (auto entity : view) {
        socket_component &socket = view.get<socket_component>(entity);
        return socket.socket;
    }

    spdlog::error("[{}] Couldn't find socket", __FUNCTION__);
    return -1;
}
