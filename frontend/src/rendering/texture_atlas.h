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
#include <array>
#include <vector>
#include <queue>

#include "texture.h"

class sprite;

class texture_atlas {
    public:
        texture_atlas(std::string const & image, std::string const & vertex_shader, std::string const & fragment_shader,
            glm::mat4 const projection_matrix, uint32_t capacity);

        ~texture_atlas() noexcept;

        void render() const noexcept;
        void set_projection(glm::mat4& projection) noexcept;
        uint32_t add_data_object(sprite *sprite);
        bool update_data_object(sprite *sprite);
        void remove_data_object(sprite *sprite);

        uint32_t texture_width() const noexcept;
        uint32_t texture_height() const noexcept;

    private:
        std::string const _image;
        texture _texture;
        GLuint _program_id;
        GLuint _buffer_object;
        GLuint _vertex_array;
        glm::vec4 _clip;
        glm::mat4 _projection;
        GLint _projection_location;
        GLint _textureunit_location;
        std::vector<sprite*> _sprites;
        std::queue<uint32_t> _vertex_data_unused;
        uint32_t _capacity;
        uint32_t _highest_allocated;
        bool _allocated_at_least_one;
};
