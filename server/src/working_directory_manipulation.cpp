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

#include "working_directory_manipulation.h"

#include <fstream>
#include <filesystem>
#include <climits>
#include <unistd.h>
#include <spdlog/spdlog.h>


string lotr::get_selfpath() {
    char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
        buff[len] = '\0';
        auto ret = string(buff);
        return ret.substr(0, ret.find_last_of('/'));
    }
    throw runtime_error("Couldn't get self path");
}

void lotr::set_cwd(string const &path) {
    spdlog::info("cwd changing to {}", path);
    filesystem::current_path(path);
}

optional<string> lotr::read_whole_file(string const &path) {
    string contents;
    ifstream file(path);

    if(!file) {
        spdlog::error("[{}] no \"{}\" file found.", __FUNCTION__, path);
        return {};
    }

    file.seekg(0, ios::end);
    contents.resize(file.tellg());
    file.seekg(0, ios::beg);
    file.read(&contents[0], contents.size());
    file.close();

    return contents;
}
