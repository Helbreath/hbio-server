//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXConnectionState.h>
#include "streams.h"
#include "time_utils.h"
#include "socket_defines.h"
#include "types.h"

class manager_client : public std::enable_shared_from_this<manager_client>, public ix::ConnectionState
{
public:
    manager_client() = default;
    ~manager_client() = default;

    manager_status_t current_status = manager_status_t::CONNECTED;

    std::list<stream_write *> outgoingqueue{};

    std::string address{};
    uint32_t client_handle{ 0 };
    bool authenticated = false;
    bool active = false;

    time_point<system_clock> connecttime = now();
    time_point<system_clock> lastpackettime = now();
    time_point<system_clock> lastchecktime = now();
    time_point<system_clock> logintime = now();
    std::weak_ptr<ix::WebSocket> websocket{};
    uint64_t socknum{};

    std::shared_ptr<ix::WebSocket> get_websocket() const noexcept
    {
        auto ws = websocket.lock();
        if (!ws) return nullptr;
        return ws;
    }

    std::shared_ptr<manager_client> get_ptr()
    {
        return shared_from_this();
    }
};
