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
        scene(uint64_t type) : _id(0), _type(type), _closed(false) {}
        virtual ~scene() = default;

        virtual void update(iscene_manager *manager, TimeDelta dt) = 0;
        virtual void handle_message(iscene_manager *manager, uint32_t type, message* msg) = 0;

        template <class TemplateClass, typename... Args>
        void send_message(iscene_manager *manager, Args&&... args)
        {
            TemplateClass req{std::forward<Args>(args)...};
#ifdef __EMSCRIPTEN__
            emscripten_websocket_send_utf8_text(manager->get_socket(), req.serialize().c_str());
#endif
        }

        unsigned int _id;
        uint64_t _type;
        bool _closed;
    };

    void enqueue_sdl_event(uint32_t type, uint32_t code, void *data1 = nullptr, void *data2 = nullptr);
}
