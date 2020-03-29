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

#include "leave_clan_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/clan/leave_clan_response.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_members_repository.h>
#include <game_queue_message_handlers/handler_helpers.h>

using namespace std;

namespace ibh {
    bool handle_leave_clan(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *leave_msg = dynamic_cast<leave_clan_message*>(msg);

        if(leave_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_view = es.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.connection_id != leave_msg->connection_id) {
                continue;
            }

            clan_members_repository<database_subtransaction> clan_members_repo{};
            auto subtransaction = transaction->create_subtransaction();
            auto clan_member = clan_members_repo.get_by_character_id(pc.id, subtransaction);
            if(!clan_member) {
                auto new_err_msg = make_unique<leave_clan_response>("Not a member of a clan");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            clan_members_repo.remove(*clan_member, subtransaction);

            send_message_to_all_clan_members(clan_member->clan_id, pc.name, "has left the clan.", "system-clan", es, outward_queue, transaction);

            auto new_err_msg = make_unique<leave_clan_response>("");
            outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
            subtransaction->commit();

            auto clan_view = es.view<clan_component>();
            for(auto clan_entity : clan_view) {
                auto &clan = clan_view.get(clan_entity);

                if (clan.id != pc.clan_id) {
                    continue;
                }

                clan.members.erase(pc.id);
            }

            spdlog::trace("[{}] left clan {} for pc {} for connection id {}", __FUNCTION__, clan_member->clan_id, pc.name, pc.connection_id);

            return true;
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, leave_msg->connection_id);

        return false;
    }
}
