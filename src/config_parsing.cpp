//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include "Map.h"

extern char G_cTxt[512];

using json = nlohmann::json;
using namespace std::chrono;
using namespace std::chrono_literals;

void CGame::load_config()
{
    bool has_log = false;
    if (log != nullptr)
        has_log = true;

    try
    {
        std::ifstream config_file("config.json");

        if (!config_file.is_open())
        {
            std::perror("File opening failed");
            throw std::runtime_error("config.json does not exist");
        }

        json cfg;
        config_file >> cfg;

        if (!cfg["log-level"].is_null())
        {
            if (cfg["log-level"].is_number_integer())
            {
                auto l = static_cast<spdlog::level::level_enum>(cfg["log-level"].get<int32_t>());
                if (cfg["log-level"].get<int32_t>() > 6 || cfg["log-level"].get<int32_t>() < 0)
                {
                    if (has_log) log->error("Unknown \"log-level\" in config.json. Default: info");
                    else std::cout << "Unknown \"log-level\" in config.json. Default: info\n";
                    l = spdlog::level::level_enum::info;
                }
                loglevel = l;
            }
            else if (cfg["log-level"].is_string())
            {
                std::string s = cfg["log-level"].get<std::string>();
                if (s == "trace")
                    loglevel = spdlog::level::level_enum::trace;
                else if (s == "debug")
                    loglevel = spdlog::level::level_enum::debug;
                else if (s == "info")
                    loglevel = spdlog::level::level_enum::info;
                else if (s == "warn")
                    loglevel = spdlog::level::level_enum::warn;
                else if (s == "err")
                    loglevel = spdlog::level::level_enum::err;
                else if (s == "critical")
                    loglevel = spdlog::level::level_enum::critical;
                else if (s == "off")
                    loglevel = spdlog::level::level_enum::off;
                else
                {
                    if (has_log) log->error(R"(Unknown "log-level" in config.json. Default: info)");
                    else std::cout << R"(Unknown "log-level" in config.json. Default: info)" << '\n';
                    loglevel = spdlog::level::level_enum::info;
                }
            }
            else
            {
                if (has_log) log->error(R"(Unknown "log-level" in config.json. Default: info)");
                else std::cout << R"(Unknown "log-level" in config.json. Default: info)" << '\n';
                loglevel = spdlog::level::level_enum::info;
            }
        }
        else
            loglevel = spdlog::level::level_enum::info;

        if (!cfg["log-format"].is_null())
            log_formatting = cfg["log-format"].get<std::string>();
        else
            log_formatting = "%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v";

        if (has_log)
        {
            log->set_pattern(log_formatting);
            log->set_level(loglevel);
        }

        std::string world_name_old = world_name;

        std::string login_sqluser_old = login_sqluser;
        std::string login_sqlpass_old = login_sqlpass;
        std::string login_sqldb_old = login_sqldb;
        std::string login_sqlhost_old = login_sqlhost;
        uint16_t login_sqlport_old = login_sqlport;

        std::string game_sqluser_old = game_sqluser;
        std::string game_sqlpass_old = game_sqlpass;
        std::string game_sqldb_old = game_sqldb;
        std::string game_sqlhost_old = game_sqlhost;
        uint16_t game_sqlport_old = game_sqlport;

        std::string bindip_old = bindip;
        uint16_t bindport_old = bindport;

        {
            if (!cfg["world_name"].is_null())
                world_name = cfg["world_name"].get<std::string>();
            else
                throw std::runtime_error("No world_name set");

//             if (!cfg["maps"].is_null())
//                 maps_loaded = cfg["maps"].get<std::vector<std::string>>();
//             else
//                 throw std::runtime_error("No maps set");

            if (!cfg["login_sqluser"].is_null())
                login_sqluser = cfg["login_sqluser"].get<std::string>();
            else
                throw std::runtime_error("No login_sqluser set");

            if (!cfg["login_sqlpass"].is_null())
                login_sqlpass = cfg["login_sqlpass"].get<std::string>();
            else
                throw std::runtime_error("No login_sqlpass set");

            if (!cfg["login_sqldb"].is_null())
                login_sqldb = cfg["login_sqldb"].get<std::string>();
            else
                throw std::runtime_error("No login_sqldb set");

            if (!cfg["login_sqlhost"].is_null())
                login_sqlhost = cfg["login_sqlhost"].get<std::string>();
            else
                throw std::runtime_error("No login_sqlhost set");

            if (!cfg["login_sqlport"].is_null())
                login_sqlport = cfg["login_sqlport"].get<uint16_t>();
            else
                throw std::runtime_error("No login_sqlport set");

            if (!cfg["game_sqluser"].is_null())
                game_sqluser = cfg["game_sqluser"].get<std::string>();
            else
                throw std::runtime_error("No game_sqluser set");

            if (!cfg["game_sqlpass"].is_null())
                game_sqlpass = cfg["game_sqlpass"].get<std::string>();
            else
                throw std::runtime_error("No game_sqlpass set");

            if (!cfg["game_sqldb"].is_null())
                game_sqldb = cfg["game_sqldb"].get<std::string>();
            else
                throw std::runtime_error("No game_sqldb set");

            if (!cfg["game_sqlhost"].is_null())
                game_sqlhost = cfg["game_sqlhost"].get<std::string>();
            else
                throw std::runtime_error("No game_sqlhost set");

            if (!cfg["game_sqlport"].is_null())
                game_sqlport = cfg["game_sqlport"].get<uint16_t>();
            else
                throw std::runtime_error("No game_sqlport set");

            if (!cfg["bind_ip"].is_null())
                bindip = cfg["bind_ip"].get<std::string>();
            else
                throw std::runtime_error("No bind_ip set");

            if (!cfg["port"].is_null())
                bindport = cfg["port"].get<uint16_t>();
            else
                throw std::runtime_error("No port set");

#if defined(HELBREATHX)
            if (!cfg["login_auth_url"].is_null())
                login_auth_url = cfg["login_auth_url"].get<std::string>();
            else
                throw std::runtime_error("No login_auth_url set");

            if (!cfg["login_auth_key"].is_null())
                login_auth_key = cfg["login_auth_key"].get<std::string>();
            else
                throw std::runtime_error("No login_auth_key set");
#endif
        }

        // if config has already been loaded once, check for changes
        if (config_loaded)
        {
            bool reconnect_sql = false;
            if (world_name_old != world_name) log->info("World name changed from {} to {}", world_name_old, world_name);
            if (login_sqluser_old != login_sqluser) { log->info("SQL user changed from {} to {}", login_sqluser_old, login_sqluser); reconnect_sql = true; }
            if (login_sqlpass_old != login_sqlpass) { log->info("SQL pass changed"); reconnect_sql = true; }
            if (login_sqldb_old != login_sqldb) { log->info("SQL DB changed from {} to {}", login_sqldb_old, login_sqldb); reconnect_sql = true; }
            if (login_sqlhost_old != login_sqlhost) { log->info("SQL host changed from {} to {}", login_sqlhost_old, login_sqlhost); reconnect_sql = true; }
            if (login_sqlport_old != login_sqlport) { log->info("SQL port changed from {} to {}", login_sqlport_old, login_sqlport); reconnect_sql = true; }
            if (game_sqluser_old != game_sqluser) { log->info("SQL user changed from {} to {}", game_sqluser_old, game_sqluser); reconnect_sql = true; }
            if (game_sqlpass_old != game_sqlpass) { log->info("SQL pass changed"); reconnect_sql = true; }
            if (game_sqldb_old != game_sqldb) { log->info("SQL DB changed from {} to {}", game_sqldb_old, game_sqldb); reconnect_sql = true; }
            if (game_sqlhost_old != game_sqlhost) { log->info("SQL host changed from {} to {}", game_sqlhost_old, game_sqlhost); reconnect_sql = true; }
            if (game_sqlport_old != game_sqlport) { log->info("SQL port changed from {} to {}", game_sqlport_old, game_sqlport); reconnect_sql = true; }
            if (bindip_old != bindip) log->info("Bind ip changed from {} to {}", bindip_old, bindip);
            if (bindport_old != bindport) log->info("Bind port changed from {} to {}", bindport_old, bindport);

            if (reconnect_sql)
            {
                std::unique_lock<std::shared_mutex> l(game_sql_mtx, std::defer_lock);
                std::unique_lock<std::shared_mutex> l2(login_sql_mtx, std::defer_lock);
                std::lock(l, l2);
                try
                {
                    pq_login = std::make_unique<pqxx::connection>(fmt::format("postgresql://{}:{}@{}:{}/{}", login_sqluser, login_sqlpass, login_sqlhost, login_sqlport, login_sqldb));
                    pq_game = std::make_unique<pqxx::connection>(fmt::format("postgresql://{}:{}@{}:{}/{}", game_sqluser, game_sqlpass, game_sqlhost, game_sqlport, game_sqldb));
                }
                catch (pqxx::broken_connection & ex)
                {
                    throw std::runtime_error(fmt::format("Unable to connect to SQL - {}", ex.what()));
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
            }
        }
    }
    catch (std::exception & e)
    {
        if (has_log) log->info("Error loading config.json : {}", e.what());
        else std::cout << "Error loading config.json : " << e.what() << '\n';

        // allows reloading of configurations to not break state since there should already be a valid value from a prior
        // successful load
        if (get_server_state() == server_status::uninitialized)
            state_valid = false;
    }
    config_loaded = true;
}

bool CGame::_bDecodeBuildItemConfigFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  i, iIndex = 0;
    CStrTok * pStrTok;
    CItem * pItem;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();
    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            memset(m_pBuildItemList[iIndex]->m_cName, 0, sizeof(m_pBuildItemList[iIndex]->m_cName));
                            memcpy(m_pBuildItemList[iIndex]->m_cName, token, strlen(token));

                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format(1).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iSkillLimit = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemID[0] = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemCount[0] = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemValue[0] = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemID[1] = atoi(token);
                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemCount[1] = atoi(token);
                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemValue[1] = atoi(token);
                            cReadModeB = 9;
                            break;

                        case 9:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemID[2] = atoi(token);
                            cReadModeB = 10;
                            break;

                        case 10:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemCount[2] = atoi(token);
                            cReadModeB = 11;
                            break;

                        case 11:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemValue[2] = atoi(token);
                            cReadModeB = 12;
                            break;


                        case 12:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemID[3] = atoi(token);
                            cReadModeB = 13;
                            break;

                        case 13:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemCount[3] = atoi(token);
                            cReadModeB = 14;
                            break;

                        case 14:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemValue[3] = atoi(token);
                            cReadModeB = 15;
                            break;

                        case 15:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemID[4] = atoi(token);
                            cReadModeB = 16;
                            break;

                        case 16:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pBuildItemList[iIndex]->m_iMaterialItemCount[4] = atoi(token);

                            cReadModeB = 17;
                            break;

                        case 17:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemValue[4] = atoi(token);

                            cReadModeB = 18;
                            break;


                        case 18:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemID[5] = atoi(token);
                            cReadModeB = 19;
                            break;

                        case 19:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pBuildItemList[iIndex]->m_iMaterialItemCount[5] = atoi(token);

                            cReadModeB = 20;
                            break;

                        case 20:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaterialItemValue[5] = atoi(token);

                            cReadModeB = 21;
                            break;



                        case 21:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iAverageValue = atoi(token);

                            cReadModeB = 22;
                            break;

                        case 22:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_iMaxSkill = atoi(token);

                            cReadModeB = 23;
                            break;

                        case 23:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! BuildItem configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pBuildItemList[iIndex]->m_wAttribute = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;

                            pItem = new CItem;
                            if (_bInitItemAttr(pItem, m_pBuildItemList[iIndex]->m_cName) == true)
                            {
                                m_pBuildItemList[iIndex]->m_sItemID = pItem->m_sIDnum;

                                for (i = 0; i < 6; i++)
                                    m_pBuildItemList[iIndex]->m_iMaxValue += (m_pBuildItemList[iIndex]->m_iMaterialItemValue[i] * 100);

                                iIndex++;
                            }
                            else
                            {
                                wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! BuildItem configuration file error - Not Existing Item(%s)", m_pBuildItemList[iIndex]->m_cName);
                                log->info(G_cTxt);

                                delete m_pBuildItemList[iIndex];
                                m_pBuildItemList[iIndex] = 0;

                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            delete pItem;
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "BuildItem", 9) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
                m_pBuildItemList[iIndex] = new CBuildItem;
            }

        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! BuildItem configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) BuildItem(Total:%d) configuration - success!", iIndex);
    log->info(cTxt);

    return true;
}

