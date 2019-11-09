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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <memory>
#include "spdlog/spdlog.h"
#include <memory>
#include <rendering/imgui/imgui.h>
#include <rendering/imgui/imgui_impl_sdl.h>
#include <rendering/imgui/imgui_impl_opengl3.h>
#include <ecs/ecs.h>
#include <ecs/systems/rendering_system.h>
#include <ecs/components/atlas_component.h>
#include <ecs/systems/scene_system.h>
#include <thread>
#include <numeric>

//#include "threadpool.h"
#include "rendering/shader_utils.h"
#include "timer.h"
#include "rendering/sprite.h"
#include "rendering/texture_atlas.h"
#include "config.h"
//#include "map_layout_config.h"
#include "map_loading/map.h"
#include "init/sdl_init.h"

using namespace std;
using namespace fresh;

namespace fresh {
    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
    glm::mat4 projection;
}

ImGuiIO& init_imgui() {
    if(!IMGUI_CHECKVERSION()) {
        spdlog::error("[main] Error with imgui check version.");
        exit(1);
    }

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("assets/fonts/TheanoDidot-Regular.ttf", 12);

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(nullptr);

    return io;
}

void set_threads_config(config& config) {
    int threads = 0;
    if(config.threads <= 0) {
        threads = thread::hardware_concurrency();

        if(threads == 0) {
            throw runtime_error("Could not auto-detect number of cpu cores. Please configure threads setting manually.");
        }

        config.threads = threads;
        spdlog::info("[main] auto-detected {} cores, creating {} background threads", threads, threads);
    } else {
        threads = config.threads;
        spdlog::info("[main] creating {} background threads", threads);
    }

    if(threads < 8) {
        spdlog::warn("[main] less than 8 threads configured, unlikely to perform well. Consider upgrading your CPU");
    }
}

void close() noexcept
{
    SDL_StopTextInput();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    window = nullptr;

    IMG_Quit();

    SDL_Quit();
}

void set_working_dir() noexcept {
    char *base_path = SDL_GetBasePath();
    if (base_path) {
        spdlog::info("[main] Set base_path to {}", base_path);
        if(chdir(base_path) != 0) {
            spdlog::warn("[main] chdir returned error {}", errno);
        }
        SDL_free(base_path);
    } else {
        spdlog::error("[main] Couldn't get base path: {}", SDL_GetError());
        exit(1);
    }

}

std::function<void()> loop;
void main_loop() { if(loop) loop(); }

#ifdef WINDOWS
extern "C" int main(int argc, char* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    set_working_dir();

    config config{};
#ifdef __EMSCRIPTEN__
    config.debug_level = "trace";
    config.tick_length = 50;
    config.log_fps = true;
    config.screen_width = 1024;
    config.screen_height = 768;
#else
    try {
        auto config_opt = parse_env_file();
        if(!config_opt) {
            return 1;
        }
        config = config_opt.value();
    } catch (const exception& e) {
        spdlog::error("[main] config/game_config.json file is malformed json.");
        return 1;
    }

    map_layout layout;
    try {
        auto layout_opt = parse_map_layout_file();
        if(!layout_opt) {
            return 1;
        }
        layout = layout_opt.value();
    } catch (const exception& e) {
        spdlog::error("[main] config/map_layout.json file is malformed json.");
        return 1;
    }


    reconfigure_logger(config);
#endif

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 0);
#endif

    init_sdl(config);
    init_sdl_image();
    init_sdl_mixer();
    set_threads_config(config);
    auto& io = init_imgui();

    bool quit = false;

    SDL_Event e;

    timer<microseconds> fps_timer{};
    timer<microseconds> tick_timer{};
    timer<microseconds> bench_timer{};
    vector<uint64_t> frame_times{};
    frame_times.reserve(config.refresh_rate);
    int counted_frames = 0;
    //ThreadPool thread_pool(config.threads);

    Mix_Chunk *sfx1 = Mix_LoadWAV("assets/sfx/Slash01.wav");
    Mix_Chunk *sfx2 = Mix_LoadWAV("assets/sfx/Slash02.wav");
    Mix_Chunk *sfx3 = Mix_LoadWAV("assets/sfx/Slash03.wav");

    Mix_Music *mus1 = Mix_LoadMUS("assets/music/8bit Stage1 Loop.wav");
    //Mix_PlayMusic(mus1, 0);

    auto map = fresh::map::load_from_file("./assets/maps/");

    fps_timer.start();
    tick_timer.start();

    entt::registry es{};
    rendering_system rs(config, window);
    scene_system ss(config);
    ss.init_main_menu();

    loop = [&] {
        while (SDL_PollEvent(&e) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            }

            if (io.WantCaptureKeyboard) {
                continue;
            }

            switch(e.type) {
                case SDL_TEXTINPUT: {
                    int x = 0;
                    int y = 0;
                    SDL_GetMouseState(&x, &y);
                    break;
                }
                case SDL_KEYDOWN: {
                    if (e.key.keysym.sym == SDLK_1) {
                        Mix_PlayChannel(-1, sfx1, 0);
                    } else if (e.key.keysym.sym == SDLK_2) {
                        Mix_PlayChannel(-1, sfx2, 0);
                    } else if (e.key.keysym.sym == SDLK_3) {
                        Mix_PlayChannel(-1, sfx3, 0);
                    } else if (e.key.keysym.sym == SDLK_p) {
                        if (Mix_PlayingMusic()) {
                            if (Mix_PausedMusic()) {
                                Mix_ResumeMusic();
                            } else {
                                Mix_PauseMusic();
                            }
                        } else {
                            Mix_PlayMusic(mus1, 0);
                        }
                    }
                    break;
                }
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    config.screen_width = e.window.data1;
                    config.screen_height = e.window.data2;
                    spdlog::info("Resize to {}x{}", config.screen_width, config.screen_height);
                    projection = glm::ortho(0.0f, (float) config.screen_width, (float) config.screen_height, 0.0f, -1.0f, 1.0f);
                    break;
                }
            }
        }

        bench_timer.start();
        rs.update(es, 1);
        ss.update(es, 1);
        rs.end_rendering();

        if (config.log_fps) {
            frame_times.push_back(bench_timer.get_ticks());
        }

        ++counted_frames;

        auto fps_ticks = fps_timer.get_ticks();
        if (config.log_fps && fps_ticks > 1'000'000) {
            spdlog::info("[main] FPS {}-{} - frame times max/avg/min: {} / {} / {} µs", counted_frames, counted_frames / (fps_ticks / 1'000'000.f),
                         *max_element(begin(frame_times), end(frame_times)), accumulate(begin(frame_times), end(frame_times), 0ul) / frame_times.size(),
                         *min_element(begin(frame_times), end(frame_times)));
            fps_timer.start();
            frame_times.clear();
            counted_frames = 0;
        }
    };

#ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (1) main_loop();
#endif

    spdlog::info("quitting");

    Mix_FreeChunk(sfx1);
    Mix_FreeChunk(sfx2);
    Mix_FreeChunk(sfx3);

    Mix_FreeMusic(mus1);

    close();

    return 0;
}
