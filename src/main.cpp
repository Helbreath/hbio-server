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
    try
    {
        game->run();
    }
    catch (const std::exception & e)
    {
        game->log->critical("Unhandled exception: {}", e.what());
    }
    catch (...)
    {
        game->log->critical("Unhandled exception: unknown");
    }

#if defined(WIN32)
    ix::uninitNetSystem();
#endif

    return 0;
}
