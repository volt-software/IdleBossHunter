/*
    IdleBossHunter client
    Copyright (C) 2016  Michael de Lang

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
#include "map.h"

#include <fstream>
//#include <nlohmann/json.hpp>
#include <base64.h>
#include <zstd.h>
#include "spdlog/spdlog.h"
#include "tileset.h"

using namespace std;
//using json = nlohmann::json;

optional<fresh::map> fresh::map::load_from_file(const string &file) {
//    spdlog::debug(R"([map] Loading map "{}")", file);
//    string env_contents;
//    ifstream env(file);
//
//    if(!env) {
//        spdlog::error(R"([map] Couldn't find file "{}".)", file);
//        return {};
//    }
//
//    env.seekg(0, ios::end);
//    env_contents.resize(env.tellg());
//    env.seekg(0, ios::beg);
//    env.read(&env_contents[0], env_contents.size());
//    env.close();
//
//    spdlog::trace(R"([map] file contents: {})", env_contents);
//
//    auto env_json = json::parse(env_contents);
//
//    try {
//        if(env_json["orientation"] != "orthogonal") {
//            spdlog::error(R"([map] Only orthogonal orientation supported for file "{}")", file);
//            return {};
//        }
//
//        if(env_json["renderorder"] != "right-down") {
//            spdlog::error(R"([map] Only renderorder right-down supported for file "{}")", file);
//            return {};
//        }
//
//        if(env_json["tiledversion"] != "1.2.4") {
//            spdlog::error(R"([map] Only tiled version 1.2.4 supported for file "{}")", file);
//            return {};
//        }
//
//        if(env_json["tileheight"] != 32 || env_json["tilewidth"] != 32) {
//            spdlog::error(R"([map] Only tilesize of 32x32 supported for file "{}")", file);
//            return {};
//        }
//
//        uint32_t width = env_json["width"];
//        uint32_t height = env_json["height"];
//
//        vector<tileset> tilesets;
//        for(auto& tileset : env_json["tilesets"]) {
//            uint32_t firstgid = tileset["firstgid"];
//            string source = tileset["source"];
//            auto pos = source.find("..");
//
//            if(pos == string::npos) {
//                spdlog::error(R"([map] Incorrect tileset source for file "{}")", file);
//                return {};
//            }
//
//            source.replace(pos, 2, "./assets");
//
//            auto loaded_tileset = tileset::load_from_file(source, firstgid);
//
//            if(!loaded_tileset) {
//                spdlog::error(R"([map] Couldn't load tileset "{}" for file "{}")", source, file);
//                return {};
//            }
//
//            tilesets.push_back(*loaded_tileset);
//        }
//
//        for(auto& layer : env_json["layers"]) {
//            if(layer["compression"] != "zstd") {
//                spdlog::error(R"([map] Only compression zstd supported for file "{}")", file);
//                return {};
//            }
//
//            if(layer["encoding"] != "base64") {
//                spdlog::error(R"([map] Only encoding base64 supported for file "{}")", file);
//                return {};
//            }
//
//            if(layer["type"] != "tilelayer") {
//                spdlog::error(R"([map] Only type tilelayer supported for file "{}")", file);
//                return {};
//            }
//
//            string data = layer["data"];
//            string decoded_data = base64_decode(data);
//            int tiles_size = width * height * 4;
//            char *tiles = new char[tiles_size];
//
//            size_t const decompressed_size = ZSTD_decompress(tiles, tiles_size, decoded_data.c_str(), decoded_data.size());
//            if (ZSTD_isError(decompressed_size)) {
//                spdlog::error(R"([map] Only type tilelayer supported because {} for file "{}")", ZSTD_getErrorName(decompressed_size), file);
//                return {};
//            }
//
//            spdlog::trace(R"([map] decompressed {} bytes, expected {} bytes)", decompressed_size, tiles_size);
//
//        }
//
//        return make_optional<fresh::map>();
//    } catch (const exception& e) {
//        spdlog::error(R"([map] Couldn't read map because "{}" for file "{}")", e.what(), file);
//        return {};
//    }
    return {};
}

