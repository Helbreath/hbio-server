//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "map.h"
#include "time_utils.h"

using json = nlohmann::json;
using namespace std::chrono;
using namespace std::chrono_literals;

void CGame::start_websocket()
{
    server = std::make_unique<ix::WebSocketServer>(bindport, bindip);

    server->setConnectionStateFactory([&]()
        {
            log->info("Client connection state factory called");
            return std::make_shared<CClient>();
        });

    server->setOnClientMessageCallback(
        [&](std::shared_ptr<ix::ConnectionState> connection_state, ix::WebSocket & websocket, const ix::WebSocketMessagePtr & message)
        {
            log->info("Message received - {} bytes", message->str.length());
            on_message((reinterpret_cast<CClient *>(connection_state.get()))->shared_from_this(), websocket, message);
        }
    );

    auto res = server->listen();
    if (!res.first)
        throw std::runtime_error(res.second);

    log->info(std::format("Listening on {}:{}", bindip, bindport));

    server->start();
}

void CGame::start_manager_websocket()
{
    manager_server = std::make_unique<ix::WebSocketServer>(manager_bindport, manager_bindip);

    manager_server->setConnectionStateFactory([&]()
        {
            log->info("Manager client connection state factory called");
            return std::make_shared<manager_client>();
        });

    manager_server->setOnClientMessageCallback(
        [&](std::shared_ptr<ix::ConnectionState> connection_state, ix::WebSocket & websocket, const ix::WebSocketMessagePtr & message)
        {
            on_manager_message((reinterpret_cast<manager_client *>(connection_state.get()))->shared_from_this(), websocket, message);
        }
    );

    auto res = manager_server->listen();
    if (!res.first)
        throw std::runtime_error(res.second);

    log->info(std::format("Manager Listening on {}:{}", manager_bindip, manager_bindport));

    manager_server->start();
}

void CGame::server_stop()
{
    set_server_state(server_status_t::SHUTDOWN);
    cv_exit.notify_one();
}

