/*
    IdleBossHunter
    Copyright (C) 2019 Michael de Lang

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

#include "sdl_init.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>
#include "spdlog/spdlog.h"

void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    if(severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    spdlog::debug("[{}] message: {}", __FUNCTION__, message);

    switch(type) {
        case GL_DEBUG_TYPE_ERROR:
            spdlog::debug("[{}] type: ERROR", __FUNCTION__);
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            spdlog::debug("[{}] type: DEPRECATED_BEHAVIOUR", __FUNCTION__);
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            spdlog::debug("[{}] type: UNDEFINED_BEHAVIOR", __FUNCTION__);
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            spdlog::debug("[{}] type: PORTABILITY", __FUNCTION__);
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            spdlog::debug("[{}] type: PERFORMANCE", __FUNCTION__);
            break;
        case GL_DEBUG_TYPE_OTHER:
            spdlog::debug("[{}] type: OTHER", __FUNCTION__);
        default:
            break;
    }

    spdlog::debug("[{}] id: {}", __FUNCTION__, id);
    switch (severity) {
        case GL_DEBUG_SEVERITY_LOW:
            spdlog::debug("[{}] severity: LOW", __FUNCTION__);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            spdlog::debug("[{}] severity: MEDIUM", __FUNCTION__);
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            spdlog::debug("[{}] severity: HIGH", __FUNCTION__);
            break;
        default:
            spdlog::debug("[{}] severity: UNKNOWN {}", __FUNCTION__, severity);
    }
}

void ibh::init_sdl(config &config) noexcept {
#ifdef WINDOWS
    // see https://nlguillemot.wordpress.com/2016/12/11/high-dpi-rendering/
    HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    if (FAILED(hr))
    {
        _com_error err(hr);
        fwprintf(stderr, L"SetProcessDpiAwareness: %s\n", err.ErrorMessage());
    }
#endif

#ifdef __EMSCRIPTEN__
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "no");
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        spdlog::error("[{}] SDL Init went wrong: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

#ifndef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if (config.auto_fullscreen) {
        window = SDL_CreateWindow("IdleBossHunter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
    } else {
        window = SDL_CreateWindow("IdleBossHunter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  config.screen_width, config.screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    }
    if (window == nullptr) {
        spdlog::error("[{}] Couldn't initialize window: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

    int w, h, draw_w, draw_h;
    SDL_GetWindowSize(window, &w, &h);
    config.screen_width = w;
    config.screen_height = h;
    spdlog::info("[{}] Screen {}x{}", __FUNCTION__, w, h);

    SDL_GL_GetDrawableSize(window, &draw_w, &draw_h);
    spdlog::info("[{}] Drawable {}x{}", __FUNCTION__, draw_w, draw_h);


    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        spdlog::error("[{}] Couldn't initialize context: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        spdlog::error("[{}] Error initializing GLEW! {}", __FUNCTION__, glewGetErrorString(glewError));
        exit(1);
    }

    if (SDL_GL_SetSwapInterval(config.disable_vsync ? 0 : 1) < 0) {
        spdlog::error("[{}] Couldn't set vsync: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

    if (SDL_GL_MakeCurrent(window, context) < 0) {
        spdlog::error("[{}] Couldn't make OpenGL context current: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

    int display_index = SDL_GetWindowDisplayIndex(window);
    if (display_index < 0) {
        spdlog::error("[{}] Couldn't get current display index: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

    SDL_DisplayMode current;
    if (SDL_GetCurrentDisplayMode(display_index, &current) < 0) {
        spdlog::error("[{}] Couldn't get current display: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

    if (current.refresh_rate == 0) {
#ifdef __EMSCRIPTEN__
        spdlog::warn("[{}] Refresh rate unknown. Setting to 0 Hz.", __FUNCTION__);
        config.refresh_rate = 0;
#else
        spdlog::warn("[{}] Refresh rate unknown. Setting to 60 Hz.", __FUNCTION__);
        config.refresh_rate = 60;
#endif
    } else {
        config.refresh_rate = (uint32_t) current.refresh_rate;
    }

    spdlog::info("[{}] display properties: {}x{}@{}", __FUNCTION__, current.w, current.h, current.refresh_rate);

    glClearColor(0.f, 0.f, 0.f, 1.f);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SCISSOR_TEST);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

#ifndef __EMSCRIPTEN__
    if (glDebugMessageCallback) {
        spdlog::debug("[{}] Register OpenGL debug callback");
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
                              GL_DONT_CARE,
                              GL_DONT_CARE,
                              0,
                              &unusedIds,
                              true);
    } else {
        spdlog::error("[{}] glDebugMessageCallback not available");
    }
#else
    spdlog::error("[{}] glDebugMessageCallback not available", __FUNCTION__);
#endif

    SDL_StartTextInput();

    projection = glm::ortho(0.0f, (float) config.screen_width, (float) config.screen_height, 0.0f, -1.0f, 1.0f);

    config.user_event_type = SDL_RegisterEvents(1);
    if(config.user_event_type == std::numeric_limits<uint32_t>::max()){
        spdlog::error("[{}] Couldn't register event", __FUNCTION__);
        exit(1);
    }

    spdlog::info("[{}] Registered user event {}", __FUNCTION__, config.user_event_type);
}

void ibh::init_sdl_image() noexcept {
    int initted = IMG_Init(IMG_INIT_PNG);
    if ((initted & IMG_INIT_PNG) != IMG_INIT_PNG) {
        spdlog::error("[{}] SDL image init went wrong: {}", __FUNCTION__, IMG_GetError());
        exit(1);
    }
}

void ibh::init_sdl_mixer() noexcept {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        spdlog::error("[{}] Couldn't open audio: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }

    if(Mix_Init(MIX_INIT_OGG) <= 0) {
        spdlog::error("[{}] Couldn't init ogg audio: {}", __FUNCTION__, SDL_GetError());
        exit(1);
    }
}
