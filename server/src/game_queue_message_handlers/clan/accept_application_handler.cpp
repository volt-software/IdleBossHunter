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

#include "accept_application_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/clan/accept_application_response.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_members_repository.h>
#include <repositories/clan_member_applications_repository.h>

using namespace std;

namespace ibh {
    bool handle_accept_application(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *accept_msg = dynamic_cast<accept_application_message*>(msg);

        if(accept_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_view = es.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.connection_id != accept_msg->connection_id) {
                continue;
            }

            clan_members_repository<database_subtransaction> clan_members_repo{};
            clan_member_applications_repository<database_subtransaction> clan_member_applications_repo{};
            auto subtransaction = transaction->create_subtransaction();

            auto clan_member = clan_members_repo.get_by_character_id(pc.id, subtransaction);
            if(!clan_member) {
                auto new_err_msg = make_unique<accept_application_response>("Not a member of a clan");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            if(clan_member->member_level == CLAN_MEMBER) {
                auto new_err_msg = make_unique<accept_application_response>("Not an admin");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto clan_application = clan_member_applications_repo.get(clan_member->clan_id, accept_msg->applicant_id, subtransaction);
            if(!clan_application) {
                auto new_err_msg = make_unique<accept_application_response>("No applicant by that name.");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            clan_application->member_level = CLAN_MEMBER;
            if(!clan_members_repo.insert(*clan_application, subtransaction)) {
                auto new_err_msg = make_unique<accept_application_response>("Server error.");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }
            subtransaction->commit();

            auto new_err_msg = make_unique<accept_application_response>("");
            outward_queue.enqueue({pc.connection_id, move(new_err_msg)});

            spdlog::trace("[{}] accepted applicant {} clan {} by pc {} connection id {}", __FUNCTION__, accept_msg->applicant_id, pc.name, pc.connection_id);

            return true;
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, accept_msg->connection_id);

        return false;
    }
}
