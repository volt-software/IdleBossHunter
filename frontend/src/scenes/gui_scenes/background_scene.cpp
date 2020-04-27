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

#include "background_scene.h"
#include <rendering/texture_atlas.h>
#include <rendering/sprite.h>
#include "spdlog/spdlog.h"
#include <filesystem>
#include <init/sdl_init.h>

using namespace std;
using namespace ibh;

background_scene::background_scene() : scene(generate_type<background_scene>()), _atlasses(), _sprites(), _microseconds_between_switch(30'000'000), _microsecond_count(0), _bg_to_render(0) {
    for(auto& p: filesystem::directory_iterator("assets/backgrounds")) {
        spdlog::info("[{}] adding background {}", __FUNCTION__, p.path().c_str());
        auto temp_proj = glm::ortho(0.0F, 1920.0F, 1080.0F,
                                    0.0F, -1.0F, 1.0F);;
        auto &atlas = _atlasses.emplace_back(make_shared<texture_atlas>(p.path().c_str(), "assets/shaders/triangle_vertex.shader", "assets/shaders/triangle_fragment.shader", temp_proj, 2));
        _sprites.emplace_back(make_unique<sprite>(atlas, glm::vec4(0, 0, 1920, 1080), std::optional<glm::vec4> {}));
    }
}

background_scene::~background_scene() {
    _sprites.clear();
    _atlasses.clear();
}

void background_scene::update(iscene_manager *manager, TimeDelta dt) {
    _atlasses[_bg_to_render]->render();

    _microsecond_count += dt;
    while(_microsecond_count >= _microseconds_between_switch) {
        _microsecond_count -= _microseconds_between_switch;
        _bg_to_render++;
        if(_bg_to_render + 1 >= _atlasses.size()) {
            _bg_to_render = 0;
        }
    }
}

void background_scene::handle_message(iscene_manager *manager, uint64_t type, message const *msg) {

}
