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

#include "player_enter_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <range/v3/algorithm/any_of.hpp>
#include <messages/generic_error_response.h>

using namespace std;

namespace ibh {
    void handle_player_enter_message(queue_message* msg, entt::registry& registry, outward_queues& outward_queue) {
        auto *enter_msg = dynamic_cast<player_enter_message*>(msg);

        if(enter_msg == nullptr) {
            spdlog::error("[{}] player_enter_message nullptr", __FUNCTION__);
            return;
        }

        auto pc_view = registry.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.id != enter_msg->character_id) {
                continue;
            }

            pc.connection_id = enter_msg->connection_id;
            break;
        }
    }
}
