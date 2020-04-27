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

#pragma once

#ifndef __gl_h_
#include <GL/glew.h>
#endif

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <memory>
#include <optional>

#include "texture_atlas.h"
namespace ibh {
    class sprite {
    public:
        sprite(std::shared_ptr<texture_atlas> texture_atlas, glm::vec4 const position,
               std::optional<glm::vec4> const clip) noexcept;

        sprite(sprite &&) = default;

        ~sprite() noexcept;
        void set_position(glm::vec4 &position) noexcept;
        [[nodiscard]] glm::vec4 get_position() noexcept;
        [[nodiscard]] std::array<GLfloat, 24> get_vertex_data();

        uint32_t _vertex_data_position;
    private:
        std::shared_ptr<texture_atlas> _texture_atlas;
        glm::vec4 _position;
        std::optional<glm::vec4> _clip;
    };
}