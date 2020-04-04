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


#include "get_company_listing_handler.h"

#include <spdlog/spdlog.h>

#include <messages/company/get_company_listing_request.h>
#include <messages/company/get_company_listing_response.h>
#include "message_handlers/handler_macros.h"
#include <websocket_thread.h>
#include <repositories/companies_repository.h>
#include <repositories/company_members_repository.h>
#include <repositories/company_stats_repository.h>
#include <repositories/characters_repository.h>
#include "macros.h"

#ifdef TEST_CODE
#include "../../../test/custom_server.h"
#endif

using namespace std;
using namespace chrono;

namespace ibh {
    template <class Server, class WebSocket>
    void handle_get_company_listing(Server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction, per_socket_data<WebSocket> *user_data,
                                 queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<WebSocket>> &user_connections) {
        MEASURE_TIME_OF_FUNCTION(trace);
        DESERIALIZE_WITH_PLAYING_CHECK(get_company_listing_request);

        companies_repository<database_transaction> company_repo{};
        company_members_repository<database_transaction> company_member_repo{};
        company_stats_repository<database_transaction> company_stat_repo{};
        characters_repository<database_transaction> char_repo{};

        auto companies = company_repo.get_all(transaction);
        vector<company> msg_companies;
        msg_companies.reserve(companies.size());
        for(auto &c : companies) {
            vector<string> msg_members;
            vector<bonus> msg_bonuses;

            auto members = company_member_repo.get_by_company_id(c.id, transaction);
            msg_members.reserve(members.size());
            for(auto &member : members) {
                auto player = char_repo.get_character(member.character_id, transaction);
                msg_members.emplace_back(player->name);
            }

            auto bonuses = company_stat_repo.get_by_company_id(c.id, transaction);
            for(auto &bonus : bonuses) {
                msg_bonuses.emplace_back(bonus.stat_id, bonus.value);
            }

            msg_companies.emplace_back(c.name, msg_members, msg_bonuses);
        }

        get_company_listing_response resp{"", msg_companies};
        s->send(user_data->ws, resp.serialize(), websocketpp::frame::opcode::value::TEXT);
    }

    template void handle_get_company_listing<server, websocketpp::connection_hdl>(server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                                               per_socket_data<websocketpp::connection_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &user_connections);

#ifdef TEST_CODE
    template void handle_get_company_listing<custom_server, custom_hdl>(custom_server *s, rapidjson::Document const &d, unique_ptr<database_transaction> const &transaction,
                                                           per_socket_data<custom_hdl> *user_data, queue_abstraction<unique_ptr<queue_message>> *q, ibh_flat_map<uint64_t, per_socket_data<custom_hdl>> &user_connections);
#endif
}
