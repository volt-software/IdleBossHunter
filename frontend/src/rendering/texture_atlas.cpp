/*
    Realm of Aesir client
    Copyright (C) 2024  Michael de Lang

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

#include "texture_atlas.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <glm/gtc/type_ptr.hpp>
#include "spdlog/spdlog.h"

#include "shader_utils.h"
#include "texture_manager.h"
#include "sprite.h"

using namespace std;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

texture_atlas::texture_atlas(string const & image, string const & vertex_shader, string const & fragment_shader,
    glm::mat4 const projection_matrix, uint32_t capacity) :
        _image(image), _texture(create_texture_from_image(image)), _capacity(capacity), _highest_allocated(0), _allocated_at_least_one(false) {
    _program_id = create_shader_program(vertex_shader, fragment_shader);
    _projection = projection_matrix;

    glGenBuffers(1, &_buffer_object);
    glGenVertexArrays(1, &_vertex_array);
    glBindVertexArray(_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, _buffer_object);
    glBufferData(GL_ARRAY_BUFFER, capacity * 24 * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(_program_id);

    _projection_location = glGetUniformLocation(_program_id, "projection");
    if(_projection_location < 0) {
        spdlog::error("[texture_atlas] projection location not found in shader");
        throw runtime_error("[texture_atlas] projection location not found in shader");
    }
    glUniformMatrix4fv(_projection_location, 1, GL_FALSE, glm::value_ptr(_projection));

    _textureunit_location = glGetUniformLocation(_program_id, "textureUnit");
    if(_textureunit_location < 0) {
        spdlog::error("[texture_atlas] textureUnit not found in shader");
        throw runtime_error("[texture_atlas] textureUnit not found in shader");
    }
    glUniform1i(_textureunit_location, 0);

    glUseProgram(0);
}

texture_atlas::~texture_atlas() noexcept {
    glDeleteBuffers(1, &_buffer_object);
    glDeleteVertexArrays(1, &_vertex_array);
    glDeleteProgram(_program_id);
    delete_texture(_image);
}

void texture_atlas::render() const noexcept {
    if(!_allocated_at_least_one) {
        return;
    }

    glUseProgram(_program_id);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, _texture._texture_id);
    glBindVertexArray(_vertex_array);

    glDrawArrays(GL_TRIANGLES, 0, 6 * (_highest_allocated + 1));

    glBindVertexArray(0);

    glUseProgram(0);
}

void texture_atlas::set_projection(glm::mat4& projection) noexcept {
    _projection = projection;
    glUseProgram(_program_id);
    glUniformMatrix4fv(_projection_location, 1, GL_FALSE, glm::value_ptr(_projection));
    glUseProgram(0);
}

uint32_t texture_atlas::add_data_object(sprite *sprite) {
    optional<uint32_t> foundLocation;
    if(!_vertex_data_unused.empty()) {
        foundLocation = _vertex_data_unused.front();
        _vertex_data_unused.pop();
    }

    glBindBuffer(GL_ARRAY_BUFFER, _buffer_object);
    uint32_t i;

    if(foundLocation) {
        i = foundLocation.value();
    } else {
        i = _highest_allocated + 1;

        if(i == _capacity) {
            _capacity *= 2;
            spdlog::debug("[texture_atlas] resizing capacity to {}", _capacity);

            glBufferData(GL_ARRAY_BUFFER, _capacity * 24 * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

            int j = 0;
            for(auto old_sprite : _sprites) {
                glBufferSubData(GL_ARRAY_BUFFER, j * 24 * sizeof(uint32_t), 24 * sizeof(uint32_t), old_sprite->get_vertex_data().data());
                old_sprite->_vertex_data_position = j;
                j++;
            }
        }
    }

    if(i > _highest_allocated) {
        _highest_allocated = i;
    }

    glBufferSubData(GL_ARRAY_BUFFER, i * 24 * sizeof(uint32_t), 24 * sizeof(uint32_t), sprite->get_vertex_data().data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    _allocated_at_least_one = true;
    _sprites.push_back(sprite);
    return i;
}

bool texture_atlas::update_data_object(sprite *sprite) {
    if(sprite->_vertex_data_position > _capacity) {
        spdlog::error("[texture_atlas] update_data_object position out of bounds, got {} expected less than {}", sprite->_vertex_data_position, _capacity);
        throw runtime_error("[texture_atlas] update_data_object position out of bounds");
    }

    glBindBuffer(GL_ARRAY_BUFFER, _buffer_object);
    glBufferSubData(GL_ARRAY_BUFFER, sprite->_vertex_data_position * 24 * sizeof(uint32_t), 24 * sizeof(uint32_t), sprite->get_vertex_data().data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void texture_atlas::remove_data_object(sprite *sprite) {
    if(sprite->_vertex_data_position > _capacity) {
        spdlog::error("[texture_atlas] update_data_object position out of bounds, got {} expected less than {}", sprite->_vertex_data_position, _capacity);
        throw runtime_error("[texture_atlas] update_data_object position out of bounds");
    }

    glBindBuffer(GL_ARRAY_BUFFER, _buffer_object);
    glBufferSubData(GL_ARRAY_BUFFER, sprite->_vertex_data_position * 24 * sizeof(uint32_t), 24 * sizeof(uint32_t), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _vertex_data_unused.push(sprite->_vertex_data_position);
}

uint32_t texture_atlas::texture_width() const noexcept {
    return _texture._width;
}

uint32_t texture_atlas::texture_height() const noexcept {
    return _texture._height;
}
