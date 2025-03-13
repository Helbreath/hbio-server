//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "socket_defines.h"

void CGame::process_binary_message(socket_message sm)
{
    PutMsgQueue(std::make_unique<socket_message>(std::move(sm)));
}
