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

#include "texture_manager.h"

#include <SDL.h>
#include <SDL_image.h>
#include "spdlog/spdlog.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <robin_hood.h>
#pragma GCC diagnostic pop

#include "shader_utils.h"

using namespace std;

robin_hood::unordered_map<string, texture> texture_cache;

texture create_texture_from_image(string const & image) {

    auto found_texture = texture_cache.find(image);

    if(found_texture != texture_cache.end()) {
        found_texture->second._reference_count++;
        return found_texture->second;
    }

    SDL_Surface *surface = IMG_Load(image.c_str());
    if(!surface) {
        spdlog::error("[{}] surface load failed: {}", __FUNCTION__, IMG_GetError());
        throw runtime_error("surface load failed");
    }

    GLenum texture_format = 0;
    GLenum internal_format = 0;
    GLenum texture_type = 0;

    if(surface->format->BytesPerPixel == 4) {
        spdlog::info("[{}] 4 byte image", __FUNCTION__);
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGBA;
            texture_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        } else {
            texture_format = GL_BGRA;
            texture_type = GL_UNSIGNED_INT_8_8_8_8;
            spdlog::info("[{}] GL_BGRA", __FUNCTION__);
        }
        internal_format = GL_RGBA8;
    } else if(surface->format->BytesPerPixel == 3) {
        spdlog::info("[{}] 3 byte image", __FUNCTION__);
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGB;
            texture_type = GL_UNSIGNED_BYTE;
        } else {
            texture_format = GL_BGR;
            texture_type = GL_UNSIGNED_BYTE;
            spdlog::info("[{}] GL_BGR", __FUNCTION__);
        }
        internal_format = GL_RGB8;
    } else {
        spdlog::error("[{}] image {} unknown BPP {}", __FUNCTION__, image, (int)surface->format->BytesPerPixel);
        throw runtime_error("image unknown BPP");
    }

    spdlog::info("[{}] size {}x{}", __FUNCTION__, surface->w, surface->h);

    uint32_t alignment = 8;
    spdlog::info("[{}] surface->pitch {}x{}", __FUNCTION__, surface->pitch);
    while (surface->pitch % alignment) { // x%1==0 for any x
        alignment >>= 1;
    }
    spdlog::info("[{}] alignment {}", __FUNCTION__, alignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, texture_format, texture_type, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(surface);

    texture tex = texture(texture_id, 1, surface->w, surface->h);
    auto succeeded = texture_cache.emplace(image, tex).second;

    if(!succeeded) {
        spdlog::error("[{}] Couldn't insert texture into cache {}", __FUNCTION__, image);
        throw runtime_error("Couldn't insert texture into cache");
    }

    spdlog::info("[{}] created texture {}", __FUNCTION__, texture_id);

    return tex;
}

GLuint create_shader_program(string const & vertex_shader, string const & fragment_shader) {
    GLuint program_id = glCreateProgram();

    auto vertexShaderMaybe = load_shader_from_file(vertex_shader, GL_VERTEX_SHADER);
    if(!vertexShaderMaybe) {
        spdlog::error("[{}] Couldn't load vertex shader file {}", __FUNCTION__, vertex_shader);
        throw runtime_error("Couldn't load vertex shader file");
    }

    glAttachShader(program_id, vertexShaderMaybe.value());

    auto fragmentShaderMaybe = load_shader_from_file(fragment_shader, GL_FRAGMENT_SHADER);
    if(!fragmentShaderMaybe) {
        spdlog::error("[{}] Couldn't load fragment shader file {}", __FUNCTION__, fragment_shader);
        throw runtime_error("Couldn't load fragment shader file");
    }

    glAttachShader(program_id, fragmentShaderMaybe.value());

    glLinkProgram(program_id);

    glDeleteShader(vertexShaderMaybe.value());
    glDeleteShader(fragmentShaderMaybe.value());

    GLint programSucces = GL_TRUE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &programSucces);
    if(programSucces != GL_TRUE) {
        spdlog::error("[{}] Couldn't link program {}", __FUNCTION__, program_id);
        print_program_log(program_id);
        throw runtime_error("Couldn't link program");
    }

    spdlog::info("[{}] Created shader program {}", __FUNCTION__, program_id);

    return program_id;
}

void delete_texture(std::string const & image) {
    auto found_texture = texture_cache.find(image);

    if(found_texture != texture_cache.end()) {
        found_texture->second._reference_count--;

        if(found_texture->second._reference_count == 0) {
            spdlog::info("[{}] deleting texture {}", __FUNCTION__, found_texture->second._texture_id);
            glDeleteTextures(1, &found_texture->second._texture_id);
            texture_cache.erase(image);
        }
    } else {
        spdlog::error("[{}] couldn't delete texture {}", __FUNCTION__, image);
        throw runtime_error("couldn't delete texture");
    }
}
