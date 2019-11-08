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

#pragma GCC diagnostic ignored "-Wduplicated-branches"
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio.hpp>
#pragma GCC diagnostic pop

namespace lotr {
    using server = websocketpp::server<websocketpp::config::asio_tls>;

    template <class WebSocket>
    struct per_socket_data {
        uint64_t connection_id;
        uint64_t user_id;
        uint32_t subscription_tier;
        bool is_tester;
        bool is_game_master;
        int32_t playing_character_slot;
        string username;
        WebSocket ws;

        per_socket_data() : connection_id(0), user_id(0), subscription_tier(0), is_tester(), is_game_master(), playing_character_slot(), username(), ws() {}
    };
}
