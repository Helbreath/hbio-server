//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include "time_utils.h"

class CClient;

class connection_state_hb : public ix::ConnectionState
{
public:
    std::shared_ptr<CClient> client;
    std::weak_ptr<ix::WebSocket> websocket;

    // deprecating in favor of iterative lists
    uint32_t client_handle = 0;

private:
    bool disconnected = false;
    time_point<steady_clock> connecttime = now();
    time_point<steady_clock> disconnecttime = now();
    time_point<steady_clock> lastpackettime = now();
    time_point<steady_clock> lastchecktime = now();
    time_point<steady_clock> logintime = now();

public:
    void set_disconnected(bool s);
    void set_connect_time(time_point<steady_clock> t);
    void set_disconnect_time(time_point<steady_clock> t);
    void set_last_packet_time(time_point<steady_clock> t);
    void set_last_check_time(time_point<steady_clock> t);
    void set_login_time(time_point<steady_clock> t);

    bool get_disconnected();
    time_point<steady_clock> get_connect_time();
    time_point<steady_clock> get_disconnect_time();
    time_point<steady_clock> get_last_packet_time();
    time_point<steady_clock> get_last_check_time();
    time_point<steady_clock> get_login_time();

    friend CClient;
};
