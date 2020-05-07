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

#ifndef __EMSCRIPTEN__
#include <networking.h>
#endif

namespace ibh {
    class texture_atlas;
    class sprite;

    struct ai_component {
        ai_component(uint64_t user_id) : user_id(user_id) {}

        uint64_t user_id;
    };

    struct character_component {
        character_component(uint64_t map_id) noexcept : map_id(map_id) {}

        uint64_t map_id;
    };

    struct effect_component {
        effect_component(uint64_t user_id) : user_id(user_id) {}

        uint64_t user_id;
    };

    struct layer_component {
        layer_component(std::vector<uint64_t> tiles, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
                : tiles(std::move(tiles)), x(x), y(y), width(width), height(height) {}

        std::vector<uint64_t> tiles;
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };

    struct tileset_component {
        tileset_component(uint32_t first_gid, std::string image_path, uint32_t tile_width, uint32_t tile_height,
                          uint32_t width, uint32_t height) : first_gid(first_gid), image_path(std::move(image_path)), tile_width(tile_width),
                                                             tile_height(tile_height), width(width), height(height) {}

        uint32_t first_gid;
        std::string image_path;
        uint32_t tile_width;
        uint32_t tile_height;
        uint32_t width;
        uint32_t height;
    };

    struct map_component {
        map_component(uint64_t id, uint32_t tile_height, uint32_t tile_width,
                      uint32_t width, uint32_t height, uint32_t no_layers, uint32_t first_tile_id, uint32_t max_tile_id)
                : id(id), tile_height(tile_height), tile_width(tile_width),
                  width(width), height(height), no_layers(no_layers), first_tile_id(first_tile_id), max_tile_id(max_tile_id)
        {}

        uint64_t id;
        uint32_t tile_height;
        uint32_t tile_width;
        uint32_t width;
        uint32_t height;
        uint32_t no_layers;
        uint32_t first_tile_id;
        uint32_t max_tile_id;
        std::vector<tileset_component> tilesets;
        std::vector<layer_component> layers;
        std::vector<uint64_t> npcs;
        std::vector<uint64_t> pcs;
        std::vector<uint64_t> items;
    };


    struct player_component {
        player_component(uint64_t user_id) : user_id(user_id) {}

        uint64_t user_id;
    };

    struct socket_component {

#ifdef __EMSCRIPTEN__
        explicit socket_component(int socket) : socket(socket) {}
        int socket;
#else
        explicit socket_component(bool running, websocketpp::connection_hdl hdl, client *socket) : running(running), hdl(move(hdl)), socket(socket) {}
        socket_component(socket_component&& other) noexcept : running(other.running.load()), hdl(move(other.hdl)), socket(other.socket) {}
        socket_component& operator=(socket_component&& other) noexcept {
            running = other.running.load();
            hdl = move(other.hdl);
            socket = other.socket;
            return *this;
        }
        std::atomic<bool> running;
        websocketpp::connection_hdl hdl;
        client *socket;
#endif
    };

    struct sprite_component {
        explicit sprite_component(sprite *_sprite) : sprite_(_sprite) {}
        sprite *sprite_;
    };

    struct stat {
        stat(std::string name, uint64_t static_value, uint32_t dice, uint32_t die_face, bool is_growth)
                : name(move(name)), static_value(static_value), dice(dice), die_face(die_face), is_growth(is_growth) {}

        std::string name;
        uint64_t static_value;
        uint32_t dice;
        uint32_t die_face;
        bool is_growth;
    };

    struct tile_component {
        tile_component(uint64_t map_id, uint16_t tile_id) : map_id(map_id), tile_id(tile_id) {}

        uint64_t map_id;
        uint16_t tile_id;
    };
}
