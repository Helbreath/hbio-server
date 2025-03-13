//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <spdlog/fmt/fmt.h>
#include <fstream>
#include "streams.h"
#include "socket_defines.h"

void CGame::on_message(std::shared_ptr<CClient> player, ix::WebSocket & websocket, const ix::WebSocketMessagePtr & message)
{
    if (message->type == ix::WebSocketMessageType::Open)
    {
        log->info("Connection opened");
        // initial client object setup on fresh connect

        // track websocket list locally to match websocket to state iteratively
        std::lock_guard<std::recursive_mutex> l(websocket_list);
        for (auto & ws : server->getClients())
            if (ws.get() == &websocket)
            {
                websocket_clients.insert(std::make_pair(ws, player));
                player->set_websocket(ws);
                player->set_connect_time(now());
                player->set_last_packet_time(now());
                log->info("Pair added to internal list");
                break;
            }
    }
    else if (message->type == ix::WebSocketMessageType::Close)
    {
        log->info("Connection closed - {}", message->closeInfo.reason);
        std::lock_guard<std::recursive_mutex> l(websocket_list);
        for (auto & wspair : websocket_clients)
            if (wspair.first.get() == &websocket)
            {
                websocket_clients.erase(wspair);
                log->info("Pair cleared from internal list - Closed");
                player->set_connected(false);
                // do logout stuff
                DeleteClient(player->client_handle, true, false, false, false);
                break;
            }
    }
    else if (message->type == ix::WebSocketMessageType::Error)
    {
        log->error("Error on websocket - {}", message->errorInfo.reason);
        std::lock_guard<std::recursive_mutex> l(websocket_list);
        for (auto & wspair : websocket_clients)
            if (wspair.first.get() == &websocket)
            {
                websocket_clients.erase(wspair);
                log->info("Pair cleared from internal list - Error");
                player->set_connected(false);
                DeleteClient(player->client_handle, true, false, false, false);
                break;
            }
    }
    else if (message->type == ix::WebSocketMessageType::Message)
    {
        //log->info(fmt::format("Message received - {} bytes", message->str.length()));

        std::unique_ptr<stream_read> sr = std::make_unique<stream_read>(message->str.c_str(), static_cast<uint32_t>(message->str.length()));

        player->set_last_packet_time(now());

        socket_message sm{ player, websocket, std::move(sr) };

        process_binary_message(std::move(sm));

//         if (message->binary)
//             // handle legacy binary packets
//             process_binary_message(std::move(sm));
//         else
//             // handle new json objects
//             process_json_message(std::move(sm));
    }
}
