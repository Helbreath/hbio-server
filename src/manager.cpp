//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "map.h"

using json = nlohmann::json;
using namespace std::chrono;
using namespace std::chrono_literals;

void CGame::process_manager_message(std::shared_ptr<manager_client> manager, ix::WebSocket & websocket, const ix::WebSocketMessagePtr & message)
{
    json j;
    try
    {
        //parse message as json
        j = json::parse(message->str);

        if (j["type"].is_null())
        {
            log->error("No type in message");
            return;
        }

        // handle initial authentication - disconnect on any other message if not authenticated
        if (manager->authenticated == false)
        {
            if (j.is_object() && j["password"].is_string())
            {
                std::string password = j["password"].get<std::string>();

                if (password != manager_password)
                {
                    log->error("Manager failed to authenticate - {}", manager->address);
                    websocket.close(ix::WebSocketCloseConstants::kNormalClosureCode, "Failed to authenticate");
                    return;
                }

                log->info("Manager authenticated - {}", manager->address);
                websocket.sendText("{\"type\":\"authenticated\"}");
                manager->authenticated = true;
                return;
            }
            else
            {
                log->error("Manager failed to authenticate - {}", manager->address);
                websocket.close(ix::WebSocketCloseConstants::kNormalClosureCode, "Failed to authenticate");
                return;
            }
        }

        // manager is authenticated, process message

        std::string type = j["type"].get<std::string>();

        if (type == "start")
        {
            manager->active = true;
            return;
        }
        if (type == "clients")
        {
            json j2;
            j2["type"] = "clients";
            j2["clients"] = json::array();
            for (auto & [ws, player] : websocket_clients)
            {
                json j3;
                j3["id"] = player->id;
                j3["account"] = player->account;
                j3["address"] = player->address;
                j3["character_status"] = player->character_status;
                j3["character_name"] = player->m_cCharName;
                if (strlen(player->m_cMapName) != 0) j3["map_name"] = player->m_cMapName;
                else j3["map_name"] = "NONE";
                j3["last_packet"] = std::chrono::duration_cast<std::chrono::seconds>(now() - player->get_last_packet_time()).count();
                j3["connect_time"] = std::chrono::duration_cast<std::chrono::seconds>(now() - player->get_connect_time()).count();
                j3["connection_status"] = player->client_status;
                j2["clients"].push_back(j3);
            }

            j2["count"] = websocket_clients.size();
            websocket.sendText(j2.dump());
        }
        else if (type == "maps")
        {
            json j2;
            j2["type"] = "maps";
            j2["maps"] = json::array();
            int i = 0;
            for (auto map_ : this->m_pMapList)
            {
                if (!map_) continue;
                ++i;
                json j3;
                j3["id"] = (int)map_->m_cMapIndex;
                j3["name"] = std::string(map_->m_cName);
                j3["location_name"] = std::string(map_->m_cLocationName);
                j3["random_mob_generator_level"] = (int)map_->m_cRandomMobGeneratorLevel;
                j3["type"] = (int)map_->m_cType;
                j3["current_fish_count"] = map_->m_iCurFish;
                j3["current_mineral_count"] = map_->m_iCurMineral;

                int count = 0;
                for (int a = 0; a < DEF_MAXCLIENTS; ++a)
                {
                    if (m_pClientList[a] && m_pClientList[a]->m_cMapIndex == map_->m_cMapIndex) count++;
                }

                j3["current_player_count"] = count;

                count = 0;
                for (int a = 0; a < DEF_MAXNPCS; ++a)
                {
                    if (m_pNpcList[a] && m_pNpcList[a]->m_cMapIndex == map_->m_cMapIndex) count++;
                }
                j3["current_mob_count"] = count;
                j2["maps"].push_back(j3);
            }

            j2["count"] = i;
            websocket.sendText(j2.dump());
        }
        else if (type == "kick")
        {
            if (j["account"].is_null())
            {
                log->error("No account in kick message");
                return;
            }
            std::string account = j["account"].get<std::string>();
            for (auto & [ws, player] : websocket_clients)
            {
                if (player->account == account)
                {
                    // disconnect player
                    break;
                }
            }
        }
        else if (type == "kill_all_npcs")
        {

        }
        else if (type == "kill_all_players")
        {

        }
        else if (type == "start_event")
        {

        }
        else if (type == "stop_event")
        {

        }
        else if (type == "client_info")
        {

        }
        else if (type == "shutdown")
        {
            server_stop();
        }
        else if (type == "reload")
        {
            load_config();
        }
        else
        {
            log->error("Unknown type in message");
        }
    }
    catch (json::parse_error & ex)
    {
        log->error("Error parsing manager message - {}", ex.what());
        return;
    }

    if (j["type"].is_null())
    {
        log->error("No type in message");
        return;
    }
}
