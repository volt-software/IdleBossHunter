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
#include <ibh_containers.h>
#include <messages/message.h>
#include "queue_abstraction.h"

using namespace std;

namespace ibh {
    struct stat_component;
    using outward_queues = queue_abstraction<outward_message>;

    struct queue_message {
        uint64_t type;
        uint64_t connection_id;

        queue_message(uint64_t type, uint64_t connection_id) noexcept : type(type), connection_id(connection_id) {}
        virtual ~queue_message() = default;
        queue_message(const queue_message&) noexcept = default;
        queue_message(queue_message&&) noexcept = default;
        queue_message& operator=(const queue_message &) noexcept = default;
        queue_message& operator=(queue_message&&) noexcept = default;
    };

    // company

    struct accept_application_message : queue_message {
        uint64_t applicant_id;
        static constexpr uint64_t _type = generate_type<accept_application_message>();

        accept_application_message(uint64_t connection_id, uint64_t applicant_id) noexcept;
        ~accept_application_message() override = default;
        accept_application_message(const accept_application_message&) noexcept = default;
        accept_application_message(accept_application_message&&) noexcept = default;
        accept_application_message& operator=(const accept_application_message &) noexcept = default;
        accept_application_message& operator=(accept_application_message&&) noexcept = default;
    };

    struct create_company_message : queue_message {
        string company_name;
        static constexpr uint64_t _type = generate_type<create_company_message>();

        create_company_message(uint64_t connection_id, string company_name) noexcept;
        ~create_company_message() override = default;
        create_company_message(const create_company_message&) = default;
        create_company_message(create_company_message&&) noexcept = default;
        create_company_message& operator=(const create_company_message &) = default;
        create_company_message& operator=(create_company_message&&) noexcept = default;
    };

    struct increase_bonus_message : queue_message {
        uint32_t bonus_type;
        static constexpr uint64_t _type = generate_type<increase_bonus_message>();

        increase_bonus_message(uint64_t connection_id, uint32_t bonus_type) noexcept;
        ~increase_bonus_message() override = default;
        increase_bonus_message(const increase_bonus_message&) noexcept = default;
        increase_bonus_message(increase_bonus_message&&) noexcept = default;
        increase_bonus_message& operator=(const increase_bonus_message &) noexcept = default;
        increase_bonus_message& operator=(increase_bonus_message&&) noexcept = default;
    };

    struct join_company_message : queue_message {
        string company_name;
        static constexpr uint64_t _type = generate_type<join_company_message>();

        join_company_message(uint64_t connection_id, string company_name) noexcept;
        ~join_company_message() override = default;
        join_company_message(const join_company_message&) = default;
        join_company_message(join_company_message&&) noexcept = default;
        join_company_message& operator=(const join_company_message &) = default;
        join_company_message& operator=(join_company_message&&) noexcept = default;
    };

    struct leave_company_message : queue_message {
        static constexpr uint64_t _type = generate_type<leave_company_message>();

        explicit leave_company_message(uint64_t connection_id) noexcept;
        ~leave_company_message() override = default;
        leave_company_message(const leave_company_message&) noexcept = default;
        leave_company_message(leave_company_message&&) noexcept = default;
        leave_company_message& operator=(const leave_company_message &) noexcept = default;
        leave_company_message& operator=(leave_company_message&&) noexcept = default;
    };

    struct reject_application_message : queue_message {
        uint64_t applicant_id;
        static constexpr uint64_t _type = generate_type<reject_application_message>();

        reject_application_message(uint64_t connection_id, uint64_t applicant_id) noexcept;
        ~reject_application_message() override = default;
        reject_application_message(const reject_application_message&) noexcept = default;
        reject_application_message(reject_application_message&&) noexcept = default;
        reject_application_message& operator=(const reject_application_message &) noexcept = default;
        reject_application_message& operator=(reject_application_message&&) noexcept = default;
    };

    struct set_tax_message : queue_message {
        uint32_t tax_percentage;
        static constexpr uint64_t _type = generate_type<set_tax_message>();

        set_tax_message(uint64_t connection_id, uint32_t tax_percentage) noexcept;
        ~set_tax_message() override = default;
        set_tax_message(const set_tax_message&) noexcept = default;
        set_tax_message(set_tax_message&&) noexcept = default;
        set_tax_message& operator=(const set_tax_message &) noexcept = default;
        set_tax_message& operator=(set_tax_message&&) noexcept = default;
    };

    // uac

    struct player_enter_message : queue_message {
        uint64_t character_id;
        string character_name;
        string race;
        string baseclass;
        vector<stat_component> player_stats;
        uint64_t level;
        uint64_t gold;
        uint64_t xp;
        uint64_t skill_points;
        static constexpr uint64_t _type = generate_type<player_enter_message>();

        player_enter_message(uint64_t character_id, string character_name, string race, string baseclass, vector<stat_component> player_stats, uint64_t connection_id, uint64_t level, uint64_t gold, uint64_t xp, uint64_t skill_points) noexcept;
    };

    struct player_leave_message : queue_message {
        static constexpr uint64_t _type = generate_type<player_leave_message>();

        explicit player_leave_message(uint64_t connection_id) noexcept;
    };

    struct player_move_message : queue_message {
        static constexpr uint64_t _type = generate_type<player_move_message>();
        uint32_t x;
        uint32_t y;

        player_move_message(uint64_t connection_id, uint32_t x, uint32_t y) noexcept;
    };
}
