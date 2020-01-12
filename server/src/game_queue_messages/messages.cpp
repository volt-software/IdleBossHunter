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

#include "messages.h"
#include <ecs/components.h>

namespace ibh {
    uint32_t const player_enter_message::_type = 1;
    uint32_t const player_leave_message::_type = 2;
    uint32_t const player_move_message::_type  = 3;

    player_enter_message::player_enter_message(uint64_t character_id, string character_name, string race, string baseclass, vector <stat_component> player_stats, uint64_t connection_id, uint64_t level, uint64_t gold, uint64_t xp, uint64_t skill_points)
            : queue_message(_type, connection_id), character_id(character_id), character_name(move(character_name)), race(move(race)), baseclass(move(baseclass)),
            player_stats(move(player_stats)), level(level), gold(gold), xp(xp), skill_points(skill_points) {}

    player_leave_message::player_leave_message(uint64_t connection_id)
            : queue_message(_type, connection_id) {}

    player_move_message::player_move_message(uint64_t connection_id, uint32_t x, uint32_t y)
            : queue_message(_type, connection_id), x(x), y(y) {}
}
