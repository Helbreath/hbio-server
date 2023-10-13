//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <memory>
#include "streams.h"
#include "connection_state_hb.h"

struct socket_message
{
    std::shared_ptr<ix::ConnectionState> ixconnstate;
    connection_state_hb * connection_state;
    ix::WebSocket & websocket;
    std::unique_ptr<stream_read> sr;
};
