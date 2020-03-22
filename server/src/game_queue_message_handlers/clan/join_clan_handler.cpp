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

#include "join_clan_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/clan/join_clan_response.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_members_repository.h>
#include <repositories/clan_member_applications_repository.h>
#include <game_queue_message_handlers/handler_helpers.h>

using namespace std;

namespace ibh {
    bool handle_join_clan(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *join_msg = dynamic_cast<join_clan_message*>(msg);

        if(join_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_view = es.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.connection_id != join_msg->connection_id) {
                continue;
            }

            clans_repository<database_subtransaction> clans_repo{};
            clan_members_repository<database_subtransaction> clan_members_repo{};
            clan_member_applications_repository<database_subtransaction> clan_member_applications_repo{};
            auto subtransaction = transaction->create_subtransaction();

            auto db_clan = clans_repo.get(join_msg->clan_name, subtransaction);
            if(!db_clan) {
                auto new_err_msg = make_unique<join_clan_response>("No clan by that name.");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto clan_member = clan_members_repo.get_by_character_id(pc.id, subtransaction);
            if(clan_member) {
                auto new_err_msg = make_unique<join_clan_response>("Already a member of a clan, leave that clan first.");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto clan_application = clan_member_applications_repo.get(db_clan->id, pc.id, subtransaction);
            if(clan_application) {
                auto new_err_msg = make_unique<join_clan_response>("Already applied to clan, please be patient.");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            db_clan_member new_member{db_clan->id, pc.id, CLAN_MEMBER};
            if(!clan_member_applications_repo.insert(new_member, subtransaction)) {
                auto new_err_msg = make_unique<join_clan_response>("Server error.");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            send_message_to_all_clan_admins(db_clan->id, pc.name, "has applied for the clan.", "system-clan", es, outward_queue, transaction);
            subtransaction->commit();

            auto new_err_msg = make_unique<join_clan_response>("");
            outward_queue.enqueue({pc.connection_id, move(new_err_msg)});

            spdlog::trace("[{}] left clan {} for pc {} for connection id {}", __FUNCTION__, clan_member->clan_id, pc.name, pc.connection_id);

            return true;
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, join_msg->connection_id);

        return false;
    }
}
