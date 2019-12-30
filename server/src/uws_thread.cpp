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

#include "uws_thread.h"
#include <spdlog/spdlog.h>
#include <ibh_containers.h>
#include <rapidjson/document.h>
#include <message_handlers/user_access/login_handler.h>
#include <message_handlers/user_access/register_handler.h>
#include <message_handlers/user_access/play_character_handler.h>
#include <message_handlers/user_access/create_character_handler.h>
#include <message_handlers/user_access/delete_character_handler.h>
#include <message_handlers/user_access/character_select_handler.h>
#include <message_handlers/chat/public_chat_handler.h>
#include <message_handlers/moderator/set_motd_handler.h>
#include <messages/user_access/login_request.h>
#include <messages/user_access/register_request.h>
#include <messages/user_access/play_character_request.h>
#include <messages/user_access/create_character_request.h>
#include <messages/user_access/delete_character_request.h>
#include <messages/user_access/character_select_request.h>
#include <messages/user_access/character_select_response.h>
#include <messages/chat/message_request.h>
#include <messages/moderator/set_motd_request.h>
#include <message_handlers/handler_macros.h>
#include <messages/user_access/user_left_game_response.h>
#include "per_socket_data.h"
#include <ecs/components.h>

using namespace std;
using namespace ibh;

using message_router_type = ibh_flat_map<uint32_t, function<void(server*, rapidjson::Document const &, shared_ptr<database_pool>, per_socket_data<websocketpp::connection_hdl>*,
                                                               moodycamel::ConcurrentQueue<unique_ptr<queue_message>> &, ibh_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> &)>>;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using context_ptr = websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>;

atomic<uint64_t> connection_id_counter = 0;
ibh_flat_map<uint64_t, per_socket_data<websocketpp::connection_hdl>> ibh::user_connections;
ibh_flat_map<websocketpp::connection_hdl, uint64_t> handle_to_connection_id_map;
moodycamel::ConcurrentQueue<unique_ptr<queue_message>> ibh::game_loop_queue;
string ibh::motd;
character_select_response ibh::select_response{{}, {}};
shared_mutex ibh::user_connections_mutex;
atomic<bool> init_done = false;

// See https://wiki.mozilla.org/Security/Server_Side_TLS for more details about
// the TLS modes. The code below demonstrates how to implement both the modern
enum tls_mode {
    MOZILLA_INTERMEDIATE = 1,
    MOZILLA_MODERN = 2
};

string get_password(config& config, size_t max_len, asio::ssl::context::password_purpose purpose) {
    return config.certificate_password;
}

//bool verify_certificate(bool preverified, asio::ssl::verify_context& ssl_verify_ctx) {
//    std::string errstr(X509_verify_cert_error_string(X509_STORE_CTX_get_error(ssl_verify_ctx.native_handle())));
//    spdlog::debug("[{}] {} {}", __FUNCTION__, preverified, errstr);
//    return true;
//}

context_ptr on_tls_init(config& config, websocketpp::connection_hdl hdl) {
    namespace asio = websocketpp::lib::asio;

    context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12);

    try {
        ctx->set_options(asio::ssl::context::default_workarounds |
                         asio::ssl::context::no_sslv2 |
                         asio::ssl::context::no_sslv3 |
                         asio::ssl::context::no_tlsv1 |
                         asio::ssl::context::no_tlsv1_1 |
                         asio::ssl::context::single_dh_use);
        ctx->set_password_callback(bind(&get_password, config, ::_1, ::_2));
        ctx->use_certificate_chain_file(config.certificate_file);
        ctx->use_private_key_file(config.private_key_file, asio::ssl::context::pem);

        std::string ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";;

        if (SSL_CTX_set_cipher_list(ctx->native_handle() , ciphers.c_str()) != 1) {
            spdlog::error("[{}] error setting cipher list", __FUNCTION__);
        }
    } catch (std::exception& e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }
    return ctx;
}

