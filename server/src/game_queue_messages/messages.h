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
        uint64_t type;
        uint64_t connection_id;

        queue_message(uint64_t type, uint64_t connection_id) noexcept : type(type), connection_id(connection_id) {}
        virtual ~queue_message() = default;
    };

    // clan

    struct create_clan_message : queue_message {
        string clan_name;
        inline static constexpr uint64_t _type = generate_type<create_clan_message>();

        create_clan_message(uint64_t connection_id, string clan_name) noexcept;
    };

    struct increase_bonus_message : queue_message {
        uint32_t bonus_type;
        inline static constexpr uint64_t _type = generate_type<increase_bonus_message>();

        increase_bonus_message(uint64_t connection_id, uint32_t bonus_type) noexcept;
    };

    struct join_clan_message : queue_message {
        string clan_name;
        inline static constexpr uint64_t _type = generate_type<join_clan_message>();

        join_clan_message(uint64_t connection_id, string clan_name) noexcept;
    };

    struct set_tax_message : queue_message {
        uint32_t tax_percentage;
        inline static constexpr uint64_t _type = generate_type<set_tax_message>();

        set_tax_message(uint64_t connection_id, uint32_t tax_percentage) noexcept;
    };

    // uac

    struct player_enter_message : queue_message {
        uint64_t character_id;
        string character_name;
        string race;
        string baseclass;
        vector<stat_component> player_stats;
        uint64_t level;
        uint64_t gold;
        uint64_t xp;
        uint64_t skill_points;
        inline static constexpr uint64_t _type = generate_type<player_enter_message>();

        player_enter_message(uint64_t character_id, string character_name, string race, string baseclass, vector<stat_component> player_stats, uint64_t connection_id, uint64_t level, uint64_t gold, uint64_t xp, uint64_t skill_points) noexcept;
    };

    struct player_leave_message : queue_message {
        inline static constexpr uint64_t _type = generate_type<player_leave_message>();

        explicit player_leave_message(uint64_t connection_id) noexcept;
    };

    struct player_move_message : queue_message {
        inline static constexpr uint64_t _type = generate_type<player_move_message>();
        uint32_t x;
        uint32_t y;

        player_move_message(uint64_t connection_id, uint32_t x, uint32_t y) noexcept;
    };
}
