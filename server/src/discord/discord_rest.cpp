//
// Copyright (c) 2019 Sharon W (sharon at aegis dot gg)
//
// Distributed under the MIT License.
//
// Shamelessly copied and modified for IdleBossHunter

#include "discord_rest.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wduplicated-branches"
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <websocketpp/common/memory.hpp>
#include <websocketpp/http/response.hpp>
#include <asio.hpp>
#pragma GCC diagnostic pop

#include <chrono>
#include <spdlog/spdlog.h>
#include <asio/ssl/error.hpp>
#include <asio/ssl/context.hpp>
#include <asio/ssl/stream.hpp>
#include <rapidjson/writer.h>
#include <messages/message.h>
#include <concurrentqueue.h>

unordered_map<string, asio::ip::basic_resolver<asio::ip::tcp>::results_type> discord_resolver_cache{};
unique_ptr<asio::io_context> discord_io_context = nullptr;
namespace ibh {
    extern string discord_token;
    extern string discord_channel_id;
    extern atomic<bool> discord_ready_to_send_messages;
    static const string discord_host{"discordapp.com"};
    moodycamel::ConcurrentQueue<string> discord_message_queue{};
    moodycamel::ProducerToken ptok(discord_message_queue);

    // discord actually calls it create message. Don't blame me for their weird naming.
    string create_create_message(const string &msg) {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String(KEY_STRING("content"));
        writer.String(msg.c_str(), msg.length());

        writer.String(KEY_STRING("tts"));
        writer.Bool(false);

        writer.EndObject();
        return sb.GetString();
    }

    void send_discord_message(const string &msg) {
        discord_message_queue.enqueue(ptok, msg);
    }

    int32_t send_discord_message_internal(const string &msg) {
        websocketpp::http::parser::response hresponse;

        int32_t retry = -1;
        asio::ip::basic_resolver<asio::ip::tcp>::results_type r;

        auto it = discord_resolver_cache.find(discord_host);
        if (it == discord_resolver_cache.end()) {
            if (discord_io_context == nullptr) {

            }
            asio::ip::tcp::resolver resolver(*discord_io_context);
            r = resolver.resolve(discord_host, "443");
            discord_resolver_cache.emplace(discord_host, r);
        } else
            r = it->second;

        asio::ssl::context ctx(asio::ssl::context::tlsv12);

        ctx.set_options(
                asio::ssl::context::default_workarounds
                | asio::ssl::context::no_sslv2
                | asio::ssl::context::no_sslv3);

        asio::ssl::stream<asio::ip::tcp::socket> socket(*discord_io_context, ctx);
        SSL_set_tlsext_host_name(socket.native_handle(), discord_host.data());

        asio::connect(socket.lowest_layer(), r);

        asio::error_code handshake_ec;
        socket.handshake(asio::ssl::stream_base::client, handshake_ec);

        auto body = create_create_message(msg);

        asio::streambuf request;
        ostream request_stream(&request);
        request_stream << "POST /api/v6/channels/" << discord_channel_id << "/messages HTTP/1.0\r\n";
        request_stream << "Host: " << discord_host << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Authorization: Bot " << discord_token << "\r\n";
        request_stream << "User-Agent: IdleBossHunterBot IdleBossHunterBot (https://github.com/Oipo/IdleBossHunter, 0.0.1)\r\n";
        request_stream << "Connection: close\r\n";
        request_stream << "Content-Length: " << body.size() << "\r\n";
        request_stream << "Content-Type: application/json\r\n\r\n";
        request_stream << body;

        asio::write(socket, request);
        asio::streambuf response;
        asio::read_until(socket, response, "\r\n");
        stringstream response_content;
        response_content << &response;

        asio::error_code error;
        while (asio::read(socket, response, asio::transfer_at_least(1), error)) {
            response_content << &response;
        }

        string result = response_content.str();
        spdlog::trace("[{}] discord response {}", __FUNCTION__, result);


        istringstream istrm(result);
        hresponse.consume(istrm);

        auto test = hresponse.get_header("retry-after");
        if (!test.empty()) {
            retry = stoul(test);
            spdlog::info("[{}] retry-after found: {}", __FUNCTION__, retry);
        }

        if (error != asio::error::eof && error != asio::ssl::error::stream_truncated) {
            spdlog::error("[{}] error during sending: {} - {}", __FUNCTION__, error.value(), error.message());
        }

        return retry;
    }

    vector<thread> run_discord_rest(atomic<bool> &quit) {
        discord_io_context = make_unique<asio::io_context>();
        auto discord_io_thread = thread([&quit] {
            while (!quit.load(memory_order_acquire)) {
                try {
                    discord_io_context->run_for(chrono::milliseconds(125));
                    this_thread::sleep_for(chrono::milliseconds(10));
                } catch (const exception &e) {
                    spdlog::error("[discord_io_thread] exception {}", e.what());
                }
            }
        });

        auto discord_send_thread = thread([&quit] {
            while(!discord_ready_to_send_messages.load(memory_order_acquire) && !quit.load(memory_order_acquire)) {
                this_thread::sleep_for(chrono::milliseconds(10));
            }

            while (!quit.load(memory_order_acquire)) {
                try {
                    string msg;
                    while (discord_message_queue.try_dequeue_from_producer(ptok, msg) && discord_ready_to_send_messages.load(memory_order_acquire)) {
                        auto retry = send_discord_message_internal(msg);
                        if(retry >= 0) {
                            spdlog::info("[discord_send_thread] retry found: {}", retry);
                            auto now = chrono::system_clock::now();
                            auto next_tick = now + chrono::milliseconds(retry);
                            while(now < next_tick || !discord_ready_to_send_messages.load(memory_order_acquire)) {
                                this_thread::sleep_for(chrono::milliseconds(10));
                                if(quit.load(memory_order_acquire)) {
                                    return;
                                }
                                now = chrono::system_clock::now();
                            }
                            send_discord_message_internal(msg);
                        }
                        this_thread::sleep_for(chrono::milliseconds(10));
                    }
                    this_thread::sleep_for(chrono::milliseconds(10));
                } catch (const exception &e) {
                    spdlog::error("[discord_send_thread] exception {}", e.what());
                }
            }
        });

        vector<thread> t;
        t.emplace_back(move(discord_io_thread));
        t.emplace_back(move(discord_send_thread));
        return t;
    }
}