bool CGame::_bDecodeDupItemIDFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iIndex = 0;
    CStrTok * pStrTok;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();

    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            if (m_pDupItemIDList[atoi(token)] != 0)
                            {
                                log->info("(!!!) ERROR! DupItemID configuration file error - Duplicate magic number.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pDupItemIDList[atoi(token)] = new CItem;
                            iIndex = atoi(token);

                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pDupItemIDList[iIndex]->m_sTouchEffectType = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pDupItemIDList[iIndex]->m_sTouchEffectValue1 = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pDupItemIDList[iIndex]->m_sTouchEffectValue2 = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pDupItemIDList[iIndex]->m_sTouchEffectValue3 = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pDupItemIDList[iIndex]->m_wPrice = (WORD)atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "DupItemID", 9) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
            }

        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) ERROR! DupItemID configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) DupItemID(Total:%d) configuration - success!", iIndex);
    log->info(cTxt);

    return true;
}

bool CGame::_bDecodeItemConfigFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iItemConfigListIndex, iTemp;
    CStrTok * pStrTok;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();
    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemIDnumber");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iItemConfigListIndex = atoi(token);

                            if (iItemConfigListIndex == 490)
                                iItemConfigListIndex = atoi(token);

                            if (m_pItemConfigList[iItemConfigListIndex] != 0)
                            {
                                wsprintf(cTxt, "(!!!) CRITICAL ERROR! Duplicate ItemIDnum(%d)", iItemConfigListIndex);
                                log->info(cTxt);
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex] = new CItem;
                            m_pItemConfigList[iItemConfigListIndex]->m_sIDnum = iItemConfigListIndex;
                            cReadModeB = 2;
                            break;

                        case 2:
                            memset(m_pItemConfigList[iItemConfigListIndex]->m_cName, 0, sizeof(m_pItemConfigList[iItemConfigListIndex]->m_cName));
                            memcpy(m_pItemConfigList[iItemConfigListIndex]->m_cName, token, strlen(token));
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemType");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_cItemType = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - EquipPos");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_cEquipPos = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectType");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectType = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue1");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue1 = atoi(token);
                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue2");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue2 = atoi(token);
                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue3");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue3 = atoi(token);
                            cReadModeB = 9;
                            break;

                        case 9:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue4");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue4 = atoi(token);
                            cReadModeB = 10;
                            break;

                        case 10:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue5");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue5 = atoi(token);
                            cReadModeB = 11;
                            break;

                        case 11:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue6");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue6 = atoi(token);
                            cReadModeB = 12;
                            break;

                        case 12:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - MaxLifeSpan");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_wMaxLifeSpan = (WORD)atoi(token);
                            cReadModeB = 13;
                            break;

                        case 13:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - MaxFixCount");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffect = atoi(token);
                            cReadModeB = 14;
                            break;

                        case 14:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - Sprite");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sSprite = atoi(token);
                            cReadModeB = 15;
                            break;

                        case 15:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - SpriteFrame");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sSpriteFrame = atoi(token);
                            cReadModeB = 16;
                            break;

                        case 16:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - Price");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iTemp = atoi(token);
                            if (iTemp < 0)
                                m_pItemConfigList[iItemConfigListIndex]->m_bIsForSale = false;
                            else m_pItemConfigList[iItemConfigListIndex]->m_bIsForSale = true;

                            m_pItemConfigList[iItemConfigListIndex]->m_wPrice = abs(iTemp);
                            cReadModeB = 17;
                            break;

                        case 17:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - Weight");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_wWeight = atoi(token);
                            cReadModeB = 18;
                            break;

                        case 18:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - ApprValue");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_cApprValue = atoi(token);
                            cReadModeB = 19;
                            break;

                        case 19:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - Speed");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_cSpeed = atoi(token);
                            cReadModeB = 20;
                            break;

                        case 20:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - LevelLimit");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sLevelLimit = atoi(token);
                            cReadModeB = 21;
                            break;

                        case 21:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - GenderLimit");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_cGenderLimit = atoi(token);
                            cReadModeB = 22;
                            break;

                        case 22:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - SM_HitRatio");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffectValue1 = atoi(token);
                            cReadModeB = 23;
                            break;

                        case 23:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - L_HitRatio");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffectValue2 = atoi(token);
                            cReadModeB = 24;
                            break;

                        case 24:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - RelatedSkill");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_sRelatedSkill = atoi(token);
                            cReadModeB = 25;
                            break;

                        case 25:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - Category");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_cCategory = atoi(token);
                            cReadModeB = 26;
                            break;

                        case 26:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! ITEM configuration file error - Category");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pItemConfigList[iItemConfigListIndex]->m_cItemColor = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "Item", 4) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
            }

            if (memcmp(token, "[ENDITEMLIST]", 13) == 0)
            {
                cReadModeA = 0;
                cReadModeB = 0;
                goto DICFC_STOPDECODING;
            }
        }
        token = pStrTok->pGet();
    }

    DICFC_STOPDECODING:;

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! ITEM configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) ITEM(Total:%d) configuration - success!", iItemConfigListIndex);
    log->info(cTxt);

    return true;
}

bool CGame::_bDecodeMagicConfigFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iMagicConfigListIndex = 0;
    CStrTok * pStrTok;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();
    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            if (m_pMagicConfigList[atoi(token)] != 0)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Duplicate magic number.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[atoi(token)] = new CMagic;
                            iMagicConfigListIndex = atoi(token);

                            cReadModeB = 2;
                            break;

                        case 2:
                            memset(m_pMagicConfigList[iMagicConfigListIndex]->m_cName, 0, sizeof(m_pMagicConfigList[iMagicConfigListIndex]->m_cName));
                            memcpy(m_pMagicConfigList[iMagicConfigListIndex]->m_cName, token, strlen(token));
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sType = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_dwDelayTime = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_dwLastTime = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue1 = atoi(token);
                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue2 = atoi(token);
                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue3 = atoi(token);
                            cReadModeB = 9;
                            break;

                        case 9:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue4 = atoi(token);
                            cReadModeB = 10;
                            break;

                        case 10:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue5 = atoi(token);
                            cReadModeB = 11;
                            break;

                        case 11:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue6 = atoi(token);
                            cReadModeB = 12;
                            break;

                        case 12:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue7 = atoi(token);
                            cReadModeB = 13;
                            break;

                        case 13:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue8 = atoi(token);
                            cReadModeB = 14;
                            break;

                        case 14:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue9 = atoi(token);
                            cReadModeB = 15;
                            break;

                        case 15:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue10 = atoi(token);
                            cReadModeB = 16;
                            break;

                        case 16:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue11 = atoi(token);
                            cReadModeB = 17;
                            break;

                        case 17:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sValue12 = atoi(token);
                            cReadModeB = 18;
                            break;

                        case 18:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_sIntLimit = atoi(token);
                            cReadModeB = 19;
                            break;

                        case 19:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_iGoldCost = atoi(token);

                            cReadModeB = 20;
                            break;

                        case 20:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_cCategory = atoi(token);
                            cReadModeB = 21;
                            break;

                        case 21:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMagicConfigList[iMagicConfigListIndex]->m_iAttribute = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "magic", 5) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
            }

        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! MAGIC configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) MAGIC(Total:%d) configuration - success!", iMagicConfigListIndex);
    log->info(cTxt);

    return true;

}

bool CGame::_bDecodeSkillConfigFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iSkillConfigListIndex = 0;
    CStrTok * pStrTok;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();
    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            if (m_pSkillConfigList[atoi(token)] != 0)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Duplicate magic number.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[atoi(token)] = new CSkill;
                            iSkillConfigListIndex = atoi(token);

                            cReadModeB = 2;
                            break;

                        case 2:
                            memset(m_pSkillConfigList[iSkillConfigListIndex]->m_cName, 0, sizeof(m_pSkillConfigList[iSkillConfigListIndex]->m_cName));
                            memcpy(m_pSkillConfigList[iSkillConfigListIndex]->m_cName, token, strlen(token));
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[iSkillConfigListIndex]->m_sType = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[iSkillConfigListIndex]->m_sValue1 = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[iSkillConfigListIndex]->m_sValue2 = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[iSkillConfigListIndex]->m_sValue3 = atoi(token);
                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[iSkillConfigListIndex]->m_sValue4 = atoi(token);
                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[iSkillConfigListIndex]->m_sValue5 = atoi(token);
                            cReadModeB = 9;
                            break;

                        case 9:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pSkillConfigList[iSkillConfigListIndex]->m_sValue6 = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "skill", 5) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
            }

        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! SKILL configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) SKILL(Total:%d) configuration - success!", iSkillConfigListIndex);
    log->info(cTxt);

    return true;
}

void CGame::_bDecodeNoticementFileContents(char * pData, uint32_t dwMsgSize)
{
    char * cp;

    cp = (pData);

    if (m_pNoticementData != 0)
    {
        delete m_pNoticementData;
        m_pNoticementData = 0;
    }

    m_pNoticementData = new char[dwMsgSize + 1];
    memset(m_pNoticementData, 0, dwMsgSize + 1);

    memcpy(m_pNoticementData, cp, dwMsgSize);
    m_dwNoticementDataSize = dwMsgSize;

    wsprintf(G_cTxt, "(!) Noticement Data Size: %d", m_dwNoticementDataSize);
    log->info(G_cTxt);
}

