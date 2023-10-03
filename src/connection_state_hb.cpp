//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "connection_state_hb.h"
#include "Client.h"

void connection_state_hb::set_disconnected(bool s)
{
    disconnected = s;
    if (client) client->disconnected = s;
}
void connection_state_hb::set_connect_time(std::chrono::time_point<std::chrono::steady_clock> t)
{
    connecttime = t;
    if (client) client->connecttime = t;
}
void connection_state_hb::set_disconnect_time(std::chrono::time_point<std::chrono::steady_clock> t)
{
    disconnecttime = t;
    if (client) client->disconnecttime = t;
}
void connection_state_hb::set_last_packet_time(std::chrono::time_point<std::chrono::steady_clock> t)
{
    lastpackettime = t;
    if (client) client->lastpackettime = t;
}
void connection_state_hb::set_last_check_time(std::chrono::time_point<std::chrono::steady_clock> t)
{
    lastchecktime = t;
    if (client) client->lastchecktime = t;
}

bool connection_state_hb::get_disconnected()
{
    if (!client) return disconnected;
    if (client->disconnected || disconnected)
        return true;
    return false;
}

std::chrono::time_point<std::chrono::steady_clock> connection_state_hb::get_connect_time()
{
    if (!client) return connecttime;
    if (client->connecttime > connecttime)
        return client->connecttime;
    return connecttime;
}

std::chrono::time_point<std::chrono::steady_clock> connection_state_hb::get_disconnect_time()
{
    if (!client) return disconnecttime;
    if (client->disconnecttime > disconnecttime)
        return client->disconnecttime;
    return connecttime;
}

std::chrono::time_point<std::chrono::steady_clock> connection_state_hb::get_last_packet_time()
{
    if (!client) return lastpackettime;
    if (client->lastpackettime > lastpackettime)
        return client->lastpackettime;
    return lastpackettime;
}

std::chrono::time_point<std::chrono::steady_clock> connection_state_hb::get_last_check_time()
{
    if (!client) return lastchecktime;
    if (client->lastchecktime > lastchecktime)
        return client->lastchecktime;
    return lastchecktime;
}
