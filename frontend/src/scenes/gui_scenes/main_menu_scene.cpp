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

#include "main_menu_scene.h"
#include "settings_menu_scene.h"
#include "login_menu_scene.h"
#include "scenes/gui_scenes/companies/companies_overview_scene.h"
#include "scenes/gui_scenes/companies/manage_company_scene.h"
#include "scenes/gui_scenes/companies/create_company_scene.h"
#include "scenes/gui_scenes/resources/battle_log_scene.h"
#include "scenes/gui_scenes/resources/resource_overview_scene.h"
#include "scenes/gui_scenes/show_characters_scene.h"
#include <messages/resources/set_action_request.h>
#include "about_scene.h"
#include <rendering/imgui/imgui.h>
#include "spdlog/spdlog.h"
#include <magic_enum.hpp>

using namespace std;
using namespace ibh;

void main_menu_scene::update(iscene_manager *manager, TimeDelta dt) {
    if (ImGui::BeginMainMenuBar()) {
        if (!manager->get_logged_in() && ImGui::MenuItem("Login")) {
            manager->open_or_close(make_unique<login_menu_scene>());
        }

        if (manager->get_logged_in() && manager->get_logged_in() && ImGui::BeginMenu("Characters")) {
            if (ImGui::MenuItem("All Characters Overview")) {
                //manager->open_or_close(make_unique<show_characters_scene>());
            }

            if (ImGui::MenuItem("Current Character Stats")) {
                //manager->open_or_close(make_unique<resource_overview_scene>());
            }

            if (ImGui::MenuItem("Stop Playing Character")) {
                //manager->open_or_close(make_unique<resource_overview_scene>());
            }

            ImGui::EndMenu();
        }

        if (manager->get_logged_in() && ImGui::BeginMenu("Resources")) {
            if (ImGui::MenuItem("Overview")) {
                manager->open_or_close(make_unique<resource_overview_scene>());
            }

            if (ImGui::MenuItem("Battle Log")) {
                manager->open_or_close(make_unique<battle_log_scene>());
            }

            if (ImGui::MenuItem("Craft")) {
                //manager->open_or_close(make_unique<battle_log_scene>());
            }

            if (ImGui::BeginMenu("Select")) {
                if (ImGui::MenuItem("Combat")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::COMBAT));
                }

                if (ImGui::MenuItem("Gather Wood")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::WOOD_GATHERING));
                }

                if (ImGui::MenuItem("Gather Ore")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::ORE_GATHERING));
                }

                if (ImGui::MenuItem("Gather Water")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::WATER_GATHERING));
                }

                if (ImGui::MenuItem("Gather Plants")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::PLANTS_GATHERING));
                }

                if (ImGui::MenuItem("Dig Clay")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::CLAY_GATHERING));
                }

                if (ImGui::MenuItem("Craft Paper")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::PAPER_CRAFTING));
                }

                if (ImGui::MenuItem("Craft Ink")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::INK_CRAFTING));
                }

                if (ImGui::MenuItem("Forge Metal")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::METAL_FORGING));
                }

                if (ImGui::MenuItem("Fire Bricks")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::BRICK_FIRING));
                }

                if (ImGui::MenuItem("Craft Gems")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::GEM_CRAFTING));
                }

                if (ImGui::MenuItem("Work Wood")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::WOOD_WORKING));
                }

                if (ImGui::MenuItem("Craft Items")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::ITEM_CRAFTING));
                }

                if (ImGui::MenuItem("Go Work")) {
                    send_message<set_action_request>(manager, magic_enum::enum_integer(selectable_actions::WORKING));
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (manager->get_logged_in() && ImGui::BeginMenu("Companies")) {
            if (ImGui::MenuItem("Overview")) {
                manager->open_or_close(make_unique<companies_overview_scene>(manager));
            }

            if (!manager->get_character()->company.empty() && ImGui::MenuItem(fmt::format("Overview {}", manager->get_character()->company).c_str())) {
                manager->open_or_close(make_unique<manage_company_scene>());
            }

            if (manager->get_character()->company.empty() && ImGui::MenuItem("Create")) {
                manager->open_or_close(make_unique<create_company_scene>());
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Settings")) {
            manager->open_or_close(make_unique<settings_menu_scene>());
        }

        if (ImGui::MenuItem("About")) {
            manager->open_or_close(make_unique<about_scene>());
        }

        ImGui::EndMainMenuBar();
    }
}

void main_menu_scene::handle_message(iscene_manager *manager, uint64_t type, message const *msg) {

}
