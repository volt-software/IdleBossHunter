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

#include "scene_system.h"
#include "spdlog/spdlog.h"
#include <scenes/gui_scenes/alpha_window_scene.h>
#include <scenes/gui_scenes/main_menu_scene.h>
#include <scenes/gui_scenes/connecting_scene.h>
#include <messages/chat/message_response.h>
#include <messages/user_access/login_response.h>
#include <messages/user_access/character_select_response.h>
#include <messages/user_access/create_character_response.h>
#include <messages/user_access/delete_character_response.h>
#include <messages/user_access/user_entered_game_response.h>
#include <messages/user_access/user_left_game_response.h>
#include <messages/user_access/play_character_response.h>
#include <messages/generic_error_response.h>
#include <messages/generic_ok_response.h>
#include <messages/battle/new_battle_response.h>
#include <messages/battle/level_up_response.h>
#include <messages/battle/battle_update_response.h>
#include <messages/battle/battle_finished_response.h>
#include <messages/company/accept_application_response.h>
#include <messages/company/create_company_response.h>
#include <messages/company/get_company_applications_response.h>
#include <messages/company/get_company_listing_response.h>
#include <messages/company/increase_bonus_response.h>
#include <messages/company/join_company_response.h>
#include <messages/company/leave_company_response.h>
#include <messages/company/reject_application_response.h>
#include <messages/company/set_tax_response.h>


using namespace std;
using namespace ibh;

scene_system::scene_system(config *config, entt::registry &es)
        : _config(config), _scenes(), _scenes_to_erase(), _scenes_to_add(), _es(es), _character(), _id_counter(0), _logged_in(), _connected() {
}

void scene_system::update(entt::registry &unused, TimeDelta dt) {
    scoped_lock sg(_m);
    for(auto const & scene : _scenes) {
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

    for(auto const &scene_id : _scenes_to_erase) {
        _scenes.erase(remove_if(begin(_scenes), end(_scenes), [scene_id](const unique_ptr<scene> &sc) noexcept { return sc->_id == scene_id; }), end(_scenes));
    }

    _scenes.reserve(_scenes.size() + _scenes_to_add.size());
    move(begin(_scenes_to_add), end(_scenes_to_add), back_inserter(_scenes));

    _scenes_to_add.clear();
    _scenes_to_erase.clear();
}

void scene_system::remove(scene *old_scene) {
    _scenes_to_erase.push_back(old_scene->_id);
    // do not delete pointer
}

void scene_system::add(unique_ptr<scene> new_scene) {
    for(auto const &scene : _scenes) {
        if(scene->_type == new_scene->_type) {
            return;
        }
    }

    new_scene->_id = _id_counter++;
    _scenes_to_add.emplace_back(move(new_scene));
}

void scene_system::force_goto_scene(unique_ptr<scene> new_scene) {
    if(_force_goto_scene) {
        spdlog::error("[{}] force goto scene wasn't empty", __FUNCTION__);
        throw runtime_error("force goto scene wasn't empty");
    }

    _force_goto_scene = move(new_scene);
}

config * scene_system::get_config() {
    return _config;
}

void scene_system::init_connection_screen() {
    auto connecting_menu = make_unique<connecting_scene>();
    connecting_menu->_id = _id_counter++;
    spdlog::info("connecting menu id {}", connecting_menu->_id);
    _scenes.push_back(move(connecting_menu));
}

void scene_system::init_main_menu() {
    _scenes[0]->_closed = true;
    auto main_menu = make_unique<main_menu_scene>();
    main_menu->_id = _id_counter++;
    spdlog::info("main menu id {}", main_menu->_id);
    auto alpha_window = make_unique<alpha_window_scene>();
    alpha_window->_id = _id_counter++;
    spdlog::info("alpha window id {}", alpha_window->_id);
    _scenes.push_back(move(main_menu));
    _scenes.push_back(move(alpha_window));
}

unique_ptr<message> deserialize_message(uint64_t const &type, rapidjson::Document const &d) {
    switch(type){
        // UAC
        case login_response::type:
            return login_response::deserialize(d);
        case generic_error_response::type:
            return generic_error_response::deserialize(d);
        case generic_ok_response::type:
            return generic_ok_response::deserialize(d);
        case character_select_response::type:
            return character_select_response::deserialize(d);
        case create_character_response::type:
            return create_character_response::deserialize(d);
        case delete_character_response::type:
            return delete_character_response::deserialize(d);
        case user_entered_game_response::type:
            return user_entered_game_response::deserialize(d);
        case user_left_game_response::type:
            return user_left_game_response::deserialize(d);
        case play_character_response::type:
            return play_character_response::deserialize(d);

        // Chat
        case message_response::type:
            return message_response::deserialize(d);

        // Companies
        case accept_application_response::type:
            return accept_application_response::deserialize(d);
        case create_company_response::type:
            return create_company_response::deserialize(d);
        case get_company_applications_response::type:
            return get_company_applications_response::deserialize(d);
        case get_company_listing_response::type:
            return get_company_listing_response::deserialize(d);
        case increase_bonus_response::type:
            return increase_bonus_response::deserialize(d);
        case join_company_response::type:
            return join_company_response::deserialize(d);
        case leave_company_response::type:
            return leave_company_response::deserialize(d);
        case reject_application_response::type:
            return reject_application_response::deserialize(d);
        case set_tax_response::type:
            return set_tax_response::deserialize(d);

        // Gameplay
        case new_battle_response::type:
            return new_battle_response::deserialize(d);
        case level_up_response::type:
            return level_up_response::deserialize(d);
        case battle_update_response::type:
            return battle_update_response::deserialize(d);
        case battle_finished_response::type:
            return battle_finished_response::deserialize(d);
        default:
            return nullptr;
    }
}

void scene_system::handle_message(rapidjson::Document const &d) {
    auto type = d["type"].GetUint64();
    auto msg = deserialize_message(type, d);

    if(!msg) {
        spdlog::error("[{}] No message for type {}", __FUNCTION__, type);
        return;
    }

    spdlog::trace("[{}] Handling message type {} for {} scenes", __FUNCTION__, type, _scenes.size());

    scoped_lock sg(_m);
    for(auto const & scene : _scenes) {
        scene->handle_message(this, type, msg.get());
    }
}

entt::registry &scene_system::get_entity_registry() {
    return _es;
}

socket_component& scene_system::get_socket() const {
    auto view = _es.view<socket_component>();
    for (auto entity : view) {
        auto &socket = view.get<socket_component>(entity);
        return socket;
    }

    spdlog::error("[{}] Couldn't find socket", __FUNCTION__);
    throw runtime_error("Couldn't find socket");
}

void scene_system::set_logged_in(bool logged_in) {
    _logged_in = logged_in;
}

bool scene_system::get_logged_in() const {
    return _logged_in;
}

void scene_system::set_connected(bool connected) {
    _connected = connected;

    if(connected) {
        init_main_menu();
    }
}

bool scene_system::get_connected() const {
    return _connected;
}

optional<character_object>& scene_system::get_character() {
    return _character;
}
