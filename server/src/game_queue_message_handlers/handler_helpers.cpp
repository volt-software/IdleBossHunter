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

#include "handler_helpers.h"
#include <repositories/clan_members_repository.h>
#include <ecs/components.h>
#include <messages/chat/message_response.h>

void ibh::send_message_to_all_clan_members(uint64_t clan_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
    clan_members_repository<database_transaction> clan_members_repo{};
    auto clan_members = clan_members_repo.get_by_clan_id(clan_id, transaction);
    auto now = chrono::system_clock::now();
    auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    auto pc_view = es.view<pc_component>();
    for (auto const &member : clan_members) {
        for(auto clan_entity : pc_view) {
            auto &clan_pc = pc_view.get(clan_entity);

            if(member.character_id != clan_pc.id) {
                continue;
            }

            auto new_applicant_msg = make_unique<message_response>(playername, message, source, timestamp);
            outward_queue.enqueue({clan_pc.connection_id, move(new_applicant_msg)});
        }
    }
}

void ibh::send_message_to_all_clan_admins(uint64_t clan_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction) {
    clan_members_repository<database_transaction> clan_members_repo{};
    auto clan_members = clan_members_repo.get_by_clan_id(clan_id, transaction);
    auto now = chrono::system_clock::now();
    auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    auto pc_view = es.view<pc_component>();
    for (auto const &member : clan_members) {

        if(member.member_level == CLAN_MEMBER) {
            continue;
        }

        for(auto clan_entity : pc_view) {
            auto &clan_pc = pc_view.get(clan_entity);

            if(member.character_id != clan_pc.id) {
                continue;
            }

            auto new_applicant_msg = make_unique<message_response>(playername, message, source, timestamp);
            outward_queue.enqueue({clan_pc.connection_id, move(new_applicant_msg)});
        }
    }
}
