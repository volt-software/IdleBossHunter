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

#define MEASURE_TIME_OF_FUNCTION()  auto start = chrono::system_clock::now(); \
                                    auto func_name = __FUNCTION__; \
                                    auto time_scope_guard = on_leaving_scope([start, func_name] { \
                                        auto end = chrono::system_clock::now(); \
                                        spdlog::info("[{}] finished in {:n} µs", func_name, chrono::duration_cast<chrono::microseconds>(end-start).count()); \
                                    }); \
                                    static_assert(true, "") // force usage of semicolon

