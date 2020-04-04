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

#include "leave_company_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/company/leave_company_response.h>
#include <repositories/companies_repository.h>
#include <repositories/company_members_repository.h>
#include <game_queue_message_handlers/handler_helpers.h>

using namespace std;

namespace ibh {
    bool handle_leave_company(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *leave_msg = dynamic_cast<leave_company_message*>(msg);

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

            company_members_repository<database_subtransaction> company_members_repo{};
            auto subtransaction = transaction->create_subtransaction();
            auto company_member = company_members_repo.get_by_character_id(pc.id, subtransaction);
            if(!company_member) {
                auto new_err_msg = make_unique<leave_company_response>("Not a member of a company");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            company_members_repo.remove(*company_member, subtransaction);

            send_message_to_all_company_members(company_member->company_id, pc.name, "has left the company.", "system-company", es, outward_queue, transaction);

            auto new_err_msg = make_unique<leave_company_response>("");
            outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
            subtransaction->commit();

            auto company_view = es.view<company_component>();
            for(auto company_entity : company_view) {
                auto &company = company_view.get(company_entity);

                if (company.id != pc.company_id) {
                    continue;
                }

                company.members.erase(pc.id);
            }

            spdlog::trace("[{}] left company {} for pc {} for connection id {}", __FUNCTION__, company_member->company_id, pc.name, pc.connection_id);

            return true;
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, leave_msg->connection_id);

        return false;
    }
}
