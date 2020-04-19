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
#include <messages/chat/message_response.h>
#include <repositories/companies_repository.h>
#include <repositories/company_stats_repository.h>
#include <game_queue_message_handlers/handler_helpers.h>
#include <magic_enum.hpp>

using namespace std;

namespace ibh {
    bool handle_set_tax(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *set_tax_msg = dynamic_cast<set_tax_message*>(msg);

        if(set_tax_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_group = es.group<pc_component>(entt::get<company_component>);
        for(auto entity : pc_group) {
            auto [pc, cc] = pc_group.get<pc_component, company_component>(entity);

            if(pc.connection_id != set_tax_msg->connection_id) {
                continue;
            }

            if(cc.member_level == magic_enum::enum_integer(company_member_level::COMPANY_MEMBER)) {
                auto new_err_msg = make_unique<set_tax_response>("Not an admin");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto current_stat = cc.stats.find(company_stat_tax_id);
            if(current_stat == end(cc.stats)) {
                auto new_err_msg = make_unique<set_tax_response>("Couldn't find tax stat, please file a bug report");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            company_stats_repository<database_subtransaction> company_stats_repo{};
            auto subtransaction = transaction->create_subtransaction();
            current_stat->second = min(set_tax_msg->tax_percentage, 100u);
            db_company_stat db_tax_stat{0, cc.id, company_stat_tax_id, current_stat->second};
            company_stats_repo.update_by_stat_id(db_tax_stat, subtransaction);
            subtransaction->commit();

            auto new_err_msg = make_unique<set_tax_response>("");
            outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});


            auto now = chrono::system_clock::now();
            auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
            auto message = fmt::format("{} set tax to {}!", pc.name, current_stat->second);
            for(auto nested_entity : pc_group) {
                auto [pc2, cc2] = pc_group.get<pc_component, company_component>(nested_entity);

                if(cc2.id != cc.id) {
                    continue;
                }

                auto tax = cc.stats.find(company_stat_tax_id);
                if(tax == end(cc.stats)) {
                    spdlog::error("[{}] missing stat tax id for player {}", __FUNCTION__, pc.id);
                } else {
                    tax->second = current_stat->second;
                    auto update_msg = make_unique<message_response>(pc.name, message, "system-company", timestamp);
                    outward_queue.enqueue(outward_message{pc.connection_id, move(update_msg)});
                }
            }

            return true;
        }

        auto new_err_msg = make_unique<set_tax_response>("Not a member of a company");
        outward_queue.enqueue(outward_message{set_tax_msg->connection_id, move(new_err_msg)});
        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, set_tax_msg->connection_id);

        return false;
    }
}
