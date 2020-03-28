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

#include "discord_thread.h"
#include <spdlog/spdlog.h>
#include <ibh_containers.h>
#include <rapidjson/document.h>
#include "per_socket_data.h"

#include <websocketpp/config/asio_client.hpp>
#include <rapidjson/writer.h>
#include <messages/message.h>
#include <messages/chat/message_response.h>

using namespace std;
using namespace ibh;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
using context_ptr = websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>;

namespace ibh {
    atomic<bool> discord_init_done = false;
    atomic<bool> discord_heartbeat_enabled = false;
    atomic<bool> discord_first_ack_received = false;
    atomic<bool> discord_ready_to_send_messages = false;
    websocketpp::connection_hdl discord_hdl;
    uint32_t heartbeat_interval;
    optional<uint64_t> last_s_received = {};
    string discord_token{};
    string discord_channel_id{};
    string discord_session_id{};

    // defined in uws_thread
    context_ptr on_tls_init(config &config, websocketpp::connection_hdl hdl);

    void send_to_discord(client *c, const string &msg) {
        if(!discord_init_done) {
            return;
        }

        spdlog::trace("[{}] sending {}", __FUNCTION__, msg);
        try {
            c->send(discord_hdl, msg, websocketpp::frame::opcode::text);
        } catch (const exception &e) {
            spdlog::error("[{}] error sending {}", __FUNCTION__, e.what());
        }
    }

    string create_heartbeat_message() {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String(KEY_STRING("op"));
        writer.Uint(1);

        writer.String(KEY_STRING("d"));
        if (last_s_received.has_value()) {
            writer.Uint64(*last_s_received);
        } else {
            writer.Null();
        }

        writer.EndObject();
        return sb.GetString();
    }

    string create_resume_message() {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String(KEY_STRING("token"));
        writer.String(discord_token.c_str(), discord_token.length());

        writer.String(KEY_STRING("session_id"));
        writer.String(discord_session_id.c_str(), discord_session_id.length());

        writer.String(KEY_STRING("s"));
        writer.Uint64(*last_s_received);

        writer.EndObject();
        return sb.GetString();
    }

    string create_identity_message() {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String(KEY_STRING("op"));
        writer.Uint(2);

        writer.String(KEY_STRING("d"));
        writer.StartObject();

        writer.String(KEY_STRING("token"));
        writer.String(discord_token.c_str(), discord_token.length());

        writer.String(KEY_STRING("properties"));
        writer.StartObject();

        writer.String(KEY_STRING("$os"));
        writer.String(KEY_STRING("linux"));

        writer.String(KEY_STRING("$browser"));
        writer.String(KEY_STRING("IdleBossHunterBot"));

        writer.String(KEY_STRING("$device"));
        writer.String(KEY_STRING("IdleBossHunterBot"));

        writer.EndObject();

        writer.EndObject();

        writer.String(KEY_STRING("intents"));
        writer.Uint((1u << 9u));

        writer.EndObject();
        return sb.GetString();
    }

    void discord_on_message(client *c, outward_queues *outward_queue, websocketpp::connection_hdl hdl, message_ptr msg) {
        try {
            string const &message = msg->get_payload();
            spdlog::trace("[{}] got message {}", __FUNCTION__, message);

            rapidjson::Document d{};
            d.Parse(&message[0], message.size());

            if (d.HasParseError() || !d.IsObject() || !d.HasMember("op")) {
                spdlog::warn("[{}] discord deserialize failed", __FUNCTION__);
                return;
            }

            if(d.HasMember("s") && d["s"].IsUint()) {
                last_s_received = d["s"].GetUint64();
            }

            if(d["op"].GetUint() == 0) {
                spdlog::trace("[{}] got op 0 Dispatch", __FUNCTION__);
                if(d["t"].GetString() == string{"MESSAGE_CREATE"} && d["d"]["channel_id"].GetString() == discord_channel_id) {
                    auto discord_msg = d["d"]["content"].GetString();
                    auto discord_user = d["d"]["author"]["username"].GetString();
                    auto now = chrono::system_clock::now();
                    auto new_chat_msg = make_unique<message_response>(discord_user, discord_msg, "discord", duration_cast<chrono::milliseconds>(now.time_since_epoch()).count());
                    outward_queue->enqueue({0ul, move(new_chat_msg)});
                    return;
                }

                if(d["t"].GetString() == string{"READY"}) {
                    discord_session_id = d["d"]["session_id"].GetString();
                    discord_ready_to_send_messages.store(true, memory_order_release);
                    return;
                }

                return;
            }

            if (d["op"].GetUint() == 1) {
                spdlog::trace("[{}] got op 1 Heartbeat", __FUNCTION__);
                send_to_discord(c, create_heartbeat_message());
                return;
            }

            if (d["op"].GetUint() == 9) {
                spdlog::trace("[{}] got op 9 Invalid Session", __FUNCTION__);
                discord_ready_to_send_messages.store(false, memory_order_release);
                discord_heartbeat_enabled.store(false, memory_order_release);
                discord_init_done.store(false, memory_order_release);
                discord_first_ack_received.store(false, memory_order_release);

                if(!d.HasMember("d") || !d["d"].IsBool() || !d["d"].GetBool()) {
                    discord_session_id = "";
                    last_s_received.reset();
                }

                c->stop();
                return;
            }

            if (d["op"].GetUint() == 10) {
                spdlog::trace("[{}] got op 10 Hello", __FUNCTION__);
                heartbeat_interval = d["d"]["heartbeat_interval"].GetUint();
                discord_heartbeat_enabled.store(true, memory_order_release);
                spdlog::info("[{}] enabled heartbeat", __FUNCTION__);
                return;
            }

            if (d["op"].GetUint() == 11) {
                spdlog::trace("[{}] got op 11 Heartbeat ACK", __FUNCTION__);
                if (!discord_first_ack_received) {
                    if(!discord_session_id.empty() && last_s_received.has_value()) {
                        send_to_discord(c, create_resume_message());
                        discord_first_ack_received.store(true, memory_order_release);
                        spdlog::info("[{}] sent resume", __FUNCTION__);
                    }

                    send_to_discord(c, create_identity_message());
                    discord_first_ack_received.store(true, memory_order_release);
                    spdlog::info("[{}] sent identify", __FUNCTION__);
                    return;
                }
            }
        } catch (websocketpp::exception const &e) {
            spdlog::error("[{}] websocketpp exception {}", __FUNCTION__, e.what());
        } catch (const exception &e) {
            spdlog::error("[{}] regular exception {}", __FUNCTION__, e.what());
        } catch (...) {
            spdlog::error("[{}] other exception", __FUNCTION__);
        }
    }