bool CGame::_bDecodeNpcConfigFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iNpcConfigListIndex = 0;
    CStrTok * pStrTok;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();
    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (strlen(token) > 20)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Too long Npc name.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            memset(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, 0, sizeof(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName));
                            memcpy(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, token, strlen(token));
                            cReadModeB = 2;
                            break;
                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_sType = atoi(token);
                            cReadModeB = 3;
                            break;
                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iHitDice = atoi(token);
                            cReadModeB = 4;
                            break;
                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iDefenseRatio = atoi(token);
                            cReadModeB = 5;
                            break;
                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iHitRatio = atoi(token);
                            cReadModeB = 6;
                            break;
                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iMinBravery = atoi(token);
                            cReadModeB = 7;
                            break;
                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iExpDiceMin = atoi(token);
                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iExpDiceMax = atoi(token);
                            cReadModeB = 9;
                            break;

                        case 9:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iGoldDiceMin = atoi(token);
                            cReadModeB = 10;
                            break;

                        case 10:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iGoldDiceMax = atoi(token);
                            cReadModeB = 11;
                            break;

                        case 11:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cAttackDiceThrow = atoi(token);
                            cReadModeB = 12;
                            break;

                        case 12:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cAttackDiceRange = atoi(token);
                            cReadModeB = 13;
                            break;

                        case 13:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cSize = atoi(token);
                            cReadModeB = 14;
                            break;

                        case 14:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cSide = atoi(token);
                            cReadModeB = 15;
                            break;

                        case 15:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cActionLimit = atoi(token);
                            cReadModeB = 16;
                            break;

                        case 16:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_dwActionTime = atoi(token);
                            cReadModeB = 17;
                            break;

                        case 17:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cResistMagic = atoi(token);
                            cReadModeB = 18;
                            break;

                        case 18:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cMagicLevel = atoi(token);
                            cReadModeB = 19;
                            break;

                        case 19:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cDayOfWeekLimit = atoi(token);
                            cReadModeB = 20;
                            break;

                        case 20:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cChatMsgPresence = atoi(token);

                            if (m_pNpcConfigList[iNpcConfigListIndex]->m_cChatMsgPresence == 1)
                            {
                            }
                            cReadModeB = 21;
                            break;

                        case 21:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cTargetSearchRange = atoi(token);

                            cReadModeB = 22;
                            break;

                        case 22:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_dwRegenTime = atoi(token);

                            cReadModeB = 23;
                            break;

                        case 23:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_cAttribute = atoi(token);

                            cReadModeB = 24;
                            break;

                        case 24:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iAbsDamage = atoi(token);

                            cReadModeB = 25;
                            break;

                        case 25:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iMaxMana = atoi(token);

                            cReadModeB = 26;
                            break;

                        case 26:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iMagicHitRatio = atoi(token);

                            cReadModeB = 27;
                            break;

                        case 27:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pNpcConfigList[iNpcConfigListIndex]->m_iAttackRange = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;
                            iNpcConfigListIndex++;
                            break;
                    }
                    break;

                case 2:
                    m_iPlayerMaxLevel = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "Npc", 3) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
                m_pNpcConfigList[iNpcConfigListIndex] = new CNpc(" ");
            }

            if (memcmp(token, "world-server-max-level", 22) == 0)
            {
                cReadModeA = 2;
                cReadModeB = 1;
            }
        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! NPC configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) NPC(Total:%d) configuration - success!", iNpcConfigListIndex);
    log->info(cTxt);

    return true;
}

bool CGame::_bDecodeOccupyFlagSaveFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token;
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iFlagIndex = 0;
    int  iSide;
    CStrTok * pStrTok;
    int dX, dY, iEKNum, iTotalFlags;

    if (m_iMiddlelandMapIndex < 0)
    {
        log->info("(!) This game server does not managing Middleland Map. OcuupyFlag data ignored.");
        return false;
    }

    iTotalFlags = 0;

    pContents = new char[dwMsgSize + 10];
    memset(pContents, 0, dwMsgSize + 10);
    memcpy(pContents, pData, dwMsgSize - 9);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();

    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! OccupyFlag save file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            iSide = atoi(token);
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! OccupyFlag save file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            dX = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! OccupyFlag save file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            dY = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! OccupyFlag save file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            iEKNum = atoi(token);

                            if (__bSetOccupyFlag(m_iMiddlelandMapIndex, dX, dY, iSide, iEKNum, -1, true) == true)
                                iTotalFlags++;

                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "flag", 4) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
            }
        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! OccupyFlag save file contents error!");
        return false;
    }

    wsprintf(G_cTxt, "(!) OccupyFlag save file decoding success! Total(%d)", iTotalFlags);
    log->info(G_cTxt);

    return true;
}

bool CGame::_bDecodePortionConfigFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iPortionConfigListIndex = 0;
    CStrTok * pStrTok;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();
    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! POTION configuration file error - Wrong Data format(1).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            if (m_pPortionConfigList[atoi(token)] != 0)
                            {
                                log->info("(!!!) CRITICAL ERROR! POTION configuration file error - Duplicate portion number.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pPortionConfigList[atoi(token)] = new CPotion;
                            iPortionConfigListIndex = atoi(token);

                            cReadModeB = 2;
                            break;

                        case 2:
                            memset(m_pPortionConfigList[iPortionConfigListIndex]->m_cName, 0, sizeof(m_pPortionConfigList[iPortionConfigListIndex]->m_cName));
                            memcpy(m_pPortionConfigList[iPortionConfigListIndex]->m_cName, token, strlen(token));
                            cReadModeB = 3;
                            break;

                        default:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pPortionConfigList[iPortionConfigListIndex]->m_sArray[cReadModeB - 3] = atoi(token);
                            cReadModeB++;
                            break;

                        case 14:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pPortionConfigList[iPortionConfigListIndex]->m_sArray[11] = atoi(token);
                            cReadModeB = 15;
                            break;

                        case 15:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pPortionConfigList[iPortionConfigListIndex]->m_iSkillLimit = atoi(token);
                            cReadModeB = 16;
                            break;

                        case 16:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! MAGIC configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pPortionConfigList[iPortionConfigListIndex]->m_iDifficulty = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "potion", 5) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
            }

        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! POTION configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) POTION(Total:%d) configuration - success!", iPortionConfigListIndex);
    log->info(cTxt);

    return true;
}

bool CGame::_bDecodeQuestConfigFileContents(char * pData, uint32_t dwMsgSize)
{
    char * pContents, * token, cTxt[120];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iQuestConfigListIndex = 0;
    CStrTok * pStrTok;

    pContents = new char[dwMsgSize + 1];
    memset(pContents, 0, dwMsgSize + 1);
    memcpy(pContents, pData, dwMsgSize);

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();

    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            if (m_pQuestConfigList[atoi(token)] != 0)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Duplicate quest number.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[atoi(token)] = new CQuest;
                            iQuestConfigListIndex = atoi(token);

                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_cSide = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iType = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iTargetType = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iMaxCount = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iFrom = atoi(token);
                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iMinLevel = atoi(token);
                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iMaxLevel = atoi(token);
                            cReadModeB = 9;
                            break;

                        case 9:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRequiredSkillNum = atoi(token);
                            cReadModeB = 10;
                            break;

                        case 10:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRequiredSkillLevel = atoi(token);
                            cReadModeB = 11;
                            break;

                        case 11:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iTimeLimit = atoi(token);
                            cReadModeB = 12;
                            break;

                        case 12:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iAssignType = atoi(token);
                            cReadModeB = 13;
                            break;

                        case 13:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRewardType[1] = atoi(token);
                            cReadModeB = 14;
                            break;

                        case 14:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRewardAmount[1] = atoi(token);
                            cReadModeB = 15;
                            break;

                        case 15:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRewardType[2] = atoi(token);
                            cReadModeB = 16;
                            break;

                        case 16:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRewardAmount[2] = atoi(token);
                            cReadModeB = 17;
                            break;

                        case 17:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRewardType[3] = atoi(token);
                            cReadModeB = 18;
                            break;

                        case 18:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRewardAmount[3] = atoi(token);
                            cReadModeB = 19;
                            break;

                        case 19:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iContribution = atoi(token);
                            cReadModeB = 20;
                            break;

                        case 20:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iContributionLimit = atoi(token);
                            cReadModeB = 21;
                            break;

                        case 21:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iResponseMode = atoi(token);
                            cReadModeB = 22;
                            break;

                        case 22:
                            memset(m_pQuestConfigList[iQuestConfigListIndex]->m_cTargetName, 0, sizeof(m_pQuestConfigList[iQuestConfigListIndex]->m_cTargetName));
                            strcpy(m_pQuestConfigList[iQuestConfigListIndex]->m_cTargetName, token);
                            cReadModeB = 23;
                            break;

                        case 23:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_sX = atoi(token);
                            cReadModeB = 24;
                            break;

                        case 24:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_sY = atoi(token);
                            cReadModeB = 25;
                            break;

                        case 25:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iRange = atoi(token);
                            cReadModeB = 26;
                            break;

                        case 26:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iQuestID = atoi(token);
                            cReadModeB = 27;
                            break;

                        case 27:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! QUEST configuration file error - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pQuestConfigList[iQuestConfigListIndex]->m_iReqContribution = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;

                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "quest", 5) == 0)
            {
                cReadModeA = 1;
                cReadModeB = 1;
            }
        }
        token = pStrTok->pGet();
    }

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! QUEST configuration file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) QUEST(Total:%d) configuration - success!", iQuestConfigListIndex);
    log->info(cTxt);

    return true;
}

