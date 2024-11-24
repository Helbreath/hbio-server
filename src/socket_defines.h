//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <memory>
#include "streams.h"
#include <ixwebsocket/IXWebSocket.h>

class CClient;

struct socket_message
{
    std::shared_ptr<CClient> player;
    ix::WebSocket & websocket;
    std::unique_ptr<stream_read> sr;
};
