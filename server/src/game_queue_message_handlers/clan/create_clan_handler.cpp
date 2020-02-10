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

#include "create_clan_handler.h"

#include <spdlog/spdlog.h>
#include <ecs/components.h>
#include <messages/generic_error_response.h>
#include <messages/clan/create_clan_response.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_stats_repository.h>
#include <repositories/clan_members_repository.h>

using namespace std;

namespace ibh {
    bool handle_create_clan(queue_message* msg, entt::registry& es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
        auto *create_msg = dynamic_cast<create_clan_message*>(msg);

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
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            if(gold_it->second < 10'000) {
                auto new_err_msg = make_unique<create_clan_response>("Not enough gold, need 10,000 to create clan.");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            clans_repository<database_subtransaction> clan_repo{};
            clan_stats_repository<database_subtransaction> clan_stats_repo{};
            clan_members_repository<database_subtransaction> clan_members_repo{};
            auto subtransaction = transaction->create_subtransaction();

            db_clan new_clan{0, create_msg->clan_name, {}, {}};
            if(!clan_repo.insert(new_clan, subtransaction)) {
                auto new_err_msg = make_unique<create_clan_response>("Clan name already exists");
                outward_queue.enqueue({pc.connection_id, move(new_err_msg)});
                return false;
            }

            vector<stat_id_component> clan_stats;
            for(auto &name : stat_names) {
                auto mapper_it = stat_name_to_id_mapper.find(name);

                if(mapper_it == end(stat_name_to_id_mapper)) {
                    spdlog::error("[{}] couldn't map {}", __FUNCTION__, name);
                    continue;
                }

                db_clan_stat stat{0, new_clan.id, name, mapper_it->second == stat_xp_id || mapper_it->second == stat_gold_id ? 5 : 0};
                clan_stats.emplace_back(stat_id_component{mapper_it->second, stat.value});
                clan_stats_repo.insert(stat, subtransaction);
            }

            db_clan_member clan_admin{new_clan.id, pc.id, CLAN_ADMIN};
            clan_members_repo.insert(clan_admin, subtransaction);
            subtransaction->commit();

            auto clan_entt = es.create();
            es.assign<clan_component>(clan_entt, new_clan.id, create_msg->clan_name, vector<clan_member_component>{{pc.id, CLAN_ADMIN}}, clan_stats);
            pc.clan_id = new_clan.id;

            gold_it->second -= 10'000;
            auto new_err_msg = make_unique<create_clan_response>("");
            outward_queue.enqueue({pc.connection_id, move(new_err_msg)});

            spdlog::trace("[{}] created clan {} for pc {} for connection id {}", __FUNCTION__, create_msg->clan_name, pc.name, pc.connection_id);

            return true;
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, create_msg->connection_id);

        return false;
    }
}
