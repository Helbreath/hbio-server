//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

// todo - turn into its own request packet instead of part of login
void CGame::build_character_list(CClient * client, stream_write & sw)
{
    try
    {
        std::shared_lock<std::shared_mutex> l(login_sql_mtx);
        pqxx::work txn{ *pq_login };
        pqxx::result r{ txn.exec_params("SELECT * FROM characters WHERE account_id=$1;", client->account_id) };
        txn.commit();

        sw.write_byte((uint8_t)r.size());
        if (!r.empty())
        {
            for (pqxx::row row : r)
            {
                sw.write_string(row["name"].as<std::string>(), 10);//char name
                sw.write_byte(0x01); // 0x00 == empty npc + 39 bytes
                //sw.write_uint32(row["id"].as<int64_t>());
                sw.write_uint16(row["appr1"].as<int16_t>());//appr1
                sw.write_uint16(row["appr2"].as<int16_t>());//appr2
                sw.write_uint16(row["appr3"].as<int16_t>());//appr3
                sw.write_uint16(row["appr4"].as<int16_t>());//appr4
//                 sw.write_uint16(row["head_appr"].as<int16_t>());//headappr
//                 sw.write_uint16(row["body_appr"].as<int16_t>());//bodyappr
//                 sw.write_uint16(row["arm_appr"].as<int16_t>());//armappr
//                 sw.write_uint16(row["leg_appr"].as<int16_t>());//legappr
                sw.write_int16(row["gender"].as<int16_t>());//gender
                sw.write_int16(row["skin"].as<int16_t>());//skin
                sw.write_int16(row["level"].as<int16_t>());//level
                sw.write_int32(row["experience"].as<int32_t>());//exp
                sw.write_int16(row["strength"].as<int16_t>());//strength
                sw.write_int16(row["vitality"].as<int16_t>());//vitality
                sw.write_int16(row["dexterity"].as<int16_t>());//dexterity
                sw.write_int16(row["intelligence"].as<int16_t>());//intelligence
                sw.write_int16(row["magic"].as<int16_t>());//magic
                sw.write_int16(row["charisma"].as<int16_t>());//agility/charisma
                sw.write_uint32(row["apprcolor"].as<int32_t>());//apprcolor
                sw.write_int16(0);//unused data
                sw.write_int16(0);//unused data
                sw.write_int16(0);//unused data
                sw.write_int16(0);//unused data
                sw.write_int16(0);//unused data
                sw.write_string(row["maploc"].as<std::string>(), 10);//char name
            }
        }
    }
    catch (std::exception & ex)
    {
        log->critical(fmt::format("Error querying login attempt for account [{}] - {}", client->account, ex.what()));
    }
}

