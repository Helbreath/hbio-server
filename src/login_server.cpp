//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include "socket_defines.h"
#include "defines.h"

void CGame::handle_login_server_message(socket_message & sm)
{
    try
    {
        stream_write sw{};
        stream_read & sr{ *sm.sr };

        auto & player = sm.player;

//         if (client == nullptr)
//         {
//             log->error("Client is null from {}", sm.connection_state->getRemoteIp());
//             return;
//         }

        int client_handle = player->client_handle;

        uint32_t msgid = sr.read_uint32();
        sr.read_uint16();

        log->info(std::format("Login Packet [{:X}]", msgid));

        static auto check_login = [&](CClient * client) -> bool
            {
                if (client == nullptr)
                {
                    sw.write_uint32(MSGID_RESPONSE_LOG);
                    sw.write_uint16(DEF_LOGRESMSGTYPE_REJECT);
                    sw.write_string("Not logged in");
                    client->write(sw);
                    log->info("Player object does not exist for <{}>", player->getRemoteIp());
                    return false;
                }
                if (!client->logged_in)
                {
                    sw.write_uint32(MSGID_RESPONSE_LOG);
                    sw.write_uint16(DEF_LOGRESMSGTYPE_REJECT);
                    sw.write_string("Not logged in");
                    client->write(sw);
                    log->info("Player trying to send messages prior to login <{}> for account <{}>", player->getRemoteIp(), client->account);
                    //if (client) delete_client_lock(client);
                    if (client) DeleteClient(client_handle, false, false);
                    return false;
                }
                return true;
            };

        static auto check_login_status = [&]() -> bool
            {
                if (get_login_server_state() != login_server_status::running && get_login_server_state() != login_server_status::running_queue)
                {
                    sw.write_uint32(MSGID_RESPONSE_LOG);
                    sw.write_uint16(DEF_LOGRESMSGTYPE_REJECT);
                    sw.write_string("Login server not online");
                    auto data = ix::IXWebSocketSendData{ sw.data, sw.position };
                    sm.websocket.sendBinary(data);
                    return false;
                }
                return true;
            };

        static auto check_game_status = [&]() -> bool
            {
                if (get_game_server_state() != game_server_status::running)
                {
                    sw.write_uint32(MSGID_RESPONSE_LOG);
                    sw.write_uint16(DEF_LOGRESMSGTYPE_REJECT);
                    sw.write_string("Login server not online");
                    auto data = ix::IXWebSocketSendData{ sw.data, sw.position };
                    sm.websocket.sendBinary(data);
                    return false;
                }
                return true;
            };

        int64_t account_id{};

        switch (msgid)
        {
            // login related events
            case MSGID_COMMAND_CHECKCONNECTION:
            {
                break;
            }
            case MSGID_SCREEN_SETTINGS:
                ScreenSettingsHandler(player, sr.data, sr.size);
                break;
            case MSGID_REQUEST_LOGIN:
            {
                if (!check_login_status()) return;

                if (player && player->logged_in)
                {
                    sw.write_uint32(MSGID_RESPONSE_LOG);
                    sw.write_uint16(DEF_LOGRESMSGTYPE_REJECT);
                    sw.write_string("Invalid access");
                    auto data = ix::IXWebSocketSendData{ sw.data, sw.position };
                    log->warn("Player trying to login twice <{}> for account <{}>", player->getRemoteIp(), player->account);
                    sm.websocket.sendBinary(data);
                    return;
                }

                std::string account = sr.read_string();
                std::string password = sr.read_string();
                std::string worldname = sr.read_string();
                if (world_name != worldname)
                {
                    // invalid world name
                    sw.write_uint32(MSGID_RESPONSE_LOG);
                    sw.write_uint16(DEF_LOGRESMSGTYPE_SERVICENOTAVAILABLE);
                    auto data = ix::IXWebSocketSendData{ sw.data, sw.position };
                    sm.websocket.sendBinary(data);
                    return;
                }

                // pass account name and password and get back account_id if successful
                uint64_t account_id{};
                try
                {
                    account_id = check_account_auth(player, account, password);
                }
                catch (std::exception & ex)
                {
                    // login failed
                    log->info("Failed login from <{}> for account <{}> - <{}>", player->getRemoteIp(), account, ex.what());
                    // todo - add login spam protection

                    sw.write_uint32(MSGID_RESPONSE_LOG);
                    sw.write_uint16(DEF_LOGRESMSGTYPE_PASSWORDMISMATCH);
                    auto data = ix::IXWebSocketSendData{ sw.data, sw.position };
                    sm.websocket.sendBinary(data);
                    return;
                }

                player->account_id = account_id;
                player->account = account;
                std::lock_guard<std::recursive_mutex> lock(client_list_mtx);
                player->address = player->getRemoteIp();
                player->set_connect_time(now());
                player->set_last_packet_time(now());

                client_list.insert(player);

                // todo: fix this client list system
                for (int i = 1; i < DEF_MAXCLIENTS; i++)
                {
                    if (m_pClientList[i] == nullptr)
                    {
                        m_pClientList[i] = player.get();
                        player->client_handle = i;
                        bAddClientShortCut(i);
                        m_pClientList[i]->auto_save_time = now();
                        m_pClientList[i]->m_dwSPTime = m_pClientList[i]->m_dwMPTime =
                            m_pClientList[i]->m_dwHPTime =
                            m_pClientList[i]->m_dwTime = m_pClientList[i]->m_dwHungerTime = m_pClientList[i]->m_dwExpStockTime =
                            m_pClientList[i]->m_dwRecentAttackTime = m_pClientList[i]->m_dwAutoExpTime = m_pClientList[i]->m_dwSpeedHackCheckTime = timeGetTime();

                        log->info("<{}> Client logged in: ({})", i, m_pClientList[i]->address);
                        break;
                    }
                }

                player->logged_in = true;
                player->currentstatus = client_status::login_screen;

                sw.write_uint32(MSGID_RESPONSE_LOG);
                sw.write_int16(DEF_MSGTYPE_CONFIRM);
                sw.write_int16(UPPER_VERSION);
                sw.write_int16(LOWER_VERSION);
                sw.write_int16(PATCH_VERSION);
                sw.write_byte(0x01);
                sw.write_int16(0);//dates \/
                sw.write_int16(0);
                sw.write_int16(0);
                sw.write_int16(0);
                sw.write_int16(0);
                sw.write_int16(0);//dates /\

                build_character_list(player.get(), sw);
                sw.write_int32(500);
                sw.write_int32(500);
                sw.write_string("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32);
                player->write(sw);
            }
            break;
            case MSGID_REQUEST_CREATENEWCHARACTER:
                if (!check_login_status() || !check_login(player.get())) return;
                create_character(player.get(), sr);
                break;
            case MSGID_REQUEST_DELETECHARACTER:
                if (!check_login_status() || !check_login(player.get())) return;
                delete_character(player.get(), sr);
                break;
            case MSGID_REQUEST_ENTERGAME:
                if (!check_login_status() || !check_login(player.get())) return;
                enter_game(player.get(), sr);
                break;
            default:
                log->error("Unknown packet received from client - {:X}", msgid);
                break;
        }
    }
    catch (int32_t reason)
    {
        if (reason == -192)
        {
            log->error("(data == 0)");
        }
        else if (reason == -193)
        {
            log->error("LServer: (position+a > size)");
        }
    }
}
