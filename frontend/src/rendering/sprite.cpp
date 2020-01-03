/*
    IdleBossHunter client
    Copyright (C) 2016  Michael de Lang

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

#include "sprite.h"

#include <array>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "spdlog/spdlog.h"

#include "shader_utils.h"
#include "texture_manager.h"

using namespace std;

ibh::sprite::sprite(std::shared_ptr<texture_atlas> texture_atlas, glm::vec4 const position, optional<glm::vec4> const clip) noexcept
     : _texture_atlas(move(texture_atlas)), _position(position), _clip(clip) {
    _vertex_data_position = _texture_atlas->add_data_object(this);
}

ibh::sprite::~sprite() noexcept {
    _texture_atlas->remove_data_object(this);
}

void ibh::sprite::set_position(glm::vec4 &position) noexcept {
    _position = position;
    _texture_atlas->update_data_object(this);
}

array<GLfloat, 24> ibh::sprite::get_vertex_data() {
    float x = _position.x;
    float y = _position.y;
    float w = _position.z;
    float h = _position.w;

    array<GLfloat, 24> vertex_data = {
            x, y, 0.0F, 0.0F,
            x+w, y, 1.0F, 0.0F,
            x, y+h, 0.0F, 1.0F,

            x+w, y, 1.0F, 0.0F,
            x, y+h, 0.0F, 1.0F,
            x+w, y+h, 1.0F, 1.0F
    };

    if(_clip) {
        if(_clip.value().x < 0 || _clip.value().x > _texture_atlas->texture_width() || _clip.value().y < 0 || _clip.value().y > _texture_atlas->texture_height()) {
            spdlog::error("[sprite] clip out of bounds");
            throw runtime_error("[sprite] clip out of bounds");
        }

        vertex_data[2] = _clip.value().x / _texture_atlas->texture_width();
        vertex_data[3] = _clip.value().y / _texture_atlas->texture_height();

        vertex_data[6] = (_clip.value().x + _clip.value().z) / _texture_atlas->texture_width();
        vertex_data[7] = _clip.value().y / _texture_atlas->texture_height();

        vertex_data[10] = _clip.value().x / _texture_atlas->texture_width();
        vertex_data[11] = (_clip.value().y + _clip.value().w) / _texture_atlas->texture_height();

        vertex_data[14] = (_clip.value().x + _clip.value().z) / _texture_atlas->texture_width();
        vertex_data[15] = _clip.value().y / _texture_atlas->texture_height();

        vertex_data[16] = _clip.value().x / _texture_atlas->texture_width();
        vertex_data[17] = (_clip.value().y + _clip.value().w) / _texture_atlas->texture_height();

        vertex_data[20] = (_clip.value().x + _clip.value().z) / _texture_atlas->texture_width();
        vertex_data[21] = (_clip.value().y + _clip.value().w) / _texture_atlas->texture_height();
    }

    return vertex_data;
}

glm::vec4 ibh::sprite::get_position() noexcept {
    return _position;
}
