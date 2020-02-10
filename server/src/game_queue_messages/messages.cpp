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
    player_enter_message::player_enter_message(uint64_t character_id, string character_name, string race, string baseclass, vector <stat_component> player_stats, uint64_t connection_id, uint64_t level, uint64_t gold, uint64_t xp, uint64_t skill_points) noexcept
            : queue_message(_type, connection_id), character_id(character_id), character_name(move(character_name)), race(move(race)), baseclass(move(baseclass)),
            player_stats(move(player_stats)), level(level), gold(gold), xp(xp), skill_points(skill_points) {}

    player_leave_message::player_leave_message(uint64_t connection_id) noexcept
            : queue_message(_type, connection_id) {}

    player_move_message::player_move_message(uint64_t connection_id, uint32_t x, uint32_t y) noexcept
            : queue_message(_type, connection_id), x(x), y(y) {}

    create_clan_message::create_clan_message(uint64_t connection_id, string clan_name) noexcept
    : queue_message(_type, connection_id), clan_name(move(clan_name)) {}

    increase_bonus_message::increase_bonus_message(uint64_t connection_id, uint32_t bonus_type) noexcept
            : queue_message(_type, connection_id), bonus_type(bonus_type) {}

    join_clan_message::join_clan_message(uint64_t connection_id, string clan_name) noexcept
            : queue_message(_type, connection_id), clan_name(move(clan_name)) {}

    set_tax_message::set_tax_message(uint64_t connection_id, uint32_t tax_percentage) noexcept
            : queue_message(_type, connection_id), tax_percentage(tax_percentage) {}
}
