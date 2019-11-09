/*
    IdleBossHunter
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

namespace lotr {
    struct user {
        uint64_t id;
        string username;
        string password;
        string email;
        uint16_t login_attempts;
        string verification_code;
        uint16_t max_characters;
        uint16_t is_game_master;

        user() : id(), username(), password(), email(), login_attempts(), verification_code(), max_characters(), is_game_master() {}
        user(uint64_t id, string username, string password, string email, uint16_t login_attempts, string verification_code, uint16_t no_of_players, uint16_t admin)
        : id(id), username(move(username)), password(move(password)), email(move(email)), login_attempts(login_attempts), verification_code(move(verification_code)), max_characters(no_of_players), is_game_master(admin) {}
    };

    struct banned_user {
        uint64_t id;
        string ip;
        optional<user> _user;
        optional<chrono::system_clock::time_point> until;

        banned_user() : id(), ip(), _user(), until() {}
        banned_user(uint64_t id, string ip, optional<user> _user, optional<chrono::system_clock::time_point> until)
        : id(id), ip(move(ip)), _user(move(_user)), until(until) {}
    };

    struct character_stat {
        uint64_t id;
        uint64_t character_id;
        string name;
        int64_t value;

        character_stat() : id(), character_id(), name(), value() {}
        character_stat(uint64_t id, uint64_t character_id, string name, int64_t value) : id(id), character_id(character_id), name(move(name)), value(value) {}
    };

    struct db_item {
        uint64_t id;
        uint64_t character_id;
        string name;
        string slot;
        string equip_slot;

        db_item() : id(), character_id(), name(), slot() , equip_slot(){}
        db_item(uint64_t id, uint64_t character_id, string name, string slot, string equip_slot) : id(id), character_id(character_id), name(move(name)), slot(move(slot)), equip_slot(move(equip_slot)) {}
    };

    struct db_item_stat {
        uint64_t id;
        uint64_t item_id;
        string name;
        int64_t value;

        db_item_stat() : id(), item_id(), name(), value() {}
        db_item_stat(uint64_t id, uint64_t item_id, string name, int64_t value) : id(id), item_id(item_id), name(move(name)), value(value) {}
    };

    struct db_character {
        uint64_t id;
        uint64_t user_id;
        uint32_t slot;
        uint64_t level;
        uint64_t gold;
        uint64_t xp;
        uint64_t skill_points;
        uint32_t x;
        uint32_t y;
        string name;
        string race;
        string _class;
        string map;
        vector<character_stat> stats;
        vector<db_item> items;

        db_character() : id(), user_id(), slot(), level(), gold(), xp(), skill_points(), x(), y(), name(), race(), _class(), map(), stats(), items() {}
        db_character(uint64_t id, uint64_t user_id, uint32_t slot, uint64_t level, uint64_t gold, uint64_t xp, uint64_t skill_points, uint32_t x, uint32_t y, string name, string race, string _class, string map, vector<character_stat> stats, vector<db_item> items)
        : id(id), user_id(user_id), slot(slot), level(level), gold(gold), xp(xp), skill_points(skill_points), x(x), y(y), name(move(name)), race(move(race)),
        _class(move(_class)), map(move(map)), stats(move(stats)), items(move(items)) {}
    };

    struct db_boss_stat {
        uint64_t id;
        uint64_t boss_id;
        string name;
        int64_t value;

        db_boss_stat() : id(), boss_id(), name(), value() {}
        db_boss_stat(uint64_t id, uint64_t boss_id, string name, int64_t value) : id(id), boss_id(boss_id), name(move(name)), value(value) {}
    };

    struct db_boss {
        uint64_t id;
        string name;
        vector<db_boss_stat> stats;

        db_boss() : id(), name(), stats() {}
        db_boss(uint64_t id, string name, vector<db_boss_stat> stats) : id(id), name(move(name)), stats(move(stats)) {}
    };

    struct db_clan_stat {
        uint64_t id;
        uint64_t clan_id;
        string name;
        int64_t value;

        db_clan_stat() : id(), clan_id(), name(), value() {}
        db_clan_stat(uint64_t id, uint64_t clan_id, string name, int64_t value) : id(id), clan_id(clan_id), name(move(name)), value(value) {}
    };

    struct db_clan_building {
        uint64_t id;
        uint64_t clan_id;
        string name;

        db_clan_building() : id(), clan_id(), name() {}
        db_clan_building(uint64_t id, uint64_t clan_id, string name) : id(id), clan_id(clan_id), name(move(name)) {}
    };

    struct db_clan {
        uint64_t id;
        string name;
        vector<db_clan_stat> stats;
        vector<db_clan_building> buildings;

        db_clan() : id(), name(), stats(), buildings() {}
        db_clan(uint64_t id, string name, vector<db_clan_stat> stats, vector<db_clan_building> buildings) : id(id), name(move(name)), stats(move(stats)), buildings(move(buildings)) {}
    };
}

