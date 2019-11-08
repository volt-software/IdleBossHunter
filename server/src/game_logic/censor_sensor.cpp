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

#include <spdlog/spdlog.h>
#include <rapidjson/document.h>

#include "censor_sensor.h"
#include "working_directory_manipulation.h"
#include <utf.h>

using namespace lotr;

censor_sensor lotr::sensor("assets/profanity_locales/en.json");

censor_sensor::censor_sensor(string const &profanity_dictionary_path) : _word_tiers() {
    auto dict_contents = read_whole_file(profanity_dictionary_path);
    if(!dict_contents) {
        throw runtime_error("no dict_contents");
    }

    rapidjson::Document d;
    d.Parse(dict_contents->c_str(), dict_contents->size());

    if (d.HasParseError() || !d.IsObject()) {
        spdlog::warn("[{}] deserialize failed: {}", __FUNCTION__, d.GetParseError());
        throw runtime_error("deserialize failed");
    }

    for (auto iter = d.MemberBegin(); iter != d.MemberEnd(); ++iter){
        string word = iter->name.GetString();
        string_tolower(word);

        int tier = iter->value.GetInt();
        _word_tiers[word] = tier;
        _enabled_tiers.insert(tier);
    }
}

bool censor_sensor::is_profane(string phrase) {
    string_tolower(phrase);

    auto phrase_view = string_view(phrase);
    string::size_type last = 0;
    string::size_type next = 0;
    bool contains_profanity = false;
    while ((next = phrase.find(' ', last)) != std::string::npos && !contains_profanity) {
        auto word = phrase_view.substr(last, next-last);
        last = next + 1;
        spdlog::trace("[{}] testing word {} in phrase {}", __FUNCTION__, word, phrase);
        auto word_iter = _word_tiers.find(word);

        if (word_iter == _word_tiers.end()) {
            continue;
        }

        auto tier_iter = _enabled_tiers.find(word_iter->second);

        if (tier_iter == _enabled_tiers.end()) {
            continue;
        }

        contains_profanity = true;
    }

    if(!contains_profanity) {
        auto word = phrase_view.substr(last);
        spdlog::trace("[{}] testing word {} in phrase {}", __FUNCTION__, word, phrase);
        auto word_iter = _word_tiers.find(word);

        if (word_iter == _word_tiers.end()) {
            goto end;
        }

        auto tier_iter = _enabled_tiers.find(word_iter->second);

        if (tier_iter == _enabled_tiers.end()) {
            goto end;
        }

        contains_profanity = true;
    }

    end:
    spdlog::trace("[{}] phrase {} profane: {}", __FUNCTION__, phrase, contains_profanity);
    return contains_profanity;
}

bool censor_sensor::is_profane_ish(string phrase) {
    string_tolower(phrase);
    for(auto& [word, tier] : _word_tiers) {
        if(phrase.find(word) != string::npos) {
            auto tier_iter = _enabled_tiers.find(tier);

            if(tier_iter != _enabled_tiers.end()) {
                spdlog::trace("[{}] phrase {} profane ish", __FUNCTION__, phrase);
                return true;
            }
        }
    }

    spdlog::trace("[{}] phrase {} not profane ish", __FUNCTION__, phrase);
    return false;
}

string censor_sensor::clean_profanity(string phrase) {
    string_tolower(phrase);

    auto phrase_view = string_view(phrase);
    string::size_type last = 0;
    string::size_type next = 0;
    while ((next = phrase.find(' ', last)) != std::string::npos) {
        auto word = phrase_view.substr(last, next-last);
        last = next + 1;
        auto word_iter = _word_tiers.find(word);

        if (word_iter == _word_tiers.end()) {
            continue;
        }

        auto tier_iter = _enabled_tiers.find(word_iter->second);

        if (tier_iter == _enabled_tiers.end()) {
            continue;
        }

        string to_insert;
        for(string::size_type i = 0; i < word_iter->first.size(); i++) {
            to_insert.insert(0, "*");
        }
        phrase.replace(phrase.begin() + last, phrase.begin() + last - next, to_insert);
    }

    auto word = phrase_view.substr(last);
    spdlog::trace("[{}] testing word {} in phrase {}", __FUNCTION__, word, phrase);
    auto word_iter = _word_tiers.find(word);

    if (word_iter != _word_tiers.end()) {
        auto tier_iter = _enabled_tiers.find(word_iter->second);

        if (tier_iter != _enabled_tiers.end()) {
            string to_insert;
            for(string::size_type i = 0; i < word_iter->first.size(); i++) {
                to_insert.insert(0, "*");
            }
            phrase.replace(phrase.begin() + last, phrase.end(), to_insert);
        }
    }

    spdlog::trace("[{}] phrase {}", __FUNCTION__, phrase);
    return phrase;
}

string censor_sensor::clean_profanity_ish(string phrase) {
    string_tolower(phrase);
    for(auto& [word, tier] : _word_tiers) {
        auto pos = phrase.find(word);
        if(pos != string::npos) {
            auto tier_iter = _enabled_tiers.find(tier);

            if(tier_iter != _enabled_tiers.end()) {
                string to_insert;
                for(string::size_type i = 0; i < word.size(); i++) {
                    to_insert.insert(0, "*");
                }
                phrase.replace(phrase.begin() + pos, phrase.begin() + pos + word.size(), to_insert);
            }
        }
    }

    spdlog::trace("[{}] phrase {}", __FUNCTION__, phrase);
    return phrase;
}

void censor_sensor::enable_tier(uint32_t tier) {
    if(tier <= static_cast<uint32_t>(profanity_type::USER_ADDED)) {
        _enabled_tiers.insert(tier);
        spdlog::trace("[{}] tier {} enabled", __FUNCTION__, tier);
    }
}

void censor_sensor::disable_tier(uint32_t tier) {
    if(tier <= static_cast<uint32_t>(profanity_type::USER_ADDED)) {
        _enabled_tiers.erase(tier);
        spdlog::trace("[{}] tier {} disabled", __FUNCTION__, tier);
    }
}