void CGame::run()
{
    if (!state_valid)
    {
        std::cout << "Server state invalid. Errors occurred on creation. Exiting core::run()\n";
        return;
    }

    try
    {
        std::unique_lock<std::shared_mutex> l(game_sql_mtx, std::defer_lock);
        std::unique_lock<std::shared_mutex> l2(login_sql_mtx, std::defer_lock);
        std::lock(l, l2);
        pq_login = std::make_unique<pqxx::connection>(std::format("postgresql://{}:{}@{}:{}/{}", login_sqluser, login_sqlpass, login_sqlhost, login_sqlport, login_sqldb));
        pq_game = std::make_unique<pqxx::connection>(std::format("postgresql://{}:{}@{}:{}/{}", game_sqluser, game_sqlpass, game_sqlhost, game_sqlport, game_sqldb));
    }
    catch (pqxx::broken_connection & ex)
    {
        throw std::runtime_error(std::format("Unable to connect to SQL - {}", ex.what()));
    }

    try
    {
        prepare_login_statements();
        prepare_game_statements();
    }
    catch (std::exception & ex)
    {
        throw std::runtime_error(std::format("Unable to prepare SQL statements - {}", ex.what()));
    }

    log->info("Connected to SQL");

    if (bInit() == false)
    {
        log->critical("Server failed to start");
        return;
    }

    load_configs();

    set_server_state(server_status_t::RUNNING);

    // todo - make a way to change these externally - maybe by admin client?
    set_login_server_state(server_status_t::RUNNING);
    set_game_server_state(server_status_t::RUNNING);

    start_websocket();
    if (manager_enabled) start_manager_websocket();

    log->info("Server started");

    OnStartGameSignal();

    auto timer_mgr = timers.get();

    int32_t on_timer_error_count = 0;

    timer_mgr->add_recurring_timer(10ms, [&](time_point<system_clock> current_time)
        {
            try
            {
                OnTimer();
            }
            catch (std::exception & ex)
            {
                log->error(ex.what());

                if (++on_timer_error_count > 100)
                {
                    log->error("on_timer error count > 100 - exiting thread");
                    return;
                }
            }
        });

    timer_mgr->add_recurring_timer(1ms, [&](time_point<system_clock> current_time)
        {
            // check the client list every 10ms for timed out users - if this becomes a performance issue (unlikely) then change to a regular for
            // so that it can do a full set iteration at once rather than break out of it when a single change is made. unfortunately ranged-for has
            // iteration limitations
            std::unique_lock<std::recursive_mutex> l(websocket_list, std::defer_lock);
            std::unique_lock<std::recursive_mutex> l2(client_list_mtx, std::defer_lock);
            std::lock(l, l2);
            // why can't anyone make a good ws library
            auto wsclients = server->getClients();
            for (auto & wspair : websocket_clients)
            {
                auto & ws = wspair.first;
                auto & connstate = wspair.second;
                if (wspair.second == nullptr)
                {
                    continue;
                }
                auto & player = wspair.second;

                if (!player)
                {
                    if (time_count(current_time - player->get_connect_time()) > 5)
                    {
                        log->info(std::format("Disconnecting socket - no client - timed out 20s [{}] account [{}]", player->getId(), player->getRemoteIp()));
                        ws->close();
                        websocket_clients.erase(wspair);
                    }
                    break;
                }

                if (time_count(current_time - player->get_last_check_time()) < 5) continue;
                if (std::chrono::duration_cast<seconds>(current_time - player->get_last_packet_time()).count() > 20)
                {
                    log->info(std::format("Disconnecting player packet timed out 20s [{}] account [{}]", player->m_cCharName, player->account));
                    delete_client_nolock(player, true, true);
                    websocket_clients.erase(wspair);
                    break;
                }
                if (!player->get_connected() && std::chrono::duration_cast<seconds>(current_time - player->get_disconnect_time()).count() > 5)
                {
                    log->info(std::format("Disconnecting player disconnected timed out 10s [{}] account [{}]", player->m_cCharName, player->account));
                    delete_client_nolock(player, true, true);
                    websocket_clients.erase(wspair);
                    break;
                }
            }
            for (auto & player : client_list)
            {
                if (std::chrono::duration_cast<seconds>(current_time - player->get_last_packet_time()).count() > 20)
                {
                    log->info(std::format("Disconnecting player packet timed out 20s [{}] account [{}] - [{}]", player->m_cCharName, player->account, player->address));
                    delete_client_nolock(player, true, true);
                    break;
                }

                if (!player->logged_in)
                {
                    delete_client_nolock(player, true, true);
                    break;
                }

                if (time_count(current_time - player->get_last_check_time()) < 5) continue;

                if (!player->get_connected() && std::chrono::duration_cast<seconds>(current_time - player->get_disconnect_time()).count() > 10)
                {
                    log->info(std::format("Disconnecting player disconnected timed out 10s [{}] account [{}]", player->m_cCharName, player->account));
                    delete_client_nolock(player, true, true);
                    break;
                }
            }
        });

    timer_mgr->add_recurring_timer(20s, [&](time_point<system_clock> current_time)
        {
            std::vector<character_db> char_data;
            for (auto & player : client_list)
            {
                if (player->m_bInitComplete)
                {
                    char_data.push_back(build_character_data_for_save(player));
                    player->auto_save_time = now();
                }
            }

            auto_save(char_data);
        });

    std::unique_lock<std::mutex> lock(cv_mtx);
    cv_exit.wait(lock, [&]
        {
            return server_status_ != server_status_t::RUNNING && game_status_ != server_status_t::RUNNING && login_status_ != server_status_t::RUNNING;
        });

    Quit();
}