bool CGame::bReadAdminSetConfigFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadMode, cGSMode[16] = "";
    char seps[] = "= \t\n";
    CStrTok * pStrTok;

    cReadMode = 0;

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        return false;
    }
    else
    {
        log->info("(!) Reading settings file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();
        while (token != 0)
        {

            if (cReadMode != 0)
            {
                switch (cReadMode)
                {

                    case 1:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelGMKill = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelGMKill = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 2:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelGMRevive = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelGMRevive = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 3:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelGMCloseconn = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelGMCloseconn = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 4:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelGMCheckRep = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelGMCheckRep = 1;
                        }
                        cReadMode = 0;
                        break;
                    case 5:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelWho = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelWho = 1;
                        }
                        cReadMode = 0;
                        break;
                    case 6:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelEnergySphere = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelEnergySphere = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 7:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelShutdown = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelShutdown = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 8:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelObserver = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelObserver = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 9:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelShutup = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelShutup = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 10:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelCallGaurd = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelCallGaurd = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 11:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSummonDemon = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSummonDemon = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 12:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSummonDeath = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSummonDeath = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 13:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelReserveFightzone = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelReserveFightzone = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 14:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelCreateFish = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelCreateFish = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 15:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelTeleport = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelTeleport = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 16:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelCheckIP = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelCheckIP = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 17:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelPolymorph = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelPolymorph = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 18:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSetInvis = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSetInvis = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 19:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSetZerk = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSetZerk = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 20:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSetIce = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSetIce = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 21:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelGetNpcStatus = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelGetNpcStatus = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 22:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSetAttackMode = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSetAttackMode = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 23:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelUnsummonAll = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelUnsummonAll = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 24:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelUnsummonDemon = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelUnsummonDemon = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 25:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSummon = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSummon = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 26:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSummonAll = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSummonAll = 4;
                        }
                        cReadMode = 0;
                        break;
                    case 27:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSummonPlayer = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSummonPlayer = 1;
                        }
                        cReadMode = 0;
                        break;
                    case 28:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelDisconnectAll = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelDisconnectAll = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 29:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelEnableCreateItem = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelEnableCreateItem = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 30:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelCreateItem = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelCreateItem = 4;
                        }
                        cReadMode = 0;
                        break;
                    case 31:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelStorm = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelStorm = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 32:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelWeather = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelWeather = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 33:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSetStatus = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSetStatus = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 34:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelGoto = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelGoto = 1;
                        }
                        cReadMode = 0;
                        break;
                    case 35:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelMonsterCount = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelMonsterCount = 1;
                        }
                        cReadMode = 0;
                        break;
                    case 36:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSetRecallTime = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSetRecallTime = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 37:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelUnsummonBoss = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelUnsummonBoss = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 38:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelClearNpc = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelClearNpc = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 39:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelTime = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelTime = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 40:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelPushPlayer = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelPushPlayer = 2;
                        }
                        cReadMode = 0;
                        break;
                    case 41:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelSummonGuild = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelSummonGuild = 3;
                        }
                        cReadMode = 0;
                        break;
                    case 42:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelCheckStatus = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelCheckStatus = 1;
                        }
                        cReadMode = 0;
                        break;
                    case 43:
                        if ((strlen(token) > 0) && (strlen(token) < 9))
                        {
                            m_iAdminLevelCleanMap = atoi(token);
                        }
                        else
                        {
                            m_iAdminLevelCleanMap = 1;
                        }
                        cReadMode = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "Admin-Level-/kill", 17) == 0)						cReadMode = 1;
                if (memcmp(token, "Admin-Level-/revive", 19) == 0)						cReadMode = 2;
                if (memcmp(token, "Admin-Level-/closecon", 21) == 0)					cReadMode = 3;
                if (memcmp(token, "Admin-Level-/checkrep", 21) == 0)					cReadMode = 4;
                if (memcmp(token, "Admin-Level-/who", 16) == 0)							cReadMode = 5;
                if (memcmp(token, "Admin-Level-/energysphere", 25) == 0)				cReadMode = 6;
                if (memcmp(token, "Admin-Level-/shutdownthisserverrightnow", 39) == 0)	cReadMode = 7;
                if (memcmp(token, "Admin-Level-/setobservermode", 28) == 0)				cReadMode = 8;
                if (memcmp(token, "Admin-Level-/shutup", 19) == 0)						cReadMode = 9;
                if (memcmp(token, "Admin-Level-/attack", 19) == 0)						cReadMode = 10;
                if (memcmp(token, "Admin-Level-/summondemon", 24) == 0)					cReadMode = 11;
                if (memcmp(token, "Admin-Level-/summondeath", 24) == 0)					cReadMode = 12;
                if (memcmp(token, "Admin-Level-/reservefightzone", 28) == 0)			cReadMode = 13;
                if (memcmp(token, "Admin-Level-/createfish", 23) == 0)					cReadMode = 14;
                if (memcmp(token, "Admin-Level-/teleport", 21) == 0)					cReadMode = 15;
                if (memcmp(token, "Admin-Level-/checkip", 20) == 0)						cReadMode = 16;
                if (memcmp(token, "Admin-Level-/polymorph", 22) == 0)					cReadMode = 17;
                if (memcmp(token, "Admin-Level-/setinvi", 20) == 0)						cReadMode = 18;
                if (memcmp(token, "Admin-Level-/setzerk", 20) == 0)						cReadMode = 19;
                if (memcmp(token, "Admin-Level-/setfreeze", 22) == 0)					cReadMode = 20;
                if (memcmp(token, "Admin-Level-/gns", 16) == 0)							cReadMode = 21;
                if (memcmp(token, "Admin-Level-/setattackmode", 26) == 0)				cReadMode = 22;
                if (memcmp(token, "Admin-Level-/unsummonall", 24) == 0)					cReadMode = 23;
                if (memcmp(token, "Admin-Level-/unsummondemon", 26) == 0)				cReadMode = 24;
                if (memcmp(token, "Admin-Level-/summonnpc", 22) == 0)					cReadMode = 25;
                if (memcmp(token, "Admin-Level-/summonall", 22) == 0)					cReadMode = 26;
                if (memcmp(token, "Admin-Level-/summonplayer", 25) == 0)				cReadMode = 27;
                if (memcmp(token, "Admin-Level-/disconnectall", 26) == 0)				cReadMode = 28;
                if (memcmp(token, "Admin-Level-/enableadmincreateitem", 34) == 0)		cReadMode = 29;
                if (memcmp(token, "Admin-Level-/createitem", 23) == 0)					cReadMode = 30;
                if (memcmp(token, "Admin-Level-/storm", 18) == 0)						cReadMode = 31;
                if (memcmp(token, "Admin-Level-/weather", 20) == 0)						cReadMode = 32;
                if (memcmp(token, "Admin-Level-/setstatus", 22) == 0)					cReadMode = 33;
                if (memcmp(token, "Admin-Level-/goto", 17) == 0)						cReadMode = 34;
                if (memcmp(token, "Admin-Level-/monstercount", 17) == 0)				cReadMode = 35;
                if (memcmp(token, "Admin-Level-/setforcerecalltime", 23) == 0)			cReadMode = 36;
                if (memcmp(token, "Admin-Level-/unsummonboss", 25) == 0)				cReadMode = 37;
                if (memcmp(token, "Admin-Level-/clearnpc", 21) == 0)					cReadMode = 38;
                if (memcmp(token, "Admin-Level-/time", 17) == 0)						cReadMode = 39;
                if (memcmp(token, "Admin-Level-/send", 17) == 0)						cReadMode = 40;
                if (memcmp(token, "Admin-Level-/summonguild", 24) == 0)					cReadMode = 41;
                if (memcmp(token, "Admin-Level-/checkstatus", 24) == 0)					cReadMode = 42;
                if (memcmp(token, "Admin-Level-/clearmap", 21) == 0)					cReadMode = 43;
            }

            token = pStrTok->pGet();
        }
        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadAdminListConfigFile(char * pFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadModeA, cReadModeB;
    char seps[] = "= \t\n";
    CStrTok * pStrTok;
    int   iIndex;
    std::size_t len;

    cReadModeA = 0;
    cReadModeB = 0;
    iIndex = 0;

    hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(pFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open AdminList.cfg file.");
        return false;
    }
    else
    {
        log->info("(!) Reading AdminList.cfg...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();

        while (token != 0)
        {
            if (cReadModeA != 0)
            {
                switch (cReadModeA)
                {
                    case 1:
                        if (iIndex >= DEF_MAXADMINS)
                        {
                            log->info("(!) WARNING! Too many GMs on the AdminList.cfg!");
                            return true;
                        }
                        len = strlen(token);
                        if (len > 10) len = 10;
                        memset(m_stAdminList[iIndex].m_cGMName, 0, sizeof(m_stAdminList[iIndex].m_cGMName));
                        memcpy(m_stAdminList[iIndex].m_cGMName, token, strlen(token));
                        iIndex++;
                        cReadModeA = 0;
                        cReadModeB = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "verified-admin", 14) == 0)
                {
                    cReadModeA = 1;
                    cReadModeB = 1;
                }
            }

            token = pStrTok->pGet();
        }
        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadBannedListConfigFile(char * pFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadModeA, cReadModeB;
    char seps[] = "= \t\n";
    CStrTok * pStrTok;
    int   iIndex;
    char len;

    cReadModeA = 0;
    cReadModeB = 0;
    iIndex = 0;

    hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(pFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open BannedList.cfg file.");
        return false;
    }
    else
    {
        log->info("(!) Reading BannedList.cfg...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();

        while (token != 0)
        {
            if (cReadModeA != 0)
            {
                switch (cReadModeA)
                {
                    case 1:
                        if (iIndex >= DEF_MAXBANNED)
                        {
                            log->info("(!) WARNING! Too many banned on the BannedList.cfg!");
                            return true;
                        }
                        len = strlen(token);
                        if (len > 20) len = 20;
                        memset(m_stBannedList[iIndex].m_cBannedIPaddress, 0, sizeof(m_stBannedList[iIndex].m_cBannedIPaddress));
                        memcpy(m_stBannedList[iIndex].m_cBannedIPaddress, token, strlen(token));
                        iIndex++;
                        cReadModeA = 0;
                        cReadModeB = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "banned-ip", 9) == 0)
                {
                    cReadModeA = 1;
                    cReadModeB = 1;
                }
            }

            token = pStrTok->pGet();
        }
        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadSettingsConfigFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadMode, cTxt[120], cGSMode[16] = "", len;
    char seps[] = "= \t\n";
    CStrTok * pStrTok;

    cReadMode = 0;

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open configuration file.");
        return false;
    }
    else
    {
        log->info("(!) Reading settings file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();
        while (token != 0)
        {

            if (cReadMode != 0)
            {
                switch (cReadMode)
                {

                    case 1:
                        if ((strlen(token) != 0) && (strlen(token) <= 10000) && (strlen(token) >= 1))
                        {
                            m_iPrimaryDropRate = atoi(token);
                        }
                        else
                        {
                            wsprintf(cTxt, "(!!!) Primary drop rate(%s) invalid must be between 1 to 10000", token);
                            log->info(cTxt);
                        }
                        wsprintf(cTxt, "(*) Primary drop rate: (%d)", m_iPrimaryDropRate);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 2:
                        if ((strlen(token) != 0) && (strlen(token) <= 10000) && (strlen(token) >= 1))
                        {
                            m_iSecondaryDropRate = atoi(token);
                        }
                        else
                        {
                            wsprintf(cTxt, "(!!!) Secondary drop rate(%s) invalid must be between 1 to 10000", token);
                            log->info(cTxt);
                        }
                        wsprintf(cTxt, "(*) Secondary drop rate: (%d)", m_iSecondaryDropRate);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 3:
                        if ((memcmp(token, "classic", 7) == 0) || (memcmp(token, "CLASSIC", 7) == 0))
                        {
                            m_bEnemyKillMode = false;
                            wsprintf(cTxt, "(*) Enemy Kill Mode: CLASSIC");
                            log->info(cTxt);
                        }
                        else if ((memcmp(token, "deathmatch", 10) == 0) || (memcmp(token, "DEATHMATCH", 10) == 0))
                        {
                            m_bEnemyKillMode = true;
                            wsprintf(cTxt, "(*) Enemy Kill Mode: DEATHMATCH");
                            log->info(cTxt);
                        }
                        else
                        {
                            wsprintf(cTxt, "(!!!) ENEMY-KILL-MODE: (%s) must be either DEATHMATCH or CLASSIC", token);
                            log->info(cTxt);
                            return false;
                        }
                        cReadMode = 0;
                        break;

                    case 4:
                        if ((strlen(token) != 0) && (strlen(token) <= 100) && (strlen(token) >= 1))
                        {
                            m_iEnemyKillAdjust = atoi(token);
                        }
                        else
                        {
                            wsprintf(cTxt, "(!!!) ENEMY-KILL-ADJUST: (%s) invalid must be between 1 to 100", token);
                            log->info(cTxt);
                        }
                        wsprintf(cTxt, "(*) Player gains (%d) EK per enemy kill.", m_iEnemyKillAdjust);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 5:
                        if ((memcmp(token, "on", 2) == 0) || (memcmp(token, "ON", 2) == 0))
                        {
                            m_bAdminSecurity = true;
                            wsprintf(cTxt, "(*) Administator actions limited through security.");
                            log->info(cTxt);
                        }
                        if ((memcmp(token, "off", 3) == 0) || (memcmp(token, "OFF", 3) == 0))
                        {
                            m_bAdminSecurity = false;
                            wsprintf(cTxt, "(*) Administator vulnerability ignored.");
                            log->info(cTxt);
                        }

                        cReadMode = 0;
                        break;

                    case 6:
                        m_sRaidTimeMonday = atoi(token);
                        if (m_sRaidTimeMonday == 0) m_sRaidTimeMonday = 3;
                        wsprintf(cTxt, "(*) Monday Raid Time : (%d) minutes", m_sRaidTimeMonday);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 7:
                        m_sRaidTimeTuesday = atoi(token);
                        if (m_sRaidTimeTuesday == 0) m_sRaidTimeTuesday = 3;
                        wsprintf(cTxt, "(*) Tuesday Raid Time : (%d) minutes", m_sRaidTimeTuesday);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 8:
                        m_sRaidTimeWednesday = atoi(token);
                        if (m_sRaidTimeWednesday == 0) m_sRaidTimeWednesday = 3;
                        wsprintf(cTxt, "(*) Wednesday Raid Time : (%d) minutes", m_sRaidTimeWednesday);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 9:
                        m_sRaidTimeThursday = atoi(token);
                        if (m_sRaidTimeThursday == 0) m_sRaidTimeThursday = 3;
                        wsprintf(cTxt, "(*) Thursday Raid Time : (%d) minutes", m_sRaidTimeThursday);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 10:
                        m_sRaidTimeFriday = atoi(token);
                        if (m_sRaidTimeFriday == 0) m_sRaidTimeFriday = 10;
                        wsprintf(cTxt, "(*) Friday Raid Time : (%d) minutes", m_sRaidTimeFriday);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 11:
                        m_sRaidTimeSaturday = atoi(token);
                        if (m_sRaidTimeSaturday == 0) m_sRaidTimeSaturday = 45;
                        wsprintf(cTxt, "(*) Saturday Raid Time : (%d) minutes", m_sRaidTimeSaturday);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 12:
                        m_sRaidTimeSunday = atoi(token);
                        if (m_sRaidTimeSunday == 0) m_sRaidTimeSunday = 60;
                        wsprintf(cTxt, "(*) Sunday Raid Time : (%d) minutes", m_sRaidTimeSunday);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 13:
                        if ((memcmp(token, "player", 6) == 0) || (memcmp(token, "PLAYER", 6) == 0))
                        {
                            m_bLogChatOption = 1;
                            wsprintf(cTxt, "(*) Logging Player Chat.");
                            log->info(cTxt);
                        }
                        if ((memcmp(token, "gm", 2) == 0) || (memcmp(token, "GM", 2) == 0))
                        {
                            m_bLogChatOption = 2;
                            wsprintf(cTxt, "(*) Logging GM Chat.");
                            log->info(cTxt);
                        }
                        if ((memcmp(token, "all", 3) == 0) || (memcmp(token, "ALL", 3) == 0))
                        {
                            m_bLogChatOption = 3;
                            wsprintf(cTxt, "(*) Logging All Chat.");
                            log->info(cTxt);
                        }
                        if ((memcmp(token, "none", 4) == 0) || (memcmp(token, "NONE", 4) == 0))
                        {
                            m_bLogChatOption = 4;
                            wsprintf(cTxt, "(*) Not Logging Any Chat.");
                            log->info(cTxt);
                        }
                        cReadMode = 0;
                        break;

                    case 14:
                        m_iSummonGuildCost = atoi(token);
                        wsprintf(cTxt, "(*) Summoning guild costs (%d) gold", m_iSummonGuildCost);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 15:
                        m_sSlateSuccessRate = atoi(token);
                        wsprintf(cTxt, "(*) Slate Success Rate (%d) percent", m_sSlateSuccessRate);
                        log->info(cTxt);
                        if (m_sSlateSuccessRate == 0) m_sSlateSuccessRate = 14;
                        cReadMode = 0;
                        break;

                    case 16:
                        m_sCharStatLimit = atoi(token);
                        if (m_sCharStatLimit == 0) m_sCharStatLimit = 200;
                        cReadMode = 0;
                        break;

                    case 17:
                        m_sCharSkillLimit = atoi(token);
                        if (m_sCharSkillLimit == 0) m_sCharSkillLimit = 700;
                        cReadMode = 0;
                        break;

                    case 18:
                        m_cRepDropModifier = atoi(token);
                        wsprintf(cTxt, "(*) Rep<->Drop modifier: (%d)", m_cRepDropModifier);
                        log->info(cTxt);
                        if (m_cRepDropModifier < 0) m_cRepDropModifier = 0;
                        cReadMode = 0;
                        break;

                    case 19:
                        memset(m_cSecurityNumber, 0, sizeof(m_cSecurityNumber));
                        len = strlen(token);
                        if (len > 10) len = 10;
                        memcpy(m_cSecurityNumber, token, len);
                        wsprintf(cTxt, "(*) Security number memorized!");
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 20:
                        m_sMaxPlayerLevel = atoi(token);
                        if (m_sMaxPlayerLevel == 0) m_sMaxPlayerLevel = 180;
                        cReadMode = 0;
                        break;

                }
            }
            else
            {
                if (memcmp(token, "primary-drop-rate", 17) == 0)   cReadMode = 1;
                if (memcmp(token, "secondary-drop-rate", 19) == 0)   cReadMode = 2;
                if (memcmp(token, "enemy-kill-mode", 15) == 0)   cReadMode = 3;
                if (memcmp(token, "enemy-kill-adjust", 17) == 0)   cReadMode = 4;
                if (memcmp(token, "admin-security", 14) == 0)   cReadMode = 5;
                if (memcmp(token, "monday-raid-time", 16) == 0)   cReadMode = 6;
                if (memcmp(token, "tuesday-raid-time", 17) == 0)   cReadMode = 7;
                if (memcmp(token, "wednesday-raid-time", 19) == 0)   cReadMode = 8;
                if (memcmp(token, "thursday-raid-time", 18) == 0)   cReadMode = 9;
                if (memcmp(token, "friday-raid-time", 16) == 0)   cReadMode = 10;
                if (memcmp(token, "saturday-raid-time", 18) == 0)   cReadMode = 11;
                if (memcmp(token, "sunday-raid-time", 16) == 0)   cReadMode = 12;
                if (memcmp(token, "log-chat-settings", 17) == 0)		cReadMode = 13;
                if (memcmp(token, "summonguild-cost", 16) == 0)		cReadMode = 14;
                if (memcmp(token, "slate-success-rate", 18) == 0)		cReadMode = 15;
                if (memcmp(token, "character-stat-limit", 20) == 0)		cReadMode = 16;
                if (memcmp(token, "character-skill-limit", 21) == 0)		cReadMode = 17;
                if (memcmp(token, "rep-drop-modifier", 17) == 0)		cReadMode = 18;
                if (memcmp(token, "admin-security-code", 19) == 0)		cReadMode = 19;
                if (memcmp(token, "max-player-level", 16) == 0)		cReadMode = 20;
            }

            token = pStrTok->pGet();
        }

        delete pStrTok;
        delete[] cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadCrusadeStructureConfigFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadModeA, cReadModeB;
    char seps[] = "= \t\n";
    int   iIndex;
    CStrTok * pStrTok;

    cReadModeA = 0;
    cReadModeB = 0;

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open Crusade configuration file.");
        return false;
    }
    else
    {
        log->info("(!) Reading Crusade configuration file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        iIndex = 0;
        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();
        while (token != 0)
        {
            if (cReadModeA != 0)
            {
                switch (cReadModeA)
                {
                    case 1:
                        switch (cReadModeB)
                        {
                            case 1:
                                if (_bGetIsStringIsNumber(token) == false)
                                {
                                    log->info("(!!!) CRITICAL ERROR!  Crusade configuration file error - Wrong Data format(1).");
                                    delete cp;
                                    delete pStrTok;
                                    return false;
                                }
                                iIndex = atoi(token);

                                if (m_stCrusadeStructures[iIndex].cType != 0)
                                {
                                    log->info("(!!!) CRITICAL ERROR!  Crusade configuration file error - Duplicate portion number.");
                                    delete cp;
                                    delete pStrTok;
                                    return false;
                                }

                                cReadModeB = 2;
                                break;

                            case 2:
                                memset(m_stCrusadeStructures[iIndex].cMapName, 0, sizeof(m_stCrusadeStructures[iIndex].cMapName));
                                memcpy(m_stCrusadeStructures[iIndex].cMapName, token, strlen(token));
                                cReadModeB = 3;
                                break;


                            case 3:
                                if (_bGetIsStringIsNumber(token) == false)
                                {
                                    log->info("(!!!) CRITICAL ERROR!  Crusade configuration file error - Wrong Data format.");
                                    delete cp;
                                    delete pStrTok;
                                    return false;
                                }
                                m_stCrusadeStructures[iIndex].cType = atoi(token);
                                cReadModeB = 4;
                                break;

                            case 4:
                                if (_bGetIsStringIsNumber(token) == false)
                                {
                                    log->info("(!!!) CRITICAL ERROR!  Crusade configuration file error - Wrong Data format.");
                                    delete cp;
                                    delete pStrTok;
                                    return false;
                                }
                                m_stCrusadeStructures[iIndex].dX = atoi(token);
                                cReadModeB = 5;
                                break;

                            case 5:
                                if (_bGetIsStringIsNumber(token) == false)
                                {
                                    log->info("(!!!) CRITICAL ERROR!  Crusade configuration file error - Wrong Data format.");
                                    delete cp;
                                    delete pStrTok;
                                    return false;
                                }
                                m_stCrusadeStructures[iIndex].dY = atoi(token);
                                cReadModeA = 0;
                                cReadModeB = 0;
                                break;
                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {
                if (memcmp(token, "crusade-structure", 17) == 0)
                {
                    cReadModeA = 1;
                    cReadModeB = 1;
                }

            }
            token = pStrTok->pGet();
        }

        delete pStrTok;
        delete cp;

        if ((cReadModeA != 0) || (cReadModeB != 0))
        {
            log->info("(!!!) CRITICAL ERROR! Crusade Structure configuration file contents error!");
            return false;
        }
    }

    if (pFile != 0) fclose(pFile);
    return true;
}

bool CGame::bReadNotifyMsgListFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadMode;
    char seps[] = "=\t\n;";
    CStrTok * pStrTok;
    int   i{};

    cReadMode = 0;
    m_iTotalNoticeMsg = 0;

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Notify Message list file not found!...");
        return false;
    }
    else
    {
        log->info("(!) Reading Notify Message list file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();
        while (token != 0)
        {

            if (cReadMode != 0)
            {
                switch (cReadMode)
                {
                    case 1:
                    for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
//                         if (m_pNoticeMsgList[i] == 0)
//                         {
//                             m_pNoticeMsgList[i] = new CMsg;
//                             m_pNoticeMsgList[i]->bPut(NULL, token, strlen(token), 0, 0);
//                             m_iTotalNoticeMsg++;
//                             goto LNML_NEXTSTEP1;
//                         }
                        LNML_NEXTSTEP1:;
                        cReadMode = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "notify_msg", 10) == 0) cReadMode = 1;
            }

            token = pStrTok->pGet();
        }

        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadCrusadeGUIDFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadMode;
    char seps[] = "= \t\n";
    CStrTok * pStrTok;

    cReadMode = 0;

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open CrusadeGUID file.");
        return false;
    }
    else
    {
        log->info("(!) Reading CrusadeGUID file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();

        while (token != 0)
        {

            if (cReadMode != 0)
            {
                switch (cReadMode)
                {
                    case 1:
                        m_dwCrusadeGUID = atoi(token);
                        wsprintf(G_cTxt, "CrusadeGUID = %d", m_dwCrusadeGUID);
                        log->info(G_cTxt);
                        cReadMode = 0;
                        break;

                    case 2:
                        m_iLastCrusadeWinner = atoi(token);
                        wsprintf(G_cTxt, "CrusadeWinnerSide = %d", m_iLastCrusadeWinner);
                        log->info(G_cTxt);
                        cReadMode = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "CrusadeGUID", 11) == 0) cReadMode = 1;
                if (memcmp(token, "winner-side", 11) == 0) cReadMode = 2;
            }

            token = pStrTok->pGet();
        }

        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadScheduleConfigFile(char * pFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadModeA, cReadModeB;
    char seps[] = "= \t\n";
    CStrTok * pStrTok;
    int   iIndex;

    cReadModeA = 0;
    cReadModeB = 0;
    iIndex = 0;

    hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(pFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open Schedule file.");
        return false;
    }
    else
    {
        log->info("(!) Reading Schedule file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();

        while (token != 0)
        {

            if (cReadModeA != 0)
            {
                switch (cReadModeA)
                {
                    case 1:
                        if (strcmp(m_cServerName, token) == 0)
                        {
                            log->info("(!) Success: This server will schedule crusade total war.");
                            m_bIsCrusadeWarStarter = true;
                        }
                        cReadModeA = 0;
                        break;

                    case 2:
                        if (strcmp(m_cServerName, token) == 0)
                        {
                            log->info("(!) Success: This server will schedule apocalypse.");
                            m_bIsApocalypseStarter = true;
                        }
                        cReadModeA = 0;
                        break;

                    case 3:
                        switch (cReadModeB)
                        {
                            case 1:
                                if (iIndex >= DEF_MAXSCHEDULE)
                                {
                                    log->info("(!) WARNING! Too many crusade war schedule!");
                                    return true;
                                }
                                m_stCrusadeWarSchedule[iIndex].iDay = atoi(token);
                                cReadModeB = 2;
                                break;
                            case 2:
                                m_stCrusadeWarSchedule[iIndex].iHour = atoi(token);
                                cReadModeB = 3;
                                break;
                            case 3:
                                m_stCrusadeWarSchedule[iIndex].iMinute = atoi(token);
                                iIndex++;
                                cReadModeA = 0;
                                cReadModeB = 0;
                                break;
                        }
                        break;

                    case 4:
                        switch (cReadModeB)
                        {
                            case 1:
                                if (iIndex >= DEF_MAXAPOCALYPSE)
                                {
                                    log->info("(!) WARNING! Too many Apocalypse schedule!");
                                    return true;
                                }
                                m_stApocalypseScheduleStart[iIndex].iDay = atoi(token);
                                cReadModeB = 2;
                                break;
                            case 2:
                                m_stApocalypseScheduleStart[iIndex].iHour = atoi(token);
                                cReadModeB = 3;
                                break;
                            case 3:
                                m_stApocalypseScheduleStart[iIndex].iMinute = atoi(token);
                                iIndex++;
                                cReadModeA = 0;
                                cReadModeB = 0;
                                break;
                        }
                        break;

                    case 5:
                        switch (cReadModeB)
                        {
                            case 1:
                                if (iIndex >= DEF_MAXAPOCALYPSE)
                                {
                                    log->info("(!) WARNING! Too many Apocalypse schedule!");
                                    return true;
                                }
                                m_stApocalypseScheduleEnd[iIndex].iDay = atoi(token);
                                cReadModeB = 2;
                                break;
                            case 2:
                                m_stApocalypseScheduleEnd[iIndex].iHour = atoi(token);
                                cReadModeB = 3;
                                break;
                            case 3:
                                m_stApocalypseScheduleEnd[iIndex].iMinute = atoi(token);
                                iIndex++;
                                cReadModeA = 0;
                                cReadModeB = 0;
                                break;
                        }
                        break;

                    case 6:
                        switch (cReadModeB)
                        {
                            case 1:
                                if (iIndex >= DEF_MAXHELDENIAN)
                                {
                                    log->info("(!) WARNING! Too many Heldenians scheduled!");
                                    return true;
                                }
                                m_stHeldenianSchedule[iIndex].iDay = atoi(token);
                                cReadModeB = 2;
                                break;
                            case 2:
                                m_stHeldenianSchedule[iIndex].StartiHour = atoi(token);
                                cReadModeB = 3;
                                break;
                            case 3:
                                m_stHeldenianSchedule[iIndex].StartiMinute = atoi(token);
                                cReadModeB = 4;
                                break;
                            case 4:
                                m_stHeldenianSchedule[iIndex].EndiHour = atoi(token);
                                cReadModeB = 5;
                                break;
                            case 5:
                                m_stHeldenianSchedule[iIndex].EndiMinute = atoi(token);
                                iIndex++;
                                cReadModeA = 0;
                                cReadModeB = 0;
                                break;

                        }
                        break;
                }
            }
            else
            {
                if (memcmp(token, "crusade-server-name", 19) == 0)
                {
                    cReadModeA = 1;
                    cReadModeB = 1;
                }
                if (memcmp(token, "apocalypse-server-name", 22) == 0)
                {
                    cReadModeA = 2;
                    cReadModeB = 1;
                }
                if (memcmp(token, "crusade-schedule", 16) == 0)
                {
                    cReadModeA = 3;
                    cReadModeB = 1;
                }
                if (memcmp(token, "apocalypse-schedule-start", 25) == 0)
                {
                    cReadModeA = 4;
                    cReadModeB = 1;
                }
                if (memcmp(token, "apocalypse-schedule-end", 23) == 0)
                {
                    cReadModeA = 5;
                    cReadModeB = 1;
                }
                if (memcmp(token, "Heldenian-schedule", 18) == 0)
                {
                    cReadModeA = 6;
                    cReadModeB = 1;
                }
            }

            token = pStrTok->pGet();
        }
        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadApocalypseGUIDFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadMode;
    char seps[] = "= \t\n";
    CStrTok * pStrTok;

    cReadMode = 0;

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open ApocalypseGUID file.");
        return false;
    }
    else
    {
        log->info("(!) Reading ApocalypseGUID file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();

        while (token != 0)
        {

            if (cReadMode != 0)
            {
                switch (cReadMode)
                {
                    case 1:
                        m_dwApocalypseGUID = atoi(token);
                        wsprintf(G_cTxt, "ApocalypseGUID = %d", m_dwApocalypseGUID);
                        log->info(G_cTxt);
                        cReadMode = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "ApocalypseGUID", 14) == 0) cReadMode = 1;
            }

            token = pStrTok->pGet();
        }

        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}

bool CGame::bReadHeldenianGUIDFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    uint32_t  dwFileSize;
    char * cp, * token, cReadMode;
    char seps[] = "= \t\n";
    CStrTok * pStrTok;

    cReadMode = 0;

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    dwFileSize = GetFileSize(hFile, 0);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        log->info("(!) Cannot open HeldenianGUID file.");
        return false;
    }
    else
    {
        log->info("(!) Reading HeldenianGUID file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();

        while (token != 0)
        {

            if (cReadMode != 0)
            {
                switch (cReadMode)
                {
                    case 1:
                        m_dwHeldenianGUID = atoi(token);
                        wsprintf(G_cTxt, "HeldenianGUID = %d", m_dwHeldenianGUID);
                        log->info(G_cTxt);
                        cReadMode = 0;
                        break;
                    case 2:
                        m_sLastHeldenianWinner = atoi(token);
                        wsprintf(G_cTxt, "HeldenianWinnerSide = %d", m_sLastHeldenianWinner);
                        log->info(G_cTxt);
                        cReadMode = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "HeldenianGUID", 13) == 0) cReadMode = 1;
                if (memcmp(token, "winner-side", 11) == 0) cReadMode = 2;
            }

            token = pStrTok->pGet();
        }

        delete pStrTok;
        delete cp;
    }
    if (pFile != 0) fclose(pFile);

    return true;
}


bool CGame::__bReadMapInfo(int iMapIndex)
{
    char * pContents, * token, * pTile, cTxt[250], cFn[255];
    char seps[] = "= \t\n";
    char cReadModeA = 0;
    char cReadModeB = 0;
    int  iTeleportLocIndex = 0;
    int  iWayPointCfgIndex = 0;
    int  iTotalNpcSetting = 0;
    int  iMGARCfgIndex = 0;
    int  iSMGRCfgIndex = 0;
    int  iNMRCfgIndex = 0;
    int  iFishPointIndex = 0;
    int  iMineralPointIndex = 0;
    int  iStrategicPointIndex = 0;
    int  iIndex = 0;

    int  iNamingValue;
    CStrTok * pStrTok;
    HANDLE hFile;
    uint32_t  dwFileSize, dwReadSize;
    FILE * pFile;

    char cName[6], cNpcName[30], cNpcMoveType, cNpcWaypointIndex[10]{}, cNamePrefix;
    short sIPindex, dX, dY;

    if (memcmp(m_pMapList[iMapIndex]->m_cName, "fightzone", 9) == 0)
        m_pMapList[iMapIndex]->m_bIsFightZone = true;

    if (memcmp(m_pMapList[iMapIndex]->m_cName, "icebound", 8) == 0)
        m_pMapList[iMapIndex]->m_bIsSnowEnabled = true;

    memset(cFn, 0, sizeof(cFn));
    strcat(cFn, "mapdata\\");
    strcat(cFn, m_pMapList[iMapIndex]->m_cName);
    strcat(cFn, ".txt");

    hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    dwFileSize = GetFileSize(hFile, 0);
    CloseHandle(hFile);


    pContents = new char[dwFileSize + 1];
    memset(pContents, 0, dwFileSize + 1);

    pFile = fopen(cFn, "rt");
    if (pFile == 0)
    {
        wsprintf(cTxt, "(!) Cannot open file : %s", cFn);
        log->info(cTxt);
        return false;
    }
    else
    {
        wsprintf(cTxt, "(!) Reading Map info file : %s", cFn);
        log->info(cTxt);
        dwReadSize = fread(pContents, dwFileSize, 1, pFile);
        fclose(pFile);
    }

    pStrTok = new CStrTok(pContents, seps);
    token = pStrTok->pGet();
    while (token != 0)
    {
        if (cReadModeA != 0)
        {
            switch (cReadModeA)
            {
                case 1:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 1 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sSrcX = atoi(token);
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 2 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sSrcY = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            ZeroMemory(m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDestMapName,
                                sizeof(m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDestMapName));
                            strcpy(m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDestMapName, token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 3 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sDestX = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 4 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sDestY = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 5 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDir = atoi(token);
                            iTeleportLocIndex++;
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 2:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 6 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iWayPointCfgIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_WaypointList[iWayPointCfgIndex].x != -1)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 7 - Duplicated waypoint");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 8 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_WaypointList[iWayPointCfgIndex].x = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 9 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_WaypointList[iWayPointCfgIndex].y = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 3:
                    switch (cReadModeB)
                    {
                        case 1:
                            memset(cNpcName, 0, sizeof(cNpcName));
                            strcpy(cNpcName, token);
                            cReadModeB = 2;
                            break;
                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 10 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            cNpcMoveType = atoi(token);
                            cReadModeB = 3;
                            break;
                        default:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 11 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            cNpcWaypointIndex[cReadModeB - 3] = atoi(token);
                            cReadModeB++;
                            break;
                        case 13:
                            cNamePrefix = token[0];

                            iNamingValue = m_pMapList[iMapIndex]->iGetEmptyNamingValue();
                            if (iNamingValue == -1)
                            {
                            }
                            else
                            {
                                memset(cName, 0, sizeof(cName));
                                wsprintf(cName, "XX%d", iNamingValue);
                                cName[0] = cNamePrefix;
                                cName[1] = iMapIndex + 65;

                                if (bCreateNewNpc(cNpcName, cName, m_pMapList[iMapIndex]->m_cName, 0, 0, cNpcMoveType, 0, 0, cNpcWaypointIndex, 0, 0, -1, false) == false)
                                {
                                    m_pMapList[iMapIndex]->SetNamingValueEmpty(iNamingValue);
                                }
                            }
                            cReadModeA = 0;
                            cReadModeB = 0;
                            iTotalNpcSetting++;
                            break;
                    }
                    break;

                case 4:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 12 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_bRandomMobGenerator = (BOOL)atoi(token);
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 13 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_cRandomMobGeneratorLevel = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 5:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error 14 - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }

                    m_pMapList[iMapIndex]->m_iMaximumObject = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 6:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 15 - Wrong Data format(MGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iMGARCfgIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].left != -1)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 16 - Duplicated Mob Gen Rect Number!");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 17 - Wrong Data format(MGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].left = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 18 - Wrong Data format(MGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].top = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 19 - Wrong Data format(MGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].right = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 20 - Wrong Data format(MGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].bottom = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 7:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 21 - Wrong Data format(MGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iSMGRCfgIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].bDefined == true)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error - ");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            cReadModeB = 2;
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].bDefined = true;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 22 - Wrong Data format(SMGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cType = atoi(token);

                            if (m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cType == 1)
                                cReadModeB = 3;
                            else if (m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cType == 2)
                                cReadModeB = 9;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 23 - Wrong Data format(SMGAR num).");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.left = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 24 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.top = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 25 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.right = atoi(token);
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 26 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.bottom = atoi(token);
                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 27 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMobType = atoi(token);
                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 28 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMaxMobs = atoi(token);
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iCurMobs = 0;
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;

                        default:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 29 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cWaypoint[cReadModeB - 9] = atoi(token);
                            cReadModeB++;
                            break;

                        case 19:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 30 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMobType = atoi(token);
                            cReadModeB = 20;
                            break;

                        case 20:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 31 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMaxMobs = atoi(token);
                            m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iCurMobs = 0;
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 8:
                    memset(m_pMapList[iMapIndex]->m_cLocationName, 0, sizeof(m_pMapList[iMapIndex]->m_cLocationName));
                    memcpy(m_pMapList[iMapIndex]->m_cLocationName, token, 10);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 9:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 32:1 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            sIPindex = atoi(token);
                            if (m_pMapList[iMapIndex]->m_pInitialPoint[sIPindex].x != -1)
                            {
                                log->info("(!!!) CRITICAL ERROR! Duplicate Initial Point Index!");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 32 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_pInitialPoint[sIPindex].x = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 33 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_pInitialPoint[sIPindex].y = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 10:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 34 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iNMRCfgIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].top != -1)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 35 - Duplicate No-Magic-Rect number");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 36 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].left = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 37 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].top = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 38 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].right = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 39 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].bottom = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 11:
                    m_pMapList[iMapIndex]->m_bIsFixedDayMode = (BOOL)atoi(token);
                    if (m_pMapList[iMapIndex]->m_bIsFixedDayMode == true)
                        m_pMapList[iMapIndex]->m_bIsSnowEnabled = false;
                    cReadModeA = 0;
                    break;

                case 12:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 40 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iFishPointIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_FishPointList[iFishPointIndex].x != -1)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 41 - Duplicate FishPoint number");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_iTotalFishPoint++;
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 42 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_FishPointList[iFishPointIndex].x = atoi(token);

                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 43 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_FishPointList[iFishPointIndex].y = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 13:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error 44 - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_iMaxFish = atoi(token);

                    cReadModeA = 0;
                    break;

                case 14:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error 45 - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_cType = atoi(token);

                    cReadModeA = 0;
                    break;

                case 15:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error 46 - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_iLevelLimit = atoi(token);

                    cReadModeA = 0;
                    break;

                case 16:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 47 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_bMineralGenerator = (BOOL)atoi(token);
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 48 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_cMineralGeneratorLevel = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 17:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 49 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iMineralPointIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_MineralPointList[iMineralPointIndex].x != -1)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 50 - Duplicate MineralPoint number");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_iTotalMineralPoint++;
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 51 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_MineralPointList[iMineralPointIndex].x = atoi(token);

                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 52 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_MineralPointList[iMineralPointIndex].y = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 18:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error 53 - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_iMaxMineral = atoi(token);

                    cReadModeA = 0;
                    break;

                case 19:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error 54 - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_iUpperLevelLimit = atoi(token);

                    cReadModeA = 0;
                    break;

                case 20:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 55 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iStrategicPointIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex] != 0)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 56 - Duplicate Strategic Point number");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex] = new CStrategicPoint;
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 57 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iSide = atoi(token);

                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 58 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iValue = atoi(token);

                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 59 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iX = atoi(token);

                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 60 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iY = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 21:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 61 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].cType != 0)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 62 - Duplicate EnergySphereCreation number");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_iTotalEnergySphereCreationPoint++;
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 63 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].cType = atoi(token);

                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 64 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].sX = atoi(token);

                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 65 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].sY = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 22:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 66 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iIndex = atoi(token);

                            if (m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].cResult != 0)
                            {
                                wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Map Info file error 67 - Duplicate EnergySphereGoal number(%d:%d)", iIndex, m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].cResult);
                                log->info(G_cTxt);
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_iTotalEnergySphereGoalPoint++;
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 68 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].cResult = atoi(token);

                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 69 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].aresdenX = atoi(token);

                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 70 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].aresdenY = atoi(token);

                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 71 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].elvineX = atoi(token);

                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 72 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].elvineY = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 23:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 73 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iIndex = atoi(token);

                            if (strlen(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName) != 0)
                            {
                                wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Map Info file error 74 - Duplicate Strike Point number(%d)", iIndex);
                                log->info(G_cTxt);
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 75 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].dX = atoi(token);

                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 76 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].dY = atoi(token);

                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iHP = atoi(token);
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iInitHP = atoi(token);
                            cReadModeB = 5;
                            break;


                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[0] = atoi(token);

                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[0] = atoi(token);

                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[1] = atoi(token);

                            cReadModeB = 8;
                            break;

                        case 8:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[1] = atoi(token);

                            cReadModeB = 9;
                            break;

                        case 9:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[2] = atoi(token);

                            cReadModeB = 10;
                            break;

                        case 10:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[2] = atoi(token);

                            cReadModeB = 11;
                            break;

                        case 11:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[3] = atoi(token);

                            cReadModeB = 12;
                            break;

                        case 12:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[3] = atoi(token);

                            cReadModeB = 13;
                            break;

                        case 13:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[4] = atoi(token);

                            cReadModeB = 14;
                            break;

                        case 14:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[4] = atoi(token);

                            cReadModeB = 15;
                            break;

                        case 15:
                            memset(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName, 0, sizeof(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName));
                            strcpy(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName, token);

                            m_pMapList[iMapIndex]->m_iTotalStrikePoints++;
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;

                    }
                    break;

                case 24:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 78 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iIndex = atoi(token);

                            if (strlen(m_pMapList[iMapIndex]->m_stItemEventList[iIndex].cItemName) != 0)
                            {
                                wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Map Info file error 79 - Duplicate Item-Event number(%d:%s)", iIndex, m_pMapList[iMapIndex]->m_stItemEventList[iIndex].cItemName);
                                log->info(G_cTxt);
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }

                            m_pMapList[iMapIndex]->m_iTotalItemEvents++;
                            cReadModeB = 2;
                            break;

                        case 2:
                            strcpy(m_pMapList[iMapIndex]->m_stItemEventList[iIndex].cItemName, token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 81 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iAmount = atoi(token);

                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 82 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iTotalNum = atoi(token);

                            cReadModeB = 5;
                            break;

                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 83 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iMonth = atoi(token);

                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 83 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iDay = atoi(token);

                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 25:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error 78 - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->sMobEventAmount = atoi(token);
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 26:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseMobGenType - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_iApocalypseMobGenType = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 27:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_iApocalypseBossMobNpcID = atoi(token);
                            cReadModeB = 2;
                            break;
                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sApocalypseBossMobRectX1 = atoi(token);
                            cReadModeB = 3;
                            break;
                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sApocalypseBossMobRectY1 = atoi(token);
                            cReadModeB = 4;
                            break;
                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sApocalypseBossMobRectX2 = atoi(token);
                            cReadModeB = 5;
                            break;
                        case 5:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sApocalypseBossMobRectY2 = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 28:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error DynamicGateType - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_cDynamicGateType = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 29:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sDynamicGateCoordRectX1 = atoi(token);
                            cReadModeB = 2;
                            break;

                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sDynamicGateCoordRectY1 = atoi(token);
                            cReadModeB = 3;
                            break;

                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sDynamicGateCoordRectX2 = atoi(token);
                            cReadModeB = 4;
                            break;

                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sDynamicGateCoordRectY2 = atoi(token);
                            cReadModeB = 5;
                            break;

                        case 5:
                            memcpy(m_pMapList[iMapIndex]->m_cDynamicGateCoordDestMap, token, strlen(token));
                            cReadModeB = 6;
                            break;

                        case 6:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sDynamicGateCoordTgtX = atoi(token);
                            cReadModeB = 7;
                            break;

                        case 7:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_sDynamicGateCoordTgtY = atoi(token);
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 30:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error RecallImpossible -  Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_bIsRecallImpossible = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 31:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error ApocalypseMap -  Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_bIsApocalypseMap = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 32:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error CitizenLimit -  Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_bIsCitizenLimit = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 33:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error CitizenLimit -  Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_bIsHeldenianMap = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 34:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Hedenian tower type id - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stHeldenianTower[iIndex].sTypeID = atoi(token);
                            cReadModeB = 2;
                            break;
                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Hedenian Tower Side - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stHeldenianTower[iIndex].cSide = atoi(token);
                            cReadModeB = 3;
                            break;
                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Hedenian Tower X pos - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stHeldenianTower[iIndex].dX = atoi(token);
                            cReadModeB = 4;
                            break;
                        case 4:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Hedenian Tower Y pos - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            iIndex++;
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 35:
                    if (_bGetIsStringIsNumber(token) == false)
                    {
                        log->info("(!!!) CRITICAL ERROR! Map Info file error Hedenian Map Mode - Wrong Data format.");
                        delete pContents;
                        delete pStrTok;
                        return false;
                    }
                    m_pMapList[iMapIndex]->m_cHeldenianModeMap = atoi(token);
                    cReadModeA = 0;
                    cReadModeB = 0;
                    break;

                case 36:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Hedenian Map Mode - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            dX = atoi(token);
                            cReadModeB = 2;
                            break;
                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Hedenian Map Mode - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            dY = atoi(token);
                            pTile = 0;
                            pTile = (char *)(m_pMapList[iMapIndex]->m_pTile + dX + dY * m_pMapList[iMapIndex]->m_sSizeY);
                            if (pTile == 0)
                            {
                                wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map Info file error HeldenianWinningZone - pTile is Null dx(%d), dy(%d).", dX, dY);
                                log->info(cTxt);
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                case 37:
                    switch (cReadModeB)
                    {
                        case 1:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Heldenian Door Direction - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stHeldenianGateDoor[iIndex].cDir = atoi(token);
                            cReadModeB = 2;
                            break;
                        case 2:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Heldenian Door X pos - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stHeldenianGateDoor[iIndex].dX = atoi(token);
                            cReadModeB = 3;
                            break;
                        case 3:
                            if (_bGetIsStringIsNumber(token) == false)
                            {
                                log->info("(!!!) CRITICAL ERROR! Map Info file error Heldenian Door Y pos - Wrong Data format.");
                                delete pContents;
                                delete pStrTok;
                                return false;
                            }
                            m_pMapList[iMapIndex]->m_stHeldenianGateDoor[iIndex].dY = atoi(token);
                            iIndex++;
                            cReadModeA = 0;
                            cReadModeB = 0;
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            if (memcmp(token, "teleport-loc", 12) == 0)
            {
                m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex] = new CTeleportLoc;
                cReadModeA = 1;
                cReadModeB = 1;
            }

            if (memcmp(token, "waypoint", 8) == 0)
            {
                cReadModeA = 2;
                cReadModeB = 1;
            }

            if (memcmp(token, "npc", 3) == 0)
            {
                cReadModeA = 3;
                cReadModeB = 1;
            }

            if (memcmp(token, "random-mob-generator", 20) == 0)
            {
                cReadModeA = 4;
                cReadModeB = 1;
            }

            if (memcmp(token, "maximum-object", 14) == 0)
                cReadModeA = 5;

            if (memcmp(token, "npc-avoidrect", 13) == 0)
            {
                cReadModeA = 6;
                cReadModeB = 1;
            }

            if (memcmp(token, "spot-mob-generator", 18) == 0)
            {
                cReadModeA = 7;
                cReadModeB = 1;
            }

            if (memcmp(token, "map-location", 12) == 0)
                cReadModeA = 8;

            if (memcmp(token, "initial-point", 13) == 0)
            {
                cReadModeA = 9;
                cReadModeB = 1;
            }

            if (memcmp(token, "no-attack-area", 14) == 0)
            {
                cReadModeA = 10;
                cReadModeB = 1;
            }

            if (memcmp(token, "fixed-dayornight-mode", 21) == 0) cReadModeA = 11;

            if (memcmp(token, "fish-point", 10) == 0)
            {
                cReadModeA = 12;
                cReadModeB = 1;
            }

            if (memcmp(token, "max-fish", 8) == 0)		cReadModeA = 13;
            if (memcmp(token, "type", 4) == 0)			cReadModeA = 14;
            if (memcmp(token, "level-limit", 11) == 0)	cReadModeA = 15;

            if (memcmp(token, "mineral-generator", 17) == 0)
            {
                cReadModeA = 16;
                cReadModeB = 1;
            }

            if (memcmp(token, "mineral-point", 13) == 0)
            {
                cReadModeA = 17;
                cReadModeB = 1;
            }

            if (memcmp(token, "max-mineral", 11) == 0)			cReadModeA = 18;
            if (memcmp(token, "upper-level-limit", 17) == 0)	cReadModeA = 19;
            if (memcmp(token, "strategic-point", 15) == 0)
            {
                cReadModeA = 20;
                cReadModeB = 1;
            }

            if (memcmp(token, "energy-sphere-creation-point", 28) == 0)
            {
                cReadModeA = 21;
                cReadModeB = 1;
            }

            if (memcmp(token, "energy-sphere-goal-point", 24) == 0)
            {
                cReadModeA = 22;
                cReadModeB = 1;
            }

            if (memcmp(token, "strike-point", 12) == 0)
            {
                cReadModeA = 23;
                cReadModeB = 1;
            }

            if (memcmp(token, "item-event", 10) == 0)
            {
                cReadModeA = 24;
                cReadModeB = 1;
            }

            if (memcmp(token, "energy-sphere-auto-creation", 27) == 0)
            {
                cReadModeA = 0;
                cReadModeB = 0;
                m_pMapList[iMapIndex]->m_bIsEnergySphereAutoCreation = true;
            }

            if (memcmp(token, "mobevent-amount", 15) == 0)
            {
                cReadModeA = 25;
                cReadModeB = 1;
            }

            if (memcmp(token, "ApocalypseMobGenType", 20) == 0)
            {
                cReadModeA = 26;
                cReadModeB = 1;
            }

            if (memcmp(token, "ApocalypseBossMob", 17) == 0)
            {
                cReadModeA = 27;
                cReadModeB = 1;
            }

            if (memcmp(token, "DynamicGateType", 15) == 0)
            {
                cReadModeA = 28;
                cReadModeB = 1;
            }

            if (memcmp(token, "DynamicGateCoord", 16) == 0)
            {
                cReadModeA = 29;
                cReadModeB = 1;
            }

            if (memcmp(token, "RecallImpossible", 16) == 0)
            {
                cReadModeA = 30;
                cReadModeB = 1;
            }

            if (memcmp(token, "ApocalypseMap", 13) == 0)
            {
                cReadModeA = 31;
                cReadModeB = 1;
            }

            if (memcmp(token, "CitizenLimit", 12) == 0)
            {
                cReadModeA = 32;
                cReadModeB = 1;
            }

            if (memcmp(token, "HeldenianMap", 12) == 0)
            {
                cReadModeA = 33;
                cReadModeB = 1;
            }

            if (memcmp(token, "HeldenianTower", 14) == 0)
            {
                cReadModeA = 34;
                cReadModeB = 1;
            }

            if (memcmp(token, "HeldenianModeMap", 16) == 0)
            {
                cReadModeA = 35;
                cReadModeB = 1;
            }

            if (memcmp(token, "HeldenianWinningZone", 20) == 0)
            {
                cReadModeA = 36;
                cReadModeB = 1;
            }

            if (memcmp(token, "HeldenianGateDoor", 17) == 0)
            {
                cReadModeA = 37;
                cReadModeB = 1;
            }

            if (memcmp(token, "[END-MAP-INFO]", 14) == 0)
            {
                cReadModeA = 0;
                cReadModeB = 0;
                goto RMI_SKIPDECODING;
            }
        }
        token = pStrTok->pGet();
    }

    RMI_SKIPDECODING:;

    delete pStrTok;
    delete pContents;

    if ((cReadModeA != 0) || (cReadModeB != 0))
    {
        log->info("(!!!) CRITICAL ERROR! map info file contents error!");
        return false;
    }

    wsprintf(cTxt, "(!) Map info file decoding(%s) - success! TL(%d) WP(%d) LNPC(%d) MXO(%d) RMG(%d / %d)", cFn, iTeleportLocIndex, iWayPointCfgIndex, iTotalNpcSetting, m_pMapList[iMapIndex]->m_iMaximumObject, m_pMapList[iMapIndex]->m_bRandomMobGenerator, m_pMapList[iMapIndex]->m_cRandomMobGeneratorLevel);
    log->info(cTxt);

    m_pMapList[iMapIndex]->_SetupNoAttackArea();

    return true;
}


