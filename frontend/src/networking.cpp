/*
    IdleBossHunter client
    Copyright (C) 2016  Michael de Lang

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

#include "networking.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/websocket.h>
#else


#include <functional>
#endif

#include <ecs/systems/scene_system.h>
#include <scenes/gui_scenes/connection_lost_scene.h>

using namespace std;

#ifdef __EMSCRIPTEN__
EM_BOOL WebSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData)
{
    try {
        spdlog::debug("open(eventType={})\n", eventType);

        if (userData == nullptr) {
            spdlog::error("[{}] No registry passed!", __FUNCTION__);
            exit(1);
        }

        auto *ss = static_cast<ibh::scene_system *>(userData);
        ss->set_connected(true);
    } catch (exception const &e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }

    return 0;
}

EM_BOOL WebSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData)
{
    try {
        spdlog::debug("close(eventType={}, wasClean={}, code={}, reason=%s)\n", eventType, e->wasClean, e->code, e->reason);

        if (userData == nullptr) {
            spdlog::error("[{}] No manager passed!", __FUNCTION__);
            exit(1);
        }

        scene_system *manager = static_cast<scene_system *>(userData);
        manager->force_goto_scene(make_unique<connection_lost_scene>());
    } catch (exception const &e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }
    return 0;
}

EM_BOOL WebSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
{
    spdlog::debug("error(eventType={})", eventType);
    return 0;
}


EM_BOOL WebSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
{
    try {
        spdlog::debug("message(eventType={}, numBytes={}, isText={})", eventType, e->numBytes, e->isText);

        if (userData == nullptr) {
            spdlog::error("[{}] No manager passed!", __FUNCTION__);
            exit(1);
        }

        if (e->isText) {
            spdlog::trace("[{}] text data: {}", __FUNCTION__, reinterpret_cast<char *>(e->data));
        } else {
            spdlog::trace("[{}] binary data, ignoring", __FUNCTION__);
            return 0;
        }

        rapidjson::Document d{};
        d.Parse(reinterpret_cast<char *>(e->data), e->numBytes);

        if (d.HasParseError() || !d.IsObject() || !d.HasMember("type") || !d["type"].IsUint64()) {
            spdlog::warn("[{}] deserialize failed", __FUNCTION__);
            return 0;
        }

        scene_system *manager = static_cast<scene_system *>(userData);
        manager->handle_message(d);
    } catch (exception const &e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }

    return 0;
}


void init_net(config const &config, entt::registry &es, scene_system &ss) {
    if (!emscripten_websocket_is_supported()) {
        spdlog::error("[{}] Websocket not supported", __FUNCTION__);
        exit(1);
    }

    EmscriptenWebSocketCreateAttributes attr;
    emscripten_websocket_init_create_attributes(&attr);
    attr.url = config.server_url.c_str();

    EMSCRIPTEN_WEBSOCKET_T socket = emscripten_websocket_new(&attr);
    if (socket <= 0)
    {
        spdlog::error("[{}] Websocket creation failed, code {}", __FUNCTION__, socket);
        exit(1);
    }

    auto entt = es.create();
    es.emplace<socket_component>(entt, socket);

    emscripten_websocket_set_onopen_callback(socket, nullptr, WebSocketOpen);
    emscripten_websocket_set_onclose_callback(socket, &ss, WebSocketClose);
    emscripten_websocket_set_onerror_callback(socket, nullptr, WebSocketError);
    emscripten_websocket_set_onmessage_callback(socket, &ss, WebSocketMessage);
}

#else
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using context_ptr = websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>;

context_ptr on_tls_init(ibh::config const &config, websocketpp::connection_hdl hdl) {
    namespace asio = websocketpp::lib::asio;

    context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12);

    try {
        ctx->set_options(asio::ssl::context::default_workarounds |
                         asio::ssl::context::no_sslv2 |
                         asio::ssl::context::no_sslv3 |
                         asio::ssl::context::no_tlsv1 |
                         asio::ssl::context::no_tlsv1_1 |
                         asio::ssl::context::single_dh_use);
        //ctx->set_password_callback(bind(&get_password, config, ::_1, ::_2));
        ctx->set_verify_mode(asio::ssl::verify_none);
//
//        std::string ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";;
//
//        if (SSL_CTX_set_cipher_list(ctx->native_handle(), ciphers.c_str()) != 1) {
//            spdlog::error("[{}] error setting cipher list", __FUNCTION__);
//        }
    } catch (std::exception &e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }
    return ctx;
}

void on_open(ibh::client *c, entt::entity entt, entt::registry *es, ibh::scene_system *ss, websocketpp::connection_hdl hdl) {
    try {
        spdlog::debug("open()");

        if (es == nullptr) {
            spdlog::error("[{}] No registry passed!", __FUNCTION__);
            exit(1);
        }

        auto &socket = ss->get_socket();
        socket.hdl = hdl;
        ss->set_connected(true);
    } catch (exception const &e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }
}

void on_message(ibh::client *c, ibh::scene_system *manager, websocketpp::connection_hdl hdl, ibh::client::message_ptr msg) {
    try {
        string const &message = msg->get_payload();

        if (message.empty() || message.length() < 4) {
            spdlog::warn("[{}] deserialize encountered empty buffer", __FUNCTION__);
            return;
        }

        spdlog::trace("[{}] text data: {}", __FUNCTION__, message);

        rapidjson::Document d{};
        d.Parse(message.c_str(), message.size());

        if (d.HasParseError() || !d.IsObject() || !d.HasMember("type") || !d["type"].IsUint64()) {
            spdlog::warn("[{}] deserialize failed", __FUNCTION__);
            return;
        }

        manager->handle_message(d);
    } catch (exception const &e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }
}

void on_close(ibh::client *c, ibh::scene_system *manager, websocketpp::connection_hdl hdl) {
    try {
        ibh::client::connection_ptr con = c->get_con_from_hdl(hdl);
        spdlog::debug("close(reason=%s)", con->get_ec().message());

        if (manager == nullptr) {
            spdlog::error("[{}] No manager passed!", __FUNCTION__);
            exit(1);
        }

        manager->force_goto_scene(make_unique<ibh::connection_lost_scene>());
    } catch (exception const &e) {
        spdlog::error("[{}] exception {}", __FUNCTION__, e.what());
    }
}

void on_fail(ibh::client *c, websocketpp::connection_hdl hdl) {
    auto con = c->get_con_from_hdl(hdl);
    spdlog::error("[{}] fail connection {} {}", __FUNCTION__, con->get_ec().value(), con->get_ec().message());
}

thread ibh::init_net(config const &config, entt::registry &es, scene_system &manager) {
    atomic<bool> init_done = false;

    auto t = thread([&config, &init_done, &es, ss = &manager] {
        client socket_client;
        auto entt = es.create();
        auto &socket = es.emplace<socket_component>(entt, false, websocketpp::connection_hdl {}, &socket_client);

        try {
            socket_client.clear_access_channels(websocketpp::log::alevel::all);
            socket_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
            socket_client.clear_error_channels(websocketpp::log::elevel::all);
            socket_client.init_asio();

            socket_client.set_message_handler(bind(&on_message, &socket_client, ss, ::_1, ::_2));
            socket_client.set_fail_handler(bind(&on_fail, &socket_client, ::_1));
            socket_client.set_open_handler(bind(&on_open, &socket_client, entt, &es, ss, ::_1));
            socket_client.set_close_handler(bind(&on_close, &socket_client, ss, ::_1));
            socket_client.set_tls_init_handler(bind(&on_tls_init, config, ::_1));
            socket_client.set_pong_timeout(2500);
            socket_client.set_open_handshake_timeout(2500);
            socket_client.set_close_handshake_timeout(2500);

            websocketpp::lib::error_code ec;
            client::connection_ptr con = socket_client.get_connection(config.server_url, ec);

            if (ec) {
                spdlog::error("[websocket++] initialization error: {}", ec.message());
                init_done.store(true, memory_order_release);
                return;
            }

            socket_client.connect(con);
            socket.running = true;
            init_done.store(true, memory_order_release);
            socket_client.run();
        } catch (websocketpp::exception const &e) {
            socket.running = false;
            spdlog::error("[websocket++] {}", e.what());
        } catch (const std::exception &e) {
            socket.running = false;
            spdlog::error("[websocket++] regular exception {}", e.what());
        }
        init_done.store(true, memory_order_release);
    });

    while (!init_done.load(memory_order_acquire)) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    return t;
}

#endif