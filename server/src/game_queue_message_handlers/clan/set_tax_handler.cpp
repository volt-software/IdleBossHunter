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

#include "set_tax_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/clan/set_tax_response.h>
#include <repositories/clans_repository.h>

using namespace std;

namespace ibh {
    bool handle_set_tax(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *create_msg = dynamic_cast<create_clan_message*>(msg);

        if(create_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_view = es.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.connection_id != create_msg->connection_id) {
                continue;
            }

            auto new_err_msg = make_unique<set_tax_response>("");
            outward_queue.enqueue({pc.connection_id, move(new_err_msg)});

            return true;
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, create_msg->connection_id);

        return false;
    }
}