void CGame::auto_save(std::vector<character_db> char_data)
{
    std::unique_lock<std::shared_mutex> l(game_sql_mtx);
    pqxx::work txn{ *pq_game };

    try
    {
        for (auto & character : char_data)
        {
            update_db_character(txn, character);
            for (auto & skill : character.skills)
            {
                if (skill.id == 0)
                    skill.id = create_db_skill(txn, skill);
                else
                    update_db_skill(txn, skill);
            }
            for (auto & item : character.items)
            {
                if (item.id == 0)
                    item.id = create_db_item(txn, item);
                else
                    update_db_item(txn, item);
            }
            for (auto & item : character.bank_items)
            {
                if (item.id == 0)
                    item.id = create_db_item(txn, item);
                else
                    update_db_item(txn, item);
            }
        }
        txn.commit();
    }
    catch (std::exception & e)
    {
        // save all or none - store a backup old hb style (text files) in event of failure for inspection?
        log->error("Error saving character data - {}", e.what());
    }
}

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

        {
            std::shared_lock<std::shared_mutex> l(game_sql_mtx);
            pqxx::work txn{ *pq_game };

            {
                pqxx::row r{
                    txn.exec_prepared1("check_character_count_by_account_id_wn", character.account_id, character.world_name)
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
                    txn.exec_prepared1("check_character_count_by_name_wn", character.name, character.world_name)
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
        delete_client_lock(client->shared_from_this(), true);
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
            txn.exec_prepared1("get_character_by_name_wn", character.name, character.world_name)
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
            delete_db_skills(txn, charid);
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
        delete_client_lock(client->shared_from_this(), true);
    }
}

bool CGame::is_account_in_use(int64_t account_id)
{
    for (auto & client : client_list)
    {
        if (client->account_id == account_id && client->client_status == client_status_t::PLAYING)
            return true;
    }
    return false;
}

void CGame::enter_game(CClient * client, stream_read & sr)
{
    stream_write sw;

    try
    {
        if (server_status_ != server_status_t::RUNNING)
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

        try
        {
            std::shared_lock<std::shared_mutex> l(game_sql_mtx);
            pqxx::work txn{ *pq_game};
            pqxx::row row{
                txn.exec_params1("SELECT * FROM characters WHERE account_id=$1 AND name=$2 LIMIT 1", client->account_id, character_name)
            };

            client->id = row["id"].as<uint64_t>();

            std::vector<item_db> items{ get_db_items(txn, client->id) };
            std::vector<skill_db> skills{ get_db_skills(txn, client->id) };
            txn.commit();


            client->m_sX = row["locx"].as<int16_t>();
            client->m_sY = row["locy"].as<int16_t>();

            client->m_cSex = (uint8_t)row["gender"].as<uint16_t>();
            client->m_cSkin = (uint8_t)row["skin"].as<uint16_t>();
            client->m_cHairStyle = (uint8_t)row["hairstyle"].as<uint16_t>();
            client->m_cHairColor = (uint8_t)row["haircolor"].as<uint16_t>();
            client->m_cUnderwear = (uint8_t)row["underwear"].as<uint16_t>();
            std::string temp = row["magicmastery"].as<std::string>();
            for (int i = 0; i < temp.length(); ++i)
            {
                client->m_cMagicMastery[i] = (temp[i] == '1') ? 1 : 0;
            }
            memset(client->m_cLocation, 0, sizeof(client->m_cLocation));
            memcpy(client->m_cLocation, row["nation"].as<std::string>().c_str(), 10);
            if (strcmp(client->m_cLocation, "Aresden") == 0)
                client->m_cSide = Side::ARESDEN;
            else if (strcmp(client->m_cLocation, "Elvine") == 0)
                client->m_cSide = Side::ELVINE;
            else
                client->m_cSide = Side::NEUTRAL;

            memset(client->m_cMapName, 0, sizeof(client->m_cMapName));
            memcpy(client->m_cMapName, row["maploc"].as<std::string>().c_str(), 10);

            client->m_cMapIndex = get_map_index(client->m_cMapName);

            if (!row["lockmapname"].is_null())
            {
                memset(client->m_cLockedMapName, 0, sizeof(client->m_cLockedMapName));
                memcpy(client->m_cLockedMapName, row["lockmapname"].as<std::string>().c_str(), 10);
            }
            client->m_iLockedMapTime = row["lockmaptime"].as<int32_t>();
            memset(client->m_cProfile, 0, sizeof(client->m_cProfile));
            memcpy(client->m_cProfile, row["profile"].as<std::string>().c_str(), 10);
            //client->m_iGuildRank = (int8_t)row["guildrank"].as<int16_t>();
            client->m_iHP = row["hp"].as<int32_t>();
            client->m_iMP = row["mp"].as<int32_t>();
            client->m_iSP = row["sp"].as<int32_t>();
            client->m_iEnemyKillCount = row["ek"].as<int32_t>();
            client->m_iPKCount = row["pk"].as<int32_t>();
            client->m_iLevel = row["level"].as<int32_t>();
            client->m_iExp = row["experience"].as<int32_t>();
            client->m_iStr = row["strength"].as<int32_t>();
            client->m_iVit = row["vitality"].as<int32_t>();
            client->m_iDex = row["dexterity"].as<int32_t>();
            client->m_iInt = row["intelligence"].as<int32_t>();
            client->m_iMag = row["magic"].as<int32_t>();
            client->m_iCharisma = row["charisma"].as<int32_t>();
            client->m_iLuck = row["luck"].as<int32_t>();
            client->m_iRewardGold = row["rewardgold"].as<int32_t>();
            client->m_iHungerStatus = row["hunger"].as<int32_t>();
            client->m_iAdminUserLevel = row["adminlevel"].as<int32_t>();
            client->m_iTimeLeft_ShutUp = row["leftshutuptime"].as<int32_t>();
            client->m_iTimeLeft_Rating = row["leftreptime"].as<int32_t>();
            client->m_iRating = row["reputation"].as<int32_t>();
            client->m_iGuildGUID = row["guild_id"].as<int32_t>();
            client->m_iDownSkillIndex = row["downskillid"].as<int32_t>();
            client->m_sCharIDnum1 = row["id1"].as<int32_t>();
            client->m_sCharIDnum2 = row["id2"].as<int32_t>();
            client->m_sCharIDnum3 = row["id3"].as<int32_t>();
            client->m_iQuest = row["questnum"].as<int32_t>();
            client->m_iCurQuestCount = row["questcount"].as<int32_t>();
            client->m_iQuestRewardType = row["questrewardtype"].as<int32_t>();
            client->m_iQuestRewardAmount = row["questrewardamount"].as<int32_t>();
            client->m_iContribution = row["contribution"].as<int32_t>();
            client->m_iQuestID = row["questid"].as<int32_t>();
            client->m_bIsQuestCompleted = row["questcompleted"].as<bool>();
            client->m_iTimeLeft_ForceRecall = row["leftforcerecalltime"].as<int32_t>();
            client->m_iTimeLeft_FirmStaminar = row["leftfirmstaminatime"].as<int32_t>();
            client->m_iSpecialEventID = row["eventid"].as<int32_t>();
            client->m_iSuperAttackLeft = row["leftsac"].as<int32_t>();
            client->m_iFightzoneNumber = row["fightnum"].as<int32_t>();
            client->m_iReserveTime = row["fightdate"].as<int32_t>();
            client->m_iFightZoneTicketNumber = row["fightticket"].as<int32_t>();
            client->m_iSpecialAbilityTime = row["leftspectime"].as<int32_t>();
            client->m_iWarContribution = row["warcon"].as<int32_t>();
            client->m_iCrusadeDuty = row["crusadejob"].as<int32_t>();
            client->m_iConstructionPoint = row["crusadeconstructpoint"].as<int32_t>();
            client->m_dwCrusadeGUID = row["crusadeid"].as<int32_t>();
            client->m_iLockedMapTime = row["leftdeadpenaltytime"].as<int32_t>();
            uint64_t partyid = row["party_id"].as<int64_t>();
            // if (partyid && partyMgr.PartyExists(partyid))
            // {
            //     client->SetParty(partyMgr.GetParty(partyid));
            // }
            client->m_iGizonItemUpgradeLeft = row["itemupgradeleft"].as<int32_t>();
            //client->m_elo = row["elo"].as<int32_t>();
            client->m_iEnemyKillCount = row["totalek"].as<int32_t>();
            client->m_iPKCount = row["totalpk"].as<int32_t>();


            if (client->m_cSex == MALE) client->m_sType = 1;
            else if (client->m_cSex == FEMALE) client->m_sType = 4;
            client->m_sType += client->m_cSkin - 1;
            client->m_sAppr1 = (client->m_cHairStyle << 8) | (client->m_cHairColor << 4) | client->m_cUnderwear;

            for (item_db & item : items)
            {
                std::string itemloc = item.itemloc;
                if (itemloc == "bag")
                {
                    uint16_t item_index = add_bag_item(client, item);
                    if (client->m_bIsItemEquipped[item_index] == true)
                    {
                        if (bEquipItemHandler(client, item_index, false) == false)
                            client->m_bIsItemEquipped[item_index] = false;
                    }

                }
                else if (itemloc == "bank")
                {
                    add_bank_item(client, item);
                }
                else if (itemloc == "mail")
                {
                    // todo: do mail items with the mail record itself
                    //add_mail_item(client, item);
                }
            }
            for (skill_db & skill : skills)
            {
                int16_t skill_id = skill.skill_id;
                client->m_cSkillId[skill_id] = skill.skill_id;
                client->m_cSkillMastery[skill_id] = skill.skill_level;
                client->m_iSkillSSN[skill_id] = skill.skill_exp;
            }
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
                if (
                    client != clnt.get()
                    && clnt->account_id == client->account_id
                    && clnt->m_cMapIndex > 0
                    && (clnt->character_status == character_status_t::DEAD || clnt->client_status == client_status_t::PLAYING)
                    )
                {
                    //BUG: potential bug point if charID == 0 - only occurs on unsuccessful login
                    // ^ is this still true?
                    //account found
                    accountfound = true;
                    if (clnt->id == client->id)
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
            if (map != nullptr && std::string(map->m_cName) == client->m_cMapName)
                client->m_cMapIndex = map->m_cMapIndex;

/*
        if (clientfound && clientfound->get_connected())
        {
            //client has recently disconnected
            //at this point, can do some fancy swapping of socket and place this current active socket
            // into the old client object letting them retake control of the disconnected session
            // - to do that though, it'd need to resend initial data - need a special case in GServer
            // - to not do the usual map data stuff when init data is sent or risk duplicating client
            // - objects in mapdata - also needs a case in GServer to not reject another initdata
            // - from an already connected client (used to be the cause of item duping on Int'l)
            // - and will need code in place to handle this specific case
            auto websocket = clientfound->get_websocket();
            if (websocket && websocket->getReadyState() == ix::ReadyState::Open)
            {
                //false alarm of some sort (or other issue) should probably breakpoint this or have some sort
                //of check if client even still exists. socket should never be open if disconnected was set
                //__debugbreak();
            }
            else
            {
                clientfound->set_disconnect_time(system_clock::time_point());
                clientfound->set_websocket(client->get_websocket());
                client->connection_state.reset();

                //since nothing is technically associated with this client, just remove it from the list to force it to delete

                std::lock_guard<std::recursive_mutex> lock(client_list_mtx);
                client_list.erase(client->shared_from_this());

                if (clientfound->m_cMapIndex != 0)
                {
                    clientfound->client_status = client_status_t::PLAYING;
                    strcpy(clientfound->m_cCharName, character_name.c_str());
                    //pgs->clientlist.push_back(client);
                    //clientlist.remove(client);

                    clientfound->m_bIsInitComplete = true;
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
        else*/ if (accountfound)
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
                client->client_status = client_status_t::PLAYING;
                strcpy(client->m_cCharName, character_name.c_str());

                client->m_bIsInitComplete = true;
                sw.write_uint32(MSGID_RESPONSE_ENTERGAME);
                sw.write_uint16(DEF_ENTERGAMERESTYPE_CONFIRM);
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
        log->critical(std::format("EnterGame() Exception: {}", std::string(ex.what())));
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

CItem * CGame::fill_item(CClient * player, item_db & item)
{
    CItem * item_ = new CItem();
    item_->id = item.id;
    if (_bInitItemAttr(item_, item.item_id) == false)
    {
        delete item_;
        return nullptr;
    }
    item_->m_dwCount = item.count;
    item_->m_sTouchEffectType = item.type;
    item_->m_sTouchEffectValue1 = item.id1;
    item_->m_sTouchEffectValue2 = item.id2;
    item_->m_sTouchEffectValue3 = item.id3;
    item_->m_cItemColor = item.color;
    item_->m_sItemSpecEffectValue1 = item.effect1;
    item_->m_sItemSpecEffectValue2 = item.effect2;
    item_->m_sItemSpecEffectValue3 = item.effect3;
    item_->m_wCurLifeSpan = item.durability;
    item_->m_dwAttribute = item.attribute;

    // todo: do this better
    if (item_->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER)
    {
        if ((item_->m_sTouchEffectValue1 != player->m_sCharIDnum1) ||
            (item_->m_sTouchEffectValue2 != player->m_sCharIDnum2) ||
            (item_->m_sTouchEffectValue3 != player->m_sCharIDnum3))
        {
            log->info(
                std::format("(!) Non-matching IDs for unique item: Player({}) Item({}) {} {} {} - {} {} {}",
                    player->m_cCharName,
                    item_->m_cName,
                    item_->m_sTouchEffectValue1,
                    item_->m_sTouchEffectValue2,
                    item_->m_sTouchEffectValue3,
                    player->m_sCharIDnum1,
                    player->m_sCharIDnum2,
                    player->m_sCharIDnum3)
            );
        }
    }

    return item_;
}

uint16_t CGame::add_bag_item(CClient * client, item_db & item)
{
    for (int i = 0; i < DEF_MAXITEMS; ++i)
    {
        if (client->m_pItemList[i] == nullptr)
        {
            CItem * item_ = fill_item(client, item);
            if (item_->m_cItemType == DEF_ITEMTYPE_EQUIP && item.equipped)
                client->m_bIsItemEquipped[i] = item.equipped;
            
            client->m_ItemPosList[i].x = item.itemposx;
            client->m_ItemPosList[i].y = item.itemposy;
            client->m_pItemList[i] = item_;
            return i;
        }
    }
    throw std::runtime_error("Bag is full");
}

uint16_t CGame::add_bank_item(CClient * client, item_db & item)
{
    for (int i = 0; i < DEF_MAXBANKITEMS; ++i)
    {
        if (client->m_pItemInBankList[i] == nullptr)
        {
            client->m_pItemInBankList[i] = fill_item(client, item);
            return i;
        }
    }
    throw std::runtime_error("Bank is full");
}

uint16_t CGame::add_mail_item(CClient * client, item_db & item)
{
    // todo
    return 0;
}

//how to delete client?
//void Server::delete_client_lock(Client * client,
//							bool save,//save player data
//							bool notify,//notify surrounding players
//							bool countlogout,//??
//							bool forceclose)//apply bleeding isle "ban" effect
//void Server::delete_client_lock(int iClientH, bool bSave, bool bNotify, bool bCountLogout, bool bForceCloseConn)

// delete_client_lock only closes the socket with all intents and purposes of leaving
// their character in game unless they are not fully logged in, in which case this
// would kill the client object as well unless param is passed to also delete the object
// a proper logout would get the deleteobj flag passed
// 
// originally Gate server code - moved to login server functionality

void CGame::delete_client_nolock(std::shared_ptr<CClient> player, bool save /*= false*/, bool deleteobj /*= false*/)
{
    if (!player) return;

    for (auto & wspair : websocket_clients)
        if (wspair.second == player)
        {
            wspair.first->close();
            break;
        }

    DeleteClient(player->client_handle, save, deleteobj);

    //delete_client(client, save, deleteobj);
}

void CGame::delete_client_lock(std::shared_ptr<CClient> player, bool save /*= false*/, bool deleteobj /*= false*/)
{
    if (!player) return;


    std::lock_guard<std::recursive_mutex> l(websocket_list);

    for (auto & wspair : websocket_clients)
        if (wspair.second == player)
            wspair.first->close();

    {
        std::lock_guard<std::recursive_mutex> l2(client_list_mtx);
        //delete_client(client, save, deleteobj);
        DeleteClient(player->client_handle, save, deleteobj);
    }
}

int16_t CGame::get_map_index(std::string_view mapname)
{
    for (int i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] && !strcmp(m_pMapList[i]->m_cName, mapname.data()))
            return i;
    throw std::runtime_error("map not found");
}

void CGame::CheckConnectionHandler(int iClientH, char * pData)
{
    char * cp;
    DWORD * dwp, dwTimeRcv, dwTime, dwTimeGapClient, dwTimeGapServer;

    if (m_pClientList[iClientH] == 0) return;
    //m_pClientList[iClientH]->m_cConnectionCheck = 0;

    dwTime = timeGetTime();
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    dwp = (DWORD *)cp;
    dwTimeRcv = *dwp;

    if (m_pClientList[iClientH]->m_dwInitCCTimeRcv == 0)
    {
        m_pClientList[iClientH]->m_dwInitCCTimeRcv = dwTimeRcv;
        m_pClientList[iClientH]->m_dwInitCCTime = dwTime;
    }
    else
    {
        dwTimeGapClient = (dwTimeRcv - m_pClientList[iClientH]->m_dwInitCCTimeRcv);
        dwTimeGapServer = (dwTime - m_pClientList[iClientH]->m_dwInitCCTime);

        if (dwTimeGapClient < dwTimeGapServer) return;
        if ((abs((long long)(dwTimeGapClient - dwTimeGapServer))) >= (DEF_CLIENTTIMEOUT))
        {
            DeleteClient(iClientH, true, true);
            return;
        }
    }
}