void CGame::create_character(CClient * client, stream_read & sr)
{
    stream_write sw;

    try
    {
        character_db character;

        character.account_id = client->account_id;
        character.name = sr.read_string(10);
        character.world_name = sr.read_string(30);

        if (world_name != character.world_name)
        {
            sw.write_uint32(MSGID_RESPONSE_LOG);
            sw.write_uint16(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED);
            client->write(sw);
            return;
        }

        std::shared_lock<std::shared_mutex> l(game_sql_mtx);
        pqxx::work txn{ *pq_game };

        {
            pqxx::row r{
                txn.exec_prepared1("check_character_count_by_account_id_wn", character.account_id, pq_game->quote(character.world_name))
            };
            auto [charcount] = r.as<uint16_t>();

            if (charcount > 3)
            {
                sw.write_uint32(MSGID_RESPONSE_LOG);
                sw.write_uint16(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED);
                client->write(sw);
                return;
            }
        }

        {
            pqxx::row r{
                txn.exec_prepared1("check_character_count_by_name_wn", pq_game->quote(character.name), pq_game->quote(character.world_name))
            };
            auto [charcount] = r.as<uint16_t>();

            if (charcount > 3)
            {
                sw.write_uint32(MSGID_RESPONSE_LOG);
                sw.write_uint16(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED);
                client->write(sw);
                return;
            }
        }

        character.gender = sr.read_byte();
        character.skin = sr.read_byte();
        character.hairstyle = sr.read_byte();
        character.haircolor = sr.read_byte();
        character.underwear = sr.read_byte();

        character.strength = sr.read_byte();
        character.vitality = sr.read_byte();
        character.dexterity = sr.read_byte();
        character.intelligence = sr.read_byte();
        character.magic = sr.read_byte();
        character.charisma = sr.read_byte();

        if (
            ((character.strength + character.vitality + character.dexterity + character.intelligence + character.magic + character.charisma) != 70)
            || character.strength < 10 || character.strength > 14
            || character.vitality < 10 || character.vitality > 14
            || character.dexterity < 10 || character.dexterity > 14
            || character.intelligence < 10 || character.intelligence > 14 ||
            character.magic < 10 || character.magic > 14 ||
            character.charisma < 10 || character.charisma > 14
            )
        {
            sw.write_uint32(MSGID_RESPONSE_LOG);
            sw.write_uint16(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED);
            client->write(sw);
            return;
        }
        character.appr1 = (character.hairstyle << 8) | (character.haircolor << 4) | character.underwear;
        character.hp = (character.vitality * 8) + (character.strength * 2) + (character.intelligence * 2) + 8;
        character.mp = (character.magic * 3) + (character.intelligence * 2) + 2;
        character.sp = character.strength + 17;

        character.id = create_db_character(txn, character);

        for (uint16_t s = 0; s < 24; ++s)//24 skills total
        {
            if (/*s == 4 ||*/ s == 5 || s == 6 || s == 7 || s == 8 || s == 9 || s == 10 || s == 11 || s == 14 || s == 19 || s == 21)// All attack skills starts at 20%
                txn.exec_params("INSERT INTO skill (char_id, skill_id, mastery, experience) VALUES ($1, $2, 20, 0)", character.id, s);
            else if (s == 3 || s == 23) // Magic Res / Poison Res starts at 2%
                txn.exec_params("INSERT INTO skill (char_id, skill_id, mastery, experience) VALUES ($1, $2, 2, 0)", character.id, s);
            else// All crafting skills starts at 0% Magic skills starts at 0%
                txn.exec_params("INSERT INTO skill (char_id, skill_id, mastery, experience) VALUES ($1, $2, 0, 0)", character.id, s);
        }

        sw.write_uint32(MSGID_RESPONSE_LOG);
        sw.write_uint16(DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED);
        sw.write_string(character.name, 10);
        build_character_list(client, sw);
        client->write(sw);
    }
    catch (std::exception & ex)
    {
        log->critical(fmt::format("Error creating character for account [{}] - {}", client->account, ex.what()));
        sw.write_uint32(MSGID_RESPONSE_LOG);
        sw.write_uint16(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED);
        client->write(sw);
        //delete_client_lock(client, true);
    }
}

void CGame::delete_character(CClient * client, stream_read & sr)
{
    stream_write sw;
    try
    {
        character_db character;

        character.account_id = client->account_id;
        character.name = sr.read_string(10);
        character.world_name = sr.read_string(30);

        if (world_name != character.world_name)
        {
            sw.write_uint32(MSGID_RESPONSE_LOG);
            sw.write_uint16(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED);
            client->write(sw);
            return;
        }

        std::shared_lock<std::shared_mutex> l(game_sql_mtx);
        pqxx::work txn{ *pq_game };

        pqxx::row r{
            txn.exec_prepared1("get_character_by_name_wn", pq_game->quote(character.name), pq_game->quote(character.world_name))
        };
        auto charid = r["id"].as<uint32_t>();
        if (charid == 0)
        {
            sw.write_uint32(MSGID_RESPONSE_LOG);
            sw.write_uint16(DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER);
            client->write(sw);
            return;
        }

        if (is_account_in_use(client->account_id))
        {
            sw.write_uint32(MSGID_RESPONSE_LOG);
            sw.write_uint16(DEF_ENTERGAMERESTYPE_PLAYING);
            client->write(sw);
            return;
        }
        else
        {
            delete_db_character(txn, character);
            sw.write_uint32(MSGID_RESPONSE_LOG);
            sw.write_uint16(DEF_LOGRESMSGTYPE_CHARACTERDELETED);
            sw.write_byte(0x01);
            build_character_list(client, sw);
            client->write(sw);
            return;
        }
    }
    catch (std::exception & ex)
    {
        log->critical(fmt::format("Error deleting character for account [{}] - {}", client->account, ex.what()));
        sw.write_uint32(MSGID_RESPONSE_LOG);
        sw.write_uint16(DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER);
        client->write(sw);
        //delete_client_lock(client, true);
    }
}

