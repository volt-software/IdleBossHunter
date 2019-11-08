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

#include "texture.h"

texture::texture(GLuint texture_id, uint32_t reference_count, uint32_t width, uint32_t height) noexcept
    : _texture_id(texture_id), _reference_count(reference_count), _width(width), _height(height) {

}

// Normally there'd be a destructor here, but checking the reference count is the duty of texture_manager.
// So no destructor necessary.