    void discord_on_open(client *c, atomic<bool> const &quit, websocketpp::connection_hdl hdl) {
        if (quit) {
            spdlog::debug("[{}] new connection in closing state", __FUNCTION__);
            return;
        }

        discord_hdl = hdl;
    }

    void discord_on_close(client *c, websocketpp::connection_hdl hdl) {
        spdlog::trace("[{}] conn close connection", __FUNCTION__);
        discord_heartbeat_enabled.store(false, memory_order_release);
    }

    void discord_on_fail(client *c, websocketpp::connection_hdl hdl) {
        server::connection_ptr con = c->get_con_from_hdl(hdl);

        spdlog::error("[{}] fail connection {} {} {}", __FUNCTION__, con->get_ec().value(), con->get_ec().message());
    }

    thread run_discord(config const &config, client_handle &c_handle, outward_queues &outward_queue, atomic<bool> &quit) {
        discord_token = config.discord_token;
        discord_channel_id = config.discord_channel_id;
        auto t = thread([&config, &c_handle, &outward_queue, &quit] {
            client ibh_client;
            c_handle.c = &ibh_client;

            auto heartbeat_thread = thread([&quit, c = &ibh_client] {
                while (!quit.load(memory_order_acquire) && !discord_heartbeat_enabled.load(memory_order_acquire)) {
                    this_thread::sleep_for(chrono::milliseconds(10));
                }

                auto next_tick = chrono::system_clock::now();
                while (!quit.load(memory_order_acquire)) {
                    auto now = chrono::system_clock::now();
                    if(discord_heartbeat_enabled.load(memory_order_acquire) && now >= next_tick) {
                        try {
                            send_to_discord(c, create_heartbeat_message());
                            next_tick += chrono::milliseconds(heartbeat_interval);
                        } catch (const exception &e) {
                            spdlog::error("[discord heartbeat] {}", e.what());
                        }
                    }
                    this_thread::sleep_for(chrono::milliseconds(10));
                }
            });

            while(!quit.load(memory_order_acquire)) {
                try {
                    // Set logging settings
                    //ibh_client.set_access_channels(websocketpp::log::alevel::none);
                    ibh_client.clear_access_channels(websocketpp::log::alevel::all);

                    // Initialize ASIO
                    ibh_client.init_asio();

                    // Register our message handler
                    ibh_client.set_message_handler(bind(&discord_on_message, &ibh_client, &outward_queue, ::_1, ::_2));

                    ibh_client.set_fail_handler(bind(&discord_on_fail, &ibh_client, ::_1));
                    ibh_client.set_open_handler(bind(&discord_on_open, &ibh_client, cref(quit), ::_1));
                    ibh_client.set_close_handler(bind(&discord_on_close, &ibh_client, ::_1));
                    ibh_client.set_tls_init_handler(bind(&on_tls_init, config, ::_1));
                    ibh_client.set_pong_timeout(2500);
                    ibh_client.set_open_handshake_timeout(2500);
                    ibh_client.set_close_handshake_timeout(2500);

                    websocketpp::lib::error_code ec;
                    client::connection_ptr con = ibh_client.get_connection("wss://gateway.discord.gg/?v=6&encoding=json", ec);
                    con->append_header("User-Agent", "IdleBossHunterBot (https://github.com/Oipo/IdleBossHunter, 0.0.1)");
                    if (ec) {
                        spdlog::error("could not create connection because: {}", ec.message());
                        return;
                    }

                    spdlog::info("[websocket++ discord] connecting to discord");
                    ibh_client.connect(con);
                    discord_init_done.store(true, memory_order_release);

                    ibh_client.run();
                } catch (websocketpp::exception const &e) {
                    spdlog::error("[websocket++ discord] websocketpp exception {}", e.what());
                } catch (const exception &e) {
                    spdlog::error("[websocket++ discord] regular exception {}", e.what());
                }

                discord_init_done.store(false, memory_order_release);
                discord_heartbeat_enabled.store(false, memory_order_release);
                discord_first_ack_received.store(false, memory_order_release);
            }
            heartbeat_thread.join();
            spdlog::warn("[websocket++ discord] done");
        });

        while(!discord_init_done.load(memory_order_acquire)) {
            this_thread::sleep_for(chrono::milliseconds(1));
        }

        return t;
    }
}