/*
    IdleBossHunter
    Copyright (C) 2019  Michael de Lang

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

#include "../ecs/ecs.h"
#include "scene_manager.h"
#include <messages/message.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/websocket.h>
#endif

namespace ibh {
    class scene {
    public:
        scene() : _id(0), _closed(false), _waiting_for_reply(false) {}
        virtual ~scene() = default;

        virtual void update(iscene_manager *manager, entt::registry &es, TimeDelta dt) = 0;
        virtual void handle_message(uint32_t type, message* msg) = 0;

        template <class TemplateClass, typename... Args>
        void send_message(entt::registry &es, Args&&... args)
        {
            auto view = es.view<socket_component>();
            for (auto entity : view) {

                TemplateClass req{std::forward<Args>(args)...};

                socket_component &socket = view.get<socket_component>(entity);
#ifdef __EMSCRIPTEN__
                emscripten_websocket_send_utf8_text(socket.socket, req.serialize().c_str());
#endif
            }
        }

        unsigned int _id;
        bool _closed;
        bool _waiting_for_reply;
    };

    void send_event(uint32_t type, uint32_t code, void *data1 = nullptr, void *data2 = nullptr);
}
