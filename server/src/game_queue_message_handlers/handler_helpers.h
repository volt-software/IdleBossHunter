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

#pragma once

#include <entt/entity/registry.hpp>
#include <database/database_transaction.h>
#include <game_queue_messages/messages.h>

using namespace std;

namespace ibh {
    void send_message_to_all_clan_members(uint64_t clan_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction);
    void send_message_to_all_clan_admins(uint64_t clan_id, string const &playername, string const &message, string const &source, entt::registry &es, outward_queues& outward_queue, unique_ptr<database_transaction> const &transaction);
}