//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#if !defined(WIN32)
#include <csignal>
#endif

char G_cTxt[512];
char G_cData50000[50000];

CGame * game;

#if !defined(WIN32)
static void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        std::lock_guard<std::mutex> lock(server->cv_mtx);
        server->set_server_state(server_status_t::SHUTDOWN);
        server->cv_exit.notify_one();
    }
}
#endif

int main(int argc, char * argv[])
{
    srand((uint32_t)time(nullptr));

#if defined(WIN32)
    ix::initNetSystem();
#else
    struct sigaction sa {};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
#endif

    game = new CGame();

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
