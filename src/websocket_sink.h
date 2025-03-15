//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <ixwebsocket/IXWebSocket.h>
#include "spdlog/sinks/base_sink.h"
#include "game.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

template<typename Mutex>
class websocket_sink : public spdlog::sinks::base_sink <Mutex>
{
public:
    websocket_sink(CGame * server)
        : server(server)
    {
    }

    CGame * server;
protected:
    void sink_it_(const spdlog::details::log_msg & msg) override
    {
        // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
        // msg.payload (before v1.3.0: msg.raw) contains pre formatted log

        // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        //std::cout << fmt::to_string(formatted);

        if (server)
        {
            json j;
            j["type"] = "log";
            j["level"] = spdlog::level::to_short_c_str(msg.level);
            j["timestamp"] = msg.time.time_since_epoch().count();
            j["thread_id"] = msg.thread_id;
            j["message"] = fmt::to_string(formatted);

            std::lock_guard<std::recursive_mutex> l(server->manager_websocket_list_mtx);

            // send log message to all manager websockets
            for (auto & wspair : server->websocket_manager_clients)
            {
                auto & ws = wspair.first;
                auto & manager = wspair.second;
                if (ws && ws->getReadyState() == ix::ReadyState::Open && manager->authenticated && manager->active)
                {
                    ws->sendText(j.dump());
                }
            }
        }
    }

    void flush_() override
    {
        //std::cout << std::flush;
    }
};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using websocket_sink_mt = websocket_sink<std::mutex>;
using websocket_sink_st = websocket_sink<spdlog::details::null_mutex>;
