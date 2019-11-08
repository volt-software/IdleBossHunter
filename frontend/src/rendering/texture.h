/*
    Realm of Aesir client
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

class texture {
    public:
        texture(GLuint texture_id, uint32_t reference_count, uint32_t width, uint32_t height) noexcept;
        texture(texture&&) = default;
        texture(const texture&) = default;

        GLuint const _texture_id;
        uint32_t _reference_count;
        uint32_t const _width;
        uint32_t const _height;
};
