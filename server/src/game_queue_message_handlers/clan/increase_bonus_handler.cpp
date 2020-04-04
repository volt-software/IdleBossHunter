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

#include "increase_bonus_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/company/increase_bonus_response.h>
#include <repositories/companies_repository.h>
#include <repositories/company_stats_repository.h>
#include <game_queue_message_handlers/handler_helpers.h>

using namespace std;

namespace ibh {
    bool handle_increase_bonus(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *increase_bonus_msg = dynamic_cast<increase_bonus_message*>(msg);

        if(increase_bonus_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_view = es.view<pc_component>();
        for(auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if(pc.connection_id != increase_bonus_msg->connection_id) {
                continue;
            }

            if(pc.company_id == 0) {
                auto new_err_msg = make_unique<increase_bonus_response>("Not a member of a company");
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
                    auto new_err_msg = make_unique<increase_bonus_response>("Not a member of a company");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                if(member_it->second == COMPANY_MEMBER) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Not an admin");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                if(increase_bonus_msg->bonus_type == company_stat_gold_id) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Can't increase gold, silly");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                auto current_stat = company.stats.find(increase_bonus_msg->bonus_type);
                if(current_stat == end(company.stats)) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Couldn't find specified bonus type");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                auto current_gold_stat = company.stats.find(company_stat_gold_id);
                if(current_gold_stat == end(company.stats)) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Couldn't find company gold, please report this as a bug.");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                auto gold_requirement = pow(10l, current_stat->second + 2);
                if(current_gold_stat->second < gold_requirement) {
                    auto new_err_msg = make_unique<increase_bonus_response>(fmt::format("You need {} company gold to increase this stat.", gold_requirement));
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                current_stat->second++;
                current_gold_stat->second -= gold_requirement;


                company_stats_repository<database_subtransaction> company_stats_repo{};
                auto subtransaction = transaction->create_subtransaction();
                db_company_stat db_current_stat{0, company.id, increase_bonus_msg->bonus_type, current_stat->second};
                db_company_stat db_gold_stat{0, company.id, company_stat_gold_id, current_gold_stat->second};
                company_stats_repo.update_by_stat_id(db_current_stat, subtransaction);
                company_stats_repo.update_by_stat_id(db_gold_stat, subtransaction);
                subtransaction->commit();

                auto new_err_msg = make_unique<increase_bonus_response>("");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});

                auto current_stat_name_it = company_stat_id_to_name_mapper.find(increase_bonus_msg->bonus_type);
                if(current_stat_name_it == end(company_stat_id_to_name_mapper)) {
                    send_message_to_all_company_members(company, pc.name, fmt::format("{} increased a bonus!", pc.name), "system-company", es, outward_queue);
                    spdlog::error("[{}] couldn't find stat name mapper for stat {}", __FUNCTION__, increase_bonus_msg->bonus_type);
                } else {
                    send_message_to_all_company_members(company, pc.name, fmt::format("{} increased the {} bonus!", pc.name, current_stat_name_it->second), "system-company", es, outward_queue);
                }

                return true;
            }

            spdlog::trace("[{}] could not find company id {} for player {}", __FUNCTION__, pc.company_id, pc.id);
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, increase_bonus_msg->connection_id);

        return false;
    }
}
