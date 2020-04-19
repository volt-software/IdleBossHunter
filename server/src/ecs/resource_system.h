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
#include "components.h"
#include "game_queue_messages/messages.h"

namespace ibh {
    class resource_system {
    public:
        resource_system(uint32_t every_n_ticks, moodycamel::ConcurrentQueue<outward_message> *outward_queue) :
                _tick_count(0), _every_n_ticks(every_n_ticks), _outward_queue(outward_queue) {}
        void do_tick(entt::registry &es);

    private:
        uint32_t _tick_count;
        uint32_t _every_n_ticks;
        outward_queues _outward_queue;
    };
}