bool CGame::bReadProgramConfigFile(char * cFn)
{
    FILE * pFile;
    HANDLE hFile;
    DWORD  dwFileSize;
    char * cp, * token, cReadMode, cTxt[120], cGSMode[16] = "";
    char seps[] = "= \t\n";
    CStrTok * pStrTok;
    bool bGateDNS = false;
    bool bLogDNS = false;

    cReadMode = 0;

    hFile = CreateFile(cFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
    dwFileSize = GetFileSize(hFile, NULL);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    pFile = fopen(cFn, "rt");
    if (pFile == NULL)
    {
        log->info("(!) Cannot open configuration file.");
        return FALSE;
    }
    else
    {
        log->info("(!) Reading configuration file...");
        cp = new char[dwFileSize + 2];
        memset(cp, 0, dwFileSize + 2);
        fread(cp, dwFileSize, 1, pFile);

        pStrTok = new CStrTok(cp, seps);
        token = pStrTok->pGet();
        while (token != NULL)
        {

            if (cReadMode != 0)
            {
                switch (cReadMode)
                {
                    case 1:
                        ZeroMemory(m_cServerName, sizeof(m_cServerName));
                        if (strlen(token) > 10)
                        {
                            wsprintf(cTxt, "(!!!) Game server name(%s) must within 10 chars!", token);
                            log->info(cTxt);
                            return false;
                        }
                        strcpy(m_cServerName, token);
                        wsprintf(cTxt, "(*) Game server name : %s", m_cServerName);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 3:
                        m_iGameServerPort = atoi(token);
                        wsprintf(cTxt, "(*) Game server port : %d", m_iGameServerPort);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;

                    case 5:
                        if (strlen(token) > 10)
                        {
                            wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map name(%s) must within 10 chars!", token);
                            log->info(cTxt);
                            return false;
                        }
                        if (_bRegisterMap(token) == false)
                        {
                            return false;
                        }
                        cReadMode = 0;
                        break;

                    case 10:
                        ZeroMemory(m_cGameServerAddr, sizeof(m_cGameServerAddr));
                        if (strlen(token) > 15)
                        {
                            wsprintf(cTxt, "(!!!) Game server address(%s) must within 15 chars!", token);
                            log->info(cTxt);
                            return false;
                        }
                        strcpy(m_cGameServerAddr, token);
                        wsprintf(cTxt, "(*) Game server address : %s", m_cGameServerAddr);
                        log->info(cTxt);
                        cReadMode = 0;
                        break;
                }
            }
            else
            {
                if (memcmp(token, "game-server-name", 16) == 0) cReadMode = 1;
                if (memcmp(token, "game-server-port", 16) == 0) cReadMode = 3;
                if (memcmp(token, "game-server-map", 15) == 0) cReadMode = 5;
                if (memcmp(token, "game-server-address-reported", 28) == 0) cReadMode = 9;
                if (memcmp(token, "game-server-address", 19) == 0) cReadMode = 10;
            }
            token = pStrTok->pGet();
        }
        delete pStrTok;
        delete cp;
    }
    if (pFile != NULL) fclose(pFile);

    return true;
}
