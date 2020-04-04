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
#include <repositories/company_members_repository.h>
#include <ecs/components.h>
#include <messages/chat/message_response.h>

namespace ibh {

    template<bool AdminOnly>
    void send_message(company_component const &company, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue) {
        auto now = chrono::system_clock::now();
        auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        auto pc_view = es.view<pc_component>();
        for (auto const &member : company.members) {
            for (auto company_entity : pc_view) {
                auto &company_pc = pc_view.get(company_entity);

                if constexpr(AdminOnly) {
                    if (member.second == COMPANY_MEMBER) {
                        continue;
                    }
                }

                if (member.first != company_pc.id) {
                    continue;
                }

                auto new_applicant_msg = make_unique<message_response>(playername, message, source, timestamp);
                outward_queue.enqueue(outward_message{company_pc.connection_id, move(new_applicant_msg)});
            }
        }
    }

    template<bool AdminOnly>
    void send_message(vector<db_company_member> const &data, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue) {
        auto now = chrono::system_clock::now();
        auto timestamp = duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        auto pc_view = es.view<pc_component>();
        for (auto const &member : data) {
            for (auto company_entity : pc_view) {
                auto &company_pc = pc_view.get(company_entity);

                if constexpr(AdminOnly) {
                    if (member.member_level == COMPANY_MEMBER) {
                        continue;
                    }
                }

                if (member.character_id != company_pc.id) {
                    continue;
                }

                auto new_applicant_msg = make_unique<message_response>(playername, message, source, timestamp);
                outward_queue.enqueue(outward_message{company_pc.connection_id, move(new_applicant_msg)});
            }
        }
    }

    void
    send_message_to_all_company_members(uint64_t company_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue,
                                     unique_ptr<database_transaction> const &transaction) {
        company_members_repository<database_transaction> company_members_repo{};
        auto company_members = company_members_repo.get_by_company_id(company_id, transaction);
        send_message<false>(company_members, playername, message, source, es, outward_queue);
    }

    void send_message_to_all_company_members(company_component const &company, string const &playername, string const &message, string const &source, entt::registry &es,
                                          outward_queues &outward_queue) {
        send_message<false>(company, playername, message, source, es, outward_queue);
    }

    void send_message_to_all_company_admins(uint64_t company_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue,
                                         unique_ptr<database_transaction> const &transaction) {
        company_members_repository<database_transaction> company_members_repo{};
        auto company_members = company_members_repo.get_by_company_id(company_id, transaction);
        send_message<true>(company_members, playername, message, source, es, outward_queue);
    }

    void send_message_to_all_company_admins(company_component const &company, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues &outward_queue) {
        send_message<true>(company, playername, message, source, es, outward_queue);
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