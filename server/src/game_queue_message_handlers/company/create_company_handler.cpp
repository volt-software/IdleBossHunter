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

#include "create_company_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/generic_error_response.h>
#include <messages/company/create_company_response.h>
#include <repositories/companies_repository.h>
#include <repositories/company_stats_repository.h>
#include <repositories/company_members_repository.h>
#include <magic_enum.hpp>

using namespace std;

namespace ibh {
    bool handle_create_company(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *create_msg = dynamic_cast<create_company_message*>(msg);

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

            auto gold_it = pc.stats.find(stat_gold_id);

            if(gold_it == end(pc.stats)) {
                spdlog::trace("[{}] pc {} not enough gold", __FUNCTION__, pc.id);
                auto new_err_msg = make_unique<generic_error_response>("unknown error", "", "", false);
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            if(gold_it->second < 10'000) {
                auto new_err_msg = make_unique<create_company_response>("Not enough gold, need 10,000 to create company.");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            companies_repository<database_subtransaction> company_repo{};
            company_stats_repository<database_subtransaction> company_stats_repo{};
            company_members_repository<database_subtransaction> company_members_repo{};
            auto subtransaction = transaction->create_subtransaction();

            db_company new_company{0, create_msg->company_name, 0, create_msg->company_type};
            if(!company_repo.insert(new_company, subtransaction)) {
                auto new_err_msg = make_unique<create_company_response>("Company name already exists");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            ibh_flat_map<uint32_t, int64_t> company_stats;
            for(auto &stat_id : stat_name_ids) {
                db_company_stat stat{0, new_company.id, stat_id, stat_id == stat_xp_id || stat_id == stat_gold_id ? 5 : 0};
                company_stats.emplace(stat_id, stat.value);
                company_stats_repo.insert(stat, subtransaction);
            }

            db_company_member company_admin{new_company.id, pc.id, magic_enum::enum_integer(company_member_level::COMPANY_ADMIN), 0};
            company_members_repo.insert(company_admin, subtransaction);
            subtransaction->commit();

            es.emplace<company_component>(entity, new_company.id, magic_enum::enum_integer(company_member_level::COMPANY_ADMIN), create_msg->company_name, company_stats);

            gold_it->second -= 10'000;
            auto new_err_msg = make_unique<create_company_response>("");
            outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});

            spdlog::trace("[{}] created company {} for pc {} for connection id {}", __FUNCTION__, create_msg->company_name, pc.name, pc.connection_id);

            return true;
        }

        auto new_err_msg = make_unique<create_company_response>("unknown error");
        outward_queue.enqueue(outward_message{create_msg->connection_id, move(new_err_msg)});
        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, create_msg->connection_id);

        return false;
    }
}
