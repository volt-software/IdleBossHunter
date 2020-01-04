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

#include <string>
#include <vector>
#include <ibh_containers.h>
#include <concurrentqueue.h>
#include <messages/message.h>

using namespace std;

namespace ibh {
    struct stat_component;
    using outward_queues = moodycamel::ConcurrentQueue<outward_message>;

    struct queue_message {
        uint32_t type;
        uint64_t connection_id;

        queue_message(uint32_t type, uint64_t connection_id) : type(type), connection_id(connection_id) {}
        virtual ~queue_message() {}
    };

    struct player_enter_message : queue_message {
        string character_name;
        string race;
        string baseclass;
        vector<stat_component> player_stats;
        uint64_t level;
        uint64_t gold;
        uint64_t xp;
        uint64_t skill_points;
        static uint32_t const _type;

        player_enter_message(string character_name, string race, string baseclass, vector<stat_component> player_stats, uint64_t connection_id, uint64_t level, uint64_t gold, uint64_t xp, uint64_t skill_points);
    };

    struct player_leave_message : queue_message {
        static uint32_t const _type;

        explicit player_leave_message(uint64_t connection_id);
    };

    struct player_move_message : queue_message {
        static uint32_t const _type;
        uint32_t x;
        uint32_t y;

        player_move_message(uint64_t connection_id, uint32_t x, uint32_t y);
    };
}
