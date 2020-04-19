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
#include <magic_enum.hpp>
#include <messages/chat/message_response.h>


using namespace std;

namespace ibh {
    bool handle_increase_bonus(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *increase_bonus_msg = dynamic_cast<increase_bonus_message*>(msg);

        if(increase_bonus_msg == nullptr) {
            spdlog::error("[{}] nullptr", __FUNCTION__);
            return false;
        }

        auto pc_group = es.group<pc_component>(entt::get<company_component>);
        for(auto entity : pc_group) {
            auto [pc, cc] = pc_group.get<pc_component, company_component>(entity);

            if(pc.connection_id != increase_bonus_msg->connection_id) {
                continue;
            }

            if(cc.member_level == magic_enum::enum_integer(company_member_level::COMPANY_MEMBER)) {
                auto new_err_msg = make_unique<increase_bonus_response>("Not an admin");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            if(increase_bonus_msg->bonus_type == company_stat_gold_id) {
                auto new_err_msg = make_unique<increase_bonus_response>("Can't increase gold, silly");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto current_stat = cc.stats.find(increase_bonus_msg->bonus_type);
            if(current_stat == end(cc.stats)) {
                auto new_err_msg = make_unique<increase_bonus_response>("Couldn't find specified bonus type");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto current_gold_stat = cc.stats.find(company_stat_gold_id);
            if(current_gold_stat == end(cc.stats)) {
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
            db_company_stat db_current_stat{0, cc.id, increase_bonus_msg->bonus_type, current_stat->second};
            db_company_stat db_gold_stat{0, cc.id, company_stat_gold_id, current_gold_stat->second};
            company_stats_repo.update_by_stat_id(db_current_stat, subtransaction);
            company_stats_repo.update_by_stat_id(db_gold_stat, subtransaction);
            subtransaction->commit();

            auto new_err_msg = make_unique<increase_bonus_response>("");
            outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});

            auto now = chrono::system_clock::now();
            auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
            string message = fmt::format("{} set tax to {}!", pc.name, current_stat->second);

            auto current_stat_name_it = company_stat_id_to_name_mapper.find(increase_bonus_msg->bonus_type);
            if(current_stat_name_it == end(company_stat_id_to_name_mapper)) {
                message = fmt::format("{} increased a bonus!", pc.name, current_stat->second);
                spdlog::error("[{}] couldn't find stat name mapper for stat {}", __FUNCTION__, increase_bonus_msg->bonus_type);
            } else {
                message = fmt::format("{} increased the {} bonus!", pc.name, current_stat_name_it->second);
            }

            for(auto nested_entity : pc_group) {
                auto [pc2, cc2] = pc_group.get<pc_component, company_component>(nested_entity);

                if(cc2.id != cc.id) {
                    continue;
                }

                auto bonus = cc.stats.find(increase_bonus_msg->bonus_type);
                if(bonus == end(cc.stats)) {
                    spdlog::error("[{}] missing bonus id {} for player {}", __FUNCTION__, increase_bonus_msg->bonus_type, pc.id);
                } else {
                    bonus->second = current_stat->second;
                    auto update_msg = make_unique<message_response>(pc.name, message, "system-company", timestamp);
                    outward_queue.enqueue(outward_message{pc.connection_id, move(update_msg)});
                }
            }

            return true;
        }
        
        auto new_err_msg = make_unique<increase_bonus_response>("Not a member of a company");
        outward_queue.enqueue(outward_message{increase_bonus_msg->connection_id, move(new_err_msg)});
        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, increase_bonus_msg->connection_id);

        return false;
    }
}
