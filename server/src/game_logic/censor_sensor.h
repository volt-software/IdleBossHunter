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

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <lotr_flat_map.h>

using namespace std;

namespace lotr {
    enum class profanity_type : uint32_t {
        SLURS,
        COMMON_PROFANITY,
        SEXUAL_TERMS,
        POSSIBLY_OFFENSIVE,
        USER_ADDED
    };

    class censor_sensor {
    public:
        explicit censor_sensor(string const &profanity_dictionary_path);
        bool is_profane(string phrase);
        bool is_profane_ish(string phrase);
        string clean_profanity(string phrase);
        string clean_profanity_ish(string phrase);
        void enable_tier(uint32_t tier);
        void disable_tier(uint32_t tier);

    private:
        lotr_flat_map<string, int> _word_tiers;
        unordered_set<int> _enabled_tiers;
    };

    extern censor_sensor sensor;
}
