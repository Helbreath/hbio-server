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
    if (s) set_disconnect_time(now());
}

void connection_state_hb::set_connect_time(time_point<steady_clock> t)
{
    connecttime = t;
    if (client) client->connecttime = t;
}

void connection_state_hb::set_disconnect_time(time_point<steady_clock> t)
{
    disconnecttime = t;
    if (client) client->disconnecttime = t;
}

void connection_state_hb::set_last_packet_time(time_point<steady_clock> t)
{
    lastpackettime = t;
    if (client) client->lastpackettime = t;
}

void connection_state_hb::set_last_check_time(time_point<steady_clock> t)
{
    lastchecktime = t;
    if (client) client->lastchecktime = t;
}

void connection_state_hb::set_login_time(time_point<steady_clock> t)
{
    logintime = t;
    if (client) client->logintime = t;
}

bool connection_state_hb::get_disconnected()
{
    if (!client) return disconnected;
    if (client->disconnected || disconnected)
        return true;
    return false;
}

time_point<steady_clock> connection_state_hb::get_connect_time()
{
    if (!client) return connecttime;
    if (client->connecttime > connecttime)
        return client->connecttime;
    return connecttime;
}

time_point<steady_clock> connection_state_hb::get_disconnect_time()
{
    if (!client) return disconnecttime;
    if (client->disconnecttime > disconnecttime)
        return client->disconnecttime;
    return connecttime;
}

time_point<steady_clock> connection_state_hb::get_last_packet_time()
{
    if (!client) return lastpackettime;
    if (client->lastpackettime > lastpackettime)
        return client->lastpackettime;
    return lastpackettime;
}

time_point<steady_clock> connection_state_hb::get_last_check_time()
{
    if (!client) return lastchecktime;
    if (client->lastchecktime > lastchecktime)
        return client->lastchecktime;
    return lastchecktime;
}

time_point<steady_clock> connection_state_hb::get_login_time()
{
    if (!client) return logintime;
    if (client->logintime > logintime)
        return client->logintime;
    return logintime;
}
