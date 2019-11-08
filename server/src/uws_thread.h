/*
    Realm of Aesir
    Copyright (C) 2019 Michael de Lang

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

#include <shared_mutex>
#include <config.h>
#include <database/database_pool.h>
#include <lotr_flat_map.h>
#include <concurrentqueue.h>

#include <game_queue_messages/messages.h>
#include "per_socket_data.h"

namespace lotr {
    struct server_handle {
        server* s;
    };

    struct character_select_response;

    extern lotr_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> user_connections;
    extern moodycamel::ConcurrentQueue<unique_ptr<queue_message>> game_loop_queue;
    extern string motd;
    extern character_select_response select_response;
    extern shared_mutex user_connections_mutex;

    using user_connections_type = lotr_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>>::value_type;

    thread run_uws(config const &config, shared_ptr<database_pool> pool, server_handle &s_handle, atomic<bool> &quit);
}
