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

using namespace std;
using namespace ibh;

void scene_system::update(entt::registry &es, TimeDelta dt) {
    for(auto& scene : _scenes) {
        scene->update(this, es, dt);
        if(scene->_closed) {
            remove(scene.get());
        }
    }

    if(_force_goto_scene) {
        spdlog::info("[scene_system] force goto new scene");
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
        spdlog::error("[scene_system] force goto scene wasn't empty");
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
