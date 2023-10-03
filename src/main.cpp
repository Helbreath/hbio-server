//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#if !defined(WIN32)
#include <signal.h>
#endif

char G_cTxt[512];
char G_cData50000[50000];

int main(int argc, char * argv[])
{
    srand((uint32_t)time(nullptr));

#if defined(WIN32)
    ix::initNetSystem();
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    CGame * game = new CGame();
    game->run();

//     core core_{};
// 
//     core_.run();

#if defined(WIN32)
    ix::uninitNetSystem();
#endif

    return 0;
}
