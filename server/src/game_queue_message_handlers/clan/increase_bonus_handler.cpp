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
#include <messages/clan/increase_bonus_response.h>
#include <repositories/clans_repository.h>
#include <repositories/clan_stats_repository.h>
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

            if(pc.clan_id == 0) {
                auto new_err_msg = make_unique<increase_bonus_response>("Not a member of a clan");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                return false;
            }

            auto clan_view = es.view<clan_component>();
            for(auto clan_entity : clan_view) {
                auto &clan = clan_view.get(clan_entity);

                if (clan.id != pc.clan_id) {
                    continue;
                }

                auto member_it = clan.members.find(pc.id);
                if(member_it == end(clan.members)) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Not a member of a clan");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                if(member_it->second == CLAN_MEMBER) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Not an admin");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                if(increase_bonus_msg->bonus_type == clan_stat_gold_id) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Can't increase gold, silly");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                auto current_stat = clan.stats.find(increase_bonus_msg->bonus_type);
                if(current_stat == end(clan.stats)) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Couldn't find specified bonus type");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                auto current_gold_stat = clan.stats.find(clan_stat_gold_id);
                if(current_gold_stat == end(clan.stats)) {
                    auto new_err_msg = make_unique<increase_bonus_response>("Couldn't find clan gold, please report this as a bug.");
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                auto gold_requirement = pow(10l, current_stat->second + 2);
                if(current_gold_stat->second < gold_requirement) {
                    auto new_err_msg = make_unique<increase_bonus_response>(fmt::format("You need {} clan gold to increase this stat.", gold_requirement));
                    outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});
                    return false;
                }

                current_stat->second++;
                current_gold_stat->second -= gold_requirement;


                clan_stats_repository<database_subtransaction> clan_stats_repo{};
                auto subtransaction = transaction->create_subtransaction();
                db_clan_stat db_current_stat{0, clan.id, increase_bonus_msg->bonus_type, current_stat->second};
                db_clan_stat db_gold_stat{0, clan.id, clan_stat_gold_id, current_gold_stat->second};
                clan_stats_repo.update_by_stat_id(db_current_stat, subtransaction);
                clan_stats_repo.update_by_stat_id(db_gold_stat, subtransaction);
                subtransaction->commit();

                auto new_err_msg = make_unique<increase_bonus_response>("");
                outward_queue.enqueue(outward_message{pc.connection_id, move(new_err_msg)});

                auto current_stat_name_it = clan_stat_id_to_name_mapper.find(increase_bonus_msg->bonus_type);
                if(current_stat_name_it == end(clan_stat_id_to_name_mapper)) {
                    send_message_to_all_clan_members(clan, pc.name, fmt::format("{} increased a bonus!", pc.name), "system-clan", es, outward_queue);
                    spdlog::error("[{}] couldn't find stat name mapper for stat {}", __FUNCTION__, increase_bonus_msg->bonus_type);
                } else {
                    send_message_to_all_clan_members(clan, pc.name, fmt::format("{} increased the {} bonus!", pc.name, current_stat_name_it->second), "system-clan", es, outward_queue);
                }

                return true;
            }

            spdlog::trace("[{}] could not find clan id {} for player {}", __FUNCTION__, pc.clan_id, pc.id);
        }

        spdlog::trace("[{}] could not find conn id {}", __FUNCTION__, increase_bonus_msg->connection_id);

        return false;
    }
}