void on_open(atomic<bool> const &quit, websocketpp::connection_hdl hdl) {
    if(quit) {
        spdlog::debug("[{}] new connection in closing state", __FUNCTION__);
        return;
    }

    per_socket_data<websocketpp::connection_hdl> user_data{};
    //only called on connect
    user_data.connection_id = connection_id_counter++;
    user_data.user_id = 0;
    user_data.playing_character_slot = -1;
    user_data.username = "";
    user_data.subscription_tier = 0;
    user_data.ws = hdl;
    spdlog::debug("[{}] conn {} open connection", __FUNCTION__, user_data.connection_id);
    {
        unique_lock lock(user_connections_mutex);
        handle_to_connection_id_map[hdl] = user_data.connection_id;
        user_connections[user_data.connection_id] = user_data;
        spdlog::debug("[{}] conn {} inserted", __FUNCTION__, user_data.connection_id);
    }
}

void on_message(shared_ptr<database_pool> pool, message_router_type &message_router, server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    string const &message = msg->get_payload();

    if (message.empty() || message.length() < 4) {
        spdlog::warn("[{}] deserialize encountered empty buffer", __FUNCTION__);
        return;
    }

    auto id_map_it = handle_to_connection_id_map.find(hdl);
    if(id_map_it == cend(handle_to_connection_id_map)) {
        spdlog::warn("[{}] no id map", __FUNCTION__);
        generic_error_response resp{"Unrecognized message", "", "", true};
        s->send(hdl, resp.serialize(), websocketpp::frame::opcode::value::TEXT);
        return;
    }

    spdlog::trace("[{}] conn {} message {}", __FUNCTION__, id_map_it->second, message);

    per_socket_data<websocketpp::connection_hdl>* user_data = nullptr;
    {
        shared_lock lock(user_connections_mutex);
        auto user_data_it = user_connections.find(id_map_it->second);

        if (user_data_it == cend(user_connections)) {
            spdlog::warn("[{}] conn {} no user data", __FUNCTION__, id_map_it->second);
            generic_error_response resp{"Unrecognized message", "", "", true};
            s->send(hdl, resp.serialize(), websocketpp::frame::opcode::value::TEXT);
            return;
        }
        user_data = &user_data_it->second;
    }

    rapidjson::Document d{};
    d.Parse(&message[0], message.size());

    if (d.HasParseError() || !d.IsObject() || !d.HasMember("type") || !d["type"].IsUint()) {
        spdlog::warn("[{}] conn {} deserialize failed", __FUNCTION__, id_map_it->second);
        SEND_ERROR("Unrecognized message", "", "", true);
        return;
    }

    auto type = d["type"].GetUint();

    auto handler = message_router.find(type);
    if (handler != message_router.end()) {
        try {
            handler->second(s, d, pool, user_data, game_loop_queue, user_connections);
        } catch (exception const &e) {
            spdlog::error("[{}] some exception {} message_type {} user_id {} connection_id {} hdl_id {}", __FUNCTION__, e.what(), type, user_data->user_id, user_data->connection_id, id_map_it->second);
        }
    } else {
        spdlog::trace("[{}] conn {} no handler for type {}", __FUNCTION__, id_map_it->second, type);
    }
}

void on_close(server* s, websocketpp::connection_hdl hdl) {
    auto id_map_it = handle_to_connection_id_map.find(hdl);
    if(id_map_it == cend(handle_to_connection_id_map)) {
        spdlog::warn("[{}] no id map", __FUNCTION__);
        return;
    }
    {
        unique_lock lock(user_connections_mutex);
        auto user_data = user_connections.find(id_map_it->second);
        if (user_data == cend(user_connections)) {
            spdlog::warn("[{}] conn {} no user data", __FUNCTION__, id_map_it->second);
            for(auto const &[key, val] : user_connections) {
                spdlog::warn("[{}] conn {} no user data {}", __FUNCTION__, id_map_it->second, key);
            }
            handle_to_connection_id_map.erase(hdl);
            return;
        }
        if (user_data->second.playing_character_slot >= 0) {
            // TODO improve performance by using queue tokens
            game_loop_queue.enqueue(make_unique<player_leave_message>(user_data->second.connection_id));
        }
        if (!user_data->second.username.empty()) {
            auto same_user_id_it = find_if(begin(user_connections), end(user_connections),
                                           [&user_data](user_connections_type const &vt) noexcept { return vt.second.user_id == user_data->second.user_id && vt.second.connection_id != user_data->second.connection_id; });

            if (same_user_id_it == end(user_connections)) {
                user_left_game_response join_msg(user_data->second.username);
                auto join_msg_str = join_msg.serialize();
                for (auto &[conn_id, other_user_data] : user_connections) {
                    if (other_user_data.user_id != user_data->second.user_id) {
                        s->send(other_user_data.ws, join_msg_str, websocketpp::frame::opcode::value::TEXT);
                    }
                }
            }
        }
        spdlog::trace("[{}] conn {} close connection {}", __FUNCTION__, user_data->second.connection_id, user_data->second.user_id);
        user_connections.erase(id_map_it->second);
        handle_to_connection_id_map.erase(hdl);
    }
}