bool CGame::is_account_in_use(int64_t account_id)
{
    for (auto & client : client_list)
    {
        if (client->account_id == account_id && client->currentstatus == client_status::in_game)
            return true;
    }
    return false;
}

void CGame::enter_game(CClient * client, stream_read & sr)
{
    stream_write sw;

    try
    {
        if (server_status_ != server_status::running)
        {
            sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
            sw.write_uint16(DEF_LOGRESMSGTYPE_REJECT);
            sw.write_string("Server not online");
            sw.write_int32(0);
            sw.write_int32(0);
            sw.write_int32(0);
            sw.write_int32(0);
            sw.write_byte(5);
            client->write(sw);
            return;
        }

        sr.reset_pos();

        sr.read_uint32();

        std::string character_name = sr.read_string();

        uint8_t charcount = 0;

        uint64_t charid = 0;
        std::string mapname;

        try
        {
            std::shared_lock<std::shared_mutex> l(login_sql_mtx);
            pqxx::work txn{ *pq_login };
            pqxx::row r{
                txn.exec_params1("SELECT * FROM characters WHERE account_id=$1 AND name=$2 LIMIT 1", client->account_id, pq_login->quote(character_name))
            };
            txn.commit();

            charid = r["id"].as<uint64_t>();
            mapname = r["maploc"].as<std::string>();
        }
        catch (pqxx::unexpected_rows &)
        {
            // character doesn't exist
            sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
            sw.write_uint16(DEF_ENTERGAMERESTYPE_REJECT);
            sw.write_string("Character does not exist");
            sw.write_int32(0);
            sw.write_int32(0);
            sw.write_int32(0);
            sw.write_byte(4);
            client->write(sw);
            return;
        }
        //server is up

        //check if account or char already logged in

        //there is a far better way to accomplish this. use this for now, but make it better later? or keep?
        //can keep track of logged in status via db, will add that eventually anyway
        //login can keep its own list of matching data to not have to lock the gameserver list - better a slow login internal query than slow game thread

        std::shared_ptr<CClient> clientfound;

        bool accountfound = false;
        {
            std::lock_guard<std::recursive_mutex> lock(client_list_mtx);
            for (const std::shared_ptr<CClient> & clnt : client_list)
            {
                // this will have a hit for itself, so check for map being set which only a character that has made it in game will have
                // that way it continues iterating to find any other instance of the account connected
                // this also lets the account have multiple connections to the character select screen as a side-effect
                // todo - allow this?
                if (clnt->account_id == client->account_id && clnt->m_cMapIndex > 0)
                {
                    //BUG: potential bug point if charID == 0 - only occurs on unsuccessful login
                    // ^ is this still true?
                    //account found
                    accountfound = true;
                    if (clnt->id == charid)
                    {
                        //exact char found
                        clientfound = clnt;
                        break;
                    }
                }
            }
        }

        // set the map for the client
        for (auto & map : m_pMapList)
            if (std::string(map->m_cName) == mapname)
                client->m_cMapIndex = map->m_cMapIndex;

        if (clientfound && clientfound->get_disconnected())
        {
            //client has recently disconnected
            //at this point, can do some fancy swapping of socket and place this current active socket
            // into the old client object letting them retake control of the disconnected session
            // - to do that though, it'd need to resend initial data - need a special case in GServer
            // - to not do the usual map data stuff when init data is sent or risk duplicating client
            // - objects in mapdata - also needs a case in GServer to not reject another initdata
            // - from an already connected client (used to be the cause of item duping on Int'l)
            // - and will need code in place to handle this specific case
            auto connection_state = clientfound->get_connection_state();
            auto websocket = clientfound->get_websocket();
            if (websocket && websocket->getReadyState() == ix::ReadyState::Open)
            {
                //false alarm of some sort (or other issue) should probably breakpoint this or have some sort
                //of check if client even still exists. socket should never be open if disconnected was set
                //__debugbreak();
            }
            else
            {
                clientfound->set_disconnect_time(std::chrono::steady_clock::time_point());
                clientfound->connection_state = client->connection_state;
                auto connstate = clientfound->get_connection_state();
                if (connstate)
                {
                    connstate->client = clientfound;
                }
                else
                {
                    // error .. connection state should exist
                    log->critical(fmt::format("Connection state invalid when resuming client connection - account: [{}]", clientfound->account));
                }
                client->connection_state.reset();

                //since nothing is technically associated with this client, just remove it from the list to force it to delete

                std::lock_guard<std::recursive_mutex> lock(client_list_mtx);
                client_list.erase(client->shared_from_this());

                if (clientfound->m_cMapIndex != 0)
                {
                    clientfound->currentstatus = client_status::in_game;
                    strcpy(clientfound->m_cCharName, character_name.c_str());
                    //pgs->clientlist.push_back(client);
                    //clientlist.remove(client);

                    sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
                    sw.write_uint16(DEF_ENTERGAMERESTYPE_REJECT);
                    sw.write_string("Character does not exist");
                    sw.write_int32(0);
                    sw.write_int32(0);
                    sw.write_int32(0);
                    sw.write_byte(4);
                    client->write(sw);

                    sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
                    sw.write_uint16(DEF_ENTERGAMERESTYPE_CONFIRM);
                    sw.write_string(std::string("127.0.0.1"), 16);
                    sw.write_uint16(2848);
                    sw.write_string(world_name, 20);
                    clientfound->write(sw);
                }
                else
                {
                    sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
                    sw.write_uint16(DEF_ENTERGAMERESTYPE_REJECT);
                    sw.write_string("Map does not exist");
                    sw.write_int32(0);
                    sw.write_int32(0);
                    sw.write_int32(0);
                    sw.write_byte(5);
                    clientfound->write(sw);
                }
            }
        }
        else if (accountfound)
        {
            //account already logged in and not resuming object
            sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
            sw.write_uint16(DEF_ENTERGAMERESTYPE_PLAYING);
            client->write(sw);
        }
        else
        {
            if (client->m_cMapIndex != 0)
            {
                client->currentstatus = client_status::in_game;
                strcpy(client->m_cCharName, character_name.c_str());
                client->id = charid;

                sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
                sw.write_uint16(DEF_ENTERGAMERESTYPE_CONFIRM);
                sw.write_string(std::string("127.0.0.1"), 16);
                sw.write_uint16(2848);
                sw.write_string(world_name, 20);
                client->write(sw);

//                 m_pMapList[client->m_cMapIndex]->clientlist.push_back(client);
//                 client->map->enter_map(client);
            }
            else
            {
                sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
                sw.write_uint16(DEF_ENTERGAMERESTYPE_REJECT);
                sw.write_string("Map does not exist");
                sw.write_int32(0);
                sw.write_int32(0);
                sw.write_int32(0);
                sw.write_byte(3);
                client->write(sw);
            }
        }
    }
    catch (std::exception & ex)
    {
        log->critical(std::format("EnterGame() Exception: %s", std::string(ex.what())));
        sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
        sw.write_uint16(DEF_ENTERGAMERESTYPE_REJECT);
        sw.write_string("Unknown");
        sw.write_int32(0);
        sw.write_int32(0);
        sw.write_int32(0);
        sw.write_byte(3);
        client->write(sw);
    }
}
