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


#include "get_company_applications_handler.h"

#include <spdlog/spdlog.h>

#include <messages/company/get_company_applications_request.h>
#include <messages/company/get_company_applications_response.h>
#include <repositories/companies_repository.h>
#include <repositories/company_members_repository.h>
#include <repositories/company_member_applications_repository.h>
#include <repositories/characters_repository.h>
#include "message_handlers/handler_macros.h"
#include <websocket_thread.h>
#include <ecs/components.h>
#include "macros.h"
#include <magic_enum.hpp>

#ifdef TEST_CODE
#include "../../../test/custom_server.h"
#endif

using namespace std;
using namespace chrono;

namespace ibh {
    template <class Server, class WebSocket>
    void handle_get_company_applications(Server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction, per_socket_data<WebSocket> *user_data,
                                   queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        MEASURE_TIME_OF_FUNCTION(trace);
        DESERIALIZE_WITH_PLAYING_CHECK(get_company_applications_request);

        companies_repository<database_transaction> companies_repo{};
        characters_repository<database_transaction> chars_repo{};
        company_member_applications_repository<database_transaction> applications_repo{};
        company_members_repository<database_transaction> company_members_repo{};

        auto company_member = company_members_repo.get_by_character_id(user_data->playing_character_id, transaction);
        if(!company_member) {
            get_company_applications_response response{"Not a member of a company", {}};
            auto response_msg = response.serialize();
            s->send(user_data->ws, response_msg, websocketpp::frame::opcode::value::TEXT);
            return;
        }

        if(company_member->member_level != magic_enum::enum_integer(company_member_level::COMPANY_SAGE) && company_member->member_level != magic_enum::enum_integer(company_member_level::COMPANY_ADMIN)) {
            get_company_applications_response response{"Not a sage or admin of company", {}};
            auto response_msg = response.serialize();
            s->send(user_data->ws, response_msg, websocketpp::frame::opcode::value::TEXT);
            return;
        }

        auto company = companies_repo.get(company_member->company_id, transaction);
        auto applications = applications_repo.get_by_company_id(company->id, transaction);

        vector<member> members;
        members.reserve(applications.size());

        for(auto const &application : applications) {
            auto character = chars_repo.get_character(application.character_id, transaction);
            members.emplace_back(character->id, character->level, move(character->name));
        }

        get_company_applications_response response{"", move(members)};
        auto response_msg = response.serialize();
        s->send(user_data->ws, response_msg, websocketpp::frame::opcode::value::TEXT);
    }

    template void handle_get_company_applications<server, websocketpp::connection_hdl>(server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                                                 per_socket_data<websocketpp::connection_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &user_connections);

#ifdef TEST_CODE
    template void handle_get_company_applications<custom_server, custom_hdl>(custom_server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                                     per_socket_data<custom_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> &user_connections);
#endif
}