void on_fail(server* s, websocketpp::connection_hdl hdl) {
    server::connection_ptr con = s->get_con_from_hdl(hdl);
    auto id_map = handle_to_connection_id_map.find(hdl);
    if(id_map == cend(handle_to_connection_id_map)) {
        spdlog::error("[{}] fail connection {} {}", __FUNCTION__, con->get_ec().value(), con->get_ec().message());
        return;
    }


    auto user_data = user_connections.find(id_map->second);
    if(user_data == cend(user_connections)) {
        spdlog::error("[{}] fail connection {} {}", __FUNCTION__, con->get_ec().value(), con->get_ec().message());
        return;
    }

    spdlog::error("[{}] fail connection {} {} {}", __FUNCTION__, con->get_ec().value(), con->get_ec().message(), user_data->second.user_id);
}

void add_routes(message_router_type &message_router) {
    message_router.emplace(login_request::type, handle_login<server, websocketpp::connection_hdl>);
    message_router.emplace(register_request::type, handle_register<server, websocketpp::connection_hdl>);
    message_router.emplace(play_character_request::type, handle_play_character<server, websocketpp::connection_hdl>);
    message_router.emplace(create_character_request::type, handle_create_character<server, websocketpp::connection_hdl>);
    message_router.emplace(delete_character_request::type, handle_delete_character<server, websocketpp::connection_hdl>);
    message_router.emplace(character_select_request::type, handle_character_select<server, websocketpp::connection_hdl>);
    message_router.emplace(message_request::type, handle_public_chat<server, websocketpp::connection_hdl>);
    message_router.emplace(set_motd_request::type, set_motd_handler<server, websocketpp::connection_hdl>);
}

thread ibh::run_uws(config const &config, shared_ptr<database_pool> pool, server_handle &s_handle, atomic<bool> &quit) {
    connection_id_counter = 0;
    motd = "";

    auto t = thread([&config, pool, &s_handle, &quit] {
        server roa_server;
        s_handle.s = &roa_server;

        message_router_type message_router;
        add_routes(message_router);

        try {
            // Set logging settings
            //roa_server.set_access_channels(websocketpp::log::alevel::none);
            roa_server.clear_access_channels(websocketpp::log::alevel::all);

            // Initialize ASIO
            roa_server.init_asio();
            roa_server.set_reuse_addr(true);

            // Register our message handler
            roa_server.set_message_handler(bind(&on_message, pool, message_router, &roa_server, ::_1, ::_2));

            roa_server.set_fail_handler(bind(&on_fail, &roa_server, ::_1));
            roa_server.set_open_handler(bind(&on_open, cref(quit), ::_1));
            roa_server.set_close_handler(bind(&on_close, &roa_server, ::_1));
            roa_server.set_tls_init_handler(bind(&on_tls_init,config,::_1));
            roa_server.set_pong_timeout(2500);
            roa_server.set_open_handshake_timeout(2500);
            roa_server.set_close_handshake_timeout(2500);


            roa_server.listen(config.port);

            // Start the server accept loop
            roa_server.start_accept();
            init_done = true;

            // Start the ASIO io_service run loop
            roa_server.run();
        } catch (websocketpp::exception const & e) {
            spdlog::error("[websocket++] {}", e.what());
            quit = true;
        } catch (const std::exception & e) {
            spdlog::error("[websocket++] regular exception {}", e.what());
            quit = true;
        } catch (...) {
            spdlog::error("[websocket++] other exception");
            quit = true;
        }

        init_done = true;
        spdlog::warn("[websocket++] done");
    });

    while(!init_done) {}

    return t;
}
