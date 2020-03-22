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

namespace ibh {

    template<bool AdminOnly>
    void send_message(clan_component const &clan, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue) {
        auto now = chrono::system_clock::now();
        auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        auto pc_view = es.view<pc_component>();
        for (auto const &member : clan.members) {
            for (auto clan_entity : pc_view) {
                auto &clan_pc = pc_view.get(clan_entity);

                if constexpr(AdminOnly) {
                    if (member.second == CLAN_MEMBER) {
                        continue;
                    }
                }

                if (member.first != clan_pc.id) {
                    continue;
                }

                auto new_applicant_msg = make_unique<message_response>(playername, message, source, timestamp);
                outward_queue.enqueue({clan_pc.connection_id, move(new_applicant_msg)});
            }
        }
    }

    template<bool AdminOnly>
    void send_message(vector<db_clan_member> const &data, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue) {
        auto now = chrono::system_clock::now();
        auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        auto pc_view = es.view<pc_component>();
        for (auto const &member : data) {
            for (auto clan_entity : pc_view) {
                auto &clan_pc = pc_view.get(clan_entity);

                if constexpr(AdminOnly) {
                    if (member.member_level == CLAN_MEMBER) {
                        continue;
                    }
                }

                if (member.character_id != clan_pc.id) {
                    continue;
                }

                auto new_applicant_msg = make_unique<message_response>(playername, message, source, timestamp);
                outward_queue.enqueue({clan_pc.connection_id, move(new_applicant_msg)});
            }
        }
    }

    void
    send_message_to_all_clan_members(uint64_t clan_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue,
                                     unique_ptr<database_transaction> const &transaction) {
        clan_members_repository<database_transaction> clan_members_repo{};
        auto clan_members = clan_members_repo.get_by_clan_id(clan_id, transaction);
        send_message<false>(clan_members, playername, message, source, es, outward_queue);
    }

    void send_message_to_all_clan_members(clan_component const &clan, string const &playername, string const &message, string const &source, entt::registry &es,
                                          outward_queues &outward_queue) {
        send_message<false>(clan, playername, message, source, es, outward_queue);
    }

    void send_message_to_all_clan_admins(uint64_t clan_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue,
                                         unique_ptr<database_transaction> const &transaction) {
        clan_members_repository<database_transaction> clan_members_repo{};
        auto clan_members = clan_members_repo.get_by_clan_id(clan_id, transaction);
        send_message<true>(clan_members, playername, message, source, es, outward_queue);
    }

    void send_message_to_all_clan_admins(clan_component const &clan, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue) {
        send_message<true>(clan, playername, message, source, es, outward_queue);
    }

    pc_component *get_player_entity_for_connection(uint64_t connection_id, entt::registry &es) {
        auto pc_view = es.view<pc_component>();
        for (auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if (pc.connection_id != connection_id) {
                return &pc;
            }
        }

        return nullptr;
    }

    pc_component* get_player_entity(uint64_t player_id, entt::registry &es) {
        auto pc_view = es.view<pc_component>();
        for (auto entity : pc_view) {
            auto &pc = pc_view.get(entity);

            if (pc.id != player_id) {
                return &pc;
            }
        }

        return nullptr;
    }
}