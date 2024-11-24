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
#include "defines.h"
#include "Tile.h"
#include "Map.h"

extern char G_cTxt[512];
extern char	G_cData50000[50000];


extern FILE * pLogFile;
extern HWND	G_hWnd;

using json = nlohmann::json;
using namespace std::chrono;
using namespace std::chrono_literals;

CGame::CGame()
{
    int i, x;

    load_config();

    spdlog::init_thread_pool(8192, 2);

    std::vector<spdlog::sink_ptr> sinks;

    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    sinks.push_back(stdout_sink);

    // daily rotation max 30 days
    auto rotating = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/helbreath.log", 0, 0, false, 30);
    sinks.push_back(rotating);

    log = std::make_shared<spdlog::async_logger>("helbreath", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(log);

    log->set_pattern(log_formatting);
    log->set_level(loglevel);

    m_bIsGameStarted = false;
    m_bIsLogSockAvailable = false;
    m_bIsGateSockAvailable = false;
    m_bIsItemAvailable = false;
    m_bIsBuildItemAvailable = false;
    m_bIsNpcAvailable = false;
    m_bIsMagicAvailable = false;
    m_bIsSkillAvailable = false;
    m_bIsQuestAvailable = false;
    m_bIsPortionAvailable = false;
    m_bIsWLServerAvailable = false;

    memset(m_cServerName, 0, sizeof(m_cServerName));

    m_iPlayerMaxLevel = DEF_PLAYERMAXLEVEL;

    m_sForceRecallTime = 0;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        m_pClientList[i] = 0;

    for (i = 0; i < DEF_MAXMAPS; i++)
        m_pMapList[i] = 0;

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        m_pItemConfigList[i] = 0;

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        m_pNpcConfigList[i] = 0;

    for (i = 0; i < DEF_MAXNPCS; i++)
        m_pNpcList[i] = 0;

    for (i = 0; i < DEF_MSGQUENESIZE; i++)
        m_pMsgQuene[i] = 0;

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        m_pMagicConfigList[i] = 0;

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        m_pSkillConfigList[i] = 0;

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        m_pQuestConfigList[i] = 0;

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        m_pDynamicObjectList[i] = 0;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        m_pDelayEventList[i] = 0;

    for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
        m_pNoticeMsgList[i] = 0;

    for (i = 0; i < DEF_MAXFISHS; i++)
        m_pFish[i] = 0;

    for (i = 0; i < DEF_MAXMINERALS; i++)
        m_pMineral[i] = 0;

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        m_pPortionConfigList[i] = 0;


    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        m_pBuildItemList[i] = 0;

    for (i = 0; i < DEF_MAXDUPITEMID; i++)
        m_pDupItemIDList[i] = 0;

    for (i = 0; i < DEF_MAXPARTYNUM; i++)
        for (x = 0; x < DEF_MAXPARTYMEMBERS; x++)
        {
            m_stPartyInfo[i].iTotalMembers = 0;
            m_stPartyInfo[i].iIndex[x] = 0;
        }

    m_iQueneHead = 0;
    m_iQueneTail = 0;

    m_iTotalClients = 0;
    m_iMaxClients = 0;
    m_iTotalMaps = 0;

    m_iTotalGameServerClients = 0;
    m_iTotalGameServerMaxClients = 0;

    m_MaxUserSysTime.wHour = 0;
    m_MaxUserSysTime.wMinute = 0;

    m_bIsServerShutdowned = false;
    m_cShutDownCode = 0;

    m_iMiddlelandMapIndex = -1;
    m_iAresdenOccupyTiles = 0;
    m_iElvineOccupyTiles = 0;

    m_iCurMsgs = 0;
    m_iMaxMsgs = 0;

    m_stCityStatus[1].iCrimes = 0;
    m_stCityStatus[1].iFunds = 0;
    m_stCityStatus[1].iWins = 0;

    m_stCityStatus[2].iCrimes = 0;
    m_stCityStatus[2].iFunds = 0;
    m_stCityStatus[2].iWins = 0;

    m_pGold = 0;

    m_bReceivedItemList = false;
}

CGame::~CGame()
{

}

bool CGame::bInit()
{
    char * cp, cTxt[120]{};
    uint32_t * dwp;
    uint16_t * wp;
    int  i;
    SYSTEMTIME SysTime;
    uint32_t dwTime = timeGetTime();

    //m_Misc.Temp();

    log->info("(!) INITIALIZING GAME SERVER...");
    //

    for (i = 0; i < DEF_MAXCLIENTS + 1; i++)
        m_iClientShortCut[i] = 0;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != 0) delete m_pClientList[i];

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0) delete m_pNpcList[i];

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0) delete m_pMapList[i];

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != 0) delete m_pItemConfigList[i];

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        if (m_pNpcConfigList[i] != 0) delete m_pNpcConfigList[i];

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        if (m_pMagicConfigList[i] != 0) delete m_pMagicConfigList[i];

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        if (m_pSkillConfigList[i] != 0) delete m_pSkillConfigList[i];

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        if (m_pQuestConfigList[i] != 0) delete m_pQuestConfigList[i];

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != 0) delete m_pDynamicObjectList[i];

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] != 0) delete m_pDelayEventList[i];

    for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
        if (m_pNoticeMsgList[i] != 0) delete m_pNoticeMsgList[i];

    for (i = 0; i < DEF_MAXFISHS; i++)
        if (m_pFish[i] != 0) delete m_pFish[i];

    for (i = 0; i < DEF_MAXMINERALS; i++)
        if (m_pMineral[i] != 0) delete m_pMineral[i];

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        if (m_pPortionConfigList[i] != 0) delete m_pPortionConfigList[i];

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        if (m_pBuildItemList[i] != 0) delete m_pBuildItemList[i];

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        m_iNpcConstructionPoint[i] = 0;

    for (i = 0; i < DEF_MAXSCHEDULE; i++)
    {
        m_stCrusadeWarSchedule[i].iDay = -1;
        m_stCrusadeWarSchedule[i].iHour = -1;
        m_stCrusadeWarSchedule[i].iMinute = -1;
    }


    m_iNpcConstructionPoint[1] = 100; // MS
    m_iNpcConstructionPoint[2] = 100; // MS
    m_iNpcConstructionPoint[3] = 100; // MS
    m_iNpcConstructionPoint[4] = 100; // MS
    m_iNpcConstructionPoint[5] = 100; // MS
    m_iNpcConstructionPoint[6] = 100; // MS

    m_iNpcConstructionPoint[43] = 1000; // LWB
    m_iNpcConstructionPoint[44] = 2000; // GHK
    m_iNpcConstructionPoint[45] = 3000; // GHKABS
    m_iNpcConstructionPoint[46] = 2000; // TK
    m_iNpcConstructionPoint[47] = 3000; // BG

    m_iNpcConstructionPoint[51] = 1500; // Catapult
    //

    m_bIsGameStarted = false;

    m_bIsLogSockAvailable = false;
    m_bIsGateSockAvailable = false;
    m_bIsItemAvailable = false;
    m_bIsBuildItemAvailable = false;
    m_bIsNpcAvailable = false;
    m_bIsMagicAvailable = false;
    m_bIsSkillAvailable = false;
    m_bIsQuestAvailable = false;
    m_bIsPortionAvailable = false;
    m_bIsWLServerAvailable = false;

    memset(m_cServerName, 0, sizeof(m_cServerName));

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        m_pClientList[i] = 0;

    for (i = 0; i < DEF_MAXMAPS; i++)
        m_pMapList[i] = 0;

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        m_pItemConfigList[i] = 0;

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        m_pNpcConfigList[i] = 0;

    for (i = 0; i < DEF_MAXNPCS; i++)
        m_pNpcList[i] = 0;

    for (i = 0; i < DEF_MSGQUENESIZE; i++)
        m_pMsgQuene[i] = 0;

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        m_pMagicConfigList[i] = 0;

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        m_pSkillConfigList[i] = 0;

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        m_pQuestConfigList[i] = 0;

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        m_pDynamicObjectList[i] = 0;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        m_pDelayEventList[i] = 0;

    for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
        m_pNoticeMsgList[i] = 0;

    for (i = 0; i < DEF_MAXFISHS; i++)
        m_pFish[i] = 0;

    for (i = 0; i < DEF_MAXMINERALS; i++)
        m_pMineral[i] = 0;

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        m_pPortionConfigList[i] = 0;

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        m_pBuildItemList[i] = 0;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
    {
        memset(m_stCrusadeStructures[i].cMapName, 0, sizeof(m_stCrusadeStructures[i].cMapName));
        m_stCrusadeStructures[i].cType = 0;
        m_stCrusadeStructures[i].dX = 0;
        m_stCrusadeStructures[i].dY = 0;
    }


    for (i = 0; i < DEF_MAXTELEPORTLIST; i++)
        m_pTeleportConfigList[i] = 0;

    for (i = 0; i < DEF_MAXGUILDS; i++)
        m_pGuildTeleportLoc[i].m_iV1 = 0;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
    {
        m_stMiddleCrusadeStructureInfo[i].cType = 0;
        m_stMiddleCrusadeStructureInfo[i].cSide = 0;
        m_stMiddleCrusadeStructureInfo[i].sX = 0;
        m_stMiddleCrusadeStructureInfo[i].sY = 0;
    }
    m_iTotalMiddleCrusadeStructures = 0;

    m_pNoticementData = 0;

    m_iQueneHead = 0;
    m_iQueneTail = 0;

    m_iTotalClients = 0;
    m_iMaxClients = 0;
    m_iTotalMaps = 0;

    m_iTotalGameServerClients = 0;
    m_iTotalGameServerMaxClients = 0;

    m_MaxUserSysTime.wHour = 0;
    m_MaxUserSysTime.wMinute = 0;

    m_bIsServerShutdowned = false;
    m_cShutDownCode = 0;

    m_iMiddlelandMapIndex = -1;
    m_iAresdenMapIndex = -1;
    m_iElvineMapIndex = -1;
    m_iAresdenOccupyTiles = 0;
    m_iElvineOccupyTiles = 0;

    m_iCurMsgs = 0;
    m_iMaxMsgs = 0;

    m_stCityStatus[1].iCrimes = 0;
    m_stCityStatus[1].iFunds = 0;
    m_stCityStatus[1].iWins = 0;

    m_stCityStatus[2].iCrimes = 0;
    m_stCityStatus[2].iFunds = 0;
    m_stCityStatus[2].iWins = 0;

    m_iStrategicStatus = 0;

    m_iCollectedMana[0] = 0;
    m_iCollectedMana[1] = 0;
    m_iCollectedMana[2] = 0;

    m_iAresdenMana = 0;
    m_iElvineMana = 0;
    //

    m_dwSpecialEventTime = m_dwWhetherTime = m_dwGameTime1 =
        m_dwGameTime2 = m_dwGameTime3 = m_dwGameTime4 = m_dwGameTime5 = m_dwGameTime6 = m_dwFishTime = dwTime;

    m_bIsSpecialEventTime = false;

    GetLocalTime(&SysTime);

    m_dwCanFightzoneReserveTime = dwTime - ((SysTime.wHour % 2) * 60 * 60 + SysTime.wMinute * 60) * 1000;

    for (i = 0; i < DEF_MAXFIGHTZONE; i++)
        m_iFightZoneReserve[i] = 0;

    for (i = 1; i < 199; i++)
    {
        m_iLevelExpTable[i] = iGetLevelExp(i);
    }

    m_iLimitedUserExp = m_iLevelExpTable[DEF_LEVELLIMIT + 1];
    m_iLevelExp20 = m_iLevelExpTable[20];

    if (bReadProgramConfigFile("GServer.cfg") == FALSE)
    {
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! GServer.cfg file contents error!");
        return FALSE;
    }
    if (bReadSettingsConfigFile("GameConfigs\\Settings.cfg") == FALSE)
    {
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! Settings.cfg file contents error!");
        return FALSE;
    }
    if (bReadAdminListConfigFile("GameConfigs\\AdminList.cfg") == FALSE)
    {
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! AdminList.cfg file contents error!");
        return FALSE;
    }
    if (bReadBannedListConfigFile("GameConfigs\\BannedList.cfg") == FALSE)
    {
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! BannedList.cfg file contents error!");
        return FALSE;
    }
    if (bReadAdminSetConfigFile("GameConfigs\\AdminSettings.cfg") == FALSE)
    {
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! AdminSettings.cfg file contents error!");
        return FALSE;
    }

    srand((unsigned)time(0));

    m_bIsGateSockAvailable = true;

    m_bF1pressed = m_bF4pressed = m_bF12pressed = false;

    m_bOnExitProcess = false;

    for (i = 0; i <= 100; i++)
    {
        m_iSkillSSNpoint[i] = _iCalcSkillSSNpoint(i);
    }

    GetLocalTime(&SysTime);
    if (SysTime.wMinute >= DEF_NIGHTTIME)
        m_cDayOrNight = 2;
    else m_cDayOrNight = 1;


    bReadNotifyMsgListFile("notice.txt");
    m_dwNoticeTime = dwTime;

    m_pNoticementData = 0;
    m_dwNoticementDataSize = 0;

    m_dwMapSectorInfoTime = dwTime;
    m_iMapSectorInfoUpdateCount = 0;

    m_iCrusadeCount = 0;
    m_bIsCrusadeMode = false;
    m_wServerID_GSS = (WORD)iDice(1, 65535);

    memset(m_cGateServerStockMsg, 0, sizeof(m_cGateServerStockMsg));
    cp = (char *)m_cGateServerStockMsg;
    dwp = (uint32_t *)cp;
    *dwp = MSGID_SERVERSTOCKMSG;
    cp += 4;
    wp = (uint16_t *)cp;
    *wp = DEF_MSGTYPE_CONFIRM;
    cp += 2;

    m_iIndexGSS = 6;
    m_dwCrusadeGUID = 0;
    m_iCrusadeWinnerSide = 0;
    m_iWinnerSide = 0;
    m_iNonAttackArea = false;
    m_bIsCrusadeWarStarter = false;
    m_iLatestCrusadeDayOfWeek = -1;
    m_iFinalShutdownCount = 0;

    return true;
}

void CGame::DeleteClient(int iClientH, bool bSave, bool bNotify, bool bCountLogout, bool bForceCloseConn)
{
    int i, iExH;
    char cTmpMap[30];

    if (m_pClientList[iClientH] == 0) return;

    auto client = m_pClientList[iClientH]->shared_from_this();

    client_list.erase(client);

    if (m_pClientList[iClientH]->m_bIsInitComplete == true)
    {
        if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 5) == 0)
        {
            wsprintf(G_cTxt, "Char(%s)-Exit(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cMapName);
            log->info(G_cTxt);
        }

        if (m_pClientList[iClientH]->m_bIsExchangeMode == true)
        {
            iExH = m_pClientList[iClientH]->m_iExchangeH;
            _ClearExchangeStatus(iExH);
            _ClearExchangeStatus(iClientH);
        }

        if ((m_pClientList[iClientH]->m_iAllocatedFish != 0) && (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] != 0))
            m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;

        if (bNotify == true)
            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);

        RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iWhisperPlayerIndex == iClientH))
            {
                m_pClientList[i]->m_iWhisperPlayerIndex = -1;

                SendNotifyMsg(NULL, i, DEF_NOTIFY_WHISPERMODEOFF, 0, 0, 0, m_pClientList[iClientH]->m_cCharName);
            }

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(2, iClientH, DEF_OWNERTYPE_PLAYER,
            m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY);

        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, 0);
    }

    if ((bSave == true) && (m_pClientList[iClientH]->m_bIsOnServerChange == false))
    {
        if (m_pClientList[iClientH]->m_bIsKilled == true)
        {
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;

            strcpy(cTmpMap, m_pClientList[iClientH]->m_cMapName);

            memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));

            if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
            {
                strcpy(m_pClientList[iClientH]->m_cMapName, "default");
            }
            else
            {
                if (m_pClientList[iClientH]->m_cSide == DEF_ARESDEN)
                {
                    if (m_bIsCrusadeMode == true)
                    {
                        if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0)
                        {
                            memset(m_pClientList[iClientH]->m_cLockedMapName, 0, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
                            strcpy(m_pClientList[iClientH]->m_cLockedMapName, "aresden");
                            m_pClientList[iClientH]->m_iLockedMapTime = 60 * 5;
                            m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10;
                        }
                        else
                        {
                            m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10;
                        }
                    }
                    if (strcmp(cTmpMap, "elvine") == 0)
                    {
                        strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvjail");
                        m_pClientList[iClientH]->m_iLockedMapTime = 60 * 3;
                        memcpy(m_pClientList[iClientH]->m_cMapName, "elvjail", 7);
                    }
                    else if (m_pClientList[iClientH]->m_iLevel > 80)
                        memcpy(m_pClientList[iClientH]->m_cMapName, "resurr1", 7);
                    else memcpy(m_pClientList[iClientH]->m_cMapName, "arefarm", 7);
                }
                else
                {
                    if (m_bIsCrusadeMode == true)
                    {

                        if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0)
                        {
                            memset(m_pClientList[iClientH]->m_cLockedMapName, 0, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
                            strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvine");
                            m_pClientList[iClientH]->m_iLockedMapTime = 60 * 5;
                            m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10;
                        }
                        else
                        {
                            m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10;
                        }
                    }
                    if (strcmp(cTmpMap, "aresden") == 0)
                    {
                        strcpy(m_pClientList[iClientH]->m_cLockedMapName, "arejail");
                        m_pClientList[iClientH]->m_iLockedMapTime = 60 * 3;
                        memcpy(m_pClientList[iClientH]->m_cMapName, "arejail", 7);

                    }
                    else if (m_pClientList[iClientH]->m_iLevel > 80)
                        memcpy(m_pClientList[iClientH]->m_cMapName, "resurr2", 7);
                    else memcpy(m_pClientList[iClientH]->m_cMapName, "elvfarm", 7);
                }
            }
        }
        else if (bForceCloseConn == true)
        {

            memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
            memcpy(m_pClientList[iClientH]->m_cMapName, "bisle", 5);
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;


            memset(m_pClientList[iClientH]->m_cLockedMapName, 0, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
            strcpy(m_pClientList[iClientH]->m_cLockedMapName, "bisle");

            m_pClientList[iClientH]->m_iLockedMapTime = 10 * 60;
        }


        if (m_pClientList[iClientH]->m_bIsObserverMode == true)
        {
            memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
            if (m_pClientList[iClientH]->m_cSide == 0)
            {

                switch (iDice(1, 2))
                {
                    case 1:
                        memcpy(m_pClientList[iClientH]->m_cMapName, "aresden", 7);
                        break;
                    case 2:
                        memcpy(m_pClientList[iClientH]->m_cMapName, "elvine", 6);
                        break;
                }
            }
            else
            {

                if (m_pClientList[iClientH]->m_cSide == DEF_ARESDEN)
                    memcpy(m_pClientList[iClientH]->m_cMapName, "aresden", 10);
                else
                    memcpy(m_pClientList[iClientH]->m_cMapName, "elvine", 10);

                // memcpy(m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cLocation, 10);
            }
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;
        }

        if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 5) == 0)
        {
            memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
            if (m_pClientList[iClientH]->m_cSide == 0)
            {
                switch (iDice(1, 2))
                {
                    case 1:
                        memcpy(m_pClientList[iClientH]->m_cMapName, "aresden", 7);
                        break;
                    case 2:
                        memcpy(m_pClientList[iClientH]->m_cMapName, "elvine", 6);
                        break;
                }
            }
            else
            {
                if (m_pClientList[iClientH]->m_cSide == DEF_ARESDEN)
                    memcpy(m_pClientList[iClientH]->m_cMapName, "aresden", 7);
                else
                    memcpy(m_pClientList[iClientH]->m_cMapName, "elvine", 6);

                // memcpy(m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cLocation, 10);
            }
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;
        }

        if (m_pClientList[iClientH]->m_bIsInitComplete == true)
        {
            if (!save_player_data(m_pClientList[iClientH]->shared_from_this()))
            {
                log->error("Save player data error for [{}]", m_pClientList[iClientH]->m_cCharName);
            }
        }
    }

    if (m_pClientList[iClientH]->m_iPartyID != 0)
    {

        for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
            if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == iClientH)
            {
                m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = 0;
                m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers--;
                //testcode
                wsprintf(G_cTxt, "PartyID:%d member:%d Out(Delete) Total:%d", m_pClientList[iClientH]->m_iPartyID, iClientH, m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers);
                log->info(G_cTxt);
                goto DC_LOOPBREAK1;
            }
        DC_LOOPBREAK1:;

        for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++)
            if ((m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0) && (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] != 0))
            {
                m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1];
                m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] = 0;
            }
    }

    //delete m_pClientList[iClientH];
    m_pClientList[iClientH] = nullptr;


    RemoveClientShortCut(iClientH);
}

void CGame::SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, uint32_t dwMsgID, uint16_t wMsgType, short sV1, short sV2, short sV3)
{
    int * ip, i, iRet, iShortCutIndex, * ipStatus;
    char * cp_a, * cp_s, * cp_sv, cData_All[200], cData_Srt[200], cData_Srt_Av[200];
    uint32_t * dwp;
    uint16_t * wp;
    short * sp, sRange, sX, sY;
    int	 iTemp, iTemp2, iTemp3, iDumm{};
    bool    bOwnerSend, bFlag;
    char  cKey;

    memset(cData_All, 0, sizeof(cData_All));
    memset(cData_Srt, 0, sizeof(cData_Srt));
    memset(cData_Srt_Av, 0, sizeof(cData_Srt_Av));
    ipStatus = (int *)&iDumm;
    cKey = (char)(rand() % 255) + 1;

    // All-Data
    dwp = (uint32_t *)(cData_All + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (uint16_t *)(cData_All + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;
    // Short-Data
    dwp = (uint32_t *)(cData_Srt + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (uint16_t *)(cData_Srt + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;
    // Short-Data-Additional-value
    dwp = (uint32_t *)(cData_Srt_Av + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (uint16_t *)(cData_Srt_Av + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;

    cp_a = (char *)(cData_All + DEF_INDEX2_MSGTYPE + 2);
    cp_s = (char *)(cData_Srt + DEF_INDEX2_MSGTYPE + 2);
    cp_sv = (char *)(cData_Srt_Av + DEF_INDEX2_MSGTYPE + 2);


    if ((dwMsgID == MSGID_EVENT_LOG) || (wMsgType == DEF_OBJECTMOVE) || (wMsgType == DEF_OBJECTRUN) ||
        (wMsgType == DEF_OBJECTATTACKMOVE) || (wMsgType == DEF_OBJECTDAMAGEMOVE) || (wMsgType == DEF_OBJECTDYING))
        sRange = 1;
    else sRange = 0;

    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
    {

        if (m_pClientList[sOwnerH] == 0) return;


        switch (wMsgType)
        {
            case DEF_OBJECTNULLACTION:
            case DEF_OBJECTDAMAGE:
            case DEF_OBJECTDYING:
                bOwnerSend = true;
                break;
            default:
                bOwnerSend = false;
                break;
        }


        wp = (uint16_t *)cp_a;
        *wp = sOwnerH;
        cp_a += 2;
        sp = (short *)cp_a;
        sX = m_pClientList[sOwnerH]->m_sX;
        *sp = sX;
        cp_a += 2;
        sp = (short *)cp_a;
        sY = m_pClientList[sOwnerH]->m_sY;
        *sp = sY;
        cp_a += 2;
        sp = (short *)cp_a;
        *sp = m_pClientList[sOwnerH]->m_sType;
        cp_a += 2;
        *cp_a = m_pClientList[sOwnerH]->m_cDir;
        cp_a++;
        memcpy(cp_a, m_pClientList[sOwnerH]->m_cCharName, 10);
        cp_a += 10;
        sp = (short *)cp_a;
        *sp = m_pClientList[sOwnerH]->m_sAppr1;
        cp_a += 2;
        sp = (short *)cp_a;
        *sp = m_pClientList[sOwnerH]->m_sAppr2;
        cp_a += 2;
        sp = (short *)cp_a;
        *sp = m_pClientList[sOwnerH]->m_sAppr3;
        cp_a += 2;
        sp = (short *)cp_a;
        *sp = m_pClientList[sOwnerH]->m_sAppr4;
        cp_a += 2;
        // v1.4
        ip = (int *)cp_a;
        *ip = m_pClientList[sOwnerH]->m_iApprColor;
        cp_a += 4;

        ip = (int *)cp_a;
        ipStatus = ip;
        *ip = m_pClientList[sOwnerH]->m_iStatus;
        cp_a += 4;

        iTemp3 = m_pClientList[sOwnerH]->m_iStatus & 0x0F0FFFF7F; //0F0FFFF7Fh

        if (wMsgType == DEF_OBJECTNULLACTION)
        {
            if (m_pClientList[sOwnerH]->m_bIsKilled == true)
                *cp_a = 1;
            else *cp_a = 0;
        }
        else *cp_a = 0;
        cp_a++;

        ip = (int *)cp_a;
        *ip = m_pClientList[sOwnerH]->m_iVit * 3 + m_pClientList[sOwnerH]->m_iLevel * 2 + m_pClientList[sOwnerH]->m_iStr / 2;
        cp_a += 4;

        ip = (int *)cp_a;
        *ip = m_pClientList[sOwnerH]->m_iHP;
        cp_a += 4;

        //iMaxPoint = m_iVit*3 + m_iLevel*2 + m_iStr/2;
        //Change HP Bar

        wp = (uint16_t *)cp_s;
        *wp = sOwnerH + 30000;
        cp_s += 2;

        *cp_s = m_pClientList[sOwnerH]->m_cDir;
        cp_s++;

        *cp_s = (unsigned char)sV1;
        cp_s++;
        *cp_s = (unsigned char)sV2;
        cp_s++;


        sp = (short *)cp_s;
        sX = m_pClientList[sOwnerH]->m_sX;
        *sp = sX;
        cp_s += 2;
        sp = (short *)cp_s;
        sY = m_pClientList[sOwnerH]->m_sY;
        *sp = sY;
        cp_s += 2;

        ip = (int *)cp_s;
        *ip = m_pClientList[sOwnerH]->m_iVit * 3 + m_pClientList[sOwnerH]->m_iLevel * 2 + m_pClientList[sOwnerH]->m_iStr / 2;
        cp_s += 4;

        ip = (int *)cp_s;
        *ip = m_pClientList[sOwnerH]->m_iHP;
        cp_s += 4;

        //iMaxPoint = m_iVit*3 + m_iLevel*2 + m_iStr/2;
        //Change HP Bar


        wp = (uint16_t *)cp_sv;
        *wp = sOwnerH + 30000;
        cp_sv += 2;

        *cp_sv = m_pClientList[sOwnerH]->m_cDir;
        cp_sv++;
        *cp_sv = sV1 - sX;
        cp_sv++;
        *cp_sv = sV2 - sY;
        cp_sv++;
        sp = (short *)cp_sv;
        *sp = sV3;
        cp_sv += 2;

        ip = (int *)cp_sv;
        *ip = m_pClientList[sOwnerH]->m_iVit * 3 + m_pClientList[sOwnerH]->m_iLevel * 2 + m_pClientList[sOwnerH]->m_iStr / 2;
        cp_sv += 4;

        ip = (int *)cp_sv;
        *ip = m_pClientList[sOwnerH]->m_iHP;
        cp_sv += 4;

        //iMaxPoint = m_iVit*3 + m_iLevel*2 + m_iStr/2;
        //Change HP Bar

        //for (i = 1; i < DEF_MAXCLIENTS; i++) {
        bFlag = true;
        iShortCutIndex = 0;
        while (bFlag == true)
        {

            i = m_iClientShortCut[iShortCutIndex];
            iShortCutIndex++;
            if (i == 0) bFlag = false;

            //if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
            if ((bFlag == true) && (m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
                if ((m_pClientList[i]->m_cMapIndex == m_pClientList[sOwnerH]->m_cMapIndex) &&
                    (m_pClientList[i]->m_sX >= m_pClientList[sOwnerH]->m_sX - 10 - sRange) &&
                    (m_pClientList[i]->m_sX <= m_pClientList[sOwnerH]->m_sX + 10 + sRange) &&
                    (m_pClientList[i]->m_sY >= m_pClientList[sOwnerH]->m_sY - 8 - sRange) &&
                    (m_pClientList[i]->m_sY <= m_pClientList[sOwnerH]->m_sY + 8 + sRange))
                {
                    if (m_pClientList[sOwnerH]->m_cSide != m_pClientList[i]->m_cSide)
                    {
                        if (m_pClientList[i]->m_iAdminUserLevel > 0)
                        {
                            iTemp = m_pClientList[sOwnerH]->m_iStatus;
                        }
                        else if (i != sOwnerH)
                        {
                            iTemp = iTemp3;
                        }
                        else
                        {
                            iTemp = m_pClientList[sOwnerH]->m_iStatus;
                        }
                    }
                    else
                    {
                        iTemp = m_pClientList[sOwnerH]->m_iStatus;
                        //iTemp = 0x0FFFFFFF;
                    }

                    //iTemp = m_pClientList[sOwnerH]->m_iStatus;
                    iTemp = 0x0FFFFFFF & iTemp;
                    iTemp2 = iGetPlayerABSStatus(sOwnerH, i);
                    iTemp = (iTemp | (iTemp2 << 28));
                    *ipStatus = iTemp;


                    if ((m_pClientList[i]->m_sX >= m_pClientList[sOwnerH]->m_sX - 9) &&
                        (m_pClientList[i]->m_sX <= m_pClientList[sOwnerH]->m_sX + 9) &&
                        (m_pClientList[i]->m_sY >= m_pClientList[sOwnerH]->m_sY - 7) &&
                        (m_pClientList[i]->m_sY <= m_pClientList[sOwnerH]->m_sY + 7))
                    {

                        switch (wMsgType)
                        {
                            case DEF_MSGTYPE_CONFIRM:
                            case DEF_MSGTYPE_REJECT:
                            case DEF_OBJECTNULLACTION:

                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_All, 43 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_All, 43 + 8, cKey);
                                }
                                break;

                            case DEF_OBJECTMAGIC:
                            case DEF_OBJECTDAMAGE:
                            case DEF_OBJECTDAMAGEMOVE:
                                //case DEF_OBJECTDYING:
                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11 + 8, cKey);
                                }
                                break;

                            case DEF_OBJECTDYING:
                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15 + 8, cKey);
                                }
                                break;

                            case DEF_OBJECTATTACK:
                            case DEF_OBJECTATTACKMOVE:

                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13 + 8, cKey);
                                }
                                break;

                            default:

                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 9 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_Srt, 9 + 8, cKey);
                                }
                                break;
                        }
                    }
                    else
                    {

                        switch (wMsgType)
                        {
                            case DEF_MSGTYPE_CONFIRM:
                            case DEF_MSGTYPE_REJECT:
                            case DEF_OBJECTNULLACTION:

                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_All, 43 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_All, 43 + 8, cKey);
                                }
                                break;

                            case DEF_OBJECTMAGIC:
                            case DEF_OBJECTDAMAGE:
                            case DEF_OBJECTDAMAGEMOVE:
                                //case DEF_OBJECTDYING:	
                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11 + 8, cKey);
                                }
                                break;

                            case DEF_OBJECTDYING:
                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15 + 8, cKey);
                                }
                                break;

                            case DEF_OBJECTATTACK:
                            case DEF_OBJECTATTACKMOVE:

                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13 + 8, cKey);
                                }
                                break;

                            default:

                                if (bOwnerSend == true)
                                    iRet = m_pClientList[i]->iSendMsg(cData_All, 43 + 8, cKey);
                                else
                                {
                                    if (i != sOwnerH)
                                        iRet = m_pClientList[i]->iSendMsg(cData_All, 43 + 8, cKey);
                                }
                                break;
                        }
                    }
                }
        }
    }
    else
    {
        if (m_pNpcList[sOwnerH] == 0) return;

        wp = (uint16_t *)cp_a;
        *wp = sOwnerH + 10000;
        cp_a += 2;
        sp = (short *)cp_a;
        sX = m_pNpcList[sOwnerH]->m_sX;
        *sp = sX;
        cp_a += 2;
        sp = (short *)cp_a;
        sY = m_pNpcList[sOwnerH]->m_sY;
        *sp = sY;
        cp_a += 2;
        sp = (short *)cp_a;
        *sp = m_pNpcList[sOwnerH]->m_sType;
        cp_a += 2;
        *cp_a = m_pNpcList[sOwnerH]->m_cDir;
        cp_a++;
        memcpy(cp_a, m_pNpcList[sOwnerH]->m_cName, 5);
        cp_a += 5;
        sp = (short *)cp_a;
        *sp = m_pNpcList[sOwnerH]->m_sAppr2;
        cp_a += 2;
        ip = (int *)cp_a;
        ipStatus = ip;
        *ip = m_pNpcList[sOwnerH]->m_iStatus;
        cp_a += 4;
        if (wMsgType == DEF_OBJECTNULLACTION)
        {
            if (m_pNpcList[sOwnerH]->m_bIsKilled == true)
                *cp_a = 1;
            else *cp_a = 0;
        }
        else *cp_a = 0;
        cp_a++;


        ip = (int *)cp_a;
        *ip = m_pNpcList[sOwnerH]->m_iMaxHP;
        cp_a += 4;

        ip = (int *)cp_a;
        *ip = m_pNpcList[sOwnerH]->m_iHP;
        cp_a += 4;

        //Change HP Bar

        wp = (uint16_t *)cp_s;
        *wp = sOwnerH + 40000;
        cp_s += 2;

        *cp_s = m_pNpcList[sOwnerH]->m_cDir;
        cp_s++;

        *cp_s = (unsigned char)sV1;
        cp_s++;
        *cp_s = (unsigned char)sV2;
        cp_s++;


        sp = (short *)cp_s;
        sX = m_pNpcList[sOwnerH]->m_sX;
        *sp = sX;
        cp_s += 2;
        sp = (short *)cp_s;
        sY = m_pNpcList[sOwnerH]->m_sY;
        *sp = sY;
        cp_s += 2;


        ip = (int *)cp_s;
        *ip = m_pNpcList[sOwnerH]->m_iMaxHP;
        cp_s += 4;

        ip = (int *)cp_s;
        *ip = m_pNpcList[sOwnerH]->m_iHP;
        cp_s += 4;

        //Change HP Bar


        wp = (uint16_t *)cp_sv;
        *wp = sOwnerH + 40000;
        cp_sv += 2;
        *cp_sv = m_pNpcList[sOwnerH]->m_cDir;
        cp_sv++;
        *cp_sv = sV1 - sX;
        cp_sv++;
        *cp_sv = sV2 - sY;
        cp_sv++;
        sp = (short *)cp_sv;
        *sp = sV3;
        cp_sv += 2;

        ip = (int *)cp_sv;
        *ip = m_pNpcList[sOwnerH]->m_iMaxHP;
        cp_sv += 4;

        ip = (int *)cp_sv;
        *ip = m_pNpcList[sOwnerH]->m_iHP;
        cp_sv += 4;

        //Change HP Bar

        for (i = 1; i < DEF_MAXCLIENTS/*DEF_MAXNPCS*/; i++)
        {
            if ((m_pClientList[i] != 0))
                if ((m_pClientList[i]->m_cMapIndex == m_pNpcList[sOwnerH]->m_cMapIndex) &&
                    (m_pClientList[i]->m_sX >= m_pNpcList[sOwnerH]->m_sX - 10 - sRange) &&
                    (m_pClientList[i]->m_sX <= m_pNpcList[sOwnerH]->m_sX + 10 + sRange) &&
                    (m_pClientList[i]->m_sY >= m_pNpcList[sOwnerH]->m_sY - 8 - sRange) &&
                    (m_pClientList[i]->m_sY <= m_pNpcList[sOwnerH]->m_sY + 8 + sRange))
                {


                    iTemp = *ipStatus;
                    iTemp = 0x0FFFFFF & iTemp;

                    iTemp2 = (int)iGetNpcRelationship(sOwnerH, i);
                    iTemp = (iTemp | (iTemp2 << 28));
                    *ipStatus = iTemp;


                    if ((m_pClientList[i]->m_sX >= m_pNpcList[sOwnerH]->m_sX - 9) &&
                        (m_pClientList[i]->m_sX <= m_pNpcList[sOwnerH]->m_sX + 9) &&
                        (m_pClientList[i]->m_sY >= m_pNpcList[sOwnerH]->m_sY - 7) &&
                        (m_pClientList[i]->m_sY <= m_pNpcList[sOwnerH]->m_sY + 7))
                    {

                        switch (wMsgType)
                        {
                            case DEF_MSGTYPE_CONFIRM:
                            case DEF_MSGTYPE_REJECT:
                            case DEF_OBJECTNULLACTION:

                                iRet = m_pClientList[i]->iSendMsg(cData_All, 27 + 8, cKey);
                                break;

                            case DEF_OBJECTDYING:
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15 + 8, cKey);
                                break;

                            case DEF_OBJECTDAMAGE:
                            case DEF_OBJECTDAMAGEMOVE:
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11 + 8, cKey);
                                break;

                            case DEF_OBJECTATTACK:
                            case DEF_OBJECTATTACKMOVE:

                                iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13 + 8, cKey);
                                break;

                            default:

                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 9 + 8, cKey);
                                break;
                        }
                    }
                    else
                    {

                        switch (wMsgType)
                        {
                            case DEF_MSGTYPE_CONFIRM:
                            case DEF_MSGTYPE_REJECT:
                            case DEF_OBJECTNULLACTION:

                                iRet = m_pClientList[i]->iSendMsg(cData_All, 27 + 8, cKey);
                                break;

                            case DEF_OBJECTDYING:
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15 + 8, cKey);
                                break;

                            case DEF_OBJECTDAMAGE:
                            case DEF_OBJECTDAMAGEMOVE:
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11 + 8, cKey);
                                break;

                            case DEF_OBJECTATTACK:
                            case DEF_OBJECTATTACKMOVE:

                                iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13 + 8, cKey);
                                break;

                            default:

                                iRet = m_pClientList[i]->iSendMsg(cData_All, 27 + 8, cKey);
                                break;
                        }
                    }
                }
        }
    }
}

void CGame::OnTimer()
{
    uint32_t dwTime;
    int iRet = 0;


    dwTime = timeGetTime();

    //	if ((dwTime - m_dwGameTime1) > 200) {
    GameProcess();
    //		m_dwGameTime1 = dwTime;
    //	}


    if ((dwTime - m_dwGameTime2) > 3000)
    {
        CheckClientResponseTime();
        CheckDayOrNightMode();

        m_dwGameTime2 = dwTime;

        if ((m_bIsGameStarted == false) && (m_bIsItemAvailable == true) &&
            (m_bIsNpcAvailable == true) && (m_bIsMagicAvailable == true) &&
            (m_bIsSkillAvailable == true) && (m_bIsPortionAvailable == true) &&
            (m_bIsQuestAvailable == true) && (m_bIsBuildItemAvailable == true)
            )
        {

            log->info("Starting server.");
            m_bIsGameStarted = true;
        }
    }

    if ((dwTime - m_dwGameTime6) > 1000)
    {
        DelayEventProcessor();

        if (m_iFinalShutdownCount != 0)
        {
            m_iFinalShutdownCount--;
            wsprintf(G_cTxt, "Final Shutdown...%d", m_iFinalShutdownCount);
            log->info(G_cTxt);
            if (m_iFinalShutdownCount <= 1)
            {

                SendMessage(m_hWnd, WM_DESTROY, 0, 0);
                return;

            }
        }

        m_dwGameTime6 = dwTime;
    }

    if ((dwTime - m_dwGameTime3) > 3000)
    {
        SyncMiddlelandMapInfo();
        CheckDynamicObjectList();
        DynamicObjectEffectProcessor();
        NoticeHandler();
        SpecialEventHandler();
        EnergySphereProcessor();
        m_dwGameTime3 = dwTime;
    }

    if ((dwTime - m_dwGameTime4) > 600)
    {
        MobGenerator();
        m_dwGameTime4 = dwTime;
    }

    if ((dwTime - m_dwGameTime5) > 1000 * 60 * 3)
    {

        if (m_iMiddlelandMapIndex >= 0)
        {
            SaveOccupyFlagData();
        }
        m_dwGameTime5 = dwTime;

        srand((unsigned)time(0));
    }

    if ((dwTime - m_dwFishTime) > 4000)
    {
        FishProcessor();
        FishGenerator();
        SendCollectedMana();
        CrusadeWarStarter();
        //ApocalypseStarter();
        ApocalypseEnder();
        m_dwFishTime = dwTime;
    }

    if ((dwTime - m_dwWhetherTime) > 1000 * 20)
    {
        WhetherProcessor();
        m_dwWhetherTime = dwTime;
    }

    if ((m_bHeldenianRunning == true) && (m_bIsHeldenianMode == true))
    {
        SetHeldenianMode();
    }


    if ((dwTime - m_dwCanFightzoneReserveTime) > 7200000)
    {
        FightzoneReserveProcessor();
        m_dwCanFightzoneReserveTime = dwTime;
    }

    if ((m_bIsServerShutdowned == false) && (m_bOnExitProcess == true) && ((dwTime - m_dwExitProcessTime) > 1000 * 2))
    {
        if (_iForcePlayerDisconect(15) == 0)
        {
            log->info("(!) GAME SERVER SHUTDOWN PROCESS COMPLETED! All players are disconnected.");
            m_bIsServerShutdowned = true;

            if (m_iFinalShutdownCount == 0)	m_iFinalShutdownCount = 20;
        }
        m_dwExitProcessTime = dwTime;
    }


    if ((dwTime - m_dwMapSectorInfoTime) > 1000 * 10)
    {
        m_dwMapSectorInfoTime = dwTime;
        UpdateMapSectorInfo();


        MineralGenerator();

        m_iMapSectorInfoUpdateCount++;
        if (m_iMapSectorInfoUpdateCount >= 5)
        {

            AgingMapSectorInfo();
            m_iMapSectorInfoUpdateCount = 0;
        }
    }
}

void CGame::CheckClientResponseTime()
{
    // int i, iPlusTime, iMaxSuperAttack, iValue, iMapSide,iTmpMapSide;
    int i, iPlusTime, iMaxSuperAttack, iValue, iMapSide{};

    short sTemp;
    uint32_t dwTime;
    //SYSTEMTIME SysTime;

    /*

    GetLocalTime(&SysTime);
    switch (SysTime.wDayOfWeek) {
    case 1:	iWarPeriod = 30; break; //?ù?äÀÏ
    case 2:	iWarPeriod = 30; break; //È??äÀÏ
    case 3:	iWarPeriod = 60; break; //?ö?äÀÏ
    case 4:	iWarPeriod = 60*2;  break; //?ñ?äÀÏ
    case 5:	iWarPeriod = 60*5;  break; //?Ý?äÀÏ
    case 6:	iWarPeriod = 60*10; break; //Åä?äÀÏ
    case 0:	iWarPeriod = 60*20; break; //ÀÏ?äÀÏ
    }
    */

    dwTime = timeGetTime();


    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != 0)
        {

            if ((dwTime - m_pClientList[i]->m_dwTime) > DEF_CLIENTTIMEOUT)
            {
                if (m_pClientList[i]->m_bIsInitComplete == true)
                {
                    wsprintf(G_cTxt, "Client Timeout: %s", m_pClientList[i]->m_cIPaddress);
                    log->info(G_cTxt);

                    DeleteClient(i, true, true);
                }
                else if ((dwTime - m_pClientList[i]->m_dwTime) > DEF_CLIENTTIMEOUT)
                {

                    DeleteClient(i, false, false);
                }
            }
            else if (m_pClientList[i]->m_bIsInitComplete == true)
            {

                m_pClientList[i]->m_iTimeLeft_ShutUp--;
                if (m_pClientList[i]->m_iTimeLeft_ShutUp < 0) m_pClientList[i]->m_iTimeLeft_ShutUp = 0;


                m_pClientList[i]->m_iTimeLeft_Rating--;
                if (m_pClientList[i]->m_iTimeLeft_Rating < 0) m_pClientList[i]->m_iTimeLeft_Rating = 0;


                if (((dwTime - m_pClientList[i]->m_dwHungerTime) > DEF_HUNGERTIME) && (m_pClientList[i]->m_bIsKilled == false))
                {


                    if ((m_pClientList[i]->m_iLevel < DEF_LEVELLIMIT) || (m_pClientList[i]->m_iAdminUserLevel >= 1))
                    {

                    }
                    else m_pClientList[i]->m_iHungerStatus--;
                    if (m_pClientList[i]->m_iHungerStatus <= 0) m_pClientList[i]->m_iHungerStatus = 0;
                    m_pClientList[i]->m_dwHungerTime = dwTime;

                    if ((m_pClientList[i]->m_iHP > 0) && (m_pClientList[i]->m_iHungerStatus < 30))
                    {

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_HUNGER, m_pClientList[i]->m_iHungerStatus, 0, 0, 0);
                    }
                }


                if ((m_pClientList[i]->m_iHungerStatus <= 30) && (m_pClientList[i]->m_iHungerStatus >= 0))
                    iPlusTime = (30 - m_pClientList[i]->m_iHungerStatus) * 1000;
                else iPlusTime = 0;

                iPlusTime = abs(iPlusTime);


                if ((dwTime - m_pClientList[i]->m_dwHPTime) > (DWORD)(DEF_HPUPTIME + iPlusTime))
                {
                    TimeHitPointsUp(i);
                    m_pClientList[i]->m_dwHPTime = dwTime;
                }


                if ((dwTime - m_pClientList[i]->m_dwMPTime) > (DWORD)(DEF_MPUPTIME + iPlusTime))
                {
                    TimeManaPointsUp(i);
                    m_pClientList[i]->m_dwMPTime = dwTime;
                }


                if ((dwTime - m_pClientList[i]->m_dwSPTime) > (DWORD)(DEF_SPUPTIME + iPlusTime))
                {
                    TimeStaminarPointsUp(i);
                    m_pClientList[i]->m_dwSPTime = dwTime;
                }


                if ((m_pClientList[i]->m_bIsPoisoned == true) && ((dwTime - m_pClientList[i]->m_dwPoisonTime) > DEF_POISONTIME))
                {
                    PoisonEffect(i, 0);
                    m_pClientList[i]->m_dwPoisonTime = dwTime;
                }


                if ((m_pMapList[m_pClientList[i]->m_cMapIndex]->m_bIsFightZone == false) &&
                    ((dwTime - m_pClientList[i]->m_dwAutoSaveTime) > (DWORD)DEF_AUTOSAVETIME))
                {
                    if (!save_player_data(m_pClientList[i]->shared_from_this()))
                    {
                        log->error("Autosave player data error for [{}]", m_pClientList[i]->m_cCharName);
                    }
                    m_pClientList[i]->m_dwAutoSaveTime = dwTime;
                }


                if ((dwTime - m_pClientList[i]->m_dwExpStockTime) > (DWORD)DEF_EXPSTOCKTIME)
                {
                    m_pClientList[i]->m_dwExpStockTime = dwTime;
                    CalcExpStock(i);
                    CheckUniqueItemEquipment(i);
                    CheckCrusadeResultCalculation(i);
                }


                if ((dwTime - m_pClientList[i]->m_dwAutoExpTime) > (DWORD)DEF_AUTOEXPTIME)
                {

                    iValue = (m_pClientList[i]->m_iLevel);
                    if (iValue <= 0) iValue = 1;
                    if (m_pClientList[i]->m_iAutoExpAmount < iValue)
                    {

                        if ((m_pClientList[i]->m_iExp + iValue) < m_iLevelExpTable[m_pClientList[i]->m_iLevel + 1])
                        {
                            GetExp(i, iValue);
                            CalcExpStock(i);
                        }
                    }

                    m_pClientList[i]->m_iAutoExpAmount = 0;
                    m_pClientList[i]->m_dwAutoExpTime = dwTime;
                }


                if (m_pClientList[i]->m_iSpecialAbilityTime == 3)
                {

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_SPECIALABILITYENABLED, 0, 0, 0, 0);
                }
                m_pClientList[i]->m_iSpecialAbilityTime -= 3;
                if (m_pClientList[i]->m_iSpecialAbilityTime < 0) m_pClientList[i]->m_iSpecialAbilityTime = 0;


                if (m_pClientList[i]->m_bIsSpecialAbilityEnabled == true)
                {
                    if (((dwTime - m_pClientList[i]->m_dwSpecialAbilityStartTime) / 1000) > m_pClientList[i]->m_iSpecialAbilityLastSec)
                    {

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_SPECIALABILITYSTATUS, 3, 0, 0, 0);
                        m_pClientList[i]->m_bIsSpecialAbilityEnabled = false;

                        m_pClientList[i]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;

                        sTemp = m_pClientList[i]->m_sAppr4;
                        sTemp = sTemp & 0xFF0F;
                        m_pClientList[i]->m_sAppr4 = sTemp;

                        SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                    }
                }

                m_pClientList[i]->m_iLockedMapTime -= 3;
                if (m_pClientList[i]->m_iLockedMapTime < 0)
                {

                    m_pClientList[i]->m_iLockedMapTime = 0;
                    memset(m_pClientList[i]->m_cLockedMapName, 0, sizeof(m_pClientList[i]->m_cLockedMapName));
                    strcpy(m_pClientList[i]->m_cLockedMapName, "NONE");
                }


                m_pClientList[i]->m_iDeadPenaltyTime -= 3;
                if (m_pClientList[i]->m_iDeadPenaltyTime < 0) m_pClientList[i]->m_iDeadPenaltyTime = 0;

                if ((m_pClientList[i]->m_bIsWarLocation == true))
                {
                    if ((m_bIsCrusadeMode == false) || (m_pClientList[i]->m_bIsInBuilding == true))
                        m_pClientList[i]->m_iTimeLeft_ForceRecall--;

                    if (m_pClientList[i]->m_iTimeLeft_ForceRecall <= 0)
                    {

                        m_pClientList[i]->m_iTimeLeft_ForceRecall = 0;
                        m_pClientList[i]->m_dwWarBeginTime = dwTime;
                        m_pClientList[i]->m_bIsWarLocation = false;


                        SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
                        RequestTeleportHandler(i, "0   ");
                    }
                }

                if (m_pClientList[i] == 0) break;
                if (m_pClientList[i]->m_iSkillMsgRecvCount >= 2)
                {

                    //wsprintf(G_cTxt, "(!) ÇØÅ· ¿ëÀÇÀÚ(%s) ¼Óµµ Á¶ÀÛ", m_pClientList[i]->m_cCharName);
                    //log->info(G_cTxt);
                    DeleteClient(i, true, true);
                }
                else
                {
                    m_pClientList[i]->m_iSkillMsgRecvCount = 0;
                }


#ifndef DEF_TESTSERVER
                //if (m_pClientList[i] == 0) break;
                //if ((dwTime - m_pClientList[i]->m_dwSpeedHackCheckTime) > (DWORD)1000*60*60) {
                //	if ((m_pClientList[i]->m_iExp - m_pClientList[i]->m_iSpeedHackCheckExp) > 300000) {
                //
                //		wsprintf(G_cTxt, "(!) ¼Óµµ Á¶ÀÛ ¿ëÀÇ Ä³¸¯ÅÍ(%s) 1½Ã°£¿¡ %dExp?", m_pClientList[i]->m_cCharName, (m_pClientList[i]->m_iExp - m_pClientList[i]->m_iSpeedHackCheckExp));
                //		
                //		SYSTEMTIME SysTime;
                //		GetLocalTime(&SysTime);
                //								
                //		if ((SysTime.wMonth+1) >= 13) {
                //			m_pClientList[i]->m_iPenaltyBlockYear = SysTime.wYear+1;
                //			m_pClientList[i]->m_iPenaltyBlockMonth = 1;
                //			m_pClientList[i]->m_iPenaltyBlockDay = SysTime.wDay;
                //		}
                //		else {
                //			m_pClientList[i]->m_iPenaltyBlockYear = SysTime.wYear;
                //			m_pClientList[i]->m_iPenaltyBlockMonth = SysTime.wMonth+1;
                //			m_pClientList[i]->m_iPenaltyBlockDay = SysTime.wDay;
                //		}
                //		
                //		log->info(G_cTxt);
                //		log->info(G_cTxt);
                //		DeleteClient(i, true, true);
                //	}
                //	else {
                //		m_pClientList[i]->m_dwSpeedHackCheckTime = dwTime;
                //		m_pClientList[i]->m_iSpeedHackCheckExp = m_pClientList[i]->m_iExp;
                //	}
                //}
#endif
                if (m_pClientList[i] == 0) break;


                if ((m_pClientList[i]->m_iLevel < m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iLevelLimit) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
                    RequestTeleportHandler(i, "0   ");
                }

                if (m_pClientList[i] == 0) break;


                if ((m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iUpperLevelLimit != 0) &&
                    (m_pClientList[i]->m_iLevel > m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iUpperLevelLimit) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
                    RequestTeleportHandler(i, "0   ");
                }





                /*
                if (m_pClientList[i] == 0) break;
                if ((m_pClientList[i]->m_dwFightzoneDeadTime != 0) &&
                ((dwTime - m_pClientList[i]->m_dwFightzoneDeadTime) > 1000*30)) {

                m_pClientList[i]->m_dwFightzoneDeadTime = 0;
                ResurrectPlayer(i);
                }
                */

                if (m_pClientList[i] == 0) break;





                if ((m_bIsCrusadeMode == false)
                    && (m_pClientList[i]->m_bIsHunter == true)
                    && (((m_iMiddlelandMapIndex == m_pClientList[i]->m_cMapIndex) && (m_iMiddlelandMapIndex != -1))
                        || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "2ndmiddle", 9) == 0)
                        || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "aresdend1", 9) == 0)
                        || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "elvined1", 8) == 0)
                        || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "huntzone3", 9) == 0)
                        || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "huntzone4", 9) == 0)
                        || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "middle", 6) == 0))
                    && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
                    RequestTeleportHandler(i, "0   ");
                }


                if (m_pClientList[i] == 0) break;

                m_pClientList[i]->m_iSuperAttackCount++;
                if (m_pClientList[i]->m_iSuperAttackCount > 12)
                {
                    m_pClientList[i]->m_iSuperAttackCount = 0;

                    iMaxSuperAttack = (m_pClientList[i]->m_iLevel / 10);
                    if (m_pClientList[i]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[i]->m_iSuperAttackLeft++;

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
                }

                m_pClientList[i]->m_iTimeLeft_FirmStaminar--;
                if (m_pClientList[i]->m_iTimeLeft_FirmStaminar < 0) m_pClientList[i]->m_iTimeLeft_FirmStaminar = 0;


                if (m_pClientList[i] == 0) break;
                if (m_pClientList[i]->m_bIsSendingMapStatus == true) _SendMapStatus(i);

                if (m_pClientList[i]->m_iConstructionPoint > 0)
                {

                    CheckCommanderConstructionPoint(i);
                }
            }
        }
    }
}

void CGame::GameProcess()
{
    NpcProcess();
    MsgProcess();
    ForceRecallProcess();
    DelayEventProcess();
}

bool CGame::_bRegisterMap(char * pName)
{
    int i;
    char cTmpName[11], cTxt[120];

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pName);

    for (i = 0; i < DEF_MAXMAPS; i++)
        if ((m_pMapList[i] != 0) && (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0))
        {

            wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map (%s) is already installed! cannot add.", cTmpName);
            log->info(cTxt);
            return false;
        }


    for (i = 1; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] == 0)
        {
            m_pMapList[i] = (CMap *)new CMap(this, i);
            wsprintf(cTxt, "(*) Add map (%s)   - Loading map info files...", pName);
            log->info(cTxt);


            if (m_pMapList[i]->bInit(pName) == false)
            {
                wsprintf(cTxt, "(!!!) Data file loading fail! - %s", pName);
                log->info(cTxt);
                return false;
            };

            log->info("(*) Data file loading success.");



            if ((m_iMiddlelandMapIndex == -1) && (strcmp("middleland", pName) == 0))
                m_iMiddlelandMapIndex = i;


            if ((m_iAresdenMapIndex == -1) && (strcmp("aresden", pName) == 0))
                m_iAresdenMapIndex = i;


            if ((m_iElvineMapIndex == -1) && (strcmp("elvine", pName) == 0))
                m_iElvineMapIndex = i;

            m_iTotalMaps++;
            return true;
        }

    wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map (%s) cannot be added - no more map space.", pName);
    log->info(cTxt);
    return false;
}

bool CGame::_bInitItemAttr(CItem * pItem, char * pItemName)
{
    int i;
    char cTmpName[22];

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pItemName);

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != 0)
        {
            if (memcmp(cTmpName, m_pItemConfigList[i]->m_cName, 20) == 0)
            {

                memset(pItem->m_cName, 0, sizeof(pItem->m_cName));
                strcpy(pItem->m_cName, m_pItemConfigList[i]->m_cName);
                pItem->m_cItemType = m_pItemConfigList[i]->m_cItemType;
                pItem->m_cEquipPos = m_pItemConfigList[i]->m_cEquipPos;
                pItem->m_sItemEffectType = m_pItemConfigList[i]->m_sItemEffectType;
                pItem->m_sItemEffectValue1 = m_pItemConfigList[i]->m_sItemEffectValue1;
                pItem->m_sItemEffectValue2 = m_pItemConfigList[i]->m_sItemEffectValue2;
                pItem->m_sItemEffectValue3 = m_pItemConfigList[i]->m_sItemEffectValue3;
                pItem->m_sItemEffectValue4 = m_pItemConfigList[i]->m_sItemEffectValue4;
                pItem->m_sItemEffectValue5 = m_pItemConfigList[i]->m_sItemEffectValue5;
                pItem->m_sItemEffectValue6 = m_pItemConfigList[i]->m_sItemEffectValue6;
                pItem->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
                pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
                pItem->m_sSpecialEffect = m_pItemConfigList[i]->m_sSpecialEffect;

                pItem->m_sSprite = m_pItemConfigList[i]->m_sSprite;
                pItem->m_sSpriteFrame = m_pItemConfigList[i]->m_sSpriteFrame;
                pItem->m_wPrice = m_pItemConfigList[i]->m_wPrice;
                pItem->m_wWeight = m_pItemConfigList[i]->m_wWeight;
                pItem->m_cApprValue = m_pItemConfigList[i]->m_cApprValue;
                pItem->m_cSpeed = m_pItemConfigList[i]->m_cSpeed;
                pItem->m_sLevelLimit = m_pItemConfigList[i]->m_sLevelLimit;
                pItem->m_cGenderLimit = m_pItemConfigList[i]->m_cGenderLimit;

                pItem->m_sSpecialEffectValue1 = m_pItemConfigList[i]->m_sSpecialEffectValue1;
                pItem->m_sSpecialEffectValue2 = m_pItemConfigList[i]->m_sSpecialEffectValue2;

                pItem->m_sRelatedSkill = m_pItemConfigList[i]->m_sRelatedSkill;
                pItem->m_cCategory = m_pItemConfigList[i]->m_cCategory;
                pItem->m_sIDnum = m_pItemConfigList[i]->m_sIDnum;

                pItem->m_bIsForSale = m_pItemConfigList[i]->m_bIsForSale;
                pItem->m_cItemColor = m_pItemConfigList[i]->m_cItemColor;
                return true;
            }
        }


    return false;
}

// v2.17 2002-7-31 ¾ÆÀÌÅÛÀ» ¾ÆÀÌÅÛ °íÀ¯¹øÈ£·Î »ý¼ºÇÒ ¼ö ÀÖ°Ô ÇÑ´Ù. 
bool CGame::_bInitItemAttr(CItem * pItem, int iItemID)
{
    int i;
    char cTmpName[21];

    memset(cTmpName, 0, sizeof(cTmpName));
    //strcpy(cTmpName, pItemName);

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != 0)
        {
            if (iItemID == m_pItemConfigList[i]->m_sIDnum)
            {

                memset(pItem->m_cName, 0, sizeof(pItem->m_cName));
                strcpy(pItem->m_cName, m_pItemConfigList[i]->m_cName);
                pItem->m_cItemType = m_pItemConfigList[i]->m_cItemType;
                pItem->m_cEquipPos = m_pItemConfigList[i]->m_cEquipPos;
                pItem->m_sItemEffectType = m_pItemConfigList[i]->m_sItemEffectType;
                pItem->m_sItemEffectValue1 = m_pItemConfigList[i]->m_sItemEffectValue1;
                pItem->m_sItemEffectValue2 = m_pItemConfigList[i]->m_sItemEffectValue2;
                pItem->m_sItemEffectValue3 = m_pItemConfigList[i]->m_sItemEffectValue3;
                pItem->m_sItemEffectValue4 = m_pItemConfigList[i]->m_sItemEffectValue4;
                pItem->m_sItemEffectValue5 = m_pItemConfigList[i]->m_sItemEffectValue5;
                pItem->m_sItemEffectValue6 = m_pItemConfigList[i]->m_sItemEffectValue6;
                pItem->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
                pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
                pItem->m_sSpecialEffect = m_pItemConfigList[i]->m_sSpecialEffect;

                pItem->m_sSprite = m_pItemConfigList[i]->m_sSprite;
                pItem->m_sSpriteFrame = m_pItemConfigList[i]->m_sSpriteFrame;
                pItem->m_wPrice = m_pItemConfigList[i]->m_wPrice;
                pItem->m_wWeight = m_pItemConfigList[i]->m_wWeight;
                pItem->m_cApprValue = m_pItemConfigList[i]->m_cApprValue;
                pItem->m_cSpeed = m_pItemConfigList[i]->m_cSpeed;
                pItem->m_sLevelLimit = m_pItemConfigList[i]->m_sLevelLimit;
                pItem->m_cGenderLimit = m_pItemConfigList[i]->m_cGenderLimit;

                pItem->m_sSpecialEffectValue1 = m_pItemConfigList[i]->m_sSpecialEffectValue1;
                pItem->m_sSpecialEffectValue2 = m_pItemConfigList[i]->m_sSpecialEffectValue2;

                pItem->m_sRelatedSkill = m_pItemConfigList[i]->m_sRelatedSkill;
                pItem->m_cCategory = m_pItemConfigList[i]->m_cCategory;
                pItem->m_sIDnum = m_pItemConfigList[i]->m_sIDnum;

                pItem->m_bIsForSale = m_pItemConfigList[i]->m_bIsForSale;
                pItem->m_cItemColor = m_pItemConfigList[i]->m_cItemColor;
                return true;
            }
        }


    return false;
}



bool CGame::_bGetIsStringIsNumber(char * pStr)
{
    int i;
    for (i = 0; i < (int)strlen(pStr); i++)
        if ((pStr[i] != '-') && ((pStr[i] < (char)'0') || (pStr[i] > (char)'9'))) return false;

    return true;
}


bool CGame::_bReadMapInfoFiles(int iMapIndex)
{
    if (__bReadMapInfo(iMapIndex) == false)
    {
        return false;
    }

    return true;
}

bool CGame::bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType)
{
    int i, iIndex, iMapIndex, iFollowIndex;
    char cTmpName[11], cFollowSide;

    memset(cTmpName, 0, sizeof(cTmpName));
    iMapIndex = -1;
    iFollowIndex = -1;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if ((m_pNpcList[i] != 0) && (memcmp(m_pNpcList[i]->m_cName, pName, 5) == 0))
        {
            iIndex = i;
            iMapIndex = m_pNpcList[i]->m_cMapIndex;
            goto NEXT_STEP_SNFM1;
        }

    NEXT_STEP_SNFM1:;

    switch (cFollowOwnerType)
    {
        case DEF_OWNERTYPE_NPC:
            for (i = 1; i < DEF_MAXNPCS; i++)
                if ((m_pNpcList[i] != 0) && (memcmp(m_pNpcList[i]->m_cName, pFollowName, 5) == 0))
                {


                    if (m_pNpcList[i]->m_cMapIndex != iMapIndex) return false;
                    iFollowIndex = i;
                    cFollowSide = m_pNpcList[i]->m_cSide;
                    goto NEXT_STEP_SNFM2;
                }
            break;

        case DEF_OWNERTYPE_PLAYER:
            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pFollowName, 10) == 0))
                {


                    if (m_pClientList[i]->m_cMapIndex != iMapIndex) return false;
                    iFollowIndex = i;
                    cFollowSide = m_pClientList[i]->m_cSide;
                    goto NEXT_STEP_SNFM2;
                }
            break;
    }

    NEXT_STEP_SNFM2:;

    if ((iIndex == -1) || (iFollowIndex == -1)) return false;

    m_pNpcList[iIndex]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
    m_pNpcList[iIndex]->m_cFollowOwnerType = cFollowOwnerType;
    m_pNpcList[iIndex]->m_iFollowOwnerIndex = iFollowIndex;
    m_pNpcList[iIndex]->m_cSide = cFollowSide;


    return true;
}


bool CGame::bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType, int * poX, int * poY, char * pWaypointList, hbxrect * pArea, int iSpotMobIndex, char cChangeSide, bool bHideGenMode, bool bIsSummoned, bool bFirmBerserk, bool bIsMaster, int iGuildGUID)
{
    int i, t, j, k, iMapIndex;
    char  cTmpName[11], cTxt[120];
    short sX, sY, sRange;
    bool  bFlag;
    SYSTEMTIME SysTime;

    if (strlen(pName) == 0)   return false;
    if (strlen(pNpcName) == 0) return false;

    GetLocalTime(&SysTime);

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pMapName);
    iMapIndex = -1;

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0)
        {
            if (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0)
                iMapIndex = i;
        }

    if (iMapIndex == -1) return false;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] == 0)
        {
            m_pNpcList[i] = new CNpc(pName);

            if (_bInitNpcAttr(m_pNpcList[i], pNpcName, sClass, cSA) == false)
            {

                wsprintf(cTxt, "(!) Not existing NPC creation request! (%s) Ignored.", pNpcName);
                log->info(cTxt);

                delete m_pNpcList[i];
                m_pNpcList[i] = 0;
                return false;
            }


            if (m_pNpcList[i]->m_cDayOfWeekLimit < 10)
            {
                if (m_pNpcList[i]->m_cDayOfWeekLimit != SysTime.wDayOfWeek)
                {

                    delete m_pNpcList[i];
                    m_pNpcList[i] = 0;
                    return false;
                }
            }


            switch (cMoveType)
            {
                case DEF_MOVETYPE_GUARD:
                case DEF_MOVETYPE_RANDOM:
                    if ((poX != 0) && (poY != 0) && (*poX != 0) && (*poY != 0))
                    {

                        sX = *poX;
                        sY = *poY;
                    }
                    else
                    {
                        for (j = 0; j <= 30; j++)
                        {

                            sX = (rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15;
                            sY = (rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15;

                            bFlag = true;
                            for (k = 0; k < DEF_MAXMGAR; k++)
                                if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left != -1)
                                {

                                    if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left) &&
                                        (sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].right) &&
                                        (sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].top) &&
                                        (sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].bottom))
                                    {

                                        bFlag = false;
                                    }
                                }
                            if (bFlag == true) goto GET_VALIDLOC_SUCCESS;

                        }

                        delete m_pNpcList[i];
                        m_pNpcList[i] = 0;
                        return false;

                        GET_VALIDLOC_SUCCESS:;

                    }
                    break;

                case DEF_MOVETYPE_RANDOMAREA:

                    sRange = (short)(pArea->right - pArea->left);
                    sX = (short)((rand() % sRange) + pArea->left);
                    sRange = (short)(pArea->bottom - pArea->top);
                    sY = (short)((rand() % sRange) + pArea->top);
                    break;

                case DEF_MOVETYPE_RANDOMWAYPOINT:

                    sX = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[iDice(1, 10) - 1]].x;
                    sY = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[iDice(1, 10) - 1]].y;
                    break;

                default:

                    if ((poX != 0) && (poY != 0) && (*poX != 0) && (*poY != 0))
                    {

                        sX = *poX;
                        sY = *poY;
                    }
                    else
                    {
                        sX = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[0]].x;
                        sY = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[0]].y;
                    }
                    break;
            }


            if (bGetEmptyPosition(&sX, &sY, iMapIndex) == false)
            {

                delete m_pNpcList[i];
                m_pNpcList[i] = 0;
                return false;
            }

            if ((bHideGenMode == true) && (_iGetPlayerNumberOnSpot(sX, sY, iMapIndex, 7) != 0))
            {

                delete m_pNpcList[i];
                m_pNpcList[i] = 0;
                return false;
            }

            if ((poX != 0) && (poY != 0))
            {

                *poX = sX;
                *poY = sY;
            }

            m_pNpcList[i]->m_sX = sX;
            m_pNpcList[i]->m_sY = sY;


            m_pNpcList[i]->m_vX = sX;
            m_pNpcList[i]->m_vY = sY;


            for (t = 0; t < 10; t++)
                m_pNpcList[i]->m_iWayPointIndex[t] = pWaypointList[t];


            m_pNpcList[i]->m_cTotalWaypoint = 0;
            for (t = 0; t < 10; t++)
                if (m_pNpcList[i]->m_iWayPointIndex[t] != -1) m_pNpcList[i]->m_cTotalWaypoint++;

            if (pArea != 0)
            {
                // RANDOMAREA Copy
                m_pNpcList[i]->m_rcRandomArea = *pArea;
            }


            switch (cMoveType)
            {
                case DEF_MOVETYPE_GUARD:
                    m_pNpcList[i]->m_dX = m_pNpcList[i]->m_sX;
                    m_pNpcList[i]->m_dY = m_pNpcList[i]->m_sY;
                    break;

                case DEF_MOVETYPE_SEQWAYPOINT:
                    m_pNpcList[i]->m_cCurWaypoint = 1;

                    m_pNpcList[i]->m_dX = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].x;
                    m_pNpcList[i]->m_dY = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].y;
                    break;

                case DEF_MOVETYPE_RANDOMWAYPOINT:
                    m_pNpcList[i]->m_cCurWaypoint = (rand() % (m_pNpcList[i]->m_cTotalWaypoint - 1)) + 1;

                    m_pNpcList[i]->m_dX = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].x;
                    m_pNpcList[i]->m_dY = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].y;
                    break;

                case DEF_MOVETYPE_RANDOMAREA:
                    m_pNpcList[i]->m_cCurWaypoint = 0;

                    sRange = (short)(m_pNpcList[i]->m_rcRandomArea.right - m_pNpcList[i]->m_rcRandomArea.left);
                    m_pNpcList[i]->m_dX = (short)((rand() % sRange) + m_pNpcList[i]->m_rcRandomArea.left);
                    sRange = (short)(m_pNpcList[i]->m_rcRandomArea.bottom - m_pNpcList[i]->m_rcRandomArea.top);
                    m_pNpcList[i]->m_dY = (short)((rand() % sRange) + m_pNpcList[i]->m_rcRandomArea.top);
                    break;

                case DEF_MOVETYPE_RANDOM:
                    m_pNpcList[i]->m_dX = (short)((rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15);
                    m_pNpcList[i]->m_dY = (short)((rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15);
                    break;
            }

            m_pNpcList[i]->m_tmp_iError = 0;


            m_pNpcList[i]->m_cMoveType = cMoveType;

            switch (m_pNpcList[i]->m_cActionLimit)
            {
                case 2:
                case 3:
                case 5:

                    m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_STOP;

                    switch (m_pNpcList[i]->m_sType)
                    {
                        case 15:
                        case 19:
                        case 20:
                        case 24:
                        case 25:
                        case 26:
                            m_pNpcList[i]->m_cDir = 4 + iDice(1, 3) - 1;
                            break;

                        default:
                            m_pNpcList[i]->m_cDir = iDice(1, 8);
                            break;
                    }
                    break;

                default:

                    m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                    m_pNpcList[i]->m_cDir = 5;
                    break;
            }

            m_pNpcList[i]->m_iFollowOwnerIndex = 0;
            m_pNpcList[i]->m_iTargetIndex = 0;
            m_pNpcList[i]->m_cTurn = (rand() % 2);

            switch (m_pNpcList[i]->m_sType)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:

                    m_pNpcList[i]->m_sAppr2 = 0xF000;
                    m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | ((rand() % 13) << 4); // ¹«±â
                    m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | (rand() % 9);
                    break;

                case 36:
                case 37:
                case 38:
                case 39:

                    m_pNpcList[i]->m_sAppr2 = 3;
                    break;
                case 64:  //v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã
                    m_pNpcList[i]->m_sAppr2 = 1; //3´Ü°è·Î ¼ºÀåÇÑ´Ù.
                    break;

                default:
                    m_pNpcList[i]->m_sAppr2 = 0;
                    break;
            }

            m_pNpcList[i]->m_cMapIndex = (char)iMapIndex;
            m_pNpcList[i]->m_dwTime = timeGetTime() + (rand() % 10000);
            m_pNpcList[i]->m_dwActionTime += (rand() % 300);
            m_pNpcList[i]->m_dwMPupTime = timeGetTime();
            m_pNpcList[i]->m_dwHPupTime = m_pNpcList[i]->m_dwMPupTime;
            m_pNpcList[i]->m_sBehaviorTurnCount = 0;
            m_pNpcList[i]->m_bIsSummoned = bIsSummoned;
            m_pNpcList[i]->m_bIsMaster = bIsMaster;

            if (bIsSummoned == true)
                m_pNpcList[i]->m_dwSummonedTime = timeGetTime();


            if (bFirmBerserk == true)
            {
                m_pNpcList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = 1;
                m_pNpcList[i]->m_iStatus = m_pNpcList[i]->m_iStatus | 0x20; // 100000

                m_pNpcList[i]->m_iExp *= 2;
            }

            // !!!
            if (cChangeSide != -1) m_pNpcList[i]->m_cSide = cChangeSide;

            m_pNpcList[i]->m_cBravery = (rand() % 3) + m_pNpcList[i]->m_iMinBravery;
            m_pNpcList[i]->m_iSpotMobIndex = iSpotMobIndex;
            m_pNpcList[i]->m_iGuildGUID = iGuildGUID;
            //testcode
            if (iGuildGUID != 0)
            {
                wsprintf(G_cTxt, "Summon War Unit(%d) GUID(%d)", m_pNpcList[i]->m_sType, iGuildGUID);
                log->info(G_cTxt);
            }

            m_pMapList[iMapIndex]->SetOwner(i, DEF_OWNERTYPE_NPC, sX, sY);
            m_pMapList[iMapIndex]->m_iTotalActiveObject++;
            m_pMapList[iMapIndex]->m_iTotalAliveObject++;


            switch (m_pNpcList[i]->m_sType)
            {
                case 36:
                case 37:
                case 38:
                case 39:
                case 42:
                    m_pMapList[iMapIndex]->bAddCrusadeStructureInfo(m_pNpcList[i]->m_sType, sX, sY, m_pNpcList[i]->m_cSide);
                    break;
                case 64:	//v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã
                    m_pMapList[iMapIndex]->bAddCropsTotalSum();
                    return i;
                    break;
            }




            SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);
            return i;  //v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã return °ªÀ» m_pNpcListÀÇ ¹øÈ£·Î ÇÑ´Ù.
        }

    return false;
}

void CGame::NpcProcess()
{
    int i, iMaxHP;
    uint32_t dwTime, dwActionTime;

    dwTime = timeGetTime();

    for (i = 1; i < DEF_MAXNPCS; i++)
    {

        if (m_pNpcList[i] != 0)
        {

            if (m_pNpcList[i]->m_cBehavior == DEF_BEHAVIOR_ATTACK)
            {
                switch (iDice(1, 7))
                {
                    case 1: dwActionTime = m_pNpcList[i]->m_dwActionTime; break;
                    case 2: dwActionTime = m_pNpcList[i]->m_dwActionTime - 100; break;
                    case 3: dwActionTime = m_pNpcList[i]->m_dwActionTime - 200; break;
                    case 4: dwActionTime = m_pNpcList[i]->m_dwActionTime - 300; break;
                    case 5: dwActionTime = m_pNpcList[i]->m_dwActionTime - 400; break;
                    case 6: dwActionTime = m_pNpcList[i]->m_dwActionTime - 600; break;
                    case 7: dwActionTime = m_pNpcList[i]->m_dwActionTime - 700; break;
                }
                if (dwActionTime < 600) dwActionTime = 600;
            }
            else dwActionTime = m_pNpcList[i]->m_dwActionTime;


            if (m_pNpcList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] != 0)
                dwActionTime += (dwActionTime / 2);
        }

        if ((m_pNpcList[i] != 0) && ((dwTime - m_pNpcList[i]->m_dwTime) > dwActionTime))
        {

            m_pNpcList[i]->m_dwTime = dwTime;

            if (abs(m_pNpcList[i]->m_cMagicLevel) > 0)
            {

                if ((dwTime - m_pNpcList[i]->m_dwMPupTime) > DEF_MPUPTIME)
                {
                    m_pNpcList[i]->m_dwMPupTime = dwTime;

                    //if (m_pNpcList[i]->m_bIsSummoned == false)
                    m_pNpcList[i]->m_iMana += iDice(1, (m_pNpcList[i]->m_iMaxMana / 5));

                    if (m_pNpcList[i]->m_iMana > m_pNpcList[i]->m_iMaxMana)
                        m_pNpcList[i]->m_iMana = m_pNpcList[i]->m_iMaxMana;
                }
            }


            if (((dwTime - m_pNpcList[i]->m_dwHPupTime) > DEF_HPUPTIME) && (m_pNpcList[i]->m_bIsKilled == false))
            {

                m_pNpcList[i]->m_dwHPupTime = dwTime;

                //iMaxHP = iDice(m_pNpcList[i]->m_iHitDice, 8) + m_pNpcList[i]->m_iHitDice;
                if (m_pNpcList[i]->m_iHP < m_pNpcList[i]->m_iMaxHP)
                {

                    if (m_pNpcList[i]->m_bIsSummoned == false)
                    {
                        m_pNpcList[i]->m_iHP += iDice(1, m_pNpcList[i]->m_iHitDice);

                        if (m_pNpcList[i]->m_iHP > m_pNpcList[i]->m_iMaxHP) m_pNpcList[i]->m_iHP = m_pNpcList[i]->m_iMaxHP;
                        if (m_pNpcList[i]->m_iHP <= 0)     m_pNpcList[i]->m_iHP = 1;
                        SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                    }
                }
            }

            switch (m_pNpcList[i]->m_cBehavior)
            {
                case DEF_BEHAVIOR_DEAD:
                    NpcBehavior_Dead(i);
                    break;
                case DEF_BEHAVIOR_STOP:
                    NpcBehavior_Stop(i);
                    break;
                case DEF_BEHAVIOR_MOVE:
                    NpcBehavior_Move(i);
                    break;
                case DEF_BEHAVIOR_ATTACK:
                    NpcBehavior_Attack(i);
                    break;
                case DEF_BEHAVIOR_FLEE:
                    NpcBehavior_Flee(i);
                    break;
            }


            if ((m_pNpcList[i] != 0) && (m_pNpcList[i]->m_iHP != 0) && (m_pNpcList[i]->m_bIsSummoned == true))
            {

                switch (m_pNpcList[i]->m_sType)
                {
                    case 29:
                        if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 90)
                            NpcKilledHandler(NULL, 0, i, 0);
                        break;
                    case 64: //v2.20 2002-12-20 ³óÀÛ¹° »ì¾Æ ÀÖ´Â ½Ã°£.
                        if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > DEF_PLANTTIME)
                            DeleteNpc(i);
                        break;

                    default:
                        if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > DEF_SUMMONTIME)
                            NpcKilledHandler(NULL, 0, i, 0);
                        break;
                }
            }
        }
    }
}

char CGame::cGetNextMoveDir(short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn, int * pError)
{
    char  cDir, cTmpDir;
    int   aX, aY, dX, dY;
    int   i, iResX, iResY;

    if ((sX == dstX) && (sY == dstY)) return 0;

    dX = sX;
    dY = sY;

    if ((abs(dX - dstX) <= 1) && (abs(dY - dstY) <= 1))
    {
        iResX = dstX;
        iResY = dstY;
    }
    else m_Misc.GetPoint(dX, dY, dstX, dstY, &iResX, &iResY, pError);

    cDir = m_Misc.cGetNextMoveDir(dX, dY, iResX, iResY);

    if (cTurn == 0)
        for (i = cDir; i <= cDir + 7; i++)
        {
            cTmpDir = i;
            if (cTmpDir > 8) cTmpDir -= 8;
            aX = _tmp_cTmpDirX[cTmpDir];
            aY = _tmp_cTmpDirY[cTmpDir];
            if (m_pMapList[cMapIndex]->bGetMoveable(dX + aX, dY + aY) == true) return cTmpDir;
        }

    if (cTurn == 1)
        for (i = cDir; i >= cDir - 7; i--)
        {
            cTmpDir = i;
            if (cTmpDir < 1) cTmpDir += 8;
            aX = _tmp_cTmpDirX[cTmpDir];
            aY = _tmp_cTmpDirY[cTmpDir];
            if (m_pMapList[cMapIndex]->bGetMoveable(dX + aX, dY + aY) == true) return cTmpDir;
        }

    return 0;
}

bool CGame::bGetEmptyPosition(short * pX, short * pY, char cMapIndex)
{
    int i;
    short sX, sY;


    for (i = 0; i < 25; i++)
        if ((m_pMapList[cMapIndex]->bGetMoveable(*pX + _tmp_cEmptyPosX[i], *pY + _tmp_cEmptyPosY[i]) == true) &&
            (m_pMapList[cMapIndex]->bGetIsTeleport(*pX + _tmp_cEmptyPosX[i], *pY + _tmp_cEmptyPosY[i]) == false))
        {
            sX = *pX + _tmp_cEmptyPosX[i];
            sY = *pY + _tmp_cEmptyPosY[i];
            *pX = sX;
            *pY = sY;
            return true;
        }

    GetMapInitialPoint(cMapIndex, &sX, &sY);
    *pX = sX;
    *pY = sY;

    return false;
}


void CGame::TargetSearch(int iNpcH, short * pTarget, char * pTargetType)
{
    int ix, iy, iPKCount;
    short sX, sY, rX, rY, dX, dY;
    short sOwner, sTargetOwner, sDistance, sTempDistance;
    char  cOwnerType, cTargetType, cTargetSide;
    int   iInv, iSearchType;

    sTargetOwner = 0;
    cTargetType = 0;
    sDistance = 100;

    sX = m_pNpcList[iNpcH]->m_sX;
    sY = m_pNpcList[iNpcH]->m_sY;


    rX = m_pNpcList[iNpcH]->m_sX - m_pNpcList[iNpcH]->m_cTargetSearchRange;
    rY = m_pNpcList[iNpcH]->m_sY - m_pNpcList[iNpcH]->m_cTargetSearchRange;

    iSearchType = 0;
    switch (m_pNpcList[iNpcH]->m_sType)
    {
        case 51: iSearchType = 1; break;
    }

    for (ix = rX; ix < rX + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; ix++)
        for (iy = rY; iy < rY + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; iy++)
        {

            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
            if (sOwner != 0)
            {

                if ((sOwner == iNpcH) && (cOwnerType == DEF_OWNERTYPE_NPC)) break;



                iPKCount = 0;
                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if (m_pClientList[sOwner] == 0)
                        {

                            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(5, sOwner, DEF_OWNERTYPE_PLAYER, ix, iy);
                        }
                        else
                        {
                            if (m_pClientList[sOwner]->m_iAdminUserLevel > 0) goto SKIP_SEARCH;
                            switch (iSearchType)
                            {
                                case 1: goto SKIP_SEARCH; break;
                                case 0: break;
                            }

                            dX = m_pClientList[sOwner]->m_sX;
                            dY = m_pClientList[sOwner]->m_sY;
                            cTargetSide = m_pClientList[sOwner]->m_cSide;
                            iPKCount = m_pClientList[sOwner]->m_iPKCount;
                            iInv = m_pClientList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];
                        }
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sOwner] == 0)
                        {

                            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(6, sOwner, DEF_OWNERTYPE_NPC, ix, iy);
                        }
                        else
                        {

                            switch (iSearchType)
                            {
                                case 1:
                                    switch (m_pNpcList[sOwner]->m_sType)
                                    {
                                        case 36:
                                        case 37:
                                        case 38:
                                        case 39:
                                        case 40:
                                        case 41: break;

                                        default:
                                            goto SKIP_SEARCH;
                                            break;
                                    }
                                    break;
                                case 0:
                                    break;
                            }

                            dX = m_pNpcList[sOwner]->m_sX;
                            dY = m_pNpcList[sOwner]->m_sY;
                            cTargetSide = m_pNpcList[sOwner]->m_cSide;
                            iPKCount = 0;
                            iInv = m_pNpcList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];

                            if (m_pNpcList[iNpcH]->m_sType == 21)
                            {

                                if (_iCalcPlayerNum(m_pNpcList[sOwner]->m_cMapIndex, dX, dY, 2) != 0)
                                {
                                    sOwner = 0;
                                    cOwnerType = 0;
                                }
                            }
                        }
                        break;
                }

                if (m_pNpcList[iNpcH]->m_cSide < 10)
                {

                    if (cTargetSide == 0)
                    {

                        if (iPKCount == 0) goto SKIP_SEARCH;
                    }
                    else
                    {

                        if ((iPKCount == 0) && (cTargetSide == m_pNpcList[iNpcH]->m_cSide)) goto SKIP_SEARCH;

                        if (m_pNpcList[iNpcH]->m_cSide == 0) goto SKIP_SEARCH;
                    }
                }
                else
                {

                    if ((cOwnerType == DEF_OWNERTYPE_NPC) && (cTargetSide == 0)) goto SKIP_SEARCH;
                    if (cTargetSide == m_pNpcList[iNpcH]->m_cSide) goto SKIP_SEARCH;
                }


                if ((iInv != 0) && (m_pNpcList[iNpcH]->m_cSpecialAbility != 1)) goto SKIP_SEARCH;

                if (abs(sX - dX) >= abs(sY - dY))
                    sTempDistance = abs(sX - dX);
                else sTempDistance = abs(sY - dY);

                if (sTempDistance < sDistance)
                {
                    sDistance = sTempDistance;
                    sTargetOwner = sOwner;
                    cTargetType = cOwnerType;
                }
                SKIP_SEARCH:;
            }
        }

    *pTarget = sTargetOwner;
    *pTargetType = cTargetType;
    return;
}



void CGame::RemoveFromTarget(short sTargetH, char cTargetType, int iCode)
{
    int i;
    uint32_t dwTime = timeGetTime();

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0)
        {


            if ((m_pNpcList[i]->m_iGuildGUID != 0) && (cTargetType == DEF_OWNERTYPE_PLAYER) &&
                (m_pClientList[sTargetH]->m_iGuildGUID == m_pNpcList[i]->m_iGuildGUID))
            {

                if (m_pNpcList[i]->m_cActionLimit == 0)
                {

                    m_pNpcList[i]->m_bIsSummoned = true;
                    m_pNpcList[i]->m_dwSummonedTime = dwTime;
                }
            }

            if ((m_pNpcList[i]->m_iTargetIndex == sTargetH) &&
                (m_pNpcList[i]->m_cTargetType == cTargetType))
            {

                switch (iCode)
                {
                    case DEF_MAGICTYPE_INVISIBILITY:
                        if (m_pNpcList[i]->m_cSpecialAbility == 1)
                        {

                        }
                        else
                        {
                            m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                            m_pNpcList[i]->m_iTargetIndex = 0;
                            m_pNpcList[i]->m_cTargetType = 0;
                        }
                        break;

                    default:
                        m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                        m_pNpcList[i]->m_iTargetIndex = 0;
                        m_pNpcList[i]->m_cTargetType = 0;
                        break;
                }
            }
        }
}


int CGame::iCalculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, bool bNearAttack, bool bIsDash)
{
    int    iAP_SM, iAP_L, iAttackerHitRatio, iTargetDefenseRatio, iDestHitRatio, iResult, iAP_Abs_Armor, iAP_Abs_Shield;
    char   cAttackerName[21], cAttackerDir, cAttackerSide, cTargetDir, cProtect = 0;
    short  sWeaponIndex, sAttackerWeapon, dX, dY, sX, sY, sAtkX, sAtkY, sTgtX, sTgtY;
    uint32_t  dwTime = timeGetTime();
    uint16_t   wWeaponType;
    double dTmp1, dTmp2, dTmp3;
    bool   bKilled = false;
    bool   bNormalMissileAttack = false;
    bool   bIsAttackerBerserk;
    int    iKilledDice, iDamage, iExp, iWepLifeOff, iSideCondition, iMaxSuperAttack, iWeaponSkill, iComboBonus, iTemp, iTemp2;
    int    iAttackerHP, iMoveDamage;
    char   cAttackerSA;
    int    iAttackerSAvalue, iHitPoint;
    char   cDamageMoveDir;
    int    iPartyID, iConstructionPoint, iWarContribution, tX, tY, iDst1, iDst2;
    int	iSkillLV, iCropLimit;
    iExp = 0;
    iPartyID = 0;

    memset(cAttackerName, 0, sizeof(cAttackerName));
    cAttackerSA = 0;
    iAttackerSAvalue = 0;
    wWeaponType = 0;


    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER:

            if (m_pClientList[sAttackerH] == 0) return 0;


            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsAttackEnabled == false) return 0;


            if (m_bIsCrusadeMode == false &&
                (m_pClientList[sAttackerH]->m_bIsHunter == true) && (cTargetType == DEF_OWNERTYPE_PLAYER)) return 0;


            if ((m_pClientList[sAttackerH]->m_iStatus & 0x10) != 0)
            {
                SetInvisibilityFlag(sAttackerH, DEF_OWNERTYPE_PLAYER, false);

                bRemoveFromDelayEventList(sAttackerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
                m_pClientList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
            }


            if ((m_pClientList[sAttackerH]->m_sAppr2 & 0xF000) == 0) return 0;

            iAP_SM = 0;
            iAP_L = 0;



            wWeaponType = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);

            //Change
            if ((bIsDash == true) && (m_pClientList[sAttackerH]->m_cSkillMastery[m_pClientList[sAttackerH]->m_sUsingWeaponSkill] != 100) && (wWeaponType != 25) && (wWeaponType != 27))
            {
                wsprintf(G_cTxt, "TSearch Fullswing Hack: (%s) Player: (%s) - dashing with only (%d) weapon skill.", m_pClientList[sAttackerH]->m_cIPaddress, m_pClientList[sAttackerH]->m_cCharName, m_pClientList[sAttackerH]->m_cSkillMastery[m_pClientList[sAttackerH]->m_sUsingWeaponSkill]);
                log->info(G_cTxt);
                DeleteClient(sAttackerH, true, true);
                return 0;
            }


            cAttackerSide = m_pClientList[sAttackerH]->m_cSide;

            if (wWeaponType == 0)
            {

                iAP_SM = iAP_L = iDice(1, (m_pClientList[sAttackerH]->m_iStr / 12));
                if (iAP_SM <= 0) iAP_SM = 1;
                if (iAP_L <= 0) iAP_L = 1;
                iAttackerHitRatio = m_pClientList[sAttackerH]->m_iHitRatio + m_pClientList[sAttackerH]->m_cSkillMastery[5];

                m_pClientList[sAttackerH]->m_sUsingWeaponSkill = 5;

            }
            else if ((wWeaponType >= 1) && (wWeaponType < 40))
            {

                iAP_SM = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_SM, m_pClientList[sAttackerH]->m_cAttackDiceRange_SM);
                iAP_L = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_L, m_pClientList[sAttackerH]->m_cAttackDiceRange_L);


                iAP_SM += m_pClientList[sAttackerH]->m_cAttackBonus_SM;
                iAP_L += m_pClientList[sAttackerH]->m_cAttackBonus_L;

                iAttackerHitRatio = m_pClientList[sAttackerH]->m_iHitRatio;


                dTmp1 = (double)iAP_SM;
                if (m_pClientList[sAttackerH]->m_iStr <= 0)
                    dTmp2 = 1.0f;
                else dTmp2 = (double)m_pClientList[sAttackerH]->m_iStr;

                dTmp2 = dTmp2 / 5.0f;
                dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
                iAP_SM = (int)(dTmp3 + 0.5f);

                dTmp1 = (double)iAP_L;
                if (m_pClientList[sAttackerH]->m_iStr <= 0)
                    dTmp2 = 1.0f;
                else dTmp2 = (double)m_pClientList[sAttackerH]->m_iStr;

                dTmp2 = dTmp2 / 5.0f;
                dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
                iAP_L = (int)(dTmp3 + 0.5f);
                //
            }
            else if (wWeaponType >= 40)
            {

                iAP_SM = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_SM, m_pClientList[sAttackerH]->m_cAttackDiceRange_SM);
                iAP_L = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_L, m_pClientList[sAttackerH]->m_cAttackDiceRange_L);


                iAP_SM += m_pClientList[sAttackerH]->m_cAttackBonus_SM;
                iAP_L += m_pClientList[sAttackerH]->m_cAttackBonus_L;

                iAttackerHitRatio = m_pClientList[sAttackerH]->m_iHitRatio;
                bNormalMissileAttack = true;


                iAP_SM += iDice(1, (m_pClientList[sAttackerH]->m_iStr / 20));
                iAP_L += iDice(1, (m_pClientList[sAttackerH]->m_iStr / 20));
                //
            }


            if (m_pClientList[sAttackerH]->m_iCustomItemValue_Attack != 0)
            {

                if ((m_pClientList[sAttackerH]->m_iMinAP_SM != 0) && (iAP_SM < m_pClientList[sAttackerH]->m_iMinAP_SM))
                {
                    iAP_SM = m_pClientList[sAttackerH]->m_iMinAP_SM;
                }
                if ((m_pClientList[sAttackerH]->m_iMinAP_L != 0) && (iAP_L < m_pClientList[sAttackerH]->m_iMinAP_L))
                {
                    iAP_L = m_pClientList[sAttackerH]->m_iMinAP_L;
                }

                if ((m_pClientList[sAttackerH]->m_iMaxAP_SM != 0) && (iAP_SM > m_pClientList[sAttackerH]->m_iMaxAP_SM))
                {
                    iAP_SM = m_pClientList[sAttackerH]->m_iMaxAP_SM;
                }
                if ((m_pClientList[sAttackerH]->m_iMaxAP_L != 0) && (iAP_L > m_pClientList[sAttackerH]->m_iMaxAP_L))
                {
                    iAP_L = m_pClientList[sAttackerH]->m_iMaxAP_L;
                }
            }

            cAttackerDir = m_pClientList[sAttackerH]->m_cDir;
            strcpy(cAttackerName, m_pClientList[sAttackerH]->m_cCharName);

            //
            if (m_pClientList[sAttackerH]->m_bHeroArmorBonus == 1)
            {
                iAttackerHitRatio += 100;
                iAP_SM += 5;
                iAP_L += 5;
            }

            if (m_pClientList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0)
                bIsAttackerBerserk = true;
            else bIsAttackerBerserk = false;


            if ((m_pClientList[sAttackerH]->m_iSuperAttackLeft > 0) && (iAttackMode >= 20))
            {


                dTmp1 = (double)iAP_SM;
                dTmp2 = (double)m_pClientList[sAttackerH]->m_iLevel;
                dTmp3 = dTmp2 / 100.0f;
                dTmp2 = dTmp1 * dTmp3;
                iTemp = (int)(dTmp2 + 0.5f);
                iAP_SM += iTemp;

                dTmp1 = (double)iAP_L;
                dTmp2 = (double)m_pClientList[sAttackerH]->m_iLevel;
                dTmp3 = dTmp2 / 100.0f;
                dTmp2 = dTmp1 * dTmp3;
                iTemp = (int)(dTmp2 + 0.5f);
                iAP_L += iTemp;


                switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill)
                {
                    case 6:  iAP_SM += (iAP_SM / 10); iAP_L += (iAP_L / 10); iAttackerHitRatio += 30; break;
                    case 7:  iAP_SM *= 2; iAP_L *= 2; break;
                    case 8:  iAP_SM += (iAP_SM / 10); iAP_L += (iAP_L / 10); iAttackerHitRatio += 30; break;
                    case 10:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5);                           break;
                    case 14:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); iAttackerHitRatio += 20; break;
                    case 21:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); iAttackerHitRatio += 50; break;

                    default: break;
                }


                iAttackerHitRatio += 100;

                iAttackerHitRatio += m_pClientList[sAttackerH]->m_iCustomItemValue_Attack;
            }


            if (bIsDash == true)
            {


                iAttackerHitRatio += 20;

                switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill)
                {
                    case 8:  iAP_SM += (iAP_SM / 10); iAP_L += (iAP_L / 10); break;
                    case 10: iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); break;
                    case 14: iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); break;
                    default: break;
                }
            }

            // v1.41
            iAttackerHP = m_pClientList[sAttackerH]->m_iHP;

            iAttackerHitRatio += m_pClientList[sAttackerH]->m_iAddAR;

            //v1.43
            sAtkX = m_pClientList[sAttackerH]->m_sX;
            sAtkY = m_pClientList[sAttackerH]->m_sY;

            iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sAttackerH] == 0) return 0;

            if (m_pMapList[m_pNpcList[sAttackerH]->m_cMapIndex]->m_bIsAttackEnabled == false) return 0;


            if ((m_pNpcList[sAttackerH]->m_iStatus & 0x10) != 0)
            {
                SetInvisibilityFlag(sAttackerH, DEF_OWNERTYPE_NPC, false);

                bRemoveFromDelayEventList(sAttackerH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_INVISIBILITY);
                m_pNpcList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
            }


            cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;

            iAP_SM = 0;
            iAP_L = 0;

            // Crusade
            if (m_pNpcList[sAttackerH]->m_cAttackDiceThrow != 0)
                iAP_L = iAP_SM = iDice(m_pNpcList[sAttackerH]->m_cAttackDiceThrow, m_pNpcList[sAttackerH]->m_cAttackDiceRange);

            iAttackerHitRatio = m_pNpcList[sAttackerH]->m_iHitRatio;

            cAttackerDir = m_pNpcList[sAttackerH]->m_cDir;
            memcpy(cAttackerName, m_pNpcList[sAttackerH]->m_cNpcName, 20);

            if (m_pNpcList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0)
                bIsAttackerBerserk = true;
            else bIsAttackerBerserk = false;

            // v1.41
            iAttackerHP = m_pNpcList[sAttackerH]->m_iHP;

            // v1.411
            cAttackerSA = m_pNpcList[sAttackerH]->m_cSpecialAbility;

            //v1.43
            sAtkX = m_pNpcList[sAttackerH]->m_sX;
            sAtkY = m_pNpcList[sAttackerH]->m_sY;
            break;
    }


    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:

            if (m_pClientList[sTargetH] == 0) return 0;

            if (m_pClientList[sTargetH]->m_bIsKilled == true) return 0;


            if ((m_bIsCrusadeMode == false) &&
                (m_pClientList[sTargetH]->m_iPKCount == 0) &&
                (m_pClientList[sTargetH]->m_bIsHunter == true) &&
                (cAttackerType == DEF_OWNERTYPE_PLAYER)) return 0;

#ifdef DEF_ALLPLAYERPROTECT

            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {
                if (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NEVERNONPK)
                {
                    if (m_pClientList[sTargetH]->m_iPKCount == 0)  return 0;
                }
                else
                {
                    if ((m_bIsCrusadeMode == false) &&
                        (m_pClientList[sTargetH]->m_iPKCount == 0) &&
                        (m_pClientList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide) &&
                        (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return 0;
                }
            }
#endif 

#ifdef DEF_SAMESIDETOWNPROTECT

            if ((m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_cSide == cAttackerSide) && (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return 0;
#endif 		
#ifdef DEF_BEGINNERTOWNPROTECT
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true)
                && (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return 0;
#endif

            if ((m_pClientList[sTargetH]->m_sX != tdX) || (m_pClientList[sTargetH]->m_sY != tdY)) return 0;


            if (m_pClientList[sTargetH]->m_iAdminUserLevel > 0) return 0;


            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return 0;


            if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return 0;

            cTargetDir = m_pClientList[sTargetH]->m_cDir;
            iTargetDefenseRatio = m_pClientList[sTargetH]->m_iDefenseRatio;


            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true))
            {
                iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
                if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
                {

                    iAP_SM = iAP_SM / 2;
                    iAP_L = iAP_L / 2;
                }
                else
                {

                    if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)
                    {
                        if (m_pClientList[sAttackerH]->m_iGuildGUID == m_pClientList[sTargetH]->m_iGuildGUID) return 0;
                        else
                        {

                            iAP_SM = iAP_SM / 2;
                            iAP_L = iAP_L / 2;
                        }
                    }
                    else return 0;
                }
            }

            /*

            if (cAttackerType == DEF_OWNERTYPE_PLAYER) {

            if ((m_pClientList[sAttackerH]->m_cMapIndex == m_iMiddlelandMapIndex) && (m_pClientList[sAttackerH]->m_cSide > 0)) {
            aX = m_pClientList[sAttackerH]->m_sX;
            aY = m_pClientList[sAttackerH]->m_sY;
            pTile = (CTile *)(m_pMapList[m_iMiddlelandMapIndex]->m_pTile + aX + aY*m_pMapList[m_iMiddlelandMapIndex]->m_sSizeY);
            if (pTile->m_iOccupyStatus != 0) {

            if (pTile->m_iOccupyStatus < 0) {

            if (m_pClientList[sAttackerH]->m_cSide == 1) {

            if (abs(pTile->m_iOccupyStatus) > 100)
            iAttackerHitRatio += 100;
            else iAttackerHitRatio += abs(pTile->m_iOccupyStatus);
            }
            }
            else if (pTile->m_iOccupyStatus > 0) {

            if (m_pClientList[sAttackerH]->m_cSide == 2) {

            if (abs(pTile->m_iOccupyStatus) > 100)
            iAttackerHitRatio += 100;
            else iAttackerHitRatio += abs(pTile->m_iOccupyStatus);
            }
            }
            }
            }
            }
            */

            // v1.42 Ãß?? ?æ?î?ª 
            iTargetDefenseRatio += m_pClientList[sTargetH]->m_iAddDR;

            //v1.43
            sTgtX = m_pClientList[sTargetH]->m_sX;
            sTgtY = m_pClientList[sTargetH]->m_sY;
            break;

        case DEF_OWNERTYPE_NPC:

            if (m_pNpcList[sTargetH] == 0) return 0;

            if (m_pNpcList[sTargetH]->m_iHP <= 0) return 0;

            if ((m_pNpcList[sTargetH]->m_sX != tdX) || (m_pNpcList[sTargetH]->m_sY != tdY)) return 0;

            cTargetDir = m_pNpcList[sTargetH]->m_cDir;
            iTargetDefenseRatio = m_pNpcList[sTargetH]->m_iDefenseRatio;


            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {


                switch (m_pNpcList[sTargetH]->m_sType)
                {
                    case 40:
                    case 41:
                        if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return 0;
                        break;
                }

                /*

                if ((m_pClientList[sAttackerH]->m_cMapIndex == m_iMiddlelandMapIndex) && (m_pClientList[sAttackerH]->m_cSide > 0)) {
                aX = m_pClientList[sAttackerH]->m_sX;
                aY = m_pClientList[sAttackerH]->m_sY;
                pTile = (CTile *)(m_pMapList[m_iMiddlelandMapIndex]->m_pTile + aX + aY*m_pMapList[m_iMiddlelandMapIndex]->m_sSizeY);
                if (pTile->m_iOccupyStatus != 0) {

                if (pTile->m_iOccupyStatus < 0) {

                if (m_pClientList[sAttackerH]->m_cSide == 1) {

                if (abs(pTile->m_iOccupyStatus) > 100)
                iAttackerHitRatio += 50;
                else iAttackerHitRatio += abs(pTile->m_iOccupyStatus)/2;
                }
                }
                else if (pTile->m_iOccupyStatus > 0) {

                if (m_pClientList[sAttackerH]->m_cSide == 2) {

                if (abs(pTile->m_iOccupyStatus) > 100)
                iAttackerHitRatio += 50;
                else iAttackerHitRatio += abs(pTile->m_iOccupyStatus)/2;
                }
                }
                }
                }
                */


                if ((wWeaponType == 25) && (m_pNpcList[sTargetH]->m_cActionLimit == 5) && (m_pNpcList[sTargetH]->m_iBuildCount > 0))
                {


                    if ((m_pClientList[sAttackerH]->m_iCrusadeDuty != 2) && (m_pClientList[sAttackerH]->m_iAdminUserLevel == 0)) break;

                    switch (m_pNpcList[sTargetH]->m_sType)
                    {
                        case 36:
                        case 37:
                        case 38:
                        case 39:
                            switch (m_pNpcList[sTargetH]->m_iBuildCount)
                            {
                                case 1:
                                    m_pNpcList[sTargetH]->m_sAppr2 = 0;
                                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);


                                    switch (m_pNpcList[sTargetH]->m_sType)
                                    {
                                        case 36: iConstructionPoint = 700; iWarContribution = 700; break;
                                        case 37: iConstructionPoint = 700; iWarContribution = 700; break;
                                        case 38: iConstructionPoint = 500; iWarContribution = 500; break;
                                        case 39: iConstructionPoint = 500; iWarContribution = 500; break;
                                    }

                                    m_pClientList[sAttackerH]->m_iWarContribution += iWarContribution;
                                    if (m_pClientList[sAttackerH]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                                        m_pClientList[sAttackerH]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                                    //testcode
                                    wsprintf(G_cTxt, "Construction Complete! WarContribution: +%d", iWarContribution);
                                    log->info(G_cTxt);

                                    SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, 0, 0);
                                    break;

                                case 5:
                                    m_pNpcList[sTargetH]->m_sAppr2 = 1;
                                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                                    break;
                                case 10:
                                    m_pNpcList[sTargetH]->m_sAppr2 = 2;
                                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                                    break;
                            }
                            break;
                    }

                    m_pNpcList[sTargetH]->m_iBuildCount--;
                    if (m_pNpcList[sTargetH]->m_iBuildCount <= 0)
                    {
                        m_pNpcList[sTargetH]->m_iBuildCount = 0;
                    }
                    return 0;
                }

                //v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã

                if ((wWeaponType == 27) && (m_pNpcList[sTargetH]->m_iNpcCrops != 0) && (m_pNpcList[sTargetH]->m_cActionLimit == 5) && (m_pNpcList[sTargetH]->m_iBuildCount > 0))
                {

                    iSkillLV = m_pClientList[sAttackerH]->m_cSkillMastery[2];
                    iCropLimit = m_pNpcList[sTargetH]->m_iCropsSkillLV;

                    if (20 > iSkillLV) return 0;  //v2.19 2002-12-16 ½ºÅ³·¹º§ 20ºÎÅÍ °¡´É..

                    if (m_pClientList[sAttackerH]->m_iLevel < 20) return 0; //v2.20 2002-12-20 ³ó»ç ½ºÅ³ ·¹º§ 20ºÎÅÍ °¡´É


                    switch (m_pNpcList[sTargetH]->m_sType)
                    {
                        case 64:
                            switch (m_pNpcList[sTargetH]->m_iBuildCount)
                            {
                                case 1:

                                    m_pNpcList[sTargetH]->m_sAppr2 = m_pNpcList[sTargetH]->m_iNpcCrops << 8 | 3;
                                    //						m_pNpcList[sTargetH]->m_sAppr2 = (char)0;

                                    if (iSkillLV <= iCropLimit + 10)
                                        CalculateSSN_SkillIndex(sAttackerH, 2, 1);

                                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

                                    //³óÀÛ¹°À» ¶³¾îÆ®¸°´Ù.
                                    if (bProbabilityTable(iSkillLV, iCropLimit, 2) > 0)
                                    {
                                        bCropsItemDrop(sAttackerH, sTargetH, true);
                                    }
                                    DeleteNpc(sTargetH);

                                    break;
                                case 8:
                                    m_pNpcList[sTargetH]->m_sAppr2 = m_pNpcList[sTargetH]->m_iNpcCrops << 8 | 3;
                                    //						m_pNpcList[sTargetH]->m_sAppr2 = (char)3;

                                    if (iSkillLV <= iCropLimit + 10)
                                        CalculateSSN_SkillIndex(sAttackerH, 2, 1);

                                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

                                    //³óÀÛ¹°À» ¶³¾îÆ®¸°´Ù.
                                    if (bProbabilityTable(iSkillLV, iCropLimit, 2) > 0)
                                    {
                                        if (iSkillLV <= iCropLimit + 10)
                                            CalculateSSN_SkillIndex(sAttackerH, 2, 1);
                                        bCropsItemDrop(sAttackerH, sTargetH);
                                    }
                                    break;

                                case 18:
                                    m_pNpcList[sTargetH]->m_sAppr2 = m_pNpcList[sTargetH]->m_iNpcCrops << 8 | 2;
                                    //						m_pNpcList[sTargetH]->m_sAppr2 = (char)2;

                                    if (iSkillLV <= iCropLimit + 10)
                                        CalculateSSN_SkillIndex(sAttackerH, 2, 1);

                                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

                                    //³óÀÛ¹°À» ¶³¾îÆ®¸°´Ù.
                                    if (bProbabilityTable(iSkillLV, iCropLimit, 2) > 0)
                                    {
                                        if (iSkillLV <= iCropLimit + 10)
                                            CalculateSSN_SkillIndex(sAttackerH, 2, 1);
                                        bCropsItemDrop(sAttackerH, sTargetH);
                                    }
                                    break;
                            }
                            break;
                    }

                    if (m_pNpcList[sTargetH] == 0)
                    {
                        return 0;
                    }

                    if (bProbabilityTable(iSkillLV, iCropLimit, 1) > 0
                        || m_pNpcList[sTargetH]->m_iBuildCount == 1
                        || m_pNpcList[sTargetH]->m_iBuildCount == 8
                        || m_pNpcList[sTargetH]->m_iBuildCount == 18)
                    {

                        iTemp = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];

                        if ((iTemp != -1) || (m_pClientList[sAttackerH]->m_pItemList[iTemp] != 0))
                        {
                            //È£¹Ì ¼ö¸íÀ» ÁÙÀÎ´Ù.
                            if (m_pClientList[sAttackerH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)
                                m_pClientList[sAttackerH]->m_pItemList[iTemp]->m_wCurLifeSpan--;

                            if (m_pClientList[sAttackerH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
                            {


                                SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sAttackerH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                ReleaseItemHandler(sAttackerH, iTemp, true);
                            }
                        }

                        //¼ºÀå Ä«¿îÆ®...
                        m_pNpcList[sTargetH]->m_iBuildCount--;
                    }

                    if (m_pNpcList[sTargetH]->m_iBuildCount <= 0)
                    {
                        m_pNpcList[sTargetH]->m_iBuildCount = 0;
                    }

                    return 0;
                }
            }

            //v1.43
            sTgtX = m_pNpcList[sTargetH]->m_sX;
            sTgtY = m_pNpcList[sTargetH]->m_sY;
            break;
    }


    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (cTargetType == DEF_OWNERTYPE_PLAYER))
    {

        sX = m_pClientList[sAttackerH]->m_sX;
        sY = m_pClientList[sAttackerH]->m_sY;

        dX = m_pClientList[sTargetH]->m_sX;
        dY = m_pClientList[sTargetH]->m_sY;


        if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(sX, sY, 0x00000006) != 0) return 0;
        if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(dX, dY, 0x00000006) != 0) return 0;
    }


    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        if (m_pClientList[sAttackerH]->m_iDex > 50)
        {
            iAttackerHitRatio += (m_pClientList[sAttackerH]->m_iDex - 50);
        }
    }


    if (wWeaponType >= 40)
    {
        switch (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cWhetherStatus)
        {
            case 0:	break;
            case 1:	iAttackerHitRatio = iAttackerHitRatio - (iAttackerHitRatio / 20); break;
            case 2:	iAttackerHitRatio = iAttackerHitRatio - (iAttackerHitRatio / 10); break;
            case 3:	iAttackerHitRatio = iAttackerHitRatio - (iAttackerHitRatio / 4);  break;
        }
    }

    if (iAttackerHitRatio < 0)   iAttackerHitRatio = 0;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            cProtect = m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT];

            break;

        case DEF_OWNERTYPE_NPC:
            cProtect = m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT];
            break;
    }



    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        // BUG POINT! Item == 0
        if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
        {
            //#ERROR POINT!
            if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]] == 0)
            {

                m_pClientList[sAttackerH]->m_bIsItemEquipped[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]] = false;
                DeleteClient(sAttackerH, true, true);
                return 0;
            }

            if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_ARROW)
            {
                if (m_pClientList[sAttackerH]->m_cArrowIndex == -1)
                {

                    return 0;
                }
                else
                {

                    if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex] == 0)
                        return 0;


                    m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex]->m_dwCount--;
                    if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex]->m_dwCount <= 0)
                    {


                        ItemDepleteHandler(sAttackerH, m_pClientList[sAttackerH]->m_cArrowIndex, false);

                        m_pClientList[sAttackerH]->m_cArrowIndex = _iGetArrowItemIndex(sAttackerH);
                    }
                    else
                    {

                        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_SETITEMCOUNT, m_pClientList[sAttackerH]->m_cArrowIndex, m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex]->m_dwCount, (char)false, 0);


                        iCalcTotalWeight(sAttackerH);
                    }
                }

                if (cProtect == 1) return 0;
            }
            else
            {


                switch (cProtect)
                {
                    case 3: iTargetDefenseRatio += 40;  break; //  Magic Shield
                    case 4: iTargetDefenseRatio += 100; break; //  Great Magic Shield
                }
                if (iTargetDefenseRatio < 0) iTargetDefenseRatio = 1;
            }
        }
    }
    else
    {

        switch (cProtect)
        {
            case 1:
                switch (m_pNpcList[sAttackerH]->m_sType)
                {
                    case 54:
                        if ((abs(sTgtX - m_pNpcList[sAttackerH]->m_sX) >= 1) || (abs(sTgtY - m_pNpcList[sAttackerH]->m_sY) >= 1)) return 0;
                }
                break;
            case 3: iTargetDefenseRatio += 40;  break; //  Magic Shield
            case 4: iTargetDefenseRatio += 100; break; //  Great Magic Shield
        }
        if (iTargetDefenseRatio < 0) iTargetDefenseRatio = 1;
    }

    //--------------

    if (cAttackerDir == cTargetDir) iTargetDefenseRatio = iTargetDefenseRatio / 2;


    if (iTargetDefenseRatio < 1)   iTargetDefenseRatio = 1;


    dTmp1 = (double)(iAttackerHitRatio);
    dTmp2 = (double)(iTargetDefenseRatio);

    dTmp3 = (dTmp1 / dTmp2) * 50.0f;
    iDestHitRatio = (int)(dTmp3);


    if (iDestHitRatio < DEF_MINIMUMHITRATIO) iDestHitRatio = DEF_MINIMUMHITRATIO;

    if (iDestHitRatio > DEF_MAXIMUMHITRATIO) iDestHitRatio = DEF_MAXIMUMHITRATIO;


    if ((bIsAttackerBerserk == true) && (iAttackMode < 20))
    {
        iAP_SM = iAP_SM * 2;
        iAP_L = iAP_L * 2;
    }


    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        iAP_SM += m_pClientList[sAttackerH]->m_iAddPhysicalDamage;
        iAP_L += m_pClientList[sAttackerH]->m_iAddPhysicalDamage;
    }



    if (bNearAttack == true)
    {

        iAP_SM = iAP_SM / 2;
        iAP_L = iAP_L / 2;
    }


    if (cTargetType == DEF_OWNERTYPE_PLAYER)
    {
        iAP_SM -= (iDice(1, m_pClientList[sTargetH]->m_iVit / 10) - 1);
        iAP_L -= (iDice(1, m_pClientList[sTargetH]->m_iVit / 10) - 1);
    }


    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        if (iAP_SM <= 1) iAP_SM = 1;
        if (iAP_L <= 1) iAP_L = 1;
    }
    else
    {

        if (iAP_SM <= 0) iAP_SM = 0;
        if (iAP_L <= 0) iAP_L = 0;
    }

    iResult = iDice(1, 100);
    //------------

    if (iResult <= iDestHitRatio)
    {

        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        {



            if (((m_pClientList[sAttackerH]->m_iHungerStatus <= 10) || (m_pClientList[sAttackerH]->m_iSP <= 0)) &&
                (iDice(1, 10) == 5)) return false;


            m_pClientList[sAttackerH]->m_iComboAttackCount++;
            if (m_pClientList[sAttackerH]->m_iComboAttackCount < 0) m_pClientList[sAttackerH]->m_iComboAttackCount = 0;
            if (m_pClientList[sAttackerH]->m_iComboAttackCount > 4) m_pClientList[sAttackerH]->m_iComboAttackCount = 1;


            iWeaponSkill = _iGetWeaponSkillType(sAttackerH);
            iComboBonus = iGetComboAttackBonus(iWeaponSkill, m_pClientList[sAttackerH]->m_iComboAttackCount);

            //v1.42 ???Ó ?ø?Ý ?ë?ÌÁö ?öÇÏ?â?
            if ((m_pClientList[sAttackerH]->m_iComboAttackCount > 1) && (m_pClientList[sAttackerH]->m_iAddCD != 0))
                iComboBonus += m_pClientList[sAttackerH]->m_iAddCD;

            iAP_SM += iComboBonus;
            iAP_L += iComboBonus;


            switch (m_pClientList[sAttackerH]->m_iSpecialWeaponEffectType)
            {
                case 0: break;
                case 1:
                    if ((m_pClientList[sAttackerH]->m_iSuperAttackLeft > 0) && (iAttackMode >= 20))
                    {
                        iAP_SM += m_pClientList[sAttackerH]->m_iSpecialWeaponEffectValue;
                        iAP_L += m_pClientList[sAttackerH]->m_iSpecialWeaponEffectValue;
                    }
                    break;

                case 2:
                    cAttackerSA = 61;
                    iAttackerSAvalue = m_pClientList[sAttackerH]->m_iSpecialWeaponEffectValue * 5;
                    break;

                case 3:
                    cAttackerSA = 62;
                    break;
            }


            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)
            {
                iAP_SM += iAP_SM / 3;
                iAP_L += iAP_L / 3;
            }


            if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == true) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
            {

                if (m_pClientList[sAttackerH]->m_iLevel <= 80)
                {
                    iAP_SM += iAP_SM;
                    iAP_L += iAP_L;
                }

                else if (m_pClientList[sAttackerH]->m_iLevel <= 100)
                {
                    iAP_SM += (iAP_SM * 7) / 10;
                    iAP_L += (iAP_L * 7) / 10;
                }
                else
                {
                    iAP_SM += iAP_SM / 3; ;
                    iAP_L += iAP_L / 3;
                }
            }
        }

        switch (cTargetType)
        {
            case DEF_OWNERTYPE_PLAYER:

                ClearSkillUsingStatus(sTargetH);


                if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME)
                {

                    return 0;
                }
                else
                {
                    // v1.42 
                    switch (cAttackerSA)
                    {
                        case 62:
                            if (m_pClientList[sTargetH]->m_iRating < 0)
                            {

                                iTemp = abs(m_pClientList[sTargetH]->m_iRating) / 10;
                                if (iTemp > 10) iTemp = 10;
                                iAP_SM += iTemp;
                            }
                            break;
                    }


                    iAP_Abs_Armor = 0;
                    iAP_Abs_Shield = 0;

                    iTemp = iDice(1, 10000);
                    if ((iTemp >= 1) && (iTemp < 5000))           iHitPoint = 1;
                    else if ((iTemp >= 5000) && (iTemp < 7500))   iHitPoint = 2;
                    else if ((iTemp >= 7500) && (iTemp < 9000))   iHitPoint = 3;
                    else if ((iTemp >= 9000) && (iTemp <= 10000)) iHitPoint = 4;

                    switch (iHitPoint)
                    {
                        case 1:
                            if (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] > 0)
                            {

                                if (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] >= 80)
                                    dTmp1 = 80.0f;
                                else dTmp1 = (double)m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY];
                                dTmp2 = (double)iAP_SM;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;

                                iAP_Abs_Armor = (int)dTmp3;
                            }
                            break;

                        case 2:
                            if ((m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_PANTS] +
                                m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS]) > 0)
                            {

                                if ((m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_PANTS] +
                                    m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS]) >= 80)
                                    dTmp1 = 80.0f;
                                else dTmp1 = (double)(m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_PANTS] + m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS]);
                                dTmp2 = (double)iAP_SM;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;

                                iAP_Abs_Armor = (int)dTmp3;
                            }
                            break;

                        case 3:
                            if (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] > 0)
                            {

                                if (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] >= 80)
                                    dTmp1 = 80.0f;
                                else dTmp1 = (double)m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS];
                                dTmp2 = (double)iAP_SM;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;

                                iAP_Abs_Armor = (int)dTmp3;
                            }
                            break;

                        case 4:
                            if (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] > 0)
                            {

                                if (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] >= 80)
                                    dTmp1 = 80.0f;
                                else dTmp1 = (double)m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD];
                                dTmp2 = (double)iAP_SM;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;

                                iAP_Abs_Armor = (int)dTmp3;
                            }
                            break;
                    }

                    if (m_pClientList[sTargetH]->m_iDamageAbsorption_Shield > 0)
                    {
                        if (iDice(1, 100) <= (m_pClientList[sTargetH]->m_cSkillMastery[11]))
                        {

                            CalculateSSN_SkillIndex(sTargetH, 11, 1);

                            if (m_pClientList[sTargetH]->m_iDamageAbsorption_Shield >= 80)
                                dTmp1 = 80.0f;
                            else dTmp1 = (double)m_pClientList[sTargetH]->m_iDamageAbsorption_Shield;
                            dTmp2 = (double)iAP_SM;
                            dTmp3 = (dTmp1 / 100.0f) * dTmp2;

                            iAP_Abs_Shield = (int)dTmp3;


                            iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND];
                            if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
                            {


                                if ((m_pClientList[sTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan--;

                                if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan == 0)
                                {


                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                    ReleaseItemHandler(sTargetH, iTemp, true);
                                }
                            }
                        }
                    }

                    iAP_SM = iAP_SM - (iAP_Abs_Armor + iAP_Abs_Shield);
                    if (iAP_SM <= 0) iAP_SM = 1;

                    //v1.432 Æ¯¼ö ´É·Â 1ÀÌ È°¼ºÈ­ µÇ¾î ÀÖÀ¸¸é ´ëÀÎ °ø°Ý½Ã¿¡ ´ë¹ÌÁö 
                    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_bIsSpecialAbilityEnabled == true))
                    {
                        switch (m_pClientList[sAttackerH]->m_iSpecialAbilityType)
                        {
                            case 0: break;
                            case 1:
                                iTemp = (m_pClientList[sTargetH]->m_iHP / 2);
                                if (iTemp > iAP_SM) iAP_SM = iTemp;
                                if (iAP_SM <= 0) iAP_SM = 1;
                                break;

                            case 2:
                                if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                {
                                    m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                    SetIceFlag(sTargetH, DEF_OWNERTYPE_PLAYER, true);

                                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (30 * 1000),
                                        sTargetH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);

                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                }
                                break;

                            case 3:
                                if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 0)
                                {
                                    m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 2;

                                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + (10 * 1000),
                                        sTargetH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 10, 0, 0);

                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_HOLDOBJECT, 10, 0, 0);
                                }
                                break;

                            case 4:
                                iAP_SM = (m_pClientList[sTargetH]->m_iHP);
                                break;

                            case 5:
                                m_pClientList[sAttackerH]->m_iHP += iAP_SM;
                                if (iGetMaxHP(sAttackerH) < m_pClientList[sAttackerH]->m_iHP) m_pClientList[sAttackerH]->m_iHP = iGetMaxHP(sAttackerH);
                                SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
                                break;
                        }
                    }


                    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true))
                    {
                        switch (m_pClientList[sTargetH]->m_iSpecialAbilityType)
                        {
                            case 50:
                                if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
                                    sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                                else sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                                if (sWeaponIndex != -1)	m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan = 0;
                                break;

                            case 51:
                                if (iHitPoint == m_pClientList[sTargetH]->m_iSpecialAbilityEquipPos)
                                    iAP_SM = 0;
                                break;

                            case 52:
                                iAP_SM = 0;
                                break;
                        }
                    }

                    if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == true) &&
                        (iDice(1, 10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iAP_SM))
                    {

                        iAP_SM = m_pClientList[sTargetH]->m_iHP - 1;
                    }





                    int iDownValue = 1;
                    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH] != 0) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sTargetH]->m_cSide != m_pClientList[sAttackerH]->m_cSide))
                    {

                        switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill)
                        {
                            case 14:

                                if (31 == ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4))
                                {
                                    //v2.20 2003-1-28 ÀÚÀÌ?ðÆ? ?èÆ?ÇØ?Ó. ?ö?? ?öÁ?.. 
                                    iTemp2 = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                                    if ((iTemp2 != -1) && (m_pClientList[sAttackerH]->m_pItemList[iTemp2] != 0))
                                    {
                                        if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 761)
                                        {
                                            iDownValue = 30;
                                        }
                                        else if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 762)
                                        {
                                            iDownValue = 35;
                                        }
                                    }
                                }
                                //							iDownValue = 30;
                                //						else if(32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))  //v2.20 2002-12-18 ÀÚÀÌ?ðÆ? ?èÆ?ÇØ?Ó. 
                                //							iDownValue = 35;
                                else
                                    iDownValue = 20;
                                break;


                            case 10: iDownValue = 3; break;
                            default: iDownValue = 1; break;
                        }

                    }

                    int iHammerChance = 100; // v2.172

                    switch (iHitPoint)
                    {
                        case 1:

                            iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
                            if (iTemp == -1) //v2.19 2002-12-11 ?êÅ?º? ?ö?í DEF_EQUIPPOS_FULLBODY?æ?ì??µµ DEF_EQUIPPOS_BODYÃ??? ?è?ê ÇÑ?Ù.
                            {
                                iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_FULLBODY];
                            }
                            if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
                            {


                                if ((m_pClientList[sTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iDownValue; // v2.16

                                if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
                                {


                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                    ReleaseItemHandler(sTargetH, iTemp, true);
                                }
                                else if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14))
                                { // v2.172


                                    if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan < 2000)
                                        iHammerChance = iDice(6, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));
                                    else
                                        iHammerChance = iDice(4, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));


                                    if (31 == ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4))
                                    {
                                        iTemp2 = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                                        if ((iTemp2 != -1) && (m_pClientList[sAttackerH]->m_pItemList[iTemp2] != 0))
                                        {
                                            if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 761)
                                            {
                                                iHammerChance = iHammerChance / 2;
                                            }
                                            else if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 762)
                                            {
                                                iHammerChance = (5 * iHammerChance) / 8;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                            }
                                        }
                                    }
                                    //								iHammerChance = iHammerChance / 2 ;
                                    //							else if(32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))  
                                    //								iHammerChance = (5 * iHammerChance) / 8 ;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                    else
                                        iHammerChance = iHammerChance / 4;


                                    switch (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_sIDnum)
                                    {
                                        case 621:
                                        case 622:
                                            iHammerChance = 0;
                                            break;
                                    }

                                    if (iHammerChance > m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan)
                                    {

                                        ReleaseItemHandler(sTargetH, iTemp, true);


                                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
                                    }
                                }
                            }
                            break;

                        case 2:

                            iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
                            if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
                            {


                                if ((m_pClientList[sTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iDownValue; // v2.16

                                if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
                                {


                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                    ReleaseItemHandler(sTargetH, iTemp, true);
                                }
                                else if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14))
                                { // v2.172

                                    if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan < 2000)
                                        iHammerChance = iDice(6, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));
                                    else
                                        iHammerChance = iDice(4, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));



                                    if (31 == ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4))
                                    {
                                        iTemp2 = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                                        if ((iTemp2 != -1) && (m_pClientList[sAttackerH]->m_pItemList[iTemp2] != 0))
                                        {
                                            if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 761)
                                            {
                                                iHammerChance = iHammerChance / 2;
                                            }
                                            else if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 762)
                                            {
                                                iHammerChance = (5 * iHammerChance) / 8;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                            }
                                        }
                                    }
                                    //								iHammerChance = iHammerChance / 2 ;
                                    //							else if(32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))  
                                    //								iHammerChance = (5 * iHammerChance) / 8 ;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                    else
                                        iHammerChance = iHammerChance / 4;


                                    switch (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_sIDnum)
                                    {
                                        case 621:
                                        case 622:
                                            iHammerChance = 0;
                                            break;
                                    }

                                    if (iHammerChance > m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan)
                                    {


                                        ReleaseItemHandler(sTargetH, iTemp, true);


                                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
                                    }

                                }
                            }

                            iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
                            if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
                            {


                                if ((m_pClientList[sTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iDownValue; // v2.16

                                if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
                                {


                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                    ReleaseItemHandler(sTargetH, iTemp, true);
                                }
                                else if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14))
                                { // v2.172

                                    if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan < 2000)
                                        iHammerChance = iDice(6, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));
                                    else
                                        iHammerChance = iDice(4, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));


                                    if (31 == ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4))
                                    {
                                        iTemp2 = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                                        if ((iTemp2 != -1) && (m_pClientList[sAttackerH]->m_pItemList[iTemp2] != 0))
                                        {
                                            if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 761)
                                            {
                                                iHammerChance = iHammerChance / 2;
                                            }
                                            else if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 762)
                                            {
                                                iHammerChance = (5 * iHammerChance) / 8;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                            }
                                        }
                                    }
                                    //								iHammerChance = iHammerChance / 2 ;
                                    //							else if(32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))  
                                    //								iHammerChance = (5 * iHammerChance) / 8 ;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                    else
                                        iHammerChance = iHammerChance / 4;


                                    switch (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_sIDnum)
                                    {
                                        case 621:
                                        case 622:
                                            iHammerChance = 0;
                                            break;
                                    }


                                    if (iHammerChance > m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan)
                                    {

                                        ReleaseItemHandler(sTargetH, iTemp, true);

                                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                    }
                                }
                            }
                            break;

                        case 3:

                            iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
                            if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
                            {


                                if ((m_pClientList[sTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iDownValue; // v2.16

                                if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
                                {


                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                    ReleaseItemHandler(sTargetH, iTemp, true);
                                }
                                else if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14))
                                { // v2.172

                                    if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan < 2000)
                                        iHammerChance = iDice(6, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));
                                    else
                                        iHammerChance = iDice(4, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));


                                    if (31 == ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4))
                                    {
                                        iTemp2 = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                                        if ((iTemp2 != -1) && (m_pClientList[sAttackerH]->m_pItemList[iTemp2] != 0))
                                        {
                                            if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 761)
                                            {
                                                iHammerChance = iHammerChance / 2;
                                            }
                                            else if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 762)
                                            {
                                                iHammerChance = (5 * iHammerChance) / 8;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                            }
                                        }
                                    }
                                    //								iHammerChance = iHammerChance / 2 ;
                                    //							else if(32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))  
                                    //								iHammerChance = (5 * iHammerChance) / 8 ;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                    else
                                        iHammerChance = iHammerChance / 4;


                                    switch (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_sIDnum)
                                    {
                                        case 621:
                                        case 622:
                                            iHammerChance = 0;
                                            break;
                                    }

                                    if (iHammerChance > m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan)
                                    {


                                        ReleaseItemHandler(sTargetH, iTemp, true);

                                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
                                    }
                                }
                            }
                            break;

                        case 4:

                            iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
                            if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
                            {


                                if ((m_pClientList[sTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iDownValue; // v2.16

                                if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
                                {


                                    m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

                                    ReleaseItemHandler(sTargetH, iTemp, true);
                                }
                                else if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14))
                                { // v2.172

                                    if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan < 3000)
                                        iHammerChance = iDice(6, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));
                                    else
                                        iHammerChance = iDice(4, (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan));


                                    if (31 == ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4))
                                    {
                                        iTemp2 = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                                        if ((iTemp2 != -1) && (m_pClientList[sAttackerH]->m_pItemList[iTemp2] != 0))
                                        {
                                            if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 761)
                                            {
                                                iHammerChance = iHammerChance / 2;
                                            }
                                            else if (m_pClientList[sAttackerH]->m_pItemList[iTemp2]->m_sIDnum == 762)
                                            {
                                                iHammerChance = (5 * iHammerChance) / 8;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                            }
                                        }
                                    }
                                    //								iHammerChance = iHammerChance / 2 ;
                                    //							else if(32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))  
                                    //								iHammerChance = (5 * iHammerChance) / 8 ;  //v2.20 2002-12-18 ÀÚÀÌ¾ðÆ® ¹èÆ²ÇØ¸Ó.
                                    else
                                        iHammerChance = iHammerChance / 4;


                                    switch (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_sIDnum)
                                    {
                                        case 621:
                                        case 622:
                                            iHammerChance = 0;
                                            break;
                                    }

                                    if (iHammerChance > m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan)
                                    {

                                        ReleaseItemHandler(sTargetH, iTemp, true);

                                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
                                    }
                                }
                            }
                            break;
                    }

                    //if ((cAttackerSA == 2) && (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0)) {
                    //	SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_PROTECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT ], 0, 0);
                    //	switch(m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT]) {
                    //	case 1:
                    //		SetProtectionFromArrowFlag(sTargetH, DEF_OWNERTYPE_PLAYER, false);
                    //		break;
                    //	case 2:
                    //	case 5:
                    //		SetMagicProtectionFlag(sTargetH, DEF_OWNERTYPE_PLAYER, false);
                    //		break;
                    //	case 3:
                    //	case 4:
                    //		SetDefenseShieldFlag(sTargetH, DEF_OWNERTYPE_PLAYER, false);
                    //		break;
                    //	}
                    //	m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_PROTECT] = 0;
                    //	bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_PROTECT);
                    //}
                    //Change Status Effects

                    if ((cAttackerSA == 2) && (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0))
                    {
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_PROTECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT], 0, 0);

                        m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = 0;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_PROTECT);
                    }


                    if ((m_pClientList[sTargetH]->m_bIsPoisoned == false) &&
                        ((cAttackerSA == 5) || (cAttackerSA == 6) || (cAttackerSA == 61)))
                    {

                        if (bCheckResistingPoisonSuccess(sTargetH, DEF_OWNERTYPE_PLAYER) == false)
                        {

                            m_pClientList[sTargetH]->m_bIsPoisoned = true;
                            if (cAttackerSA == 5)		m_pClientList[sTargetH]->m_iPoisonLevel = 15;
                            else if (cAttackerSA == 6)  m_pClientList[sTargetH]->m_iPoisonLevel = 40;
                            else if (cAttackerSA == 61) m_pClientList[sTargetH]->m_iPoisonLevel = iAttackerSAvalue;

                            m_pClientList[sTargetH]->m_dwPoisonTime = dwTime;

                            SetPoisonFlag(sTargetH, DEF_OWNERTYPE_PLAYER, true);
                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pClientList[sTargetH]->m_iPoisonLevel, 0, 0);
#ifdef DEF_TAIWANLOG
                            _bItemLog(DEF_ITEMLOG_POISONED, sTargetH, (char *)0, NULL);
#endif
                        }
                    }

                    m_pClientList[sTargetH]->m_iHP -= iAP_SM;
                    if (m_pClientList[sTargetH]->m_iHP <= 0)
                    {



                        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                            bAnalyzeCriminalAction(sAttackerH, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);

                        ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iAP_SM);
                        bKilled = true;
                        iKilledDice = m_pClientList[sTargetH]->m_iLevel;

                    }
                    else
                    {
                        if (iAP_SM > 0)
                        {

                            if (m_pClientList[sTargetH]->m_iAddTransMana > 0)
                            {
                                dTmp1 = (double)m_pClientList[sTargetH]->m_iAddTransMana;
                                dTmp2 = (double)iAP_SM;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;

                                // ÃÖ´ë ¸¶³ªÄ¡ 
                                iTemp = (2 * m_pClientList[sTargetH]->m_iMag) + (2 * m_pClientList[sTargetH]->m_iLevel) + (m_pClientList[sTargetH]->m_iInt / 2);
                                m_pClientList[sTargetH]->m_iMP += (int)dTmp3;
                                if (m_pClientList[sTargetH]->m_iMP > iTemp) m_pClientList[sTargetH]->m_iMP = iTemp;
                            }


                            if (m_pClientList[sTargetH]->m_iAddChargeCritical > 0)
                            {

                                if (iDice(1, 100) < (m_pClientList[sTargetH]->m_iAddChargeCritical))
                                {
                                    iMaxSuperAttack = (m_pClientList[sTargetH]->m_iLevel / 10);
                                    if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;

                                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
                                }
                            }


                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);

                            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                                sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
                            else sAttackerWeapon = 1;


                            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true))
                                iMoveDamage = 60;
                            else iMoveDamage = 40;

                            if (iAP_SM >= iMoveDamage)
                            {
                                //´ë¹ÌÁö°¡ 40ÀÌ»óÀÌ¸é ¸ÂÀº Å¸°ÝÀ¸·Î ¹°·¯³­´Ù.
                                //¹æÇâÀ» °è»êÇÑ´Ù.

                                if (sTgtX == sAtkX)
                                {
                                    if (sTgtY == sAtkY)     goto CAE_SKIPDAMAGEMOVE;
                                    else if (sTgtY > sAtkY) cDamageMoveDir = 5;
                                    else if (sTgtY < sAtkY) cDamageMoveDir = 1;
                                }
                                else if (sTgtX > sAtkX)
                                {
                                    if (sTgtY == sAtkY)     cDamageMoveDir = 3;
                                    else if (sTgtY > sAtkY) cDamageMoveDir = 4;
                                    else if (sTgtY < sAtkY) cDamageMoveDir = 2;
                                }
                                else if (sTgtX < sAtkX)
                                {
                                    if (sTgtY == sAtkY)     cDamageMoveDir = 7;
                                    else if (sTgtY > sAtkY) cDamageMoveDir = 6;
                                    else if (sTgtY < sAtkY) cDamageMoveDir = 8;
                                }


                                m_pClientList[sTargetH]->m_iLastDamage = iAP_SM;

                                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iAP_SM, sAttackerWeapon, 0);
                            }
                            else
                            {
                                CAE_SKIPDAMAGEMOVE:;
                                int iProb;


                                if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                                {
                                    switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill)
                                    {
                                        case 6: iProb = 3500; break;
                                        case 8: iProb = 1000; break;
                                        case 9: iProb = 2900; break;
                                        case 10: iProb = 2500; break;
                                        case 14: iProb = 2000; break;
                                        case 21: iProb = 2000; break;
                                        default: iProb = 1; break;
                                    }
                                }
                                else iProb = 1;

                                if (iDice(1, 10000) >= iProb)
                                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iAP_SM, sAttackerWeapon, 0);
                            }

                            if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 1)
                            {

                                // 1: Hold-Person 
                                // 2: Paralize
                                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], 0, 0);

                                m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                                bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                            }


                            m_pClientList[sTargetH]->m_iSuperAttackCount++;
                            if (m_pClientList[sTargetH]->m_iSuperAttackCount > 14)
                            {

                                m_pClientList[sTargetH]->m_iSuperAttackCount = 0;

                                iMaxSuperAttack = (m_pClientList[sTargetH]->m_iLevel / 10);
                                if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;


                                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
                            }
                        }
                    }
                }
                break;

            case DEF_OWNERTYPE_NPC:

                if (m_pNpcList[sTargetH]->m_cBehavior == DEF_BEHAVIOR_DEAD) return 0;
                if (m_pNpcList[sTargetH]->m_bIsKilled == true) return 0;

                if (m_bIsCrusadeMode == true)
                {

                    if (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)
                    {
                        switch (m_pNpcList[sTargetH]->m_sType)
                        {
                            case 40:
                            case 41:
                            case 43:
                            case 44:
                            case 45:
                            case 46:
                            case 47:
                            case 51:
                                return 0;

                            default: break;
                        }
                    }
                    else
                    {
                        switch (m_pNpcList[sTargetH]->m_sType)
                        {
                            case 41:
                                if (cAttackerSide != 0)
                                {

                                    m_pNpcList[sTargetH]->m_iV1 += iAP_L;
                                    if (m_pNpcList[sTargetH]->m_iV1 > 500)
                                    {

                                        m_pNpcList[sTargetH]->m_iV1 = 0;
                                        m_pNpcList[sTargetH]->m_iManaStock--;
                                        if (m_pNpcList[sTargetH]->m_iManaStock <= 0) m_pNpcList[sTargetH]->m_iManaStock = 0;
                                        //testcode
                                        wsprintf(G_cTxt, "ManaStock down: %d", m_pNpcList[sTargetH]->m_iManaStock);
                                        log->info(G_cTxt);
                                    }
                                }
                                break;
                        }
                    }
                }

                switch (m_pNpcList[sTargetH]->m_cActionLimit)
                {
                    case 1:
                    case 2:
                    case 6: //v2.20 2002-12-20 ??À? NPC ?ø?Ý ?ÊµÇ?Ô 
                        return 0;
                }



                if (m_pNpcList[sTargetH]->m_cSize == 0)
                    iDamage = iAP_SM;
                else iDamage = iAP_L;


                if (m_pNpcList[sTargetH]->m_iAbsDamage < 0)
                {
                    dTmp1 = (double)iDamage;
                    dTmp2 = (double)(abs(m_pNpcList[sTargetH]->m_iAbsDamage)) / 100.0f;
                    dTmp3 = dTmp1 * dTmp2;
                    dTmp2 = dTmp1 - dTmp3;
                    iDamage = (int)dTmp2;
                    if (iDamage < 0) iDamage = 1;
                }

                if ((31 == m_pNpcList[sTargetH]->m_sType) &&  //v2.19 2002-12-18Á??ó ÀÌ?óÇÏ?å ?öÁ? ÇÊ?ä
                    (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)
                    && (m_pClientList[sAttackerH]->m_iSpecialAbilityType == 7))
                {
                    iDamage = iDamage + iDice(3, 2);
                }


                if ((cAttackerSA == 2) && (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0))
                {

                    m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = 0;
                    bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_PROTECT);
                }


                switch (m_pNpcList[sTargetH]->m_cActionLimit)
                {
                    case 0:
                    case 3:
                    case 5:
                        m_pNpcList[sTargetH]->m_iHP -= iDamage;
                        break;
                }

                if (m_pNpcList[sTargetH]->m_iHP <= 0)
                {

                    NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);

                    bKilled = true;
                    iKilledDice = m_pNpcList[sTargetH]->m_iHitDice;

                }
                else
                {







                    if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
                        && (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) goto CAE_SKIPCOUNTERATTACK;


                    if (m_pNpcList[sTargetH]->m_cActionLimit != 0) goto CAE_SKIPCOUNTERATTACK;
                    if (m_pNpcList[sTargetH]->m_bIsPermAttackMode == true) goto CAE_SKIPCOUNTERATTACK;

                    if ((m_pNpcList[sTargetH]->m_bIsSummoned == true) &&
                        (m_pNpcList[sTargetH]->m_iSummonControlMode == 1)) goto CAE_SKIPCOUNTERATTACK;

                    if (m_pNpcList[sTargetH]->m_sType == 51) goto CAE_SKIPCOUNTERATTACK;


                    if (iDice(1, 3) == 2)
                    {

                        if (m_pNpcList[sTargetH]->m_cBehavior == DEF_BEHAVIOR_ATTACK)
                        {
                            tX = tY = 0;
                            switch (m_pNpcList[sTargetH]->m_cTargetType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex] != 0)
                                    {
                                        tX = m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sX;
                                        tY = m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sY;
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[m_pNpcList[sTargetH]->m_iTargetIndex] != 0)
                                    {
                                        tX = m_pNpcList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sX;
                                        tY = m_pNpcList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sY;
                                    }
                                    break;
                            }

                            iDst1 = (m_pNpcList[sTargetH]->m_sX - tX) * (m_pNpcList[sTargetH]->m_sX - tX) + (m_pNpcList[sTargetH]->m_sY - tY) * (m_pNpcList[sTargetH]->m_sY - tY);

                            tX = tY = 0;
                            switch (cAttackerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sAttackerH] != 0)
                                    {
                                        tX = m_pClientList[sAttackerH]->m_sX;
                                        tY = m_pClientList[sAttackerH]->m_sY;
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sAttackerH] != 0)
                                    {
                                        tX = m_pNpcList[sAttackerH]->m_sX;
                                        tY = m_pNpcList[sAttackerH]->m_sY;
                                    }
                                    break;
                            }


                            iDst2 = (m_pNpcList[sTargetH]->m_sX - tX) * (m_pNpcList[sTargetH]->m_sX - tX) + (m_pNpcList[sTargetH]->m_sY - tY) * (m_pNpcList[sTargetH]->m_sY - tY);

                            if (iDst2 <= iDst1)
                            {

                                m_pNpcList[sTargetH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                                m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;
                                m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
                                m_pNpcList[sTargetH]->m_cTargetType = cAttackerType;
                            }
                        }
                        else
                        {
                            m_pNpcList[sTargetH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                            m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;
                            m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
                            m_pNpcList[sTargetH]->m_cTargetType = cAttackerType;
                        }
                    }

                    CAE_SKIPCOUNTERATTACK:;


                    if ((iDice(1, 3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0))
                        m_pNpcList[sTargetH]->m_dwTime = dwTime;


                    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                        sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
                    else sAttackerWeapon = 1;

                    if ((wWeaponType < 40) && (m_pNpcList[sTargetH]->m_cActionLimit == 4))
                    {


                        if (sTgtX == sAtkX)
                        {
                            if (sTgtY == sAtkY)     goto CAE_SKIPDAMAGEMOVE2;
                            else if (sTgtY > sAtkY) cDamageMoveDir = 5;
                            else if (sTgtY < sAtkY) cDamageMoveDir = 1;
                        }
                        else if (sTgtX > sAtkX)
                        {
                            if (sTgtY == sAtkY)     cDamageMoveDir = 3;
                            else if (sTgtY > sAtkY) cDamageMoveDir = 4;
                            else if (sTgtY < sAtkY) cDamageMoveDir = 2;
                        }
                        else if (sTgtX < sAtkX)
                        {
                            if (sTgtY == sAtkY)     cDamageMoveDir = 7;
                            else if (sTgtY > sAtkY) cDamageMoveDir = 6;
                            else if (sTgtY < sAtkY) cDamageMoveDir = 8;
                        }

                        dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                        dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false)
                        {

                            cDamageMoveDir = iDice(1, 8);
                            dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                            dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                            if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) goto CAE_SKIPDAMAGEMOVE2;
                        }


                        m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);

                        m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
                        m_pNpcList[sTargetH]->m_sX = dX;
                        m_pNpcList[sTargetH]->m_sY = dY;
                        m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;


                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);


                        dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                        dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false)
                        {

                            cDamageMoveDir = iDice(1, 8);
                            dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                            dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                            if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) goto CAE_SKIPDAMAGEMOVE2;
                        }


                        m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);

                        m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
                        m_pNpcList[sTargetH]->m_sX = dX;
                        m_pNpcList[sTargetH]->m_sY = dY;
                        m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;


                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);


                        if (bCheckEnergySphereDestination(sTargetH, sAttackerH, cAttackerType) == true)
                        {
                            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                            {
                                iExp = (m_pNpcList[sTargetH]->m_iExp / 3);
                                if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0)
                                    iExp += m_pNpcList[sTargetH]->m_iNoDieRemainExp;

                                //v1.42 °æÇèÄ¡ Áõ°¡ 
                                if (m_pClientList[sAttackerH]->m_iAddExp != 0)
                                {
                                    dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                    dTmp2 = (double)iExp;
                                    dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                    iExp += (int)dTmp3;
                                }

                                // Crusade
                                if ((m_bIsCrusadeMode == true) && (iExp > 10)) iExp = 10;

                                //m_pClientList[sAttackerH]->m_iExpStock += iExp;
                                GetExp(sAttackerH, iExp);


                                DeleteNpc(sTargetH);
                                return false;
                            }
                        }

                        CAE_SKIPDAMAGEMOVE2:;
                    }
                    else
                    {
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, sAttackerWeapon, 0);
                    }


                    if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 1)
                    {

                        // 1: Hold-Person 
                        m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                    else if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 2)
                    {

                        // 2: Paralize
                        if ((m_pNpcList[sTargetH]->m_iHitDice > 50) && (iDice(1, 10) == 5))
                        {
                            m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                            bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                        }
                    }


                    if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != true) &&
                        (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
                    {


                        if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage)
                        {
                            iExp = iDamage;
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
                        }
                        else
                        {
                            iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
                        }

                        //v1.42 ?æÇèÄ? Áõ?? 
                        if (m_pClientList[sAttackerH]->m_iAddExp != 0)
                        {
                            dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                            dTmp2 = (double)iExp;
                            dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                            iExp += (int)dTmp3;
                        }

                        if (m_bIsCrusadeMode == true) iExp = iExp / 3;


                        if (m_pClientList[sAttackerH]->m_iLevel > 100)
                        {
                            switch (m_pNpcList[sTargetH]->m_sType)
                            {
                                case 55:
                                case 56:
                                    iExp = 0;
                                    break;
                                default: break;
                            }
                        }

                    }
                }
                break;
        }

        //CAE_SKIPCOUNTERATTACK:;

        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        {

            if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
                sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
            else sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];

            if (sWeaponIndex != -1)
            {

                if ((m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex] != 0) &&
                    (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_sIDnum != 231))
                {

                    if (bKilled == false)
                        CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, 1);
                    else
                    {
                        if (m_pClientList[sAttackerH]->m_iHP <= 3)
                            CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, iDice(1, iKilledDice) * 2);
                        else CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, iDice(1, iKilledDice));
                    }
                }

                if ((m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex] != 0) &&
                    (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wMaxLifeSpan != 0))
                {


                    iWepLifeOff = 1;
                    if ((wWeaponType >= 1) && (wWeaponType < 40))
                    {
                        switch (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cWhetherStatus)
                        {
                            case 0:	break;
                            case 1:	if (iDice(1, 3) == 1) iWepLifeOff++; break;
                            case 2:	if (iDice(1, 2) == 1) iWepLifeOff += iDice(1, 2); break;
                            case 3:	if (iDice(1, 2) == 1) iWepLifeOff += iDice(1, 3); break;
                        }
                    }


                    if (m_pClientList[sAttackerH]->m_cSide != 0)
                    {
                        if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan < iWepLifeOff)
                            m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan = 0;
                        else m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan -= iWepLifeOff;
                    }

                    if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan == 0)
                    {


                        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_cEquipPos, sWeaponIndex, 0, 0);

                        ReleaseItemHandler(sAttackerH, sWeaponIndex, true);
                    }
                }
            }
            else
            {
                if (wWeaponType == 0)
                {

                    CalculateSSN_SkillIndex(sAttackerH, 5, 1);
                }
            }
        }
    }
    else
    {

        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        {


            m_pClientList[sAttackerH]->m_iComboAttackCount = 0;
        }
    }

    return iExp;
}




int CGame::iGetDangerValue(int iNpcH, short dX, short dY)
{
    int ix, iy, iDangerValue;
    short sOwner, sDOType;
    char  cOwnerType;
    uint32_t dwRegisterTime;

    if (m_pNpcList[iNpcH] == 0) return false;

    iDangerValue = 0;

    for (ix = dX - 2; ix <= dX + 2; ix++)
        for (iy = dY - 2; iy <= dY + 2; iy++)
        {
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sDOType, &dwRegisterTime);

            if (sDOType == 1) iDangerValue++;

            switch (cOwnerType)
            {
                case 0:
                    break;
                case DEF_OWNERTYPE_PLAYER:
                    if (m_pClientList[sOwner] == 0) break;
                    if (m_pNpcList[iNpcH]->m_cSide != m_pClientList[sOwner]->m_cSide)
                        iDangerValue++;
                    else iDangerValue--;
                    break;
                case DEF_OWNERTYPE_NPC:
                    if (m_pNpcList[sOwner] == 0) break;
                    if (m_pNpcList[iNpcH]->m_cSide != m_pNpcList[sOwner]->m_cSide)
                        iDangerValue++;
                    else iDangerValue--;
                    break;
            }
        }

    return iDangerValue;
}



void CGame::MsgProcess()
{
    char * pData{}, cFrom{}, cKey{};
    uint32_t    dwMsgSize{}, * dwpMsgID{};
    uint16_t * wpMsgType{}, * wp{};
    int      i{}, iClientH{};

    uint32_t * dwp{}, dwTimeRcv{};
    int itmp{}, itmp2{}, itmp3{}, itmp4{}, itmp5{};
    int * iptmp, * ip, iRet{};
    char cCharListBuffer[5000]{}, * cptmp;
    bool bTest = false;
    char * cp;
    bool bCheck = true;
    char cBuffer[256]{};

    itmp = itmp2 = itmp3 = itmp4 = itmp5 = 0;

    char cName[11]{}, cName2[11]{}, cBuffer2[256]{};
    memset(cBuffer, 0, 256);
    memset(cName, 0, 11);
    memset(cName2, 0, 11);
    memset(cBuffer2, 0, 256);

    memset(m_pMsgBuffer, 0, DEF_MSGBUFFERSIZE + 1);
    pData = (char *)m_pMsgBuffer;

    m_iCurMsgs = 0;
    std::unique_lock<std::mutex> l(packet_mtx, std::defer_lock);
    while (!packet_queue.empty())
    {
        l.lock();
        std::unique_ptr<socket_message> msg = std::move(packet_queue.front());
        if (msg == nullptr)
        {
            // ???
            log->warn("Socket message is empty??");
            l.unlock();
            continue;
        }
        socket_message & sm = *msg;

        CClient * client = sm.player.get();
        int iClientH = sm.player->client_handle;

        stream_read & sr = *sm.sr;
        stream_write sw;

        packet_queue.pop_front();
        l.unlock();

        pData = sr.data;
        dwMsgSize = sr.size;

        dwpMsgID = (uint32_t *)(sr.data + DEF_INDEX4_MSGID);

        client->m_dwTime = timeGetTime();

        log->info("Game Packet [{:X}]", *dwpMsgID);

        if (client == nullptr || client->currentstatus == client_status::login_screen)
        {
            handle_login_server_message(sm);
            continue;
        }

        if (m_pClientList[iClientH] == nullptr)
        {
            //log->error(std::format("ClientList is null - Game Packet [{:X}]", *dwpMsgID));
            if (sm.websocket.getReadyState() == ix::ReadyState::Open)
                sm.websocket.close();
            continue;
        }

        if (client->currentstatus != client_status::in_game)
        {
            log->error(std::format("Client's status is not in game - Game Packet [{:X}]", *dwpMsgID));
            if (sm.websocket.getReadyState() == ix::ReadyState::Open)
                sm.websocket.close();
            continue;
        }

        switch (*dwpMsgID)
        {
            case MSGID_COMMAND_CHECKCONNECTION:
                CheckConnectionHandler(iClientH, pData);
                break;

            case MSGID_REQUEST_SELLITEMLIST:
                RequestSellItemListHandler(iClientH, pData);
                break;

            case MSGID_REQUEST_RESTART:
                RequestRestartHandler(iClientH);
                break;

            case MSGID_REQUEST_PANNING:
                iRequestPanningMapDataRequest(iClientH, pData);
                break;

            case MSGID_REQUEST_NOTICEMENT:
                RequestNoticementHandler(iClientH, pData);
                break;

            case MSGID_REQUEST_SETITEMPOS:

                _SetItemPos(iClientH, pData);
                break;

            case MSGID_ADMINUSER:

                RequestAdminUserMode(iClientH, pData);
                break;

            case MSGID_REQUEST_FULLOBJECTDATA:

                RequestFullObjectData(iClientH, pData);
                break;

            case MSGID_REQUEST_RETRIEVEITEM:
                RequestRetrieveItemHandler(iClientH, pData);
                break;

            case MSGID_REQUEST_CIVILRIGHT:

                RequestCivilRightHandler(iClientH, pData);
                break;
            case MSGID_REQUEST_TELEPORT:

                RequestTeleportHandler(iClientH, pData);
                break;

            case MSGID_REQUEST_INITDATA:
                RequestInitDataHandler(iClientH, pData, cKey);
                break;

            case MSGID_COMMAND_COMMON:
                ClientCommonHandler(iClientH, pData, dwMsgSize);
                break;

            case MSGID_COMMAND_MOTION:
                ClientMotionHandler(iClientH, pData);
                break;

            case MSGID_REQUEST_FRIENDSLIST:
                memset(cCharListBuffer, 0, 5000);

                cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
                itmp5 = *cp;
                cp++;

                //itmp5 = *cp;

                switch (itmp5)
                {
                    case 1://Add friend
                        break;
                    case 2://Delete friend
                        break;
                    case 3://Update client list
                        dwp = (uint32_t *)(cCharListBuffer + DEF_INDEX4_MSGID);
                        *dwp = MSGID_RESPONSE_FRIENDSLIST;
                        wp = (uint16_t *)(cCharListBuffer + DEF_INDEX2_MSGTYPE);
                        *wp = 3; // 3 = full update / 2 = delete friend / 1 = add friend

                        cp = (char *)(cCharListBuffer + DEF_INDEX2_MSGTYPE + 2);

                        cptmp = cp;
                        cp++;

                        itmp2 = itmp3 = 0;


                        for (i = 0; i < 50; i++)
                            if (m_pClientList[iClientH]->m_stFriendsList[i].m_bUsed != 0)
                            {
                                //bTest = false;
                                //for (int o = 0; o < DEF_MAXCLIENTS; o++)
                                //{
                                //	if (m_pClientList[o] != 0)
                                //		if (memcmp(m_pClientList[iClientH]->m_stFriendsList[i].m_cCharName, m_pClientList[o]->m_cCharName, 10) == 0)
                                //		{
                                //			*cp = 1;
                                //			cp++;

                                //			memcpy(cp, m_pClientList[iClientH]->m_stFriendsList[i].m_cCharName, 10);
                                //			cp+=10;

                                //			memcpy(cp, m_pClientList[o]->m_cMapName, 10);
                                //			cp+=10;

                                //			//memcpy(cp, m_pClientList[o]->m_cLocation, 10);
                                //			//cp+=10;

                                //			*cp = m_pClientList[o]->m_cSide;
                                //			cp++;

                                //			ip = (int *)cp;
                                //			*ip = m_pClientList[o]->m_iLevel;
                                //			cp+=4;

                                //			ip = (int *)cp;
                                //			*ip = m_pClientList[o]->m_iPKCount;
                                //			cp+=4;

                                //			itmp2++;
                                //			bTest = true;
                                //			break;
                                //		}
                                //}
                                //if (bTest == false)
                                {
                                    *cp = 0;
                                    cp++;

                                    memcpy(cp, m_pClientList[iClientH]->m_stFriendsList[i].m_cCharName, 10);
                                    cp += 10;

                                    itmp3++;
                                }
                            }

                        *cptmp = (itmp2 + itmp3);

                        iRet = m_pClientList[iClientH]->iSendMsg(cCharListBuffer, 7 + (itmp2 * 49) + (itmp3 * 11));

                        for (i = 0; i < 50; i++)
                            if (m_pClientList[iClientH]->m_stFriendsList[i].m_bUsed == true)
                            {
                            }

                        break;
                    case 7://Single Updates (full update)

                        for (i = 0; i < 50; i++)
                            if (m_pClientList[iClientH]->m_stFriendsList[i].m_bUsed == true)
                            {

                            }

                        break;
                }
                break;

            case MSGID_REQUEST_REGISTER:
                if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_bIsInitComplete2 == false))
                {

                    memset(cCharListBuffer, 0, 5000);

                    char ctmp{}, ctmp2{};

                    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
                    ip = (int *)cp;
                    itmp = *ip;
                    cp += 4;
                    ip = (int *)cp;
                    itmp2 = *ip;
                    cp += 4;
                    ip = (int *)cp;
                    itmp3 = *ip;
                    cp += 4;

                    //log->info("_REGISTER");

                    dwp = (uint32_t *)(cBuffer + DEF_INDEX4_MSGID);
                    *dwp = MSGID_RESPONSE_REGISTER;
                    wp = (uint16_t *)(cBuffer + DEF_INDEX2_MSGTYPE);
                    *wp = 0;

                    cp = (char *)(cBuffer + DEF_INDEX2_MSGTYPE + 2);

                    if ((itmp == UPPER_VERSION) && (itmp2 == LOWER_VERSION) && (itmp3 == PATCH_VERSION))
                    {

                        m_pClientList[iClientH]->m_bIsInitComplete2 = true;
                        *cp = 1;
                        cp++;
                        iRet = m_pClientList[iClientH]->iSendMsg(cBuffer, 8);

                        //log->info("_REGISTER2");
                        dwp = (uint32_t *)(cCharListBuffer + DEF_INDEX4_MSGID);
                        *dwp = MSGID_RESPONSE_FRIENDSLIST;
                        wp = (uint16_t *)(cCharListBuffer + DEF_INDEX2_MSGTYPE);
                        *wp = 4; // 5 = friend go offline / 4 = friend come online / = 3 = full update / 2 = delete friend / 1 = add friend

                        cp = (char *)(cCharListBuffer + DEF_INDEX2_MSGTYPE + 2);

                        memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
                        cp += 10;

                        memcpy(cp, m_pClientList[iClientH]->m_cMapName, 10);
                        cp += 10;

                        *cp = m_pClientList[iClientH]->m_cSide;
                        cp++;

                        ip = (int *)cp;
                        *ip = m_pClientList[iClientH]->m_iLevel;
                        cp += 4;

                        ip = (int *)cp;
                        *ip = m_pClientList[iClientH]->m_iPKCount;
                        cp += 4;

                        //log->info("_REGISTER3");
                        for (i = 1; i < DEF_MAXCLIENTS; i++)
                            if ((m_pClientList[i] != 0))
                                for (int o = 0; o < 50; o++)
                                    if (strcmp(m_pClientList[i]->m_stFriendsList[o].m_cCharName, m_pClientList[iClientH]->m_cCharName) == 0)
                                        iRet = m_pClientList[i]->iSendMsg(cCharListBuffer, 35);

                        //////////////////////////////////////////////////////////////////////////


                    }
                    else
                    {
                        *cp = 2;
                        iRet = m_pClientList[iClientH]->iSendMsg(cBuffer, 8);
                    }
                }

                break;

            case MSGID_REQUEST_PKLIST:
                if (m_pClientList[iClientH] == 0)
                    break;
                bool bFlag;
                int iShortCutIndex;
                bFlag = true;

                memset(cCharListBuffer, 0, 5000);

                dwp = (uint32_t *)(cCharListBuffer + DEF_INDEX4_MSGID);
                *dwp = MSGID_RESPONSE_PKLIST;
                wp = (uint16_t *)(cCharListBuffer + DEF_INDEX2_MSGTYPE);
                *wp = 0;

                cp = (char *)(cCharListBuffer + DEF_INDEX2_MSGTYPE + 2);

                itmp2 = 0;

                iptmp = (int *)cp;
                cp += 4;

                for (i = 0; i < DEF_MAXCLIENTS; i++)
                {
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))

                        if ((m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex) &&
                            (m_pClientList[i]->m_sX >= m_pClientList[iClientH]->m_sX - 10) &&
                            (m_pClientList[i]->m_sX <= m_pClientList[iClientH]->m_sX + 10) &&
                            (m_pClientList[i]->m_sY >= m_pClientList[iClientH]->m_sY - 8) &&
                            (m_pClientList[i]->m_sY <= m_pClientList[iClientH]->m_sY + 8))
                        {
                            //								if (m_pClientList[i]->m_iPKCount > 0)
                            //								{
                            memcpy(cp, m_pClientList[i]->m_cCharName, 10);
                            cp += 10;

                            ip = (int *)cp;
                            *ip = m_pClientList[i]->m_iPKCount;
                            cp += 4;

                            ip = (int *)cp;
                            *ip = m_pClientList[i]->m_iAdminUserLevel;
                            cp += 4;

                            itmp2++;
                            //								}
                        }
                }
                *iptmp = itmp2;

                iRet = m_pClientList[iClientH]->iSendMsg(cCharListBuffer, 14 + (itmp2 * 18));

                break;
            case MSGID_REQUEST_CHARLIST:
                if (m_pClientList[iClientH] == 0)
                    break;
                CTile * pTile;

                memset(cCharListBuffer, 0, 5000);

                dwp = (uint32_t *)(cCharListBuffer + DEF_INDEX4_MSGID);
                *dwp = MSGID_RESPONSE_CHARLIST;
                wp = (uint16_t *)(cCharListBuffer + DEF_INDEX2_MSGTYPE);
                *wp = 0;

                cp = (char *)(cCharListBuffer + DEF_INDEX2_MSGTYPE + 2);
                //dwp = (uint32_t *)cp;
                //*dwp = dwTimeRcv;
                //cp += 4;

                itmp2 = 0;

                iptmp = (int *)cp;
                cp += 4;

                for (itmp = 0; itmp < DEF_MAXCLIENTS; itmp++)
                {
                    if (m_pClientList[itmp] != 0)
                    {
                        memcpy(cp, m_pClientList[itmp]->m_cCharName, 10);
                        cp += 10;
                        memcpy(cp, m_pClientList[itmp]->m_cLocation, 10);
                        cp += 10;
                        memcpy(cp, m_pClientList[itmp]->m_cMapName, 10);
                        cp += 10;
                        memcpy(cp, m_pClientList[itmp]->m_cIPaddress, 20);
                        cp += 20;

                        ip = (int *)cp;
                        *ip = m_pClientList[itmp]->m_sX;
                        cp += 4;

                        ip = (int *)cp;
                        *ip = m_pClientList[itmp]->m_sY;
                        cp += 4;

                        ip = (int *)cp;
                        *ip = m_pClientList[itmp]->m_iLevel;
                        cp += 4;

                        ip = (int *)cp;
                        *ip = m_pClientList[itmp]->m_iPKCount;
                        cp += 4;

                        *cp = m_pClientList[itmp]->m_iAdminUserLevel;
                        cp++;

                        itmp2++;
                    }
                }

                *iptmp = itmp2;

                itmp3 = 0;

                iptmp = (int *)cp;
                cp += 4;

                for (itmp = 0; itmp < DEF_MAXMAPS; itmp++)
                {
                    if (m_pMapList[itmp] != 0)
                    {
                        memcpy(cp, m_pMapList[itmp]->m_cName, 10);
                        cp += 10;

                        itmp4 = 0;
                        for (i = 0; i < DEF_MAXCLIENTS; i++)
                            if (m_pClientList[i] != 0)
                                if (memcmp(m_pClientList[i]->m_cMapName, m_pMapList[itmp]->m_cName, 10) == 0)
                                    itmp4++;

                        ip = (int *)cp;
                        *ip = itmp4;
                        cp += 4;

                        itmp4 = 0;

                        for (i = 0; i < DEF_MAXNPCS; i++)
                            if (m_pNpcList[i] != 0)
                                if (m_pNpcList[i]->m_cMapIndex == itmp)
                                    itmp4++;

                        ip = (int *)cp;
                        *ip = itmp4;
                        cp += 4;

                        itmp4 = 0;

                        for (int iy = 0; iy < m_pMapList[itmp]->m_sSizeY; iy++)
                            for (int ix = 0; ix < m_pMapList[itmp]->m_sSizeX; ix++)
                            {
                                pTile = (CTile *)(m_pMapList[itmp]->m_pTile + ix + iy * m_pMapList[itmp]->m_sSizeY);
                                itmp4 += pTile->m_cTotalItem;
                            }


                        ip = (int *)cp;
                        *ip = itmp4;
                        cp += 4;

                        itmp3++;
                    }
                }

                *iptmp = itmp3;

                iRet = m_pClientList[iClientH]->iSendMsg(cCharListBuffer, 14 + (itmp2 * 67) + (itmp3 * 22));
                break;

            case MSGID_REQUEST_PING:
                sr.read_uint32();
                sr.read_uint16();

                sw.write_int32(MSGID_RESPONSE_PING);
                sw.write_int16(0);
                sw.write_int64(sr.read_uint64());
                sw.write_int64(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
                m_pClientList[iClientH]->write(sw);
                break;

            case MSGID_COMMAND_CHATMSG:

                ChatMsgHandler(iClientH, pData, dwMsgSize);
                break;
            case MSGID_REQUEST_CREATENEWGUILD:

                RequestCreateNewGuildHandler(iClientH, pData, dwMsgSize);
                break;
            case MSGID_REQUEST_DISBANDGUILD:

                RequestDisbandGuildHandler(iClientH, pData, dwMsgSize);
                break;

            case MSGID_REQUEST_FIGHTZONE_RESERVE:

                FightzoneReserveHandler(iClientH, pData, dwMsgSize);
                break;

            case MSGID_LEVELUPSETTINGS:
                LevelUpSettingsHandler(iClientH, pData, dwMsgSize);
                break;

            case MSGID_STATECHANGEPOINT:
                StateChangeHandler(iClientH, pData, dwMsgSize);
                break;


            case MSGID_REQUEST_TELEPORT_LIST:
                RequestTeleportListHandler(iClientH, pData, dwMsgSize);
                break;


            case MSGID_REQUEST_CHARGED_TELEPORT:
                RequestChargedTeleportHandler(iClientH, pData, dwMsgSize);
                break;

            default:
                wsprintfA(G_cTxt, "Unknown message received 0x%lX! Delete Client", *dwpMsgID);
                log->info(G_cTxt);
                //DeleteClient(iClientH, true, true); // v1.4
                break;
        }
    }
}


void CGame::PutMsgQueue(std::unique_ptr<socket_message> & sm)
{
    if (sm == nullptr) __debugbreak();
    packet_queue.push_back(std::move(sm));
}
 
void CGame::DropItemHandler(int iClientH, short sItemIndex, int iAmount, char * pItemName, bool bByPlayer)
{
    CItem * pItem;

    // 

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;  //DEBUG
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
    if ((iAmount != -1) && (iAmount < 0)) return;


    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
        (iAmount == -1))
        iAmount = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount;



    if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0) return;

    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
        (((int)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount - iAmount) > 0))
    {

        pItem = new CItem;
        if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == false)
        {

            delete pItem;
            return;
        }
        else
        {
            if (iAmount <= 0)
            {

                delete pItem;
                return;
            }
            pItem->m_dwCount = (DWORD)iAmount;
        }




        if ((DWORD)iAmount > m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount)
        {
            delete pItem;
            return;
        }

        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount -= iAmount;


        // v1.41 !!!
        SetItemCount(iClientH, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount);


        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY, pItem);



        if (bByPlayer == true)
            _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);
        else
            _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem, true);


        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED, sItemIndex, iAmount, 0, 0);
    }
    else
    {




        ReleaseItemHandler(iClientH, sItemIndex, true);


        if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == true)
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

        // v1.432
        if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP) &&
            (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan == 0))
        {

            delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
            m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
        }
        else
        {

            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]);



            if (bByPlayer == true)
                _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);
            else
                _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex], true);


            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor); //v1.4 color
        }


        m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
        m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;


        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, 0, 0);


        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
    }


    iCalcTotalWeight(iClientH);
}


bool CGame::_bAddClientItemList(int iClientH, CItem * pItem, int * pDelReq)
{
    int i;

    if (m_pClientList[iClientH] == 0) return false;
    if (pItem == 0) return false;


    if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW))
    {

        if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount)) > (DWORD)_iCalcMaxLoad(iClientH))
            return false;
    }
    else
    {

        if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, 1)) > (DWORD)_iCalcMaxLoad(iClientH))
            return false;
    }


    if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW))
    {
        for (i = 0; i < DEF_MAXITEMS; i++)
            if ((m_pClientList[iClientH]->m_pItemList[i] != 0) &&
                (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, pItem->m_cName, 20) == 0))
            {

                m_pClientList[iClientH]->m_pItemList[i]->m_dwCount += pItem->m_dwCount;
                //delete pItem;
                *pDelReq = 1;


                iCalcTotalWeight(iClientH);

                return true;
            }
    }

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] == 0)
        {

            m_pClientList[iClientH]->m_pItemList[i] = pItem;

            m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
            m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

            *pDelReq = 0;


            if (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)
                m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);


            iCalcTotalWeight(iClientH);

            return true;
        }


    return false;
}

bool CGame::bEquipItemHandler(int iClientH, short sItemIndex, bool bNotify)
{
    char  cEquipPos;
    short sTemp, sSpeed;
    int   iTemp;

    if (m_pClientList[iClientH] == 0) return false;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return false;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return false;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return false;

    //¸¸¾à ÀåÂøÇÏ°íÀÚ ÇÏ´Â ¾ÆÀÌÅÛÀÇ ¼ö¸íÀÌ 0ÀÎ °æ¿ì´Â ÀåÂøÇÒ ¼ö ¾ø´Ù.
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan == 0) return false;


    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) == 0) &&
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sLevelLimit > m_pClientList[iClientH]->m_iLevel)) return false;


    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 0)
    {
        switch (m_pClientList[iClientH]->m_sType)
        {
            case 1:
            case 2:
            case 3:

                if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 1) return false;
                break;
            case 4:
            case 5:
            case 6:

                if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 2) return false;
                break;
        }
    }


    // v1.432
    if (iGetItemWeight(m_pClientList[iClientH]->m_pItemList[sItemIndex], 1) > m_pClientList[iClientH]->m_iStr * 100) return false;

    cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;


    if ((cEquipPos == DEF_EQUIPPOS_BODY) || (cEquipPos == DEF_EQUIPPOS_LEGGINGS) || (cEquipPos == DEF_EQUIPPOS_ARMS) || (cEquipPos == DEF_EQUIPPOS_HEAD))
    {
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4)
        {
            case 10:
                if (m_pClientList[iClientH]->m_iStr < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
                    return false;
                }
                break;
            case 11: // Dex
                if (m_pClientList[iClientH]->m_iDex < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
                    return false;
                }
                break;
            case 12: // Vit
                if (m_pClientList[iClientH]->m_iVit < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
                    return false;
                }
                break;
            case 13: // Int
                if (m_pClientList[iClientH]->m_iInt < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
                    return false;
                }
                break;
            case 14: // Mag
                if (m_pClientList[iClientH]->m_iMag < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
                    return false;
                }
                break;
            case 15: // Chr
                if (m_pClientList[iClientH]->m_iCharisma < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
                    return false;
                }
                break;
        }
    }


    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY))
    {

        if ((m_pClientList[iClientH]->m_iSpecialAbilityType != 0))
        {


            if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos != m_pClientList[iClientH]->m_iSpecialAbilityEquipPos)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], 0, 0);

                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], true);
            }
        }
    }


    if (cEquipPos == DEF_EQUIPPOS_NONE) return false;

    if (cEquipPos == DEF_EQUIPPOS_TWOHAND)
    {

        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);
        else
        {

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND], false);
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND], false);
        }
    }
    else
    {

        if ((cEquipPos == DEF_EQUIPPOS_LHAND) || (cEquipPos == DEF_EQUIPPOS_RHAND))
        {

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND], false);
        }


        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);
    }

    if (cEquipPos == DEF_EQUIPPOS_FULLBODY)
    {
        //v2.19 2002-12-7 »êÅ¸º¹ °ü·Ã 
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);
        else
        {

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD], false);
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY], true);
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS], false);
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS], false);
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS], false);
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK], false);

        }
    }
    else
    {

        if ((cEquipPos == DEF_EQUIPPOS_HEAD)
            || (cEquipPos == DEF_EQUIPPOS_BODY)
            || (cEquipPos == DEF_EQUIPPOS_ARMS)
            || (cEquipPos == DEF_EQUIPPOS_LEGGINGS)
            || (cEquipPos == DEF_EQUIPPOS_PANTS)
            || (cEquipPos == DEF_EQUIPPOS_BACK))
        {

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_FULLBODY] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_FULLBODY], false);
        }


        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);

    }
    m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = sItemIndex;
    m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = true;



    switch (cEquipPos)
    {
        case DEF_EQUIPPOS_RHAND:

            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4);
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0x0FFFFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
            m_pClientList[iClientH]->m_iApprColor = iTemp;


            iTemp = m_pClientList[iClientH]->m_iStatus;
            iTemp = iTemp & 0xFFFFFFF0;//Change 0xFFF0
            sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);



            m_pClientList[iClientH]->m_sUsingWeaponSkill = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill;


#ifdef DEF_WEAPONSPEEDLIMIT
            // v2.16 2002-5-27 
            sSpeed -= (m_pClientList[iClientH]->m_iStr / 13);
            switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill)
            {
                //case 6:  if (sSpeed < 1) sSpeed = 1; break;
                //case 7:  if (sSpeed < 0) sSpeed = 0; break;
                //case 8:  if (sSpeed < 2) sSpeed = 2; break; // ±äÄ®
                //case 9:  if (sSpeed < 1) sSpeed = 1; break;
                //case 10: if (sSpeed < 3) sSpeed = 1; break;
                case 14: if (sSpeed < 1) sSpeed = 1; break;
                default: if (sSpeed < 0) sSpeed = 0; break;
            }
#else
            sSpeed -= (m_pClientList[iClientH]->m_iStr / 13);
            if (sSpeed < 0) sSpeed = 0;
#endif
            iTemp = iTemp | sSpeed;
            m_pClientList[iClientH]->m_iStatus = iTemp;

            m_pClientList[iClientH]->m_iComboAttackCount = 0;
            break;

        case DEF_EQUIPPOS_LHAND:

            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xFFF0;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue));
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xF0FFFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 24);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_TWOHAND:

            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4);
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0x0FFFFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
            m_pClientList[iClientH]->m_iApprColor = iTemp;


            iTemp = m_pClientList[iClientH]->m_iStatus;
            iTemp = iTemp & 0xFFFFFFF0;//Change 0xFFF0
            sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);



            m_pClientList[iClientH]->m_sUsingWeaponSkill = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill;


#ifdef DEF_WEAPONSPEEDLIMIT
            // v2.16 2002-5-27 
            sSpeed -= (m_pClientList[iClientH]->m_iStr / 13);
            switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill)
            {
                //case 6:  if (sSpeed < 1) sSpeed = 1; break;
                //case 7:  if (sSpeed < 0) sSpeed = 0; break;
                //case 8:  if (sSpeed < 2) sSpeed = 2; break; // ±äÄ®
                //case 9:  if (sSpeed < 1) sSpeed = 1; break;
                //case 10: if (sSpeed < 3) sSpeed = 1; break;
                case 14: if (sSpeed < 1) sSpeed = 1; break;
                default: if (sSpeed < 0) sSpeed = 0; break;
            }
#else
            sSpeed -= (m_pClientList[iClientH]->m_iStr / 13);
            if (sSpeed < 0) sSpeed = 0;
#endif

            iTemp = iTemp | sSpeed;
            m_pClientList[iClientH]->m_iStatus = iTemp;

            m_pClientList[iClientH]->m_iComboAttackCount = 0;
            break;

        case DEF_EQUIPPOS_BODY:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;

            if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue < 100)
            {

                sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12);
                m_pClientList[iClientH]->m_sAppr3 = sTemp;
            }
            else
            {

                sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue - 100) << 12);
                m_pClientList[iClientH]->m_sAppr3 = sTemp;

                sTemp = m_pClientList[iClientH]->m_sAppr4;
                sTemp = sTemp | 0x080;
                m_pClientList[iClientH]->m_sAppr4 = sTemp;
            }

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFF0FFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 20);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_BACK:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xF0FF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8);
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFF0FFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 16);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_ARMS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFFF0;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue));
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFF0FFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 12);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_PANTS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xF0FF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8);
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFF0FF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 8);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_LEGGINGS:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0x0FFF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12);
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFF0F;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 4);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_HEAD:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFF0F;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4);
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFFF0;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor));
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_FULLBODY: //v2.19 2002-12-7 »êÅ¸º¹ °ü·Ã 	
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12);
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFF0FFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;
    }

    //v1.432 Æ¯¼ö ´É·ÂÀÌ ºÎ¿©µÈ ¾ÆÀÌÅÛÀÌ¶ó¸é ÇÃ·¡±× ¼³Á¤ 
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY)
    {

        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;     // 1111111111110011   
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect)
        {
            case 0: break;
            case 1:
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0004;	// 0100
                break;

            case 2:
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x000C;	// 1100
                break;

            case 3:
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0008;	// 1000
                break;
        }
    }

    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)
    {

        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC; // 1111111111111100
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect)
        {
            case 0: break;
            case 50:
            case 51:
            case 52: // ³ì»ö
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0002;	// 10
                break;

            default:
#ifndef DEF_KOREA

                if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) break;
#endif
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0001;	 // 01
                break;
        }
    }


    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);


    CalcTotalItemEffect(iClientH, sItemIndex, bNotify);

    return true;
}

// 2002-12-11 Áß±¹ °ü·Ã ÇØÅ· ¹æÁö ·çÆ¾ Ãß°¡
void CGame::SendEventToNearClient_TypeB(uint32_t dwMsgID, uint16_t wMsgType, char cMapIndex, short sX, short sY, short sV1, short sV2, short sV3, short sV4)
{
    int i, iRet, iShortCutIndex;
    char * cp, cData[100];
    uint32_t * dwp, dwTime;
    uint16_t * wp;
    short * sp;
    bool bFlag;
    char  cKey;

    cKey = (char)(rand() % 255) + 1; // v1.4

    memset(cData, 0, sizeof(cData));

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    sp = (short *)cp;
    *sp = sX;
    cp += 2;

    sp = (short *)cp;
    *sp = sY;
    cp += 2;

    sp = (short *)cp;
    *sp = sV1;
    cp += 2;

    sp = (short *)cp;
    *sp = sV2;
    cp += 2;

    sp = (short *)cp;
    *sp = sV3;
    cp += 2;

    sp = (short *)cp;
    *sp = sV4;
    cp += 2;

    dwTime = timeGetTime();

    //for (i = 1; i < DEF_MAXCLIENTS; i++)
    bFlag = true;
    iShortCutIndex = 0;
    while (bFlag == true)
    {

        i = m_iClientShortCut[iShortCutIndex];
        iShortCutIndex++;
        if (i == 0) bFlag = false;

        if ((bFlag == true) && (m_pClientList[i] != 0))
        {
            if ((m_pClientList[i]->m_cMapIndex == cMapIndex) &&
                (m_pClientList[i]->m_sX >= sX - 10) &&
                (m_pClientList[i]->m_sX <= sX + 10) &&
                (m_pClientList[i]->m_sY >= sY - 8) &&
                (m_pClientList[i]->m_sY <= sY + 8))
            {

                iRet = m_pClientList[i]->iSendMsg(cData, 18, cKey);
            }
        }
    }
}

void CGame::SendNotifyMsg(int iFromH, int iToH, uint16_t wMsgType, uint32_t sV1, uint32_t sV2, uint32_t sV3, char * pString, uint32_t sV4, uint32_t sV5, uint32_t sV6, uint32_t sV7, uint32_t sV8, uint32_t sV9, char * pString2)
{
    char cData[1000];
    uint32_t * dwp;
    uint16_t * wp;
    char * cp;
    short * sp;
    int * ip, iRet = 0, i;

    if (m_pClientList[iToH] == 0) return;

    memset(cData, 0, sizeof(cData));

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_NOTIFY;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);




    switch (wMsgType)
    {


        case DEF_NOTIFY_RESPONSE_HUNTMODE:
            memcpy(cp, pString, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 16);
            break;

            // v2.171 2002-6-14
        case DEF_NOTIFY_REQGUILDNAMEANSWER:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            memcpy(cp, pString, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 32);
            break;

        case DEF_NOTIFY_REQPLAYERNAMEANSWER:
            //SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REQPLAYERNAMEANSWER, iIndex, m_pClientList[iObjectID]->m_iAdminUserLevel,  m_pClientList[iObjectID]->m_iPKCount, m_pClientList[iObjectID]->m_cCharName, m_pClientList[iObjectID]->m_bEnabled, 0, 0, 0, 0, 0, m_pClientList[iObjectID]->m_cProfile);

            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV3;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV4;
            cp += 2;

            memcpy(cp, pString, 10);
            cp += 10;

            memcpy(cp, pString2, 90);
            cp += 90;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 114);
            break;

        case DEF_NOTIFY_TCLOC:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            memcpy(cp, pString, 10);
            cp += 10;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV4;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV5;
            cp += 2;

            memcpy(cp, pString2, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 34);
            break;

        case DEF_NOTIFY_PARTY:
            switch (sV1)
            {
                case 4:
                case 6:
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV1;
                    cp += 2;
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV2;
                    cp += 2;
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV3;
                    cp += 2;
                    memcpy(cp, pString, 10);
                    cp += 10;
                    iRet = m_pClientList[iToH]->iSendMsg(cData, 12 + 10);
                    break;

                case 5:
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV1;
                    cp += 2;
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV2;
                    cp += 2;
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV3;
                    cp += 2;
                    memcpy(cp, pString, sV3 * 11);
                    cp += sV3 * 11;
                    iRet = m_pClientList[iToH]->iSendMsg(cData, 12 + sV3 * 11);
                    break;

                default:
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV1;
                    cp += 2;
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV2;
                    cp += 2;
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV3;
                    cp += 2;
                    wp = (uint16_t *)cp;
                    *wp = (WORD)sV4;
                    cp += 2;
                    iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
                    break;
            }
            break;

        case DEF_NOTIFY_GRANDMAGICRESULT:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV3;
            cp += 2;

            memcpy(cp, pString, 10);
            cp += 10;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV4;
            cp += 2;



            //		sp = (short *)cp;
            //		*sp = (short)sV9;
            //		cp += 2;


            if (sV9 > 0)
            {
                memcpy(cp, pString2, (sV9 + 1) * 2);
                cp += (sV9 + 1) * 2;
            }
            else
            {
                sp = (short *)cp;
                *sp = (short)0;
                cp += 2;
            }

            iRet = m_pClientList[iToH]->iSendMsg(cData, 24 + (sV9 + 1) * 2);
            break;

        case DEF_NOTIFY_MAPSTATUSNEXT:
            memcpy(cp, pString, sV1);
            cp += sV1;
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6 + sV1);
            break;

        case DEF_NOTIFY_MAPSTATUSLAST:
            memcpy(cp, pString, sV1);
            cp += sV1;
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6 + sV1);
            break;

        case DEF_NOTIFY_LOCKEDMAP:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            memcpy(cp, pString, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 18);
            break;

        case DEF_NOTIFY_BUILDITEMSUCCESS:
        case DEF_NOTIFY_BUILDITEMFAIL:

            if (sV1 >= 0)
            {
                sp = (short *)cp;
                *sp = (short)sV1;
                cp += 2;
            }
            else
            {
                sp = (short *)cp;
                *sp = (short)sV1 + 10000;
                cp += 2;
            }

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_HELP:
        case DEF_NOTIFY_QUESTREWARD:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            ip = (int *)cp;
            *ip = (int)sV3;
            cp += 4;

            memcpy(cp, pString, 20);
            cp += 20;

            ip = (int *)cp;
            *ip = (int)sV4;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 38);
            break;

        case DEF_NOTIFY_CANNOTCONSTRUCT:
        case DEF_NOTIFY_METEORSTRIKECOMING:
        case DEF_NOTIFY_METEORSTRIKEHIT:
        case DEF_NOTIFY_HELPFAILED:
        case DEF_NOTIFY_SPECIALABILITYENABLED:
        case DEF_NOTIFY_FORCEDISCONN:
        case DEF_NOTIFY_OBSERVERMODE:
        case DEF_NOTIFY_QUESTCOMPLETED:
        case DEF_NOTIFY_QUESTABORTED:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_QUESTCONTENTS:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV3;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV4;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV5;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV6;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV7;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV8;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV9;
            cp += 2;

            if (pString2 != 0) memcpy(cp, pString2, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 44);
            break;

        case DEF_NOTIFY_GIZONITEMUPGRADELEFT:
        case DEF_NOTIFY_ITEMATTRIBUTECHANGE:

            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV2;
            cp += 4;

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV3;
            cp += 4;

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV4;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 20);
            break;

        case DEF_NOTIFY_ITEMUPGRADEFAIL:
            // v2.17 2002-7-21 
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);

            break;

        case DEF_NOTIFY_GIZONITEMCHANGE:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            *cp = (char)sV2;
            cp++;

            sp = (short *)cp;
            *sp = (short)sV3;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV4;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV5;
            cp += 2;

            *cp = (char)sV6;
            cp++;

            *cp = (char)sV7;
            cp++;

            // v1.42
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV8;
            cp += 4;

            memcpy(cp, pString, 20);
            cp += 20;


            iRet = m_pClientList[iToH]->iSendMsg(cData, 41);
            break;

        case DEF_NOTIFY_ENERGYSPHERECREATED:
        case DEF_NOTIFY_ITEMCOLORCHANGE:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_NOMOREAGRICULTURE:	   //? ³óÀÛ¹° Á¦ÇÑ
        case DEF_NOTIFY_AGRICULTURESKILLLIMIT: //? ½ºÅ³ Á¦ÇÑ	
        case DEF_NOTIFY_AGRICULTURENOAREA:     //? ³óÀÛ¹°À» ÁöÀ»¼ö ÀÖ´Â °ø°£ÀÌ ¾Æ´Ï´Ù.
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_NOMORECRUSADESTRUCTURE:
        case DEF_NOTIFY_EXCHANGEITEMCOMPLETE:
        case DEF_NOTIFY_CANCELEXCHANGEITEM:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_SETEXCHANGEITEM:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV3;
            cp += 2;

            ip = (int *)cp;
            *ip = (int)sV4;
            cp += 4;

            *cp = (char)sV5;
            cp++;

            sp = (short *)cp;
            *sp = (short)sV6;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV7;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV8;
            cp += 2;

            memcpy(cp, pString, 20);
            cp += 20;

            memcpy(cp, m_pClientList[iFromH]->m_cCharName, 10);
            cp += 10;

            // v1.42
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV9;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 57);
            break;

        case DEF_NOTIFY_OPENEXCHANGEWINDOW:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV3;
            cp += 2;

            ip = (int *)cp;
            *ip = (int)sV4;
            cp += 4;

            *cp = (char)sV5;
            cp++;

            sp = (short *)cp;
            *sp = (short)sV6;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV7;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV8;
            cp += 2;

            memcpy(cp, pString, 20);
            cp += 20;

            memcpy(cp, m_pClientList[iFromH]->m_cCharName, 10);
            cp += 10;

            // v1.42
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV9;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 57);
            break;

        case DEF_NOTIFY_NOTFLAGSPOT:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_ITEMPOSLIST:
            for (i = 0; i < DEF_MAXITEMS; i++)
            {
                sp = (short *)cp;
                *sp = (short)m_pClientList[iToH]->m_ItemPosList[i].x;
                cp += 2;
                sp = (short *)cp;
                *sp = (short)m_pClientList[iToH]->m_ItemPosList[i].y;
                cp += 2;
            }
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6 + DEF_MAXITEMS * 4);
            break;

        case DEF_NOTIFY_ENEMYKILLS:
            ip = (int *)cp;
            *ip = (int)sV1;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_CRUSADE:
            ip = (int *)cp;
            *ip = (int)sV1;
            cp += 4;

            ip = (int *)cp;
            *ip = (int)sV2;
            cp += 4;

            ip = (int *)cp;
            *ip = (int)sV3;
            cp += 4;

            ip = (int *)cp;
            *ip = (int)sV4;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 22);
            break;

        case DEF_NOTIFY_CONSTRUCTIONPOINT:
        case DEF_NOTIFY_SPECIALABILITYSTATUS:
        case DEF_NOTIFY_DAMAGEMOVE:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV3;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 12);
            break;

        case DEF_NOTIFY_DOWNSKILLINDEXSET:
        case DEF_NOTIFY_RESPONSE_CREATENEWPARTY:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_ADMINIFO:
            switch (sV1)
            {
                case 1:

                    ip = (int *)cp;
                    *ip = m_pNpcList[sV2]->m_iHP;
                    cp += 4;

                    ip = (int *)cp;
                    *ip = m_pNpcList[sV2]->m_iDefenseRatio;
                    cp += 4;

                    ip = (int *)cp;
                    *ip = m_pNpcList[sV2]->m_bIsSummoned;
                    cp += 4;

                    ip = (int *)cp;
                    *ip = m_pNpcList[sV2]->m_cActionLimit;
                    cp += 4;

                    ip = (int *)cp;
                    *ip = m_pNpcList[sV2]->m_iHitDice;
                    cp += 4;

                    dwp = (uint32_t *)cp; // v1.4
                    *dwp = m_pNpcList[sV2]->m_dwDeadTime;
                    cp += 4;

                    dwp = (uint32_t *)cp;
                    *dwp = m_pNpcList[sV2]->m_dwRegenTime;
                    cp += 4;

                    ip = (int *)cp;
                    *ip = (int)m_pNpcList[sV2]->m_bIsKilled;
                    cp += 4;

                    iRet = m_pClientList[iToH]->iSendMsg(cData, 26 + 12);
                    break;
            }
            break;

        case DEF_NOTIFY_NPCTALK:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV3;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV4;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV5;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV6;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV7;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV8;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV9;
            cp += 2;

            if (pString != 0) memcpy(cp, pString, 20);
            cp += 20;

            if (pString2 != 0) memcpy(cp, pString2, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 64);
            break;

        case DEF_NOTIFY_POTIONSUCCESS:
        case DEF_NOTIFY_LOWPOTIONSKILL:
        case DEF_NOTIFY_POTIONFAIL:
        case DEF_NOTIFY_NOMATCHINGPOTION:

            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_SUPERATTACKLEFT:
            sp = (short *)cp;
            *sp = m_pClientList[iToH]->m_iSuperAttackLeft;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_SAFEATTACKMODE:
            *cp = m_pClientList[iToH]->m_bIsSafeAttackMode;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 7);
            break;

        case DEF_NOTIFY_QUERY_JOINPARTY:
        case DEF_NOTIFY_IPACCOUNTINFO:
            strcpy(cp, pString);
            cp += strlen(pString);

            iRet = m_pClientList[iToH]->iSendMsg(cData, 6 + strlen(pString) + 1);
            break;

        case DEF_NOTIFY_REWARDGOLD:
            dwp = (uint32_t *)cp;
            *dwp = m_pClientList[iToH]->m_iRewardGold;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_SERVERSHUTDOWN:
            *cp = (char)sV1;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 7);
            break;

        case DEF_NOTIFY_GLOBALATTACKMODE:
        case DEF_NOTIFY_WHETHERCHANGE:
            *cp = (char)sV1;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 7);
            break;

        case DEF_NOTIFY_MONSTEREVENT_POSITION:
            *cp = (char)sV3;
            cp++;

            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 11);
            break;


        case DEF_NOTIFY_FISHCANCELED:
        case DEF_NOTIFY_FISHSUCCESS:
        case DEF_NOTIFY_FISHFAIL:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_DEBUGMSG:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_FISHCHANCE:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_ENERGYSPHEREGOALIN:
        case DEF_NOTIFY_EVENTFISHMODE:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV3;
            cp += 2;

            memcpy(cp, pString, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 32);
            break;

        case DEF_NOTIFY_NOTICEMSG:
            memcpy(cp, pString, strlen(pString));
            cp += strlen(pString);

            *cp = 0;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, strlen(pString) + 7);
            break;

        case DEF_NOTIFY_CANNOTRATING:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_RATINGPLAYER:
            *cp = (char)sV1;
            cp++;

            memcpy(cp, pString, 10);
            cp += 10;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iRating;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 22);
            break;

        case DEF_NOTIFY_ADMINUSERLEVELLOW:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_PLAYERSHUTUP:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            memcpy(cp, pString, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 19);
            break;

        case DEF_NOTIFY_TIMECHANGE:
            *cp = (char)sV1;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 7);
            break;

        case DEF_NOTIFY_TOBERECALLED:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_HUNGER:
            *cp = (char)sV1;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 7);
            break;

        case DEF_NOTIFY_PLAYERPROFILE:
            if (strlen(pString) > 100)
            {
                memcpy(cp, pString, 100);
                cp += 100;
            }
            else
            {
                memcpy(cp, pString, strlen(pString));
                cp += strlen(pString);
            }
            *cp = 0;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 7 + strlen(pString));
            break;

        case DEF_NOTIFY_PLAYERONGAME:
            memcpy(cp, pString, 10);
            cp += 10;
            if (pString != 0)
            {
                memcpy(cp, pString2, 14);
                cp += 14;
            }
            iRet = m_pClientList[iToH]->iSendMsg(cData, 31);
            break;

        case DEF_NOTIFY_WHISPERMODEON:
        case DEF_NOTIFY_WHISPERMODEOFF:
        case DEF_NOTIFY_PLAYERNOTONGAME:
            memcpy(cp, pString, 10);
            cp += 10;
            memcpy(cp, "             ", 10);
            cp += 10;
            iRet = m_pClientList[iToH]->iSendMsg(cData, 27);
            break;

        case DEF_NOTIFY_ITEMSOLD:
        case DEF_NOTIFY_ITEMREPAIRED:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV1;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV2;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
            break;

        case DEF_NOTIFY_REPAIRITEMPRICE:
        case DEF_NOTIFY_SELLITEMPRICE:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV1;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV2;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV3;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV4;
            cp += 4;

            memcpy(cp, pString, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 42);
            break;

        case DEF_NOTIFY_CANNOTREPAIRITEM:
        case DEF_NOTIFY_CANNOTSELLITEM:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            memcpy(cp, pString, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 30);

            break;

        case DEF_NOTIFY_SHOWMAP:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_SKILLUSINGEND:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_TOTALUSERS:
            wp = (uint16_t *)cp;
            *wp = (WORD)(m_iTotalGameServerClients); //_iGetTotalClients();
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;

        case DEF_NOTIFY_MAGICEFFECTOFF:
        case DEF_NOTIFY_MAGICEFFECTON:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV2;
            cp += 4;

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV3;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 16);
            break;

        case DEF_NOTIFY_CANNOTITEMTOBANK:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_SKILL:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_SETITEMCOUNT:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)sV2;
            cp += 4;

            *cp = (char)sV3;
            cp++;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 13);
            break;

        case DEF_NOTIFY_ITEMDEPLETED_ERASEITEM:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            ip = (int *)cp;
            *ip = (int)sV2;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 12);
            break;

        case DEF_NOTIFY_DROPITEMFIN_ERASEITEM:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            ip = (int *)cp;
            *ip = (int)sV2;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 12);
            break;

        case DEF_NOTIFY_CANNOTGIVEITEM:
        case DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            ip = (int *)cp;
            *ip = (int)sV2;
            cp += 4;

            memcpy(cp, pString, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 32);
            break;

        case DEF_NOTIFY_GIVEITEMFIN_ERASEITEM:
            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            ip = (int *)cp;
            *ip = (int)sV2;
            cp += 4;

            memcpy(cp, pString, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 32);
            break;

        case DEF_NOTIFY_ENEMYKILLREWARD:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iExp;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iEnemyKillCount;
            cp += 4;
            memcpy(cp, m_pClientList[sV1]->m_cCharName, 10);
            cp += 10;
            memcpy(cp, m_pClientList[sV1]->m_cGuildName, 20);
            cp += 20;
            sp = (short *)cp;
            *sp = (short)m_pClientList[sV1]->m_iGuildRank;
            cp += 2;
            sp = (short *)cp;
            *sp = (short)m_pClientList[iToH]->m_iWarContribution;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 48);
            break;

        case DEF_NOTIFY_PKCAPTURED:


            wp = (uint16_t *)cp;
            *wp = (WORD)sV1;
            cp += 2;
            wp = (uint16_t *)cp;
            *wp = (WORD)sV2;
            cp += 2;
            memcpy(cp, pString, 10);
            cp += 10;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iRewardGold;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = m_pClientList[iToH]->m_iExp;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 28);
            break;

        case DEF_NOTIFY_PKPENALTY:

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iExp;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iStr;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iVit;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iDex;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iInt;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iMag;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iCharisma;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iPKCount;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 38);
            break;

        case DEF_NOTIFY_TRAVELERLIMITEDLEVEL:
        case DEF_NOTIFY_LIMITEDLEVEL:

            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iExp;
            cp += 4;
            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_ITEMRELEASED:
        case DEF_NOTIFY_ITEMLIFESPANEND:

            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;
            sp = (short *)cp;
            *sp = (short)sV2;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_KILLED:

            memcpy(cp, pString, 20);
            cp += 20;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 26);
            break;

        case DEF_NOTIFY_EXP:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iExp;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iRating;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
            break;

        case DEF_NOTIFY_HP:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iHP;
            cp += 4;
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iMP;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
            break;

        case DEF_NOTIFY_MP:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iMP;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_SP:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iSP;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_CHARISMA:
            dwp = (uint32_t *)cp;
            *dwp = (DWORD)m_pClientList[iToH]->m_iCharisma;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;

        case DEF_NOTIFY_STATECHANGE_FAILED:		// 2003-04-14 ÃÃ¶ÃÂ¸ Ã†Ã·Ã€ÃŽÃ†Â®Â¸Â¦ Â·Â¹ÂºÂ§ Â¼Ã¶ÃÂ¤Â¿Â¡ Â½Ã‡Ã†Ã..korean buttplugs
        case DEF_NOTIFY_SETTING_FAILED:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;

        case DEF_NOTIFY_STATECHANGE_SUCCESS:
        {
            int i;

            for (i = 0; i < DEF_MAXMAGICTYPE; i++)
            {
                *cp = m_pClientList[iToH]->m_cMagicMastery[i];
                cp++;
            }

            for (i = 0; i < DEF_MAXSKILLTYPE; i++)
            {
                *cp = m_pClientList[iToH]->m_cSkillMastery[i];
                cp++;
            }

            iRet = m_pClientList[iToH]->iSendMsg(cData, 6 + DEF_MAXMAGICTYPE + DEF_MAXSKILLTYPE);
        }
        break;
        case DEF_NOTIFY_SETTING_SUCCESS:
        case DEF_NOTIFY_LEVELUP:
            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iLevel;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iStr;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iVit;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iDex;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iInt;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iMag;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pClientList[iToH]->m_iCharisma;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 34);
            break;

        case DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION:
        case DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION:
        case DEF_NOTIFY_CANNOTJOINMOREGUILDSMAN:

            memcpy(cp, m_pClientList[iFromH]->m_cCharName, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 16);
            break;

        case DEF_COMMONTYPE_JOINGUILDAPPROVE:
            if (m_pClientList[iFromH] != 0)
                memcpy(cp, m_pClientList[iFromH]->m_cGuildName, 20);
            else memcpy(cp, "?", 1);
            cp += 20;

            sp = (short *)cp;
            *sp = DEF_GUILDSTARTRANK;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 28);
            break;

        case DEF_COMMONTYPE_JOINGUILDREJECT:
        case DEF_COMMONTYPE_DISMISSGUILDAPPROVE:
        case DEF_COMMONTYPE_DISMISSGUILDREJECT:
            if (m_pClientList[iFromH] != 0)
                memcpy(cp, m_pClientList[iFromH]->m_cGuildName, 20);
            else memcpy(cp, "?", 1);
            cp += 20;

            sp = (short *)cp;
            *sp = DEF_GUILDSTARTRANK;
            cp += 2;

            memcpy(cp, m_pClientList[iToH]->m_cLocation, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 38);
            break;

        case DEF_NOTIFY_GUILDDISBANDED:

            memcpy(cp, pString, 20);
            cp += 20;

            memcpy(cp, m_pClientList[iToH]->m_cLocation, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 36);
            break;


        case DEF_NOTIFY_FIGHTZONERESERVE:
            ip = (int *)cp;
            *ip = (int)sV1;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
            break;


        case DEF_NOTIFY_NOGUILDMASTERLEVEL:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;


        case DEF_NOTIFY_CANNOTBANGUILDMAN:
            iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
            break;


        case DEF_NOTIFY_FORCERECALLTIME:
            sp = (short *)cp;
            *sp = (short)sV1;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
            break;
    }

    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:


            //DeleteClient(iToH, true, true);
            return;
    }
}



uint32_t CGame::dwGetItemCount(int iClientH, char * pName)
{
    int i;
    char cTmpName[21];

    if (m_pClientList[iClientH] == 0) return 0;

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pName);

    for (i = 0; i < DEF_MAXITEMS; i++)
        if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0))
        {

            return m_pClientList[iClientH]->m_pItemList[i]->m_dwCount;
        }

    return 0;
}

int CGame::SetItemCount(int iClientH, char * pItemName, uint32_t dwCount)
{
    int i;
    char cTmpName[21];
    uint16_t wWeight;

    if (m_pClientList[iClientH] == 0) return -1;

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pItemName);

    for (i = 0; i < DEF_MAXITEMS; i++)
        if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0))
        {

            wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[i], 1);// m_pClientList[iClientH]->m_pItemList[i]->m_wWeight;


            if (dwCount == 0)
            {
                ItemDepleteHandler(iClientH, i, false);
            }
            else
            {

                m_pClientList[iClientH]->m_pItemList[i]->m_dwCount = dwCount;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETITEMCOUNT, i, dwCount, (char)true, 0);
            }

            return wWeight;
        }

    return -1;
}


int CGame::SetItemCount(int iClientH, int iItemIndex, uint32_t dwCount)
{
    uint16_t wWeight;

    if (m_pClientList[iClientH] == 0) return -1;
    if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return -1;

    wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1);//m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wWeight;


    if (dwCount == 0)
    {
        ItemDepleteHandler(iClientH, iItemIndex, false);
    }
    else
    {

        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount = dwCount;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETITEMCOUNT, iItemIndex, dwCount, (char)true, 0);
    }

    return wWeight;
}



bool CGame::_bInitNpcAttr(CNpc * pNpc, char * pNpcName, short sClass, char cSA)
{
    int i, iTemp;
    char cTmpName[21];
    double dV1, dV2, dV3;

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pNpcName);

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        if (m_pNpcConfigList[i] != 0)
        {
            if (memcmp(cTmpName, m_pNpcConfigList[i]->m_cNpcName, 20) == 0)
            {

                memset(pNpc->m_cNpcName, 0, sizeof(pNpc->m_cNpcName));
                memcpy(pNpc->m_cNpcName, m_pNpcConfigList[i]->m_cNpcName, 20);

                pNpc->m_sType = m_pNpcConfigList[i]->m_sType;


                //			if (m_pNpcConfigList[i]->m_iHitDice <= 5)
                //				 pNpc->m_iHP  = abs(iDice(m_pNpcConfigList[i]->m_iHitDice, 4) + m_pNpcConfigList[i]->m_iHitDice);
                //			else
                //				pNpc->m_iHP  = abs((m_pNpcConfigList[i]->m_iHitDice * 4) + m_pNpcConfigList[i]->m_iHitDice + iDice(1, m_pNpcConfigList[i]->m_iHitDice));

                pNpc->m_iHP = m_pNpcConfigList[i]->m_iHitDice * 6;
                if (pNpc->m_iHP == 0) pNpc->m_iHP = 1;
                pNpc->m_iMaxHP = m_pNpcConfigList[i]->m_iHitDice * 6;
                pNpc->m_bIsKilled = false;
                //Change HP bar

                pNpc->m_iExpDiceMin = m_pNpcConfigList[i]->m_iExpDiceMin;
                pNpc->m_iExpDiceMax = m_pNpcConfigList[i]->m_iExpDiceMax;
                pNpc->m_iGoldDiceMin = m_pNpcConfigList[i]->m_iGoldDiceMin;
                pNpc->m_iGoldDiceMax = m_pNpcConfigList[i]->m_iGoldDiceMax;
                pNpc->m_iExp = (iDice(1, (m_pNpcConfigList[i]->m_iExpDiceMax - m_pNpcConfigList[i]->m_iExpDiceMin)) + m_pNpcConfigList[i]->m_iExpDiceMin);

                pNpc->m_iHitDice = m_pNpcConfigList[i]->m_iHitDice;
                pNpc->m_iDefenseRatio = m_pNpcConfigList[i]->m_iDefenseRatio;
                pNpc->m_iHitRatio = m_pNpcConfigList[i]->m_iHitRatio;
                pNpc->m_iMinBravery = m_pNpcConfigList[i]->m_iMinBravery;
                pNpc->m_cAttackDiceThrow = m_pNpcConfigList[i]->m_cAttackDiceThrow;
                pNpc->m_cAttackDiceRange = m_pNpcConfigList[i]->m_cAttackDiceRange;
                pNpc->m_cSize = m_pNpcConfigList[i]->m_cSize;
                pNpc->m_cSide = m_pNpcConfigList[i]->m_cSide;
                pNpc->m_cActionLimit = m_pNpcConfigList[i]->m_cActionLimit;
                pNpc->m_dwActionTime = m_pNpcConfigList[i]->m_dwActionTime;
                pNpc->m_dwRegenTime = m_pNpcConfigList[i]->m_dwRegenTime;
                pNpc->m_cResistMagic = m_pNpcConfigList[i]->m_cResistMagic;
                pNpc->m_cMagicLevel = m_pNpcConfigList[i]->m_cMagicLevel;
                pNpc->m_iMaxMana = m_pNpcConfigList[i]->m_iMaxMana; // v1.4
                pNpc->m_iMana = m_pNpcConfigList[i]->m_iMaxMana;
                pNpc->m_cChatMsgPresence = m_pNpcConfigList[i]->m_cChatMsgPresence;
                pNpc->m_cDayOfWeekLimit = m_pNpcConfigList[i]->m_cDayOfWeekLimit;
                pNpc->m_cTargetSearchRange = m_pNpcConfigList[i]->m_cTargetSearchRange;

                switch (sClass)
                {
                    case 43:
                    case 44:
                    case 45:
                    case 46:
                    case 47:
                    case 51:
                        pNpc->m_iAttackStrategy = DEF_ATTACKAI_NORMAL;
                        break;

                    case 53:
                        cSA = 1;
                        break;

                    default:
                        pNpc->m_iAttackStrategy = iDice(1, 10);
                        break;
                }

                pNpc->m_iAILevel = iDice(1, 3);
                pNpc->m_iAbsDamage = m_pNpcConfigList[i]->m_iAbsDamage;
                pNpc->m_iMagicHitRatio = m_pNpcConfigList[i]->m_iMagicHitRatio;
                pNpc->m_iAttackRange = m_pNpcConfigList[i]->m_iAttackRange;
                pNpc->m_cSpecialAbility = cSA;
                pNpc->m_iBuildCount = m_pNpcConfigList[i]->m_iMinBravery;
                pNpc->m_cAttribute = m_pNpcConfigList[i]->m_cAttribute;


                switch (pNpc->m_cSpecialAbility)
                {
                    case 1:
                        dV2 = (double)pNpc->m_iExp;
                        dV3 = 25.0f / 100.0f;
                        dV1 = dV2 * dV3;
                        pNpc->m_iExp += (int)dV1;
                        break;

                    case 2:
                        dV2 = (double)pNpc->m_iExp;
                        dV3 = 30.0f / 100.0f;
                        dV1 = dV2 * dV3;
                        pNpc->m_iExp += (int)dV1;
                        break;

                    case 3: // Absorbing Physical Damage
                        if (pNpc->m_iAbsDamage > 0)
                        {

                            pNpc->m_cSpecialAbility = 0;
                            cSA = 0;
                        }
                        else
                        {
                            iTemp = 20 + iDice(1, 60);
                            pNpc->m_iAbsDamage -= iTemp;
                            if (pNpc->m_iAbsDamage < -90) pNpc->m_iAbsDamage = -90;
                        }

                        dV2 = (double)pNpc->m_iExp;
                        dV3 = (double)abs(pNpc->m_iAbsDamage) / 100.0f;
                        dV1 = dV2 * dV3;
                        pNpc->m_iExp += (int)dV1;
                        break;

                    case 4: // Absorbing Magical Damage
                        if (pNpc->m_iAbsDamage < 0)
                        {

                            pNpc->m_cSpecialAbility = 0;
                            cSA = 0;
                        }
                        else
                        {
                            iTemp = 20 + iDice(1, 60);
                            pNpc->m_iAbsDamage += iTemp;
                            if (pNpc->m_iAbsDamage > 90) pNpc->m_iAbsDamage = 90;
                        }

                        dV2 = (double)pNpc->m_iExp;
                        dV3 = (double)(pNpc->m_iAbsDamage) / 100.0f;
                        dV1 = dV2 * dV3;
                        pNpc->m_iExp += (int)dV1;
                        break;

                    case 5:
                        dV2 = (double)pNpc->m_iExp;
                        dV3 = 15.0f / 100.0f;
                        dV1 = dV2 * dV3;
                        pNpc->m_iExp += (int)dV1;
                        break;

                    case 6:
                    case 7:
                        dV2 = (double)pNpc->m_iExp;
                        dV3 = 20.0f / 100.0f;
                        dV1 = dV2 * dV3;
                        pNpc->m_iExp += (int)dV1;
                        break;

                    case 8:
                        dV2 = (double)pNpc->m_iExp;
                        dV3 = 25.0f / 100.0f;
                        dV1 = dV2 * dV3;
                        pNpc->m_iExp += (int)dV1;
                        break;
                }


                pNpc->m_iNoDieRemainExp = (pNpc->m_iExp) - (pNpc->m_iExp / 3);


                pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFF0FF;//Change 0xF0FF
                iTemp = cSA;
                iTemp = iTemp << 8;
                pNpc->m_iStatus = pNpc->m_iStatus | iTemp;


                pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFFFF0;
                pNpc->m_iStatus = pNpc->m_iStatus | (sClass);

                return true;
            }
        }


    return false;
}

int CGame::iDice(int iThrow, int iRange)
{
    int i, iRet;

    if (iRange <= 0) return 0;

    iRet = 0;
    for (i = 1; i <= iThrow; i++)
    {

        iRet += (rand() % iRange) + 1;
    }

    return iRet;
}

void CGame::OnStartGameSignal()
{
    int i;


    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0)
            _bReadMapInfoFiles(i);


    bReadCrusadeStructureConfigFile("Crusade.cfg");

    _LinkStrikePointMapIndex();

    bReadCrusadeGUIDFile("GameData\\CrusadeGUID.txt");

    bReadCrusadeScheduleConfigFile("WarSchedule.cfg");

    log->info("");
    log->info("(!) Game Server Activated.");

    m_pGold = new CItem;
    _bInitItemAttr(m_pGold, "Gold");

}

void CGame::CalculateGuildEffect(int iVictimH, char cVictimType, short sAttackerH)
{
    int ix, iy, iExp;
    short sOwnerH;
    char cOwnerType;


    return;

    if (m_pClientList[sAttackerH] == 0) return;

    switch (cVictimType)
    {

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[iVictimH] == 0) return;

            for (ix = m_pNpcList[iVictimH]->m_sX - 2; ix <= m_pNpcList[iVictimH]->m_sX + 2; ix++)
                for (iy = m_pNpcList[iVictimH]->m_sY - 2; iy <= m_pNpcList[iVictimH]->m_sY + 2; iy++)
                {

                    m_pMapList[m_pNpcList[iVictimH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);

                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (sOwnerH != 0) && (sOwnerH != sAttackerH) &&
                        (memcmp(m_pClientList[sAttackerH]->m_cGuildName, m_pClientList[sOwnerH]->m_cGuildName, 20) == 0) &&
                        (m_pClientList[sAttackerH]->m_iLevel >= m_pClientList[sOwnerH]->m_iLevel))
                    {


                        if (iDice(1, 3) == 2)
                        {
                            iExp = (m_pNpcList[iVictimH]->m_iExp / 3);

                            //				if (m_pClientList[sAttackerH]->m_iLevel >= m_iPlayerMaxLevel) iExp = 0;

                            if (iExp > 0)
                            {

                                m_pClientList[sOwnerH]->m_iExp += iExp;
                                if (bCheckLimitedUser(sOwnerH) == false)
                                {

                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
                                }

                                bCheckLevelUp(sOwnerH);
                            }
                        }
                    }
                }
            break;
    }
}



void CGame::SendGuildMsg(int iClientH, uint16_t wNotifyMsgType, short sV1, short sV2, char * pString)
{
    char cData[500];
    uint32_t * dwp;
    uint16_t * wp;
    char * cp;
    int i, iRet = 0;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) &&
            (memcmp(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName, 20) == 0))
        {


            memset(cData, 0, sizeof(cData));

            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = wNotifyMsgType;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);


            switch (wNotifyMsgType)
            {
                case DEF_NOTIFY_GUILDDISBANDED:
                    if (i == iClientH) break;

                    memcpy(cp, m_pClientList[iClientH]->m_cGuildName, 20);
                    cp += 20;

                    iRet = m_pClientList[i]->iSendMsg(cData, 26);

                    memset(m_pClientList[i]->m_cGuildName, 0, sizeof(m_pClientList[i]->m_cGuildName));
                    strcpy(m_pClientList[i]->m_cGuildName, "NONE");
                    m_pClientList[i]->m_iGuildRank = -1;
                    m_pClientList[i]->m_iGuildGUID = -1;
                    break;

                case DEF_NOTIFY_EVENTMSGSTRING:

                    strcpy(cp, pString);
                    cp += strlen(pString);

                    iRet = m_pClientList[i]->iSendMsg(cData, 6 + strlen(pString) + 1);
                    break;

                case DEF_NOTIFY_NEWGUILDSMAN:
                    memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
                    cp += 10;

                    iRet = m_pClientList[i]->iSendMsg(cData, 6 + 10 + 1);
                    break;

                case DEF_NOTIFY_DISMISSGUILDSMAN:
                    memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
                    cp += 10;

                    iRet = m_pClientList[i]->iSendMsg(cData, 6 + 10 + 1);
                    break;
            }

            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:

                    DeleteClient(i, true, true);
                    return;
            }
        }

}












void CGame::ReleaseFollowMode(short sOwnerH, char cOwnerType)
{
    int i;

    for (i = 0; i < DEF_MAXNPCS; i++)
        if ((i != sOwnerH) && (m_pNpcList[i] != 0))
        {
            if ((m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW) &&
                (m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH) &&
                (m_pNpcList[i]->m_cFollowOwnerType == cOwnerType))
            {

                m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOMWAYPOINT;
            }
        }
}

// 12-22 ¼ºÈÄ´Ï ¼öÁ¤  ¸¶¹ý¹è¿ì±â¿¡ ½ÇÆÐÇÑ ÀÌÀ¯¸¦ Å¬¶óÀÌ¾ðÆ®¿¡ º¸³»ÁØ´Ù.

int CGame::_iGetMagicNumber(char * pMagicName, int * pReqInt, int * pCost)
{
    int i;
    char cTmpName[31];

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pMagicName);

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        if (m_pMagicConfigList[i] != 0)
        {
            if (memcmp(cTmpName, m_pMagicConfigList[i]->m_cName, 30) == 0)
            {

                *pReqInt = (int)m_pMagicConfigList[i]->m_sIntLimit;
                *pCost = (int)m_pMagicConfigList[i]->m_iGoldCost;

                return i;
            }
        }

    return -1;
}

void CGame::TrainSkillResponse(bool bSuccess, int iClientH, int iSkillNum, int iSkillLevel)
{
    char * cp, cData[100];
    uint32_t * dwp;
    uint16_t * wp;
    int   iRet;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if ((iSkillNum < 0) || (iSkillNum > 100)) return;
    if ((iSkillLevel < 0) || (iSkillLevel > 100)) return;

    if (bSuccess == true)
    {

        if (m_pClientList[iClientH]->m_cSkillMastery[iSkillNum] != 0) return;

        m_pClientList[iClientH]->m_cSkillMastery[iSkillNum] = iSkillLevel;

        bCheckTotalSkillMasteryPoints(iClientH, iSkillNum);


        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_SKILLTRAINSUCCESS;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);


        *cp = iSkillNum;
        cp++;

        // ?â?ú ??º? .
        *cp = iSkillLevel;
        cp++;

#ifdef DEF_TAIWANLOG
        if (m_pSkillConfigList[iSkillNum]->m_cName != 0)
            _bItemLog(DEF_ITEMLOG_SKILLLEARN, iClientH, m_pSkillConfigList[iSkillNum]->m_cName, NULL);
#endif


        iRet = m_pClientList[iClientH]->iSendMsg(cData, 8);
        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:

                DeleteClient(iClientH, true, true);
                return;
        }
    }
    else
    {


    }

}


int CGame::_iGetSkillNumber(char * pSkillName)
{
    int i;
    char cTmpName[21];

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pSkillName);

    for (i = 1; i < DEF_MAXSKILLTYPE; i++)
        if (m_pSkillConfigList[i] != 0)
        {
            if (memcmp(cTmpName, m_pSkillConfigList[i]->m_cName, 20) == 0)
            {

                return i;
            }
        }

    return 0;
}

void CGame::Quit()
{
    int i;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != 0) delete m_pClientList[i];

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0) delete m_pNpcList[i];

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0) delete m_pMapList[i];

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != 0) delete m_pItemConfigList[i];

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        if (m_pNpcConfigList[i] != 0) delete m_pNpcConfigList[i];

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        if (m_pMagicConfigList[i] != 0) delete m_pMagicConfigList[i];

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        if (m_pSkillConfigList[i] != 0) delete m_pSkillConfigList[i];

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        if (m_pQuestConfigList[i] != 0) delete m_pQuestConfigList[i];

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != 0) delete m_pDynamicObjectList[i];

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] != 0) delete m_pDelayEventList[i];

    for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
        if (m_pNoticeMsgList[i] != 0) delete m_pNoticeMsgList[i];

    for (i = 0; i < DEF_MAXFISHS; i++)
        if (m_pFish[i] != 0) delete m_pFish[i];

    for (i = 0; i < DEF_MAXMINERALS; i++)
        if (m_pMineral[i] != 0) delete m_pMineral[i];

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        if (m_pPortionConfigList[i] != 0) delete m_pPortionConfigList[i];

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        if (m_pBuildItemList[i] != 0) delete m_pBuildItemList[i];

    for (i = 0; i < DEF_MAXDUPITEMID; i++)
        if (m_pDupItemIDList[i] != 0) delete m_pDupItemIDList[i];

    if (m_pNoticementData != 0) delete m_pNoticementData;

    if (m_pGold != 0) delete m_pGold;

}

int CGame::iGetLevelExp(int iLevel)
{
    int iRet;

    if (iLevel == 0) return 0;

    iRet = iGetLevelExp(iLevel - 1) + iLevel * (50 + (iLevel * (iLevel / 17) * (iLevel / 17)));


    return iRet;
}

int CGame::_iCalcSkillSSNpoint(int iLevel)
{
    int iRet;

    if (iLevel < 1) return 1;

    if (iLevel <= 50)
        iRet = iLevel;
    else if (iLevel > 50)
    {
        iRet = (iLevel * iLevel) / 10;
    }

    return iRet;
}

bool CGame::bCheckLevelUp(int iClientH)
{
    bool bStr, bVit, bDex, bInt, bMag, bChr;
    char cLoopCnt;

    if (m_pClientList[iClientH] == 0) return false;


    if (m_pClientList[iClientH]->m_iLevel >= m_iPlayerMaxLevel)
    {

        if (m_pClientList[iClientH]->m_iExp >= m_iLevelExpTable[m_iPlayerMaxLevel + 1])
        {

            m_pClientList[iClientH]->m_iExp = m_iLevelExpTable[m_iPlayerMaxLevel];
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);

            m_pClientList[iClientH]->m_iGizonItemUpgradeLeft++;
            // if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft > 8) m_pClientList[iClientH]->m_iGizonItemUpgradeLeft = 8;
            if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft > DEF_MAXGIZONPOINT) m_pClientList[iClientH]->m_iGizonItemUpgradeLeft = DEF_MAXGIZONPOINT; // adamas
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 1, 0, 0);
        }
        return false;
    }

    if (m_pClientList[iClientH]->m_iExp < m_pClientList[iClientH]->m_iNextLevelExp) return false;

    bStr = bVit = bDex = bInt = bMag = bChr = false;

    cLoopCnt = 0;
    while (1)
    {
        if (cLoopCnt++ > 100) return false;
        if (m_pClientList[iClientH]->m_iExp >= m_pClientList[iClientH]->m_iNextLevelExp)
        {



            if (m_pClientList[iClientH]->m_iLevel >= m_iPlayerMaxLevel)
            {
                if (m_pClientList[iClientH]->m_iExp >= m_iLevelExpTable[m_iPlayerMaxLevel + 1])
                {
                    m_pClientList[iClientH]->m_iExp = m_iLevelExpTable[m_iPlayerMaxLevel];
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
                    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft++;
                    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft > DEF_MAXGIZONPOINT) m_pClientList[iClientH]->m_iGizonItemUpgradeLeft = DEF_MAXGIZONPOINT; // adamas
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 1, 0, 0);
                }
                return false;
            }


            m_pClientList[iClientH]->m_iLevel++;
            m_pClientList[iClientH]->m_iLU_Point += 3;

            if ((m_pClientList[iClientH]->m_iLevel > DEF_LIMITHUNTERLEVEL - 1) && (m_pClientList[iClientH]->m_bIsHunter == true))
            {
                SetNoHunterMode(iClientH, true);
            }


            if (m_pClientList[iClientH]->m_iStr > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iStr = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iDex > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iDex = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iVit > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iVit = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iInt > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iInt = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iMag > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iMag = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iCharisma > DEF_CHARPOINTLIMIT) m_pClientList[iClientH]->m_iCharisma = DEF_CHARPOINTLIMIT;


            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LEVELUP, 0, 0, 0, 0);


            m_pClientList[iClientH]->m_iNextLevelExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel + 1]; //iGetLevelExp(m_pClientList[iClientH]->m_iLevel + 1);


            CalcTotalItemEffect(iClientH, -1, false);

            if (m_pClientList[iClientH]->m_iLevel >= DEF_PLAYERMAXLEVEL)
            {
                wsprintf(G_cTxt, "(!) Character(%s) Level up. (%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iLevel);
                log->info(G_cTxt);
            }
        }
        else return true;
    }

    return false;
}

// 2003-04-21 Ã€ÃŽÃ†Â®Â¿Â¡ ÂµÃ»Â¸Â¥ Â¸Â¶Â¹Ã½Ã€Â» Â»Ã¨ÃÂ¦ Â½ÃƒÃ„Ã‘ÃÃ˜Â´Ã™...
/////////////////////////////////////////////////////////////////////////////////////
//  bool CGame::bCheckMagicInt(int iClientH)  //another retarded korean function
//  desc		 :: Ã€ÃŽÃ†Â®Â°Â¡ Â³Â»Â·ÃÂ°Â¬Ã€Â»Â¶Â§ Â»Ã¨ÃÂ¦Ã‡Ã’ Â¸Â¶Â¹Ã½Ã€Â» ÃƒÂ³Â¸Â®Ã‡Ã‘Â´Ã™... ÃƒÂ¼Ã…Â©Â¸Â¸...
//	return value :: Â¹Â«ÃÂ¶Â°Ã‡ true   // ....dumbass koreans
//  date		 :: 2003-04-21
/////////////////////////////////////////////////////////////////////////////////////
bool CGame::bCheckMagicInt(int iClientH)
{

    for (int i = 0; i < DEF_MAXMAGICTYPE; i++)
    {
        if (m_pMagicConfigList[i] != 0)
            if (m_pMagicConfigList[i]->m_sIntLimit > m_pClientList[iClientH]->m_iInt)
            {
                m_pClientList[iClientH]->m_cMagicMastery[i] = 0;
            }
    }

    return true;
}

// 2003-04-14 ÃÃ¶ÃÂ¸ Ã†Ã·Ã€ÃŽÃ†Â®Â¸Â¦ Â·Â¹ÂºÂ§ Â¼Ã¶ÃÂ¤Â¿Â¡ Â¾ÂµÂ¼Ã¶ Ã€Ã–Â´Ã™...
/////////////////////////////////////////////////////////////////////////////////////
//  bChangeState(char cStateChange 
//  		  ,char* cStr, char *cVit,char *cDex,char *cInt,char *cMag,char *cChar)
//  desc		 :: ÃÃ¶ÃÂ¸ Ã†Ã·Ã€ÃŽÃ†Â®Â·ÃŽ Ã†Â¯Â¼ÂºÂ°Âª Â¹Ã™Â²ÃœÆ’â€Ã€Â» Â´ÃµÃ‡Ã˜ ÃÃ˜Â´Ã™...
//	return value :: BOOLÃ‡Ã¼ 0(FASLE) Â¿Â¡Â·Â¯ Â·Â¹ÂºÂ§ Â¼Ã¶ÃÂ¤ ÂºÃ’Â°Â¡...
//  date		 :: [2003-04-14] 
/////////////////////////////////////////////////////////////////////////////////////
bool CGame::bChangeState(char cStateChange, char * cStr, char * cVit, char * cDex, char * cInt, char * cMag, char * cChar)
{
    if (cStateChange == DEF_STR)
        *cStr += 1;
    else if (cStateChange == DEF_VIT)
        *cVit += 1;
    else if (cStateChange == DEF_DEX)
        *cDex += 1;
    else if (cStateChange == DEF_INT)
        *cInt += 1;
    else if (cStateChange == DEF_MAG)
        *cMag += 1;
    else if (cStateChange == DEF_CHR)
        *cChar += 1;
    else
        return 0;

    return cStateChange;
}



bool CGame::bCheckLimitedUser(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return false;

    if ((m_pClientList[iClientH]->m_cSide == DEF_NETURAL) &&
        (m_pClientList[iClientH]->m_iExp >= m_iLevelExp20))
    {


        m_pClientList[iClientH]->m_iExp = m_iLevelExp20 - 1;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_TRAVELERLIMITEDLEVEL, 0, 0, 0, 0);
        return true;
    }

    return false;
}




bool CGame::bSetItemToBankItem(int iClientH, short sItemIndex)
{
    int i, iRet;
    uint32_t * dwp;
    uint16_t * wp;
    char * cp;
    short * sp;
    char cData[100]{};
    CItem * pItem;


    if (m_pClientList[iClientH] == 0) return false;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return false;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return false;

    if (m_pClientList[iClientH]->m_bIsOnWarehouse == false) return false;


    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] == 0)
        {


            m_pClientList[iClientH]->m_pItemInBankList[i] = m_pClientList[iClientH]->m_pItemList[sItemIndex];
            pItem = m_pClientList[iClientH]->m_pItemInBankList[i];

            m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;


            iCalcTotalWeight(iClientH);

            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_ITEMTOBANK;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

            *cp = i;
            cp++;


            *cp = 1;
            cp++;

            memcpy(cp, pItem->m_cName, 20);
            cp += 20;

            dwp = (uint32_t *)cp;
            *dwp = pItem->m_dwCount;
            cp += 4;

            *cp = pItem->m_cItemType;
            cp++;

            *cp = pItem->m_cEquipPos;
            cp++;

            *cp = (char)0;
            cp++;

            sp = (short *)cp;
            *sp = pItem->m_sLevelLimit;
            cp += 2;

            *cp = pItem->m_cGenderLimit;
            cp++;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wCurLifeSpan;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wWeight;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSprite;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSpriteFrame;
            cp += 2;

            *cp = pItem->m_cItemColor;
            cp++;

            // v1.432
            sp = (short *)cp;
            *sp = pItem->m_sItemEffectValue2;
            cp += 2;

            // v1.42
            dwp = (uint32_t *)cp;
            *dwp = pItem->m_dwAttribute;
            cp += 4;


            *cp = (char)pItem->m_sItemSpecEffectValue2;
            cp++;


            iRet = m_pClientList[iClientH]->iSendMsg(cData, 56);

#ifdef DEF_TAIWANLOG
            _bItemLog(DEF_ITEMLOG_DEPOSIT, iClientH, (int)-1, pItem);
#endif


            m_pClientList[iClientH]->m_bIsBankModified = true;

            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:

                    // DeleteClient(iClientH, true, true);
                    return true;
            }

            return true;
        }


    return false;
}


void CGame::ApplyPKpenalty(short sAttackerH, short sVictumH)
{
    int iV1, iV2;

    if (m_pClientList[sAttackerH] == 0) return;
    if (m_pClientList[sVictumH] == 0) return;


    if ((m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true) && (m_pClientList[sAttackerH]->m_iPKCount == 0)) return;


    m_pClientList[sAttackerH]->m_iPKCount++;

    _bPKLog(DEF_PKLOG_BYPK, sAttackerH, sVictumH, NULL);


    iV1 = iDice((m_pClientList[sVictumH]->m_iLevel / 2) + 1, 50);
    iV2 = iDice((m_pClientList[sAttackerH]->m_iLevel / 2) + 1, 50);

    m_pClientList[sAttackerH]->m_iExp -= iV1;
    m_pClientList[sAttackerH]->m_iExp -= iV2;
    if (m_pClientList[sAttackerH]->m_iExp < 0) m_pClientList[sAttackerH]->m_iExp = 0;


    SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_PKPENALTY, 0, 0, 0, 0);


    SendEventToNearClient_TypeA(sAttackerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);


    //wsprintf(G_cTxt, "(!) PK-penalty: °ø°ÝÀÚ(%s) ¼Õ½Ç °æÇèÄ¡(%d) °æÇèÄ¡(%d) ", m_pClientList[sAttackerH]->m_cCharName, iV1+iV2, m_pClientList[sAttackerH]->m_iExp);
    //log->info(G_cTxt);


    m_stCityStatus[m_pClientList[sAttackerH]->m_cSide].iCrimes++;


    m_pClientList[sAttackerH]->m_iRating -= 10;
    if (m_pClientList[sAttackerH]->m_iRating > 10000)  m_pClientList[sAttackerH]->m_iRating = 10000;
    if (m_pClientList[sAttackerH]->m_iRating < -10000) m_pClientList[sAttackerH]->m_iRating = -10000;


    if (m_pClientList[sAttackerH]->m_cSide == DEF_ARESDEN)
    { // 2002-11-15 ¼öÁ¤
#ifndef DEF_PKPENALTY
        if (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cLocationName, "aresden") != 0) return;
#endif			

        // v2.16 ¼ºÈÄ´Ï ¼öÁ¤
        memset(m_pClientList[sAttackerH]->m_cLockedMapName, 0, sizeof(m_pClientList[sAttackerH]->m_cLockedMapName));
        strcpy(m_pClientList[sAttackerH]->m_cLockedMapName, "arejail");
        m_pClientList[sAttackerH]->m_iLockedMapTime = 60 * 3;
        RequestTeleportHandler(sAttackerH, "2   ", "arejail", -1, -1);
        return;
    }

    if (m_pClientList[sAttackerH]->m_cSide == DEF_ELVINE)
    { // 2002-11-15 ¼öÁ¤

#ifndef DEF_PKPENALTY
        if (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cLocationName, "elvine") != 0) return;
#endif		


        memset(m_pClientList[sAttackerH]->m_cLockedMapName, 0, sizeof(m_pClientList[sAttackerH]->m_cLockedMapName));
        strcpy(m_pClientList[sAttackerH]->m_cLockedMapName, "elvjail");
        m_pClientList[sAttackerH]->m_iLockedMapTime = 60 * 3;
        RequestTeleportHandler(sAttackerH, "2   ", "elvjail", -1, -1);
        return;

    }
}



void CGame::ApplyCombatKilledPenalty(int iClientH, char cPenaltyLevel, bool bIsSAattacked, bool bItemDrop)
{
    int iExp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    // Crusade
    if (m_bIsCrusadeMode == true)
    {


        if (m_pClientList[iClientH]->m_iPKCount > 0)
        {
            m_pClientList[iClientH]->m_iPKCount--;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PKPENALTY, 0, 0, 0, 0);
            // v2.15 
            _bPKLog(DEF_PKLOG_REDUCECRIMINAL, (int)-1, iClientH, NULL);

        }
        return;
    }
    else
    {

        if (m_pClientList[iClientH]->m_iPKCount > 0)
        {
            m_pClientList[iClientH]->m_iPKCount--;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PKPENALTY, 0, 0, 0, 0);
            // v2.15
            _bPKLog(DEF_PKLOG_REDUCECRIMINAL, (int)-1, iClientH, NULL);
        }


        iExp = iDice(1, (5 * cPenaltyLevel * m_pClientList[iClientH]->m_iLevel));


        if (m_pClientList[iClientH]->m_bIsNeutral == true) iExp = iExp / 3;


        // if (m_pClientList[iClientH]->m_iLevel == DEF_PLAYERMAXLEVEL) iExp = 0;

        m_pClientList[iClientH]->m_iExp -= iExp;
        if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);


        if (m_pClientList[iClientH]->m_bIsNeutral != true)
        {
            // v2.19 2002-11-14
#ifdef DEF_ITEMDROP
            if (m_pClientList[iClientH]->m_iLevel < 80)
            {

                // v2.03 60 -> 80
                cPenaltyLevel--;
                if (cPenaltyLevel <= 0) cPenaltyLevel = 1;

                _PenaltyItemDrop(iClientH, cPenaltyLevel, bIsSAattacked, bItemDrop);

            }
            else
            {
                _PenaltyItemDrop(iClientH, cPenaltyLevel, bIsSAattacked, bItemDrop);
            }
#else
            if (m_pClientList[iClientH]->m_iLevel < 80)
            {

                // v2.03 60 -> 80
                cPenaltyLevel--;
                if (cPenaltyLevel <= 0) cPenaltyLevel = 1;
                _PenaltyItemDrop(iClientH, cPenaltyLevel, bIsSAattacked);
            }
            else _PenaltyItemDrop(iClientH, cPenaltyLevel, bIsSAattacked);
#endif
        }
    }
}


void CGame::_PenaltyItemDrop(int iClientH, int iTotal, bool bIsSAattacked, bool bItemDrop)
{
    int i, j, iRemainItem;
    char cItemIndexList[DEF_MAXITEMS], cItemIndex;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    if ((m_pClientList[iClientH]->m_iAlterItemDropIndex != -1) && (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex] != 0))
    {
        // Testcode
        if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP)
        {


            if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan > 0)
                m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan--;

            DropItemHandler(iClientH, m_pClientList[iClientH]->m_iAlterItemDropIndex, -1, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_cName);

            m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
        }
        else
        {
            // v2.04 testcode
            log->info("Alter Drop Item Index Error1");

            for (i = 0; i < DEF_MAXITEMS; i++)
                if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP))
                {
                    m_pClientList[iClientH]->m_iAlterItemDropIndex = i;
                    if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan > 0)
                        m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan--;

                    DropItemHandler(iClientH, m_pClientList[iClientH]->m_iAlterItemDropIndex, -1, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_cName);
                    m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
                    return;
                }


            goto PID_DROP;
        }
        return;
    }

    PID_DROP:;

    for (i = 1; i <= iTotal; i++)
    {
        iRemainItem = 0;
        memset(cItemIndexList, 0, sizeof(cItemIndexList));

#ifdef DEF_ITEMDROP
        //v2.19 2002-11-14 ¼Ò¸ð¼º ¾ÆÀÌÅÛ°ú ¸Ô´Â ¾ÆÀÌÅÛ¸¸ µå¶ø
        if (bItemDrop)
        {
            //¹üÁËÀÚ...			
            for (j = 0; j < DEF_MAXITEMS; j++)
                if (m_pClientList[iClientH]->m_pItemList[j] != 0)
                {
                    cItemIndexList[iRemainItem] = j;
                    iRemainItem++;
                }
        }
        else
        {
            //
            for (j = 0; j < DEF_MAXITEMS; j++)
                if (m_pClientList[iClientH]->m_pItemList[j] != 0
                    && ((m_pClientList[iClientH]->m_pItemList[j]->m_cItemType == DEF_ITEMTYPE_CONSUME)
                        || (m_pClientList[iClientH]->m_pItemList[j]->m_cItemType == DEF_ITEMTYPE_EAT)))
                {
                    cItemIndexList[iRemainItem] = j;
                    iRemainItem++;
                }
        }
#else
        for (j = 0; j < DEF_MAXITEMS; j++)
            if (m_pClientList[iClientH]->m_pItemList[j] != 0)
            {
                cItemIndexList[iRemainItem] = j;
                iRemainItem++;
            }
#endif
        if (iRemainItem == 0) return;
        cItemIndex = cItemIndexList[iDice(1, iRemainItem) - 1];

        if ((m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectType != 0) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectValue1 == m_pClientList[iClientH]->m_sCharIDnum1) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectValue2 == m_pClientList[iClientH]->m_sCharIDnum2) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectValue3 == m_pClientList[iClientH]->m_sCharIDnum3))
        {

        }
        else if (((m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) ||
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)) &&
            (bIsSAattacked == false))
        {

        }
        else if ((m_pClientList[iClientH]->m_bIsLuckyEffect == true) && (iDice(1, 10) == 5))
        {

        }
        else DropItemHandler(iClientH, cItemIndex, -1, m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_cName);
    }
}

void CGame::GetRewardMoneyHandler(int iClientH)
{
    int iRet, iEraseReq, iWeightLeft, iRewardGoldLeft;
    uint32_t * dwp;
    uint16_t * wp;
    char * cp, cData[100], cItemName[21];
    CItem * pItem;
    short * sp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;



    iWeightLeft = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);

    if (iWeightLeft <= 0) return;

    iWeightLeft = iWeightLeft / 2;
    if (iWeightLeft <= 0) return;

    pItem = new CItem;
    memset(cItemName, 0, sizeof(cItemName));
    wsprintf(cItemName, "Gold");
    _bInitItemAttr(pItem, cItemName);
    //pItem->m_dwCount = m_pClientList[iClientH]->m_iRewardGold;


    if ((iWeightLeft / iGetItemWeight(pItem, 1)) >= m_pClientList[iClientH]->m_iRewardGold)
    {

        pItem->m_dwCount = m_pClientList[iClientH]->m_iRewardGold;
        iRewardGoldLeft = 0;
    }
    else
    {
        // (iWeightLeft / pItem->m_wWeight)¸¸ ¹Þ´Â´Ù.
        pItem->m_dwCount = (iWeightLeft / iGetItemWeight(pItem, 1));
        iRewardGoldLeft = m_pClientList[iClientH]->m_iRewardGold - (iWeightLeft / iGetItemWeight(pItem, 1));
    }

    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
    {



        m_pClientList[iClientH]->m_iRewardGold = iRewardGoldLeft;

        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);


        *cp = 1;
        cp++;

        memcpy(cp, pItem->m_cName, 20);
        cp += 20;

        dwp = (uint32_t *)cp;
        *dwp = pItem->m_dwCount;
        cp += 4;

        *cp = pItem->m_cItemType;
        cp++;

        *cp = pItem->m_cEquipPos;
        cp++;

        *cp = (char)0;
        cp++;

        sp = (short *)cp;
        *sp = pItem->m_sLevelLimit;
        cp += 2;

        *cp = pItem->m_cGenderLimit;
        cp++;

        wp = (uint16_t *)cp;
        *wp = pItem->m_wCurLifeSpan;
        cp += 2;

        wp = (uint16_t *)cp;
        *wp = pItem->m_wWeight;
        cp += 2;

        sp = (short *)cp;
        *sp = pItem->m_sSprite;
        cp += 2;

        sp = (short *)cp;
        *sp = pItem->m_sSpriteFrame;
        cp += 2;

        *cp = pItem->m_cItemColor;
        cp++;

        *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
        cp++;

        dwp = (uint32_t *)cp;
        *dwp = pItem->m_dwAttribute;
        cp += 4;
        /*
        *cp = (char)(pItem->m_dwAttribute & 0x00000001);
        cp++;
        */

        if (iEraseReq == 1) delete pItem;


        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:

                DeleteClient(iClientH, true, true);
                return;
        }


        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REWARDGOLD, 0, 0, 0, 0);
    }
    else
    {


    }
}


int CGame::_iCalcMaxLoad(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return 0;

    return (m_pClientList[iClientH]->m_iStr * 500 + m_pClientList[iClientH]->m_iLevel * 500);
}

int CGame::iAddDynamicObjectList(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, uint32_t dwLastTime, int iV1)
{
    int i;
    short sPreType;
    uint32_t dwTime, dwRegisterTime;


    m_pMapList[cMapIndex]->bGetDynamicObject(sX, sY, &sPreType, &dwRegisterTime);
    if (sPreType != 0) return 0;


    switch (sType)
    {
        case DEF_DYNAMICOBJECT_FIRE:
            if (m_pMapList[cMapIndex]->bGetIsMoveAllowedTile(sX, sY) == false)
                return 0;

            if (dwLastTime != 0)
            {
                switch (m_pMapList[cMapIndex]->m_cWhetherStatus)
                {
                    case 1:	dwLastTime = dwLastTime - (dwLastTime / 2);       break;
                    case 2:	dwLastTime = (dwLastTime / 2) - (dwLastTime / 3); break;
                    case 3:	dwLastTime = (dwLastTime / 3) - (dwLastTime / 4); break;
                }


                if (dwLastTime == 0) dwLastTime = 1000;
            }
            break;

        case DEF_DYNAMICOBJECT_FISHOBJECT:
        case DEF_DYNAMICOBJECT_FISH:
            if (m_pMapList[cMapIndex]->bGetIsWater(sX, sY) == false)
                return 0;
            break;

        case DEF_DYNAMICOBJECT_MINERAL1:
        case DEF_DYNAMICOBJECT_MINERAL2:
            if (m_pMapList[cMapIndex]->bGetMoveable(sX, sY) == false)
                return 0;

            m_pMapList[cMapIndex]->SetTempMoveAllowedFlag(sX, sY, false);
            break;
    }


    for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] == 0)
        {

            dwTime = timeGetTime();

            if (dwLastTime != 0)
                dwLastTime += (iDice(1, 4) * 1000);

            m_pDynamicObjectList[i] = new CDynamicObject(sOwner, cOwnerType, sType, cMapIndex, sX, sY, dwTime, dwLastTime, iV1);

            m_pMapList[cMapIndex]->SetDynamicObject(i, sType, sX, sY, dwTime);
            SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_CONFIRM, cMapIndex, sX, sY, sType, i, 0);

            return i;
        }

    return 0;
}

void CGame::CheckDynamicObjectList()
{
    int i;
    uint32_t dwTime = timeGetTime(), dwRegisterTime;
    short sType;


    for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)
    {
        if ((m_pDynamicObjectList[i] != 0) && (m_pDynamicObjectList[i]->m_dwLastTime != 0))
        {

            switch (m_pDynamicObjectList[i]->m_sType)
            {
                case DEF_DYNAMICOBJECT_FIRE:

                    switch (m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus)
                    {
                        case 0: break;
                        case 1:
                        case 2:
                        case 3:

                            m_pDynamicObjectList[i]->m_dwLastTime = m_pDynamicObjectList[i]->m_dwLastTime -
                                (m_pDynamicObjectList[i]->m_dwLastTime / 10) * m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus;
                            break;
                    }
                    break;
            }
        }
    }


    for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)
    {
        if ((m_pDynamicObjectList[i] != 0) && (m_pDynamicObjectList[i]->m_dwLastTime != 0) &&
            ((dwTime - m_pDynamicObjectList[i]->m_dwRegisterTime) >= m_pDynamicObjectList[i]->m_dwLastTime))
        {


            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, &sType, &dwRegisterTime);


            if (dwRegisterTime == m_pDynamicObjectList[i]->m_dwRegisterTime)
            {
                SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[i]->m_cMapIndex, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, m_pDynamicObjectList[i]->m_sType, i, 0);

                m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->SetDynamicObject(NULL, 0, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, dwTime);
            }

            switch (sType)
            {
                case DEF_DYNAMICOBJECT_FISHOBJECT:
                case DEF_DYNAMICOBJECT_FISH:

                    bDeleteFish(m_pDynamicObjectList[i]->m_sOwner, 2);
                    break;
            }


            delete m_pDynamicObjectList[i];
            m_pDynamicObjectList[i] = 0;
        }
    }
}

void CGame::CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue)
{
    short sSkillIndex;
    int   iOldSSN, iSSNpoint, iWeaponIndex;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_pItemList[sWeaponIndex] == 0) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;

    sSkillIndex = m_pClientList[iClientH]->m_pItemList[sWeaponIndex]->m_sRelatedSkill;
    if ((sSkillIndex < 0) || (sSkillIndex >= DEF_MAXSKILLTYPE)) return;

    if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] == 0) return;

#ifdef DEF_TESTSERVER
    iValue = iValue * 100;
#endif

    iOldSSN = m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex];
    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] += iValue;

    iSSNpoint = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] + 1];


    if ((m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] < 100) &&
        (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] > iSSNpoint))
    {


        m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]++;


        switch (sSkillIndex)
        {
            case 0:
            case 2:
            case 5:
            case 13:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iStr * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 3:

                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iLevel * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 4:
            case 21:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iMag * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 1:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 14:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iDex * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 12:
            case 15:
            case 19:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iInt * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 23:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iVit * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            default:
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;
        }


        if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] == 0)
        {

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
            {

                iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex)
                {

                    m_pClientList[iClientH]->m_iHitRatio++;
                }
            }

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
            {

                iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex)
                {

                    m_pClientList[iClientH]->m_iHitRatio++;
                }
            }
        }

        if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] == 0)
        {

            bCheckTotalSkillMasteryPoints(iClientH, sSkillIndex);

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], 0, 0);
        }
    }
}


void CGame::CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue)
{
    int   iOldSSN, iSSNpoint, iWeaponIndex;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if ((sSkillIndex < 0) || (sSkillIndex >= DEF_MAXSKILLTYPE)) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;


    if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] == 0) return;


    switch (sSkillIndex)
    {
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 14:
        case 21:
            iValue *= 2;
            break;
    }

#ifdef DEF_TESTSERVER
    iValue = iValue * 100;

#endif

    iOldSSN = m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex];
    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] += iValue;

    iSSNpoint = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] + 1];


    if ((m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] < 100) &&
        (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] > iSSNpoint))
    {


        m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]++;

        switch (sSkillIndex)
        {
            case 0:
            case 5:
            case 13:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iStr * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 3:

                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iLevel * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 4:
            case 21:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iMag * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 1:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 14: // v2.16 ¼ºÈÄ´Ï ¼öÁ¤ 
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iDex * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 2:		//v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã INT·Î Á¦ÇÑ..
            case 12:
            case 15:
            case 19:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iInt * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            case 23:
                if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iVit * 2))
                {

                    m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
                }
                else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;

            default:
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
                break;
        }


        if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] == 0)
        {

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
            {

                iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex)
                {

                    m_pClientList[iClientH]->m_iHitRatio++;
                }
            }

            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
            {

                iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex)
                {

                    m_pClientList[iClientH]->m_iHitRatio++;
                }
            }
        }

        if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] == 0)
        {

            bCheckTotalSkillMasteryPoints(iClientH, sSkillIndex);


            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], 0, 0);
        }
    }
}

void CGame::MobGenerator()
{
    int i, j, iNamingValue, iResult, iTotalMob;
    char cNpcName[21], cName_Master[11], cName_Slave[11], cWaypoint[11];
    char cSA;
    int  pX, pY, iMapLevel = 0, iProbSA = 0, iKindSA, iResultNum, iMin{}, iFirmSAType;
    bool bMaster, bFirmBerserk, bIsSpecialEvent;
    double dV1{}, dV2{}, dV3{};
    short sNpcType = 0;


    if (m_bOnExitProcess == true) return;

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        // Random Mob Generator
        iResultNum = 0;

#ifdef DEF_CHECKUSERNPCNUM

        if ((m_pMapList[i] != 0) && (m_pMapList[i]->m_bRandomMobGenerator == true))
        {

            if (m_iTotalGameServerClients >= 1000)
            {
                iResultNum = (m_pMapList[i]->m_iMaximumObject - 30);
            }
            else
            {
                dV2 = (double)m_iTotalGameServerClients;
                dV3 = (m_pMapList[i]->m_iMaximumObject - 30);

                dV1 = (dV2 / 1000.0f) * dV3;
                iResultNum = (int)dV1;
            }


            iMin = (m_pMapList[i]->m_iMaximumObject - 30) - ((m_pMapList[i]->m_iMaximumObject - 30) / 3);
            if (iResultNum > (m_pMapList[i]->m_iMaximumObject - 30)) iResultNum = (m_pMapList[i]->m_iMaximumObject - 30);
            if (iResultNum < iMin) iResultNum = iMin;
        }
#endif		

        //if ( (m_pMapList[i] != 0) && (m_pMapList[i]->m_bRandomMobGenerator == true) && 
        //	 ((m_pMapList[i]->m_iMaximumObject - 30) > m_pMapList[i]->m_iTotalActiveObject) ) {

        if (m_pMapList[i] != 0)
        {
            iResultNum = (m_pMapList[i]->m_iMaximumObject - 30);
        }

        if ((m_pMapList[i] != 0) && (m_pMapList[i]->m_bRandomMobGenerator == true) && (iResultNum > m_pMapList[i]->m_iTotalActiveObject))
        {



            if ((m_iMiddlelandMapIndex != -1) && (m_iMiddlelandMapIndex == i) && (m_bIsCrusadeMode == true)) break;

            iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
            if (iNamingValue != -1)
            {

                memset(cName_Master, 0, sizeof(cName_Master));
                wsprintf(cName_Master, "XX%d", iNamingValue);
                cName_Master[0] = '_';
                cName_Master[1] = i + 65;

                memset(cNpcName, 0, sizeof(cNpcName));


                iFirmSAType = 0;
                bFirmBerserk = false;
                iResult = iDice(1, 100);

                switch (m_pMapList[i]->m_cRandomMobGeneratorLevel)
                {
                    case 1:
                        if ((iResult >= 1) && (iResult < 20))
                        {
                            iResult = 1;
                        }
                        else if ((iResult >= 20) && (iResult < 40))
                        {
                            iResult = 2;
                        }
                        else if ((iResult >= 40) && (iResult < 85))
                        {
                            iResult = 24;
                        }
                        else if ((iResult >= 85) && (iResult < 95))
                        {
                            iResult = 25;
                        }
                        else if ((iResult >= 95) && (iResult <= 100))
                        {
                            iResult = 3;
                        }

                        iMapLevel = 1;
                        break;

                    case 2:
                        if ((iResult >= 1) && (iResult < 40))
                        {
                            iResult = 1;
                        }
                        else if ((iResult >= 40) && (iResult < 80))
                        {
                            iResult = 2;
                        }
                        else iResult = 10;
                        iMapLevel = 1;
                        break;

                    case 3:
                        if ((iResult >= 1) && (iResult < 20))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 3; break;
                                case 2: iResult = 4; break;
                            }
                        }
                        else if ((iResult >= 20) && (iResult < 25))
                        {
                            iResult = 30;  //v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã
                        }
                        else if ((iResult >= 25) && (iResult < 50))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iResult = 5; break;
                                case 2:	iResult = 6; break;
                                case 3: iResult = 7; break;
                            }
                        }
                        else if ((iResult >= 50) && (iResult < 75))
                        {

                            switch (iDice(1, 7))
                            {
                                case 1:
                                case 2: iResult = 8;  break;
                                case 3:	iResult = 11; break;
                                case 4:	iResult = 12; break;
                                case 5: iResult = 18; break;
                                case 6: iResult = 26; break;
                                case 7: iResult = 28; break;	// Ettin
                            }
                        }
                        else if ((iResult >= 75) && (iResult <= 100))
                        {

                            switch (iDice(1, 5))
                            {
                                case 1:
                                case 2:	iResult = 9;  break;
                                case 3:	iResult = 13; break;
                                case 4: iResult = 14; break;
                                case 5: iResult = 27; break;	// Mountain-Giant
                            }
                        }
                        iMapLevel = 4;
                        break;

                    case 4:
                        if ((iResult >= 1) && (iResult < 50))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1:	iResult = 2; break;
                                case 2: iResult = 10; break;
                            }
                        }
                        else if ((iResult >= 50) && (iResult < 80))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 8; break;
                                case 2: iResult = 11; break;
                            }
                        }
                        else if ((iResult >= 80) && (iResult < 100))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 14; break;
                                case 2:	iResult = 9; break;
                            }
                        }
                        iMapLevel = 2;
                        break;

                    case 5:
                        if ((iResult >= 1) && (iResult < 30))
                        {
                            switch (iDice(1, 5))
                            {
                                case 1:
                                case 2:
                                case 3:
                                case 4:
                                case 5: iResult = 2; break;
                            }
                        }
                        else if ((iResult >= 30) && (iResult < 60))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 3; break;
                                case 2: iResult = 4; break;
                            }
                        }
                        else if ((iResult >= 60) && (iResult < 80))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 5; break;
                                case 2: iResult = 7; break;
                            }
                        }
                        else if ((iResult >= 80) && (iResult < 95))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1:
                                case 2: iResult = 8;  break;
                                case 3:	iResult = 11; break;
                            }
                        }
                        else if ((iResult >= 95) && (iResult <= 100))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iResult = 11; break;
                                case 2: iResult = 14; break;
                                case 3: iResult = 9; break;
                            }
                        }
                        iMapLevel = 3;
                        break;

                    case 6: // huntzone 2,4
                        if ((iResult >= 1) && (iResult < 60))
                        {
                            switch (iDice(1, 4))
                            {
                                case 1: iResult = 5; break;
                                case 2:	iResult = 6; break;
                                case 3: iResult = 7; break;
                                case 4: iResult = 4; break;
                            }
                        }
                        else if ((iResult >= 60) && (iResult < 90))
                        {
                            switch (iDice(1, 4))
                            {
                                case 1:
                                case 2: iResult = 8;  break;
                                case 3:	iResult = 11; break;
                                case 4:	iResult = 12; break;
                            }
                        }
                        else if ((iResult >= 90) && (iResult <= 100))
                        {


                            switch (iDice(1, 8))
                            {
                                case 1: iResult = 26; break;
                                case 2:	iResult = 9;  break;
                                case 3:	iResult = 13; break;
                                case 4: iResult = 14; break;
                                case 5: iResult = 18; break;
                                case 6: iResult = 28; break;	// Ettin
                                case 7: iResult = 27; break;	// Mountain-Giant
                                case 8: iResult = 29; break;	// Cannibal-Plant
                            }
                        }
                        iMapLevel = 4;
                        break;

                    case 7:
                        if ((iResult >= 1) && (iResult < 50))
                        {
                            switch (iDice(1, 4))
                            {
                                case 1:	iResult = 1;  break;
                                case 2: iResult = 2;  break;
                                case 3: iResult = 10; break;
                                case 4: iResult = 3;  break;
                            }
                        }
                        else if ((iResult >= 50) && (iResult < 60))
                        {
                            iResult = 30;  //v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã
                        }
                        else if ((iResult >= 60) && (iResult < 85))
                        {
                            switch (iDice(1, 4))
                            {
                                case 1:
                                case 2:
                                case 3: iResult = 6;  break;
                                case 4: iResult = 12; break;
                            }
                        }
                        else if ((iResult >= 85) && (iResult < 100))
                        {
                            switch (iDice(1, 4))
                            {
                                case 1: iResult = 12; break;
                                case 2: iResult = 2;  break;
                                case 3:
                                    if (iDice(1, 100) == 13)
                                        iResult = 17;
                                    else iResult = 12;
                                    break;
                                    // 2002-09-18 #1
                                case 4: iResult = 29; break; // Cannibal-Plant
                            }
                        }
                        iMapLevel = 4;
                        break;

                    case 8:
                        if ((iResult >= 1) && (iResult < 70))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1:	iResult = 4;  break;
                                case 2: iResult = 5;  break;
                            }
                        }
                        else if ((iResult >= 70) && (iResult < 90))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iResult = 8;  break;
                                case 2: iResult = 11; break;
                                case 3: iResult = 14; break;
                            }
                        }
                        else if ((iResult >= 90) && (iResult < 100))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 9;  break;
                                case 2: iResult = 14; break;
                            }
                        }
                        iMapLevel = 4;
                        break;

                    case 9:
                        if ((iResult >= 1) && (iResult < 70))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1:	iResult = 4;  break;
                                case 2: iResult = 5;  break;
                            }
                        }
                        else if ((iResult >= 70) && (iResult < 95))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iResult = 8;  break;
                                case 2: iResult = 9;  break;
                                case 3: iResult = 13; break;
                            }
                        }
                        else if ((iResult >= 95) && (iResult < 100))
                        {
                            switch (iDice(1, 6))
                            {
                                case 1:
                                case 2:
                                case 3: iResult = 9;  break;
                                case 4:
                                case 5: iResult = 14; break;
                                case 6: iResult = 15; break;
                            }
                        }


                        if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = true;

                        iMapLevel = 4;
                        break;

                    case 10:
                        if ((iResult >= 1) && (iResult < 70))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1:	iResult = 4; break;
                                case 2: iResult = 5; break;
                            }
                        }
                        else if ((iResult >= 70) && (iResult < 95))
                        {
                            // 
                            switch (iDice(1, 3))
                            {
                                case 1:
                                case 2:	iResult = 13; break;
                                case 3: iResult = 14; break;
                            }
                        }
                        else if ((iResult >= 95) && (iResult < 100))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1:
                                case 2: iResult = 14; break;
                                case 3: iResult = 15; break;
                            }
                        }


                        if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = true;

                        iMapLevel = 5;
                        break;

                    case 11:
                        if ((iResult >= 1) && (iResult < 30))
                        {
                            switch (iDice(1, 5))
                            {
                                case 1:
                                case 2:
                                case 3:
                                case 4:
                                case 5: iResult = 2; break;
                            }
                        }
                        else if ((iResult >= 30) && (iResult < 60))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 3; break;
                                case 2: iResult = 4; break;
                            }
                        }
                        else if ((iResult >= 60) && (iResult < 80))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 5; break;
                                case 2: iResult = 7; break;
                            }
                        }
                        else if ((iResult >= 80) && (iResult < 95))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1:
                                case 2: iResult = 10;  break;
                                case 3:	iResult = 11; break;
                            }
                        }
                        else if ((iResult >= 95) && (iResult <= 100))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iResult = 11; break;
                                case 2: iResult = 7; break;
                                case 3: iResult = 8; break;
                            }
                        }
                        iMapLevel = 4;
                        break;

                    case 12: // middled1n
                        if ((iResult >= 1) && (iResult < 50))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1:	iResult = 1; break;
                                case 2: iResult = 2; break;
                                case 3: iResult = 10; break;
                            }
                        }
                        else if ((iResult >= 50) && (iResult < 85))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 5; break;
                                case 2: iResult = 4; break;
                            }
                        }
                        else if ((iResult >= 85) && (iResult < 100))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iResult = 8; break;
                                case 2: iResult = 11; break;
                                case 3: iResult = 26; break;
                            }
                        }
                        iMapLevel = 4;
                        break;

                    case 13:
                        if ((iResult >= 1) && (iResult < 15))
                        {

                            iResult = 4;
                            bFirmBerserk = true;
                            iFirmSAType = 4 - (iDice(1, 2) - 1);
                        }
                        else if ((iResult >= 15) && (iResult < 40))
                        {

                            iResult = 14;
                            bFirmBerserk = true;
                            iFirmSAType = 4 - (iDice(1, 2) - 1);
                        }

                        else if ((iResult >= 40) && (iResult < 60))
                        {

                            iResult = 9;
                            bFirmBerserk = true;
                            iFirmSAType = 4 - (iDice(1, 2) - 1);
                        }
                        else if ((iResult >= 60) && (iResult < 75))
                        {

                            iResult = 13;
                            bFirmBerserk = true;
                            iFirmSAType = 4 - (iDice(1, 2) - 1);
                        }
                        else if ((iResult >= 75) && (iResult < 95))
                        {

                            iResult = 23;
                        }
                        else if ((iResult >= 95) && (iResult < 100))
                        {
                            // ºñÈ¦´õ
                            iResult = 22;
                        }
                        iMapLevel = 4;
                        break;

                    case 14:
                        if ((iResult >= 1) && (iResult < 30))
                        {

                            iResult = 23;
                        }
                        else if ((iResult >= 30) && (iResult < 50))
                        {
                            // ºñÈ¦´õ
                            iResult = 22;
                        }
                        else if ((iResult >= 50) && (iResult < 70))
                        {

                            iResult = 15;
                            bFirmBerserk = true;
                            iFirmSAType = 4 - (iDice(1, 2) - 1);
                        }
                        else if ((iResult >= 70) && (iResult < 90))
                        {

                            iResult = 16;
                        }
                        else if ((iResult >= 90) && (iResult < 100))
                        {

                            iResult = 21;
                        }
                        iMapLevel = 4;
                        break;

                    case 15:
                        if ((iResult >= 1) && (iResult < 35))
                        {

                            iResult = 23;
                            bFirmBerserk = true;
                        }
                        else if ((iResult >= 35) && (iResult < 50))
                        {
                            // ºñÈ¦´õ
                            iResult = 22;
                            bFirmBerserk = true;
                        }
                        else if ((iResult >= 50) && (iResult < 80))
                        {

                            iResult = 16;
                            bFirmBerserk = true;
                        }
                        else if ((iResult >= 80) && (iResult < 100))
                        {

                            iResult = 21;
                        }
                        iMapLevel = 4;
                        break;
                    case 16: //Huntzone 1,2 ÃÊº¸ ¹Ìµé.  //v2.19 2002-12-9
                        if ((iResult >= 1) && (iResult < 40))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1:	iResult = 1; break;
                                case 2: iResult = 2; break;
                                case 3: iResult = 10; break;
                            }
                        }
                        else if ((iResult >= 40) && (iResult < 50))
                        {
                            iResult = 30;	//v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã
                        }
                        else if ((iResult >= 50) && (iResult < 85))
                        {
                            switch (iDice(1, 2))
                            {
                                case 1: iResult = 5; break;
                                case 2: iResult = 4; break;
                            }
                        }
                        else if ((iResult >= 85) && (iResult < 100))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iResult = 8; break;
                                case 2: iResult = 11; break;
                                case 3: iResult = 26; break;
                            }
                        }
                        iMapLevel = 1;
                        break;
                    case 17:	//v2.19 2002-12-18 ¸Ê ¸ó½ºÅÍ ¾ÆÀÌÅÛ Ãß°¡
                        if ((iResult >= 1) && (iResult < 30))
                        {
                            switch (iDice(1, 4))
                            {
                                case 1:	iResult = 22; break;
                                case 2: iResult = 8; break;
                                case 3: iResult = 24; break;
                                case 4: iResult = 5; break;
                            }
                        }
                        else if ((iResult >= 30) && (iResult < 40))
                        {
                            iResult = 30;	//v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã
                        }
                        else if ((iResult >= 40) && (iResult < 70))
                        {
                            iResult = 32;

                        }
                        else if ((iResult >= 70) && (iResult < 90))
                        {
                            iResult = 31;
                            if (iDice(1, 5) == 1) bFirmBerserk = true;
                        }
                        else if ((iResult >= 90) && (iResult <= 100))
                        {
                            iResult = 33;
                        }
                        iMapLevel = 1;
                        break;

                }

                pX = 0;
                pY = 0;


                bIsSpecialEvent = false;
                if ((m_bIsSpecialEventTime == true) && (iDice(1, 10) == 3)) bIsSpecialEvent = true;

                if (bIsSpecialEvent == true)
                {
                    switch (m_cSpecialEventType)
                    {
                        case 1:

                            if (m_pMapList[i]->m_iMaxPx != 0)
                            {
                                pX = m_pMapList[i]->m_iMaxPx * 20 + 10;
                                pY = m_pMapList[i]->m_iMaxPy * 20 + 10;

                                if (pX < 0) pX = 0;
                                if (pY < 0) pY = 0;

                                if (m_bIsCrusadeMode == true)
                                {

                                    if (strcmp(m_pMapList[i]->m_cName, "aresden") == 0)
                                        iResult = 20;
                                    else if (strcmp(m_pMapList[i]->m_cName, "elvine") == 0)
                                        iResult = 19;
                                }

                                wsprintf(G_cTxt, "(!) Mob-Event Map(%s) Loc(%d,%d)", m_pMapList[i]->m_cName, pX, pY);
                                log->info(G_cTxt);
                            }
                            break;

                        case 2:

                            if (iDice(1, 3) == 2)
                            {




                                if ((memcmp(m_pMapList[i]->m_cName, "aresden", 7) == 0) ||
                                    (memcmp(m_pMapList[i]->m_cName, "middled1n", 9) == 0) ||
                                    (memcmp(m_pMapList[i]->m_cName, "arefarm", 7) == 0) ||
                                    (memcmp(m_pMapList[i]->m_cName, "elvfarm", 7) == 0) ||
                                    (memcmp(m_pMapList[i]->m_cName, "elvine", 6) == 0))
                                {

                                    if (iDice(1, 30) == 5)
                                        iResult = 16;
                                    else iResult = 5;
                                }
                                else iResult = 16;
                            }
                            else iResult = 17;


                            m_bIsSpecialEventTime = false;
                            break;
                    }
                }

                memset(cNpcName, 0, sizeof(cNpcName));
                switch (iResult)
                {
                    case 1:  strcpy(cNpcName, "Slime");     sNpcType = 10;   iProbSA = 5;  iKindSA = 1; break;
                    case 2:  strcpy(cNpcName, "Giant-Ant"); sNpcType = 16;   iProbSA = 10; iKindSA = 2; break;
                    case 3:  strcpy(cNpcName, "Orc");       sNpcType = 14;   iProbSA = 15; iKindSA = 1; break;
                    case 4:  strcpy(cNpcName, "Zombie");    sNpcType = 18;   iProbSA = 15; iKindSA = 3; break;
                    case 5:  strcpy(cNpcName, "Skeleton");  sNpcType = 11;   iProbSA = 35; iKindSA = 8; break;
                    case 6:  strcpy(cNpcName, "Orc-Mage");  sNpcType = 14;   iProbSA = 30; iKindSA = 7; break;
                    case 7:  strcpy(cNpcName, "Scorpion");  sNpcType = 17;   iProbSA = 15; iKindSA = 3; break;
                    case 8:  strcpy(cNpcName, "Stone-Golem"); sNpcType = 12; iProbSA = 25; iKindSA = 5; break;
                    case 9:  strcpy(cNpcName, "Cyclops");    sNpcType = 13;  iProbSA = 35; iKindSA = 8; break;
                    case 10: strcpy(cNpcName, "Amphis");     sNpcType = 22;  iProbSA = 20; iKindSA = 3; break;
                    case 11: strcpy(cNpcName, "Clay-Golem"); sNpcType = 23;  iProbSA = 20; iKindSA = 5; break;
                    case 12: strcpy(cNpcName, "Troll");      sNpcType = 28;  iProbSA = 25; iKindSA = 3; break;
                    case 13: strcpy(cNpcName, "Orge");       sNpcType = 29;  iProbSA = 25; iKindSA = 1; break;
                    case 14: strcpy(cNpcName, "Hellbound");  sNpcType = 27;  iProbSA = 25; iKindSA = 8; break;
                    case 15: strcpy(cNpcName, "Liche");		 sNpcType = 30;  iProbSA = 30; iKindSA = 8; break;
                    case 16: strcpy(cNpcName, "Demon");		 sNpcType = 31;  iProbSA = 20; iKindSA = 8; break;
                    case 17: strcpy(cNpcName, "Unicorn");	 sNpcType = 32;  iProbSA = 35; iKindSA = 7; break;
                    case 18: strcpy(cNpcName, "WereWolf");	 sNpcType = 33;  iProbSA = 25; iKindSA = 1; break;
                    case 19: strcpy(cNpcName, "YB-Aresden"); sNpcType = 1;   iProbSA = 15; iKindSA = 1; break;
                    case 20: strcpy(cNpcName, "YB-Elvine");	 sNpcType = 1;   iProbSA = 15; iKindSA = 1; break;
                    case 21: strcpy(cNpcName, "Gagoyle");	 sNpcType = 52;  iProbSA = 20; iKindSA = 8; break;
                    case 22: strcpy(cNpcName, "Beholder");	 sNpcType = 53;  iProbSA = 20; iKindSA = 5; break;
                    case 23: strcpy(cNpcName, "Dark-Elf");	 sNpcType = 54;  iProbSA = 20; iKindSA = 3; break;

                    case 24: strcpy(cNpcName, "Rabbit");	 sNpcType = 55;  iProbSA = 5; iKindSA = 1; break;
                    case 25: strcpy(cNpcName, "Cat");		 sNpcType = 56;  iProbSA = 10; iKindSA = 2; break;
                    case 26: strcpy(cNpcName, "Giant-Frog"); sNpcType = 57;  iProbSA = 10; iKindSA = 2; break;

                    case 27: strcpy(cNpcName, "Mountain-Giant");  sNpcType = 58; 	iProbSA = 25; iKindSA = 1; break;
                    case 28: strcpy(cNpcName, "Ettin");			  sNpcType = 59;	iProbSA = 20; iKindSA = 8; break;
                    case 29: strcpy(cNpcName, "Cannibal-Plant");  sNpcType = 60; 	iProbSA = 20; iKindSA = 5; break;
                        //v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã 
                    case 30: strcpy(cNpcName, "Rudolph");    sNpcType = 61; 	iProbSA = 20; iKindSA = 5; break;
                    case 31: strcpy(cNpcName, "Ice-Golem");  sNpcType = 65;    iProbSA = 35; iKindSA = 8; break;
                    case 32: strcpy(cNpcName, "DireBoar");  sNpcType = 62; 	iProbSA = 20; iKindSA = 5; break;
                    case 33: strcpy(cNpcName, "Frost");		 sNpcType = 63;    iProbSA = 30; iKindSA = 8; break;
                    default: strcpy(cNpcName, "Orc");		      sNpcType = 14; 	iProbSA = 15; iKindSA = 1; break;
                }


                cSA = 0;
                if (iFirmSAType == 0)
                {
                    if (iDice(1, 100) <= iProbSA) cSA = _cGetSpecialAbility(iKindSA);
                }
                else cSA = (char)iFirmSAType;

                if ((bMaster = bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, bFirmBerserk, true)) == false)
                {

                    m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
                }
                else
                {

                }
            }


            switch (iResult)
            {
                case 1:	 iTotalMob = iDice(1, 5) - 1; break;  // Slime 
                case 2:	 iTotalMob = iDice(1, 5) - 1; break; // Giant-Ant
                case 3:	 iTotalMob = iDice(1, 5) - 1; break; // Orc
                case 4:	 iTotalMob = iDice(1, 3) - 1; break; // Zombie
                case 5:	 iTotalMob = iDice(1, 3) - 1; break;
                case 6:  iTotalMob = iDice(1, 3) - 1; break; // Skeleton
                case 7:  iTotalMob = iDice(1, 3) - 1; break; // Scorpion
                case 8:  iTotalMob = iDice(1, 2) - 1; break; // Stone-Golem
                case 9:  iTotalMob = iDice(1, 2) - 1; break;
                case 10: iTotalMob = iDice(1, 5) - 1; break; // Amphis
                case 11: iTotalMob = iDice(1, 3) - 1; break; // Clay-Golem
                case 12: iTotalMob = iDice(1, 5) - 1; break; // Troll
                case 13: iTotalMob = iDice(1, 3) - 1; break; // Orge
                case 14: iTotalMob = iDice(1, 3) - 1; break; // Rabbit
                case 15: iTotalMob = iDice(1, 3) - 1; break; // Cat
                case 16: iTotalMob = iDice(1, 2) - 1; break; // Giant-Frog
                case 17: iTotalMob = iDice(1, 2) - 1; break; // Mountain-Giant
                case 18: iTotalMob = iDice(1, 5) - 1; break; // Ettin
                case 21: iTotalMob = iDice(1, 2) - 1; break; // Cannibal-Plant
                case 22: iTotalMob = iDice(1, 2) - 1; break;
                case 23: iTotalMob = iDice(1, 5) - 1; break;

                case 24: iTotalMob = iDice(1, 2) - 1; break;
                case 25: iTotalMob = iDice(1, 2) - 1; break;
                case 26: iTotalMob = iDice(1, 4) - 1; break;
                    // 2002-09-12 #1
                case 27: iTotalMob = iDice(1, 2) - 1; break;
                case 28: iTotalMob = iDice(1, 3) - 1; break;
                case 29: iTotalMob = iDice(1, 3) - 1; break;
                    //v2.19 2002-12-11 ·çµ¹ÇÁ Ãß°¡ °ü·Ã 
                case 30: iTotalMob = iDice(1, 3) - 1; break;

                case 31: iTotalMob = iDice(1, 5) - 1; break;
                case 32: iTotalMob = iDice(1, 3) - 1; break;
                case 33: iTotalMob = iDice(1, 3) - 1; break;

                default: iTotalMob = 0; break;
            }

            if (bMaster == false) iTotalMob = 0;


            //if ((iTotalMob >= 2) && (iDice(1,2) == 1)) iTotalMob = 0;


            if (iTotalMob >= 2)
            {
                switch (iResult)
                {
                    case 1:   // Slime 
                    case 2:  // Giant-Ant
                    case 3:  // Orc
                    case 4:  // Zombie
                    case 5:  // Skeleton
                    case 7:  // Scorpion
                    case 8:  // Stone-Golem
                    case 10: // Amphis
                    case 11: // Clay-Golem
                    case 12: // Troll
                    case 13: // Orge
                    case 24: // Rabbit
                    case 25: // Cat
                    case 26: // Giant-Frog
                    case 27: // Mountain-Giant
                    case 28: // Ettin
                    case 29: // Cannibal-Plant
                        //v2.19 2002-12-11 ·çµ¹ÇÁ Ãß°¡ °ü·Ã 
                    case 30:
                    case 32: // DireBoar
                    case 33: // Frost
                        if (iDice(1, 5) != 1) iTotalMob = 0;
                        break;

                    case 9:  // Cyclops
                    case 6:  // Orc-Mage
                    case 14: // Hellbound
                    case 15: // Liche
                    case 16: // Demon
                    case 17: // Unicorn
                    case 18: // WereWolf
                    case 31: // Ice-Golem
                        if (iDice(1, 5) == 1) iTotalMob = 0;  // 75% È®·ü·Î ¹«¸®¸¦ Áþ´Â´Ù.
                        break;

                    default:
                        break;
                }
            }


            // 2002-7-4
            if (bIsSpecialEvent == true)
            {
                switch (m_cSpecialEventType)
                {
                    case 1:

                        if ((iResult != 15) && (iResult != 16) && (iResult != 21))
                            iTotalMob = m_pMapList[i]->sMobEventAmount;


                        for (j = 1; j < DEF_MAXCLIENTS; j++)
                            if ((m_pClientList[j] != 0) && (m_pClientList[j]->m_bIsInitComplete == true) && (m_pClientList[j]->m_cMapIndex == i))
                                SendNotifyMsg(NULL, j, DEF_NOTIFY_MONSTEREVENT_POSITION, pX, pY, sNpcType, 0);

                        break;


                    case 2:




                        if ((memcmp(m_pMapList[i]->m_cName, "aresden", 7) == 0) ||
                            (memcmp(m_pMapList[i]->m_cName, "elvine", 6) == 0) ||
                            (memcmp(m_pMapList[i]->m_cName, "elvfarm", 7) == 0) ||
                            (memcmp(m_pMapList[i]->m_cName, "arefarm", 7) == 0))
                        {

                            iTotalMob = 0;
                        }
                        break;
                }
                m_bIsSpecialEventTime = false;
            }

            for (j = 0; j < iTotalMob; j++)
            {
                iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
                if (iNamingValue != -1)
                {

                    memset(cName_Slave, 0, sizeof(cName_Slave));
                    wsprintf(cName_Slave, "XX%d", iNamingValue);
                    cName_Slave[0] = '_';
                    cName_Slave[1] = i + 65;


                    cSA = 0;
                    if (iFirmSAType == 0)
                    {
                        if (iDice(1, 100) <= iProbSA) cSA = _cGetSpecialAbility(iKindSA);
                    }
                    else cSA = (char)iFirmSAType;

                    if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, bFirmBerserk) == false)
                    {

                        m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
                    }
                    else
                    {

                        bSetNpcFollowMode(cName_Slave, cName_Master, DEF_OWNERTYPE_NPC);
                    }
                }
            }
        }
        // random mob generator	^

        // spot mob generator 
        if ((m_pMapList[i] != 0) && ((m_pMapList[i]->m_iMaximumObject) > m_pMapList[i]->m_iTotalActiveObject))
        {
            for (j = 1; j < DEF_MAXSPOTMOBGENERATOR; j++)
                if ((iDice(1, 3) == 2) && (m_pMapList[i]->m_stSpotMobGenerator[j].bDefined == true) &&
                    (m_pMapList[i]->m_stSpotMobGenerator[j].iMaxMobs > m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs))
                {

                    iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
                    if (iNamingValue != -1)
                    {


                        memset(cNpcName, 0, sizeof(cNpcName));
                        switch (m_pMapList[i]->m_stSpotMobGenerator[j].iMobType)
                        {
                            case 10:  strcpy(cNpcName, "Slime");        iProbSA = 5;  iKindSA = 1; break;
                            case 16:  strcpy(cNpcName, "Giant-Ant");    iProbSA = 10; iKindSA = 2; break;
                            case 14:  strcpy(cNpcName, "Orc");          iProbSA = 15; iKindSA = 1; break;
                            case 18:  strcpy(cNpcName, "Zombie");       iProbSA = 15; iKindSA = 3; break;
                            case 11:  strcpy(cNpcName, "Skeleton");     iProbSA = 35; iKindSA = 8; break;
                            case 6:   strcpy(cNpcName, "Orc-Mage");     iProbSA = 30; iKindSA = 7; break;
                            case 17:  strcpy(cNpcName, "Scorpion");     iProbSA = 15; iKindSA = 3; break;
                            case 12:  strcpy(cNpcName, "Stone-Golem");  iProbSA = 25; iKindSA = 5; break;
                            case 13:  strcpy(cNpcName, "Cyclops");      iProbSA = 35; iKindSA = 8; break;
                            case 22:  strcpy(cNpcName, "Amphis");       iProbSA = 20; iKindSA = 3; break;
                            case 23:  strcpy(cNpcName, "Clay-Golem");   iProbSA = 20; iKindSA = 5; break;
                            case 24:  strcpy(cNpcName, "Guard-Aresden"); iProbSA = 20; iKindSA = 1; break;
                            case 25:  strcpy(cNpcName, "Guard-Elvine"); iProbSA = 20; iKindSA = 1; break;
                            case 26:  strcpy(cNpcName, "Guard-Neutral"); iProbSA = 20; iKindSA = 1; break;
                            case 27:  strcpy(cNpcName, "Hellbound");    iProbSA = 20; iKindSA = 1; break;
                            case 29:  strcpy(cNpcName, "Orge");         iProbSA = 20; iKindSA = 1; break;
                            case 30:  strcpy(cNpcName, "Liche");        iProbSA = 30; iKindSA = 8; break;
                            case 31:  strcpy(cNpcName, "Demon");        iProbSA = 20; iKindSA = 8; break;
                            case 32:  strcpy(cNpcName, "Unicorn");      iProbSA = 35; iKindSA = 7; break;
                            case 33:  strcpy(cNpcName, "WereWolf");     iProbSA = 25; iKindSA = 1; break;
                            case 34:  strcpy(cNpcName, "Dummy");        iProbSA = 5;  iKindSA = 1; break;

                            case 35:  strcpy(cNpcName, "Attack-Dummy"); iProbSA = 5;  iKindSA = 1; break;
                            case 48:  strcpy(cNpcName, "Stalker");      iProbSA = 20; iKindSA = 3; break;
                            case 49:
                                if (iDice(1, 40 * 60 * 60) == 13)
                                {
                                    strcpy(cNpcName, "Hellclaw");
                                    iProbSA = 20;
                                    iKindSA = 8;
                                }
                                break;

                            case 50:
                                if (iDice(1, 2 * 60 * 30) == 135)
                                {
                                    strcpy(cNpcName, "Tigerworm");
                                    iProbSA = 20;
                                    iKindSA = 8;
                                }
                                else
                                {
                                    strcpy(cNpcName, "Stalker");
                                    iProbSA = 15;
                                    iKindSA = 1;
                                }
                                break;

                            case 51:  strcpy(cNpcName, "Hellclaw");      iProbSA = 20; iKindSA = 8; break;
                            case 52:  strcpy(cNpcName, "Tigerworm");     iProbSA = 20; iKindSA = 8; break;
                            case 53:  strcpy(cNpcName, "Dark-Elf");      iProbSA = 20; iKindSA = 8; break;
                            case 54:  strcpy(cNpcName, "Beholder");      iProbSA = 20; iKindSA = 8; break;
                            case 55:  strcpy(cNpcName, "Gagoyle");       iProbSA = 20; iKindSA = 8; break;
                            case 56:
                                if (iDice(1, 2 * 60 * 30) == 13)
                                {
                                    strcpy(cNpcName, "Hellclaw");
                                    iProbSA = 20;
                                    iKindSA = 8;
                                }
                                break;


                            case 57:   strcpy(cNpcName, "Giant-Frog");		iProbSA = 10; iKindSA = 2; break;


                            case 58:   strcpy(cNpcName, "Mountain-Giant");	iProbSA = 25; iKindSA = 1; break;
                            case 59:   strcpy(cNpcName, "Ettin");			iProbSA = 20; iKindSA = 8; break;
                            case 60:   strcpy(cNpcName, "Cannibal_Plant");	iProbSA = 20; iKindSA = 5; break;
                                //v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã
                            case 61:   strcpy(cNpcName, "Rudolph");			iProbSA = 20; iKindSA = 1; break;
                                //v2.20 2002-12-9 ¸ó½ºÅÍ Ãß°¡ °ü·Ã (npc.cfg¶û ¹Ø¿¡ ¼ýÀÚ´Â °°Áö°¡ ¾ÊÀ½.. -_-;;)
                            case 62:   strcpy(cNpcName, "DireBoar");			iProbSA = 20; iKindSA = 1; break;
                            case 63:   strcpy(cNpcName, "Frost");			iProbSA = 20; iKindSA = 8; break;
                            case 65:   strcpy(cNpcName, "Ice-Golem");		iProbSA = 20; iKindSA = 8; break;
                            case 66:   strcpy(cNpcName, "Wyvern");			iProbSA = 20; iKindSA = 1; break;
                            case 5:   strcpy(cNpcName, "Rabbit");			iProbSA = 20; iKindSA = 1; break;
                                //v2.20 2002-12-20 µµ¿ì¹Ì NPC
                            case 67:   strcpy(cNpcName, "McGaffin");			iProbSA = 20; iKindSA = 1; break;
                            case 68:   strcpy(cNpcName, "Perry");			iProbSA = 20; iKindSA = 1; break;
                            case 69:   strcpy(cNpcName, "Devlin");			iProbSA = 20; iKindSA = 1; break;
                            default:
                                strcpy(cNpcName, "Orc");
                                iProbSA = 15;
                                iKindSA = 1;
                                break;
                        }


                        bFirmBerserk = false;
                        if ((m_pMapList[i]->m_cRandomMobGeneratorLevel >= 9) && (iMapLevel >= 4) && (iDice(1, 3) == 1)) bFirmBerserk = true;

                        memset(cName_Master, 0, sizeof(cName_Master));
                        wsprintf(cName_Master, "XX%d", iNamingValue);
                        cName_Master[0] = '_';
                        cName_Master[1] = i + 65;


                        cSA = 0;
                        if ((m_pMapList[i]->m_stSpotMobGenerator[j].iMobType != 34) && (iDice(1, 100) <= iProbSA))
                        {
                            cSA = _cGetSpecialAbility(iKindSA);
                        }

                        switch (m_pMapList[i]->m_stSpotMobGenerator[j].cType)
                        {
                            case 1:
                                // RANDOMAREA
                                if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMAREA, &pX, &pY, cWaypoint, &m_pMapList[i]->m_stSpotMobGenerator[j].rcRect, j, -1, false, false, bFirmBerserk) == false)
                                {
                                    //log->info("SpotMobGenerator(RANDOMAREA) Fail! Cannot locate mob.");	

                                    m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
                                }
                                else
                                {
                                    //wsprintf(G_cTxt, "SpotMobGenerator(RANDOMAREA) - Map:%d TotalActiveObject:%d Mob:%s ", i, m_pMapList[i]->m_iTotalActiveObject, cNpcName );
                                    //log->info(G_cTxt);
                                    m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
                                }
                                break;

                            case 2:
                                // RANDOMWAYPOINT
                                if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMWAYPOINT, 0, 0, m_pMapList[i]->m_stSpotMobGenerator[j].cWaypoint, 0, j, -1, false, false, bFirmBerserk) == false)
                                {
                                    //log->info("SpotMobGenerator(RANDOMWAYPOINT) Fail! Cannot locate mob.");	

                                    m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
                                }
                                else
                                {
                                    //wsprintf(G_cTxt, "SpotMobGenerator(RANDOMWAYPOINT) - Map:%d TotalActiveObject:%d Mob:%s (%d %d)", i, m_pMapList[i]->m_iTotalActiveObject, cNpcName, pX, pY);
                                    //log->info(G_cTxt);
                                    m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
                                }
                                break;
                        }
                    }
                }
        }
        // spot mob generator ^
    }
}


void CGame::CalcNextWayPointDestination(int iNpcH)
{
    short sRange, sX, sY;
    int i, j, iMapIndex;
    bool bFlag;

    switch (m_pNpcList[iNpcH]->m_cMoveType)
    {
        case DEF_MOVETYPE_GUARD:

            break;

        case DEF_MOVETYPE_SEQWAYPOINT:

            m_pNpcList[iNpcH]->m_cCurWaypoint++;
            if (m_pNpcList[iNpcH]->m_cCurWaypoint >= m_pNpcList[iNpcH]->m_cTotalWaypoint)
                m_pNpcList[iNpcH]->m_cCurWaypoint = 1;
            m_pNpcList[iNpcH]->m_dX = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].x);
            m_pNpcList[iNpcH]->m_dY = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].y);
            break;

        case DEF_MOVETYPE_RANDOMWAYPOINT:

            m_pNpcList[iNpcH]->m_cCurWaypoint = (short)((rand() % (m_pNpcList[iNpcH]->m_cTotalWaypoint - 1)) + 1);
            m_pNpcList[iNpcH]->m_dX = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].x);
            m_pNpcList[iNpcH]->m_dY = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].y);
            break;

        case DEF_MOVETYPE_RANDOMAREA:

            sRange = (short)(m_pNpcList[iNpcH]->m_rcRandomArea.right - m_pNpcList[iNpcH]->m_rcRandomArea.left);
            m_pNpcList[iNpcH]->m_dX = (short)((rand() % sRange) + m_pNpcList[iNpcH]->m_rcRandomArea.left);
            sRange = (short)(m_pNpcList[iNpcH]->m_rcRandomArea.bottom - m_pNpcList[iNpcH]->m_rcRandomArea.top);
            m_pNpcList[iNpcH]->m_dY = (short)((rand() % sRange) + m_pNpcList[iNpcH]->m_rcRandomArea.top);
            break;

        case DEF_MOVETYPE_RANDOM:

            //m_pNpcList[iNpcH]->m_dX = (rand() % (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_sSizeX - 50)) + 15;
            //m_pNpcList[iNpcH]->m_dY = (rand() % (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_sSizeY - 50)) + 15;
            iMapIndex = m_pNpcList[iNpcH]->m_cMapIndex;

            for (i = 0; i <= 30; i++)
            {

                sX = (rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15;
                sY = (rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15;

                bFlag = true;
                for (j = 0; j < DEF_MAXMGAR; j++)
                    if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left != -1)
                    {

                        if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left) &&
                            (sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].right) &&
                            (sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].top) &&
                            (sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].bottom))
                        {

                            bFlag = false;
                        }
                    }
                if (bFlag == true) goto CNW_GET_VALIDLOC_SUCCESS;
            }
            // Fail! 
            m_pNpcList[iNpcH]->m_tmp_iError = 0;
            return;

            CNW_GET_VALIDLOC_SUCCESS:;
            m_pNpcList[iNpcH]->m_dX = sX;
            m_pNpcList[iNpcH]->m_dY = sY;
            break;
    }

    m_pNpcList[iNpcH]->m_tmp_iError = 0; // @@@ !!! @@@
}

void CGame::DeleteNpc(int iNpcH)
{
    int  i, iNamingValue, iItemID = 0;
    char cTmp[21], cItemName[21];
    CItem * pItem;
    uint32_t dwCount, dwTime;

    if (m_pNpcList[iNpcH] == 0) return;

    dwTime = timeGetTime();

    //Á×¾îÀÖ´ø À§Ä¡¿¡¼­ »èÁ¦ 
    SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(11, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

    memset(cTmp, 0, sizeof(cTmp));
    strcpy(cTmp, (char *)(m_pNpcList[iNpcH]->m_cName + 2));

    iNamingValue = atoi(cTmp);


    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalActiveObject--;

    // Spot-mob-generatorÁ¤º¸ 
    if (m_pNpcList[iNpcH]->m_iSpotMobIndex != 0)
        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stSpotMobGenerator[m_pNpcList[iNpcH]->m_iSpotMobIndex].iCurMobs--;

    RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);


    switch (m_pNpcList[iNpcH]->m_sType)
    {
        case 36:
        case 37:
        case 38:
        case 39:
        case 42:
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCrusadeStructureInfo(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);


            for (i = 0; i < DEF_MAXGUILDS; i++)
                if (m_pGuildTeleportLoc[i].m_iV1 == m_pNpcList[iNpcH]->m_iGuildGUID)
                {
                    m_pGuildTeleportLoc[i].m_dwTime = dwTime;
                    m_pGuildTeleportLoc[i].m_iV2--;
                    if (m_pGuildTeleportLoc[i].m_iV2 < 0) m_pGuildTeleportLoc[i].m_iV2 = 0;
                    break;
                }
            break;


        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 51:
            for (i = 0; i < DEF_MAXGUILDS; i++)
                if (m_pGuildTeleportLoc[i].m_iV1 == m_pNpcList[iNpcH]->m_iGuildGUID)
                {
                    m_pGuildTeleportLoc[i].m_iNumSummonNpc--;
                    if (m_pGuildTeleportLoc[i].m_iNumSummonNpc < 0) m_pGuildTeleportLoc[i].m_iNumSummonNpc = 0;
                    break;
                }
            break;

        case 64:	//v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum();
            break;
    }

    // DelayEvent¸¦ »èÁ¦ 
    bRemoveFromDelayEventList(iNpcH, DEF_OWNERTYPE_NPC, 0);

    int iItemprobability = 65;

    if ((m_iMiddlelandMapIndex == m_pNpcList[iNpcH]->m_cMapIndex) && (m_iMiddlelandMapIndex != -1))
    {
        iItemprobability = 55;
    }

    if (m_pNpcList[iNpcH]->m_bIsSummoned == false)
    {

        pItem = new CItem;
        memset(cItemName, 0, sizeof(cItemName));

        switch (m_pNpcList[iNpcH]->m_sType)
        {
            case 10:
                if (iDice(1, 25) == 1) iItemID = 220;
                break;
            case 16:
                switch (iDice(1, 3))
                {
                    case 1: if (iDice(1, 9) == 1)  iItemID = 192; break;
                    case 2: if (iDice(1, 10) == 1) iItemID = 193; break;
                    case 3: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;
            case 22:
                switch (iDice(1, 5))
                {
                    case 1: if (iDice(1, 15) == 1) iItemID = 188; break;
                    case 2: if (iDice(1, 16) == 1) iItemID = 189; break; // "¹ì²®Áú"	// 1/24
                    case 3: if (iDice(1, 16) == 1) iItemID = 190; break;
                    case 4: if (iDice(1, 17) == 1) iItemID = 191; break;
                    case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;
            case 14:
                switch (iDice(1, 4))
                {
                    case 1: if (iDice(1, 11) == 1) iItemID = 206; break;
                    case 2: if (iDice(1, 20) == 1) iItemID = 207; break;
                    case 3: if (iDice(1, 21) == 1) iItemID = 208; break;
                    case 4: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;
            case 17:
                switch (iDice(1, 5))
                {

                    case 1: if (iDice(1, 50) == 1) iItemID = 215; break;
                    case 2: if (iDice(1, 20) == 1) iItemID = 216; break;
                    case 3: if (iDice(1, 50) == 1) iItemID = 217; break;
                    case 4: if (iDice(1, 40) == 1) iItemID = 218; break;
                    case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;

            case 18: // Zombie
                switch (iDice(1, 1))
                {
                    case 1: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                }
                break;

            case 11:
                switch (iDice(1, 2))
                {
                    case 1: if (iDice(1, 20) == 1) iItemID = 219; break;
                    case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                }
                break;
            case 23:
                switch (iDice(1, 2))
                {
                    case 1: if (iDice(1, 30) == 1) iItemID = 205; break;
                    case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                }
                break;
            case 12:
                switch (iDice(1, 2))
                {
                    case 1: if (iDice(1, 30) == 1) iItemID = 221; break;
                    case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                }
                break;
            case 27:
                switch (iDice(1, 7))
                {
                    case 1: if (iDice(1, 40) == 1) iItemID = 199; break;
                    case 2: if (iDice(1, 38) == 1) iItemID = 200; break;
                    case 3: if (iDice(1, 38) == 1) iItemID = 201; break;
                    case 4: if (iDice(1, 36) == 1) iItemID = 202; break;
                    case 5: if (iDice(1, 36) == 1) iItemID = 203; break;
                    case 6: if (iDice(1, 50) == 1) iItemID = 204; break;
                    case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;
            case 13:
                switch (iDice(1, 6))
                {
                    case 1: if (iDice(1, 36) == 1) iItemID = 194; break;
                    case 2: if (iDice(1, 40) == 1) iItemID = 195; break;
                    case 3: if (iDice(1, 30) == 1) iItemID = 196; break;
                    case 4: if (iDice(1, 22) == 1) iItemID = 197; break;
                        // 2002-09-11 #2

                    case 5: if (iDice(1, 40) == 1) iItemID = 198; break;
                    case 6: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;
            case 28:
                switch (iDice(1, 5))
                {
                    case 1: if (iDice(1, 35) == 1) iItemID = 222; break;
                    case 2: if (iDice(1, 23) == 1) iItemID = 223; break;
                    case 3: if (iDice(1, 25) == 1) iItemID = 224; break;
                    case 4: if (iDice(1, 27) == 1) iItemID = 225; break;
                    case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;
            case 29:
                switch (iDice(1, 7))
                {
                    case 1: if (iDice(1, 20) == 1) iItemID = 209; break;
                    case 2: if (iDice(1, 22) == 1) iItemID = 210; break;
                    case 3: if (iDice(1, 25) == 1) iItemID = 211; break;
                    case 4: if (iDice(1, 25) == 1) iItemID = 212; break;
                    case 5: if (iDice(1, 28) == 1) iItemID = 213; break;
                    case 6: if (iDice(1, 28) == 1) iItemID = 214; break;
                    case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;
            case 30:
                // ¸®Ä¡
                switch (iDice(1, 1))
                {
                    case 1: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                }
                break;

            case 31:

                switch (iDice(1, 5))
                {
                    case 1: if (iDice(1, 400) == 123) iItemID = 541; break;
                    case 2:	if (iDice(1, 100) == 123) iItemID = 542; break;
                    case 3:	if (iDice(1, 200) == 123) iItemID = 543; break;
                    case 4:	if (iDice(1, 300) == 123) iItemID = 540; break;
                    case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                }
                break;

            case 32:

                switch (iDice(1, 5))
                {
                    case 1: if (iDice(1, 3000) == 396) iItemID = 544; break;
                    case 2: if (iDice(1, 500) == 3)    iItemID = 545; break;
                    case 3: if (iDice(1, 100) == 3)    iItemID = 546; break;
                    case 4: if (iDice(1, 200) == 3)    iItemID = 547; break;
                    case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                }
                break;

            case 33:

                switch (iDice(1, 8))
                {
                    case 1: if (iDice(1, 30) == 3) iItemID = 551; break;
                        // 2002-09-11 #2

                    case 2: if (iDice(1, 28) == 3) iItemID = 548; break;
                    case 3: if (iDice(1, 25) == 3) iItemID = 550; break;
                    case 4: if (iDice(1, 35) == 3) iItemID = 553; break;
                    case 5: if (iDice(1, 28) == 3) iItemID = 552; break;
                    case 6: if (iDice(1, 28) == 3) iItemID = 554; break;
                    case 7: if (iDice(1, 38) == 3) iItemID = 549; break;
                    case 8: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability);
                    default: break;
                }
                break;

            case 48:
            case 49:
            case 50:
            case 52:
            case 53:
            case 54:
            case 55:
            case 56:
            case 57:
            case 61:
                bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType, iItemprobability); break;

                break;
        }


        dwCount = 1;
        if (iItemID == 0)
        {
            switch (m_pNpcList[iNpcH]->m_sType)
            {
                case 30:
                case 31:
                case 32:
                    iItemID = 90;
                    pItem->m_dwCount = (iDice(1, (m_pNpcList[iNpcH]->m_iGoldDiceMax - m_pNpcList[iNpcH]->m_iGoldDiceMin))) + m_pNpcList[iNpcH]->m_iGoldDiceMin;
                    dwCount *= 0.66f;
                    break;
            }
        }

        if (_bInitItemAttr(pItem, iItemID) == false)
        {
            delete pItem;
            pItem = 0;
        }
        else
        {

            pItem->m_dwCount = dwCount;


            pItem->m_sTouchEffectType = DEF_ITET_ID;
            pItem->m_sTouchEffectValue1 = iDice(1, 100000);
            pItem->m_sTouchEffectValue2 = iDice(1, 100000);
#ifdef DEF_LOGTIME
            pItem->m_sTouchEffectValue3 = timeGetTime();
#else 

            SYSTEMTIME SysTime;
            char cTemp[256];
            GetLocalTime(&SysTime);
            memset(cTemp, 0, sizeof(cTemp));
            //			wsprintf(cTemp, "%d%02d%02d",  (short)SysTime.wMonth, (short)SysTime.wDay,(short) SysTime.wHour);
            wsprintf(cTemp, "%d%02d%", (short)SysTime.wMonth, (short)SysTime.wDay);

            pItem->m_sTouchEffectValue3 = atoi(cTemp);
#endif


            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem);


            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
                m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
                pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color

            // ·Î±× ³²±ä´Ù.
            _bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, m_pNpcList[iNpcH]->m_cNpcName, pItem);
        }
    }

    delete m_pNpcList[iNpcH];
    m_pNpcList[iNpcH] = 0;
}


int CGame::_iGetArrowItemIndex(int iClientH)
{
    int i;

    if (m_pClientList[iClientH] == 0) return -1;

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != 0)
        {


            if ((m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_ARROW) &&
                (m_pClientList[iClientH]->m_pItemList[i]->m_dwCount > 0))
                return i;
        }

    return -1;
}

bool CGame::bCheckResistingMagicSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio)
{
    double dTmp1, dTmp2, dTmp3;
    int    iTargetMagicResistRatio = 0, iDestHitRatio, iResult;
    char   cTargetDir, cProtect = 0;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == 0) return false;

            if (m_pClientList[sTargetH]->m_iAdminUserLevel > 0) return true;

            cTargetDir = m_pClientList[sTargetH]->m_cDir;
            iTargetMagicResistRatio = m_pClientList[sTargetH]->m_cSkillMastery[3] + m_pClientList[sTargetH]->m_iAddMR;

            if (m_pClientList[sTargetH]->m_iMag > 50)
                iTargetMagicResistRatio += (m_pClientList[sTargetH]->m_iMag - 50);

            iTargetMagicResistRatio += m_pClientList[sTargetH]->m_iAddResistMagic;
            cProtect = m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT];
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == 0) return false;
            cTargetDir = m_pNpcList[sTargetH]->m_cDir;
            iTargetMagicResistRatio = m_pNpcList[sTargetH]->m_cResistMagic;
            cProtect = m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT];
            break;
    }

    // Absolute Magic Protection
    if (cProtect == 5) return true;


    if ((iHitRatio < 1000) && (cProtect == 2)) return true;

    if (iHitRatio >= 10000) iHitRatio -= 10000;

    if (iTargetMagicResistRatio < 1) iTargetMagicResistRatio = 1;


    dTmp1 = (double)(iHitRatio);
    dTmp2 = (double)(iTargetMagicResistRatio);

    dTmp3 = (dTmp1 / dTmp2) * 50.0f;
    iDestHitRatio = (int)(dTmp3);


    if (iDestHitRatio < DEF_MINIMUMHITRATIO) iDestHitRatio = DEF_MINIMUMHITRATIO;

    if (iDestHitRatio > DEF_MAXIMUMHITRATIO) iDestHitRatio = DEF_MAXIMUMHITRATIO;

    if (iDestHitRatio >= 100) return false;

    iResult = iDice(1, 100);
    if (iResult <= iDestHitRatio) return false;


    if (cTargetType == DEF_OWNERTYPE_PLAYER)
        CalculateSSN_SkillIndex(sTargetH, 3, 1);

    return true;
}


bool CGame::bCheckResistingIceSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio)
{

    int    iTargetIceResistRatio = 0, iResult;
    char   cTargetDir;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == 0) return false;

            if (m_pClientList[sTargetH]->m_iAdminUserLevel > 0) return true;

            //		if ( m_pClientList[sTargetH]->m_pItemList[sItemIndex]->m_sIDnum == 770 
            //			|| m_pClientList[sTargetH]->m_pItemList[sItemIndex]->m_sIDnum == 771 )
            //			return true;

            iTargetIceResistRatio = m_pClientList[sTargetH]->m_iAddAbsWater * 2;

            if (m_pClientList[sTargetH]->m_dwWarmEffectTime == 0)
            {
            }
            else if ((timeGetTime() - m_pClientList[sTargetH]->m_dwWarmEffectTime) < 1000 * 30) return true;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == 0) return false;
            if (m_pNpcList[sTargetH]->m_cAttribute == DEF_ATTRIBUTE_WATER) return true;
            iTargetIceResistRatio = (m_pNpcList[sTargetH]->m_cResistMagic) - (m_pNpcList[sTargetH]->m_cResistMagic / 3);
            break;
    }

    if (iTargetIceResistRatio < 1) iTargetIceResistRatio = 1;

    iResult = iDice(1, 100);
    if (iResult <= iTargetIceResistRatio) return true;

    return false;
}


bool CGame::bSetItemToBankItem(int iClientH, CItem * pItem)
{
    int i, iRet;
    uint32_t * dwp;
    uint16_t * wp;
    char * cp;
    short * sp;
    char cData[100];


    if (m_pClientList[iClientH] == 0) return false;
    if (pItem == 0) return false;


    if (m_pClientList[iClientH]->m_bIsOnWarehouse == false) return false;


    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] == 0)
        {




            m_pClientList[iClientH]->m_pItemInBankList[i] = pItem;

            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_ITEMTOBANK;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

            *cp = i;
            cp++;


            *cp = 1;
            cp++;

            memcpy(cp, pItem->m_cName, 20);
            cp += 20;

            dwp = (uint32_t *)cp;
            *dwp = pItem->m_dwCount;
            cp += 4;

            *cp = pItem->m_cItemType;
            cp++;

            *cp = pItem->m_cEquipPos;
            cp++;

            *cp = (char)0;
            cp++;

            sp = (short *)cp;
            *sp = pItem->m_sLevelLimit;
            cp += 2;

            *cp = pItem->m_cGenderLimit;
            cp++;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wCurLifeSpan;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wWeight;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSprite;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSpriteFrame;
            cp += 2;

            *cp = pItem->m_cItemColor;
            cp++;

            // v1.432
            sp = (short *)cp;
            *sp = pItem->m_sItemEffectValue2;
            cp += 2;

            // v1.42
            dwp = (uint32_t *)cp;
            *dwp = pItem->m_dwAttribute;
            cp += 4;


            *cp = (char)pItem->m_sItemSpecEffectValue2;
            cp++;


            iRet = m_pClientList[iClientH]->iSendMsg(cData, 56);

#ifdef DEF_TAIWANLOG
            _bItemLog(DEF_ITEMLOG_DEPOSIT, iClientH, (int)-1, pItem);
#endif


            m_pClientList[iClientH]->m_bIsBankModified = true;

            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:

                    // DeleteClient(iClientH, true, true);
                    return true;
            }

            return true;
        }


    return false;
}

bool CGame::bCheckTotalSkillMasteryPoints(int iClientH, int iSkill)
{
    int i;
    int iRemainPoint, iTotalPoints, iWeaponIndex, iDownSkillSSN, iDownPoint;
    short sDownSkillIndex;

    if (m_pClientList[iClientH] == 0) return false;

    iTotalPoints = 0;
    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        iTotalPoints += m_pClientList[iClientH]->m_cSkillMastery[i];

    iRemainPoint = iTotalPoints - DEF_MAXSKILLPOINTS;

    if (iRemainPoint > 0)
    {

        while (iRemainPoint > 0)
        {

            sDownSkillIndex = -1; // v1.4
            if (m_pClientList[iClientH]->m_iDownSkillIndex != -1)
            {

                switch (m_pClientList[iClientH]->m_iDownSkillIndex)
                {
                    case 3:

                        /* ÀÌ ½ºÅ³µµ 0À¸·Î µÈ´Ù.
                        case 4:
                        case 5:
                        case 7:

                        if (m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_iDownSkillIndex] > 20) {
                        sDownSkillIndex = m_pClientList[iClientH]->m_iDownSkillIndex;
                        }
                        else {

                        iDownSkillSSN = 99999999;
                        for (i = 0; i < DEF_MAXSKILLTYPE; i++)
                        if ((m_pClientList[iClientH]->m_cSkillMastery[i] >= 21) && (i != iSkill) &&
                        (m_pClientList[iClientH]->m_iSkillSSN[i] <= iDownSkillSSN)) {

                        iDownSkillSSN = m_pClientList[iClientH]->m_iSkillSSN[i];
                        sDownSkillIndex = i;
                        }
                        }
                        break;
                        */

                    default:

                        if (m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_iDownSkillIndex] > 0)
                        {
                            sDownSkillIndex = m_pClientList[iClientH]->m_iDownSkillIndex;
                        }
                        else
                        {


                            iDownSkillSSN = 99999999;
                            for (i = 0; i < DEF_MAXSKILLTYPE; i++)
                                if ((m_pClientList[iClientH]->m_cSkillMastery[i] >= 21) && (i != iSkill) &&
                                    (m_pClientList[iClientH]->m_iSkillSSN[i] <= iDownSkillSSN))
                                {

                                    iDownSkillSSN = m_pClientList[iClientH]->m_iSkillSSN[i];
                                    sDownSkillIndex = i;
                                }
                        }
                        break;
                }
            }


            if (sDownSkillIndex != -1)
            {

                if (m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] <= 20) // v1.4
                    iDownPoint = m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex];
                else iDownPoint = 1;

                m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] -= iDownPoint; // v1.4
                m_pClientList[iClientH]->m_iSkillSSN[sDownSkillIndex] = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] + 1] - 1;
                iRemainPoint -= iDownPoint; // v1.4


                if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
                {

                    iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                    if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sDownSkillIndex)
                    {

                        m_pClientList[iClientH]->m_iHitRatio -= iDownPoint; // v1.4
                        if (m_pClientList[iClientH]->m_iHitRatio < 0) m_pClientList[iClientH]->m_iHitRatio = 0;
                    }
                }

                if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
                {

                    iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                    if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sDownSkillIndex)
                    {

                        m_pClientList[iClientH]->m_iHitRatio -= iDownPoint; // v1.4
                        if (m_pClientList[iClientH]->m_iHitRatio < 0) m_pClientList[iClientH]->m_iHitRatio = 0;
                    }
                }

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, sDownSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex], 0, 0);
            }
            else
            {

                return false;
            }
        }
        return true;
    }

    return false;
}

int CGame::iGetFollowerNumber(short sOwnerH, char cOwnerType)
{
    int i, iTotal;

    iTotal = 0;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if ((m_pNpcList[i] != 0) && (m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW))
        {

            if ((m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH) && (m_pNpcList[i]->m_cFollowOwnerType == cOwnerType))
                iTotal++;
        }

    return iTotal;
}


void CGame::SendObjectMotionRejectMsg(int iClientH)
{
    char * cp, cData[30];
    uint32_t * dwp;
    uint16_t * wp;
    short * sp;
    int     iRet;

    m_pClientList[iClientH]->m_bIsMoveBlocked = true; // v2.171

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOTION_REJECT;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sX;
    cp += 2;
    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sY;
    cp += 2;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 10);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:

            DeleteClient(iClientH, true, true);
            return;
    }
    return;
}

int CGame::_iGetTotalClients()
{
    int i, iTotal;

    iTotal = 0;
    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != 0) iTotal++;

    return iTotal;
}

void CGame::DynamicObjectEffectProcessor()
{
    int i, ix, iy, iIndex;
    short sOwnerH, iDamage, sType;
    char  cOwnerType;
    uint32_t dwTime = timeGetTime(), dwRegisterTime;

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != 0)
        {
            switch (m_pDynamicObjectList[i]->m_sType)
            {
                case DEF_DYNAMICOBJECT_PCLOUD_BEGIN:

                    for (ix = m_pDynamicObjectList[i]->m_sX - 1; ix <= m_pDynamicObjectList[i]->m_sX + 1; ix++)
                        for (iy = m_pDynamicObjectList[i]->m_sY - 1; iy <= m_pDynamicObjectList[i]->m_sY + 1; iy++)
                        {

                            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if (sOwnerH != 0)
                            {

                                switch (cOwnerType)
                                {
                                    case DEF_OWNERTYPE_PLAYER:
                                        if (m_pClientList[sOwnerH] == 0) break;
                                        if (m_pClientList[sOwnerH]->m_bIsKilled == true) break;

                                        if (m_pClientList[sOwnerH]->m_iAdminUserLevel > 0) break;


                                        //if ((m_pClientList[sOwnerH]->m_bIsNeutral == true) && (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) == 0) break;


                                        if (m_pDynamicObjectList[i]->m_iV1 < 20)
                                            iDamage = iDice(1, 6);
                                        else iDamage = iDice(1, 8);

                                        m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                        if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                        {

                                            ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                        }
                                        else
                                        {
                                            if (iDamage > 0)
                                            {

                                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);

                                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                                {

                                                    // 1: Hold-Person 
                                                    // 2: Paralize
                                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], 0, 0);

                                                    m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                                                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                                                }
                                            }


                                            if ((bCheckResistingMagicSuccess(1, sOwnerH, DEF_OWNERTYPE_PLAYER, 100) == false) &&
                                                (m_pClientList[sOwnerH]->m_bIsPoisoned == false))
                                            {

                                                m_pClientList[sOwnerH]->m_bIsPoisoned = true;
                                                m_pClientList[sOwnerH]->m_iPoisonLevel = m_pDynamicObjectList[i]->m_iV1;
                                                m_pClientList[sOwnerH]->m_dwPoisonTime = dwTime;
                                                SetPoisonFlag(sOwnerH, cOwnerType, true);

                                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pClientList[sOwnerH]->m_iPoisonLevel, 0, 0);
#ifdef DEF_TAIWANLOG
                                                _bItemLog(DEF_ITEMLOG_POISONED, sOwnerH, (char *)0, NULL);
#endif
                                            }
                                        }
                                        break;

                                    case DEF_OWNERTYPE_NPC:

                                        if (m_pNpcList[sOwnerH] == 0) break;


                                        if (m_pDynamicObjectList[i]->m_iV1 < 20)
                                            iDamage = iDice(1, 6);
                                        else iDamage = iDice(1, 8);


                                        switch (m_pNpcList[sOwnerH]->m_sType)
                                        {
                                            case 40:
                                            case 41:
                                            case 6:
                                                iDamage = 0;
                                                break;
                                        }


                                        switch (m_pNpcList[sOwnerH]->m_cActionLimit)
                                        {
                                            case 0:
                                            case 3:
                                            case 5:
                                                m_pNpcList[sOwnerH]->m_iHP -= iDamage;
                                                break;
                                        }
                                        //if (m_pNpcList[sOwnerH]->m_cActionLimit == 0) 
                                        //	m_pNpcList[sOwnerH]->m_iHP -= iDamage;

                                        if (m_pNpcList[sOwnerH]->m_iHP <= 0)
                                        {

                                            NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0); //v1.2 Áßµ¶±¸¸§¿¡ Á×À¸¸é ¸¶Áö¸· ´ë¹ÌÁö°¡ 0. ¾ÆÀÌÅÛÀ» ½±°Ô ±¸ÇÏÁö ¸øÇÏ°Ô ÇÏ±â À§ÇÔ.
                                        }
                                        else
                                        {

                                            if (iDice(1, 3) == 2)
                                                m_pNpcList[sOwnerH]->m_dwTime = dwTime;

                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                            {

                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                                            }


                                            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
                                        }
                                        break;
                                }
                            }
                        }
                    break;

                case DEF_DYNAMICOBJECT_ICESTORM:

                    for (ix = m_pDynamicObjectList[i]->m_sX - 2; ix <= m_pDynamicObjectList[i]->m_sX + 2; ix++)
                        for (iy = m_pDynamicObjectList[i]->m_sY - 2; iy <= m_pDynamicObjectList[i]->m_sY + 2; iy++)
                        {

                            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if (sOwnerH != 0)
                            {

                                switch (cOwnerType)
                                {
                                    case DEF_OWNERTYPE_PLAYER:
                                        if (m_pClientList[sOwnerH] == 0) break;
                                        if (m_pClientList[sOwnerH]->m_bIsKilled == true) break;

                                        if (m_pClientList[sOwnerH]->m_iAdminUserLevel > 0) break;

                                        //if ((m_pClientList[sOwnerH]->m_bIsNeutral == true) && (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) == 0) break;

                                        iDamage = iDice(3, 3) + 5;
                                        m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                        if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                        {

                                            ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                        }
                                        else
                                        {
                                            if (iDamage > 0)
                                            {

                                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);

                                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 1)
                                                {

                                                    // 1: Hold-Person 
                                                    // 2: Paralize
                                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], 0, 0);

                                                    m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                                                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                                                }
                                            }


                                            if ((bCheckResistingIceSuccess(1, sOwnerH, DEF_OWNERTYPE_PLAYER, m_pDynamicObjectList[i]->m_iV1) == false) &&
                                                (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0))
                                            {

                                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                                SetIceFlag(sOwnerH, cOwnerType, true);

                                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (20 * 1000),
                                                    sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                            }
                                        }
                                        break;

                                    case DEF_OWNERTYPE_NPC:
                                        if (m_pNpcList[sOwnerH] == 0) break;

                                        iDamage = iDice(3, 3) + 5;


                                        switch (m_pNpcList[sOwnerH]->m_sType)
                                        {
                                            case 40:
                                            case 41:
                                            case 6:
                                                iDamage = 0;
                                                break;
                                        }


                                        switch (m_pNpcList[sOwnerH]->m_cActionLimit)
                                        {
                                            case 0:
                                            case 3:
                                            case 5:
                                                m_pNpcList[sOwnerH]->m_iHP -= iDamage;
                                                break;
                                        }
                                        //if (m_pNpcList[sOwnerH]->m_cActionLimit == 0) 
                                        //	m_pNpcList[sOwnerH]->m_iHP -= iDamage;

                                        if (m_pNpcList[sOwnerH]->m_iHP <= 0)
                                        {

                                            NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0); //v1.2 Å¸¼­ Á×À¸¸é ¸¶Áö¸· ´ë¹ÌÁö°¡ 0. ¾ÆÀÌÅÛÀ» ½±°Ô ±¸ÇÏÁö ¸øÇÏ°Ô ÇÏ±â À§ÇÔ.
                                        }
                                        else
                                        {

                                            if (iDice(1, 3) == 2)
                                                m_pNpcList[sOwnerH]->m_dwTime = dwTime;

                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                            {

                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                                            }


                                            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);


                                            if ((bCheckResistingIceSuccess(1, sOwnerH, DEF_OWNERTYPE_NPC, m_pDynamicObjectList[i]->m_iV1) == false) &&
                                                (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0))
                                            {

                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                                SetIceFlag(sOwnerH, cOwnerType, true);

                                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (20 * 1000),
                                                    sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                            }
                                        }
                                        break;
                                }
                            }


                            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                                (m_pClientList[sOwnerH]->m_iHP > 0))
                            {

                                iDamage = iDice(3, 2);
                                m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                {

                                    ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                }
                                else
                                {
                                    if (iDamage > 0)
                                    {

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
                                    }
                                }
                            }


                            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sType, &dwRegisterTime, &iIndex);
                            if ((sType == DEF_DYNAMICOBJECT_FIRE) && (m_pDynamicObjectList[iIndex] != 0))
                                m_pDynamicObjectList[iIndex]->m_dwLastTime = m_pDynamicObjectList[iIndex]->m_dwLastTime - (m_pDynamicObjectList[iIndex]->m_dwLastTime / 10);
                        }
                    break;

                case DEF_DYNAMICOBJECT_FIRE:

                    if (m_pDynamicObjectList[i]->m_iCount == 1)
                    {

                        CheckFireBluring(m_pDynamicObjectList[i]->m_cMapIndex, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY);
                    }
                    m_pDynamicObjectList[i]->m_iCount++;
                    if (m_pDynamicObjectList[i]->m_iCount > 10) m_pDynamicObjectList[i]->m_iCount = 10;


                    for (ix = m_pDynamicObjectList[i]->m_sX - 1; ix <= m_pDynamicObjectList[i]->m_sX + 1; ix++)
                        for (iy = m_pDynamicObjectList[i]->m_sY - 1; iy <= m_pDynamicObjectList[i]->m_sY + 1; iy++)
                        {

                            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if (sOwnerH != 0)
                            {

                                switch (cOwnerType)
                                {

                                    case DEF_OWNERTYPE_PLAYER:
                                        if (m_pClientList[sOwnerH] == 0) break;
                                        if (m_pClientList[sOwnerH]->m_bIsKilled == true) break;

                                        if (m_pClientList[sOwnerH]->m_iAdminUserLevel > 0) break;


                                        //if ((m_pClientList[sOwnerH]->m_bIsNeutral == true) && (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) == 0) break;

                                        iDamage = iDice(1, 6);
                                        m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                        if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                        {

                                            ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                        }
                                        else
                                        {
                                            if (iDamage > 0)
                                            {

                                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);

                                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                                {

                                                    // 1: Hold-Person 
                                                    // 2: Paralize	
                                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], 0, 0);

                                                    m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                                                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                                                }
                                            }
                                        }
                                        break;

                                    case DEF_OWNERTYPE_NPC:
                                        if (m_pNpcList[sOwnerH] == 0) break;

                                        iDamage = iDice(1, 6);


                                        switch (m_pNpcList[sOwnerH]->m_sType)
                                        {
                                            case 40:
                                            case 41:
                                            case 6:
                                                iDamage = 0;
                                                break;
                                        }


                                        switch (m_pNpcList[sOwnerH]->m_cActionLimit)
                                        {
                                            case 0:
                                            case 3:
                                            case 5:
                                                m_pNpcList[sOwnerH]->m_iHP -= iDamage;
                                                break;
                                        }
                                        //if (m_pNpcList[sOwnerH]->m_cActionLimit == 0) 
                                        //	m_pNpcList[sOwnerH]->m_iHP -= iDamage;

                                        if (m_pNpcList[sOwnerH]->m_iHP <= 0)
                                        {

                                            NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0); //v1.2 Å¸¼­ Á×À¸¸é ¸¶Áö¸· ´ë¹ÌÁö°¡ 0. ¾ÆÀÌÅÛÀ» ½±°Ô ±¸ÇÏÁö ¸øÇÏ°Ô ÇÏ±â À§ÇÔ.
                                        }
                                        else
                                        {

                                            if (iDice(1, 3) == 2)
                                                m_pNpcList[sOwnerH]->m_dwTime = dwTime;

                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                            {

                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                                            }


                                            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
                                        }
                                        break;
                                }
                            }


                            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                                (m_pClientList[sOwnerH]->m_iHP > 0))
                            {

                                iDamage = iDice(1, 6);
                                m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                {

                                    ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                }
                                else
                                {
                                    if (iDamage > 0)
                                    {

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
                                    }
                                }
                            }


                            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sType, &dwRegisterTime, &iIndex);
                            if ((sType == DEF_DYNAMICOBJECT_ICESTORM) && (m_pDynamicObjectList[iIndex] != 0))
                                m_pDynamicObjectList[iIndex]->m_dwLastTime = m_pDynamicObjectList[iIndex]->m_dwLastTime - (m_pDynamicObjectList[iIndex]->m_dwLastTime / 10);
                        }
                    break;
            }
        }
}


void CGame::ClearSkillUsingStatus(int iClientH)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
    {
        m_pClientList[iClientH]->m_bSkillUsingStatus[i] = false;
        m_pClientList[iClientH]->m_iSkillUsingTimeID[i] = 0; //v1.12
    }



    if (m_pClientList[iClientH]->m_iAllocatedFish != 0)
    {



        if (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] != 0)
            m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;

        m_pClientList[iClientH]->m_iAllocatedFish = 0;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FISHCANCELED, 0, 0, 0, 0);
    }

}

int CGame::iCalculateUseSkillItemEffect(int iOwnerH, char cOwnerType, char cOwnerSkill, int iSkillNum, char cMapIndex, int dX, int dY)
{
    CItem * pItem;
    short lX, lY;
    int   iResult, iFish, iItemID = 0;

    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[iOwnerH] == 0) return 0;
            if (m_pClientList[iOwnerH]->m_cMapIndex != cMapIndex) return 0;
            lX = m_pClientList[iOwnerH]->m_sX;
            lY = m_pClientList[iOwnerH]->m_sY;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[iOwnerH] == 0) return 0;
            if (m_pNpcList[iOwnerH]->m_cMapIndex != cMapIndex) return 0;
            lX = m_pNpcList[iOwnerH]->m_sX;
            lY = m_pNpcList[iOwnerH]->m_sY;
            break;
    }


    if (cOwnerSkill == 0) return 0;


    iResult = iDice(1, 105);
    if (cOwnerSkill <= iResult)	return 0;

    // ???????Â ???Ã?? ºÒ???É 
    if (m_pMapList[cMapIndex]->bGetIsWater(dX, dY) == false) return 0;


    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
        CalculateSSN_SkillIndex(iOwnerH, iSkillNum, 1);

    switch (m_pSkillConfigList[iSkillNum]->m_sType)
    {
        case DEF_SKILLEFFECTTYPE_TAMING:

            _TamingHandler(iOwnerH, iSkillNum, cMapIndex, dX, dY);
            break;

        case DEF_SKILLEFFECTTYPE_GET:

            switch (m_pSkillConfigList[iSkillNum]->m_sValue1)
            {
                case 1:

                    iItemID = 99;  // ?í?â 
                    break;

                case 2:
                    // ???í?â 

                    //if (m_pMapList[cMapIndex]->bGetIsWater(dX, dY) == false) return 0; 


                    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                    {
                        iFish = iCheckFish(iOwnerH, cMapIndex, dX, dY);
                        if (iFish == 0) iItemID = 100; // ???í?â 
                    }
                    else iItemID = 100; // ???í?â 
                    break;
            }

            if (iItemID != 0)
            {


                if (iItemID == 100)
                {
                    SendNotifyMsg(NULL, iOwnerH, DEF_NOTIFY_FISHSUCCESS, 0, 0, 0, 0);


                    GetExp(iOwnerH, iDice(2, 5)); //m_pClientList[iOwnerH]->m_iExpStock += iDice(1,2);
                }

                pItem = new CItem;
                if (pItem == 0) return 0;
                if (_bInitItemAttr(pItem, iItemID) == true)
                {


                    pItem->m_sTouchEffectType = DEF_ITET_ID;
                    pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                    pItem->m_sTouchEffectValue2 = iDice(1, 100000);

                    pItem->m_sTouchEffectValue3 = (short)timeGetTime();


                    m_pMapList[cMapIndex]->bSetItem(lX, lY, pItem);


                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, cMapIndex,
                        lX, lY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4
                }
            }
            break;

    }

    return 1;
}





int CGame::iCalcTotalWeight(int iClientH)
{
    int i, iWeight;
    short sItemIndex;

    if (m_pClientList[iClientH] == 0) return 0;

    m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;

    for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
        if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0)
        {
            switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
            {
                case DEF_ITEMEFFECTTYPE_ALTERITEMDROP:
                    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan > 0)
                    {

                        m_pClientList[iClientH]->m_iAlterItemDropIndex = sItemIndex;
                    }
                    break;
            }
        }

    iWeight = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != 0)
        {

            iWeight += iGetItemWeight(m_pClientList[iClientH]->m_pItemList[i], m_pClientList[iClientH]->m_pItemList[i]->m_dwCount);
        }

    m_pClientList[iClientH]->m_iCurWeightLoad = iWeight;

    return iWeight;
}







void CGame::___RestorePlayerCharacteristics(int iClientH)
{
    int iStr, iDex, iInt, iVit, iMag, iCharisma;
    int iOriginalPoint, iCurPoint, iVerifyPoint, iToBeRestoredPoint;
    int iMax, iA, iB;
    bool bFlag;
    char cTxt[120];

    if (m_pClientList[iClientH] == 0) return;


    iCurPoint = m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iInt +
        m_pClientList[iClientH]->m_iVit + m_pClientList[iClientH]->m_iDex +
        m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma;

    iOriginalPoint = (m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70;


    m_pClientList[iClientH]->m_iLU_Point = iOriginalPoint - iCurPoint;


    return;
    //////////////////////////////////////////////////////////////////////////
    //HALT
    //////////////////////////////////////////////////////////////////////////



    iStr = m_pClientList[iClientH]->m_iStr;
    iDex = m_pClientList[iClientH]->m_iDex;
    iInt = m_pClientList[iClientH]->m_iInt;
    iVit = m_pClientList[iClientH]->m_iVit;
    iMag = m_pClientList[iClientH]->m_iMag;
    iCharisma = m_pClientList[iClientH]->m_iCharisma;


    iCurPoint = m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iInt +
        m_pClientList[iClientH]->m_iVit + m_pClientList[iClientH]->m_iDex +
        m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma;

    iOriginalPoint = (m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70;

    iToBeRestoredPoint = iOriginalPoint - iCurPoint;


    if (iToBeRestoredPoint == 0) return;

    if (iToBeRestoredPoint > 0)
    {


        while (1)
        {
            bFlag = false;

            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iStr < 10))
            {
                m_pClientList[iClientH]->m_iStr++;
                iToBeRestoredPoint--;
                bFlag = true;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iMag < 10))
            {
                m_pClientList[iClientH]->m_iMag++;
                iToBeRestoredPoint--;
                bFlag = true;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iInt < 10))
            {
                m_pClientList[iClientH]->m_iInt++;
                iToBeRestoredPoint--;
                bFlag = true;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iDex < 10))
            {
                m_pClientList[iClientH]->m_iDex++;
                iToBeRestoredPoint--;
                bFlag = true;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iVit < 10))
            {
                m_pClientList[iClientH]->m_iVit++;
                iToBeRestoredPoint--;
                bFlag = true;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iCharisma < 10))
            {
                m_pClientList[iClientH]->m_iCharisma++;
                iToBeRestoredPoint--;
                bFlag = true;
            }

            if (bFlag == false)          break;
            if (iToBeRestoredPoint <= 0) break;
        }


        iMax = m_pClientList[iClientH]->m_cSkillMastery[5];

        if (m_pClientList[iClientH]->m_iStr < (iMax / 2))
        {

            while (1)
            {
                if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iStr < (iMax / 2)))
                {
                    m_pClientList[iClientH]->m_iStr++;
                    iToBeRestoredPoint--;
                }

                if (m_pClientList[iClientH]->m_iStr == (iMax / 2)) break;
                if (iToBeRestoredPoint <= 0) break;
            }
        }


        iA = m_pClientList[iClientH]->m_cSkillMastery[7];
        iB = m_pClientList[iClientH]->m_cSkillMastery[8];
        if (iA > iB)
            iMax = iA;
        else iMax = iB;
        iA = m_pClientList[iClientH]->m_cSkillMastery[9];
        if (iA > iMax) iMax = iA;
        iA = m_pClientList[iClientH]->m_cSkillMastery[6];
        if (iA > iMax) iMax = iA;


        if (m_pClientList[iClientH]->m_iDex < (iMax / 2))
        {

            while (1)
            {
                if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iDex < (iMax / 2)))
                {
                    m_pClientList[iClientH]->m_iDex++;
                    iToBeRestoredPoint--;
                }

                if (m_pClientList[iClientH]->m_iDex == (iMax / 2)) break;
                if (iToBeRestoredPoint <= 0) break;
            }
        }


        iMax = m_pClientList[iClientH]->m_cSkillMastery[19];

        if (m_pClientList[iClientH]->m_iInt < (iMax / 2))
        {

            while (1)
            {
                if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iInt < (iMax / 2)))
                {
                    m_pClientList[iClientH]->m_iInt++;
                    iToBeRestoredPoint--;
                }

                if (m_pClientList[iClientH]->m_iInt == (iMax / 2)) break;
                if (iToBeRestoredPoint <= 0) break;
            }
        }


        iA = m_pClientList[iClientH]->m_cSkillMastery[3];
        iB = m_pClientList[iClientH]->m_cSkillMastery[4];
        if (iA > iB)
            iMax = iA;
        else iMax = iB;

        if (m_pClientList[iClientH]->m_iMag < (iMax / 2))
        {

            while (1)
            {
                if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iMag < (iMax / 2)))
                {
                    m_pClientList[iClientH]->m_iMag++;
                    iToBeRestoredPoint--;
                }

                if (m_pClientList[iClientH]->m_iMag == (iMax / 2)) break;
                if (iToBeRestoredPoint <= 0) break;
            }
        }


        while (iToBeRestoredPoint != 0)
        {
            switch (iDice(1, 6))
            {
                case 1:
                    if (m_pClientList[iClientH]->m_iStr < DEF_CHARPOINTLIMIT)
                    {
                        m_pClientList[iClientH]->m_iStr++;
                        iToBeRestoredPoint--;
                    }
                    break;
                case 2:
                    if (m_pClientList[iClientH]->m_iVit < DEF_CHARPOINTLIMIT)
                    {
                        m_pClientList[iClientH]->m_iVit++;
                        iToBeRestoredPoint--;
                    }
                    break;
                case 3:
                    if (m_pClientList[iClientH]->m_iDex < DEF_CHARPOINTLIMIT)
                    {
                        m_pClientList[iClientH]->m_iDex++;
                        iToBeRestoredPoint--;
                    }
                    break;
                case 4:
                    if (m_pClientList[iClientH]->m_iMag < DEF_CHARPOINTLIMIT)
                    {
                        m_pClientList[iClientH]->m_iMag++;
                        iToBeRestoredPoint--;
                    }
                    break;
                case 5:
                    if (m_pClientList[iClientH]->m_iInt < DEF_CHARPOINTLIMIT)
                    {
                        m_pClientList[iClientH]->m_iInt++;
                        iToBeRestoredPoint--;
                    }
                    break;
                case 6:
                    if (m_pClientList[iClientH]->m_iCharisma < DEF_CHARPOINTLIMIT)
                    {
                        m_pClientList[iClientH]->m_iCharisma++;
                        iToBeRestoredPoint--;
                    }
                    break;
            }
        }


        iVerifyPoint = m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iInt +
            m_pClientList[iClientH]->m_iVit + m_pClientList[iClientH]->m_iDex +
            m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma;

        if (iVerifyPoint != iOriginalPoint)
        {
            wsprintf(cTxt, "(T_T) RestorePlayerCharacteristics(Minor) FAIL! Player(%s)-(%d/%d)", m_pClientList[iClientH]->m_cCharName, iVerifyPoint, iOriginalPoint);
            log->info(cTxt);


            m_pClientList[iClientH]->m_iStr = iStr;
            m_pClientList[iClientH]->m_iDex = iDex;
            m_pClientList[iClientH]->m_iInt = iInt;
            m_pClientList[iClientH]->m_iVit = iVit;
            m_pClientList[iClientH]->m_iMag = iMag;
            m_pClientList[iClientH]->m_iCharisma = iCharisma;
        }
        else
        {
            wsprintf(cTxt, "(^o^) RestorePlayerCharacteristics(Minor) SUCCESS! Player(%s)-(%d/%d)", m_pClientList[iClientH]->m_cCharName, iVerifyPoint, iOriginalPoint);
            log->info(cTxt);
        }
    }
    else
    {



        while (1)
        {
            bFlag = false;
            if (m_pClientList[iClientH]->m_iStr > DEF_CHARPOINTLIMIT)
            {
                bFlag = true;
                m_pClientList[iClientH]->m_iStr--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iDex > DEF_CHARPOINTLIMIT)
            {
                bFlag = true;
                m_pClientList[iClientH]->m_iDex--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iVit > DEF_CHARPOINTLIMIT)
            {
                bFlag = true;
                m_pClientList[iClientH]->m_iVit--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iInt > DEF_CHARPOINTLIMIT)
            {
                bFlag = true;
                m_pClientList[iClientH]->m_iInt--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iMag > DEF_CHARPOINTLIMIT)
            {
                bFlag = true;
                m_pClientList[iClientH]->m_iMag--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iCharisma > DEF_CHARPOINTLIMIT)
            {
                bFlag = true;
                m_pClientList[iClientH]->m_iCharisma--;
                iToBeRestoredPoint++;
            }

            if (bFlag == false)	break;
            if (iToBeRestoredPoint >= 0) break;
        }

        if (iToBeRestoredPoint < 0)
        {

            while (iToBeRestoredPoint != 0)
            {
                switch (iDice(1, 6))
                {
                    case 1:
                        if (m_pClientList[iClientH]->m_iStr > 10)
                        {
                            m_pClientList[iClientH]->m_iStr--;
                            iToBeRestoredPoint++;
                        }
                        break;
                    case 2:
                        if (m_pClientList[iClientH]->m_iVit > 10)
                        {
                            m_pClientList[iClientH]->m_iVit--;
                            iToBeRestoredPoint++;
                        }
                        break;
                    case 3:
                        if (m_pClientList[iClientH]->m_iDex > 10)
                        {
                            m_pClientList[iClientH]->m_iDex--;
                            iToBeRestoredPoint++;
                        }
                        break;
                    case 4:
                        if (m_pClientList[iClientH]->m_iMag > 10)
                        {
                            m_pClientList[iClientH]->m_iMag--;
                            iToBeRestoredPoint++;
                        }
                        break;
                    case 5:
                        if (m_pClientList[iClientH]->m_iInt > 10)
                        {
                            m_pClientList[iClientH]->m_iInt--;
                            iToBeRestoredPoint++;
                        }
                        break;
                    case 6:
                        if (m_pClientList[iClientH]->m_iCharisma > 10)
                        {
                            m_pClientList[iClientH]->m_iCharisma--;
                            iToBeRestoredPoint++;
                        }
                        break;
                }
            }
        }
        else
        {

            while (iToBeRestoredPoint != 0)
            {
                switch (iDice(1, 6))
                {
                    case 1:
                        if (m_pClientList[iClientH]->m_iStr < DEF_CHARPOINTLIMIT)
                        {
                            m_pClientList[iClientH]->m_iStr++;
                            iToBeRestoredPoint--;
                        }
                        break;
                    case 2:
                        if (m_pClientList[iClientH]->m_iVit < DEF_CHARPOINTLIMIT)
                        {
                            m_pClientList[iClientH]->m_iVit++;
                            iToBeRestoredPoint--;
                        }
                        break;
                    case 3:
                        if (m_pClientList[iClientH]->m_iDex < DEF_CHARPOINTLIMIT)
                        {
                            m_pClientList[iClientH]->m_iDex++;
                            iToBeRestoredPoint--;
                        }
                        break;
                    case 4:
                        if (m_pClientList[iClientH]->m_iMag < DEF_CHARPOINTLIMIT)
                        {
                            m_pClientList[iClientH]->m_iMag++;
                            iToBeRestoredPoint--;
                        }
                        break;
                    case 5:
                        if (m_pClientList[iClientH]->m_iInt < DEF_CHARPOINTLIMIT)
                        {
                            m_pClientList[iClientH]->m_iInt++;
                            iToBeRestoredPoint--;
                        }
                        break;
                    case 6:
                        if (m_pClientList[iClientH]->m_iCharisma < DEF_CHARPOINTLIMIT)
                        {
                            m_pClientList[iClientH]->m_iCharisma++;
                            iToBeRestoredPoint--;
                        }
                        break;
                }
            }
        }


        iVerifyPoint = m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iInt +
            m_pClientList[iClientH]->m_iVit + m_pClientList[iClientH]->m_iDex +
            m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma;

        if (iVerifyPoint != iOriginalPoint)
        {
            wsprintf(cTxt, "(T_T) RestorePlayerCharacteristics(Over) FAIL! Player(%s)-(%d/%d)", m_pClientList[iClientH]->m_cCharName, iVerifyPoint, iOriginalPoint);
            log->info(cTxt);


            /*
            m_pClientList[iClientH]->m_iStr = iStr;
            m_pClientList[iClientH]->m_iDex = iDex;
            m_pClientList[iClientH]->m_iInt = iInt;
            m_pClientList[iClientH]->m_iVit = iVit;
            m_pClientList[iClientH]->m_iMag = iMag;
            m_pClientList[iClientH]->m_iCharisma = iCharisma;
            */
        }
        else
        {
            wsprintf(cTxt, "(^o^) RestorePlayerCharacteristics(Over) SUCCESS! Player(%s)-(%d/%d)", m_pClientList[iClientH]->m_cCharName, iVerifyPoint, iOriginalPoint);
            log->info(cTxt);
        }
    }
}


void CGame::CalcTotalItemEffect(int iClientH, int iEquipItemID, bool bNotify)
{
    short sItemIndex;
    int  i, iArrowIndex, iPrevSAType, iTemp;
    char cEquipPos;
    double dV1, dV2, dV3;
    uint32_t  dwSWEType, dwSWEValue;
    short  sTemp;

    if (m_pClientList[iClientH] == 0) return;

    if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) &&
        (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1))
    {


        if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] != 0)
        {

            m_pClientList[iClientH]->m_bIsItemEquipped[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] = false;
            m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] = -1;
        }
    }

    m_pClientList[iClientH]->m_cAttackDiceThrow_SM = 0;
    m_pClientList[iClientH]->m_cAttackDiceRange_SM = 0;
    m_pClientList[iClientH]->m_cAttackBonus_SM = 0;

    m_pClientList[iClientH]->m_cAttackDiceThrow_L = 0;
    m_pClientList[iClientH]->m_cAttackDiceRange_L = 0;
    m_pClientList[iClientH]->m_cAttackBonus_L = 0;

    m_pClientList[iClientH]->m_iHitRatio = 0;
    m_pClientList[iClientH]->m_iDefenseRatio = m_pClientList[iClientH]->m_iDex * 2;
    m_pClientList[iClientH]->m_iDamageAbsorption_Shield = 0;

    for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
        m_pClientList[iClientH]->m_iDamageAbsorption_Armor[i] = 0;

    m_pClientList[iClientH]->m_iManaSaveRatio = 0;
    m_pClientList[iClientH]->m_iAddResistMagic = 0;

    m_pClientList[iClientH]->m_iAddPhysicalDamage = 0;
    m_pClientList[iClientH]->m_iAddMagicalDamage = 0;

    m_pClientList[iClientH]->m_bIsLuckyEffect = false;
    m_pClientList[iClientH]->m_iMagicDamageSaveItemIndex = -1;
    m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = 0;

    m_pClientList[iClientH]->m_iAddAbsAir = 0;
    m_pClientList[iClientH]->m_iAddAbsEarth = 0;
    m_pClientList[iClientH]->m_iAddAbsFire = 0;
    m_pClientList[iClientH]->m_iAddAbsWater = 0;

    m_pClientList[iClientH]->m_iCustomItemValue_Attack = 0;
    m_pClientList[iClientH]->m_iCustomItemValue_Defense = 0;

    m_pClientList[iClientH]->m_iMinAP_SM = 0;
    m_pClientList[iClientH]->m_iMinAP_L = 0;

    m_pClientList[iClientH]->m_iMaxAP_SM = 0;
    m_pClientList[iClientH]->m_iMaxAP_L = 0;

    m_pClientList[iClientH]->m_iSpecialWeaponEffectType = 0;
    m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = 0;

    m_pClientList[iClientH]->m_iAddHP = m_pClientList[iClientH]->m_iAddSP = m_pClientList[iClientH]->m_iAddMP = 0;
    m_pClientList[iClientH]->m_iAddAR = m_pClientList[iClientH]->m_iAddPR = m_pClientList[iClientH]->m_iAddDR = 0;
    m_pClientList[iClientH]->m_iAddMR = m_pClientList[iClientH]->m_iAddAbsPD = m_pClientList[iClientH]->m_iAddAbsMD = 0;
    m_pClientList[iClientH]->m_iAddCD = m_pClientList[iClientH]->m_iAddExp = m_pClientList[iClientH]->m_iAddGold = 0;
    // v2.20 2002-12-28 3ÁÖ³â ±â³ä¹ÝÁö ¹ö±× ¼öÁ¤  
    m_pClientList[iClientH]->m_iHPStatic_stock = 0;

    iPrevSAType = m_pClientList[iClientH]->m_iSpecialAbilityType;

    m_pClientList[iClientH]->m_iSpecialAbilityType = 0;
    m_pClientList[iClientH]->m_iSpecialAbilityLastSec = 0;
    m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = 0;

    m_pClientList[iClientH]->m_iAddTransMana = 0;
    m_pClientList[iClientH]->m_iAddChargeCritical = 0;

    m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;

    for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
        if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0)
        {
            switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
            {
                case DEF_ITEMEFFECTTYPE_ALTERITEMDROP:
                    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan > 0)
                    {

                        m_pClientList[iClientH]->m_iAlterItemDropIndex = sItemIndex;
                    }
                    break;
            }
        }


    for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
        if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) &&
            (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == true))
        {

            cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

            switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
            {

                case DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE:

                    m_pClientList[iClientH]->m_iMagicDamageSaveItemIndex = sItemIndex;
                    break;


                    // v2.20 2002-12-28 3ÁÖ³â ±â³ä¹ÝÁö ¹ö±× ¼öÁ¤ 
                case DEF_ITEMEFFECTTYPE_HP:
                    m_pClientList[iClientH]->m_iHPStatic_stock += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
                    break;

                case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:
                case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE:
                case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
                case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:
                case DEF_ITEMEFFECTTYPE_ATTACK:

                    m_pClientList[iClientH]->m_cAttackDiceThrow_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                    m_pClientList[iClientH]->m_cAttackDiceRange_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                    m_pClientList[iClientH]->m_cAttackBonus_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
                    m_pClientList[iClientH]->m_cAttackDiceThrow_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
                    m_pClientList[iClientH]->m_cAttackDiceRange_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5;
                    m_pClientList[iClientH]->m_cAttackBonus_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;


                    iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
                    //testcode
                    //wsprintf(G_cTxt, "Add Damage: %d", iTemp);
                    //log->info(G_cTxt);


                    m_pClientList[iClientH]->m_iAddPhysicalDamage += iTemp;
                    m_pClientList[iClientH]->m_iAddMagicalDamage += iTemp;


                    m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];


                    //m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSM_HitRatio;
                    //m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L  += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sL_HitRatio;

                    m_pClientList[iClientH]->m_sUsingWeaponSkill = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill;


                    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != 0)
                    {
                        m_pClientList[iClientH]->m_iCustomItemValue_Attack += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
                        // ¿¡·¯ º¸Á¤¿ë 
                        if (m_pClientList[iClientH]->m_iCustomItemValue_Attack > 100)
                            m_pClientList[iClientH]->m_iCustomItemValue_Attack = 100;

                        if (m_pClientList[iClientH]->m_iCustomItemValue_Attack < -100)
                            m_pClientList[iClientH]->m_iCustomItemValue_Attack = -100;

                        if (m_pClientList[iClientH]->m_iCustomItemValue_Attack > 0)
                        {

                            dV2 = (double)m_pClientList[iClientH]->m_iCustomItemValue_Attack;
                            dV1 = (dV2 / 100.0f) * (5.0f);
                            m_pClientList[iClientH]->m_iMinAP_SM = m_pClientList[iClientH]->m_cAttackDiceThrow_SM +
                                m_pClientList[iClientH]->m_cAttackBonus_SM + (int)dV1;

                            m_pClientList[iClientH]->m_iMinAP_L = m_pClientList[iClientH]->m_cAttackDiceThrow_L +
                                m_pClientList[iClientH]->m_cAttackBonus_L + (int)dV1;

                            // ¼öÄ¡ Á¶Á¤ 
                            if (m_pClientList[iClientH]->m_iMinAP_SM < 1) m_pClientList[iClientH]->m_iMinAP_SM = 1;
                            if (m_pClientList[iClientH]->m_iMinAP_L < 1)  m_pClientList[iClientH]->m_iMinAP_L = 1;


                            if (m_pClientList[iClientH]->m_iMinAP_SM > (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
                                m_pClientList[iClientH]->m_iMinAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);

                            if (m_pClientList[iClientH]->m_iMinAP_L > (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
                                m_pClientList[iClientH]->m_iMinAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);

                            //testcode
                            //wsprintf(G_cTxt, "MinAP: %d %d +(%d)", m_pClientList[iClientH]->m_iMinAP_SM, m_pClientList[iClientH]->m_iMinAP_L, (int)dV1);
                            //log->info(G_cTxt);
                        }
                        else if (m_pClientList[iClientH]->m_iCustomItemValue_Attack < 0)
                        {

                            dV2 = (double)m_pClientList[iClientH]->m_iCustomItemValue_Attack;
                            dV1 = (dV2 / 100.0f) * (5.0f);
                            m_pClientList[iClientH]->m_iMaxAP_SM = m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM
                                + m_pClientList[iClientH]->m_cAttackBonus_SM + (int)dV1;

                            m_pClientList[iClientH]->m_iMaxAP_L = m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L
                                + m_pClientList[iClientH]->m_cAttackBonus_L + (int)dV1;

                            // ¼öÄ¡ Á¶Á¤ 
                            if (m_pClientList[iClientH]->m_iMaxAP_SM < 1) m_pClientList[iClientH]->m_iMaxAP_SM = 1;
                            if (m_pClientList[iClientH]->m_iMaxAP_L < 1)  m_pClientList[iClientH]->m_iMaxAP_L = 1;


                            if (m_pClientList[iClientH]->m_iMaxAP_SM < (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
                                m_pClientList[iClientH]->m_iMaxAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);

                            if (m_pClientList[iClientH]->m_iMaxAP_L < (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
                                m_pClientList[iClientH]->m_iMaxAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);

                            //testcode
                            //wsprintf(G_cTxt, "MaxAP: %d %d +(%d)", m_pClientList[iClientH]->m_iMaxAP_SM, m_pClientList[iClientH]->m_iMaxAP_L, (int)dV1);
                            //log->info(G_cTxt);
                        }
                    }


                    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                        dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;




                        m_pClientList[iClientH]->m_iSpecialWeaponEffectType = (int)dwSWEType;
                        m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = (int)dwSWEValue;

                        switch (dwSWEType)
                        {
                            case 7:
                                m_pClientList[iClientH]->m_cAttackDiceRange_SM++;
                                m_pClientList[iClientH]->m_cAttackDiceRange_L++;
                                break;

                            case 9:
                                m_pClientList[iClientH]->m_cAttackDiceRange_SM += 2;
                                m_pClientList[iClientH]->m_cAttackDiceRange_L += 2;
                                break;
                        }
                    }


                    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != 0)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
                        dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;


                        //Ãß°¡ µ¶¼ºÀúÇ×(1), Ãß°¡ ¸íÁß°ª(2), Ãß°¡ ¹æ¾î°ª(3), HP È¸º¹·® Ãß°¡(4), SP È¸º¹·® Ãß°¡(5)
                        //MP È¸º¹·® Ãß°¡(6), Ãß°¡ ¸¶¹ýÀúÇ×(7), ¹°¸® ´ë¹ÌÁö Èí¼ö(8), ¸¶¹ý ´ë¹ÌÁö Èí¼ö(9)
                        //¿¬Å¸ ´ë¹ÌÁö Ãß°¡(10), ´õ ¸¹Àº °æÇèÄ¡(11), ´õ¸¹Àº Gold(12)

                        switch (dwSWEType)
                        {
                            case 0:  break;
                            case 1:  m_pClientList[iClientH]->m_iAddPR += (int)dwSWEValue * 7; break;
                            case 2:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break;
                            case 3:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
                            case 4:  m_pClientList[iClientH]->m_iAddHP += (int)dwSWEValue * 7; break;
                            case 5:  m_pClientList[iClientH]->m_iAddSP += (int)dwSWEValue * 7; break;
                            case 6:  m_pClientList[iClientH]->m_iAddMP += (int)dwSWEValue * 7; break;
                            case 7:  m_pClientList[iClientH]->m_iAddMR += (int)dwSWEValue * 7; break;
                            case 8:  m_pClientList[iClientH]->m_iDamageAbsorption_Armor[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos] += (int)dwSWEValue * 3; break;
                            case 9:  m_pClientList[iClientH]->m_iAddAbsMD += (int)dwSWEValue * 3; break;
                            case 10: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
                            case 11: m_pClientList[iClientH]->m_iAddExp += (int)dwSWEValue * 10; break;
                            case 12: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
                        }


                        switch (dwSWEType)
                        {
                            case 9: if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80; break;
                        }
                    }


                    switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
                    {
                        case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:

                            m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
                            break;

                        case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:

                            m_pClientList[iClientH]->m_iManaSaveRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
                            if (m_pClientList[iClientH]->m_iManaSaveRatio > 80) m_pClientList[iClientH]->m_iManaSaveRatio = 80;
                            break;

                        case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE:

                            m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
                            break;

                        case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:

                            m_pClientList[iClientH]->m_iSpecialAbilityType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;

                            m_pClientList[iClientH]->m_iSpecialAbilityLastSec = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;

                            m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = (int)cEquipPos;


                            if ((bNotify == true) && (iEquipItemID == (int)sItemIndex))
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, 0);
                            break;
                    }
                    break;

                case DEF_ITEMEFFECTTYPE_ADDEFFECT:
                    switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1)
                    {
                        case 1:

                            m_pClientList[iClientH]->m_iAddResistMagic += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 2:

                            m_pClientList[iClientH]->m_iManaSaveRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;

                            if (m_pClientList[iClientH]->m_iManaSaveRatio > 80) m_pClientList[iClientH]->m_iManaSaveRatio = 80;
                            break;

                        case 3:

                            m_pClientList[iClientH]->m_iAddPhysicalDamage += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 4:

                            m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 5:

                            if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 != 0)
                                m_pClientList[iClientH]->m_bIsLuckyEffect = true;
                            else m_pClientList[iClientH]->m_bIsLuckyEffect = false;
                            break;

                        case 6:

                            m_pClientList[iClientH]->m_iAddMagicalDamage += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 7:
                            m_pClientList[iClientH]->m_iAddAbsAir += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 8:
                            m_pClientList[iClientH]->m_iAddAbsEarth += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 9:
                            m_pClientList[iClientH]->m_iAddAbsFire += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 10:

                            m_pClientList[iClientH]->m_iAddAbsWater += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;

                        case 11:

                            m_pClientList[iClientH]->m_iAddPR += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                            break;
                    }
                    break;

                case DEF_ITEMEFFECTTYPE_ATTACK_ARROW:


                    if ((m_pClientList[iClientH]->m_cArrowIndex != -1) &&
                        (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cArrowIndex] == 0))
                    {

                        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
                    }
                    else if (m_pClientList[iClientH]->m_cArrowIndex == -1)
                        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

                    if (m_pClientList[iClientH]->m_cArrowIndex == -1)
                    {

                        m_pClientList[iClientH]->m_cAttackDiceThrow_SM = 0;
                        m_pClientList[iClientH]->m_cAttackDiceRange_SM = 0;
                        m_pClientList[iClientH]->m_cAttackBonus_SM = 0;
                        m_pClientList[iClientH]->m_cAttackDiceThrow_L = 0;
                        m_pClientList[iClientH]->m_cAttackDiceRange_L = 0;
                        m_pClientList[iClientH]->m_cAttackBonus_L = 0;
                    }
                    else
                    {
                        iArrowIndex = m_pClientList[iClientH]->m_cArrowIndex;
                        /*
                        m_pClientList[iClientH]->m_cAttackDiceThrow_SM = m_pClientList[iClientH]->m_pItemList[iArrowIndex]->m_sItemEffectValue1;
                        m_pClientList[iClientH]->m_cAttackDiceRange_SM = m_pClientList[iClientH]->m_pItemList[iArrowIndex]->m_sItemEffectValue2;
                        m_pClientList[iClientH]->m_cAttackBonus_SM     = m_pClientList[iClientH]->m_pItemList[iArrowIndex]->m_sItemEffectValue3;
                        m_pClientList[iClientH]->m_cAttackDiceThrow_L  = m_pClientList[iClientH]->m_pItemList[iArrowIndex]->m_sItemEffectValue4;
                        m_pClientList[iClientH]->m_cAttackDiceRange_L  = m_pClientList[iClientH]->m_pItemList[iArrowIndex]->m_sItemEffectValue5;
                        m_pClientList[iClientH]->m_cAttackBonus_L      = m_pClientList[iClientH]->m_pItemList[iArrowIndex]->m_sItemEffectValue6;
                        */

                        m_pClientList[iClientH]->m_cAttackDiceThrow_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                        m_pClientList[iClientH]->m_cAttackDiceRange_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                        m_pClientList[iClientH]->m_cAttackBonus_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
                        m_pClientList[iClientH]->m_cAttackDiceThrow_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
                        m_pClientList[iClientH]->m_cAttackDiceRange_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5;
                        m_pClientList[iClientH]->m_cAttackBonus_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
                    }


                    m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];

                    // m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSM_HitRatio;
                    // m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L  += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sL_HitRatio;
                    break;

                case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
                case DEF_ITEMEFFECTTYPE_DEFENSE:


                    m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;


                    //m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSM_HitRatio;
                    //m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L  += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sL_HitRatio;


                    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != 0)
                    {
                        m_pClientList[iClientH]->m_iCustomItemValue_Defense += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;


                        dV2 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
                        dV3 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                        dV1 = (double)(dV2 / 100.0f) * dV3;

                        dV1 = dV1 / 2.0f;

                        m_pClientList[iClientH]->m_iDefenseRatio += (int)dV1;
                        if (m_pClientList[iClientH]->m_iDefenseRatio <= 0) m_pClientList[iClientH]->m_iDefenseRatio = 1;

                        //testcode
                        //wsprintf(G_cTxt, "Custom-Defense: %d", (int)dV1);
                        //log->info(G_cTxt);
                    }


                    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                        dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;





                        switch (dwSWEType)
                        {
                            case 7:
                                m_pClientList[iClientH]->m_cAttackDiceRange_SM++;
                                m_pClientList[iClientH]->m_cAttackDiceRange_L++;
                                break;

                            case 9:
                                m_pClientList[iClientH]->m_cAttackDiceRange_SM += 2;
                                m_pClientList[iClientH]->m_cAttackDiceRange_L += 2;
                                break;

                                // v2.04 
                            case 11:
                                m_pClientList[iClientH]->m_iAddTransMana += dwSWEValue;
                                if (m_pClientList[iClientH]->m_iAddTransMana > 13) m_pClientList[iClientH]->m_iAddTransMana = 13;
                                break;

                            case 12:
                                m_pClientList[iClientH]->m_iAddChargeCritical += dwSWEValue;
                                if (m_pClientList[iClientH]->m_iAddChargeCritical > 20) m_pClientList[iClientH]->m_iAddChargeCritical = 20;
                                break;
                        }
                    }


                    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != 0)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
                        dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;


                        //Ãß°¡ µ¶¼ºÀúÇ×(1), Ãß°¡ ¸íÁß°ª(2), Ãß°¡ ¹æ¾î°ª(3), HP È¸º¹·® Ãß°¡(4), SP È¸º¹·® Ãß°¡(5)
                        //MP È¸º¹·® Ãß°¡(6), Ãß°¡ ¸¶¹ýÀúÇ×(7), ¹°¸® ´ë¹ÌÁö Èí¼ö(8), ¸¶¹ý ´ë¹ÌÁö Èí¼ö(9)
                        //¿¬Å¸ ´ë¹ÌÁö Ãß°¡(10), ´õ ¸¹Àº °æÇèÄ¡(11), ´õ¸¹Àº Gold(12)

                        switch (dwSWEType)
                        {
                            case 0:  break;
                            case 1:  m_pClientList[iClientH]->m_iAddPR += (int)dwSWEValue * 7; break;
                            case 2:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break;
                            case 3:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
                            case 4:  m_pClientList[iClientH]->m_iAddHP += (int)dwSWEValue * 7; break;
                            case 5:  m_pClientList[iClientH]->m_iAddSP += (int)dwSWEValue * 7; break;
                            case 6:  m_pClientList[iClientH]->m_iAddMP += (int)dwSWEValue * 7; break;
                            case 7:  m_pClientList[iClientH]->m_iAddMR += (int)dwSWEValue * 7; break;
                            case 8:  m_pClientList[iClientH]->m_iDamageAbsorption_Armor[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos] += (int)dwSWEValue * 3; break;
                            case 9:  m_pClientList[iClientH]->m_iAddAbsMD += (int)dwSWEValue * 3; break;
                            case 10: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
                            case 11: m_pClientList[iClientH]->m_iAddExp += (int)dwSWEValue * 10; break;
                            case 12: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
                        }


                        switch (dwSWEType)
                        {
                            case 9: if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80; break;
                        }
                    }

                    switch (cEquipPos)
                    {
                        case DEF_EQUIPPOS_LHAND:


                            m_pClientList[iClientH]->m_iDamageAbsorption_Shield = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) - (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) / 3;
                            break;
                        default:

                            m_pClientList[iClientH]->m_iDamageAbsorption_Armor[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
                            break;
                    }


                    switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
                    {
                        case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:

                            m_pClientList[iClientH]->m_iSpecialAbilityType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;

                            m_pClientList[iClientH]->m_iSpecialAbilityLastSec = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;

                            m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = (int)cEquipPos;


                            if ((bNotify == true) && (iEquipItemID == (int)sItemIndex))
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, 0);
                            break;
                    }
                    break;
            }
        }

    //v1.432
    if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType == 0) && (bNotify == true))
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 4, 0, 0, 0);

        if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == true)
        {
            m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = false;

            m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;

            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xFF0F;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
        }
    }

    if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType != 0) &&
        (iPrevSAType != m_pClientList[iClientH]->m_iSpecialAbilityType) && (bNotify == true))
    {

        if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == true)
        {

            SendNotifyMsg(NULL, i, DEF_NOTIFY_SPECIALABILITYSTATUS, 3, 0, 0, 0);
            m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = false;

            m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;

            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xFF0F;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
        }
    }
}

int CGame::_iGetPlayerNumberOnSpot(short dX, short dY, char cMapIndex, char cRange)
{
    int ix, iy, iSum = 0;
    short sOwnerH;
    char  cOwnerType;

    for (ix = dX - cRange; ix <= dX + cRange; ix++)
        for (iy = dY - cRange; iy <= dY + cRange; iy++)
        {
            m_pMapList[cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
            if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
                iSum++;
        }

    return iSum;
}


bool CGame::bAnalyzeCriminalAction(int iClientH, short dX, short dY, bool bIsCheck)
{
    int   iNamingValue, tX, tY;
    short sOwnerH;
    char  cOwnerType, cName[11], cNpcName[21];
    char  cNpcWaypoint[11];

    if (m_pClientList[iClientH] == 0) return false;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return false;
    if (m_bIsCrusadeMode == true) return false;


    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0))
    {
        if (_bGetIsPlayerHostile(iClientH, sOwnerH) != true)
        {

            if (bIsCheck == true) return true;

            memset(cNpcName, 0, sizeof(cNpcName));
            if (strcmp(m_pClientList[iClientH]->m_cMapName, "aresden") == 0)
                strcpy(cNpcName, "Guard-Aresden");
            else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvine") == 0)
                strcpy(cNpcName, "Guard-Elvine");
            else  if (strcmp(m_pClientList[iClientH]->m_cMapName, "default") == 0)
                strcpy(cNpcName, "Guard-Neutral");
            else strcpy(cNpcName, "XXX");

            iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
            if (iNamingValue == -1)
            {

            }
            else
            {

                memset(cNpcWaypoint, 0, sizeof(cNpcWaypoint));

                memset(cName, 0, sizeof(cName));
                wsprintf(cName, "XX%d", iNamingValue);
                cName[0] = '_';
                cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

                tX = (int)m_pClientList[iClientH]->m_sX;
                tY = (int)m_pClientList[iClientH]->m_sY;
                if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM,
                    &tX, &tY, cNpcWaypoint, 0, 0, -1, false, true) == false)
                {

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                }
                else
                {

                    bSetNpcAttackMode(cName, iClientH, DEF_OWNERTYPE_PLAYER, true);
                }
            }
            //
        }
    }

    return false;
}

bool CGame::_bGetIsPlayerHostile(int iClientH, int sOwnerH)
{
    if (m_pClientList[iClientH] == 0) return false;
    if (m_pClientList[sOwnerH] == 0) return false;


    if (iClientH == sOwnerH) return true;

    if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
    {


        if (m_pClientList[sOwnerH]->m_iPKCount != 0)
            return true;
        else return false;
    }
    else
    {

        if (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide)
        {

            if (m_pClientList[sOwnerH]->m_cSide == 0)
            {
                if (m_pClientList[sOwnerH]->m_iPKCount != 0)
                    return true;
                else return false;
            }
            else return true;
        }
        else
        {
            if (m_pClientList[sOwnerH]->m_iPKCount != 0)
                return true;
            else return false;
        }
    }

    return false;
}

void CGame::bSetNpcAttackMode(char * cName, int iTargetH, char cTargetType, bool bIsPermAttack)
{
    int i, iIndex;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if ((m_pNpcList[i] != 0) && (memcmp(m_pNpcList[i]->m_cName, cName, 5) == 0))
        {
            iIndex = i;
            goto NEXT_STEP_SNAM1;

            //testcode
            //log->info("bSetNpcAttackMode - Npc found");
        }

    return;

    NEXT_STEP_SNAM1:;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[iTargetH] == 0) return;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[iTargetH] == 0) return;
            break;
    }


    m_pNpcList[iIndex]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
    m_pNpcList[iIndex]->m_sBehaviorTurnCount = 0;
    m_pNpcList[iIndex]->m_iTargetIndex = iTargetH;
    m_pNpcList[iIndex]->m_cTargetType = cTargetType;


    m_pNpcList[iIndex]->m_bIsPermAttackMode = bIsPermAttack;

    //testcode
    //log->info("bSetNpcAttackMode - complete");
}


void CGame::PoisonEffect(int iClientH, int iV1)
{
    int iPoisonLevel, iDamage, iPrevHP, iProb;


    if (m_pClientList[iClientH] == 0)     return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    iPoisonLevel = m_pClientList[iClientH]->m_iPoisonLevel;

    iDamage = iDice(1, iPoisonLevel);

    iPrevHP = m_pClientList[iClientH]->m_iHP;
    m_pClientList[iClientH]->m_iHP -= iDamage;
    if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 1;

    if (iPrevHP != m_pClientList[iClientH]->m_iHP)
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);


    iProb = m_pClientList[iClientH]->m_cSkillMastery[23] - 10 + m_pClientList[iClientH]->m_iAddPR;
    if (iProb <= 10) iProb = 10;
    if (iDice(1, 100) <= iProb)
    {
        m_pClientList[iClientH]->m_bIsPoisoned = false;

        SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);
    }
}



bool CGame::bCheckResistingPoisonSuccess(short sOwnerH, char cOwnerType)
{
    int iResist, iResult;


    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return false;
            iResist = m_pClientList[sOwnerH]->m_cSkillMastery[23] + m_pClientList[sOwnerH]->m_iAddPR;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return false;
            iResist = 0;
            break;
    }

    iResult = iDice(1, 100);
    if (iResult >= iResist)
        return false;


    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
        CalculateSSN_SkillIndex(sOwnerH, 23, 1);

    return true;
}

bool CGame::bCheckBadWord(char * pString)
{
    char * cp;


    cp = pString;
    while (*cp != 0)
    {

        cp++;
    }

    return false;
}

void CGame::CheckDayOrNightMode()
{
    SYSTEMTIME SysTime;
    char cPrevMode;
    int  i;



    cPrevMode = m_cDayOrNight;

    GetLocalTime(&SysTime);
    if (SysTime.wMinute >= DEF_NIGHTTIME)
        m_cDayOrNight = 2;
    else m_cDayOrNight = 1;

    if (cPrevMode != m_cDayOrNight)
    {

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
            {
                if ((m_pClientList[i]->m_cMapIndex >= 0) &&
                    (m_pMapList[m_pClientList[i]->m_cMapIndex] != 0) &&
                    (m_pMapList[m_pClientList[i]->m_cMapIndex]->m_bIsFixedDayMode == false))
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TIMECHANGE, m_cDayOrNight, 0, 0, 0);
            }
    }
}

void CGame::NoticeHandler()
{
    char  cTemp, cBuffer[1000];
    uint32_t dwSize, dwTime = timeGetTime();
    int i, iMsgIndex, iTemp;


    if (m_iTotalNoticeMsg <= 1) return;

    if ((dwTime - m_dwNoticeTime) > DEF_NOTICETIME)
    {

        m_dwNoticeTime = dwTime;
        do
        {
            iMsgIndex = iDice(1, m_iTotalNoticeMsg) - 1;
        } while (iMsgIndex == m_iPrevSendNoticeMsg);

        m_iPrevSendNoticeMsg = iMsgIndex;

        memset(cBuffer, 0, sizeof(cBuffer));
        if (m_pNoticeMsgList[iMsgIndex] != 0)
        {
            m_pNoticeMsgList[iMsgIndex]->Get(&cTemp, cBuffer, &dwSize, &iTemp);
        }

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if (m_pClientList[i] != 0)
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cBuffer);
            }
    }
}


void CGame::ResponseSavePlayerDataReplyHandler(char * pData, uint32_t dwMsgSize)
{
    char * cp, cCharName[11];
    int i;

    memset(cCharName, 0, sizeof(cCharName));

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memcpy(cCharName, cp, 10);


    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != 0)
        {
            if (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0)
            {

                SendNotifyMsg(NULL, i, DEF_NOTIFY_SERVERCHANGE, 0, 0, 0, 0);
            }
        }
}

void CGame::CalcExpStock(int iClientH)
{
    bool bIsLevelUp;
    CItem * pItem;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_iExpStock <= 0) return;

    // !!!!

    //if ((m_pClientList[iClientH]->m_iLevel >= m_iPlayerMaxLevel) && (m_pClientList[iClientH]->m_iExp >= m_iLevelExpTable[m_iPlayerMaxLevel])) return;


    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD)
    {
        m_pClientList[iClientH]->m_iExpStock = 0;
        return;
    }


    m_pClientList[iClientH]->m_iExp += m_pClientList[iClientH]->m_iExpStock;
    m_pClientList[iClientH]->m_iAutoExpAmount += m_pClientList[iClientH]->m_iExpStock;
    m_pClientList[iClientH]->m_iExpStock = 0;

    if (bCheckLimitedUser(iClientH) == false)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
    }

    bIsLevelUp = bCheckLevelUp(iClientH);


    if ((bIsLevelUp == true) && (m_pClientList[iClientH]->m_iLevel <= 5))
    {
        // ÃÊº¸¿ë Gold Áö±Þ. ·¹º§ 1~5±îÁö 100 Gold Áö±Þ.
        pItem = new CItem;
        if (_bInitItemAttr(pItem, "Gold") == false)
        {
            delete pItem;
            return;
        }
        else pItem->m_dwCount = (DWORD)100;
        bAddItem(iClientH, pItem, 0);
    }

    if ((bIsLevelUp == true) && (m_pClientList[iClientH]->m_iLevel > 5) && (m_pClientList[iClientH]->m_iLevel <= 20))
    {
        // ÃÊº¸¿ë Gold Áö±Þ. ·¹º§ 5~20±îÁö 300 Gold Áö±Þ.
        pItem = new CItem;
        if (_bInitItemAttr(pItem, "Gold") == false)
        {
            delete pItem;
            return;
        }
        else pItem->m_dwCount = (DWORD)300;
        bAddItem(iClientH, pItem, 0);
    }

}

void CGame::___RestorePlayerRating(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_iRating < -10000) m_pClientList[iClientH]->m_iRating = 0;
    if (m_pClientList[iClientH]->m_iRating > 10000) m_pClientList[iClientH]->m_iRating = 0;
}

int CGame::iGetExpLevel(int iExp)
{
    int i;


    for (i = 1; i < 199; i++)
        if ((m_iLevelExpTable[i] <= iExp) && (m_iLevelExpTable[i + 1] > iExp)) return i;

    return 0;
}


// v1.4311-3 Ãß°¡¹× º¯°æ ÇÔ¼ö  ±æµå¿ø °­Åð ¸í·É void CGame::UserCommand_BanGuildsman


int CGame::iCreateFish(char cMapIndex, short sX, short sY, short sType, CItem * pItem, int iDifficulty, uint32_t dwLastTime)
{
    int i, iDynamicHandle;


    if ((cMapIndex < 0) || (cMapIndex >= DEF_MAXMAPS)) return 0;
    if (m_pMapList[cMapIndex] == 0) return 0;
    if (m_pMapList[cMapIndex]->bGetIsWater(sX, sY) == false) return 0;

    for (i = 1; i < DEF_MAXFISHS; i++)
        if (m_pFish[i] == 0)
        {

            m_pFish[i] = new CFish(cMapIndex, sX, sY, sType, pItem, iDifficulty);
            if (m_pFish[i] == 0) return 0;


            switch (pItem->m_sIDnum)
            {
                case 101:
                case 102:
                case 103:
                case 570:
                case 571:
                case 572:
                case 573:
                case 574:
                case 575:
                case 576:
                case 577:
                    iDynamicHandle = iAddDynamicObjectList(i, 0, DEF_DYNAMICOBJECT_FISH, cMapIndex, sX, sY, dwLastTime);
                    break;
                default:

                    iDynamicHandle = iAddDynamicObjectList(i, 0, DEF_DYNAMICOBJECT_FISHOBJECT, cMapIndex, sX, sY, dwLastTime);
                    break;
            }

            if (iDynamicHandle == 0)
            {
                delete m_pFish[i];
                m_pFish[i] = 0;
                return 0;
            }
            m_pFish[i]->m_sDynamicObjectHandle = iDynamicHandle;
            m_pMapList[cMapIndex]->m_iCurFish++;

            return i;
        }

    return 0;
}


bool CGame::bDeleteFish(int iHandle, int iDelMode)
{
    int i, iH;
    uint32_t dwTime;

    if (m_pFish[iHandle] == 0) return false;

    dwTime = timeGetTime();


    iH = m_pFish[iHandle]->m_sDynamicObjectHandle;

    if (m_pDynamicObjectList[iH] != 0)
    {
        SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iH]->m_cMapIndex, m_pDynamicObjectList[iH]->m_sX, m_pDynamicObjectList[iH]->m_sY, m_pDynamicObjectList[iH]->m_sType, iH, 0);

        m_pMapList[m_pDynamicObjectList[iH]->m_cMapIndex]->SetDynamicObject(NULL, 0, m_pDynamicObjectList[iH]->m_sX, m_pDynamicObjectList[iH]->m_sY, dwTime);
        m_pMapList[m_pDynamicObjectList[iH]->m_cMapIndex]->m_iCurFish--;

        delete m_pDynamicObjectList[iH];
        m_pDynamicObjectList[iH] = 0;
    }


    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) &&
            (m_pClientList[i]->m_iAllocatedFish == iHandle))
        {

            SendNotifyMsg(NULL, i, DEF_NOTIFY_FISHCANCELED, iDelMode, 0, 0, 0);
            ClearSkillUsingStatus(i);
        }
    }


    delete m_pFish[iHandle];
    m_pFish[iHandle] = 0;

    return true;
}


int CGame::iCheckFish(int iClientH, char cMapIndex, short dX, short dY)
{
    int i;
    short sDistX, sDistY;

    if (m_pClientList[iClientH] == 0) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

    if ((cMapIndex < 0) || (cMapIndex >= DEF_MAXMAPS)) return 0;


    for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != 0)
        {
            sDistX = abs(m_pDynamicObjectList[i]->m_sX - dX);
            sDistY = abs(m_pDynamicObjectList[i]->m_sY - dY);

            if ((m_pDynamicObjectList[i]->m_cMapIndex == cMapIndex) &&
                ((m_pDynamicObjectList[i]->m_sType == DEF_DYNAMICOBJECT_FISH) || (m_pDynamicObjectList[i]->m_sType == DEF_DYNAMICOBJECT_FISHOBJECT)) &&
                (sDistX <= 2) && (sDistY <= 2))
            {


                if (m_pFish[m_pDynamicObjectList[i]->m_sOwner] == 0) return 0;
                if (m_pFish[m_pDynamicObjectList[i]->m_sOwner]->m_sEngagingCount >= DEF_MAXENGAGINGFISH) return 0;


                if (m_pClientList[iClientH]->m_iAllocatedFish != 0) return 0;
                if (m_pClientList[iClientH]->m_cMapIndex != cMapIndex) return 0;

                m_pClientList[iClientH]->m_iAllocatedFish = m_pDynamicObjectList[i]->m_sOwner;
                m_pClientList[iClientH]->m_iFishChance = 1;

                m_pClientList[iClientH]->m_bSkillUsingStatus[1] = true;

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EVENTFISHMODE, (m_pFish[m_pDynamicObjectList[i]->m_sOwner]->m_pItem->m_wPrice / 2), m_pFish[m_pDynamicObjectList[i]->m_sOwner]->m_pItem->m_sSprite,
                    m_pFish[m_pDynamicObjectList[i]->m_sOwner]->m_pItem->m_sSpriteFrame, m_pFish[m_pDynamicObjectList[i]->m_sOwner]->m_pItem->m_cName);


                m_pFish[m_pDynamicObjectList[i]->m_sOwner]->m_sEngagingCount++;

                return i;
            }
        }

    return 0;
}

void CGame::FishProcessor()
{
    int i, iSkillLevel, iResult, iChangeValue;


    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) &&
            (m_pClientList[i]->m_iAllocatedFish != 0))
        {

            if (m_pFish[m_pClientList[i]->m_iAllocatedFish] == 0) break;



            iSkillLevel = m_pClientList[i]->m_cSkillMastery[1];

            iSkillLevel -= m_pFish[m_pClientList[i]->m_iAllocatedFish]->m_iDifficulty;
            if (iSkillLevel <= 0) iSkillLevel = 1;

            iChangeValue = iSkillLevel / 10;
            if (iChangeValue <= 0) iChangeValue = 1;
            iChangeValue = iDice(1, iChangeValue);

            iResult = iDice(1, 100);
            if (iSkillLevel > iResult)
            {

                m_pClientList[i]->m_iFishChance += iChangeValue;
                if (m_pClientList[i]->m_iFishChance > 99) m_pClientList[i]->m_iFishChance = 99;

                SendNotifyMsg(NULL, i, DEF_NOTIFY_FISHCHANCE, m_pClientList[i]->m_iFishChance, 0, 0, 0);
            }
            else if (iSkillLevel < iResult)
            {

                m_pClientList[i]->m_iFishChance -= iChangeValue;
                if (m_pClientList[i]->m_iFishChance < 1) m_pClientList[i]->m_iFishChance = 1;

                SendNotifyMsg(NULL, i, DEF_NOTIFY_FISHCHANCE, m_pClientList[i]->m_iFishChance, 0, 0, 0);
            }
        }
    }
}






void CGame::FishGenerator()
{
    CItem * pItem;
    int i, iP, tX, tY, iRet;
    int iItemID = 0;
    short sDifficulty;
    uint32_t dwLastTime;

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        if ((iDice(1, 4) == 1) && (m_pMapList[i] != 0) &&
            (m_pMapList[i]->m_iCurFish < m_pMapList[i]->m_iMaxFish))
        {

            iP = iDice(1, m_pMapList[i]->m_iTotalFishPoint) - 1;
            if ((m_pMapList[i]->m_FishPointList[iP].x == -1) || (m_pMapList[i]->m_FishPointList[iP].y == -1)) break;

            tX = m_pMapList[i]->m_FishPointList[iP].x + (iDice(1, 3) - 2);
            tY = m_pMapList[i]->m_FishPointList[iP].y + (iDice(1, 3) - 2);

            pItem = new CItem;
            if (pItem == 0) break;



            switch (iDice(1, 9))
            {
                case 1:	iItemID = 570; sDifficulty = iDice(1, 10) + 5; break;
                case 2:	iItemID = 571; sDifficulty = iDice(1, 5) + 15;  break;
                case 3:	iItemID = 572; sDifficulty = iDice(1, 10) + 20; break;
                case 4:	iItemID = 573; sDifficulty = 1;  break;
                case 5:	iItemID = 574; sDifficulty = iDice(1, 15) + 1;  break;
                case 6:	iItemID = 576; sDifficulty = iDice(1, 18) + 1;  break;
                case 7:	iItemID = 575; sDifficulty = iDice(1, 12) + 1;  break;
                case 8:	iItemID = 577; sDifficulty = iDice(1, 10) + 1;  break;
                case 9:

                    switch (iDice(1, 150))
                    {
                        case 1:
                        case 2:
                        case 3:
                            iItemID = 390;
                            sDifficulty = iDice(4, 4) + 20;
                            break;

                        case 10:
                        case 11:
                            iItemID = 391;
                            sDifficulty = iDice(4, 4) + 40;
                            break;

                        case 20:
                            iItemID = 5;
                            sDifficulty = iDice(4, 4) + 5;
                            break;

                        case 30:
                            iItemID = 19;
                            sDifficulty = iDice(4, 4) + 10;
                            break;

                        case 40:
                            iItemID = 27;
                            sDifficulty = iDice(4, 4) + 15;
                            break;

                        case 50:
                            iItemID = 36;
                            sDifficulty = iDice(4, 4) + 35;
                            break;

                        case 60:
                            iItemID = 56;
                            sDifficulty = iDice(4, 4) + 40;
                            break;

                        case 70:
                            iItemID = 73;
                            sDifficulty = iDice(4, 4) + 30;
                            break;

                        case 90:
                            iItemID = 351;  // ·çºñ 
                            sDifficulty = iDice(4, 4) + 30;
                            break;

                        case 95:
                            iItemID = 350;
                            sDifficulty = iDice(4, 4) + 30;
                            break;
                    }
                    break;
            }

            dwLastTime = (60000 * 10) + (iDice(1, 3) - 1) * (60000 * 10);

            if (_bInitItemAttr(pItem, iItemID) == true)
            {
                iRet = iCreateFish(i, tX, tY, 1, pItem, sDifficulty, dwLastTime);
            }
            else
            {
                delete pItem;
                pItem = 0;
            }
        }
    }
}


int CGame::_iCalcPlayerNum(char cMapIndex, short dX, short dY, char cRadius)
{
    int ix, iy, iRet;
    CTile * pTile;


    if ((cMapIndex < 0) || (cMapIndex > DEF_MAXMAPS)) return 0;
    if (m_pMapList[cMapIndex] == 0) return 0;

    iRet = 0;
    for (ix = dX - cRadius; ix <= dX + cRadius; ix++)
        for (iy = dY - cRadius; iy <= dY + cRadius; iy++)
        {
            if ((ix < 0) || (ix >= m_pMapList[cMapIndex]->m_sSizeX) ||
                (iy < 0) || (iy >= m_pMapList[cMapIndex]->m_sSizeY))
            {

            }
            else
            {
                pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy * m_pMapList[cMapIndex]->m_sSizeY);
                if ((pTile->m_sOwner != 0) && (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER))
                    iRet++;
            }
        }

    return iRet;
}


void CGame::WhetherProcessor()
{
    char cPrevMode;
    int i, j;
    uint32_t dwTime;

    dwTime = timeGetTime();

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        if ((m_pMapList[i] != 0) && (m_pMapList[i]->m_bIsFixedDayMode == false))
        {

            cPrevMode = m_pMapList[i]->m_cWhetherStatus;

            if (m_pMapList[i]->m_cWhetherStatus != 0)
            {

                if ((dwTime - m_pMapList[i]->m_dwWhetherStartTime) > m_pMapList[i]->m_dwWhetherLastTime)
                    m_pMapList[i]->m_cWhetherStatus = 0;
            }
            else
            {


                if (iDice(1, 300) == 13)
                {

#if defined(DEF_WINTER)
                    m_pMapList[i]->m_cWhetherStatus = iDice(1, 3) + 3; // 1~3 ºñ 4~6 : Snow
#else
                    m_pMapList[i]->m_cWhetherStatus = iDice(1, 3); // 1~3 ºñ 4~6 : Snow
#endif
                    m_pMapList[i]->m_dwWhetherStartTime = dwTime;
                    m_pMapList[i]->m_dwWhetherLastTime = 60000 * 3 + 60000 * iDice(1, 7);
                }
            }


            if (m_pMapList[i]->m_bIsFixedSnowMode == true)
            {
                m_pMapList[i]->m_cWhetherStatus = iDice(1, 3) + 3;
                m_pMapList[i]->m_dwWhetherStartTime = dwTime;
                m_pMapList[i]->m_dwWhetherLastTime = 60000 * 3 + 60000 * iDice(1, 7);
            }

            if (cPrevMode != m_pMapList[i]->m_cWhetherStatus)
            {

                for (j = 1; j < DEF_MAXCLIENTS; j++)
                    if ((m_pClientList[j] != 0) && (m_pClientList[j]->m_bIsInitComplete == true) && (m_pClientList[j]->m_cMapIndex == i))
                        SendNotifyMsg(NULL, j, DEF_NOTIFY_WHETHERCHANGE, m_pMapList[i]->m_cWhetherStatus, 0, 0, 0);
            }

        }
    }
}

// v1.4311-3 Ãß°¡ ÇÔ¼ö  »çÅõÀå ¿¹¾à ÃÊ±âÈ­¹× »ç¿ëÀÚ¿¡°Ô ¿¹¾àÀÌ Ãë¼Ò µÇ¾úÀ½ ¾Ë¸°´Ù.FightzoneReserveProcessor 
void CGame::FightzoneReserveProcessor()
{
    int i;

    // v2.02 ·Î±× »èÁ¦ 
    //	log->info("CAN FIGHTZONE RESERVE!!");


    for (i = 0; i < DEF_MAXFIGHTZONE; i++)
    {

        if (m_iFightZoneReserve[i] == -1) continue;
        m_iFightZoneReserve[i] = 0;
    }

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {


        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iFightzoneNumber != 0))
            SendNotifyMsg(NULL, i, DEF_NOTIFY_FIGHTZONERESERVE, -2, 0, 0, 0);
    }
}




int CGame::iGetWhetherMagicBonusEffect(short sType, char cWheatherStatus)
{
    int iWheatherBonus;

    iWheatherBonus = 0;
    switch (cWheatherStatus)
    {
        case 0: break;
        case 1:
        case 2:
        case 3:

            switch (sType)
            {
                case 10:
                case 37:
                case 43:
                case 51:
                    //iWheatherBonus = 2*cWheatherStatus;
                    iWheatherBonus = 1;
                    break;

                case 20:
                case 30:
                    //iWheatherBonus = -2*cWheatherStatus;
                    iWheatherBonus = -1;
                    break;
            }
            break;
    }

    return iWheatherBonus;
}


int CGame::iGetPlayerRelationship(int iClientH, int iOpponentH)
{
    int iRet;

    if (m_pClientList[iClientH] == 0) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

    if (m_pClientList[iOpponentH] == 0) return 0;
    if (m_pClientList[iOpponentH]->m_bIsInitComplete == false) return 0;

    iRet = 0;

    if (m_pClientList[iClientH]->m_iPKCount != 0)
    {

        if ((m_pClientList[iClientH]->m_cSide == m_pClientList[iOpponentH]->m_cSide) &&
            (m_pClientList[iClientH]->m_cSide != DEF_NETURAL) && (m_pClientList[iOpponentH]->m_cSide != DEF_NETURAL))
            iRet = 7;
        else iRet = 2;
    }
    else if (m_pClientList[iOpponentH]->m_iPKCount != 0)
    {

        if ((m_pClientList[iClientH]->m_cSide == m_pClientList[iOpponentH]->m_cSide) &&
            (m_pClientList[iClientH]->m_cSide != DEF_NETURAL) && (m_pClientList[iOpponentH]->m_cSide != DEF_NETURAL))
            iRet = 6;
        else iRet = 2;
    }
    else
    {
        if (m_pClientList[iClientH]->m_cSide != m_pClientList[iOpponentH]->m_cSide)
        {
            if ((m_pClientList[iClientH]->m_cSide != DEF_NETURAL) && (m_pClientList[iOpponentH]->m_cSide != DEF_NETURAL))
            {

                iRet = 2;
            }
            else
            {
                iRet = 0;
            }
        }
        else
        {

            if ((memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[iOpponentH]->m_cGuildName, 20) == 0) &&
                (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0))
            {

                if (m_pClientList[iOpponentH]->m_iGuildRank == 0)
                    iRet = 5;
                else iRet = 3;
            }
            else
                if ((m_pClientList[iClientH]->m_cSide == m_pClientList[iOpponentH]->m_cSide) &&
                    (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0) &&
                    (memcmp(m_pClientList[iOpponentH]->m_cGuildName, "NONE", 4) != 0) &&
                    (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[iOpponentH]->m_cGuildName, 20) != 0))
                {

                    iRet = 4;
                }
                else iRet = 1;
        }
    }

    return iRet;
}


// 2002-11-14
/* int CGame::iGetPlayerABSStatus(int iWhatH, int iRecvH)
{
// Àý?ëÀûÀÎ Ä???ÅÍÀÇ ?Ò?ÓÀ? ?ò?Â?Ù.

//		0: ???Ò?Ó

//		1: ?Æ???ºµ? ?Ò?Ó
//		2: ?Æ???ºµ? ?üÁËÀÚ
//		3: ?Æ???ºµ? ??Àº ?æµå?ø
//		4: ?Æ???ºµ? ?Ù?? ?æµå?ø
//		5: ?Æ???ºµ? ??Àº ?æµå???ºÅÍ
//		6: ?Æ???ºµ? ?Ù?? ?æµå???ºÅÍ

//		8:  ???ÙÀÎ ?Ò?Ó
//		9:  ???ÙÀÎ ?üÁËÀÚ
//		10: ???ÙÀÎ ??Àº ?æµå?ø
//		11: ???ÙÀÎ ?Ù?? ?æµå?ø
//		12: ???ÙÀÎ ??Àº ?æµå???ºÅÍ
//		13: ???ÙÀÎ ?Ù?? ?æµå???ºÅÍ

//		15: ???Ò?Ó ?üÁËÀÚ





if (m_pClientList[iWhatH] == 0) return 0;
if (m_pClientList[iRecvH] == 0) return 0;

if (memcmp(m_pClientList[iWhatH]->m_cLocation, "NONE", 4) == 0) {
// ???Ò?ÓÀÌ?Ù.
if (m_pClientList[iWhatH]->m_iPKCount != 0) {
// ???Ò?Ó ?üÁËÀÚ
return 15;
}
else return 0;
}
else if (memcmp(m_pClientList[iWhatH]->m_cLocation, "aresden", 7) == 0) {
// ?Æ???ºµ? ?Ò?Ó
if (m_pClientList[iWhatH]->m_iPKCount != 0) {
// ?Æ???ºµ?ÀÇ ?üÁËÀÚ
return 2;
}
else {
if (m_pClientList[iWhatH]->m_iGuildRank == 0) {
// ?Æ???ºµ? ?æµå???ºÅÍ
if (memcmp(m_pClientList[iWhatH]->m_cGuildName, m_pClientList[iRecvH]->m_cGuildName, 20) == 0) {
// ?? ?æµå ???ºÅÍ
return 5;
}
else {
// ?Ù?? ?æµå ???ºÅÍ
// v1.44 ?çÅõÀåÀÌ?é ?Ù?? ?æµå?Â ÀûÀÌ?Ù.
if (m_pMapList[m_pClientList[iRecvH]->m_cMapIndex]->m_bIsFightZone == true)
return 2;
else return 6;
}
}
else if (m_pClientList[iWhatH]->m_iGuildRank == 12) {
// ?Æ???ºµ? ?æµå?ø
if (memcmp(m_pClientList[iWhatH]->m_cGuildName, m_pClientList[iRecvH]->m_cGuildName, 20) == 0) {
// ??Àº ?æµå?ø
return 3;
}
else {
// ?Ù?? ?æµå?ø
// v1.44 ?çÅõÀåÀÌ?é ?Ù?? ?æµå?Â ÀûÀÌ?Ù.
if (m_pMapList[m_pClientList[iRecvH]->m_cMapIndex]->m_bIsFightZone == true)
return 2;
else return 4;
}
}
else {
// ?Æ???ºµ? ?Ã?Î
return 1;
}
}
}
else if (memcmp(m_pClientList[iWhatH]->m_cLocation, "elvine", 6) == 0) {
// ???ÙÀÎ ?Ò?Ó
if (m_pClientList[iWhatH]->m_iPKCount != 0) {
// ???ÙÀÎÀÇ ?üÁËÀÚ
return 9;
}
else {
if (m_pClientList[iWhatH]->m_iGuildRank == 0) {
// ???ÙÀÎ ?æµå???ºÅÍ
if (memcmp(m_pClientList[iWhatH]->m_cGuildName, m_pClientList[iRecvH]->m_cGuildName, 20) == 0) {
// ?? ?æµå ???ºÅÍ
return 12;
}
else {
// ?Ù?? ?æµå ???ºÅÍ
// v1.44 ?çÅõÀåÀÌ?é ?Ù?? ?æµå?Â ÀûÀÌ?Ù.
if (m_pMapList[m_pClientList[iRecvH]->m_cMapIndex]->m_bIsFightZone == true)
return 9;
else return 13;
}
}
else if (m_pClientList[iWhatH]->m_iGuildRank == 12) {
// ???ÙÀÎ ?æµå?ø
if (memcmp(m_pClientList[iWhatH]->m_cGuildName, m_pClientList[iRecvH]->m_cGuildName, 20) == 0) {
// ??Àº ?æµå?ø
return 10;
}
else {
// ?Ù?? ?æµå?ø
// v1.44 ?çÅõÀåÀÌ?é ?Ù?? ?æµå?Â ÀûÀÌ?Ù.
if (m_pMapList[m_pClientList[iRecvH]->m_cMapIndex]->m_bIsFightZone == true)
return 9;
else return 11;
}
}
else {
// ???ÙÀÎ ?Ã?Î
return 8;
}
}
}

return 0;


}
*/
// v2.19 2002-12-2  ?çÅõÀå?????Â ?Ù?? ?æµå?Â ÀûÀ??Î ÆÇ?ÜÇØ?ß ÇÑ?Ù.
int CGame::iGetPlayerABSStatus(int iWhatH, int iRecvH)
{



    //
    // |1|2|3|4|
    //

    // 2 : ?Ã?Î(1) / Áß??(0)



    if (m_pClientList[iWhatH] == 0) return 0;
    if (m_pClientList[iRecvH] == 0) return 0;

    int	rtn = 0x0000;


    if (m_pMapList[m_pClientList[iRecvH]->m_cMapIndex]->m_bIsFightZone == true)
    {
        if ((m_pClientList[iWhatH]->m_iGuildRank != -1) && (m_pClientList[iRecvH]->m_iGuildRank != -1))
        {
            if (memcmp(m_pClientList[iWhatH]->m_cGuildName, m_pClientList[iRecvH]->m_cGuildName, 20) != 0)
            {
                rtn = 0x0001 << 3;
            }
        }
    }

    if (m_pClientList[iWhatH]->m_iPKCount != 0)
        rtn = 0x0001 << 3;


    // ?Ã?Î / Áß??
    if (m_pClientList[iWhatH]->m_cSide != DEF_NETURAL)
        rtn |= 0x0001 << 2;


    if (m_pClientList[iWhatH]->m_cSide == DEF_ARESDEN)
        rtn |= 0x0001 << 1;


    if (m_pClientList[iWhatH]->m_bIsHunter == true)
        rtn |= 0x0001;

    return rtn;
}
/*

int CGame::iGetPlayerABSStatus(int iWhatH)
{
// 2002-11-14 ?ç?É?Û ?ðµå Ãß??
// 2002-11-14 º?ÇüµÊ
// 4??ÀÇ bit?? µû?ó ÀÇ?Ì?? ºÎ??ÇÑ?Ù.
//
// |1|2|3|4|
//
// 1 : ?üÁËÀÚ À?(1) / ??(0)
// 2 : ?Ã?Î(1) / Áß??(0)
// 3 : ?Æ???ºµ?(1) / ???ÙÀÎ(0)
// 4 : ?ç?É?Û(1) / ÀÏ?Ý(0)

if (m_pClientList[iWhatH] == 0) return 0;

int	rtn=0x0000;

// ?üÁËÀÚ
if ( m_pClientList[iWhatH]->m_iPKCount != 0 )
rtn  = 0x0001 << 3;

// ?Ã?Î / Áß??
if( m_pClientList[iWhatH]->m_cSide != DEF_NETURAL )
rtn |= 0x0001 << 2;

// ??À?
if( m_pClientList[iWhatH]->m_cSide == DEF_ARESDEN  )
rtn |= 0x0001 << 1;

// ?ç?É?Û / ÀÏ?Ý
if ( m_pClientList[iWhatH]->m_bIsHunter == true )
rtn |= 0x0001;

return rtn;
}

*/
int CGame::iGetNpcRelationship(int iWhatH, int iRecvH)
{
    if (m_pClientList[iRecvH] == 0) return 0;
    if (m_pNpcList[iWhatH] == 0) return 0;

    int rtn = 0x0000;

    switch (m_pNpcList[iWhatH]->m_cSide)
    {
        case 0:	// Áß??
            break;

        case 10:
            rtn |= 0x0001 << 3;
            break;

        case 1:
            rtn |= 0x0001 << 2;
            rtn |= 0x0001 << 1;
            break;

        case 2:
            rtn |= 0x0001 << 2;
            break;
    }

    return rtn;
}


/*
int CGame::iGetNpcRelationship(int iClientH, int iOpponentH)
{
int iRet;

// ÇÃ??ÀÌ?î?Í NPCÀÇ ?ü?è?? ??ÇØ NPC???Ô ÁØ?Ù.
if (m_pClientList[iClientH] == 0) return 0;
if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

if (m_pNpcList[iOpponentH] == 0) return 0;

iRet = 0;

if (m_pClientList[iClientH]->m_iPKCount != 0) {
// º?ÀÎÀÌ ?üÁËÀÚÀÌÇÏ?é NPC?Â ?ðµÎ ÀûÀÌ?Ù.
iRet = 2;
}
else {
// º?ÀÎÀº ???íÇÏ?Ù.
if (m_pNpcList[iOpponentH]->m_cSide == 0)
iRet = 0;
else
if (m_pNpcList[iOpponentH]->m_cSide == 10)
iRet = 2;
else if (m_pClientList[iClientH]->m_cSide == m_pNpcList[iOpponentH]->m_cSide)
iRet = 1;
else if (m_pClientList[iClientH]->m_cSide == 0)
iRet = 0;
else iRet = 2;
}

return iRet;
}
*/

// 2002-11-15 ÀçÀÛ?º
int CGame::iGetPlayerRelationship_SendEvent(int iClientH, int iOpponentH)
{
    int iRet;



    if (m_pClientList[iClientH] == 0) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

    if (m_pClientList[iOpponentH] == 0) return 0;
    if (m_pClientList[iOpponentH]->m_bIsInitComplete == false) return 0;

    iRet = 0;

    if (m_pClientList[iClientH]->m_iPKCount != 0)
    {

        if ((m_pClientList[iClientH]->m_cSide != m_pClientList[iOpponentH]->m_cSide) &&
            (m_pClientList[iClientH]->m_cSide != DEF_NETURAL) && (m_pClientList[iOpponentH]->m_cSide != DEF_NETURAL))
            iRet = 2;
        else iRet = 6; // PK 
    }
    else if (m_pClientList[iOpponentH]->m_iPKCount != 0)
    {

        if ((m_pClientList[iClientH]->m_cSide != m_pClientList[iOpponentH]->m_cSide) &&
            (m_pClientList[iClientH]->m_cSide != DEF_NETURAL) && (m_pClientList[iOpponentH]->m_cSide != DEF_NETURAL))
            iRet = 2;
        else
        {
            iRet = 7;
        }
    }
    else
    {

        if (m_pClientList[iClientH]->m_cSide != m_pClientList[iOpponentH]->m_cSide)
        {

            if ((m_pClientList[iClientH]->m_cSide != DEF_NETURAL) && (m_pClientList[iOpponentH]->m_cSide != DEF_NETURAL)
                && ((m_pClientList[iClientH]->m_bIsHunter != true))
                )
            {

                iRet = 2;
            }
            else
            {

                iRet = 0;
            }
        }
        else
        {

            if ((m_pClientList[iClientH]->m_cSide == m_pClientList[iOpponentH]->m_cSide) &&
                (m_pClientList[iClientH]->m_cSide == DEF_NETURAL))
            {
                iRet = 0;
            }
            else
                if ((memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[iOpponentH]->m_cGuildName, 20) == 0) &&
                    (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0))
                {

                    if (m_pClientList[iClientH]->m_iGuildRank == 0)
                        iRet = 5;
                    else iRet = 3;
                }
                else
                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iOpponentH]->m_cLocation, 10) == 0) &&
                        (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0) &&
                        (memcmp(m_pClientList[iOpponentH]->m_cGuildName, "NONE", 4) != 0) &&
                        (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[iOpponentH]->m_cGuildName, 20) != 0))
                    {

                        iRet = 4;
                    }
                    else
                    {
                        iRet = 1;
                    }
        }
    }

    return iRet;
}


int CGame::iGetNpcRelationship_SendEvent(int iNpcH, int iOpponentH)
{
    int iRet;



    if (m_pClientList[iOpponentH] == 0) return 0;
    if (m_pClientList[iOpponentH]->m_bIsInitComplete == false) return 0;

    if (m_pNpcList[iNpcH] == 0) return 0;

    iRet = 0;

    if (m_pClientList[iOpponentH]->m_iPKCount != 0)
    {

        if (m_pNpcList[iNpcH]->m_cSide == m_pClientList[iOpponentH]->m_cSide)
            iRet = 7;
        else iRet = 2;
    }
    else
    {
        if (m_pNpcList[iNpcH]->m_cSide != m_pClientList[iOpponentH]->m_cSide)
        {

            if (m_pNpcList[iNpcH]->m_cSide == 10)
                iRet = 2;
            else
                if (m_pNpcList[iNpcH]->m_cSide == 0) iRet = 0;
                else
                    if (m_pClientList[iOpponentH]->m_cSide == 0)
                        iRet = 0;
                    else iRet = 2;

        }
        else iRet = 1;
    }

    return iRet;
}


int CGame::iGetMapIndex(char * pMapName)
{
    int i, iMapIndex;
    char cTmpName[256];

    memset(cTmpName, 0, sizeof(cTmpName));
    strcpy(cTmpName, pMapName);

    iMapIndex = -1;
    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0)
        {
            if (memcmp(m_pMapList[i]->m_cName, pMapName, 10) == 0)
                iMapIndex = i;
        }

    return iMapIndex;
}



int CGame::_iForcePlayerDisconect(int iNum)
{
    int i, iCnt;

    iCnt = 0;
    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != 0)
        {
            if (m_pClientList[i]->m_bIsInitComplete == true)
                DeleteClient(i, true, true);
            else DeleteClient(i, false, false);
            iCnt++;
            if (iCnt >= iNum) break;
        }

    return iCnt;
}



void CGame::ForceDisconnectAccount(char * pAccountName, uint16_t wCount)
{
    int i;


    if (wCount > 10) wCount = 10;


    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cAccountName, pAccountName, 10) == 0))
        {
            wsprintf(G_cTxt, "<%d> Force disconnect account: CharName(%s) AccntName(%s) Count(%d)", i, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cAccountName, wCount);
            log->info(G_cTxt);


            //DeleteClient(i, true, true);

            //v1.4312
            SendNotifyMsg(NULL, i, DEF_NOTIFY_FORCEDISCONN, wCount, 0, 0, 0);
        }
}


bool CGame::bOnClose()
{
    if (m_bIsServerShutdowned == false)
        if (MessageBox(NULL, "Player data not saved! Shutdown server now?", m_cServerName, MB_ICONEXCLAMATION | MB_YESNO) == IDYES) return true;
        else return false;
    else return true;

    return false;
}

void CGame::_CheckAttackType(int iClientH, short * spType)
{
    uint16_t wType;

    if (m_pClientList[iClientH] == 0) return;
    wType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);

    switch (*spType)
    {
        case 2:

            if (m_pClientList[iClientH]->m_cArrowIndex == -1) *spType = 0;

            if (wType < 40) *spType = 1;
            break;

        case 20:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
            if (m_pClientList[iClientH]->m_cSkillMastery[5] < 100) *spType = 1;
            break;

        case 21:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
            if (m_pClientList[iClientH]->m_cSkillMastery[7] < 100) *spType = 1;
            break;

        case 22:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
            if (m_pClientList[iClientH]->m_cSkillMastery[9] < 100) *spType = 1;
            break;

        case 23:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)   *spType = 1;
            if (m_pClientList[iClientH]->m_cSkillMastery[8] < 100) *spType = 1;
            break;

        case 24:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
            if (m_pClientList[iClientH]->m_cSkillMastery[10] < 100) *spType = 1;
            break;

        case 25:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 2;
            if (m_pClientList[iClientH]->m_cSkillMastery[6] < 100) *spType = 2;
            if (m_pClientList[iClientH]->m_cArrowIndex == -1)      *spType = 0;

            if (wType < 40) *spType = 1;
            break;
        case 26:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
            if (m_pClientList[iClientH]->m_cSkillMastery[14] < 100) *spType = 1;
            break;

        case 27:

            if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
            if (m_pClientList[iClientH]->m_cSkillMastery[21] < 100) *spType = 1;
            break;
    }
}



void CGame::MineralGenerator()
{
    int i, iP, tX, tY, iRet;

    for (i = 0; i < DEF_MAXMAPS; i++)
    {

        if ((iDice(1, 6) == 1) && (m_pMapList[i] != 0) &&
            (m_pMapList[i]->m_bMineralGenerator == true) &&
            (m_pMapList[i]->m_iCurMineral < m_pMapList[i]->m_iMaxMineral))
        {

            iP = iDice(1, m_pMapList[i]->m_iTotalMineralPoint) - 1;
            if ((m_pMapList[i]->m_MineralPointList[iP].x == -1) || (m_pMapList[i]->m_MineralPointList[iP].y == -1)) break;

            tX = m_pMapList[i]->m_MineralPointList[iP].x;
            tY = m_pMapList[i]->m_MineralPointList[iP].y;

            iRet = iCreateMineral(i, tX, tY, m_pMapList[i]->m_cMineralGeneratorLevel);
        }
    }
}

int CGame::iCreateMineral(char cMapIndex, int tX, int tY, char cLevel)
{
    int i, iDynamicHandle, iMineralType;

    if ((cMapIndex < 0) || (cMapIndex >= DEF_MAXMAPS)) return 0;
    if (m_pMapList[cMapIndex] == 0) return 0;

    for (i = 1; i < DEF_MAXMINERALS; i++)
        if (m_pMineral[i] == 0)
        {

            iMineralType = iDice(1, cLevel);
            m_pMineral[i] = new CMineral(iMineralType, cMapIndex, tX, tY, 1);
            if (m_pMineral[i] == 0) return 0;

            iDynamicHandle = 0;
            switch (iMineralType)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                    iDynamicHandle = iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_MINERAL1, cMapIndex, tX, tY, 0, i);
                    break;

                case 5: // º¸¼®·ù 
                case 6:
                    iDynamicHandle = iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_MINERAL2, cMapIndex, tX, tY, 0, i);
                    break;

                default:

                    iDynamicHandle = iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_MINERAL1, cMapIndex, tX, tY, 0, i);
                    break;
            }

            if (iDynamicHandle == 0)
            {
                delete m_pMineral[i];
                m_pMineral[i] = 0;
                return 0;
            }
            m_pMineral[i]->m_sDynamicObjectHandle = iDynamicHandle;
            m_pMineral[i]->m_cMapIndex = cMapIndex;


            switch (iMineralType)
            {
                case 1: m_pMineral[i]->m_iDifficulty = 10; m_pMineral[i]->m_iRemain = 20; break;
                case 2: m_pMineral[i]->m_iDifficulty = 15; m_pMineral[i]->m_iRemain = 15; break;
                case 3: m_pMineral[i]->m_iDifficulty = 20; m_pMineral[i]->m_iRemain = 10; break;
                case 4: m_pMineral[i]->m_iDifficulty = 50; m_pMineral[i]->m_iRemain = 8; break;
                case 5: m_pMineral[i]->m_iDifficulty = 70; m_pMineral[i]->m_iRemain = 6; break;
                case 6: m_pMineral[i]->m_iDifficulty = 90; m_pMineral[i]->m_iRemain = 4; break;
                default: m_pMineral[i]->m_iDifficulty = 10; m_pMineral[i]->m_iRemain = 20; break;
            }


            m_pMapList[cMapIndex]->m_iCurMineral++;

            return i;
        }

    return 0;
}


void CGame::_CheckMiningAction(int iClientH, int dX, int dY)
{
    short sType;
    uint32_t dwRegisterTime;
    int   iDynamicIndex, iSkillLevel, iResult;
    int   iMineralID;
    CItem * pItem;
    uint16_t  wWeaponType;

    if (m_pClientList[iClientH] == 0)  return;


    if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
    {
        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
    }


    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetDynamicObject(dX, dY, &sType, &dwRegisterTime, &iDynamicIndex);

    switch (sType)
    {
        case DEF_DYNAMICOBJECT_MINERAL1:
        case DEF_DYNAMICOBJECT_MINERAL2:


            wWeaponType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);
            if (wWeaponType == 25)
            {

            }
            else return;


            if ((m_pClientList[iClientH]->m_sAppr2 & 0xF000) == 0) return;

            iSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[0];
            if (iSkillLevel == 0) break;

            if (m_pDynamicObjectList[iDynamicIndex] == 0) break;

            iSkillLevel -= m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_iDifficulty;
            if (iSkillLevel <= 0) iSkillLevel = 1;

            iResult = iDice(1, 100);
            if (iResult <= iSkillLevel)
            {

                CalculateSSN_SkillIndex(iClientH, 0, 1);


                switch (m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_cType)
                {
                    case 1:
                        switch (iDice(1, 4))
                        {
                            case 1:
                            case 2:
                            case 3:
                                iMineralID = 355;
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                            case 4:
                                iMineralID = 357;  // Ã¶±¤¼®
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                        }
                        break;
                    case 2:
                        switch (iDice(1, 4))
                        {
                            case 1:
                            case 2:
                                iMineralID = 355;
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                            case 3:
                            case 4:
                                iMineralID = 357;  // Ã¶±¤¼®
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                        }
                        break;
                    case 3:
                        switch (iDice(1, 6))
                        {
                            case 1:
                                iMineralID = 355;
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                            case 2:
                            case 3:
                            case 4:
                            case 5:
                                iMineralID = 357;  // Ã¶±¤¼®
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                            case 6:
                                if (iDice(1, 8) == 3)
                                {
                                    if (iDice(1, 2) == 1)
                                    {
                                        iMineralID = 356;
                                        GetExp(iClientH, iDice(2, 4)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,4);
                                    }
                                    else
                                    {
                                        iMineralID = 357;  // Ã¶±¤¼®
                                        GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                    }
                                }
                                else
                                {
                                    iMineralID = 357;  // Ã¶±¤¼®
                                    GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                }
                                break;
                        }
                        break;
                    case 4:
                        switch (iDice(1, 6))
                        {
                            case 1:
                                iMineralID = 355;
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                            case 2:
                            case 3:
                            case 4:
                            case 5:
                                iMineralID = 357;  // Ã¶±¤¼®
                                GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                break;
                            case 6:
                                if (iDice(1, 8) == 3)
                                {
                                    if (iDice(1, 4) == 3)
                                    {
                                        if (iDice(1, 4) == 3)
                                        {
                                            iMineralID = 508;
                                            GetExp(iClientH, iDice(2, 15)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,15);
                                        }
                                        else
                                        {
                                            iMineralID = 356;
                                            GetExp(iClientH, iDice(2, 5)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,5);
                                        }
                                    }
                                    else
                                    {
                                        iMineralID = 356;
                                        GetExp(iClientH, iDice(2, 4)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,4);
                                    }
                                }
                                else
                                {
                                    iMineralID = 357;  // Ã¶±¤¼®
                                    GetExp(iClientH, iDice(2, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(1,3);
                                }
                                break;
                        }
                        break;
                    case 5:
                        switch (iDice(1, 18))
                        {
                            case 3:
                                iMineralID = 352;
                                GetExp(iClientH, iDice(3, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,3);
                                break;
                            default:
                                iMineralID = 358;  // ¼öÁ¤
                                GetExp(iClientH, iDice(3, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,3);
                                break;
                        }
                        break;
                    case 6:
                        switch (iDice(1, 5))
                        {
                            case 1:
                                if (iDice(1, 6) == 3)
                                {
                                    iMineralID = 353;
                                    GetExp(iClientH, iDice(3, 4)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,4);
                                }
                                else
                                {
                                    iMineralID = 358;  // ¼öÁ¤
                                    GetExp(iClientH, iDice(3, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,3);
                                }
                                break;
                            case 2:
                                if (iDice(1, 6) == 3)
                                {
                                    iMineralID = 352;
                                    GetExp(iClientH, iDice(3, 4)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,4);
                                }
                                else
                                {
                                    iMineralID = 358;  // ¼öÁ¤
                                    GetExp(iClientH, iDice(3, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,3);
                                }
                                break;
                            case 3:
                                if (iDice(1, 6) == 3)
                                {
                                    iMineralID = 351;  // ·çºñ
                                    GetExp(iClientH, iDice(3, 4)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,4);
                                }
                                else
                                {
                                    iMineralID = 358;  // ¼öÁ¤
                                    GetExp(iClientH, iDice(3, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,3);
                                }
                                break;
                            case 4:
                                iMineralID = 358;  // ¼öÁ¤
                                GetExp(iClientH, iDice(3, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,3);
                                break;
                            case 5:
                                if (iDice(1, 12) == 3)
                                {
                                    iMineralID = 350;
                                    GetExp(iClientH, iDice(3, 5)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,5);
                                }
                                else
                                {
                                    iMineralID = 358;  // ¼öÁ¤
                                    GetExp(iClientH, iDice(3, 3)); //m_pClientList[iClientH]->m_iExpStock += iDice(2,3);
                                }
                        }
                        break;
                }

                pItem = new CItem;
                if (_bInitItemAttr(pItem, iMineralID) == false)
                {
                    delete pItem;
                }
                else
                {

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                        m_pClientList[iClientH]->m_sY, pItem);

                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4
                    //testcode
                    //wsprintf(G_cTxt, "Mining Success: %s", cMineralName); 
                    //log->info(G_cTxt);
                }


                m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_iRemain--;
                if (m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_iRemain <= 0)
                {

                    bDeleteMineral(m_pDynamicObjectList[iDynamicIndex]->m_iV1);


                    delete m_pDynamicObjectList[iDynamicIndex];
                    m_pDynamicObjectList[iDynamicIndex] = 0;
                }
            }
            break;

        default:
            break;
    }
}


bool CGame::bDeleteMineral(int iIndex)
{
    int iDynamicIndex;
    uint32_t dwTime;

    dwTime = timeGetTime();

    if (m_pMineral[iIndex] == 0) return false;
    iDynamicIndex = m_pMineral[iIndex]->m_sDynamicObjectHandle;
    if (m_pDynamicObjectList[iDynamicIndex] == 0) return false;

    SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iDynamicIndex]->m_cMapIndex,
        m_pDynamicObjectList[iDynamicIndex]->m_sX, m_pDynamicObjectList[iDynamicIndex]->m_sY,
        m_pDynamicObjectList[iDynamicIndex]->m_sType, iDynamicIndex, 0);

    m_pMapList[m_pDynamicObjectList[iDynamicIndex]->m_cMapIndex]->SetDynamicObject(NULL, 0, m_pDynamicObjectList[iDynamicIndex]->m_sX, m_pDynamicObjectList[iDynamicIndex]->m_sY, dwTime);

    m_pMapList[m_pMineral[iIndex]->m_cMapIndex]->SetTempMoveAllowedFlag(m_pDynamicObjectList[iDynamicIndex]->m_sX, m_pDynamicObjectList[iDynamicIndex]->m_sY, true);


    m_pMapList[m_pMineral[iIndex]->m_cMapIndex]->m_iCurMineral--;


    delete m_pMineral[iIndex];
    m_pMineral[iIndex] = 0;

    return true;
}


void CGame::CheckFireBluring(char cMapIndex, int sX, int sY)
{
    int ix, iy, iItemNum;
    short sSpr, sSprFrame;
    char  cItemColor;
    CItem * pItem;

    for (ix = sX - 1; ix <= sX + 1; ix++)
        for (iy = sY - 1; iy <= sY + 1; iy++)
        {

            iItemNum = m_pMapList[cMapIndex]->iCheckItem(ix, iy);

            switch (iItemNum)
            {
                case 355:

                    pItem = m_pMapList[cMapIndex]->pGetItem(ix, iy, &sSpr, &sSprFrame, &cItemColor);
                    if (pItem != 0) delete pItem;
                    iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_FIRE, cMapIndex, ix, iy, 6000);

                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, cMapIndex,
                        ix, iy, sSpr, sSprFrame, cItemColor);
                    break;
            }
        }
}





int CGame::_iGetWeaponSkillType(int iClientH)
{
    uint16_t wWeaponType;

    if (m_pClientList[iClientH] == 0) return 0;



    wWeaponType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);

    if (wWeaponType == 0)
    {

        return 5;
    }
    else if ((wWeaponType >= 1) && (wWeaponType <= 2))
    {

        return 7;
    }
    else if ((wWeaponType > 2) && (wWeaponType < 20))
    {
        if (wWeaponType == 7)
            return 9;
        else return 8;
    }
    else if ((wWeaponType >= 20) && (wWeaponType < 30))
    {

        return 10;
    }
    else if ((wWeaponType >= 30) && (wWeaponType < 35))
    {

        return 14;
    }
    else if ((wWeaponType >= 35) && (wWeaponType < 40))
    {

        return 21;
    }
    else if (wWeaponType >= 40)
    {

        return 6;
    }

    return 1;
}


// ÄÞº¸ °ø°Ý º¸³Ê½º Å×ÀÌºí
//                             1 2 3 
static int ___iCAB5[] = { 0,0, 0,1,2 };
static int ___iCAB6[] = { 0,0, 0,0,0 };
static int ___iCAB7[] = { 0,0, 1,2,3 };
static int ___iCAB8[] = { 0,0, 1,3,5 };
static int ___iCAB9[] = { 0,0, 2,4,8 };
static int ___iCAB10[] = { 0,0, 1,2,3 };

int CGame::iGetComboAttackBonus(int iSkill, int iComboCount)
{
    if (iComboCount <= 1) return 0;
    if (iComboCount > 6) return 0;
    switch (iSkill)
    {
        case 5:
            return ___iCAB5[iComboCount];
            break;
        case 6:
            return ___iCAB6[iComboCount];
            break;
        case 7:
            return ___iCAB7[iComboCount];
            break;
        case 8:
            return ___iCAB8[iComboCount];
            break;
        case 9:
            return ___iCAB9[iComboCount];
            break;
        case 10:
            return ___iCAB10[iComboCount];
            break;
        case 14:
            return ___iCAB6[iComboCount];
            break;
        case 21:
            return ___iCAB10[iComboCount];
            break;
    }

    return 0;
}




bool CGame::_bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID)
{
    bool bRet;


    if (m_pClientList[iClientH] == 0) return false;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return false;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return false;

    switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
    {
        case DEF_ITEMEFFECTTYPE_OCCUPYFLAG:



            if ((BOOL)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 == true)
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 = 100;

            bRet = __bSetOccupyFlag(m_pClientList[iClientH]->m_cMapIndex, dX, dY,                                 // 
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,        //
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1,    //!!! SpecValue!!!
                iClientH,
                (BOOL)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2); //
            if (bRet == true)
            {

            }
            else
            {

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTFLAGSPOT, 0, 0, 0, 0);
            }
            return bRet;

            // crusade
        case DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT:

            bRet = __bSetConstructionKit(m_pClientList[iClientH]->m_cMapIndex, dX, dY,                                 // 
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,        //
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,        //
                iClientH);
            if (bRet == true)
            {

            }
            else
            {

            }
            return bRet;

        case DEF_ITEMEFFECTTYPE_DYE:

            if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS))
            {
                if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != 0)
                {
                    if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 11) ||
                        (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 12))
                    {

                        m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, 0, 0);
                        return true;
                    }
                    else
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, 0, 0);
                        return false;
                    }
                }
            }
            break;
        case DEF_ITEMEFFECTTYPE_ITEMFARM:	//v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã
            bRet = __bSetAgricultureItem(m_pClientList[iClientH]->m_cMapIndex, dX, dY,                                 // 
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,		//
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,		//
                iClientH);
            if (bRet == true)
            {

            }
            else
            {

            }
            return bRet;
            break;


        default:
            break;
    }

    return true;
}


bool CGame::__bSetOccupyFlag(char cMapIndex, int dX, int dY, int iSide, int iEKNum, int iClientH, bool bAdminFlag)
{
    int ix, iy, tx, ty, fx, fy;
    int   iLocalSide, iLocalEKNum, iDynamicObjectIndex, iIndex, iPrevStatus;
    CTile * pTile;
    bool  bFlag;
    uint32_t dwTime = timeGetTime();


    return false;



    if (cMapIndex != m_iMiddlelandMapIndex) return false;

    if ((dX < 25) || (dX >= m_pMapList[cMapIndex]->m_sSizeX - 25) ||
        (dY < 25) || (dY >= m_pMapList[cMapIndex]->m_sSizeY - 25)) return false;

    if ((iClientH > 0) && (m_pClientList[iClientH] != 0))
    {
        if ((bAdminFlag == false) && (m_pClientList[iClientH]->m_cSide != iSide)) return false;
    }


    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + dX + dY * m_pMapList[cMapIndex]->m_sSizeY);
    if (pTile->m_iOccupyFlagIndex != 0) return false;
    if (pTile->m_bIsMoveAllowed == false)  return false;


    for (ix = dX - 5; ix <= dX + 5; ix++)
        for (iy = dY - 5; iy <= dY + 5; iy++)
        {
            if ((ix == dX) && (iy == dY))
            {

            }
            else
            {
                pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy * m_pMapList[cMapIndex]->m_sSizeY);
                if ((pTile->m_iOccupyFlagIndex != 0) && (pTile->m_iOccupyFlagIndex > 0) &&
                    (pTile->m_iOccupyFlagIndex < DEF_MAXOCCUPYFLAG) && (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex] != 0))
                {

                    if (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_cSide == iSide) return false;
                }
            }
        }

    if (m_pMapList[cMapIndex]->m_iTotalOccupyFlags >= DEF_MAXOCCUPYFLAG)
    {

        return false;
    }


    bFlag = false;
    for (ix = dX - 10; ix <= dX + 10; ix++)
        for (iy = dY - 8; iy <= dY + 8; iy++)
        {
            if ((dX < 0) || (dX >= m_pMapList[cMapIndex]->m_sSizeX) ||
                (dY < 0) || (dY >= m_pMapList[cMapIndex]->m_sSizeY))
            {

            }
            else
            {
                pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy * m_pMapList[cMapIndex]->m_sSizeY);
                if ((pTile->m_iOccupyFlagIndex != 0) && (pTile->m_iOccupyFlagIndex > 0) &&
                    (pTile->m_iOccupyFlagIndex < DEF_MAXOCCUPYFLAG) && (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex] != 0))
                {

                    if (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_cSide == iSide)
                    {
                        bFlag = true;
                        goto SOF_LOOPBREAK1;
                    }
                }
            }
        }

    SOF_LOOPBREAK1:;


    if ((bFlag == false) && (bAdminFlag == false)) return false;

    // Crusade

    int iTemp, iDistance = 9999999;
    int iMinFlagIndex = -1;
    CTile * pMinTile;

    for (ix = dX - 10; ix <= dX + 10; ix++)
        for (iy = dY - 8; iy <= dY + 8; iy++)
        {
            if ((ix < 0) || (ix >= m_pMapList[cMapIndex]->m_sSizeX) ||
                (iy < 0) || (iy >= m_pMapList[cMapIndex]->m_sSizeY))
            {

            }
            else
            {
                pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy * m_pMapList[cMapIndex]->m_sSizeY);
                if ((pTile->m_iOccupyFlagIndex != 0) && (pTile->m_iOccupyFlagIndex > 0) &&
                    (pTile->m_iOccupyFlagIndex < DEF_MAXOCCUPYFLAG) && (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex] != 0) &&
                    (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_cSide != iSide))
                {

                    iTemp = (dX - ix) * (dX - ix) + (dY - iy) * (dY - iy);
                    if (iTemp < iDistance)
                    {
                        iDistance = iTemp;
                        iMinFlagIndex = pTile->m_iOccupyFlagIndex;
                        pMinTile = pTile;
                    }
                }
            }
        }


    if (iMinFlagIndex != -1)
    {

        fx = m_pMapList[cMapIndex]->m_pOccupyFlag[iMinFlagIndex]->m_sX;
        fy = m_pMapList[cMapIndex]->m_pOccupyFlag[iMinFlagIndex]->m_sY;
        iLocalSide = m_pMapList[cMapIndex]->m_pOccupyFlag[iMinFlagIndex]->m_cSide;
        iLocalEKNum = m_pMapList[cMapIndex]->m_pOccupyFlag[iMinFlagIndex]->m_iEKCount;


        m_pMapList[cMapIndex]->m_iTotalOccupyFlags--;

        iDynamicObjectIndex = m_pMapList[cMapIndex]->m_pOccupyFlag[iMinFlagIndex]->m_iDynamicObjectIndex;

        SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex,
            m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY,
            m_pDynamicObjectList[iDynamicObjectIndex]->m_sType, iDynamicObjectIndex, 0);

        m_pMapList[m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex]->SetDynamicObject(NULL, 0, m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY, dwTime);


        delete m_pMapList[cMapIndex]->m_pOccupyFlag[iMinFlagIndex];
        m_pMapList[cMapIndex]->m_pOccupyFlag[iMinFlagIndex] = 0;


        pMinTile->m_iOccupyFlagIndex = 0;


        delete m_pDynamicObjectList[iDynamicObjectIndex];
        m_pDynamicObjectList[iDynamicObjectIndex] = 0;


        for (tx = fx - 10; tx <= fx + 10; tx++)
            for (ty = fy - 8; ty <= fy + 8; ty++)
            {
                if ((tx < 0) || (tx >= m_pMapList[cMapIndex]->m_sSizeX) ||
                    (ty < 0) || (ty >= m_pMapList[cMapIndex]->m_sSizeY))
                {

                }
                else
                {
                    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + tx + ty * m_pMapList[cMapIndex]->m_sSizeY);
                    iPrevStatus = pTile->m_iOccupyStatus;

                    switch (iLocalSide)
                    {
                        case 1:
                            pTile->m_iOccupyStatus += iLocalEKNum;
                            if (pTile->m_iOccupyStatus > 0) pTile->m_iOccupyStatus = 0;
                            break;
                        case 2:
                            pTile->m_iOccupyStatus -= iLocalEKNum;
                            if (pTile->m_iOccupyStatus < 0) pTile->m_iOccupyStatus = 0;
                            break;
                    }
                }
            }
    }
    //


    switch (iSide)
    {
        case 1:	iDynamicObjectIndex = iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_ARESDENFLAG1, cMapIndex, dX, dY, 0, 0);	break;
        case 2:	iDynamicObjectIndex = iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_ELVINEFLAG1, cMapIndex, dX, dY, 0, 0);	break;
    }


    iEKNum = 1;

    iIndex = m_pMapList[cMapIndex]->iRegisterOccupyFlag(dX, dY, iSide, iEKNum, iDynamicObjectIndex);
    if (iIndex < 0)
    {


        return true;
    }

    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + dX + dY * m_pMapList[cMapIndex]->m_sSizeY);
    pTile->m_iOccupyFlagIndex = iIndex;


    m_pMapList[cMapIndex]->m_iTotalOccupyFlags++;


    for (ix = dX - 10; ix <= dX + 10; ix++)
        for (iy = dY - 8; iy <= dY + 8; iy++)
        {
            if ((ix < 0) || (ix >= m_pMapList[cMapIndex]->m_sSizeX) ||
                (iy < 0) || (iy >= m_pMapList[cMapIndex]->m_sSizeY))
            {

            }
            else
            {
                pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy * m_pMapList[cMapIndex]->m_sSizeY);

                switch (iSide)
                {
                    case 1:
                        pTile->m_iOccupyStatus -= iEKNum;
                        break;
                    case 2:
                        pTile->m_iOccupyStatus += iEKNum;
                        break;
                }
            }
        }


    iPrevStatus = m_iStrategicStatus;
    _CheckStrategicPointOccupyStatus(cMapIndex);
    if ((iPrevStatus * m_iStrategicStatus) < 0)
    {

    }

    return true;

    /*

    for (ix = dX - 10; ix <= dX + 10; ix++)
    for (iy = dY - 8; iy <= dY + 8; iy++) {
    if (iEKNum <= 0) goto SOF_LOOPBREAK2;

    if ((ix < 0) || (ix >= m_pMapList[cMapIndex]->m_sSizeX) ||
    (iy < 0) || (iy >= m_pMapList[cMapIndex]->m_sSizeY)) {

    }
    else {
    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy*m_pMapList[cMapIndex]->m_sSizeY);
    if ((pTile->m_iOccupyFlagIndex != 0) && (pTile->m_iOccupyFlagIndex > 0) &&
    (pTile->m_iOccupyFlagIndex < DEF_MAXOCCUPYFLAG) && (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex] != 0)) {

    if (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_cSide != iSide) {

    if (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_iEKCount <= iEKNum) {


    // Crusade

    //if ((iClientH > 0) && (m_pClientList[iClientH] != 0))
    //	m_pClientList[iClientH]->m_iExpStock += m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_iEKCount * 40;


    fx = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_sX;
    fy = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_sY;
    iLocalSide  = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_cSide;
    iLocalEKNum = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_iEKCount;


    m_pMapList[cMapIndex]->m_iTotalOccupyFlags--;


    iEKNum -= m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_iEKCount;

    iDynamicObjectIndex = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_iDynamicObjectIndex;

    SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex,
    m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY,
    m_pDynamicObjectList[iDynamicObjectIndex]->m_sType, iDynamicObjectIndex, 0);

    m_pMapList[m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex]->SetDynamicObject(NULL, 0, m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY, dwTime);


    delete m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex];
    m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex] = 0;


    pTile->m_iOccupyFlagIndex = 0;


    delete m_pDynamicObjectList[iDynamicObjectIndex];
    m_pDynamicObjectList[iDynamicObjectIndex] = 0;


    for (tx = fx - 10; tx <= fx + 10; tx++)
    for (ty = fy - 8; ty <= fy + 8; ty++) {
    if ((tx < 0) || (tx >= m_pMapList[cMapIndex]->m_sSizeX) ||
    (ty < 0) || (ty >= m_pMapList[cMapIndex]->m_sSizeY)) {

    }
    else {
    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + tx + ty*m_pMapList[cMapIndex]->m_sSizeY);
    iPrevStatus = pTile->m_iOccupyStatus;

    switch (iLocalSide) {
    case 1:
    pTile->m_iOccupyStatus += iLocalEKNum;
    if (pTile->m_iOccupyStatus > 0) pTile->m_iOccupyStatus = 0;
    break;
    case 2:
    pTile->m_iOccupyStatus -= iLocalEKNum;
    if (pTile->m_iOccupyStatus < 0) pTile->m_iOccupyStatus = 0;
    break;
    }
    }
    }
    }
    else {

    m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_iEKCount -= iEKNum;
    iLocalSide = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_cSide;


    fx = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_sX;
    fy = m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_sY;


    for (tx = fx - 10; tx <= fx + 10; tx++)
    for (ty = fy - 8; ty <= fy + 8; ty++) {
    if ((tx < 0) || (tx >= m_pMapList[cMapIndex]->m_sSizeX) ||
    (ty < 0) || (ty >= m_pMapList[cMapIndex]->m_sSizeY)) {

    }
    else {
    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + tx + ty*m_pMapList[cMapIndex]->m_sSizeY);

    switch (iLocalSide) {
    case 1:
    pTile->m_iOccupyStatus += iEKNum;
    if (pTile->m_iOccupyStatus > 0) pTile->m_iOccupyStatus = 0;
    break;
    case 2:
    pTile->m_iOccupyStatus -= iEKNum;
    if (pTile->m_iOccupyStatus < 0) pTile->m_iOccupyStatus = 0;
    break;
    }
    }
    }

    iEKNum = 0;
    }
    }
    }
    }
    }

    SOF_LOOPBREAK2:;



    if (iEKNum > 0) {

    switch (iSide) {
    case 1:	iDynamicObjectIndex = iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_ARESDENFLAG1, cMapIndex, dX, dY, 0, 0);	break;
    case 2:	iDynamicObjectIndex = iAddDynamicObjectList(NULL, 0, DEF_DYNAMICOBJECT_ELVINEFLAG1, cMapIndex, dX, dY, 0, 0);	break;
    }


    iIndex = m_pMapList[cMapIndex]->iRegisterOccupyFlag(dX, dY, iSide, iEKNum, iDynamicObjectIndex);
    if (iIndex < 0) {


    return true;
    }

    // Crusade

    //if ((iClientH > 0) && (m_pClientList[iClientH] != 0))
    //	m_pClientList[iClientH]->m_iExpStock += iEKNum * 25;

    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + dX + dY*m_pMapList[cMapIndex]->m_sSizeY);
    pTile->m_iOccupyFlagIndex = iIndex;


    m_pMapList[cMapIndex]->m_iTotalOccupyFlags++;


    for (ix = dX - 10; ix <= dX + 10; ix++)
    for (iy = dY - 8; iy <= dY + 8; iy++) {
    if ((ix < 0) || (ix >= m_pMapList[cMapIndex]->m_sSizeX) ||
    (iy < 0) || (iy >= m_pMapList[cMapIndex]->m_sSizeY)) {

    }
    else {
    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy*m_pMapList[cMapIndex]->m_sSizeY);

    switch (iSide) {
    case 1:
    pTile->m_iOccupyStatus -= iEKNum;
    break;
    case 2:
    pTile->m_iOccupyStatus += iEKNum;
    break;
    }
    }
    }
    }


    iPrevStatus = m_iStrategicStatus;
    _CheckStrategicPointOccupyStatus(cMapIndex);
    if ((iPrevStatus * m_iStrategicStatus) < 0) {


    }

    return true;
    */
}


void CGame::SetSummonMobAction(int iClientH, int iMode, uint32_t dwMsgSize, char * pData)
{
    int i, iTargetIndex;
    char   seps[] = "= \t\n";
    char * token, cTargetName[11], cBuff[256];
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_cSide == 0) return;

    switch (iMode)
    {
        case 0:
        case 1:

            for (i = 0; i < DEF_MAXNPCS; i++)
                if (m_pNpcList[i] != 0)
                {
                    if ((m_pNpcList[i]->m_bIsSummoned == true) &&
                        (m_pNpcList[i]->m_iFollowOwnerIndex == iClientH) &&
                        (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER))
                    {

                        m_pNpcList[i]->m_iSummonControlMode = iMode;
                        m_pNpcList[i]->m_bIsPermAttackMode = false;
                        m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                        m_pNpcList[i]->m_sBehaviorTurnCount = 0;
                        m_pNpcList[i]->m_iTargetIndex = 0;
                    }
                }
            break;

        case 2:

            if ((dwMsgSize) <= 0) return;
            memcpy(cBuff, pData, dwMsgSize);

            pStrTok = new CStrTok(cBuff, seps);
            token = pStrTok->pGet();
            token = pStrTok->pGet();

            iTargetIndex = 0;
            if (token != 0)
            {

                if (strlen(token) > 10)
                    memcpy(cTargetName, token, 10);
                else memcpy(cTargetName, token, strlen(token));


                for (i = 1; i < DEF_MAXCLIENTS; i++)
                {
                    // if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) { // original
                    if ((m_pClientList[i] != 0) &&
                        (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0) &&
                        (strcmp(m_pClientList[iClientH]->m_cMapName, m_pClientList[i]->m_cMapName) == 0))
                    {

                        iTargetIndex = i;
                        goto SSMA_SKIPSEARCH;
                    }
                }
            }

            SSMA_SKIPSEARCH:;

            if ((iTargetIndex != 0) && (m_pClientList[iTargetIndex]->m_cSide != 0) &&
                (m_pClientList[iTargetIndex]->m_cSide != m_pClientList[iClientH]->m_cSide))
            {

                for (i = 0; i < DEF_MAXNPCS; i++)
                    if (m_pNpcList[i] != 0)
                    {
                        if ((m_pNpcList[i]->m_bIsSummoned == true) &&
                            (m_pNpcList[i]->m_iFollowOwnerIndex == iClientH) &&
                            (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER))
                        {

                            m_pNpcList[i]->m_iSummonControlMode = iMode;
                            m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                            m_pNpcList[i]->m_sBehaviorTurnCount = 0;
                            m_pNpcList[i]->m_iTargetIndex = iTargetIndex;
                            m_pNpcList[i]->m_cTargetType = DEF_OWNERTYPE_PLAYER;
                            m_pNpcList[i]->m_bIsPermAttackMode = true;
                        }
                    }
            }
            break;
    }
}

void CGame::GetOccupyFlagHandler(int iClientH)
{
    int   i, iNum, iRet, iEraseReq, iEKNum;
    char * cp, cData[256], cItemName[21];
    CItem * pItem;
    uint32_t * dwp;
    short * sp;
    uint16_t * wp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iEnemyKillCount < 3) return;
    if (m_pClientList[iClientH]->m_cSide == 0) return;

    memset(cItemName, 0, sizeof(cItemName));
    switch (m_pClientList[iClientH]->m_cSide)
    {
        case 1: strcpy(cItemName, "¾Æ·¹½ºµ§±ê¹ß"); break;
        case 2: strcpy(cItemName, "¿¤¹ÙÀÎ±ê¹ß");   break;
    }


    iNum = 1;
    for (i = 1; i <= iNum; i++)
    {

        pItem = new CItem;
        if (_bInitItemAttr(pItem, cItemName) == false)
        {

            delete pItem;
        }
        else
        {

            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
            {

                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;


                if (m_pClientList[iClientH]->m_iEnemyKillCount > 12)
                {
                    iEKNum = 12;
                    m_pClientList[iClientH]->m_iEnemyKillCount -= 12;
                }
                else
                {
                    iEKNum = m_pClientList[iClientH]->m_iEnemyKillCount;
                    m_pClientList[iClientH]->m_iEnemyKillCount = 0;
                }


                pItem->m_sItemSpecEffectValue1 = iEKNum;

                //testcode ·Î±×ÆÄÀÏ¿¡ ±â·ÏÇÑ´Ù.
                wsprintf(G_cTxt, "(*) Get Flag : Char(%s) Flag-EK(%d) Player-EK(%d)", m_pClientList[iClientH]->m_cCharName, iEKNum, m_pClientList[iClientH]->m_iEnemyKillCount);
                log->info(G_cTxt);


                dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_ITEMOBTAINED;

                cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

                *cp = 1;
                cp++;

                memcpy(cp, pItem->m_cName, 20);
                cp += 20;

                dwp = (uint32_t *)cp;
                *dwp = pItem->m_dwCount;
                cp += 4;

                *cp = pItem->m_cItemType;
                cp++;

                *cp = pItem->m_cEquipPos;
                cp++;

                *cp = (char)0;
                cp++;

                sp = (short *)cp;
                *sp = pItem->m_sLevelLimit;
                cp += 2;

                *cp = pItem->m_cGenderLimit;
                cp++;

                wp = (uint16_t *)cp;
                *wp = pItem->m_wCurLifeSpan;
                cp += 2;

                wp = (uint16_t *)cp;
                *wp = pItem->m_wWeight;
                cp += 2;

                sp = (short *)cp;
                *sp = pItem->m_sSprite;
                cp += 2;

                sp = (short *)cp;
                *sp = pItem->m_sSpriteFrame;
                cp += 2;

                *cp = pItem->m_cItemColor;
                cp++;

                *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
                cp++;

                dwp = (uint32_t *)cp;
                *dwp = pItem->m_dwAttribute;
                cp += 4;
                /*
                *cp = (char)(pItem->m_dwAttribute & 0x00000001);
                cp++;
                */

                if (iEraseReq == 1) delete pItem;


                iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);


                iCalcTotalWeight(iClientH);

                switch (iRet)
                {
                    case DEF_XSOCKEVENT_QUENEFULL:
                    case DEF_XSOCKEVENT_SOCKETERROR:
                    case DEF_XSOCKEVENT_CRITICALERROR:
                    case DEF_XSOCKEVENT_SOCKETCLOSED:

                        DeleteClient(iClientH, true, true);
                        return;
                }


                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, 0, 0, 0);
            }
            else
            {

                delete pItem;


                iCalcTotalWeight(iClientH);

                dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

                iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
                switch (iRet)
                {
                    case DEF_XSOCKEVENT_QUENEFULL:
                    case DEF_XSOCKEVENT_SOCKETERROR:
                    case DEF_XSOCKEVENT_CRITICALERROR:
                    case DEF_XSOCKEVENT_SOCKETCLOSED:

                        DeleteClient(iClientH, true, true);
                        return;
                }
            }
        }
    }
}



// v1.4311-3 Ãß°¡  ÇÔ¼ö ÀÔÀå±ÇÀ» ÁÖ´Â ÇÔ¼ö   GetFightzoneTicketHandler 
void CGame::GetFightzoneTicketHandler(int iClientH)
{
    int   iRet, iEraseReq, iMonth, iDay, iHour;
    char * cp, cData[256], cItemName[21];
    CItem * pItem;
    uint32_t * dwp;
    short * sp;
    uint16_t * wp;

    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_iFightZoneTicketNumber <= 0)
    {


        m_pClientList[iClientH]->m_iFightzoneNumber *= -1;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -1, 0, 0, 0);
        return;
    }

    memset(cItemName, 0, sizeof(cItemName));


    if (m_pClientList[iClientH]->m_iFightzoneNumber == 1)
        strcpy(cItemName, DEF_ITEMNAME_ARENATICKET);
    else  wsprintf(cItemName, DEF_ITEMNAME_ARENATICKET2, m_pClientList[iClientH]->m_iFightzoneNumber);

    pItem = new CItem;
    if (_bInitItemAttr(pItem, cItemName) == false)
    {
        delete pItem;
        return;
    }

    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
    {

        if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;


        m_pClientList[iClientH]->m_iFightZoneTicketNumber = m_pClientList[iClientH]->m_iFightZoneTicketNumber - 1;

        pItem->m_sTouchEffectType = DEF_ITET_DATE;

        iMonth = m_pClientList[iClientH]->m_iReserveTime / 10000;
        iDay = (m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000) / 100;
        iHour = m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000 - iDay * 100;

        pItem->m_sTouchEffectValue1 = iMonth;
        pItem->m_sTouchEffectValue2 = iDay;
        pItem->m_sTouchEffectValue3 = iHour;

        _bItemLog(DEF_ITEMLOG_GET, iClientH, (int)-1, pItem);

        memset(cData, 0, sizeof(cData));


        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        *cp = 1;
        cp++;

        memcpy(cp, pItem->m_cName, 20);
        cp += 20;

        dwp = (uint32_t *)cp;
        *dwp = pItem->m_dwCount;
        cp += 4;

        *cp = pItem->m_cItemType;
        cp++;

        *cp = pItem->m_cEquipPos;
        cp++;

        *cp = (char)0;
        cp++;

        sp = (short *)cp;
        *sp = pItem->m_sLevelLimit;
        cp += 2;

        *cp = pItem->m_cGenderLimit;
        cp++;

        wp = (uint16_t *)cp;
        *wp = pItem->m_wCurLifeSpan;
        cp += 2;

        wp = (uint16_t *)cp;
        *wp = pItem->m_wWeight;
        cp += 2;

        sp = (short *)cp;
        *sp = pItem->m_sSprite;
        cp += 2;

        sp = (short *)cp;
        *sp = pItem->m_sSpriteFrame;
        cp += 2;

        *cp = pItem->m_cItemColor;
        cp++;

        *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
        cp++;

        dwp = (uint32_t *)cp;
        *dwp = pItem->m_dwAttribute;
        cp += 4;

        if (iEraseReq == 1)
        {
            delete pItem;
            pItem = 0;
        }



        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);


        iCalcTotalWeight(iClientH);

        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:

                DeleteClient(iClientH, true, true);
                return;
        }
    }
    else
    {

        delete pItem;
        pItem = 0;


        iCalcTotalWeight(iClientH);

        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:

                DeleteClient(iClientH, true, true);
                return;
        }
    }
}

int CGame::_iComposeFlagStatusContents(char * pData)
{
    SYSTEMTIME SysTime;
    char cTxt[120];
    int i;

    if (m_iMiddlelandMapIndex < 0) return 0;

    GetLocalTime(&SysTime);
    strcat(pData, "[FILE-DATE]\n\n");

    wsprintf(cTxt, "file-saved-date: %d %d %d %d %d\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
    strcat(pData, cTxt);
    strcat(pData, "\n\n");

    // 
    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pMapList[m_iMiddlelandMapIndex]->m_pOccupyFlag[i] != 0)
        {

            wsprintf(cTxt, "flag = %d %d %d %d", m_pMapList[m_iMiddlelandMapIndex]->m_pOccupyFlag[i]->m_cSide,
                m_pMapList[m_iMiddlelandMapIndex]->m_pOccupyFlag[i]->m_sX,
                m_pMapList[m_iMiddlelandMapIndex]->m_pOccupyFlag[i]->m_sY,
                m_pMapList[m_iMiddlelandMapIndex]->m_pOccupyFlag[i]->m_iEKCount);
            strcat(pData, cTxt);
            strcat(pData, "\n");
        }

    strcat(pData, "\n\n");

    return strlen(pData);
}





void CGame::CheckUniqueItemEquipment(int iClientH)
{
    int i, iDamage;

    if (m_pClientList[iClientH] == 0) return;


    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != 0)
        {
            if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER) &&
                (m_pClientList[iClientH]->m_bIsItemEquipped[i] == true))
            {


                if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue1 == m_pClientList[iClientH]->m_sCharIDnum1) &&
                    (m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue2 == m_pClientList[iClientH]->m_sCharIDnum2) &&
                    (m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue3 == m_pClientList[iClientH]->m_sCharIDnum3))
                {

                }
                else
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos, i, 0, 0);
                    ReleaseItemHandler(iClientH, i, true);

                    iDamage = iDice(10, 10);
                    m_pClientList[iClientH]->m_iHP -= iDamage;
                    if (m_pClientList[iClientH]->m_iHP <= 0)
                    {
                        ClientKilledHandler(iClientH, 0, 0, iDamage);
                    }
                }
            }
        }
}

void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, uint16_t wObjectID, char * pItemName)
{
    short sOwnerH;
    char  cOwnerType;

    if (m_pClientList[iClientH] == 0) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount < iAmount) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;
    if (m_pClientList[iClientH]->m_bIsExchangeMode == true) return;
    if (wObjectID >= DEF_MAXCLIENTS) return;


    //ÀÌÁ¦ dX, dY¿¡ ÀÖ´Â ¿ÀºêÁ§Æ®¿¡°Ô ¼Òºñ¼º ¾ÆÀÌÅÛÀ» °Ç³×ÁØ´Ù. 
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);


    if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
    {

        if (wObjectID != 0)
        {
            if (wObjectID < 10000)
            {

                if (m_pClientList[wObjectID] != 0)
                {
                    if ((WORD)sOwnerH != wObjectID) sOwnerH = 0;
                }
            }
            else sOwnerH = 0;
        }

        if ((sOwnerH == 0) || (m_pClientList[sOwnerH] == 0))
        {
            //Â¾Ã†Ã€ÃŒÃ…Ã› Â±Â³ÃˆÂ¯Ã€Â» Â¿Ã¤ÃƒÂ»Ã‡Ã‘ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â¿Â¡Â°Ã” Ã‡Ã˜Â´Ã§ Ã€Â§Ã„Â¡Â¿Â¡ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â¾Ã¸Ã€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™. 
            _ClearExchangeStatus(iClientH);
        }
        else
        {
            if ((m_pClientList[sOwnerH]->m_bIsExchangeMode == true) || (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) ||
                (m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->m_bIsFightZone == true))
            {

                _ClearExchangeStatus(iClientH);
            }
            else
            {

                m_pClientList[iClientH]->m_bIsExchangeMode = true;
                m_pClientList[iClientH]->m_iExchangeH = sOwnerH;
                memset(m_pClientList[iClientH]->m_cExchangeName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeName));
                strcpy(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[sOwnerH]->m_cCharName);


                //Clear items in the list
                m_pClientList[iClientH]->iExchangeCount = 0;
                m_pClientList[sOwnerH]->iExchangeCount = 0;
                for (int i = 0; i < 4; i++)
                {
                    //Clear the trader
                    memset(m_pClientList[iClientH]->m_cExchangeItemName[i], 0, sizeof(m_pClientList[iClientH]->m_cExchangeItemName[i]));
                    m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
                    m_pClientList[iClientH]->m_iExchangeItemAmount[i] = 0;
                    //Clear the guy we're trading with
                    memset(m_pClientList[sOwnerH]->m_cExchangeItemName[i], 0, sizeof(m_pClientList[sOwnerH]->m_cExchangeItemName[i]));
                    m_pClientList[sOwnerH]->m_cExchangeItemIndex[i] = -1;
                    m_pClientList[sOwnerH]->m_iExchangeItemAmount[i] = 0;
                }


                m_pClientList[iClientH]->m_cExchangeItemIndex[m_pClientList[iClientH]->iExchangeCount] = (char)sItemIndex;
                m_pClientList[iClientH]->m_iExchangeItemAmount[m_pClientList[iClientH]->iExchangeCount] = iAmount;

                //memset(m_pClientList[iClientH]->m_cExchangeItemName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
                memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->iExchangeCount], m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, 20);

                m_pClientList[sOwnerH]->m_bIsExchangeMode = true;
                m_pClientList[sOwnerH]->m_iExchangeH = iClientH;
                memset(m_pClientList[sOwnerH]->m_cExchangeName, 0, sizeof(m_pClientList[sOwnerH]->m_cExchangeName));
                strcpy(m_pClientList[sOwnerH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName);

                m_pClientList[iClientH]->iExchangeCount++;
                SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex + 1000, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);

                SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);
            }
        }
    }
    else
    {

        _ClearExchangeStatus(iClientH);

    }
}

void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount, int sItemIndex)
{
    int iExH;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;
    if (m_pClientList[iClientH]->iExchangeCount > 4) return;	//only 4 items trade

    ////no admin trade
    //if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel < 1)){
    //	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 2203)
    //		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 2204)
    //			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 2205)
    //				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 2206)
    //					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 2207)
    //						if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 2208)
    //							_ClearExchangeStatus(m_pClientList[iClientH]->m_iExchangeH);
    //	_ClearExchangeStatus(iClientH);
    //}


    if ((m_pClientList[iClientH]->m_bIsExchangeMode == true) && (m_pClientList[iClientH]->m_iExchangeH != 0))
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;
        if ((m_pClientList[iExH] == 0) || (memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0))
        {


        }
        else
        {


            if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
            if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return;
            if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount < iAmount) return;

            //No Duplicate items
            for (int i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
            {
                if (m_pClientList[iClientH]->m_cExchangeItemIndex[i] == (char)iItemIndex)
                {
                    _ClearExchangeStatus(iExH);
                    _ClearExchangeStatus(iClientH);
                    return;
                }
            }


            m_pClientList[iClientH]->m_cExchangeItemIndex[m_pClientList[iClientH]->iExchangeCount] = (char)iItemIndex;
            m_pClientList[iClientH]->m_iExchangeItemAmount[m_pClientList[iClientH]->iExchangeCount] = iAmount;

            //memset(m_pClientList[iClientH]->m_cExchangeItemName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
            memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->iExchangeCount], m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, 20);

            //m_pClientList[iClientH]->m_cExchangeItemIndex  = iItemIndex;
            //m_pClientList[iClientH]->m_iExchangeItemAmount = iAmount;
            //memset(m_pClientList[iClientH]->m_cExchangeItemName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
            //memcpy(m_pClientList[iClientH]->m_cExchangeItemName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, 20);

            m_pClientList[iClientH]->iExchangeCount++;
            SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_SETEXCHANGEITEM, iItemIndex + 1000, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 + 100,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);

            SendNotifyMsg(iClientH, iExH, DEF_NOTIFY_SETEXCHANGEITEM, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 + 100,
                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
        }
    }
    else
    {

    }
}

void CGame::ConfirmExchangeItem(int iClientH)
{
    int iExH;
    int iItemWeightA, iItemWeightB, iWeightLeftA, iWeightLeftB, iAmountLeft;
    CItem * pItemA[4], * pItemB[4], * pItemAcopy[4], * pItemBcopy[4];

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;


    if ((m_pClientList[iClientH]->m_bIsExchangeMode == true) && (m_pClientList[iClientH]->m_iExchangeH != 0))
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;


        if (iClientH == iExH) return;

        if (m_pClientList[iExH] != 0)
        {
            if ((memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0) ||
                (m_pClientList[iExH]->m_bIsExchangeMode != true) ||
                (memcmp(m_pClientList[iExH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName, 10) != 0))
            {

                _ClearExchangeStatus(iClientH);
                _ClearExchangeStatus(iExH);
                return;
            }
            else
            {
                m_pClientList[iClientH]->m_bIsExchangeConfirm = true;
                if (m_pClientList[iExH]->m_bIsExchangeConfirm == true)
                {


                    //Check all items
                    for (int i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
                    {
                        if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] == 0) ||
                            (memcmp(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName, m_pClientList[iClientH]->m_cExchangeItemName[i], 20) != 0))
                        {
                            _ClearExchangeStatus(iClientH);
                            _ClearExchangeStatus(iExH);
                            return;
                        }
                    }
                    for (int i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
                    {
                        if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] == 0) ||
                            (memcmp(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName, m_pClientList[iExH]->m_cExchangeItemName[i], 20) != 0))
                        {
                            _ClearExchangeStatus(iClientH);
                            _ClearExchangeStatus(iExH);
                            return;
                        }
                    }

                    iWeightLeftA = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);
                    iWeightLeftB = _iCalcMaxLoad(iExH) - iCalcTotalWeight(iExH);

                    //Calculate weight for items
                    iItemWeightA = 0;
                    for (int i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
                    {
                        iItemWeightA = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]],
                            m_pClientList[iClientH]->m_iExchangeItemAmount[i]);
                    }
                    iItemWeightB = 0;
                    for (int i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
                    {
                        iItemWeightB = iGetItemWeight(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]],
                            m_pClientList[iExH]->m_iExchangeItemAmount[i]);
                    }

                    //See if the other person can take the item weightload
                    if ((iWeightLeftA < iItemWeightB) || (iWeightLeftB < iItemWeightA))
                    {

                        _ClearExchangeStatus(iClientH);
                        _ClearExchangeStatus(iExH);
                        return;
                    }

                    for (int i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
                    {

                        if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                            (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW))
                        {

                            if (m_pClientList[iClientH]->m_iExchangeItemAmount[i] > m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_dwCount)
                            {

                                _ClearExchangeStatus(iClientH);
                                _ClearExchangeStatus(iExH);
                                return;
                            }
                            pItemA[i] = new CItem;
                            _bInitItemAttr(pItemA[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
                            pItemA[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];


                            pItemAcopy[i] = new CItem;
                            _bInitItemAttr(pItemAcopy[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
                            bCopyItemContents(pItemAcopy[i], pItemA[i]);
                            pItemAcopy[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];
                        }
                        else
                        {
                            pItemA[i] = (CItem *)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]];
                            pItemA[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];


                            pItemAcopy[i] = new CItem;
                            _bInitItemAttr(pItemAcopy[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
                            bCopyItemContents(pItemAcopy[i], pItemA[i]);
                            pItemAcopy[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];
                        }
                    }

                    for (int i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
                    {
                        if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                            (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW))
                        {

                            if (m_pClientList[iExH]->m_iExchangeItemAmount[i] > m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_dwCount)
                            {

                                _ClearExchangeStatus(iClientH);
                                _ClearExchangeStatus(iExH);
                                return;
                            }
                            pItemB[i] = new CItem;
                            _bInitItemAttr(pItemB[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
                            pItemB[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];


                            pItemBcopy[i] = new CItem;
                            _bInitItemAttr(pItemBcopy[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
                            bCopyItemContents(pItemBcopy[i], pItemB[i]);
                            pItemBcopy[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];
                        }
                        else
                        {
                            pItemB[i] = (CItem *)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]];
                            pItemB[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];


                            pItemBcopy[i] = new CItem;
                            _bInitItemAttr(pItemBcopy[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
                            bCopyItemContents(pItemBcopy[i], pItemB[i]);
                            pItemBcopy[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];
                        }
                    }


                    for (int i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
                    {
                        bAddItem(iClientH, pItemB[i], 0);
                        _bItemLog(DEF_ITEMLOG_EXCHANGE, iExH, iClientH, pItemBcopy[i]);
                        delete pItemBcopy[i];
                        pItemBcopy[i] = 0;
                        if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                            (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW))
                        {
                            //
                            iAmountLeft = (int)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iExH]->m_iExchangeItemAmount[i];
                            if (iAmountLeft < 0) iAmountLeft = 0;
                            // v1.41 !!!
                            SetItemCount(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], iAmountLeft);
                            // m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cName, iAmountLeft);
                            //
                        }
                        else
                        {

                            ReleaseItemHandler(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], true);
                            SendNotifyMsg(NULL, iExH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iExH]->m_cExchangeItemIndex[i], m_pClientList[iExH]->m_iExchangeItemAmount[i], 0, m_pClientList[iClientH]->m_cCharName);
                            m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] = 0;
                        }
                    }

                    for (int i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
                    {
                        bAddItem(iExH, pItemA[i], 0);
                        _bItemLog(DEF_ITEMLOG_EXCHANGE, iClientH, iExH, pItemAcopy[i]);
                        delete pItemAcopy[i];
                        pItemAcopy[i] = 0;

                        if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                            (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW))
                        {
                            //
                            iAmountLeft = (int)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iClientH]->m_iExchangeItemAmount[i];
                            if (iAmountLeft < 0) iAmountLeft = 0;
                            // v1.41 !!!
                            SetItemCount(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], iAmountLeft);
                            // m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cName, iAmountLeft);
                            //
                        }
                        else
                        {

                            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], true);
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iClientH]->m_cExchangeItemIndex[i], m_pClientList[iClientH]->m_iExchangeItemAmount[i], 0, m_pClientList[iExH]->m_cCharName);
                            m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] = 0;
                        }
                    }

                    m_pClientList[iClientH]->m_bIsExchangeMode = false;
                    m_pClientList[iClientH]->m_bIsExchangeConfirm = false;
                    memset(m_pClientList[iClientH]->m_cExchangeName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeName));
                    m_pClientList[iClientH]->m_iExchangeH = 0;
                    m_pClientList[iClientH]->iExchangeCount = 0;

                    m_pClientList[iExH]->m_bIsExchangeMode = false;
                    m_pClientList[iExH]->m_bIsExchangeConfirm = false;
                    memset(m_pClientList[iExH]->m_cExchangeName, 0, sizeof(m_pClientList[iExH]->m_cExchangeName));
                    m_pClientList[iExH]->m_iExchangeH = 0;
                    m_pClientList[iExH]->iExchangeCount = 0;

                    for (int i = 0; i < 4; i++)
                    {
                        m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
                        m_pClientList[iExH]->m_cExchangeItemIndex[i] = -1;
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, 0, 0, 0, 0);
                    SendNotifyMsg(NULL, iExH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, 0, 0, 0, 0);


                    iCalcTotalWeight(iClientH);
                    iCalcTotalWeight(iExH);
                    return;
                }
            }
        }
        else
        {

            _ClearExchangeStatus(iClientH);
            return;
        }
    }
}
//void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, uint16_t wObjectID, char *pItemName)
//{
// short sOwnerH;
// char  cOwnerType;
// 
//	if (m_pClientList[iClientH] == 0) return;
//	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
//	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
//	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount < iAmount) return;
//	if (m_pClientList[iClientH]->m_bIsExchangeMode == true) return;
//	if (wObjectID >= DEF_MAXCLIENTS) return;
//	if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;	//DEBUG
//
//#ifdef DEF_GUILDWARMODE
//
//	return;
//#endif
//
//
//	if (m_pClientList[iClientH]->m_bIsExchangeMode == true) {
//		_ClearExchangeStatus(iClientH);
//		_ClearExchangeStatus(m_pClientList[iClientH]->m_iExchangeH);
//		return;
//	}
//
//
//	//ÀÌÁ¦ dX, dY¿¡ ÀÖ´Â ¿ÀºêÁ§Æ®¿¡°Ô ¼Òºñ¼º ¾ÆÀÌÅÛÀ» °Ç³×ÁØ´Ù. 
//	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
//	
//	if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_PLAYER)) {
//
//		if (wObjectID != 0) { 
//			if (wObjectID < 10000) {
//
//				if (m_pClientList[wObjectID] != 0) {
//					if ((WORD)sOwnerH != wObjectID) sOwnerH = 0;
//				}
//			}
//			else sOwnerH = 0;
//		}
//		
//		if ((sOwnerH == 0) || (m_pClientList[sOwnerH] == 0)) {
//			//¾ÆÀÌÅÛ ±³È¯À» ¿äÃ»ÇÑ ÇÃ·¹ÀÌ¾î¿¡°Ô ÇØ´ç À§Ä¡¿¡ ÇÃ·¹ÀÌ¾î°¡ ¾øÀ½À» ¾Ë¸°´Ù. 
//			_ClearExchangeStatus(iClientH); 
//		}
//		else {
//			if ((m_pClientList[sOwnerH]->m_bIsExchangeMode == true) || (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) ||
//				(m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->m_bIsFightZone == true)) {
//
//				_ClearExchangeStatus(iClientH);
//			}
//			else {
//
//				m_pClientList[iClientH]->m_bIsExchangeMode = true;
//				m_pClientList[iClientH]->m_iExchangeH = sOwnerH;
//				memset(m_pClientList[iClientH]->m_cExchangeName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeName));
//				strcpy(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[sOwnerH]->m_cCharName);
//
//
//				m_pClientList[iClientH]->m_cExchangeItemIndex  = (char)sItemIndex;
//				m_pClientList[iClientH]->m_iExchangeItemAmount = iAmount;
//				memset(m_pClientList[iClientH]->m_cExchangeItemName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
//				memcpy(m_pClientList[iClientH]->m_cExchangeItemName, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, 20);
//				
//				m_pClientList[sOwnerH]->m_bIsExchangeMode  = true;
//				m_pClientList[sOwnerH]->m_iExchangeH = iClientH;
//				memset(m_pClientList[sOwnerH]->m_cExchangeName, 0, sizeof(m_pClientList[sOwnerH]->m_cExchangeName));
//				strcpy(m_pClientList[sOwnerH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName);
//
//				SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_OPENEXCHANGEWINDOW, 0, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite, 
//				              m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 +100,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);
//
//				SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_OPENEXCHANGEWINDOW, 1, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite, 
//				              m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 +100,
//							  m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);
//			}
//		}
//	}
//	else {
//
//		_ClearExchangeStatus(iClientH);
//	}
//}
//
//
//
//void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount)
//{
// int iExH;
//
//	if (m_pClientList[iClientH] == 0) return;
//	if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;	//DEBUG
//
//	if ((m_pClientList[iClientH]->m_bIsExchangeMode == true) && (m_pClientList[iClientH]->m_iExchangeH != 0)) {
//		iExH = m_pClientList[iClientH]->m_iExchangeH;
//		if ( (m_pClientList[iExH] == 0) || (memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0) ) {
//
//
//		}
//		else {
//
//
//			if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
//			if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return;
//			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount < iAmount) return;
//
//
//			m_pClientList[iClientH]->m_cExchangeItemIndex  = iItemIndex;
//			m_pClientList[iClientH]->m_iExchangeItemAmount = iAmount;
//			memset(m_pClientList[iClientH]->m_cExchangeItemName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
//			memcpy(m_pClientList[iClientH]->m_cExchangeItemName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, 20);
//			
//			SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_SETEXCHANGEITEM, 0, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
//				          m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 +100,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute); 
//
//			SendNotifyMsg(iClientH, iExH, DEF_NOTIFY_SETEXCHANGEITEM, 1, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
//				          m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 +100,
//						  m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
//		}
//	}
//	else {
//
//	}
//}
//
//
//void CGame::ConfirmExchangeItem(int iClientH)
//{
// int iExH;
// int iItemWeightA, iItemWeightB, iWeightLeftA, iWeightLeftB, iAmountLeft;
// CItem * pItemA, * pItemB, * pItemAcopy, * pItemBcopy;
// 
//	if (m_pClientList[iClientH] == 0) return;
//	if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;	//DEBUG
//		
//	if ((m_pClientList[iClientH]->m_bIsExchangeMode == true) && (m_pClientList[iClientH]->m_iExchangeH != 0)) {
//		iExH = m_pClientList[iClientH]->m_iExchangeH;
//	
//
//		if (iClientH == iExH) return;
//
//		if (m_pClientList[iExH] != 0) {
//			if ( (memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0) ||
//			     (m_pClientList[iExH]->m_bIsExchangeMode != true) ||
//				 (memcmp(m_pClientList[iExH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName, 10) != 0) ) {
//
//				_ClearExchangeStatus(iClientH);
//				_ClearExchangeStatus(iExH);
//				return;
//			}
//			else {
//				m_pClientList[iClientH]->m_bIsExchangeConfirm = true;
//				if (m_pClientList[iExH]->m_bIsExchangeConfirm == true) {
//
//					
//					if ( (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex] == 0) ||
//						 (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex] == 0) ) {
//
//						_ClearExchangeStatus(iClientH);
//						_ClearExchangeStatus(iExH);
//						return;
//					}
//					else if ( (memcmp(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cName, m_pClientList[iClientH]->m_cExchangeItemName, 20) != 0) ||
//						      (memcmp(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cName, m_pClientList[iExH]->m_cExchangeItemName, 20) != 0) ) {
//
//						_ClearExchangeStatus(iClientH);
//						_ClearExchangeStatus(iExH);
//						return;
//					}
//					else {
//
//						iWeightLeftA = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);
//						iWeightLeftB = _iCalcMaxLoad(iExH) - iCalcTotalWeight(iExH);
//							
//						//iItemWeightA = m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_wWeight * 
//						//	           m_pClientList[iClientH]->m_iExchangeItemAmount;
//
//						//iItemWeightB = m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_wWeight * 
//						//	           m_pClientList[iExH]->m_iExchangeItemAmount;
//
//						// v1.432
//						iItemWeightA = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex],
//							                          m_pClientList[iClientH]->m_iExchangeItemAmount);
//
//						iItemWeightB = iGetItemWeight(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex],
//													  m_pClientList[iExH]->m_iExchangeItemAmount);
//
//						if ((iWeightLeftA < iItemWeightB) || (iWeightLeftB < iItemWeightA)) {
//
//							_ClearExchangeStatus(iClientH);
//							_ClearExchangeStatus(iExH);
//							return;
//						}
//						else {
//
//							if ( (_iGetItemSpaceLeft(iClientH) == 0) || (_iGetItemSpaceLeft(iExH) == 0) ) {
//
//								_ClearExchangeStatus(iClientH);
//								_ClearExchangeStatus(iExH);
//								return;
//							}
//							else {
//
//			
//
//								if ( (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
//									 (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) ) {
//									
//									if (m_pClientList[iClientH]->m_iExchangeItemAmount > m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_dwCount) {
//
//										_ClearExchangeStatus(iClientH);
//										_ClearExchangeStatus(iExH);
//										return;		
//									}
//									pItemA = new CItem;
//									_bInitItemAttr(pItemA, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cName);
//									pItemA->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount;
//
//
//									pItemAcopy = new CItem;
//									_bInitItemAttr(pItemAcopy, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cName);
//									bCopyItemContents(pItemAcopy, pItemA);
//									pItemAcopy->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount;
//								}
//								else {
//									pItemA = (CItem *)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex];
//									pItemA->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount;
//
//
//									pItemAcopy = new CItem;
//									_bInitItemAttr(pItemAcopy, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cName);
//									bCopyItemContents(pItemAcopy, pItemA);
//									pItemAcopy->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount;
//								}
//
//
//								if ( (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
//									 (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) ) {
//									
//									if (m_pClientList[iExH]->m_iExchangeItemAmount > m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_dwCount) {
//
//										_ClearExchangeStatus(iClientH);
//										_ClearExchangeStatus(iExH);
//										return;		
//									}
//									pItemB = new CItem;
//									_bInitItemAttr(pItemB, m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cName);
//									pItemB->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount;
//
//
//									pItemBcopy = new CItem;
//									_bInitItemAttr(pItemBcopy, m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cName);
//									bCopyItemContents(pItemBcopy, pItemB);
//									pItemBcopy->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount;
//								}
//								else {
//									pItemB = (CItem *)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex];
//									pItemB->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount;
//
//
//									pItemBcopy = new CItem;
//									_bInitItemAttr(pItemBcopy, m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cName);
//									bCopyItemContents(pItemBcopy, pItemB);
//									pItemBcopy->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount;
//								}
//
//
//								bAddItem(iClientH, pItemB, 0);
//								bAddItem(iExH, pItemA, 0);
//
//
//
//
//								_bItemLog(DEF_ITEMLOG_EXCHANGE, iClientH, iExH, pItemAcopy);
//								_bItemLog(DEF_ITEMLOG_EXCHANGE, iExH, iClientH, pItemBcopy);
//
//								delete pItemAcopy;
//								pItemAcopy = 0;
//								delete pItemBcopy;
//								pItemBcopy = 0;
//		
//
//								if ( (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
//									 (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) ) {
//									//
//									iAmountLeft = (int)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_dwCount - m_pClientList[iClientH]->m_iExchangeItemAmount;
//									if (iAmountLeft < 0) iAmountLeft = 0;	 
//									// v1.41 !!!
//									SetItemCount(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex, iAmountLeft);
//									// m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cName, iAmountLeft);
//									//
//								}
//								else {
//
//									ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex, true);
//									SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iClientH]->m_cExchangeItemIndex, m_pClientList[iClientH]->m_iExchangeItemAmount, 0, m_pClientList[iExH]->m_cCharName);
//									m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex] = 0;
//								}
//
//								// v1.42 ###BUG POINT!!! m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex] == 0
//								if (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex] == 0) {
//								
//								
//								}
//
//								if ( (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
//									 (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) ) {
//									//
//									iAmountLeft = (int)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_dwCount - m_pClientList[iExH]->m_iExchangeItemAmount;
//									if (iAmountLeft < 0) iAmountLeft = 0;	  
//									// v1.41 !!!
//									SetItemCount(iExH, m_pClientList[iExH]->m_cExchangeItemIndex, iAmountLeft);
//									// m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cName, iAmountLeft);
//									//
//								}
//								else {
//
//									ReleaseItemHandler(iExH, m_pClientList[iExH]->m_cExchangeItemIndex, true);
//									SendNotifyMsg(NULL, iExH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iExH]->m_cExchangeItemIndex, m_pClientList[iExH]->m_iExchangeItemAmount, 0, m_pClientList[iClientH]->m_cCharName);
//									m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex] = 0;
//								}
//
//
//								m_pClientList[iClientH]->m_bIsExchangeMode = false;
//								m_pClientList[iClientH]->m_bIsExchangeConfirm = false;
//								m_pClientList[iClientH]->m_cExchangeItemIndex = -1;
//								memset(m_pClientList[iClientH]->m_cExchangeName, 0, sizeof(m_pClientList[iClientH]->m_cExchangeName));
//								m_pClientList[iClientH]->m_iExchangeH = 0;
//
//								m_pClientList[iExH]->m_bIsExchangeMode = false;
//								m_pClientList[iExH]->m_bIsExchangeConfirm = false;
//								m_pClientList[iExH]->m_cExchangeItemIndex = -1;
//								memset(m_pClientList[iExH]->m_cExchangeName, 0, sizeof(m_pClientList[iExH]->m_cExchangeName));
//								m_pClientList[iExH]->m_iExchangeH = 0;
//								
//								SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, 0, 0, 0, 0);
//								SendNotifyMsg(NULL, iExH,     DEF_NOTIFY_EXCHANGEITEMCOMPLETE, 0, 0, 0, 0);
//
//
//								iCalcTotalWeight(iClientH);
//								iCalcTotalWeight(iExH);
//								return;
//							}
//						}
//					}
//				}
//			}
//		}
//		else {
//
//			_ClearExchangeStatus(iClientH);
//			return;
//		}
//	}
//}
//
//
//
int CGame::_iGetItemSpaceLeft(int iClientH)
{
    int i, iTotalItem;

    iTotalItem = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != 0) iTotalItem++;

    return (DEF_MAXITEMS - iTotalItem);
}

bool CGame::bAddItem(int iClientH, CItem * pItem, char cMode)
{
    char * cp, cData[256];
    uint32_t * dwp;
    uint16_t * wp;
    short * sp;
    int iRet, iEraseReq;


    memset(cData, 0, sizeof(cData));
    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
    {

        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);


        *cp = 1;
        cp++;

        memcpy(cp, pItem->m_cName, 20);
        cp += 20;

        dwp = (uint32_t *)cp;
        *dwp = pItem->m_dwCount;
        cp += 4;

        *cp = pItem->m_cItemType;
        cp++;

        *cp = pItem->m_cEquipPos;
        cp++;

        *cp = (char)0;
        cp++;

        sp = (short *)cp;
        *sp = pItem->m_sLevelLimit;
        cp += 2;

        *cp = pItem->m_cGenderLimit;
        cp++;

        wp = (uint16_t *)cp;
        *wp = pItem->m_wCurLifeSpan;
        cp += 2;

        wp = (uint16_t *)cp;
        *wp = pItem->m_wWeight;
        cp += 2;

        sp = (short *)cp;
        *sp = pItem->m_sSprite;
        cp += 2;

        sp = (short *)cp;
        *sp = pItem->m_sSpriteFrame;
        cp += 2;

        *cp = pItem->m_cItemColor;
        cp++;

        *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
        cp++;

        dwp = (uint32_t *)cp;
        *dwp = pItem->m_dwAttribute;
        cp += 4;
        /*
        *cp = (char)(pItem->m_dwAttribute & 0x00000001);
        cp++;
        */

        if (iEraseReq == 1)
        {
            //testcode
            //wsprintf(G_cTxt, "AddItem: Delete (%s)", pItem->m_cName);
            //log->info(G_cTxt);
            delete pItem;
            pItem = 0;
        }


        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

        return true;
    }
    else
    {


        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY,
            pItem);


        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color


        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);

        return true;
    }

    return false;
}


void CGame::CancelExchangeItem(int iClientH)
{
    int iExH;


    iExH = m_pClientList[iClientH]->m_iExchangeH;
    _ClearExchangeStatus(iExH);
    _ClearExchangeStatus(iClientH);
}


void CGame::_ClearExchangeStatus(int iToH)
{
    if ((iToH <= 0) || (iToH >= DEF_MAXCLIENTS)) return;
    if (m_pClientList[iToH] == 0) return;

    if (m_pClientList[iToH]->m_cExchangeName != false)
        SendNotifyMsg(NULL, iToH, DEF_NOTIFY_CANCELEXCHANGEITEM, 0, 0, 0, 0, 0, 0,
            NULL, 0, 0, 0, 0);
    m_pClientList[iToH]->m_bIsExchangeConfirm = false;
    // m_pClientList[iToH]->m_cExchangeName    = false;
    m_pClientList[iToH]->m_dwInitCCTime = false;
    m_pClientList[iToH]->m_iAlterItemDropIndex = 0;
    //m_pClientList[iToH]->m_cExchangeItemIndex = -1;
    m_pClientList[iToH]->m_iExchangeH = 0;

    m_pClientList[iToH]->m_bIsExchangeMode = false;

    memset(m_pClientList[iToH]->m_cExchangeName, 0, sizeof(m_pClientList[iToH]->m_cExchangeName));

}


int CGame::__iSearchForQuest(int iClientH, int iWho, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    int i, iQuestList[DEF_MAXQUESTTYPE], iIndex, iQuest, iReward, iQuestIndex;

    if (m_pClientList[iClientH] == 0) return -1;


    iIndex = 0;
    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        iQuestList[i] = -1;

    for (i = 1; i < DEF_MAXQUESTTYPE; i++)
        if (m_pQuestConfigList[i] != 0)
        {

            if (m_pQuestConfigList[i]->m_iFrom != iWho) goto SFQ_SKIP;
            if (m_pQuestConfigList[i]->m_cSide != m_pClientList[iClientH]->m_cSide) goto SFQ_SKIP;
            if (m_pQuestConfigList[i]->m_iMinLevel > m_pClientList[iClientH]->m_iLevel) goto SFQ_SKIP;
            if (m_pQuestConfigList[i]->m_iMaxLevel < m_pClientList[iClientH]->m_iLevel) goto SFQ_SKIP;
            if (m_pQuestConfigList[i]->m_iReqContribution > m_pClientList[iClientH]->m_iContribution) goto SFQ_SKIP;

            if (m_pQuestConfigList[i]->m_iRequiredSkillNum != -1)
            {

                if (m_pClientList[iClientH]->m_cSkillMastery[m_pQuestConfigList[i]->m_iRequiredSkillNum] <
                    m_pQuestConfigList[i]->m_iRequiredSkillLevel) goto SFQ_SKIP;
            }


            if ((m_bIsCrusadeMode == true) && (m_pQuestConfigList[i]->m_iAssignType != 1)) goto SFQ_SKIP;

            if ((m_bIsCrusadeMode == false) && (m_pQuestConfigList[i]->m_iAssignType == 1)) goto SFQ_SKIP;

            if (m_pQuestConfigList[i]->m_iContributionLimit < m_pClientList[iClientH]->m_iContribution) goto SFQ_SKIP;


            iQuestList[iIndex] = i;
            iIndex++;

            SFQ_SKIP:;
        }


    if (iIndex == 0) return -1;
    iQuest = (iDice(1, iIndex)) - 1;
    iQuestIndex = iQuestList[iQuest];

    iReward = iDice(1, 3);
    *pMode = m_pQuestConfigList[iQuestIndex]->m_iResponseMode;
    *pRewardType = m_pQuestConfigList[iQuestIndex]->m_iRewardType[iReward];
    *pRewardAmount = m_pQuestConfigList[iQuestIndex]->m_iRewardAmount[iReward];
    *pContribution = m_pQuestConfigList[iQuestIndex]->m_iContribution;

    strcpy(pTargetName, m_pQuestConfigList[iQuestIndex]->m_cTargetName);
    *pX = m_pQuestConfigList[iQuestIndex]->m_sX;
    *pY = m_pQuestConfigList[iQuestIndex]->m_sY;
    *pRange = m_pQuestConfigList[iQuestIndex]->m_iRange;

    *pTargetType = m_pQuestConfigList[iQuestIndex]->m_iTargetType;
    *pTargetCount = m_pQuestConfigList[iQuestIndex]->m_iMaxCount;
    *pQuestType = m_pQuestConfigList[iQuestIndex]->m_iType;

    return iQuestIndex;
}





void CGame::_SendQuestContents(int iClientH)
{
    int iWho, iIndex, iQuestType, iContribution, iTargetType, iTargetCount, iX, iY, iRange, iQuestCompleted;
    char cTargetName[21];

    if (m_pClientList[iClientH] == 0) return;

    iIndex = m_pClientList[iClientH]->m_iQuest;
    if (iIndex == 0)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCONTENTS, 0, 0, 0, 0,
            NULL, 0, 0, 0, 0, 0, 0);
    }
    else
    {

        if (m_pQuestConfigList[iIndex] == 0) return;

        iWho = m_pQuestConfigList[iIndex]->m_iFrom;
        iQuestType = m_pQuestConfigList[iIndex]->m_iType;
        iContribution = m_pQuestConfigList[iIndex]->m_iContribution;
        iTargetType = m_pQuestConfigList[iIndex]->m_iTargetType;
        iTargetCount = m_pQuestConfigList[iIndex]->m_iMaxCount;
        iX = m_pQuestConfigList[iIndex]->m_sX;
        iY = m_pQuestConfigList[iIndex]->m_sY;
        iRange = m_pQuestConfigList[iIndex]->m_iRange;
        memset(cTargetName, 0, sizeof(cTargetName));
        memcpy(cTargetName, m_pQuestConfigList[iIndex]->m_cTargetName, 20);
        iQuestCompleted = (int)m_pClientList[iClientH]->m_bIsQuestCompleted;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCONTENTS, iWho, iQuestType, iContribution, 0,
            iTargetType, iTargetCount, iX, iY, iRange, iQuestCompleted, cTargetName);
    }
}

void CGame::_CheckQuestEnvironment(int iClientH)
{
    int iIndex;
    char cTargetName[21];

    if (m_pClientList[iClientH] == 0) return;

    iIndex = m_pClientList[iClientH]->m_iQuest;
    if (iIndex == 0) return;


    if (iIndex >= 35 && iIndex <= 40)
    {
        m_pClientList[iClientH]->m_iQuest = 0;
        m_pClientList[iClientH]->m_iQuestID = 0;
        m_pClientList[iClientH]->m_iQuestRewardAmount = 0;
        m_pClientList[iClientH]->m_iQuestRewardType = 0;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTABORTED, 0, 0, 0, 0);
        return;
    }

    if (m_pQuestConfigList[iIndex] == 0) return;

    if (m_pQuestConfigList[iIndex]->m_iQuestID != m_pClientList[iClientH]->m_iQuestID)
    {

        m_pClientList[iClientH]->m_iQuest = 0;
        m_pClientList[iClientH]->m_iQuestID = 0;
        m_pClientList[iClientH]->m_iQuestRewardAmount = 0;
        m_pClientList[iClientH]->m_iQuestRewardType = 0;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTABORTED, 0, 0, 0, 0);
        return;
    }

    switch (m_pQuestConfigList[iIndex]->m_iType)
    {
        case DEF_QUESTTYPE_MONSTERHUNT:
        case DEF_QUESTTYPE_GOPLACE:

            memset(cTargetName, 0, sizeof(cTargetName));
            memcpy(cTargetName, m_pQuestConfigList[iIndex]->m_cTargetName, 20);
            if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, cTargetName, 10) == 0)
                m_pClientList[iClientH]->m_bQuestMatchFlag_Loc = true;
            else m_pClientList[iClientH]->m_bQuestMatchFlag_Loc = false;
            break;
    }
}

bool CGame::_bCheckIsQuestCompleted(int iClientH)
{
    int iQuestIndex;


    if (m_pClientList[iClientH] == 0) return false;
    if (m_pClientList[iClientH]->m_bIsQuestCompleted == true) return false;
    iQuestIndex = m_pClientList[iClientH]->m_iQuest;
    if (iQuestIndex == 0) return false;

    if (m_pQuestConfigList[iQuestIndex] != 0)
    {
        switch (m_pQuestConfigList[iQuestIndex]->m_iType)
        {
            case DEF_QUESTTYPE_MONSTERHUNT:
                if ((m_pClientList[iClientH]->m_bQuestMatchFlag_Loc == true) &&
                    (m_pClientList[iClientH]->m_iCurQuestCount >= m_pQuestConfigList[iQuestIndex]->m_iMaxCount))
                {

                    m_pClientList[iClientH]->m_bIsQuestCompleted = true;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCOMPLETED, 0, 0, 0, 0);
                    return true;
                }
                break;

            case DEF_QUESTTYPE_GOPLACE:
                if ((m_pClientList[iClientH]->m_bQuestMatchFlag_Loc == true) &&
                    (m_pClientList[iClientH]->m_sX >= m_pQuestConfigList[iQuestIndex]->m_sX - m_pQuestConfigList[iQuestIndex]->m_iRange) &&
                    (m_pClientList[iClientH]->m_sX <= m_pQuestConfigList[iQuestIndex]->m_sX + m_pQuestConfigList[iQuestIndex]->m_iRange) &&
                    (m_pClientList[iClientH]->m_sY >= m_pQuestConfigList[iQuestIndex]->m_sY - m_pQuestConfigList[iQuestIndex]->m_iRange) &&
                    (m_pClientList[iClientH]->m_sY <= m_pQuestConfigList[iQuestIndex]->m_sY + m_pQuestConfigList[iQuestIndex]->m_iRange))
                {

                    m_pClientList[iClientH]->m_bIsQuestCompleted = true;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCOMPLETED, 0, 0, 0, 0);
                    return true;
                }
                break;
        }
    }

    return false;
}

void CGame::SendItemNotifyMsg(int iClientH, uint16_t wMsgType, CItem * pItem, int iV1)
{
    char * cp, cData[512]{};
    uint32_t * dwp;
    uint16_t * wp;
    short * sp;
    int     iRet;

    if (m_pClientList[iClientH] == 0) return;

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_NOTIFY;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;
    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    switch (wMsgType)
    {
        case DEF_NOTIFY_ITEMOBTAINED:
            *cp = 1;
            cp++;

            memcpy(cp, pItem->m_cName, 20);
            cp += 20;

            dwp = (uint32_t *)cp;
            *dwp = pItem->m_dwCount;
            cp += 4;

            *cp = pItem->m_cItemType;
            cp++;

            *cp = pItem->m_cEquipPos;
            cp++;

            *cp = (char)0;
            cp++;

            sp = (short *)cp;
            *sp = pItem->m_sLevelLimit;
            cp += 2;

            *cp = pItem->m_cGenderLimit;
            cp++;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wCurLifeSpan;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wWeight;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSprite;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSpriteFrame;
            cp += 2;

            *cp = pItem->m_cItemColor; // v1.4
            cp++;

            *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
            cp++;

            dwp = (uint32_t *)cp;
            *dwp = pItem->m_dwAttribute;
            cp += 4;
            /*
            *cp = (char)(pItem->m_dwAttribute & 0x00000001);
            cp++;
            */

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);
            break;

        case DEF_NOTIFY_ITEMPURCHASED:
            *cp = 1;
            cp++;

            memcpy(cp, pItem->m_cName, 20);
            cp += 20;

            dwp = (uint32_t *)cp;
            *dwp = pItem->m_dwCount;
            cp += 4;

            *cp = pItem->m_cItemType;
            cp++;

            *cp = pItem->m_cEquipPos;
            cp++;

            *cp = (char)0;
            cp++;

            sp = (short *)cp;
            *sp = pItem->m_sLevelLimit;
            cp += 2;

            *cp = pItem->m_cGenderLimit;
            cp++;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wCurLifeSpan;
            cp += 2;

            wp = (uint16_t *)cp;
            *wp = pItem->m_wWeight;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSprite;
            cp += 2;

            sp = (short *)cp;
            *sp = pItem->m_sSpriteFrame;
            cp += 2;

            *cp = pItem->m_cItemColor;
            cp++;

            wp = (uint16_t *)cp;
            *wp = iV1; // (iCost - iDiscountCost);

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 48);
            break;

        case DEF_NOTIFY_CANNOTCARRYMOREITEM:
            iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
            break;
    }
}

bool CGame::_bCheckItemReceiveCondition(int iClientH, CItem * pItem)
{
    int i;

    if (m_pClientList[iClientH] == 0) return false;


    /*
    if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)) {

    if ((m_pClientList[iClientH]->m_iCurWeightLoad + (pItem->m_wWeight * pItem->m_dwCount)) > (DWORD)_iCalcMaxLoad(iClientH))
    return false;
    }
    else {

    if ((m_pClientList[iClientH]->m_iCurWeightLoad + pItem->m_wWeight) > _iCalcMaxLoad(iClientH))
    return false;
    }
    */
    if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount) > (DWORD)_iCalcMaxLoad(iClientH))
        return false;


    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] == 0) return true;

    return false;
}

void CGame::_ClearQuestStatus(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;

    m_pClientList[iClientH]->m_iQuest = 0;
    m_pClientList[iClientH]->m_iQuestID = 0;
    m_pClientList[iClientH]->m_iQuestRewardType = 0;
    m_pClientList[iClientH]->m_iQuestRewardAmount = 0;
    m_pClientList[iClientH]->m_bIsQuestCompleted = false;
}

int CGame::iGetMaxHP(int iClientH)
{
    int iRet;

    if (m_pClientList[iClientH] == 0) return 0;

    iRet = m_pClientList[iClientH]->m_iVit * 3 + m_pClientList[iClientH]->m_iLevel * 2 + m_pClientList[iClientH]->m_iStr / 2;

    if (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown != 0)
        iRet = iRet - (iRet / m_pClientList[iClientH]->m_iSideEffect_MaxHPdown);

    return iRet;
}

void CGame::GetMapInitialPoint(int iMapIndex, short * pX, short * pY, char * pPlayerLocation)
{
    int i, iTotalPoint;
    POINT  pList[DEF_MAXINITIALPOINT];

    if (m_pMapList[iMapIndex] == 0) return;


    iTotalPoint = 0;
    for (i = 0; i < DEF_MAXINITIALPOINT; i++)
        if (m_pMapList[iMapIndex]->m_pInitialPoint[i].x != -1)
        {
            pList[iTotalPoint].x = m_pMapList[iMapIndex]->m_pInitialPoint[i].x;
            pList[iTotalPoint].y = m_pMapList[iMapIndex]->m_pInitialPoint[i].y;
            iTotalPoint++;
        }

    if (iTotalPoint == 0) return;


    if ((pPlayerLocation != 0) && (memcmp(pPlayerLocation, "NONE", 4) == 0))
        i = 0;
    else i = iDice(1, iTotalPoint) - 1;

    *pX = pList[i].x;
    *pY = pList[i].y;
}



void CGame::_CheckStrategicPointOccupyStatus(char cMapIndex)
{
    CTile * pTile;
    int i, iX, iY, iSide, iValue;

    m_iStrategicStatus = 0;

    for (i = 0; i < DEF_MAXSTRATEGICPOINTS; i++)
        if (m_pMapList[cMapIndex]->m_pStrategicPointList[i] != 0)
        {

            iSide = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iSide;
            iValue = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iValue;
            iX = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iX;
            iY = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iY;

            pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + iX + iY * m_pMapList[cMapIndex]->m_sSizeY);


            m_iStrategicStatus += pTile->m_iOccupyStatus * iValue;
        }
}

bool CGame::_bItemLog(int iAction, int iGiveH, int iRecvH, CItem * pItem, bool bForceItemLog)
{
    if (pItem == 0) return false;

    if (m_pClientList[iGiveH]->m_cCharName == 0) return false;


    if (bForceItemLog != true)
    {
        if (_bCheckGoodItem(pItem) == false) return false;
        if (iAction == DEF_ITEMLOG_RETRIEVE) return false;
    }


    char  cTxt[1024]{};
    int iItemCount;

    memset(cTxt, 0, sizeof(cTxt));

    switch (iAction)
    {
        case DEF_ITEMLOG_EXCHANGE:
            if (m_pClientList[iRecvH]->m_cCharName == 0) return false;
            wsprintf(cTxt, "PC(%s)\tExchange\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)\tIP(%s->%s)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
            break;

        case DEF_ITEMLOG_GIVE:
            if (m_pClientList[iRecvH]->m_cCharName == 0) return false;
            wsprintf(cTxt, "PC(%s)\tGive\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
            break;

        case DEF_ITEMLOG_DROP:

            wsprintf(cTxt, "PC(%s)\tDrop\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;

        case DEF_ITEMLOG_GET:

            wsprintf(cTxt, "PC(%s)\tGet\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;
        case DEF_ITEMLOG_MAKE:

            wsprintf(cTxt, "PC(%s)\tMake\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;

        case DEF_ITEMLOG_DEPLETE:

            wsprintf(cTxt, "PC(%s)\tDeplete\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;

        case DEF_ITEMLOG_BUY:
            iItemCount = iRecvH;
            wsprintf(cTxt, "PC(%s)\tBuy\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                iItemCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;

        case DEF_ITEMLOG_SELL:
            wsprintf(cTxt, "PC(%s)\tSell\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;

        case DEF_ITEMLOG_RETRIEVE:
            wsprintf(cTxt, "PC(%s)\tRetrieve\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;

        case DEF_ITEMLOG_DEPOSIT:
            wsprintf(cTxt, "PC(%s)\tDeposit\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;
        case DEF_ITEMLOG_DUPITEMID:

            wsprintf(cTxt, "PC(%s)\tHaveDupItem\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;
        case DEF_ITEMLOG_UPGRADEFAIL:
            wsprintf(cTxt, "PC(%s)\tUpgrade Fail\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;
        case DEF_ITEMLOG_UPGRADESUCCESS:
            wsprintf(cTxt, "PC(%s)\tUpgrade Success\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
                pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
                m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
            break;


        default:
            return false;
    }
    return true;
}

// v2.14 ÇÑ±¹ÂÊ ¾ÆÀÌÅÛ ·Î±× ·Î±× Ãß°¡ 
bool CGame::_bItemLog(int iAction, int iClientH, char * cName, CItem * pItem)
{
    if (pItem == 0) return false;
    if (_bCheckGoodItem(pItem) == false) return false;
    if (iAction != DEF_ITEMLOG_NEWGENDROP)
    {
        if (m_pClientList[iClientH] == 0) return false;
    }
    char  cTxt[200], cTemp1[120];
    //  ·Î±× ³²±ä´Ù. 
    memset(cTxt, 0, sizeof(cTxt));
    memset(cTemp1, 0, sizeof(cTemp1));

    switch (iAction)
    {
        case DEF_ITEMLOG_NEWGENDROP:
            if (pItem == 0) return false;
            wsprintf(cTxt, "NPC(%s)\tDrop\t%s(%d %d %d %d)", cName, pItem->m_cName, pItem->m_dwCount,
                pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3);
            break;
        case DEF_ITEMLOG_SKILLLEARN:
        case DEF_ITEMLOG_MAGICLEARN:
            if (cName == 0) return false;
            if (m_pClientList[iClientH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tLearn\t(%s)\t%s(%d %d)", m_pClientList[iClientH]->m_cCharName, cName,
                m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
            break;
        case DEF_ITEMLOG_SUMMONMONSTER:
            if (cName == 0) return false;
            if (m_pClientList[iClientH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tSummon\t(%s)\t%s(%d %d)", m_pClientList[iClientH]->m_cCharName, cName,
                m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
            break;
        case DEF_ITEMLOG_POISONED:
            if (m_pClientList[iClientH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tBe Poisoned\t \t%s(%d %d)", m_pClientList[iClientH]->m_cCharName,
                m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
            break;

        case DEF_ITEMLOG_REPAIR:
            if (cName == 0) return false;
            if (m_pClientList[iClientH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tRepair\t(%s)\t%s(%d %d)", m_pClientList[iClientH]->m_cCharName, cName,
                m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
            break;

        default:
            return false;
    }
    return true;
}



bool CGame::_bCheckGoodItem(CItem * pItem)
{
    if (pItem == 0) return false;

    if (pItem->m_sIDnum == 90)
    {
        if (pItem->m_dwCount > 10000) return true;  //Gold¿¡ ÇÑÇØ 10000¿ø ÀÌ»ó¸¸ ·Î±×¿¡ ³²±ä´Ù.
        else return false;
    }
    switch (pItem->m_sIDnum)
    {
        case 20: // ¿¢½º Ä®¸®¹ö 
            //	case 90:
        case 259:
        case 290:
        case 291:
        case 292:
        case 300:
        case 305:
        case 308:
        case 311:
        case 334:
        case 335:
        case 336:
        case 338:
        case 380:
        case 381:
        case 382:
        case 391:
        case 400:
        case 401:
        case 490:
        case 491:
        case 492:
        case 508:
        case 581:
        case 610:
        case 611:
        case 612:
        case 613:
        case 614:
        case 616:
        case 618:

        case 620:
        case 621:
        case 622:
        case 623:

        case 630:
        case 631:

        case 632:
        case 633:
        case 634:
        case 635:
        case 636:
        case 637:
        case 638:
        case 639:
        case 640:
        case 641:

        case 642:
        case 643:

        case 644:
        case 645:
        case 646:
        case 647:
        case 648: //³ØÅ¬¸®½º¿Àºê¸®Ä¡

        case 650:
        case 651:
        case 652:
        case 653:
        case 654:
        case 655:
        case 656:
        case 657:

        case 700:
        case 701:
        case 702:
        case 703:
        case 704:
        case 705:
        case 706:
        case 707:
        case 708:
        case 709:
        case 710:
        case 711:
        case 712:
        case 713:
        case 714:
        case 715:
        case 716:
        case 717:
        case 718:
        case 719:

        case 720:
        case 721:
        case 722:
        case 723:

        case 724:
        case 725:
        case 726:
        case 727:
        case 728:
        case 729:
        case 730:
        case 731:
        case 732:
        case 733:

        case 734:
        case 735:

        case 736:
        case 737:
        case 738:
        case 762:
        case 765:  // 3ÁÖ³â ±â³ä ¹ÝÁö
        case 775:
        case 776:

            return true;  //Æ¯º°ÇÑ ¾ÆÀÌÅÛÀÌ±â ¶«½Ã ±â·Ï...
            break;
        default:

            if ((pItem->m_dwAttribute & 0xF0F0F001) == 0) return false;  //Æ¯º°ÇÑ ¾ÆÀÌÅÛµÎ ¾Æ´Ï±¸ Æ¯¼ºÄ¡µµ ¾ø´Ù¸é º°·ç..
            else if (pItem->m_sIDnum > 30) return true;  //Æ¯º°ÇÑ ¾ÆÀÌÅÛÀº ¾Æ´ÏÁö¸¸ Æ¯¼ºÄ¡°¡ ÀÖ°í ´Ü°Ë·ù°¡ ¾Æ´Ï¶ó¸é ÁÁÀº ¾ÆÅÛ..
            else return false;  //Æ¯º°ÇÑ ¾ÆÀÌÅÛµÎ ¾Æ´Ï±¸ Æ¯¼ºÄ¡´Â ÀÖÁö¸¸ ´Ü°Ë·ù¶ó¸é º°·ç...
    }
}
bool CGame::_bCrusadeLog(int iAction, int iClientH, int iData, char * cName)
{
    char  cTxt[200];

    //  ·Î±× ³²±ä´Ù. 
    memset(cTxt, 0, sizeof(cTxt));

    switch (iAction)
    {

        case DEF_CRUSADELOG_ENDCRUSADE:
            if (cName == 0) return false;
            wsprintf(cTxt, "\tEnd Crusade\t%s", cName);
            break;

        case DEF_CRUSADELOG_SELECTDUTY:
            if (cName == 0) return false;
            if (m_pClientList[iClientH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tSelect Duty\t(%s)\t \t(%s)", m_pClientList[iClientH]->m_cCharName, cName, m_pClientList[iClientH]->m_cGuildName);
            break;

        case DEF_CRUSADELOG_GETEXP:
            if (m_pClientList[iClientH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tGet Exp\t(%d)\t \t(%s)", m_pClientList[iClientH]->m_cCharName, iData, m_pClientList[iClientH]->m_cGuildName);
            break;

        case DEF_CRUSADELOG_STARTCRUSADE:
            wsprintf(cTxt, "\tStart Crusade");
            break;

        default:
            return false;
    }

    return true;
}

// v2.14 ´ë¸¸¿ë ¾ÆÀÌÅÛ ·Î±× Ãß°¡ sAttackerH, short sVictumH)
bool CGame::_bPKLog(int iAction, int iAttackerH, int iVictumH, char * pNPC)
{
    char  cTxt[1024], cTemp1[120], cTemp2[120];

    //  ·Î±× ³²±ä´Ù. 
    memset(cTxt, 0, sizeof(cTxt));
    memset(cTemp1, 0, sizeof(cTemp1));
    memset(cTemp2, 0, sizeof(cTemp2));

    if (m_pClientList[iVictumH] == 0) return false;

    switch (iAction)
    {

        case DEF_PKLOG_REDUCECRIMINAL:
            wsprintf(cTxt, "PC(%s)\tReduce\tCC(%d)\t%s(%d %d)\t", m_pClientList[iVictumH]->m_cCharName, m_pClientList[iVictumH]->m_iPKCount,
                m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY);
            break;

        case DEF_PKLOG_BYPLAYER:
            if (m_pClientList[iAttackerH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tKilled by PC\t \t%s(%d %d)\tPC(%s)", m_pClientList[iVictumH]->m_cCharName,
                m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, m_pClientList[iAttackerH]->m_cCharName);
            break;
        case DEF_PKLOG_BYPK:
            if (m_pClientList[iAttackerH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tKilled by PK\tCC(%d)\t%s(%d %d)\tPC(%s)", m_pClientList[iVictumH]->m_cCharName, m_pClientList[iAttackerH]->m_iPKCount,
                m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, m_pClientList[iAttackerH]->m_cCharName);
            break;
        case DEF_PKLOG_BYENERMY:
            if (m_pClientList[iAttackerH] == 0) return false;
            wsprintf(cTxt, "PC(%s)\tKilled by EN\t \t%s(%d %d)\tPC(%s)", m_pClientList[iVictumH]->m_cCharName,
                m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, m_pClientList[iAttackerH]->m_cCharName);
            break;
        case DEF_PKLOG_BYNPC:
            if (pNPC == NULL) return false;
            wsprintf(cTxt, "PC(%s)\tKilled by NPC\t \t%s(%d %d)\tNPC(%s)", m_pClientList[iVictumH]->m_cCharName,
                m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, pNPC);
            break;
        case DEF_PKLOG_BYOTHER:
            wsprintf(cTxt, "PC(%s)\tKilled by Other\t \t%s(%d %d)\tUnKnown", m_pClientList[iVictumH]->m_cCharName,
                m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY);
            break;
        default:
            return false;
    }
    return true;
}


char CGame::_cGetSpecialAbility(int iKindSA)
{
    char cSA;

    switch (iKindSA)
    {
        case 1:
            switch (iDice(1, 2))
            {
                case 1: cSA = 3; break;
                case 2: cSA = 4; break;
            }
            break;

        case 2:
            switch (iDice(1, 3))
            {
                case 1: cSA = 3; break;
                case 2: cSA = 4; break;
                case 3: cSA = 5; break;
            }
            break;

        case 3:
            switch (iDice(1, 4))
            {
                case 1: cSA = 3; break;
                case 2: cSA = 4; break;
                case 3: cSA = 5; break;
                case 4: cSA = 6; break;
            }
            break;

        case 4:
            switch (iDice(1, 3))
            {
                case 1: cSA = 3; break;
                case 2: cSA = 4; break;
                case 3: cSA = 7; break;
            }
            break;

        case 5:
            switch (iDice(1, 4))
            {
                case 1: cSA = 3; break;
                case 2: cSA = 4; break;
                case 3: cSA = 7; break;
                case 4: cSA = 8; break;
            }
            break;

        case 6:
            switch (iDice(1, 3))
            {
                case 1: cSA = 3; break;
                case 2: cSA = 4; break;
                case 3: cSA = 5; break;
            }
            break;

        case 7:
            switch (iDice(1, 3))
            {
                case 1: cSA = 1; break;
                case 2: cSA = 2; break;
                case 3: cSA = 4; break;
            }
            break;

        case 8:
            switch (iDice(1, 5))
            {
                case 1: cSA = 1; break;
                case 2: cSA = 2; break;
                case 3: cSA = 4; break;
                case 4: cSA = 3; break;
                case 5: cSA = 8; break;
            }
            break;

        case 9:
            cSA = iDice(1, 8);
            break;
    }

    return cSA;
}

void CGame::CheckSpecialEvent(int iClientH)
{
    CItem * pItem;
    char  cItemName[21];
    int   iEraseReq;

    if (m_pClientList[iClientH] == 0) return;

    return;

    //v1.42 ÀÌº¥Æ® ¾ÆÀÌÅÛÀ» ÁØ´Ù. 2000.8.1ÀÏ 1ÁÖ³â ¹ÝÁö ¼ö¿© 
    if (m_pClientList[iClientH]->m_iSpecialEventID == 200081)
    {


        if (m_pClientList[iClientH]->m_iLevel < 11)
        {
            m_pClientList[iClientH]->m_iSpecialEventID = 0;
            return;
        }


        memset(cItemName, 0, sizeof(cItemName));
        strcpy(cItemName, DEF_ITEMNAME_MEMORIALRING);

        pItem = new CItem;
        if (_bInitItemAttr(pItem, cItemName) == false)
        {

            delete pItem;
            pItem = 0;
        }
        else
        {
            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
            {

                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

                //testcode ·Î±×ÆÄÀÏ¿¡ ±â·ÏÇÑ´Ù.
                wsprintf(G_cTxt, "(*) Get 1ÁÖ³â±â³ä¹ÝÁö : Char(%s)", m_pClientList[iClientH]->m_cCharName);
                log->info(G_cTxt);


                pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
                pItem->m_cItemColor = 9;


                m_pClientList[iClientH]->m_iSpecialEventID = 0;
            }
        }
    }
}


// 2002-10-24 3ÁÖ³â ÀÌº¥Æ® : 3ÁÖ³â ±â³ä ¹ÝÁö ÁØ´Ù.
void CGame::CheckSpecialEventThirdYear(int iClientH)
{
    CItem * pItem;
    char		cItemName[21];
    int		iEraseReq;
    SYSTEMTIME SysTime;

    int iRet;
    short * sp, sOwnerH{};
    char * cp, cOwnerType{}, cData[100];
    uint32_t * dwp;
    uint16_t * wp;


    if (m_pClientList[iClientH] == 0)
        return;


    if (m_pClientList[iClientH]->m_cSide == 0)
        return;


    if (m_pClientList[iClientH]->m_iLevel < 15)
        return;

#ifndef DEF_TESTSERVER

    GetLocalTime(&SysTime);
    if (SysTime.wYear != 2002 || SysTime.wMonth != 11)
        return;
#endif


    if (m_pClientList[iClientH]->m_iSpecialEventID != 20021101)
    {

        memset(cItemName, 0, sizeof(cItemName));
        strcpy(cItemName, DEF_ITEMNAME_MEMORIALRING3);

        pItem = new CItem;
        if (_bInitItemAttr(pItem, cItemName) == false)
        {

            delete pItem;
            pItem = 0;
        }
        else
        {
            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
            {

                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;


                wsprintf(G_cTxt, "(*) Get 3ÁÖ³â±â³ä¹ÝÁö : Char(%s)", m_pClientList[iClientH]->m_cCharName);
                log->info(G_cTxt);


                pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
                pItem->m_cItemColor = 9;


                m_pClientList[iClientH]->m_iSpecialEventID = 20021101;


                _bItemLog(DEF_ITEMLOG_GET, iClientH, (int)-1, pItem);


                memset(cData, 0, sizeof(cData));

                dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_ITEMOBTAINED;

                cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);


                *cp = 1;
                cp++;

                memcpy(cp, pItem->m_cName, 20);
                cp += 20;

                dwp = (uint32_t *)cp;
                *dwp = pItem->m_dwCount;
                cp += 4;

                *cp = pItem->m_cItemType;
                cp++;

                *cp = pItem->m_cEquipPos;
                cp++;

                *cp = (char)0;
                cp++;

                sp = (short *)cp;
                *sp = pItem->m_sLevelLimit;
                cp += 2;

                *cp = pItem->m_cGenderLimit;
                cp++;

                wp = (uint16_t *)cp;
                *wp = pItem->m_wCurLifeSpan;
                cp += 2;

                wp = (uint16_t *)cp;
                *wp = pItem->m_wWeight;
                cp += 2;

                sp = (short *)cp;
                *sp = pItem->m_sSprite;
                cp += 2;

                sp = (short *)cp;
                *sp = pItem->m_sSpriteFrame;
                cp += 2;

                *cp = pItem->m_cItemColor; // v1.4
                cp++;

                *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
                cp++;

                dwp = (uint32_t *)cp;
                *dwp = pItem->m_dwAttribute;
                cp += 4;


                iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

                switch (iRet)
                {
                    case DEF_XSOCKEVENT_QUENEFULL:
                    case DEF_XSOCKEVENT_SOCKETERROR:
                    case DEF_XSOCKEVENT_CRITICALERROR:
                    case DEF_XSOCKEVENT_SOCKETCLOSED:

                        DeleteClient(iClientH, true, true);
                        return;
                }
            }
        }
    }

} // CheckSpecialEventThirdYear



// 2002-10-24 ÇØ´ç ¾ÆÀÌÅÛÀÌ ¾ÆÀÌÅÛ ÀÌº¥Æ®¿¡ µî·ÏµÇ¾î ÀÖ°í typeÀÌ 0ÀÎÁö °Ë»çÇÑ´Ù.
// ¼öÁ¤ ÇÊ¿äÇÏ´Ù. ¿À·ù ³»Æ÷(adamas)
bool CGame::bCheckInItemEventList(int iItemID, int iNpcH)
{
    int		i;
    char	cItemName[21];

    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalItemEvents != 0)
    {

        for (i = 0; i < DEF_MAXITEMTYPES; i++)
        {
            if (m_pItemConfigList[i] == 0)
                continue;

            if (m_pItemConfigList[i]->m_sIDnum == iItemID)
            {
                strcpy(cItemName, m_pItemConfigList[i]->m_cName);
                break;
            }
        }


        for (i = 0; i < m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalItemEvents; i++)
        {

            if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iType != 0)
                continue;

            if (strcmp(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].cItemName, cItemName) == 0)
                return true;
        }
    }

    return false;
} // bCheckInItemEventList()


bool CGame::_bCheckDupItemID(CItem * pItem)
{
    int i;

    for (i = 0; i < DEF_MAXDUPITEMID; i++)
        if (m_pDupItemIDList[i] != 0)
        {
            if ((pItem->m_sTouchEffectType == m_pDupItemIDList[i]->m_sTouchEffectType) &&
                (pItem->m_sTouchEffectValue1 == m_pDupItemIDList[i]->m_sTouchEffectValue1) &&
                (pItem->m_sTouchEffectValue2 == m_pDupItemIDList[i]->m_sTouchEffectValue2) &&
                (pItem->m_sTouchEffectValue3 == m_pDupItemIDList[i]->m_sTouchEffectValue3))
            {

                pItem->m_wPrice = m_pDupItemIDList[i]->m_wPrice;
                return true;
            }
        }

    return false;
}

void CGame::_AdjustRareItemValue(CItem * pItem)
{
    uint32_t dwSWEType, dwSWEValue;
    double dV1, dV2, dV3;

    if ((pItem->m_dwAttribute & 0x00F00000) != 0)
    {
        dwSWEType = (pItem->m_dwAttribute & 0x00F00000) >> 20;
        dwSWEValue = (pItem->m_dwAttribute & 0x000F0000) >> 16;



        switch (dwSWEType)
        {
            case 0: break;

            case 5:
                pItem->m_cSpeed--;
                if (pItem->m_cSpeed < 0) pItem->m_cSpeed = 0;
                break;

            case 6:
                dV2 = (double)pItem->m_wWeight;
                dV3 = (double)(dwSWEValue * 4);
                dV1 = (dV3 / 100.0f) * dV2;
                pItem->m_wWeight -= (int)dV1;

                if (pItem->m_wWeight < 1) pItem->m_wWeight = 1;
                break;

            case 8:
            case 9:
                dV2 = (double)pItem->m_wMaxLifeSpan;
                dV3 = (double)(dwSWEValue * 7);
                dV1 = (dV3 / 100.0f) * dV2;
                pItem->m_wMaxLifeSpan += (int)dV1;
                break;
        }
    }
}



void CGame::_TamingHandler(int iClientH, int iSkillNum, char cMapIndex, int dX, int dY)
{
    int iSkillLevel, iRange, iTamingLevel, iResult, iX, iY;
    short sOwnerH;
    char  cOwnerType;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pMapList[cMapIndex] == 0) return;

    iSkillLevel = (int)m_pClientList[iClientH]->m_cSkillMastery[iSkillNum];
    iRange = iSkillLevel / 12;

    for (iX = dX - iRange; iX <= dX + iRange; iX++)
        for (iY = dY - iRange; iY <= dY + iRange; iY++)
        {
            sOwnerH = 0;
            if ((iX > 0) && (iY > 0) && (iX < m_pMapList[cMapIndex]->m_sSizeX) && (iY < m_pMapList[cMapIndex]->m_sSizeY))
                m_pMapList[cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, iX, iY);

            if (sOwnerH != 0)
            {
                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if (m_pClientList[sOwnerH] == 0) break;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sOwnerH] == 0) break;

                        iTamingLevel = 10;
                        switch (m_pNpcList[sOwnerH]->m_sType)
                        {
                            case 10:
                            case 16: iTamingLevel = 1; break;
                            case 22: iTamingLevel = 2; break;	// ¹ì
                            case 17:
                            case 14: iTamingLevel = 3; break;
                            case 18: iTamingLevel = 4; break;   // Á»ºñ
                            case 11: iTamingLevel = 5; break;
                            case 23:
                            case 12: iTamingLevel = 6; break;
                            case 28: iTamingLevel = 7; break;
                            case 13:
                            case 27: iTamingLevel = 8; break;
                            case 29: iTamingLevel = 9; break;
                            case 33: iTamingLevel = 9; break;
                            case 30: iTamingLevel = 9; break;  // ¸®Ä¡
                            case 31:
                            case 32: iTamingLevel = 10; break;
                        }

                        iResult = (iSkillLevel / 10);


                        if (iResult < iTamingLevel) break;

                        break;
                }
            }
        }
}




void CGame::_DeleteRandomOccupyFlag(int iMapIndex)
{
    int i, iCount, iTotalFlags, iTargetFlag, iDynamicObjectIndex;
    int tx, ty, fx, fy, iLocalSide, iLocalEKNum, iPrevStatus;
    CTile * pTile;
    uint32_t dwTime;

    if (m_pMapList[iMapIndex] == 0) return;

    dwTime = timeGetTime();


    iTotalFlags = 0;
    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != 0)
        {
            iTotalFlags++;
        }


    iTargetFlag = iDice(1, iTotalFlags);

    iCount = 0;
    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != 0)
        {
            iCount++;
            if ((iCount == iTotalFlags) && (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != 0))
            {


                //testcode
                wsprintf(G_cTxt, "(*)Delete OccupyFlag: Side(%d) XY(%d, %d)", m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_cSide, m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sX, m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sY);
                log->info(G_cTxt);

                fx = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sX;
                fy = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sY;
                iLocalSide = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_cSide;
                iLocalEKNum = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_iEKCount;

                pTile = (CTile *)(m_pMapList[iMapIndex]->m_pTile + m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sX +
                    m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sY * m_pMapList[iMapIndex]->m_sSizeY);

                m_pMapList[iMapIndex]->m_iTotalOccupyFlags--;

                iDynamicObjectIndex = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_iDynamicObjectIndex;

                SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex,
                    m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY,
                    m_pDynamicObjectList[iDynamicObjectIndex]->m_sType, iDynamicObjectIndex, 0);

                m_pMapList[m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex]->SetDynamicObject(NULL, 0, m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY, dwTime);


                delete m_pMapList[iMapIndex]->m_pOccupyFlag[i];
                m_pMapList[iMapIndex]->m_pOccupyFlag[i] = 0;


                pTile->m_iOccupyFlagIndex = 0;


                delete m_pDynamicObjectList[iDynamicObjectIndex];
                m_pDynamicObjectList[iDynamicObjectIndex] = 0;


                for (tx = fx - 10; tx <= fx + 10; tx++)
                    for (ty = fy - 8; ty <= fy + 8; ty++)
                    {
                        if ((tx < 0) || (tx >= m_pMapList[iMapIndex]->m_sSizeX) ||
                            (ty < 0) || (ty >= m_pMapList[iMapIndex]->m_sSizeY))
                        {

                        }
                        else
                        {
                            pTile = (CTile *)(m_pMapList[iMapIndex]->m_pTile + tx + ty * m_pMapList[iMapIndex]->m_sSizeY);
                            iPrevStatus = pTile->m_iOccupyStatus;

                            switch (iLocalSide)
                            {
                                case 1:
                                    pTile->m_iOccupyStatus += iLocalEKNum;
                                    if (pTile->m_iOccupyStatus > 0) pTile->m_iOccupyStatus = 0;
                                    break;
                                case 2:
                                    pTile->m_iOccupyStatus -= iLocalEKNum;
                                    if (pTile->m_iOccupyStatus < 0) pTile->m_iOccupyStatus = 0;
                                    break;
                            }
                        }
                    }
                //
                return;
            }
        }
}






bool CGame::bCheckEnergySphereDestination(int iNpcH, short sAttackerH, char cAttackerType)
{
    int i, sX, sY, dX, dY, iGoalMapIndex;
    char cResult;

    if (m_pNpcList[iNpcH] == 0) return false;
    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iCurEnergySphereGoalPointIndex == -1) return false;

    if (m_pNpcList[iNpcH]->m_cMapIndex != m_iMiddlelandMapIndex)
    {

        iGoalMapIndex = m_pNpcList[iNpcH]->m_cMapIndex;

        sX = m_pNpcList[iNpcH]->m_sX;
        sY = m_pNpcList[iNpcH]->m_sY;

        cResult = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].cResult;
        dX = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenX;
        dY = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenY;
        if ((sX >= dX - 1) && (sX <= dX + 1) && (sY >= dY - 1) && (sY <= dY + 1))
        {


            m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 1)
                { // Aresden (Side:1)

                    wsprintf(G_cTxt, "(!) ¿¡³ÊÁö ½ºÇÇ¾î °ñÀÎ (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {

                }


                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
                    {

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 2, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }

            if (m_pMapList[iGoalMapIndex]->m_bIsEnergySphereAutoCreation == true) EnergySphereProcessor(true, -1 * iGoalMapIndex);
            return true;
        }

        dX = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineX;
        dY = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineY;
        if ((sX >= dX - 1) && (sX <= dX + 1) && (sY >= dY - 1) && (sY <= dY + 1))
        {


            m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 2)
                { // Elvine (Side:2)

                    wsprintf(G_cTxt, "(!) ¿¡³ÊÁö ½ºÇÇ¾î °ñÀÎ (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {

                }


                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
                    {
                        //
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 1, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }

            if (m_pMapList[iGoalMapIndex]->m_bIsEnergySphereAutoCreation == true) EnergySphereProcessor(true, -1 * iGoalMapIndex);
            return true;
        }

        return false;
    }
    else
    {



        sX = m_pNpcList[iNpcH]->m_sX;
        sY = m_pNpcList[iNpcH]->m_sY;

        cResult = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].cResult;
        dX = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenX;
        dY = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenY;
        if ((sX >= dX - 1) && (sX <= dX + 1) && (sY >= dY - 1) && (sY <= dY + 1))
        {


            m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 1)
                { // Aresden (Side:1)

                    m_pClientList[sAttackerH]->m_iContribution += 5;

                    wsprintf(G_cTxt, "(!) ???ÊÁö ?ºÇÇ?î ?ñÀÎ (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {

                    m_pClientList[sAttackerH]->m_iContribution -= 10;
                }


                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
                    {

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 2, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }
            return true;
        }

        dX = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineX;
        dY = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineY;
        if ((sX >= dX - 1) && (sX <= dX + 1) && (sY >= dY - 1) && (sY <= dY + 1))
        {


            m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 2)
                { // Elvine (Side:2)

                    m_pClientList[sAttackerH]->m_iContribution += 5;

                    wsprintf(G_cTxt, "(!) ???ÊÁö ?ºÇÇ?î ?ñÀÎ (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {

                    m_pClientList[sAttackerH]->m_iContribution -= 10;
                }


                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
                    {
                        //
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 1, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }
            return true;
        }
        return false;
    }
}

void CGame::EnergySphereProcessor(bool bIsAdminCreate, int iClientH)
{
    int i, iNamingValue, iCIndex, iTemp, pX, pY, iMapIndex;
    char cSA, cName_Internal[31], cWaypoint[31];

    if (bIsAdminCreate != true)
    {

        if (m_iMiddlelandMapIndex < 0) return;
        if (m_pMapList[m_iMiddlelandMapIndex] == 0) return;

        if (iDice(1, 2000) != 123) return;

        if (m_iTotalGameServerClients < 500) return;


        if (m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex >= 0) return;


        iCIndex = iDice(1, m_pMapList[m_iMiddlelandMapIndex]->m_iTotalEnergySphereCreationPoint);


        if (m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereCreationList[iCIndex].cType == 0) return;


        cSA = 0;
        pX = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereCreationList[iCIndex].sX;
        pY = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereCreationList[iCIndex].sY;
        memset(cWaypoint, 0, sizeof(cWaypoint));

        iNamingValue = m_pMapList[m_iMiddlelandMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue != -1)
        {
            // 
            memset(cName_Internal, 0, sizeof(cName_Internal));
            wsprintf(cName_Internal, "XX%d", iNamingValue);
            cName_Internal[0] = '_';
            cName_Internal[1] = m_iMiddlelandMapIndex + 65;

            if ((bCreateNewNpc("Energy-Sphere", cName_Internal, m_pMapList[m_iMiddlelandMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, false)) == false)
            {

                m_pMapList[m_iMiddlelandMapIndex]->SetNamingValueEmpty(iNamingValue);
                return;
            }
        }


        iTemp = iDice(1, m_pMapList[m_iMiddlelandMapIndex]->m_iTotalEnergySphereGoalPoint);
        if (m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[iTemp].cResult == 0) return;


        m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex = iTemp;


        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHERECREATED, pX, pY, 0, 0);
            }

        //testcode
        wsprintf(G_cTxt, "(!) Energy Sphere Created! (%d, %d)", pX, pY);
        log->info(G_cTxt);
        log->info(G_cTxt);
    }
    else
    {
        if (iClientH == 0)
            iMapIndex = 0;
        else if (iClientH < 0)
            iMapIndex = -1 * iClientH;
        else iMapIndex = m_pClientList[iClientH]->m_cMapIndex;


        if (m_pMapList[iMapIndex]->m_iCurEnergySphereGoalPointIndex >= 0) return;


        iCIndex = iDice(1, m_pMapList[iMapIndex]->m_iTotalEnergySphereCreationPoint);


        if (m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iCIndex].cType == 0) return;


        cSA = 0;
        pX = m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iCIndex].sX;
        pY = m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iCIndex].sY;
        memset(cWaypoint, 0, sizeof(cWaypoint));

        iNamingValue = m_pMapList[iMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue != -1)
        {
            // 
            memset(cName_Internal, 0, sizeof(cName_Internal));
            wsprintf(cName_Internal, "XX%d", iNamingValue);
            cName_Internal[0] = '_';
            cName_Internal[1] = iMapIndex + 65;

            if ((bCreateNewNpc("Energy-Sphere", cName_Internal, m_pMapList[iMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, false)) == false)
            {

                m_pMapList[iMapIndex]->SetNamingValueEmpty(iNamingValue);
                return;
            }
        }


        iTemp = iDice(1, m_pMapList[iMapIndex]->m_iTotalEnergySphereGoalPoint);
        if (m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iTemp].cResult == 0) return;


        m_pMapList[iMapIndex]->m_iCurEnergySphereGoalPointIndex = iTemp;


        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHERECREATED, pX, pY, 0, 0);
            }

        //testcode
        wsprintf(G_cTxt, "(!) Admin Energy Sphere Created! (%d, %d)", pX, pY);
        log->info(G_cTxt);
        log->info(G_cTxt);
    }
}


void CGame::ActivateSpecialAbilityHandler(int iClientH)
{
    uint32_t dwTime = timeGetTime();
    short sTemp;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iSpecialAbilityTime != 0) return;
    if (m_pClientList[iClientH]->m_iSpecialAbilityType == 0) return;
    if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == true) return;

#ifdef DEF_GUILDWARMODE

    return;
#endif

    m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = true;
    m_pClientList[iClientH]->m_dwSpecialAbilityStartTime = dwTime;


    m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;


    sTemp = m_pClientList[iClientH]->m_sAppr4;
    sTemp = sTemp & 0xFF0F;

    switch (m_pClientList[iClientH]->m_iSpecialAbilityType)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            sTemp = sTemp | 0x0010;
            break;
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
            sTemp = sTemp | 0x0020;
            break;
    }
    m_pClientList[iClientH]->m_sAppr4 = sTemp;


    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 1, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityLastSec, 0);

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
}

void CGame::CancelQuestHandler(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;


    _ClearQuestStatus(iClientH);

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTABORTED, 0, 0, 0, 0);
}

int CGame::iGetItemWeight(CItem * pItem, int iCount)
{
    int iWeight;

    iWeight = (pItem->m_wWeight);
    if (iCount < 0) iCount = 1;
    iWeight = iWeight * iCount;
    if (pItem->m_sIDnum == 90) iWeight = iWeight / 20;
    if (iWeight <= 0) iWeight = 1;

    return iWeight;
}

// NPC¸¦ »èÁ¦ÇÒ¶§ Æ¯¼ö ¾ÆÀÌÅÛÀ» ¹ß»ý½ÃÅ³ °ÍÀÎÁöÀÇ ¿©ºÎ¸¦ °è»êÇÑ´Ù.
bool CGame::bGetItemNameWhenDeleteNpc(int & iItemID, short sNpcType, int iItemprobability)
{

    //-- NPC ITEM -----------------------------------------------------------------------------------------------------

    // 2002-09-09 #1
#if		defined(DEF_NPCITEM) 

    CNpcItem CTempNpcItem;
    int	iResult;
    int	iNpcIndex;
    int	iNumNpcitem;
    int	iIndex;
    int	iDiceValue;
    BOOL	bFirstDice = false, bSecondDice = false;

    for (iNpcIndex = 0; iNpcIndex < DEF_MAXNPCTYPES; iNpcIndex++)
    {
        if (m_pNpcConfigList[iNpcIndex] != 0)
        {
            if (m_pNpcConfigList[iNpcIndex]->m_sType == sNpcType) break;
        }
    }

    if (iNpcIndex == DEF_MAXNPCTYPES) return false;

    if (m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size() <= 0)  return false;

    switch (m_pNpcConfigList[iNpcIndex]->m_iNpcItemType)
    {
        case 1:

            iResult = iDice(1, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size()) - 1;

            CTempNpcItem = m_pNpcConfigList[iNpcIndex]->m_vNpcItem.at(iResult);

            if (iDice(1, CTempNpcItem.m_sFirstProbability) == CTempNpcItem.m_cFirstTargetValue) bFirstDice = true;
            if (iDice(1, CTempNpcItem.m_sSecondProbability) == CTempNpcItem.m_cSecondTargetValue) bSecondDice = true;

            if ((bFirstDice == true) && (bSecondDice == true))
            {
                iItemID = CTempNpcItem.m_sItemID;

                wsprintf(G_cTxt, "NpcType 1 (%d) size(%d) %s(%d) (%d)", sNpcType, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size(), CTempNpcItem.m_cName, CTempNpcItem.m_sItemID, iItemID);
                //	PutDebugMsg(G_cTxt);

            }
            break;

        case 2:
            iNumNpcitem = m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size();
            iDiceValue = iDice(1, m_pNpcConfigList[iNpcIndex]->m_iNpcItemMax);

            for (iIndex = 0; iIndex < iNumNpcitem; iIndex++)
            {
                CTempNpcItem = m_pNpcConfigList[iNpcIndex]->m_vNpcItem.at(iIndex);

                if (CTempNpcItem.m_sFirstProbability <= iDiceValue && iDiceValue < CTempNpcItem.m_sSecondProbability)
                {
                    iItemID = CTempNpcItem.m_sItemID;

                    wsprintf(G_cTxt, "NpcType 2 (%d) size(%d) %s(%d) (%d)", sNpcType, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size(), CTempNpcItem.m_cName, CTempNpcItem.m_sItemID, iItemID);
                    //PutDebugMsg(G_cTxt);

                    break;
                }
            }
            break;

    } // switch

    if (iItemID <= 0)
        return false;

    return true;

#else	// !DEF_NPCITEM

    //-- End : NPC ITEM -----------------------------------------------------------------------------------------------

    int iResult;


    switch (sNpcType)
    {
        case 49:
            //		iResult = iDice(1,20000);
            //		if ((iResult >= 1) && (iResult <  5000)) iItemID = 300 ;
            //		else if ((iResult >= 5000) && (iResult <  10000)) iItemID = 259 ;
            iResult = iDice(1, 15000) + 5000;
            if ((iResult >= 5000) && (iResult < 10000)) iItemID = 300;
            else if ((iResult >= 10000) && (iResult < 13000))  iItemID = 337;  // "·çºñ¹ÝÁö"				3000
            else if ((iResult >= 13000) && (iResult < 15000))  iItemID = 335;
            else if ((iResult >= 15000) && (iResult < 17500))  iItemID = 308;
            else if ((iResult >= 17500) && (iResult < 18750))  iItemID = 311;
            else if ((iResult >= 18750) && (iResult < 19000))  iItemID = 305;
            else if ((iResult >= 19000) && (iResult < 19700))  iItemID = 634;
            else if ((iResult >= 19700) && (iResult < 19844))  iItemID = 635;
            else if ((iResult >= 19844) && (iResult < 19922))  iItemID = 643;
            else if ((iResult >= 19922) && (iResult < 19961))  iItemID = 640;
            else if ((iResult >= 19961) && (iResult < 19981))  iItemID = 637;
            else if ((iResult >= 19981) && (iResult < 19991))  iItemID = 620;
            else if ((iResult >= 19991) && (iResult < 19996))  iItemID = 644;
            else if ((iResult >= 19996) && (iResult < 19999))  iItemID = 614;
            else if ((iResult >= 19999) && (iResult <= 20000)) iItemID = 636;
            return true;

        case 50:
            iResult = iDice(1, 10000);
            if ((iResult >= 1) && (iResult <= 4999))
            {
                if (iDice(1, 2) == 1)
                    iItemID = 311;
                else iItemID = 305;
            }
            else if ((iResult >= 5000) && (iResult <= 7499))  iItemID = 614;
            else if ((iResult >= 7500) && (iResult <= 8749))  iItemID = 290;
            else if ((iResult >= 8750) && (iResult <= 9374))  iItemID = 633;
            else if ((iResult >= 9375) && (iResult <= 9687))  iItemID = 492;
            else if ((iResult >= 9688) && (iResult <= 9843))  iItemID = 490;
            else if ((iResult >= 9844) && (iResult <= 9921))  iItemID = 491;
            else if ((iResult >= 9922) && (iResult <= 9960))  iItemID = 291;
            else if ((iResult >= 9961) && (iResult <= 9980))  iItemID = 630;
            else if ((iResult >= 9981) && (iResult <= 9990))  iItemID = 612;
            else if ((iResult >= 9991) && (iResult <= 9996))  iItemID = 610;
            else if ((iResult >= 9996) && (iResult <= 9998))  iItemID = 611;
            else if ((iResult >= 9999) && (iResult <= 10000)) iItemID = 631;
            return true;

        default:
            break;
    }

#ifdef DEF_TESTSERVER


    switch (sNpcType)
    {
        case 11: if (iDice(1, 55) != 11) return false; break; 	  // Skeleton   2 * 100	
        case 12: if (iDice(1, 40) != 11) return false; break;	  // Stone-Golem 2 * 100	
        case 13: if (iDice(1, 10) != 11) return false; break;	  // Cyclops  6 * 100	
        case 14: if (iDice(1, 70) != 11) return false; break;	  // Orc 4 * 100	
        case 17: if (iDice(1, 60) != 11) return false; break;	  // Scorpoin 5 * 100	
        case 18: if (iDice(1, 85) != 11) return false; break;	  // Zombie 1 * 100	
        case 22: if (iDice(1, 60) != 11) return false; break;	  // Amphis 5 * 100	
        case 23: if (iDice(1, 40) != 11) return false; break;	  // Clay-Golem 2 * 100	
        case 27: if (iDice(1, 10) != 11) return false; break;	  // Hellhound 7 * 100	
        case 28: if (iDice(1, 10) != 11) return false; break;	  // Troll 5 * 100	
        case 29:
        case 30:
        case 31: break;	                                   // Demon 5 * 100	
        case 32: if (iDice(1, 2) != 1) return false; break;	   // Unicorn 5 * 100	
        case 33: if (iDice(1, 3) != 3) return false; break;	   // WereWolf 7 * 100
        case 48: if (iDice(1, 10) != 3) return false; break;	   // Stalker 
        case 52: if (iDice(1, 3) != 1) return false; break;      // Gagoyle
        case 53: if (iDice(1, 50) != 11) return false; break;	   // Beholder
        case 54: if (iDice(1, 2) != 11) return false; break;	   // Dark-Elf
        case 57: if (iDice(1, 40) != 11) return false; break;	   // Giant-Frog


        case 58:	// Mountain-Giant
        case 59:	// Ettin
        case 60:	// Cannibal-Plant
            break;

        default: return false;
    } // switch

#else 


    if (iDice(1, iItemprobability) != 13) return false;

    switch (sNpcType)
    {
        case 11: if (iDice(1, 550) != 11) return false; break;	  // Skeleton   2 * 100	
        case 12: if (iDice(1, 400) != 11) return false; break;	  // Stone-Golem 2 * 100	
        case 13: if (iDice(1, 100) != 11) return false; break;	  // Cyclops  6 * 100	
        case 14: if (iDice(1, 700) != 11) return false; break;	  // Orc 4 * 100	
        case 17: if (iDice(1, 600) != 11) return false; break;	  // Scorpoin 5 * 100	
        case 18: if (iDice(1, 850) != 11) return false; break;	  // Zombie 1 * 100	
        case 22: if (iDice(1, 600) != 11) return false; break;	  // Amphis 5 * 100	
        case 23: if (iDice(1, 400) != 11) return false; break;	  // Clay-Golem 2 * 100	
        case 27: if (iDice(1, 100) != 11) return false; break;	  // Hellhound 7 * 100	
        case 28: if (iDice(1, 100) != 11) return false; break;	  // Troll 5 * 100	
        case 29: if (iDice(1, 15) != 3) return false; break;	  // Orge  7 * 100	
        case 30: if (iDice(1, 12) != 3) return false; break;	  // Liche 1 * 100   
        case 31:  	                                      // Demon 5 * 100	
        case 32: if (iDice(1, 2) != 1) return false; break;	  // Unicorn 5 * 100	
        case 33: if (iDice(1, 30) != 3) return false; break;	  // WereWolf 7 * 100
        case 48: if (iDice(1, 100) != 3) return false; break;	  // Stalker 
        case 52: if (iDice(1, 3) != 1) return false; break;     // Gagoyle
        case 53: if (iDice(1, 300) != 11) return false; break;	  // Beholder
        case 54: if (iDice(1, 20) != 11) return false; break;	  // Dark-Elf
        case 57: if (iDice(1, 500) != 11) return false; break;	  // Giant-Frog

        case 58: if (iDice(1, 15) != 11) return false; break;	  // Mountain-Giant
        case 59: if (iDice(1, 10) != 11) return false; break;	  // Ettin
        case 60: if (iDice(1, 80) != 11) return false; break;	  // Cannibal-Plant
        default: return false;
    }

#endif // DEF_TESTSERVER

    // GenType ?áÁ?
    switch (sNpcType)
    {

        case 11: // Skeleton
        case 17: // Scorpoin
        case 14: // Orc
        case 28: // Troll
        case 57: // Giant-Frog
            switch (iDice(1, 7))
            {
                case 1: iItemID = 334; break;
                case 2: iItemID = 336; break;
                case 3: if (iDice(1, 15) == 3) iItemID = 335; break;
                case 4: iItemID = 337; break; // "?çºñ?ÝÁö"
                case 5: iItemID = 333; break; // "?é?Ý?ÝÁö"
                case 6: if (iDice(1, 15) == 3) iItemID = 634; break;
                case 7: if (iDice(1, 25) == 3) iItemID = 635; break;
            }
            break;


        case 13: // Cyclops 
        case 27: // Hellhound
        case 29: // Orge
            switch (iDice(1, 7))
            {
                case 1: iItemID = 311; break;
                case 2: if (iDice(1, 20) == 13) iItemID = 308; break;
                case 3: if (iDice(1, 10) == 13) iItemID = 305; break;
                case 4: iItemID = 300; break;
                case 5: if (iDice(1, 30) == 13) iItemID = 632; break;
                case 6: if (iDice(1, 30) == 13) iItemID = 637; break;
                case 7: if (iDice(1, 30) == 13) iItemID = 638; break;
            }
            break;

        case 18: // Zombie
        case 22: // Amphis
            switch (iDice(1, 4))
            {
                case 1: if (iDice(1, 75) == 13) iItemID = 613; break;
                case 2: if (iDice(1, 20) == 13) iItemID = 639; break;
                case 3: if (iDice(1, 40) == 13) iItemID = 641; break;
                case 4: if (iDice(1, 30) == 13) iItemID = 640; break;
            }
            break;

        case 12: // Stone-Golem
            switch (iDice(1, 5))
            {
                case 1: if (iDice(1, 40) == 13) iItemID = 620; break;
                case 2: if (iDice(1, 40) == 13) iItemID = 621; break;
                case 3: if (iDice(1, 40) == 13) iItemID = 622; break;
                case 4: if (iDice(1, 20) == 11) iItemID = 644; break;
                case 5: if (iDice(1, 20) == 11) iItemID = 647; break;
            }
            break;

        case 23: // Clay-Golem
        case 32: // Unicorn
            switch (iDice(1, 4))
            {
                case 1: if (iDice(1, 40) == 13) iItemID = 620; break;
                case 2: if (iDice(1, 40) == 13) iItemID = 621; break;
                case 3: if (iDice(1, 40) == 13) iItemID = 622; break;
                case 4: if (iDice(1, 20) == 11) iItemID = 644; break;
            }
            break;

        case 33: // WereWolf
        case 48: // Stalker
            switch (iDice(1, 2))
            {
                case 1: if (iDice(1, 30) == 3) iItemID = 290; break;
                case 2: iItemID = 292; break;
            }
            break;

        case 30: // Liche
            switch (iDice(1, 9))
            {
                case 1: if (iDice(1, 10) == 3) iItemID = 380; break; // "???ý??º?(Ice-Storm)"
                case 2: iItemID = 259; break;
                case 3: if (iDice(1, 30) == 3) iItemID = 291; break;
                case 4: if (iDice(1, 10) == 3) iItemID = 614; break;
                case 5: if (iDice(1, 10) == 3) iItemID = 642; break;
                case 6: if (iDice(1, 15) == 3) iItemID = 643; break;
                case 7: if (iDice(1, 30) == 3) iItemID = 636; break;
                case 8: if (iDice(1, 30) == 3) iItemID = 734; break;
                case 9: if (iDice(1, 30) == 3) iItemID = 648; break;
            }
            break;

        case 31:

            switch (iDice(1, 8))
            {
                case 1: if (iDice(1, 30) == 3) iItemID = 382; break; // "???ý??º?(Bloody.S.W)"
                case 2: iItemID = 491; break;
                case 3: if (iDice(1, 10) == 3) iItemID = 490; break;
                case 4: iItemID = 492; break;
                case 5: if (iDice(1, 10) == 3) iItemID = 381; break; // "???ý??º?(Mass.F.S)"
                case 6: if (iDice(1, 30) == 3) iItemID = 633; break;
                case 7: if (iDice(1, 10) == 3) iItemID = 645; break;
                case 8: if (iDice(1, 20) == 3) iItemID = 616; break;
            }
            break;
        case 52:
            switch (iDice(1, 11))
            {
                case 1: if (iDice(1, 30) == 3) iItemID = 382; break; // "???ý??º?(Bloody.S.W)"
                case 2: if (iDice(1, 20) == 3) iItemID = 610; break;
                case 3: if (iDice(1, 20) == 3) iItemID = 611; break; // "Á?????-???º"
                case 4: if (iDice(1, 20) == 3) iItemID = 612; break;
                case 5: if (iDice(1, 10) == 3) iItemID = 381; break; // "???ý??º?(Mass.F.S)"
                case 6: if (iDice(1, 30) == 3) iItemID = 633; break;
                case 7: if (iDice(1, 10) == 3) iItemID = 645; break;
                case 8: if (iDice(1, 40) == 3) iItemID = 630; break;
                case 9: if (iDice(1, 50) == 3) iItemID = 631; break;
                case 10: if (iDice(1, 20) == 3) iItemID = 735; break;
                case 11: if (iDice(1, 40) == 3) iItemID = 20; break;  // "???ºÄ????ö"
            }
            break;

        case 53: // ºñÈ??õ
            //switch (iDice(1,1)) {
            //case 1: if (iDice(1,20) == 11) iItemID = 530 ; break;
            //}
            break;
        case 54:
            if (iDice(1, 20) == 11) iItemID = 618; break;


        case 58: // Mountain-Giant
            switch (iDice(1, 6))
            {
                case 1: if (iDice(1, 40) == 13) iItemID = 620; break;
                case 2: if (iDice(1, 40) == 13) iItemID = 621; break;
                case 3: if (iDice(1, 40) == 13) iItemID = 622; break;
                case 4: if (iDice(1, 20) == 11) iItemID = 644; break;
                case 5: if (iDice(1, 30) == 11) iItemID = 762; break;
                case 6: iItemID = 337; break;			// "?çºñ?ÝÁö"
            }
            break;

        case 59: // Ettin
            switch (iDice(1, 8))
            {
                case 1: if (iDice(1, 40) == 3) iItemID = 382; break; // "???ý??º?(Bloody.S.W)"
                case 2: iItemID = 491; break;
                case 3: if (iDice(1, 30) == 3) iItemID = 490; break;
                case 4: iItemID = 492; break;
                case 5: if (iDice(1, 15) == 3) iItemID = 381; break; // "???ý??º?(Mass.F.S)"
                case 6: if (iDice(1, 35) == 3) iItemID = 633; break;
                case 7: if (iDice(1, 15) == 3) iItemID = 645; break;
                case 8: if (iDice(1, 30) == 3) iItemID = 762; break;
                    //		case 8: if (iDice(1,25) == 3) iItemID = 616 ; break;
            }
            break;

        case 60: // Cannibal-Plant
            switch (iDice(1, 7))
            {
                case 1: iItemID = 334; break;
                case 2: iItemID = 336; break;
                case 3: if (iDice(1, 15) == 3) iItemID = 335; break;
                case 4: iItemID = 337; break; // "?çºñ?ÝÁö"
                case 5: iItemID = 333; break; // "?é?Ý?ÝÁö"
                case 6: if (iDice(1, 15) == 3) iItemID = 634; break;
                case 7: if (iDice(1, 25) == 3) iItemID = 635; break;
            }
            break;
    }

    if (iItemID == 0)
        return false;
    else return true;

#endif // !DEF_NPCITEM

} // bGetItemNameWhenDeleteNpc


void CGame::UpdateMapSectorInfo()
{
    int i, ix, iy;
    int iMaxNeutralActivity, iMaxAresdenActivity, iMaxElvineActivity, iMaxMonsterActivity, iMaxPlayerActivity;

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0)
        {

            iMaxNeutralActivity = iMaxAresdenActivity = iMaxElvineActivity = iMaxMonsterActivity = iMaxPlayerActivity = 0;
            m_pMapList[i]->m_iMaxNx = m_pMapList[i]->m_iMaxNy = m_pMapList[i]->m_iMaxAx = m_pMapList[i]->m_iMaxAy = 0;
            m_pMapList[i]->m_iMaxEx = m_pMapList[i]->m_iMaxEy = m_pMapList[i]->m_iMaxMx = m_pMapList[i]->m_iMaxMy = 0;
            m_pMapList[i]->m_iMaxPx = m_pMapList[i]->m_iMaxPy = 0;


            for (ix = 0; ix < DEF_MAXSECTORS; ix++)
                for (iy = 0; iy < DEF_MAXSECTORS; iy++)
                {
                    if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iNeutralActivity > iMaxNeutralActivity)
                    {
                        iMaxNeutralActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iNeutralActivity;
                        m_pMapList[i]->m_iMaxNx = ix;
                        m_pMapList[i]->m_iMaxNy = iy;
                    }

                    if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iAresdenActivity > iMaxAresdenActivity)
                    {
                        iMaxAresdenActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iAresdenActivity;
                        m_pMapList[i]->m_iMaxAx = ix;
                        m_pMapList[i]->m_iMaxAy = iy;
                    }

                    if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iElvineActivity > iMaxElvineActivity)
                    {
                        iMaxElvineActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iElvineActivity;
                        m_pMapList[i]->m_iMaxEx = ix;
                        m_pMapList[i]->m_iMaxEy = iy;
                    }

                    if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iMonsterActivity > iMaxMonsterActivity)
                    {
                        iMaxMonsterActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iMonsterActivity;
                        m_pMapList[i]->m_iMaxMx = ix;
                        m_pMapList[i]->m_iMaxMy = iy;
                    }

                    if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iPlayerActivity > iMaxPlayerActivity)
                    {
                        iMaxPlayerActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iPlayerActivity;
                        m_pMapList[i]->m_iMaxPx = ix;
                        m_pMapList[i]->m_iMaxPy = iy;
                    }
                }


            m_pMapList[i]->ClearTempSectorInfo();


            if (m_pMapList[i]->m_iMaxNx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxNx][m_pMapList[i]->m_iMaxNy].iNeutralActivity++;
            if (m_pMapList[i]->m_iMaxAx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxAx][m_pMapList[i]->m_iMaxAy].iAresdenActivity++;
            if (m_pMapList[i]->m_iMaxEx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxEx][m_pMapList[i]->m_iMaxEy].iElvineActivity++;
            if (m_pMapList[i]->m_iMaxMx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxMx][m_pMapList[i]->m_iMaxMy].iMonsterActivity++;
            if (m_pMapList[i]->m_iMaxPx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxPx][m_pMapList[i]->m_iMaxPy].iPlayerActivity++;
        }
}


void CGame::AgingMapSectorInfo()
{
    int i, ix, iy;

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0)
        {
            for (ix = 0; ix < DEF_MAXSECTORS; ix++)
                for (iy = 0; iy < DEF_MAXSECTORS; iy++)
                {
                    m_pMapList[i]->m_stSectorInfo[ix][iy].iNeutralActivity--;
                    m_pMapList[i]->m_stSectorInfo[ix][iy].iAresdenActivity--;
                    m_pMapList[i]->m_stSectorInfo[ix][iy].iElvineActivity--;
                    m_pMapList[i]->m_stSectorInfo[ix][iy].iMonsterActivity--;
                    m_pMapList[i]->m_stSectorInfo[ix][iy].iPlayerActivity--;

                    if (m_pMapList[i]->m_stSectorInfo[ix][iy].iNeutralActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iNeutralActivity = 0;
                    if (m_pMapList[i]->m_stSectorInfo[ix][iy].iAresdenActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iAresdenActivity = 0;
                    if (m_pMapList[i]->m_stSectorInfo[ix][iy].iElvineActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iElvineActivity = 0;
                    if (m_pMapList[i]->m_stSectorInfo[ix][iy].iMonsterActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iMonsterActivity = 0;
                    if (m_pMapList[i]->m_stSectorInfo[ix][iy].iPlayerActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iPlayerActivity = 0;
                }
        }
}






void CGame::SaveOccupyFlagData()
{
    char * pData;
    int iSize;
    FILE * pFile;

    return;

    log->info("(!) Middleland OccupyFlag data saved.");

    pData = new char[1000000 + 1];
    if (pData == 0) return;
    memset(pData, 0, 1000000);

    iSize = _iComposeFlagStatusContents(pData);

    _mkdir("GameData");

    pFile = fopen("GameData\\OccupyFlag.txt", "wt");
    if (pFile == 0) return;

    fwrite(pData, 1, iSize, pFile);

    delete pData;
    fclose(pFile);
}






// v2.15 ?Ç??ÀÇ HP?? º???ÁÖ?â À?ÇØ º??æÇÔ 





bool CGame::bAddClientShortCut(int iClientH)
{
    int i;



    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_iClientShortCut[i] == iClientH) return false;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_iClientShortCut[i] == 0)
        {
            m_iClientShortCut[i] = iClientH;
            return true;
        }

    return false;
}

void CGame::RemoveClientShortCut(int iClientH)
{
    int i;

    for (i = 0; i < DEF_MAXCLIENTS + 1; i++)
        if (m_iClientShortCut[i] == iClientH)
        {
            m_iClientShortCut[i] = 0;
            goto RCSC_LOOPBREAK;
        }

    RCSC_LOOPBREAK:;


    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_iClientShortCut[i] == 0) && (m_iClientShortCut[i + 1] != 0))
        {
            m_iClientShortCut[i] = m_iClientShortCut[i + 1];
            m_iClientShortCut[i + 1] = 0;
        }
}





int CGame::iGetMapLocationSide(char * pMapName)
{



    if (strcmp(pMapName, "aresden") == 0) return 3;
    if (strcmp(pMapName, "elvine") == 0)  return 4;
    if (memcmp(pMapName, "arebrk11", 8) == 0) return 3;
    if (memcmp(pMapName, "elvbrk11", 8) == 0) return 4;


    if (memcmp(pMapName, "cityhall_1", 10) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "cityhall_2", 10) == 0)  return DEF_ELVINE;

    if (memcmp(pMapName, "cath_1", 6) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "cath_2", 6) == 0) return DEF_ELVINE;

    if (memcmp(pMapName, "gshop_1", 7) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "gshop_2", 7) == 0) return DEF_ELVINE;

    if (memcmp(pMapName, "bsmith_1", 8) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "bsmith_2", 8) == 0) return DEF_ELVINE;

    if (memcmp(pMapName, "wrhus_1", 7) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "wrhus_2", 7) == 0) return DEF_ELVINE;

    if (memcmp(pMapName, "gldhall_1", 9) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "gldhall_2", 9) == 0) return DEF_ELVINE;

    if (memcmp(pMapName, "wzdtwr_1", 8) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "wzdtwr_2", 8) == 0) return DEF_ELVINE;

    if (memcmp(pMapName, "arefarm", 7) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "elvfarm", 7) == 0) return DEF_ELVINE;

    if (memcmp(pMapName, "arewrhus", 8) == 0) return DEF_ARESDEN;
    if (memcmp(pMapName, "elvwrhus", 8) == 0) return DEF_ELVINE;

    /*  memcmp ¸¦ÇÏ±â ¶§¹®¿¡ ¾Õ¿¡¼­ °É¸°´Ù.
    if (memcmp(pMapName, "gshop_1f") == 0) return 1;
    if (memcmp(pMapName, "bsmith_1f") == 0) return 1;
    if (memcmp(pMapName, "wrhus_1f") == 0) return 1;

    if (memcmp(pMapName, "gshop_2f") == 0) return 2;
    if (memcmp(pMapName, "bsmith_2f") == 0) return 2;
    if (memcmp(pMapName, "wrhus_2f") == 0) return 2; */


    return 0;
}

bool CGame::bCopyItemContents(CItem * pCopy, CItem * pOriginal)
{
    if (pOriginal == 0) return false;
    if (pCopy == 0) return false;

    pCopy->m_sIDnum = pOriginal->m_sIDnum;
    pCopy->m_cItemType = pOriginal->m_cItemType;
    pCopy->m_cEquipPos = pOriginal->m_cEquipPos;
    pCopy->m_sItemEffectType = pOriginal->m_sItemEffectType;
    pCopy->m_sItemEffectValue1 = pOriginal->m_sItemEffectValue1;
    pCopy->m_sItemEffectValue2 = pOriginal->m_sItemEffectValue2;
    pCopy->m_sItemEffectValue3 = pOriginal->m_sItemEffectValue3;
    pCopy->m_sItemEffectValue4 = pOriginal->m_sItemEffectValue4;
    pCopy->m_sItemEffectValue5 = pOriginal->m_sItemEffectValue5;
    pCopy->m_sItemEffectValue6 = pOriginal->m_sItemEffectValue6;
    pCopy->m_wMaxLifeSpan = pOriginal->m_wMaxLifeSpan;
    pCopy->m_sSpecialEffect = pOriginal->m_sSpecialEffect;

    //short m_sSM_HitRatio, m_sL_HitRatio;
    //v1.432 ¸íÁß·ü °¡°¨ »ç¿ë ¾ÈÇÑ´Ù. ´ë½Å Æ¯¼ö ´É·Â ¼öÄ¡°¡ µé¾î°£´Ù.
    pCopy->m_sSpecialEffectValue1 = pOriginal->m_sSpecialEffectValue1;
    pCopy->m_sSpecialEffectValue2 = pOriginal->m_sSpecialEffectValue2;

    pCopy->m_sSprite = pOriginal->m_sSprite;
    pCopy->m_sSpriteFrame = pOriginal->m_sSpriteFrame;

    pCopy->m_cApprValue = pOriginal->m_cApprValue;
    pCopy->m_cSpeed = pOriginal->m_cSpeed;

    pCopy->m_wPrice = pOriginal->m_wPrice;
    pCopy->m_wWeight = pOriginal->m_wWeight;
    pCopy->m_sLevelLimit = pOriginal->m_sLevelLimit;
    pCopy->m_cGenderLimit = pOriginal->m_cGenderLimit;

    pCopy->m_sRelatedSkill = pOriginal->m_sRelatedSkill;

    pCopy->m_cCategory = pOriginal->m_cCategory;
    pCopy->m_bIsForSale = pOriginal->m_bIsForSale;
    // 

    pCopy->m_dwCount = pOriginal->m_dwCount;
    pCopy->m_sTouchEffectType = pOriginal->m_sTouchEffectType;
    pCopy->m_sTouchEffectValue1 = pOriginal->m_sTouchEffectValue1;
    pCopy->m_sTouchEffectValue2 = pOriginal->m_sTouchEffectValue2;
    pCopy->m_sTouchEffectValue3 = pOriginal->m_sTouchEffectValue3;
    pCopy->m_cItemColor = pOriginal->m_cItemColor;
    pCopy->m_sItemSpecEffectValue1 = pOriginal->m_sItemSpecEffectValue1;
    pCopy->m_sItemSpecEffectValue2 = pOriginal->m_sItemSpecEffectValue2;
    pCopy->m_sItemSpecEffectValue3 = pOriginal->m_sItemSpecEffectValue3;
    pCopy->m_wCurLifeSpan = pOriginal->m_wCurLifeSpan;
    pCopy->m_dwAttribute = pOriginal->m_dwAttribute;

    return true;
}







void CGame::ResurrectPlayer(int iClientH)
{
    //testcode
    log->info("(*) Resurrect Player!");



    RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(13, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);


    GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY);

    m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
    m_pClientList[iClientH]->m_iMP = (2 * m_pClientList[iClientH]->m_iMag) + (2 * m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iInt / 2);
    m_pClientList[iClientH]->m_iSP = (2 * m_pClientList[iClientH]->m_iStr) + (2 * m_pClientList[iClientH]->m_iLevel);
    m_pClientList[iClientH]->m_bIsKilled = false;

    RequestInitDataHandler(iClientH, 0, 0, true);
    return;
}

bool CGame::bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex)
{

    if (m_pClientList[iClientH] == 0) return false;
    if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return false;

    switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
    {
        case 4:  // ?Ü?Ë +1
        case 9:
        case 13: // ?Á?í?? +1
        case 16:
        case 18:
        case 19:
        case 21: // ???ºÄ????ö +1
        case 24:
        case 26:
        case 27:
        case 29:
        case 30:
        case 32:
        case 33:
        case 35:
        case 36:
        case 39:
        case 40:
        case 43:
        case 44:
        case 47:
        case 48:
        case 51:
        case 52:
        case 55:
        case 56:
        case 60:
        case 61:
        case 63:
        case 64:
        case 66: // ?ö?????º +1
        case 67: // ?ö?????º +2
        case 69: // ?õºí???º +1
        case 70: // ?õºí???º +2
        case 72: // ?ö???º +1
        case 73: // ?ö???º +2

        case 580:
        case 581:
        case 582:
            break;
    }
}





void CGame::GetExp(int iClientH, int iExp, bool bIsAttackerOwn)
{
    double dV1, dV2, dV3;
    int i, iH, iUnitValue, iPartyTotalMember = 0;
    uint32_t dwTime = timeGetTime();
    char cMapName[12];


    if (m_pClientList[iClientH] == 0) return;
    if (iExp <= 0) return;

    //x2 rates \/
    iExp *= 2;
    //x2 rates ^^


    if (m_pClientList[iClientH]->m_iLevel <= 80)
    {
        dV1 = (double)(80 - m_pClientList[iClientH]->m_iLevel);
        dV2 = dV1 * 0.025f;
        dV3 = (double)iExp;
        dV1 = (dV2 + 1.025f) * dV3;

#ifdef DEF_BEGINNEREVENTMODE
        memset(cMapName, 0, sizeof(cMapName));
        strcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);
        if ((memcmp(cMapName, "arefarm", 7) == 0) || (memcmp(cMapName, "elvfarm", 7) == 0)
            || (memcmp(cMapName, "arebrk", 6) == 0) || (memcmp(cMapName, "elvbrk", 6) == 0)
            || (memcmp(cMapName, "middled1n", 9) == 0))
        {
            dV1 = (int)(dV1) * 2;
        }
#endif
        iExp = (int)dV1;
    }
    else
    {

        if ((m_pClientList[iClientH]->m_iLevel >= 100) && ((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "arefarm") == 0)
            || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvfarm") == 0)))
        {
            iExp = (iExp / 10);
        }
        else if ((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "arefarm") == 0)
            || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvfarm") == 0))
        {
            iExp = (iExp * 1 / 4);
        }
    }

    if ((m_pClientList[iClientH]->m_iPartyID != 0) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM) &&
        ((dwTime - m_pClientList[iClientH]->m_dwLastActionTime) < 1000 * 60 * 5))
    {

        if ((iExp >= 10) && (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers > 1))
        {


            iPartyTotalMember = 0;

            for (i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++)
            {
                iH = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];
                if ((m_pClientList[iH] != 0) && (m_pClientList[iH]->m_iHP > 0))
                {
                    iPartyTotalMember++;
                }
            }

            // ¿¡¶ó »óÈ² 
            if (iPartyTotalMember > 8)
            {
                wsprintf(G_cTxt, "(X) Party Bug !! partyMember %d XXXXXXXXXX", iPartyTotalMember);
                log->info(G_cTxt);
                iPartyTotalMember = 8;
            }



            dV1 = (double)iExp;

            switch (iPartyTotalMember)
            {
#ifdef DEF_V219  
                case 1:  dV2 = dV1;  break;
                case 2:  dV2 = (dV1 + (dV1 * 0.02)) / 2; break;
                case 3:  dV2 = (dV1 + (dV1 * 0.05)) / 3; break;
                case 4:  dV2 = (dV1 + (dV1 * 0.07)) / 4; break;
                case 5:  dV2 = (dV1 + (dV1 * 0.1)) / 5; break;
                case 6:  dV2 = (dV1 + (dV1 * 0.14)) / 6; break;
                case 7:  dV2 = (dV1 + (dV1 * 0.17)) / 7; break;
                case 8:  dV2 = (dV1 + (dV1 * 0.2)) / 8; break;
#else
                case 1:  dV2 = dV1;  break;
                case 2:  dV2 = (dV1 + (dV1 * 0.1)) / 2; break;
                case 3:  dV2 = (dV1 + (dV1 * 0.14)) / 3; break;
                case 4:  dV2 = (dV1 + (dV1 * 0.17)) / 4; break;
                case 5:  dV2 = (dV1 + (dV1 * 0.2)) / 5; break;
                case 6:  dV2 = (dV1 + (dV1 * 0.24)) / 6; break;
                case 7:  dV2 = (dV1 + (dV1 * 0.27)) / 7; break;
                case 8:  dV2 = (dV1 + (dV1 * 0.3)) / 8; break;
#endif
            }


            dV3 = dV2 + 0.5f;
            iUnitValue = (int)dV3;

            /*
            dV1 = (double)m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers;
            dV2 = 2.5f * dV1;
            dV3 = (double)iExp;
            dV1 = ((dV2/100.0f) * dV3) +0.5f;
            iExp += (int)dV1;

            dV1 = (double)iExp;
            if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers <= 0)
            dV2 = 1.0f;
            else dV2 = (double)m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers;
            dV3 = (dV1 / dV2) + 0.5f;
            iUnitValue = (int)dV3; */

#ifdef DEF_TESTSERVER
            iUnitValue = iUnitValue * 30;
#endif


            for (i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++)
            {
                iH = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];
                if ((m_pClientList[iH] != 0) && (m_pClientList[iH]->m_iHP > 0))
                {
                    m_pClientList[iH]->m_iExpStock += iUnitValue;
                }
            }

            if ((bIsAttackerOwn == true) && (iPartyTotalMember > 1))
                m_pClientList[iClientH]->m_iExpStock += (iUnitValue / 10) * 20;

        }
#ifdef DEF_TESTSERVER
        else m_pClientList[iClientH]->m_iExpStock += (iExp * 70) * 20;
#else
        else m_pClientList[iClientH]->m_iExpStock += iExp;
#endif
    } // if
#ifdef DEF_TESTSERVER
    else m_pClientList[iClientH]->m_iExpStock += (iExp * 70) * 20;
#else
    else m_pClientList[iClientH]->m_iExpStock += iExp;
#endif
}


bool CGame::bSerchMaster(int iNpcH)
{
    int ix, iy, rX, rY;
    short sOwnerH, sMasterH;
    char cOwnerType;


    rX = m_pNpcList[iNpcH]->m_sX - m_pNpcList[iNpcH]->m_cTargetSearchRange;
    rY = m_pNpcList[iNpcH]->m_sY - m_pNpcList[iNpcH]->m_cTargetSearchRange;

    sMasterH = 0;

    for (ix = rX; ix < rX + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; ix++)
        for (iy = rY; iy < rY + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; iy++)
        {
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
            switch (cOwnerType)
            {
                case DEF_OWNERTYPE_NPC:
                    if ((sOwnerH != iNpcH) && (m_pNpcList[sOwnerH] != 0) && (m_pNpcList[sOwnerH]->m_sType == m_pNpcList[iNpcH]->m_sType) &&
                        (m_pNpcList[sOwnerH]->m_cMoveType == DEF_MOVETYPE_RANDOM))
                    {

                        m_pNpcList[sOwnerH]->m_bIsMaster = true;
                        m_pNpcList[iNpcH]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
                        m_pNpcList[iNpcH]->m_cFollowOwnerType = DEF_OWNERTYPE_NPC;
                        m_pNpcList[iNpcH]->m_iFollowOwnerIndex = sOwnerH;
                        return true;
                    }
                    break;
            }
        }

    return false;
}

bool CGame::bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH, bool bBonus)
{
    int iValue, iProb, iResult;
    char cTxt[126]{};

    if (m_pClientList[iClientH]->m_pItemList[iSomH] == 0) return false;
    iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;

    switch (iValue)
    {
        case 0: iProb = 20; break;
        case 1: iProb = 15; break;  // +2 :80%      +3
        case 2: iProb = 9; break;  // +3 :48%      +4 
        case 3: iProb = 8; break;  // +4 :24%      +5
        case 4: iProb = 7; break;  // +5 :9.6%     +6
        case 5: iProb = 7; break;  // +6 :2.8%     +7
        case 6: iProb = 5; break;  // +7 :0.57%    +8
        case 7: iProb = 5; break;  // +8 :0.05%    +9
        case 8: iProb = 3; break;  // +9 :0.004%   +10
        case 9: iProb = 2; break;  // +10:0.00016%
        default: iProb = 0; break;
    }

    if (((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 > 100))
    {

        if (iProb > 20)
            iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 10);
        else if (iProb > 7)
            iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 20);
        else
            iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 40);
    }
    if (bBonus == true) iProb *= 2;

    iProb *= 100;
    iResult = iDice(1, 10000);

    if (iProb >= iResult)
    {
        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
        return true;
    }

    _bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);

    return false;
}

// v2.16 2002-5-23 °í±¤Çö ¼öÁ¤ ¼ºÈÄ´Ï ¼öÁ¤
void CGame::ArmorLifeDecrement(int iAttackerH, int iTargetH, char cOwnerType, int iValue)
{
    int iTemp;

    if (m_pClientList[iAttackerH] == 0) return;

    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[iTargetH] == 0) return;
            break;

        case DEF_OWNERTYPE_NPC:	return;
        default: return;
    }


    if (m_pClientList[iAttackerH]->m_cSide == m_pClientList[iTargetH]->m_cSide) return;


    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0))
    {

        if ((m_pClientList[iTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;


            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

            ReleaseItemHandler(iTargetH, iTemp, true);
        }
    }


    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0))
    {


        if ((m_pClientList[iTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;


            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

            ReleaseItemHandler(iTargetH, iTemp, true);
        }
    }

    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0))
    {


        if ((m_pClientList[iTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;


            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

            ReleaseItemHandler(iTargetH, iTemp, true);
        }
    }


    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0))
    {


        if ((m_pClientList[iTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;


            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

            ReleaseItemHandler(iTargetH, iTemp, true);
        }
    }


    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0))
    {


        if ((m_pClientList[iTargetH]->m_cSide != DEF_NETURAL) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;


            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);

            ReleaseItemHandler(iTargetH, iTemp, true);
        }
    }
}


// v2.171 2002-6-14

//v2.171
bool CGame::bCheckClientAttackFrequency(int iClientH, uint32_t dwClientTime)
{
    uint32_t dwTimeGap;

    if (m_pClientList[iClientH] == 0) return false;

    if (m_pClientList[iClientH]->m_dwAttackFreqTime == 0)
        m_pClientList[iClientH]->m_dwAttackFreqTime = dwClientTime;
    else
    {
        dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwAttackFreqTime;
        m_pClientList[iClientH]->m_dwAttackFreqTime = dwClientTime;

        if (dwTimeGap < 500)
        {
            wsprintf(G_cTxt, "(!) Speed hack suspect(%s)", m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, true, true);

            return false;
        }

        //testcode
        //wsprintf(G_cTxt, "Attack: %d", dwTimeGap);
        //log->info(G_cTxt);
    }

    return false;
}


// v2.171
bool CGame::bCheckClientMagicFrequency(int iClientH, uint32_t dwClientTime)
{
    uint32_t dwTimeGap;

    if (m_pClientList[iClientH] == 0) return false;

    if (m_pClientList[iClientH]->m_dwMagicFreqTime == 0)
        m_pClientList[iClientH]->m_dwMagicFreqTime = dwClientTime;
    else
    {
        dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwMagicFreqTime;
        m_pClientList[iClientH]->m_dwMagicFreqTime = dwClientTime;

        if (dwTimeGap < 1500)
        {
            wsprintf(G_cTxt, "(!) Speed hack suspect(%s)", m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, true, true);

            return false;
        }

        //testcode
        //wsprintf(G_cTxt, "Magic: %d", dwTimeGap);
        //log->info(G_cTxt);
    }

    return false;
}

// v2.171
bool CGame::bCheckClientMoveFrequency(int iClientH, uint32_t dwClientTime)
{
    uint32_t dwTimeGap;

    if (m_pClientList[iClientH] == 0) return false;

    if (m_pClientList[iClientH]->m_dwMoveFreqTime == 0)
        m_pClientList[iClientH]->m_dwMoveFreqTime = dwClientTime;
    else
    {
        if (m_pClientList[iClientH]->m_bIsMoveBlocked == true)
        {
            m_pClientList[iClientH]->m_dwMoveFreqTime = 0;
            m_pClientList[iClientH]->m_bIsMoveBlocked = false;
            return false;
        }


        if (m_pClientList[iClientH]->m_bIsAttackModeChange == true)
        {
            m_pClientList[iClientH]->m_dwMoveFreqTime = 0;
            m_pClientList[iClientH]->m_bIsAttackModeChange = false;
            return false;
        }

        dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwMoveFreqTime;
        m_pClientList[iClientH]->m_dwMoveFreqTime = dwClientTime;

//         if (dwTimeGap < 250)
//         {
//             wsprintf(G_cTxt, "(!) Speed hack suspect(%s)", m_pClientList[iClientH]->m_cCharName);
//             log->info(G_cTxt);
//             DeleteClient(iClientH, true, true);
// 
//             return false;
//         }

        //testcode
        //wsprintf(G_cTxt, "Move: %d", dwTimeGap);
        //log->info(G_cTxt);
    }

    return false;
}

// v2.17 2002-7-15 ¿äÀÏ¿¡ °ü°è¾øÀÌ °­ÄÝ ½Ã°£À» Á¶Àý ÇÒ ¼ö ÀÖ°Ô ¼öÁ¤ÇÑ´Ù.
void CGame::SetForceRecallTime(int iClientH)
{
    int iTL_ = 0;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0)
    {





        if (m_sForceRecallTime > 0)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sForceRecallTime;
        }
        else
        {
            GetLocalTime(&SysTime);
            switch (SysTime.wDayOfWeek)
            {
                // v2.19 2002-11-28
#ifndef DEF_CONSULTATION
                case 1:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 3; break;  //¿ù¿äÀÏ  1 2002-09-10 #1
                case 2:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 3; break;  //È­¿äÀÏ  1 
                case 3:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 3; break;  //¼ö¿äÀÏ  1 
                case 4:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 3; break;  //¸ñ¿äÀÏ  1ºÐ 
#else 
                case 1:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 1; break;  //¿ù¿äÀÏ  1 2002-09-10 #1
                case 2:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 1; break;  //È­¿äÀÏ  1 
                case 3:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 1; break;  //¼ö¿äÀÏ  1 
                case 4:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 1; break;  //¸ñ¿äÀÏ  1ºÐ 

#endif 
                case 5:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 15; break; //±Ý¿äÀÏ 15ºÐ
                case 6:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 45; break; //Åä¿äÀÏ 45ºÐ 
                case 0:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 60; break; //ÀÏ¿äÀÏ 60ºÐ
            }
        }
    }
    else
    { // if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0) 
        if (m_sForceRecallTime > 0)
        {
            iTL_ = 20 * m_sForceRecallTime;
        }
        else
        {


            GetLocalTime(&SysTime);
            switch (SysTime.wDayOfWeek)
            {
                case 1:	iTL_ = 20 * 1; break;  //¿ù¿äÀÏ  1ºÐ 2002-09-10 #1
                case 2:	iTL_ = 20 * 1; break;  //È­¿äÀÏ  1ºÐ
                case 3:	iTL_ = 20 * 1; break;  //¼ö¿äÀÏ  1ºÐ
                case 4:	iTL_ = 20 * 1; break;  //¸ñ¿äÀÏ  1ºÐ
                case 5:	iTL_ = 20 * 15; break; //±Ý¿äÀÏ 15ºÐ
                case 6:	iTL_ = 20 * 45; break; //Åä¿äÀÏ 45ºÐ 
                case 0:	iTL_ = 20 * 60; break; //ÀÏ¿äÀÏ 60ºÐ
            }
        }

        if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > iTL_)
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;

    }

    return;
}

// 2002-11-14 »ç³É²Û ¸ðµå Ãß°¡
void CGame::RequestHuntmode(int iClientH)
{
#ifndef DEF_V219
    return;
#endif

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iPKCount > 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (memcmp(m_pClientList[iClientH]->m_cMapName, "cityhall", 8) != 0) return;

    if ((m_pClientList[iClientH]->m_iLevel > DEF_LIMITHUNTERLEVEL) && (m_pClientList[iClientH]->m_bIsHunter == false)) return;


    if (memcmp(m_pClientList[iClientH]->m_cLocation, "aresden", 7) == 0)
    {
        strcpy(m_pClientList[iClientH]->m_cLocation, "arehunter");
    }
    else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvine", 6) == 0)
    {
        strcpy(m_pClientList[iClientH]->m_cLocation, "elvhunter");
    }
    else if (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0)
    {
        strcpy(m_pClientList[iClientH]->m_cLocation, "aresden");
    }
    else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0)
    {
        strcpy(m_pClientList[iClientH]->m_cLocation, "elvine");
    }

    if (m_pClientList[iClientH]->m_bIsHunter == true)
        m_pClientList[iClientH]->m_bIsHunter = false;
    else
        m_pClientList[iClientH]->m_bIsHunter = true;


    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RESPONSE_HUNTMODE, 0, 0, 0, m_pClientList[iClientH]->m_cLocation);


    SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
}

void CGame::SetNoHunterMode(int iClientH, bool bSendMsg)
{
#ifndef DEF_V219
    return;
#endif

    if (m_pClientList[iClientH] == 0) return;

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0)
    {
        strcpy(m_pClientList[iClientH]->m_cLocation, "aresden");
    }
    else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0)
    {
        strcpy(m_pClientList[iClientH]->m_cLocation, "elvine");
    }

    if (m_pClientList[iClientH]->m_bIsHunter == true)
        m_pClientList[iClientH]->m_bIsHunter = false;

    if (bSendMsg == true)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RESPONSE_HUNTMODE, 0, 0, 0, m_pClientList[iClientH]->m_cLocation);


        SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

    }

}

// 2002-11-15 Å¬¶óÀÌ¾ðÆ®ÀÇ Side, OnTown, OnShop Á¤º¸¸¦ ¼³Á¤ÇÑ´Ù.
int CGame::iSetSide(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return -1;

    char	cLocationName[12], cMapLocationName[12];

    memset(cLocationName, 0, sizeof(cLocationName));
    memset(cMapLocationName, 0, sizeof(cMapLocationName));

    strcpy(cLocationName, m_pClientList[iClientH]->m_cLocation);
    strcpy(cMapLocationName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName);




    m_pClientList[iClientH]->m_cSide = DEF_NETURAL; // v2.172
    m_pClientList[iClientH]->m_iIsOnTown = DEF_PK;
    m_pClientList[iClientH]->m_bIsHunter = false;





    if (memcmp(cLocationName, cMapLocationName, 3) == 0)
        m_pClientList[iClientH]->m_iIsOnTown = DEF_NONPK;

    if (memcmp(cLocationName, "are", 3) == 0)
    {
        m_pClientList[iClientH]->m_cSide = DEF_ARESDEN;
    }
    else if (memcmp(cLocationName, "elv", 3) == 0)
    {
        m_pClientList[iClientH]->m_cSide = DEF_ELVINE;
    }
    else
    {
        if ((strcmp(cMapLocationName, "elvine") == 0)
            || (strcmp(cMapLocationName, "aresden") == 0))
        {
            m_pClientList[iClientH]->m_iIsOnTown = DEF_NONPK;
        }
        m_pClientList[iClientH]->m_bIsNeutral = true;
    }

#ifdef DEF_V219
    if ((memcmp(cLocationName, "arehunter", 9) == 0) || (memcmp(cLocationName, "elvhunter", 9) == 0))
    {
        m_pClientList[iClientH]->m_bIsHunter = true;
    }
#endif


    if ((memcmp(m_pClientList[iClientH]->m_cMapName, "bisle", 5) == 0))
    {
        m_pClientList[iClientH]->m_bIsHunter = false;
    }

    if ((memcmp(m_pClientList[iClientH]->m_cMapName, "bsmith", 6) == 0) ||
        (memcmp(m_pClientList[iClientH]->m_cMapName, "gldhall", 7) == 0) ||
        (memcmp(m_pClientList[iClientH]->m_cMapName, "gshop", 5) == 0))
        m_pClientList[iClientH]->m_bIsOnShop = true;
    else
        m_pClientList[iClientH]->m_bIsOnShop = false;



    if ((memcmp(m_pClientList[iClientH]->m_cMapName, "wrhus", 5) == 0)
        || (memcmp(m_pClientList[iClientH]->m_cMapName, "arewrhus", 8) == 0)
        || (memcmp(m_pClientList[iClientH]->m_cMapName, "elvwrhus", 8) == 0)
        )
        m_pClientList[iClientH]->m_bIsOnWarehouse = true;
    else
        m_pClientList[iClientH]->m_bIsOnWarehouse = false;


    if (memcmp(m_pClientList[iClientH]->m_cMapName, "wzdtwr", 6) == 0)
        m_pClientList[iClientH]->m_bIsOnTower = true;
    else
        m_pClientList[iClientH]->m_bIsOnTower = false;

#ifdef DEF_ALLPLAYERPROTECT

    if (memcmp(cMapLocationName, "2ndmiddle", 9) == 0)
    {
        m_pClientList[iClientH]->m_iIsOnTown = DEF_NEVERNONPK;
    }
#endif

    return m_pClientList[iClientH]->m_cSide;
}


//v2.19 2002-12-16 ³ó»ç½ºÅ³..
bool CGame::__bSetAgricultureItem(int iMapIndex, int dX, int dY, int iType, int iSsn, int iClientH)
{
    int iNamingValue, ix, iy, tX, tY;
    char cNpcName[21], cName[21], cNpcWaypoint[11], cOwnerType;
    short sOwnerH;
    BOOL	iRet;

    //
    //	if ((m_bIsCrusadeMode == false) || (m_pClientList[iClientH]->m_iCrusadeDuty != 2)) return false;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAgriculture >= DEF_MAXAGRICULTURE)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOMOREAGRICULTURE, 0, 0, 0, 0);
        return false;
    }

    if (iSsn > m_pClientList[iClientH]->m_cSkillMastery[2]) //¾µ¼ö°¡ ¾ø´Ù.. ½ºÅ³ Á¦ÇÑ¿¡ °É¸°´Ù..
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_AGRICULTURESKILLLIMIT, 0, 0, 0, 0);  //??
        return false;
    }




    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue == -1)
    {

    }
    else
    {


        //		for (ix = dX -1; ix <= dX +3; ix++)
        //		for (iy = dY -1; iy <= dY +3; iy++) {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
        if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_NPC) && (m_pNpcList[sOwnerH]->m_cActionLimit == 5))
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_AGRICULTURENOAREA, 0, 0, 0, 0);
            return false;
        }
        // 		}

        //³ó»ç¸¦ ÁöÀ»¼ö ÀÖ´Â ÀÚ¸®°¡ ¾Æ´Ï´Ù.
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetIsFarm(dX, dY) == false)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_AGRICULTURENOAREA, 0, 0, 0, 0);
            return false;
        }

        memset(cNpcName, 0, sizeof(cNpcName));
        strcpy(cNpcName, "Crops");

        memset(cName, 0, sizeof(cName));
        wsprintf(cName, "XX%d", iNamingValue);
        cName[0] = '_';
        cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        memset(cNpcWaypoint, 0, sizeof(cNpcWaypoint));

        tX = (int)dX;
        tY = (int)dY;

        iRet = bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0,
            DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, 0, false, 0 /*Áß¸³*/, false, true);

        if (iRet == false)
        {

            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
        else
        {

            if (m_pNpcList[iRet] == 0)
            {
                return false;
            }

            m_pNpcList[iRet]->m_iNpcCrops = iType;


            switch (iType)
            {
                case 1: //¼ö¹Ú
                    m_pNpcList[iRet]->m_iCropsSkillLV = iSsn;
                    break;
                case 2: //È£¹Ú
                    m_pNpcList[iRet]->m_iCropsSkillLV = iSsn;
                    break;
                case 3: //¸¶´Ã
                    m_pNpcList[iRet]->m_iCropsSkillLV = iSsn;
                    break;
                case 4: //º¸¸®
                    m_pNpcList[iRet]->m_iCropsSkillLV = iSsn;
                    break;

                default: //¾ø´Â ÀÛ¹°...
                    m_pNpcList[iRet]->m_iCropsSkillLV = 100;
                    break;
            }

            m_pNpcList[iRet]->m_sAppr2 = iType << 8 | 1;
            //			m_pNpcList[iRet]->m_sAppr2 = (char)1;


            SendEventToNearClient_TypeA(iRet, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);


            wsprintf(G_cTxt, "plant(%s) Agriculture begin(%d,%d) sum(%d)!", cNpcName, tX, tY,
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAgriculture);
            log->info(G_cTxt);
            return true;
        }
    }

    return false;
}

//v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã :: ¾ÆÀÌÅÛÀ» ¶³¾îÆ®¸°´Ù..
//////////////////////////////////////////////////////////////////////////////////////////////
//		bool bCropsItemDrop(int iClientH, short iTargetH,bool bMobDropPos)  
//		bMobDropPos :: ¸÷ À§¿¡ ¾ÆÀÌÅÛÀ» ¶³¾î Æ®¸°´Ù. (true)
//////////////////////////////////////////////////////////////////////////////////////////////
bool CGame::bCropsItemDrop(int iClientH, short iTargetH, bool bMobDropPos)
{
    CItem * pItem;
    int	  iItemID = 0;
    int	  iSeedID = 0;

    iSeedID = m_pNpcList[iTargetH]->m_iNpcCrops;

    switch (iSeedID)
    {
        case 1: //¼ö¹Ú
            iItemID = 820;
            GetExp(iClientH, iDice(3, 10));  //°æÇèÄ¡¸¦ ÁØ´Ù.
            break;
        case 2: //È£¹Ú
            iItemID = 821;
            GetExp(iClientH, iDice(3, 10));  //°æÇèÄ¡¸¦ ÁØ´Ù.
            break;
        case 3: //¸¶´Ã
            iItemID = 822;
            GetExp(iClientH, iDice(4, 10));  //°æÇèÄ¡¸¦ ÁØ´Ù.
            break;
        case 4: //º¸¸®
            iItemID = 823;
            GetExp(iClientH, iDice(4, 10));  //°æÇèÄ¡¸¦ ÁØ´Ù.
            break;

        default:
            iItemID = 0;
            break;
    }


    if (iItemID == 0) return false;

    pItem = new CItem;
    if (_bInitItemAttr(pItem, iItemID) == false)
    {
        delete pItem;
        return false;
    }
    else
    {

        if (bMobDropPos == false)
        {

            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                m_pClientList[iClientH]->m_sY, pItem);

            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4
        }
        else
        {
            //³óÀÛ¹° À§¿¡ ¹ö¸°´Ù.
            m_pMapList[m_pNpcList[iTargetH]->m_cMapIndex]->bSetItem(m_pNpcList[iTargetH]->m_sX,
                m_pNpcList[iTargetH]->m_sY,
                pItem);


            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iTargetH]->m_cMapIndex,
                m_pNpcList[iTargetH]->m_sX, m_pNpcList[iTargetH]->m_sY,
                pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color

        }


        //testcode
        //wsprintf(G_cTxt, "Mining Success: %s", cMineralName); 
        //log->info(G_cTxt);
    }

    return true;
}

//v2.19 2002-12-16 ½ºÅ³ ·¹º§°ú ³óÀÛ¹° ·¹º§¿¡ µû¸¥ Å¸°Ý ¼º°ø È®·ü
int SKILLCROPSTABLE[9][9] =
// 20  30  40  50  60  70  80  90 100
{
    42, 34, 27, 21, 16, 12,  9,  7,  6,  //20
        43, 40, 33, 27, 22, 18, 15, 13, 10,	//30
        44, 41, 38, 32, 27, 23, 20, 18, 13,  //40
        45, 42, 39, 36, 31, 27, 24, 22, 15,  //50
        46, 43, 40, 37, 34, 30, 27, 25, 16,  //60
        47, 44, 41, 38, 35, 32, 29, 27, 20,  //70
        48, 45, 42, 39, 36, 33, 30, 28, 23,  //80
        49, 46, 43, 40, 37, 34, 31, 28, 25,  //90
        50, 47, 44, 41, 38, 35, 32, 29, 26  //100
};

//v2.19 2002-12-16 ½ºÅ³ ·¹º§°ú ³óÀÛ¹° ·¹º§¿¡ µû¸¥ ³óÀÛ¹° µå·Ó È®·ü
int CROPSDROPTABLE[9][9] =
// 20  30  40  50  60  70  80  90 100
{
    40,  0,  0,  0,  0,  0,  0,  0,  0,  //20
        41, 38,  0,  0,  0,  0,  0,  0,  0,	//30
        43, 40, 36,  0,  0,  0,  0,  0,  0,  //40
        46, 42, 38, 35,  0,  0,  0,  0,  0,  //50
        50, 45, 41, 37, 33,  0,  0,  0,  0,  //60
        55, 49, 44, 40, 35, 31,  0,  0,  0,  //70
        61, 54, 48, 43, 38, 33, 30,  0,  0,  //80
        68, 60, 53, 47, 42, 37, 32, 28,  0,  //90
        76, 67, 59, 52, 46, 41, 35, 29, 24  //100
};












/////////////////////////////////////////////////////////////////////////////////
//	int bProbabilityTable(int x,int y,int iTable) 
//	ÀÎÀÚ :: x ½ºÅ³ ·¹º§(20-100), y ³óÀÛ¹° ·¹º§(20-100), 
//			iTable 1 ::Å¸°Ý ¼º°ø È®·ü 
//				   2 ::³óÀÛ¹° µå·Ó È®·ü
//			return 0 :: false È®·ü Á¦·Î.
//				   1- 100 ³ª¿Ã È®·ü¿¡ µé¾ú°í Àû¿ë È®·üÀ» ¸®ÅÏ..
/////////////////////////////////////////////////////////////////////////////////
int CGame::bProbabilityTable(int x, int y, int iTable)
{
    int iProb;
    int iX, iY;

    iX = (x - 20) / 10;
    iY = (y - 20) / 10;

    if (iX < 0 || iX > 8) return 0;
    if (iY < 0 || iY > 8) return 0;

    iProb = iDice(1, 100);

    if (iTable == 1)
    {
        if (SKILLCROPSTABLE[iX][iY] >= iProb)
            return SKILLCROPSTABLE[iX][iY];
        else
            return 0;
    }
    else if (iTable == 2)
    {
        if (CROPSDROPTABLE[iX][iY] >= iProb)
            return CROPSDROPTABLE[iX][iY];
        else
            return 0;
    }
    else return 0;
}



void CGame::RemoveOccupyFlags(int iMapIndex)
{
    DWORD dwTime = timeGetTime();
    int i;
    short dX, dY;
    int iDynamicObjectIndex;
    COccupyFlag * iOccupyFlagIndex;
    CTile * pTile;
    int iy, ix;

    if (m_pMapList[iMapIndex] == NULL) return;
    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != FALSE) return;
    if (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != FALSE)
    {
        dX = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sX;
        dY = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_sY;
        pTile = (CTile *)(m_pMapList[iMapIndex]->m_pTile + dX + dY * m_pMapList[iMapIndex]->m_sSizeY);
        m_pMapList[iMapIndex]->m_iTotalOccupyFlags--;
        iDynamicObjectIndex = m_pMapList[iMapIndex]->m_pOccupyFlag[i]->m_iDynamicObjectIndex;
        if (m_pDynamicObjectList[iDynamicObjectIndex] == 0) return;

        SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex,
            m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY,
            m_pDynamicObjectList[iDynamicObjectIndex]->m_sType, iDynamicObjectIndex, NULL);

        m_pMapList[m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex]->SetDynamicObject(NULL, NULL, m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY, dwTime);

        iOccupyFlagIndex = m_pMapList[iMapIndex]->m_pOccupyFlag[i];

        if (m_pDynamicObjectList[iDynamicObjectIndex] == 0)
        {
            for (ix = dX - 2; ix <= dX + 2; ix++)
                for (iy = dY - 2; iy <= dY + 2; iy++)
                {
                    pTile = (CTile *)(m_pMapList[iMapIndex]->m_pTile + ix + iy * m_pMapList[iMapIndex]->m_sSizeY);
                    pTile->m_sOwner = NULL;
                }
        }
    }
}

int CGame::iGetPlayerABSStatus(int iClientH)
{
    int iRet;

    if (m_pClientList[iClientH] == NULL) return 0;

    iRet = 0;

    if (m_pClientList[iClientH]->m_iPKCount != 0)
    {
        iRet = 8;
    }

    if (m_pClientList[iClientH]->m_cSide != 0)
    {
        iRet = iRet | 4;
    }

    if (m_pClientList[iClientH]->m_cSide == 1)
    {
        iRet = iRet | 2;
    }

    if (m_pClientList[iClientH]->m_bIsPlayerCivil == true)
        iRet = iRet | 1;

    return iRet;
}

void CGame::ForceRecallProcess()
{
    int i;
    int iMapSide = 0;

    DWORD * dwp{}, dwTime;

    dwTime = timeGetTime();

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != NULL)
        {
            if (m_pClientList[i]->m_bIsInitComplete == TRUE)
            {
                //force recall in enemy buidlings at crusade
                iMapSide = iGetMapLocationSide(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName);
                if ((memcmp(m_pClientList[i]->m_cLocation, "are", 3) == 0) && (iMapSide == 2) && (m_pClientList[i]->m_iAdminUserLevel == 0) && (m_bIsCrusadeMode == TRUE))
                {
                    RequestTeleportHandler(i, "2   ", "aresden", -1, -1);
                }
                if ((memcmp(m_pClientList[i]->m_cLocation, "elv", 3) == 0) && (iMapSide == 1) && (m_pClientList[i]->m_iAdminUserLevel == 0) && (m_bIsCrusadeMode == TRUE))
                {
                    RequestTeleportHandler(i, "2   ", "elvine", -1, -1);
                }

                //remove mim in building
                if ((memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "wrhus", 5) == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "gshop_1") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "bsmith_1") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "cath_1") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "CmdHall_1") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "cityhall_1") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "gshop_2") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "bsmith_2") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "cath_2") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "CmdHall_2") == 0)
                    || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "cityhall_2") == 0)
                    || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "wzdtwr", 6) == 0)
                    || (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "gldhall", 7) == 0))
                {
                    //SetIllusionFlag(i, DEF_OWNERTYPE_PLAYER, FALSE);
                    if (m_pClientList[i]->m_iStatus & 0x00200000)
                    {
                        SetIllusionMovementFlag(i, DEF_OWNERTYPE_PLAYER, FALSE);
                        bRemoveFromDelayEventList(i, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_CONFUSE);
                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_CONFUSE, dwTime + 2, i, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 4, NULL, NULL);
                    }
                }
            }
            //check gizon errors
            if (m_pClientList[i]->m_iLevel < 180)
            {
                if (m_pClientList[i]->m_iGizonItemUpgradeLeft > 0)
                {
                    m_pClientList[i]->m_iGizonItemUpgradeLeft = 0;
                }
            }
        }
    }
}
void CGame::CritInc(int iClientH)
{
    int iMaxSuperAttack;
    int DEFAULT_CRIT_INC_VALUE = 5;

    iMaxSuperAttack = (m_pClientList[iClientH]->m_iLevel / 10);

    m_pClientList[iClientH]->m_iSuperAttackLeft += DEFAULT_CRIT_INC_VALUE;
    if (m_pClientList[iClientH]->m_iSuperAttackLeft > iMaxSuperAttack)
    {
        m_pClientList[iClientH]->m_iSuperAttackLeft = iMaxSuperAttack;
    }
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SUPERATTACKLEFT, NULL, NULL, NULL, NULL);
}

void CGame::AddGizon(int iClientH)
{
    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft++;
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);

    m_pClientList[iClientH]->m_iNextLevelExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel + 1];
    m_pClientList[iClientH]->m_iExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel] + 1;
}

void CGame::SetTimeOut(int iClientH)
{
    int forcedIn;
    time_t seconds;
    SYSTEMTIME SysTime;

    seconds = time(NULL);
    GetLocalTime(&SysTime);

    if (m_pClientList[iClientH] == NULL) return;

    m_pClientList[iClientH]->m_iForceStart = seconds;
    m_pClientList[iClientH]->isForceSet = TRUE;

    switch (SysTime.wDayOfWeek)
    {
        case 1:	forcedIn = m_sRaidTimeMonday; break;
        case 2:	forcedIn = m_sRaidTimeTuesday; break;
        case 3:	forcedIn = m_sRaidTimeWednesday; break;
        case 4:	forcedIn = m_sRaidTimeThursday; break;
        case 5:	forcedIn = m_sRaidTimeFriday; break;
        case 6:	forcedIn = m_sRaidTimeSaturday; break;
        case 0:	forcedIn = m_sRaidTimeSunday; break;
    }

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FORCERECALLTIME, forcedIn, NULL, NULL, NULL);

}

// todo: fix this
void CGame::CheckTimeOut(int iClientH)
{
    int forcedIn;
    time_t seconds;
    SYSTEMTIME SysTime;

    seconds = time(NULL);
    GetLocalTime(&SysTime);

    if (m_pClientList[iClientH] == NULL) return;

    switch (SysTime.wDayOfWeek)
    {
        case 1:	forcedIn = m_sRaidTimeMonday; break;
        case 2:	forcedIn = m_sRaidTimeTuesday; break;
        case 3:	forcedIn = m_sRaidTimeWednesday; break;
        case 4:	forcedIn = m_sRaidTimeThursday; break;
        case 5:	forcedIn = m_sRaidTimeFriday; break;
        case 6:	forcedIn = m_sRaidTimeSaturday; break;
        case 0:	forcedIn = m_sRaidTimeSunday; break;
    }

    if ((seconds - m_pClientList[iClientH]->m_iForceStart) > (forcedIn * 60))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
        if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0)
        {
            RequestTeleportHandler(iClientH, "2   ", "aresden", -1, -1);
        }
        if (memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0)
        {
            RequestTeleportHandler(iClientH, "2   ", "elvine", -1, -1);
        }
    }

}

void CGame::SkillCheck(int sTargetH)
{
    //magic
    while ((m_pClientList[sTargetH]->m_iMag * 2) < m_pClientList[sTargetH]->m_cSkillMastery[4])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[4]--;
    }
    //hand attack
    while ((m_pClientList[sTargetH]->m_iStr * 2) < m_pClientList[sTargetH]->m_cSkillMastery[5])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[5]--;
    }
    //hammer
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[14])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[14]--;
    }
    //shield
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[11])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[11]--;
    }
    //axe
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[10])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[10]--;
    }
    //fencing
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[9])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[9]--;
    }
    //LS
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[8])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[8]--;
    }
    //SS
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[7])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[7]--;
    }
    //archery
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[6])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[6]--;
    }
    //staff
    while ((m_pClientList[sTargetH]->m_iMag * 2) < m_pClientList[sTargetH]->m_cSkillMastery[21])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[21]--;
    }
    //alc
    while ((m_pClientList[sTargetH]->m_iInt * 2) < m_pClientList[sTargetH]->m_cSkillMastery[12])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[12]--;
    }
    //manu
    while ((m_pClientList[sTargetH]->m_iStr * 2) < m_pClientList[sTargetH]->m_cSkillMastery[13])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[13]--;
    }
    //pr
    while ((m_pClientList[sTargetH]->m_iVit * 2) < m_pClientList[sTargetH]->m_cSkillMastery[23])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[23]--;
    }
    //pc
    while ((m_pClientList[sTargetH]->m_iInt * 2) < m_pClientList[sTargetH]->m_cSkillMastery[19])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[19]--;
    }
    //farming
    while ((m_pClientList[sTargetH]->m_iInt * 2) < m_pClientList[sTargetH]->m_cSkillMastery[2])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[2]--;
    }
    //fishing
    while ((m_pClientList[sTargetH]->m_iDex * 2) < m_pClientList[sTargetH]->m_cSkillMastery[1])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[1]--;
    }
    //mining
    while ((m_pClientList[sTargetH]->m_iStr * 2) < m_pClientList[sTargetH]->m_cSkillMastery[0])
    {
        m_pClientList[sTargetH]->m_cSkillMastery[0]--;
    }
}

BOOL CGame::IsEnemyZone(int i)
{
    if (memcmp(m_pClientList[i]->m_cLocation, "elv", 3) == 0)
    {
        if ((strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "aresden") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "aresdend1") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "areuni") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "huntzone2") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "huntzone4") == 0))
        {
            return TRUE;
        }
    }
    else if (memcmp(m_pClientList[i]->m_cLocation, "are", 3) == 0)
    {
        if ((strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "elvine") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "elvined1") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "elvuni") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "huntzone1") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "huntzone3") == 0))
        {
            return TRUE;
        }
    }
    return FALSE;
}
