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
#include <messages/company/set_tax_response.h>
#include <repositories/companies_repository.h>
#include <repositories/company_stats_repository.h>
#include <game_queue_message_handlers/handler_helpers.h>

using namespace std;

namespace ibh {
    bool handle_set_tax(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *set_tax_msg = dynamic_cast<set_tax_message*>(msg);

        if(set_tax_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_view = es.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.connection_id != set_tax_msg->connection_id) {
                continue;
            }

            if(pc.company_id == 0) {
                auto new_err_msg = make_unique<set_tax_response>("Not a member of a company");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto company_view = es.view<company_component>();
            for(auto company_entity : company_view) {
                auto &company = company_view.get(company_entity);

                if (company.id != pc.company_id) {
                    continue;
                }

                auto member_it = company.members.find(pc.id);
                if(member_it == end(company.members)) {
                    auto new_err_msg = make_unique<set_tax_response>("Not a member of a company");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                if(member_it->second == COMPANY_MEMBER) {
                    auto new_err_msg = make_unique<set_tax_response>("Not an admin");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                auto current_stat = company.stats.find(company_stat_tax_id);
                if(current_stat == end(company.stats)) {
                    auto new_err_msg = make_unique<set_tax_response>("Couldn't find tax stat, please file a bug report");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                company_stats_repository<database_subtransaction> company_stats_repo{};
                auto subtransaction = transaction->create_subtransaction();
                current_stat->second = min(set_tax_msg->tax_percentage, 100u);
                db_company_stat db_tax_stat{0, company.id, company_stat_tax_id, current_stat->second};
                company_stats_repo.update_by_stat_id(db_tax_stat, subtransaction);
                subtransaction->commit();

                auto new_err_msg = make_unique<set_tax_response>("");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});

                send_message_to_all_company_members(company, pc.name, fmt::format("{} set tax to {}!", pc.name, current_stat->second), "system-company", es, outward_queue);

                return true;
            }

            spdlog::trace("[{}] could not find company id {} for player {}", __FUNCTION__, pc.company_id, pc.id);
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, set_tax_msg->connection_id);

        return false;
    }
}
