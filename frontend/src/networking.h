/*
    IdleBossHunter client
    Copyright (C) 2016  Michael de Lang

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

#include <config.h>
#include <ecs/ecs.h>

#ifndef __EMSCRIPTEN__
#include <thread>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#endif

namespace ibh {
    class scene_system;
    typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
#ifdef __EMSCRIPTEN__
    void init_net(config& config, entt::registry &es, scene_system &ss);
#else
    std::thread init_net(config const &config, entt::registry &es, scene_system &ss);
#endif
}