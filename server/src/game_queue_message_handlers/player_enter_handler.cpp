/*
    Realm of Aesir
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

#include "player_enter_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <range/v3/algorithm/any_of.hpp>
#include <messages/generic_error_response.h>

using namespace std;

namespace lotr {
    void handle_player_enter_message(queue_message* msg, entt::registry& registry, outward_queues& outward_queue) {
        auto *enter_msg = dynamic_cast<player_enter_message*>(msg);

        if(enter_msg == nullptr) {
            spdlog::error("[{}] player_enter_message nullptr", __FUNCTION__);
            return;
        }

        // TODO
        /*auto map_view = registry.view<map_component>();

        for(auto m_entity : map_view) {
            map_component const &m = map_view.get(m_entity);

            if(ranges::any_of(m.players, [&](pc_component const &pc){ return pc.name == enter_msg->character_name; })) {
                spdlog::warn("[{}] character already in game {} {}", __FUNCTION__, enter_msg->character_name, enter_msg->connection_id);
                outward_queue.enqueue(outward_message{enter_msg->connection_id, make_unique<generic_error_response>("already playing that character", "already playing that character", "already playing that character", true)});
                return;
            }
        }

        for(auto m_entity : map_view) {
            map_component &m = map_view.get(m_entity);

            if(m.name != enter_msg->map_name) {
                continue;
            }

            if(enter_msg->x >= m.width || enter_msg->y >= m.height) {
                spdlog::error("[{}] wrong coordinates {} {} {} {}", __FUNCTION__, enter_msg->map_name, enter_msg->x, enter_msg->y, enter_msg->connection_id);
                outward_queue.enqueue(outward_message{enter_msg->connection_id, make_unique<generic_error_response>("Wrong coordinates", "Wrong coordinates", "Wrong coordinates", true)});
                return;
            }

            pc_component pc{};
            pc.name = enter_msg->character_name;
            pc.level = enter_msg->level;
            pc.gold = enter_msg->gold;
            pc.loc = make_tuple(enter_msg->x, enter_msg->y);
            pc.connection_id = enter_msg->connection_id;
            pc.gender = enter_msg->gender;
            pc.race = enter_msg->race;
            pc.character_class = enter_msg->baseclass;

            for(auto &stat : enter_msg->player_stats) {
                pc.stats[stat.name] = stat.value;
            }

            m.players.emplace_back(pc);

            spdlog::info("[{}] character {} entered game {}", __FUNCTION__, pc.name, enter_msg->connection_id);
            break;
        }*/
    }
}
