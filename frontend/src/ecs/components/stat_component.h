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

namespace ibh {
    struct stat {
        stat(std::string name, uint64_t static_value, uint32_t dice, uint32_t die_face, bool is_growth)
                : name(move(name)), static_value(static_value), dice(dice), die_face(die_face), is_growth(is_growth) {}

        std::string name;
        uint64_t static_value;
        uint32_t dice;
        uint32_t die_face;
        bool is_growth;
    };

    struct stat_component {
        stat_component() : stats() {}
        explicit stat_component(std::vector<stat> stats) : stats(move(stats)) {}

        std::vector<stat> stats;
    };
}
