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

#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>
#include <spdlog/spdlog.h>
#include <working_directory_manipulation.h>
#include <game_logic/censor_sensor.h>

#include "../src/config.h"
#include "../src/config_parsers.h"
#include "test_helpers/startup_helper.h"
#include "on_leaving_scope.h"
#include "macros.h"

using namespace std;
using namespace ibh;

int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::trace);

    set_cwd(get_selfpath());
    locale::global(locale("en_US.UTF-8"));

    sensor.add_dictionary("assets/profanity_locales/en.json");

    try {
        auto config_opt = parse_env_file();
        if(!config_opt) {
            return 1;
        }
        config = config_opt.value();
    } catch (const exception& e) {
        spdlog::error("[main] config.json file is malformed json.");
        return 1;
    }

#ifndef EXCLUDE_PSQL_TESTS
    db_pool = make_shared<database_pool>();
    db_pool->create_connections(config.connection_string, 2);
#endif

    MEASURE_TIME_OF_FUNCTION(trace);
    int result = Catch::Session().run( argc, argv );
    // global clean-up...

    return ( result < 0xff ? result : 0xff );
}
