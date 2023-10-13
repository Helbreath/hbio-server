//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <spdlog/fmt/fmt.h>
#include <fstream>
#include "streams.h"
#include "socket_defines.h"

void CGame::on_message(std::shared_ptr<ix::ConnectionState> ixconnstate, ix::WebSocket & websocket, const ix::WebSocketMessagePtr & message)
{
    connection_state_hb * connection_state = reinterpret_cast<connection_state_hb *>(ixconnstate.get());
    if (!connection_state)
    {
        log->critical("sockets.cpp # Connection state is null for unknown connection");
        websocket.close();
        return;
    }
    connection_state->set_last_packet_time(steady_clock::now());

    if (message->type == ix::WebSocketMessageType::Open)
    {
        log->info("Connection opened");
        // initial client object setup on fresh connect

        // track websocket list locally to match websocket to state iteratively
        std::lock_guard<std::mutex> l(websocket_list);
        for (auto & ws : server->getClients())
            if (ws.get() == &websocket)
            {
                websocket_clients.insert(std::make_pair(ws, ixconnstate));
                connection_state->websocket = ws;
                connection_state->set_connect_time(now());
                log->info("Pair added to internal list");
                auto c = client_list.emplace(std::make_shared<CClient>());
                connection_state->client = *c.first;
                (*c.first)->connection_state = ixconnstate;
                (*c.first)->set_connect_time(now());
                (*c.first)->set_last_packet_time(now());

                // todo: fix this client list system
                for (int i = 1; i < DEF_MAXCLIENTS; i++)
                {
                    if (m_pClientList[i] == nullptr)
                    {
                        m_pClientList[i] = c.first->get();
                        connection_state->client_handle = i;
                        bAddClientShortCut(i);
                        (*c.first)->address = ixconnstate->getRemoteIp();
                        m_pClientList[i]->m_dwSPTime = m_pClientList[i]->m_dwMPTime =
                            m_pClientList[i]->m_dwHPTime = m_pClientList[i]->m_dwAutoSaveTime =
                            m_pClientList[i]->m_dwTime = m_pClientList[i]->m_dwHungerTime = m_pClientList[i]->m_dwExpStockTime =
                            m_pClientList[i]->m_dwRecentAttackTime = m_pClientList[i]->m_dwAutoExpTime = m_pClientList[i]->m_dwSpeedHackCheckTime = timeGetTime();

                        log->info("<{}> Client Connected: ({})", i, m_pClientList[i]->address);
                        break;
                    }
                }

                if (connection_state->client_handle == 0)
                {
                    // could not add client to client list
                    log->critical("Could not add client to client list");
                }
                break;
            }
    }
    else if (message->type == ix::WebSocketMessageType::Close)
    {
        log->info("Connection closed");
        std::lock_guard<std::mutex> l(websocket_list);
        for (auto & wspair : websocket_clients)
            if (wspair.first.get() == &websocket)
            {
                websocket_clients.erase(wspair);
                log->info("Pair cleared from internal list");
                connection_state->set_disconnected(true);
                // do logout stuff
                break;
            }
    }
    else if (message->type == ix::WebSocketMessageType::Error)
    {
        log->error("Error on websocket");
        std::lock_guard<std::mutex> l(websocket_list);
        for (auto & wspair : websocket_clients)
            if (wspair.first.get() == &websocket)
            {
                websocket_clients.erase(wspair);
                log->info("Pair cleared from internal list");
                connection_state->set_disconnected(true);
                break;
            }
    }
    else if (message->type == ix::WebSocketMessageType::Message)
    {
        log->info(fmt::format("Message received - {} bytes", message->str.length()));

        std::unique_ptr<stream_read> sr = std::make_unique<stream_read>(message->str.c_str(), static_cast<uint32_t>(message->str.length()));

        socket_message sm{ ixconnstate, connection_state, websocket, std::move(sr) };

        process_binary_message(std::move(sm));

//         if (message->binary)
//             // handle legacy binary packets
//             process_binary_message(std::move(sm));
//         else
//             // handle new json objects
//             process_json_message(std::move(sm));
    }
}
