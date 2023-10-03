//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>

class CClient;

class connection_state_hb : public ix::ConnectionState
{
public:
    std::shared_ptr<CClient> client;
    std::weak_ptr<ix::WebSocket> websocket;

private:
    bool disconnected = false;
    std::chrono::time_point<std::chrono::steady_clock> connecttime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> disconnecttime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastpackettime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastchecktime = std::chrono::steady_clock::now();

public:
    void set_disconnected(bool s);
    void set_connect_time(std::chrono::time_point<std::chrono::steady_clock> t);
    void set_disconnect_time(std::chrono::time_point<std::chrono::steady_clock> t);
    void set_last_packet_time(std::chrono::time_point<std::chrono::steady_clock> t);
    void set_last_check_time(std::chrono::time_point<std::chrono::steady_clock> t);

    bool get_disconnected();
    std::chrono::time_point<std::chrono::steady_clock> get_connect_time();
    std::chrono::time_point<std::chrono::steady_clock> get_disconnect_time();
    std::chrono::time_point<std::chrono::steady_clock> get_last_packet_time();
    std::chrono::time_point<std::chrono::steady_clock> get_last_check_time();

    friend CClient;
};
