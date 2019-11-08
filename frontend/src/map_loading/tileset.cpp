/*
    freshprinceofbijlmer
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

#include "tileset.h"

#include <fstream>
//#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

using namespace std;
//using json = nlohmann::json;
using namespace fresh;

std::optional<tileset> tileset::load_from_file(const std::string &file, uint32_t firstgid) {
//    spdlog::debug(R"([tileset] Loading tileset "{}" firstgid {})", file, firstgid);
//    string env_contents;
//    ifstream env(file);
//
//    if(!env) {
//        spdlog::error("[tileset] Couldn't find file \"{}\".", file);
//        return {};
//    }
//
//    env.seekg(0, ios::end);
//    env_contents.resize(env.tellg());
//    env.seekg(0, ios::beg);
//    env.read(&env_contents[0], env_contents.size());
//    env.close();
//
//    spdlog::trace(R"([tileset] file size {} contents: {})", env_contents.size(), env_contents);
//
//    auto env_json = json::parse(env_contents);
//
//    try {
//        if(env_json["type"] != "tileset") {
//            spdlog::error("[tileset] Only type tileset supported");
//            return {};
//        }
//
//        if(env_json["tiledversion"] != "1.2.4") {
//            spdlog::error("[tileset] Only tiled version 1.2.4 supported");
//            return {};
//        }
//
//        if(env_json["tileheight"] != 32 || env_json["tilewidth"] != 32) {
//            spdlog::error("[tileset] Only tilesize of 32x32 supported");
//            return {};
//        }
//
//        string filename = env_json["image"];
//
//        if(filename.empty()) {
//            spdlog::error("[tileset] empty filename");
//            return {};
//        }
//
//        return make_optional<tileset>(filename, firstgid);
//    } catch (const exception& e) {
//        spdlog::error("[tileset] Couldn't read tileset \"{}\"", e.what());
//        return {};
//    }

    return {};
}
