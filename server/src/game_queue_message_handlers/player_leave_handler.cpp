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

#include "player_leave_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>

using namespace std;

namespace ibh {
    void handle_player_leave_message(queue_message* msg, entt::registry& registry, outward_queues&) {
        auto *leave_message = dynamic_cast<player_leave_message*>(msg);

        if(leave_message == nullptr) {
            spdlog::error("[{}] player_leave_message nullptr", __FUNCTION__);
            return;
        }

        auto pc_view = registry.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.connection_id != leave_message->connection_id) {
                continue;
            }

            pc.connection_id = 0;
        }
    }
}
