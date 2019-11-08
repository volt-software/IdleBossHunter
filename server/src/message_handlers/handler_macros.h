/*
    Realm of Aesir
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

#include <messages/generic_error_response.h>
#include <on_leaving_scope.h>

#define SEND_ERROR(err, pretty_name, pretty_desc, clear_login)  generic_error_response resp{err, pretty_name, pretty_desc, clear_login}; \
                                                                s->send(user_data->ws, resp.serialize(), websocketpp::frame::opcode::value::TEXT);

#define DESERIALIZE_WITH_CHECK(type)    auto msg = type::deserialize(d); \
                                        if (!msg) { \
                                            spdlog::warn("[{}}] deserialize failed", __FUNCTION__); \
                                            SEND_ERROR("Unrecognized message", "", "", true); \
                                            return; \
                                        }

#define DESERIALIZE_WITH_LOGIN_CHECK(type)  if(user_data->username.empty()) { \
                                                return; \
                                            } \
                                            auto msg = type::deserialize(d); \
                                            if (!msg) { \
                                                spdlog::warn("[{}] deserialize failed", __FUNCTION__); \
                                                SEND_ERROR("Unrecognized message", "", "", true); \
                                                return; \
                                            }

#define DESERIALIZE_WITH_NOT_LOGIN_CHECK(type) if(!user_data->username.empty()) { \
                                                    return; \
                                                } \
                                                auto msg = type::deserialize(d); \
                                                if (!msg) { \
                                                    spdlog::warn("[{}] deserialize failed", __FUNCTION__); \
                                                    SEND_ERROR("Unrecognized message", "", "", true); \
                                                    return; \
                                                }

#define DESERIALIZE_WITH_PLAYING_CHECK(type)    if(user_data->playing_character_slot < 0) { \
                                                    SEND_ERROR("Not playing character", "", "", true); \
                                                    return; \
                                                } \
                                                auto msg = type::deserialize(d); \
                                                if (!msg) { \
                                                    spdlog::warn("[{}] deserialize failed", __FUNCTION__); \
                                                    SEND_ERROR("Unrecognized message", "", "", true); \
                                                    return; \
                                                }

#define DESERIALIZE_WITH_NOT_PLAYING_CHECK(type) if(user_data->username.empty()) { \
                                                    SEND_ERROR("Not logged in", "", "", true); \
                                                    return; \
                                                } \
                                                if(user_data->playing_character_slot >= 0) { \
                                                    SEND_ERROR("Already playing character", "", "", true); \
                                                    return; \
                                                } \
                                                auto msg = type::deserialize(d); \
                                                if (!msg) { \
                                                    spdlog::warn("[{}] deserialize failed", __FUNCTION__); \
                                                    SEND_ERROR("Unrecognized message", "", "", true); \
                                                    return; \
                                                }

#define MEASURE_TIME_OF_FUNCTION()  auto start = chrono::system_clock::now(); \
                                    auto func_name = __FUNCTION__; \
                                    auto time_scope_guard = on_leaving_scope([start, func_name] { \
                                        auto end = chrono::system_clock::now(); \
                                        spdlog::trace("[{}] finished in {} µs", func_name, chrono::duration_cast<chrono::microseconds>(end-start).count()); \
                                    });
