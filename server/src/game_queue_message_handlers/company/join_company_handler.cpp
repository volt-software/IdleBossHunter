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

#include "join_company_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/company/join_company_response.h>
#include <repositories/companies_repository.h>
#include <repositories/company_members_repository.h>
#include <repositories/company_member_applications_repository.h>
#include <game_queue_message_handlers/handler_helpers.h>
#include <magic_enum.hpp>

using namespace std;

namespace ibh {
    bool handle_join_company(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *join_msg = dynamic_cast<join_company_message*>(msg);

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

            companies_repository<database_subtransaction> companies_repo{};
            company_members_repository<database_subtransaction> company_members_repo{};
            company_member_applications_repository<database_subtransaction> company_member_applications_repo{};
            auto subtransaction = transaction->create_subtransaction();

            auto db_company = companies_repo.get(join_msg->company_name, subtransaction);
            if(!db_company) {
                auto new_err_msg = make_unique<join_company_response>("No company by that name.");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto company_member = company_members_repo.get_by_character_id(pc.id, subtransaction);
            if(company_member) {
                auto new_err_msg = make_unique<join_company_response>("Already a member of a company, leave that company first.");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto company_application = company_member_applications_repo.get(db_company->id, pc.id, subtransaction);
            if(company_application) {
                auto new_err_msg = make_unique<join_company_response>("Already applied to company, please be patient.");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            db_company_member new_member{db_company->id, pc.id, magic_enum::enum_integer(company_member_level::COMPANY_MEMBER), 0};
            if(!company_member_applications_repo.insert(new_member, subtransaction)) {
                auto new_err_msg = make_unique<join_company_response>("Server error.");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            send_message_to_all_company_admins(db_company->id, pc.name, "has applied for the company.", "system-company", es, outward_queue, transaction);
            subtransaction->commit();

            auto new_err_msg = make_unique<join_company_response>("");
            outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});

            spdlog::trace("[{}] left company {} for pc {} for connection id {}", __FUNCTION__, company_member->company_id, pc.name, pc.connection_id);

            return true;
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, join_msg->connection_id);

        return false;
    }
}
