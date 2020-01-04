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

#include "rendering_system.h"
#include "spdlog/spdlog.h"
#include <GL/glew.h>
#include <rendering/imgui/imgui.h>
#include <rendering/imgui/imgui_impl_sdl.h>
#include <rendering/imgui/imgui_impl_opengl3.h>
#include "../components.h"
#include "../../rendering/sprite.h"
#include "../../rendering/texture_atlas.h"

using namespace std;
using namespace ibh;

void rendering_system::update(entt::registry &es, TimeDelta dt) {
    SDL_GL_MakeCurrent(_window, _context);
    glClear(GL_COLOR_BUFFER_BIT);

    auto view = es.view<atlas_component>();
    for(auto entity : view) {
        atlas_component const &atlas_comp = view.get(entity);
        atlas_comp.atlas->render();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_window);
    ImGui::NewFrame();
    if(_config->show_demo_window) {
        ImGui::ShowDemoWindow();
    }
}

void rendering_system::end_rendering() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(_window);
}
