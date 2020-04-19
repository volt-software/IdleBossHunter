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

#include "components.h"

using namespace std;
using namespace ibh;

namespace ibh {
    auto get_stat(decltype(pc_component::stats) &stats, decltype(pc_component::stats)::key_type stat_id) -> decltype(pc_component::stats)::mapped_type& {
        auto stat = stats.find(stat_id);
        if(stat == end(stats)) {
            spdlog::error("[{}] missing {}", __FUNCTION__, stat_id);
            throw std::runtime_error("missing stat");
        }

        return stat->second;
    }

    auto get_stat_or_initialize_default(decltype(pc_component::stats) &stats, decltype(pc_component::stats)::key_type stat_id, decltype(pc_component::stats)::mapped_type default_val) -> decltype(pc_component::stats)::mapped_type& {
        auto stat = stats.find(stat_id);
        if(stat == end(stats)) {
            stats.emplace(stat_id, default_val);
            stat = stats.find(stat_id);
        }

        return stat->second;
    }
}