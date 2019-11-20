/*
    IdleBossHunter
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

#include <optional>
#include <string>

namespace ibh {
    class tileset {
    public:
        tileset(std::string filename, uint32_t firstgid) : filename(move(filename)), firstgid(firstgid) {}

        static std::optional<tileset> load_from_file(const std::string &file, uint32_t firstgid);

        std::string filename;
        uint32_t firstgid;
    };
}
