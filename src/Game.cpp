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


    m_bIsGameStarted = FALSE;
    m_bIsItemAvailable = FALSE;
    m_bIsBuildItemAvailable = FALSE;
    m_bIsNpcAvailable = FALSE;
    m_bIsMagicAvailable = FALSE;
    m_bIsSkillAvailable = FALSE;
    m_bIsQuestAvailable = FALSE;
    m_bIsPortionAvailable = FALSE;

    ZeroMemory(m_cServerName, sizeof(m_cServerName));

    m_iPlayerMaxLevel = DEF_PLAYERMAXLEVEL;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        m_pClientList[i] = NULL;

    for (i = 0; i < DEF_MAXMAPS; i++)
        m_pMapList[i] = NULL;

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        m_pItemConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        m_pNpcConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXNPCS; i++)
        m_pNpcList[i] = NULL;

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        m_pMagicConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        m_pSkillConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        m_pQuestConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        m_pDynamicObjectList[i] = NULL;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        m_pDelayEventList[i] = NULL;

//     for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
//         m_pNoticeMsgList[i] = NULL;

    for (i = 0; i < DEF_MAXFISHS; i++)
        m_pFish[i] = NULL;

    for (i = 0; i < DEF_MAXMINERALS; i++)
        m_pMineral[i] = NULL;

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        m_pPortionConfigList[i] = NULL;

    //	for (i = 0; i < DEF_MAXTELEPORTTYPE; i++)
    //		m_pTeleportConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        m_pBuildItemList[i] = NULL;

    for (i = 0; i < DEF_MAXDUPITEMID; i++)
        m_pDupItemIDList[i] = NULL;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
    {
        m_stPartyInfo[i].iTotalMembers = 0;
        for (x = 0; x < DEF_MAXPARTYMEMBERS; x++)
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

    m_bIsServerShutdowned = FALSE;
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

    m_bEnemyKillMode = FALSE;
    m_iEnemyKillAdjust = 1;
    m_bAdminSecurity = TRUE;
    m_sRaidTimeMonday = 0;
    m_sRaidTimeTuesday = 0;
    m_sRaidTimeWednesday = 0;
    m_sRaidTimeThursday = 0;
    m_sRaidTimeFriday = 0;
    m_sRaidTimeSaturday = 0;
    m_sRaidTimeSunday = 0;
    m_sCharPointLimit = 0;
    m_sSlateSuccessRate = 0;
    m_sForceRecallTime = 0;

    m_bReceivedItemList = FALSE;
}

CGame::~CGame()
{
}

BOOL CGame::bInit()
{
    char * cp;
    DWORD * dwp;
    WORD * wp;
    int  i;
    SYSTEMTIME SysTime;
    DWORD dwTime = timeGetTime();

    log->info("(!) INITIALIZING GAME SERVER...");

    for (i = 0; i < DEF_MAXCLIENTS + 1; i++)
        m_iClientShortCut[i] = 0;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != NULL) delete m_pClientList[i];

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != NULL) delete m_pNpcList[i];

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL) delete m_pMapList[i];

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != NULL) delete m_pItemConfigList[i];

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        if (m_pNpcConfigList[i] != NULL) delete m_pNpcConfigList[i];

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        if (m_pMagicConfigList[i] != NULL) delete m_pMagicConfigList[i];

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        if (m_pSkillConfigList[i] != NULL) delete m_pSkillConfigList[i];

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        if (m_pQuestConfigList[i] != NULL) delete m_pQuestConfigList[i];

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != NULL) delete m_pDynamicObjectList[i];

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] != NULL) delete m_pDelayEventList[i];

//     for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
//         if (m_pNoticeMsgList[i] != NULL) delete m_pNoticeMsgList[i];

    for (i = 0; i < DEF_MAXFISHS; i++)
        if (m_pFish[i] != NULL) delete m_pFish[i];

    for (i = 0; i < DEF_MAXMINERALS; i++)
        if (m_pMineral[i] != NULL) delete m_pMineral[i];

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        if (m_pPortionConfigList[i] != NULL) delete m_pPortionConfigList[i];

    //	for (i = 0; i < DEF_MAXTELEPORTTYPE; i++)
    //	if (m_pTeleportConfigList[i] != NULL) delete m_pTeleportConfigList[i];

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        if (m_pBuildItemList[i] != NULL) delete m_pBuildItemList[i];

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        m_iNpcConstructionPoint[i] = NULL;


    for (i = 0; i < DEF_MAXSCHEDULE; i++)
    {
        m_stCrusadeWarSchedule[i].iDay = -1;
        m_stCrusadeWarSchedule[i].iHour = -1;
        m_stCrusadeWarSchedule[i].iMinute = -1;
    }

    for (i = 0; i < DEF_MAXAPOCALYPSE; i++)
    {
        m_stApocalypseScheduleStart[i].iDay = -1;
        m_stApocalypseScheduleStart[i].iHour = -1;
        m_stApocalypseScheduleStart[i].iMinute = -1;
    }

    for (i = 0; i < DEF_MAXHELDENIAN; i++)
    {
        m_stHeldenianSchedule[i].iDay = -1;
        m_stHeldenianSchedule[i].StartiHour = -1;
        m_stHeldenianSchedule[i].StartiMinute = -1;
        m_stHeldenianSchedule[i].EndiHour = -1;
        m_stHeldenianSchedule[i].EndiMinute = -1;
    }

    for (i = 0; i < DEF_MAXAPOCALYPSE; i++)
    {
        m_stApocalypseScheduleEnd[i].iDay = -1;
        m_stApocalypseScheduleEnd[i].iHour = -1;
        m_stApocalypseScheduleEnd[i].iMinute = -1;
    }

    m_iNpcConstructionPoint[1] = 100;
    m_iNpcConstructionPoint[2] = 100;
    m_iNpcConstructionPoint[3] = 100;
    m_iNpcConstructionPoint[4] = 100;
    m_iNpcConstructionPoint[5] = 100;
    m_iNpcConstructionPoint[6] = 100;

    m_iNpcConstructionPoint[43] = 1000;
    m_iNpcConstructionPoint[44] = 2000;
    m_iNpcConstructionPoint[45] = 3000;
    m_iNpcConstructionPoint[46] = 2000;
    m_iNpcConstructionPoint[47] = 3000;
    m_iNpcConstructionPoint[51] = 1500;

    m_bIsGameStarted = FALSE;

    m_bIsItemAvailable = FALSE;
    m_bIsBuildItemAvailable = FALSE;
    m_bIsNpcAvailable = FALSE;
    m_bIsMagicAvailable = FALSE;
    m_bIsSkillAvailable = FALSE;
    m_bIsQuestAvailable = FALSE;
    m_bIsPortionAvailable = FALSE;

    ZeroMemory(m_cServerName, sizeof(m_cServerName));

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        m_pClientList[i] = NULL;

    for (i = 0; i < DEF_MAXMAPS; i++)
        m_pMapList[i] = NULL;

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        m_pItemConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        m_pNpcConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXNPCS; i++)
        m_pNpcList[i] = NULL;

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        m_pMagicConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        m_pSkillConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        m_pQuestConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        m_pDynamicObjectList[i] = NULL;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        m_pDelayEventList[i] = NULL;

//     for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
//         m_pNoticeMsgList[i] = NULL;

    for (i = 0; i < DEF_MAXFISHS; i++)
        m_pFish[i] = NULL;

    for (i = 0; i < DEF_MAXMINERALS; i++)
        m_pMineral[i] = NULL;

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        m_pPortionConfigList[i] = NULL;

    //	for (i = 0; i < DEF_MAXTELEPORTTYPE; i++)
    //		m_pTeleportConfigList[i] = NULL;

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        m_pBuildItemList[i] = NULL;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
    {
        ZeroMemory(m_stCrusadeStructures[i].cMapName, sizeof(m_stCrusadeStructures[i].cMapName));
        m_stCrusadeStructures[i].cType = NULL;
        m_stCrusadeStructures[i].dX = NULL;
        m_stCrusadeStructures[i].dY = NULL;
    }

    for (i = 0; i < DEF_MAXADMINS; i++)
    {
        ZeroMemory(m_stAdminList[i].m_cGMName, sizeof(m_stAdminList[i].m_cGMName));
    }

    for (i = 0; i < DEF_MAXBANNED; i++)
    {
        ZeroMemory(m_stBannedList[i].m_cBannedIPaddress, sizeof(m_stBannedList[i].m_cBannedIPaddress));
    }

    for (i = 0; i < DEF_MAXGUILDS; i++)
        m_pGuildTeleportLoc[i].m_iV1 = NULL;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
    {
        m_stMiddleCrusadeStructureInfo[i].cType = NULL;
        m_stMiddleCrusadeStructureInfo[i].cSide = NULL;
        m_stMiddleCrusadeStructureInfo[i].sX = NULL;
        m_stMiddleCrusadeStructureInfo[i].sY = NULL;
    }
    m_iTotalMiddleCrusadeStructures = 0;

    m_pNoticementData = NULL;

    m_iQueneHead = 0;
    m_iQueneTail = 0;

    m_iTotalClients = 0;
    m_iMaxClients = 0;
    m_iTotalMaps = 0;

    m_iTotalGameServerClients = 0;
    m_iTotalGameServerMaxClients = 0;

    m_MaxUserSysTime.wHour = 0;
    m_MaxUserSysTime.wMinute = 0;

    m_bIsServerShutdowned = FALSE;
    m_cShutDownCode = 0;

    m_iMiddlelandMapIndex = -1;
    m_iAresdenMapIndex = -1;
    m_iElvineMapIndex = -1;
    m_iGodHMapIndex = -1;
    m_iBTFieldMapIndex = -1;

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

    m_dwSpecialEventTime = m_dwWhetherTime = m_dwGameTime1 =
        m_dwGameTime2 = m_dwGameTime3 = m_dwGameTime4 = m_dwGameTime5 = m_dwGameTime6 = m_dwFishTime = dwTime;

    m_bIsSpecialEventTime = FALSE;

    GetLocalTime(&SysTime);
    m_dwCanFightzoneReserveTime = dwTime - ((SysTime.wHour % 2) * 60 * 60 + SysTime.wMinute * 60) * 1000;

    for (i = 0; i < DEF_MAXFIGHTZONE; i++)
        m_iFightZoneReserve[i] = 0;

    m_iFightzoneNoForceRecall = 0;

    for (i = 1; i < 200; i++)
        m_iLevelExpTable[i] = iGetLevelExp(i);

    m_iLimitedUserExp = m_iLevelExpTable[DEF_LEVELLIMIT + 1];
    m_iLevelExp20 = m_iLevelExpTable[20];

    m_iGameServerMode = 0;
    if (bReadProgramConfigFile("GServer.cfg") == FALSE)
    {
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! GServer.cfg file contents error!");
        return FALSE;
    }
    if (bReadSettingsConfigFile("GameConfigs\\Settings.cfg") == FALSE)
    {
        ;
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! Settings.cfg file contents error!");
        return FALSE;
    }
    if (bReadAdminListConfigFile("GameConfigs\\AdminList.cfg") == FALSE)
    {
        ;
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! AdminList.cfg file contents error!");
        return FALSE;
    }
    if (bReadBannedListConfigFile("GameConfigs\\BannedList.cfg") == FALSE)
    {
        ;
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! BannedList.cfg file contents error!");
        return FALSE;
    }
    if (bReadAdminSetConfigFile("GameConfigs\\AdminSettings.cfg") == FALSE)
    {
        ;
        log->info(" ");
        log->info("(!!!) CRITICAL ERROR! Cannot execute server! AdminSettings.cfg file contents error!");
        return FALSE;
    }

    m_bF1pressed = m_bF4pressed = m_bF12pressed = m_bF5pressed = FALSE;

    m_bOnExitProcess = FALSE;

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

    m_pNoticementData = NULL;
    m_dwNoticementDataSize = 0;

    m_dwMapSectorInfoTime = dwTime;
    m_iMapSectorInfoUpdateCount = 0;

    m_iCrusadeCount = NULL;
    m_bIsCrusadeMode = FALSE;
    m_bIsApocalypseMode = FALSE;
    m_wServerID_GSS = iDice(1, 65535);

    ZeroMemory(m_cGateServerStockMsg, sizeof(m_cGateServerStockMsg));
    cp = (char *)m_cGateServerStockMsg;
    dwp = (DWORD *)cp;
    *dwp = MSGID_SERVERSTOCKMSG;
    cp += 4;
    wp = (WORD *)cp;
    *wp = DEF_MSGTYPE_CONFIRM;
    cp += 2;

    m_iIndexGSS = 6;
    m_dwCrusadeGUID = NULL;
    m_iCrusadeWinnerSide = NULL;
    m_iLastCrusadeWinner = 0;
    m_sLastHeldenianWinner = 0;
    m_sLastCrusadeDate = -1;
    m_iFinalShutdownCount = 0;
    m_bIsCrusadeWarStarter = FALSE;
    m_bIsApocalypseStarter = FALSE;
    m_iLatestCrusadeDayOfWeek = -1;

    m_bHeldenianInitiated = FALSE;
    m_cHeldenianType = FALSE;
    m_bIsHeldenianMode = FALSE;
    m_bHeldenianRunning = FALSE;
    m_iHeldenianAresdenLeftTower = 0;
    m_cHeldenianModeType = -1;
    m_sLastHeldenianWinner = -1;
    m_iHeldenianAresdenLeftTower = 0;
    m_iHeldenianElvineLeftTower = 0;
    m_iHeldenianAresdenDead = 0;
    m_iHeldenianElvineDead = 0;

    return TRUE;
}

void CGame::DeleteClient(int iClientH, BOOL bSave, BOOL bNotify, BOOL bCountLogout, BOOL bForceCloseConn)
{
    int i, iExH;
    char * cp, cData[120], cTmpMap[30];
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    client_list.erase(m_pClientList[iClientH]->shared_from_this());
    if (m_pClientList[iClientH]->m_bIsInitComplete == TRUE)
    {
        if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 5) == 0)
        {
            wsprintf(G_cTxt, "Char(%s)-Exit(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cMapName);
            log->info(G_cTxt);
        }

        if (m_pClientList[iClientH]->m_bIsExchangeMode == TRUE)
        {
            iExH = m_pClientList[iClientH]->m_iExchangeH;
            _ClearExchangeStatus(iExH);
            _ClearExchangeStatus(iClientH);
        }

        if ((m_pClientList[iClientH]->m_iAllocatedFish != NULL) && (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] != NULL))
            m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;

        if (bNotify == TRUE)
            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, NULL, NULL, NULL);

        RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iWhisperPlayerIndex == iClientH))
            {
                m_pClientList[i]->m_iWhisperPlayerIndex = -1;
                SendNotifyMsg(NULL, i, DEF_NOTIFY_WHISPERMODEOFF, NULL, NULL, NULL, m_pClientList[iClientH]->m_cCharName);
            }

        ZeroMemory(cData, sizeof(cData));
        cp = (char *)cData;
        *cp = GSM_DISCONNECT;
        cp++;
        memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
        cp += 10;
        bStockMsgToGateServer(cData, 11);

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(2, iClientH, DEF_OWNERTYPE_PLAYER,
            m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY);

        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, NULL);
    }

    if ((bSave == TRUE) && (m_pClientList[iClientH]->m_bIsOnServerChange == FALSE))
    {

        if (m_pClientList[iClientH]->m_bIsKilled == TRUE)
        {
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;

            strcpy(cTmpMap, m_pClientList[iClientH]->m_cMapName);

            ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));

            if (m_pClientList[iClientH]->m_cSide == 0)
            {
                strcpy(m_pClientList[iClientH]->m_cMapName, "default");
            }
            else
            {
                if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0)
                {
                    if (m_bIsCrusadeMode == TRUE)
                    {
                        if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0)
                        {
                            ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
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
                    if (m_bIsCrusadeMode == TRUE)
                    {
                        if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0)
                        {
                            ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
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
        else if (bForceCloseConn == TRUE)
        {
            ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
            memcpy(m_pClientList[iClientH]->m_cMapName, "bisle", 5);
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;

            ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
            strcpy(m_pClientList[iClientH]->m_cLockedMapName, "bisle");
            m_pClientList[iClientH]->m_iLockedMapTime = 10 * 60;
        }

        if (m_pClientList[iClientH]->m_bIsObserverMode == TRUE)
        {
            ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
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
                memcpy(m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cLocation, 10);
            }
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;
        }

        if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 5) == 0)
        {
            ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
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
                memcpy(m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cLocation, 10);
            }
            m_pClientList[iClientH]->m_sX = -1;
            m_pClientList[iClientH]->m_sY = -1;
        }

        if (m_pClientList[iClientH]->m_bIsInitComplete == TRUE)
        {
            if (m_pClientList[iClientH]->m_iPartyID != NULL)
            {
                ZeroMemory(cData, sizeof(cData));
                cp = (char *)cData;

                dwp = (DWORD *)cp;
                *dwp = MSGID_PARTYOPERATION;
                cp += 4;

                wp = (WORD *)cp;
                *wp = 4;
                cp += 2;

                wp = (WORD *)cp;
                *wp = iClientH;
                cp += 2;

                memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
                cp += 10;

                wp = (WORD *)cp;
                *wp = m_pClientList[iClientH]->m_iPartyID;
                cp += 2;

                //SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
            }
            if (!save_player_data(m_pClientList[iClientH]->shared_from_this()))
            {
                log->error("Save player data error for [{}]", m_pClientList[iClientH]->m_cCharName);
            }
            //if (bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATALOGOUT, iClientH, bCountLogout) == FALSE) LocalSavePlayerData(iClientH);
        }
        //else bSendMsgToLS(MSGID_REQUEST_NOSAVELOGOUT, iClientH, bCountLogout);
    }
    else
    {
        if (m_pClientList[iClientH]->m_bIsOnServerChange == FALSE)
        {
            if (m_pClientList[iClientH]->m_iPartyID != NULL)
            {
                ZeroMemory(cData, sizeof(cData));
                cp = (char *)cData;

                dwp = (DWORD *)cp;
                *dwp = MSGID_PARTYOPERATION;
                cp += 4;

                wp = (WORD *)cp;
                *wp = 4;
                cp += 2;

                wp = (WORD *)cp;
                *wp = iClientH;
                cp += 2;

                memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
                cp += 10;

                wp = (WORD *)cp;
                *wp = m_pClientList[iClientH]->m_iPartyID;
                cp += 2;

                //SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
            }

            //bSendMsgToLS(MSGID_REQUEST_NOSAVELOGOUT, iClientH, bCountLogout);
        }
        else
        {
            if (m_pClientList[iClientH]->m_iPartyID != NULL)
            {
                ZeroMemory(cData, sizeof(cData));
                cp = (char *)cData;

                dwp = (DWORD *)cp;
                *dwp = MSGID_PARTYOPERATION;
                cp += 4;

                wp = (WORD *)cp;
                *wp = 7;
                cp += 2;

                wp = (WORD *)cp;
                *wp = NULL;
                cp += 2;

                memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
                cp += 10;

                wp = (WORD *)cp;
                *wp = m_pClientList[iClientH]->m_iPartyID;
                cp += 2;

                //SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
            }

            //bSendMsgToLS(MSGID_REQUEST_SETACCOUNTWAITSTATUS, iClientH, FALSE);
        }
    }

    if (m_pClientList[iClientH]->m_iPartyID != NULL)
    {
        for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
            if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == iClientH)
            {
                m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = 0;
                m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers--;
                m_pClientList[iClientH]->m_iPartyID = NULL;
                m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                m_pClientList[iClientH]->m_iReqJoinPartyClientH = NULL;
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

    m_iTotalClients--;

    //client_list.erase(m_pClientList[iClientH]->shared_from_this());

    //delete m_pClientList[iClientH]; // TODO: should this still be commented?
    m_pClientList[iClientH] = NULL;

    RemoveClientShortCut(iClientH);
}

void CGame::SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, DWORD dwMsgID, WORD wMsgType, short sV1, short sV2, short sV3)
{
    int * ip, i, iRet, iShortCutIndex;
    char * cp_a, * cp_s, * cp_sv, cData_All[200], cData_Srt[200], cData_Srt_Av[200];
    DWORD * dwp;
    WORD * wp;
    int * ipStatus, iDumm{};
    short * sp, sRange, sX, sY;
    BOOL  bFlag, cOwnerSend;
    char cKey;
    int iTemp3, iTemp, iTemp2;

    ZeroMemory(cData_All, sizeof(cData_All));
    ZeroMemory(cData_Srt, sizeof(cData_Srt));
    ZeroMemory(cData_Srt_Av, sizeof(cData_Srt_Av));
    ipStatus = (int *)&iDumm;
    cKey = (rand() % 255) + 1;

    dwp = (DWORD *)(cData_All + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (WORD *)(cData_All + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;

    dwp = (DWORD *)(cData_Srt + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (WORD *)(cData_Srt + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;

    dwp = (DWORD *)(cData_Srt_Av + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (WORD *)(cData_Srt_Av + DEF_INDEX2_MSGTYPE);
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
        if (m_pClientList[sOwnerH] == NULL) return;

        switch (wMsgType)
        {
        case DEF_OBJECTNULLACTION:
        case DEF_OBJECTDAMAGE:
        case DEF_OBJECTDYING:
        case DEF_MSGTYPE_CONFIRM:
            cOwnerSend = TRUE;
            break;
        default:
            cOwnerSend = FALSE;
            break;
        }

        wp = (WORD *)cp_a;
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

        ip = (int *)cp_a;
        *ip = m_pClientList[sOwnerH]->m_iApprColor;
        cp_a += 4;

        ip = (int *)cp_a;
        ipStatus = ip;
        *ip = m_pClientList[sOwnerH]->m_iStatus;
        cp_a += 4;

        iTemp3 = m_pClientList[sOwnerH]->m_iStatus & 0x0F0FFFF7F;

        if (wMsgType == DEF_OBJECTNULLACTION)
        {
            if (m_pClientList[sOwnerH]->m_bIsKilled == TRUE)
                *cp_a = 1;
            else *cp_a = 0;
        }
        else *cp_a = 0;
        cp_a++;

        wp = (WORD *)cp_s;
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

        wp = (WORD *)cp_sv;
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

        bFlag = TRUE;
        iShortCutIndex = 0;

        while (bFlag)
        {
            i = m_iClientShortCut[iShortCutIndex];
            iShortCutIndex++;
            if (i == 0) bFlag = FALSE;

            if ((bFlag == TRUE) && (m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))

                if ((m_pClientList[i]->m_cMapIndex == m_pClientList[sOwnerH]->m_cMapIndex) &&
                    (m_pClientList[i]->m_sX >= m_pClientList[sOwnerH]->m_sX - 20 - sRange) &&
                    (m_pClientList[i]->m_sX <= m_pClientList[sOwnerH]->m_sX + 20 + sRange) &&
                    (m_pClientList[i]->m_sY >= m_pClientList[sOwnerH]->m_sY - 16 - sRange) &&
                    (m_pClientList[i]->m_sY <= m_pClientList[sOwnerH]->m_sY + 16 + sRange))
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
                    }

                    iTemp = 0x0FFFFFFF & iTemp;
                    iTemp2 = iGetPlayerABSStatus(sOwnerH, i);
                    iTemp = (iTemp | (iTemp2 << 28));
                    *ipStatus = iTemp;

                    if ((m_pClientList[i]->m_sX >= m_pClientList[sOwnerH]->m_sX - 18) &&
                        (m_pClientList[i]->m_sX <= m_pClientList[sOwnerH]->m_sX + 18) &&
                        (m_pClientList[i]->m_sY >= m_pClientList[sOwnerH]->m_sY - 14) &&
                        (m_pClientList[i]->m_sY <= m_pClientList[sOwnerH]->m_sY + 14))
                    {

                        switch (wMsgType)
                        {
                        case DEF_MSGTYPE_CONFIRM:
                        case DEF_MSGTYPE_REJECT:
                        case DEF_OBJECTNULLACTION:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_All, 43, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_All, 43, cKey);
                            break;

                        case DEF_OBJECTATTACK:
                        case DEF_OBJECTATTACKMOVE:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13, cKey);
                            break;

                        case DEF_OBJECTMAGIC:
                        case DEF_OBJECTDAMAGE:
                        case DEF_OBJECTDAMAGEMOVE:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11, cKey);
                            break;

                        case DEF_OBJECTDYING:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15, cKey);
                            break;

                        default:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 9, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 9, cKey);
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
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_All, 43, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_All, 43, cKey);
                            break;

                        case DEF_OBJECTATTACK:
                        case DEF_OBJECTATTACKMOVE:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13, cKey);
                            break;

                        case DEF_OBJECTMAGIC:
                        case DEF_OBJECTDAMAGE:
                        case DEF_OBJECTDAMAGEMOVE:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11, cKey);
                            break;

                        case DEF_OBJECTDYING:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15, cKey);
                            break;

                        default:
                            if (cOwnerSend == TRUE)
                                iRet = m_pClientList[i]->iSendMsg(cData_All, 43, cKey);
                            else
                                if (i != sOwnerH)
                                    iRet = m_pClientList[i]->iSendMsg(cData_All, 43, cKey);
                            break;
                        }
                    }
                }
        }
    }
    else
    {

        if (m_pNpcList[sOwnerH] == NULL) return;

        wp = (WORD *)cp_a;
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
            if (m_pNpcList[sOwnerH]->m_bIsKilled == TRUE)
                *cp_a = 1;
            else *cp_a = 0;
        }
        else *cp_a = 0;
        cp_a++;

        wp = (WORD *)cp_s;
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

        wp = (WORD *)cp_sv;
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

        bFlag = TRUE;
        iShortCutIndex = 0;

        while (bFlag)
        {

            i = m_iClientShortCut[iShortCutIndex];
            iShortCutIndex++;
            if (i == 0) bFlag = FALSE;

            if ((bFlag == TRUE) && (m_pClientList[i] != NULL))

                if ((m_pClientList[i]->m_cMapIndex == m_pNpcList[sOwnerH]->m_cMapIndex) &&
                    (m_pClientList[i]->m_sX >= m_pNpcList[sOwnerH]->m_sX - 20 - sRange) &&
                    (m_pClientList[i]->m_sX <= m_pNpcList[sOwnerH]->m_sX + 20 + sRange) &&
                    (m_pClientList[i]->m_sY >= m_pNpcList[sOwnerH]->m_sY - 16 - sRange) &&
                    (m_pClientList[i]->m_sY <= m_pNpcList[sOwnerH]->m_sY + 16 + sRange))
                {

                    iTemp = *ipStatus;
                    iTemp = 0x0FFFFFFF & iTemp;
                    iTemp2 = iGetNpcRelationship(sOwnerH, i);
                    iTemp = (iTemp | (iTemp2 << 28));
                    *ipStatus = iTemp;

                    if ((m_pClientList[i]->m_sX >= m_pNpcList[sOwnerH]->m_sX - 18) &&
                        (m_pClientList[i]->m_sX <= m_pNpcList[sOwnerH]->m_sX + 18) &&
                        (m_pClientList[i]->m_sY >= m_pNpcList[sOwnerH]->m_sY - 14) &&
                        (m_pClientList[i]->m_sY <= m_pNpcList[sOwnerH]->m_sY + 14))
                    {
                        switch (wMsgType)
                        {
                        case DEF_MSGTYPE_CONFIRM:
                        case DEF_MSGTYPE_REJECT:
                        case DEF_OBJECTNULLACTION:
                            iRet = m_pClientList[i]->iSendMsg(cData_All, 27, cKey);
                            break;

                        case DEF_OBJECTDYING:
                            iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15, cKey);
                            break;

                        case DEF_OBJECTDAMAGE:
                        case DEF_OBJECTDAMAGEMOVE:
                            iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11, cKey);
                            break;

                        case DEF_OBJECTATTACK:
                        case DEF_OBJECTATTACKMOVE:
                            iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13, cKey);
                            break;

                        default:
                            iRet = m_pClientList[i]->iSendMsg(cData_Srt, 9, cKey);
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
                            iRet = m_pClientList[i]->iSendMsg(cData_All, 27, cKey);
                            break;

                        case DEF_OBJECTDYING:
                            iRet = m_pClientList[i]->iSendMsg(cData_Srt, 15, cKey);
                            break;

                        case DEF_OBJECTDAMAGE:
                        case DEF_OBJECTDAMAGEMOVE:
                            iRet = m_pClientList[i]->iSendMsg(cData_Srt, 11, cKey);
                            break;

                        case DEF_OBJECTATTACK:
                        case DEF_OBJECTATTACKMOVE:
                            iRet = m_pClientList[i]->iSendMsg(cData_Srt_Av, 13, cKey);
                            break;

                        default:
                            iRet = m_pClientList[i]->iSendMsg(cData_All, 27, cKey);
                            break;

                        }
                    }
                }
        }
    }
}

void CGame::CheckClientResponseTime()
{
    int i, iPlusTime, iMaxSuperAttack, iValue;
    short sTemp;
    DWORD dwTime;
    short sItemIndex;

    /*
    GetLocalTime(&SysTime);
    switch (SysTime.wDayOfWeek) {
        case 1:	iWarPeriod = 30; break;
        case 2:	iWarPeriod = 30; break;
        case 3:	iWarPeriod = 60; break;
        case 4:	iWarPeriod = 60*2;  break;
        case 5:	iWarPeriod = 60*5;  break;
        case 6:	iWarPeriod = 60*10; break;
        case 0:	iWarPeriod = 60*20; break;
    }
    */

    dwTime = timeGetTime();
    std::unique_lock<std::recursive_mutex> l(client_list_mtx);

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != NULL)
        {

            if ((dwTime - m_pClientList[i]->m_dwTime) > DEF_CLIENTTIMEOUT)
            {
                if (m_pClientList[i]->m_bIsInitComplete == TRUE)
                {
                    wsprintf(G_cTxt, "Client Timeout: %s", m_pClientList[i]->m_cIPaddress);
                    log->info(G_cTxt);

                    DeleteClient(i, TRUE, TRUE);
                    continue;
                }
                else if ((dwTime - m_pClientList[i]->m_dwTime) > DEF_CLIENTTIMEOUT)
                {
                    DeleteClient(i, FALSE, FALSE);
                    continue;
                }
            }
            else if (m_pClientList[i]->m_bIsInitComplete == TRUE)
            {
                m_pClientList[i]->m_iTimeLeft_ShutUp--;
                if (m_pClientList[i]->m_iTimeLeft_ShutUp < 0) m_pClientList[i]->m_iTimeLeft_ShutUp = 0;

                m_pClientList[i]->m_iTimeLeft_Rating--;
                if (m_pClientList[i]->m_iTimeLeft_Rating < 0) m_pClientList[i]->m_iTimeLeft_Rating = 0;

                if (((dwTime - m_pClientList[i]->m_dwHungerTime) > DEF_HUNGERTIME) && (m_pClientList[i]->m_bIsKilled == FALSE))
                {
                    if ((m_pClientList[i]->m_iLevel < DEF_LEVELLIMIT) || (m_pClientList[i]->m_iAdminUserLevel >= 1))
                    {
                    }
                    else m_pClientList[i]->m_iHungerStatus--;
                    if (m_pClientList[i]->m_iHungerStatus <= 0) m_pClientList[i]->m_iHungerStatus = 0;
                    m_pClientList[i]->m_dwHungerTime = dwTime;

                    if ((m_pClientList[i]->m_iHP > 0) && (m_pClientList[i]->m_iHungerStatus < 30))
                    {
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_HUNGER, m_pClientList[i]->m_iHungerStatus, NULL, NULL, NULL);
                    }
                }

//                 if (_bCheckCharacterData(i) == FALSE)
//                 {
//                     DeleteClient(i, TRUE, TRUE);
//                     break;
//                 }

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

                if ((m_pClientList[i]->m_bIsPoisoned == TRUE) && ((dwTime - m_pClientList[i]->m_dwPoisonTime) > DEF_POISONTIME))
                {
                    PoisonEffect(i, NULL);
                    m_pClientList[i]->m_dwPoisonTime = dwTime;
                }

                if ((m_pMapList[m_pClientList[i]->m_cMapIndex]->m_bIsFightZone == FALSE) &&
                    ((dwTime - m_pClientList[i]->m_dwAutoSaveTime) > (DWORD)DEF_AUTOSAVETIME))
                {
                    if (!save_player_data(m_pClientList[i]->shared_from_this()))
                    {
                        log->error("Save player data error for [{}]", m_pClientList[i]->m_cCharName);
                    }
                    //bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA, i);
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
                    iValue = (m_pClientList[i]->m_iLevel / 2);
                    if (iValue <= 0) iValue = 1;
                    if (m_pClientList[i]->m_iAutoExpAmount < iValue)
                    {
                        if ((m_pClientList[i]->m_iExp + iValue) < m_iLevelExpTable[m_pClientList[i]->m_iLevel + 1])
                        {
                            GetExp(i, iValue, FALSE);
                            CalcExpStock(i);
                        }
                    }

                    m_pClientList[i]->m_iAutoExpAmount = 0;
                    m_pClientList[i]->m_dwAutoExpTime = dwTime;
                }

                if (m_pClientList[i]->m_iSpecialAbilityTime == 3)
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_SPECIALABILITYENABLED, NULL, NULL, NULL, NULL);
                    sItemIndex = m_pClientList[i]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                    if (sItemIndex != -1)
                    {
                        if ((m_pClientList[i]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[i]->m_pItemList[sItemIndex]->m_sIDnum == 866))
                        {
                            if (m_pClientList[i]->m_iInt > 99 && m_pClientList[i]->m_iMag > 99)
                            {
                                m_pClientList[i]->m_cMagicMastery[94] = TRUE;
                                SendNotifyMsg(NULL, i, DEF_NOTIFY_STATECHANGE_SUCCESS, NULL, NULL, NULL, NULL);
                            }
                        }
                    }
                }
                m_pClientList[i]->m_iSpecialAbilityTime -= 3;
                if (m_pClientList[i]->m_iSpecialAbilityTime < 0) m_pClientList[i]->m_iSpecialAbilityTime = 0;

                if (m_pClientList[i]->m_bIsSpecialAbilityEnabled == TRUE)
                {
                    if (((dwTime - m_pClientList[i]->m_dwSpecialAbilityStartTime) / 1000) > m_pClientList[i]->m_iSpecialAbilityLastSec)
                    {
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_SPECIALABILITYSTATUS, 3, NULL, NULL, NULL);
                        m_pClientList[i]->m_bIsSpecialAbilityEnabled = FALSE;
                        m_pClientList[i]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
                        sTemp = m_pClientList[i]->m_sAppr4;
                        sTemp = sTemp & 0xFF0F;
                        m_pClientList[i]->m_sAppr4 = sTemp;
                        SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                    }
                }

                m_pClientList[i]->m_iLockedMapTime -= 3;
                if (m_pClientList[i]->m_iLockedMapTime < 0)
                {
                    m_pClientList[i]->m_iLockedMapTime = 0;
                    ZeroMemory(m_pClientList[i]->m_cLockedMapName, sizeof(m_pClientList[i]->m_cLockedMapName));
                    strcpy(m_pClientList[i]->m_cLockedMapName, "NONE");
                }

                m_pClientList[i]->m_iDeadPenaltyTime -= 3;
                if (m_pClientList[i]->m_iDeadPenaltyTime < 0) m_pClientList[i]->m_iDeadPenaltyTime = 0;

                if ((m_pClientList[i]->m_bIsWarLocation == TRUE) && IsEnemyZone(i) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {
                    if (m_bIsCrusadeMode == FALSE)
                        if (m_pClientList[i]->m_bIsInsideOwnTown == FALSE)
                            m_pClientList[i]->m_iTimeLeft_ForceRecall--;

                    if (m_pClientList[i]->m_iTimeLeft_ForceRecall <= 0)
                    {
                        m_pClientList[i]->m_iTimeLeft_ForceRecall = 0;
                        m_pClientList[i]->m_dwWarBeginTime = dwTime;
                        m_pClientList[i]->m_bIsWarLocation = FALSE;

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                        RequestTeleportHandler(i, "1   ");
                    }
                }

                if ((m_bIsHeldenianMode == TRUE) && (m_pMapList[m_pClientList[i]->m_cMapIndex] != 0))
                {
                    if (bCheckHeldenianMap(i, m_iBTFieldMapIndex, DEF_OWNERTYPE_PLAYER) == 1)
                    {
                        SetHeroFlag(i, DEF_OWNERTYPE_PLAYER, TRUE);
                    }
                    else
                    {
                        SetHeroFlag(i, DEF_OWNERTYPE_PLAYER, FALSE);
                    }
                }

                if (m_pClientList[i] == NULL) break;
                if (m_pClientList[i]->m_iSkillMsgRecvCount >= 2)
                {
                    DeleteClient(i, TRUE, TRUE);
                }
                else
                {
                    m_pClientList[i]->m_iSkillMsgRecvCount = 0;
                }

                if (m_pClientList[i] == NULL) break;
                if ((m_pClientList[i]->m_iLevel < m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iLevelLimit) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                    RequestTeleportHandler(i, "0   ");
                }

                if (m_pClientList[i] == NULL) break;
                if ((m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iUpperLevelLimit != 0) &&
                    (m_pClientList[i]->m_iLevel > m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iUpperLevelLimit) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                    if ((m_pClientList[i]->m_cSide == 1) &&
                        (m_pClientList[i]->m_iAdminUserLevel == 0))
                    {
                        RequestTeleportHandler(i, "2   ", "aresden", -1, -1);
                    }
                    else if ((m_pClientList[i]->m_cSide == 2) &&
                        (m_pClientList[i]->m_iAdminUserLevel == 0))
                    {
                        RequestTeleportHandler(i, "2   ", "elvine", -1, -1);
                    }
                }

                if (m_pClientList[i] == NULL) break;
                if ((strcmp(m_pClientList[i]->m_cLocation, "elvine") != 0) &&
                    (strcmp(m_pClientList[i]->m_cLocation, "elvhunter") != 0) &&
                    (strcmp(m_pClientList[i]->m_cLocation, "arehunter") != 0) &&
                    (strcmp(m_pClientList[i]->m_cLocation, "aresden") != 0) &&
                    (m_pClientList[i]->m_iLevel >= 20) &&
                    (m_pClientList[i]->m_iAdminUserLevel == 0))
                {
                    wsprintf(G_cTxt, "Traveller Hack: (%s) Player: (%s) is a traveller and is greater than level 19.", m_pClientList[i]->m_cIPaddress, m_pClientList[i]->m_cCharName);
                    log->info(G_cTxt);
                    DeleteClient(i, TRUE, TRUE);
                }

                if (m_pClientList[i] == NULL) break;
                if ((m_pMapList[m_pClientList[i]->m_cMapIndex]->m_bIsApocalypseMap == TRUE) &&
                    (m_bIsApocalypseMode == FALSE) &&
                    (m_pClientList[i]->m_iAdminUserLevel == 0))
                {
                    RequestTeleportHandler(i, "1   ");
                }

                /*
                if (m_pClientList[i] == NULL) break;
                if (((memcmp(m_pClientList[i]->m_cLocation, "aresden",7) == 0) || (memcmp(m_pClientList[i]->m_cLocation, "elvine",6) == 0)) ) {
                    iMapside = iGetMapLocationSide(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName);
                    if (iMapside > 3) iMapside2 = iMapside - 2;
                    else iMapside2 = iMapside;

                    if ((m_pClientList[i]->m_cSide != iMapside2) && (iMapside != 0)) {
                        if ((iMapside <= 2) && (m_pClientList[i]->m_iAdminUserLevel < 1)) {
                            if (m_pClientList[i]->m_cSide != 0) {
                                m_pClientList[i]->m_dwWarBeginTime = timeGetTime();
                                m_pClientList[i]->m_bIsWarLocation = TRUE;
                                m_pClientList[i]->m_iTimeLeft_ForceRecall = 1;

                                RequestTeleportHandler(i, "1   ");
                                SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                            }
                        }
                    }
                }*/

                if (m_pClientList[i] == NULL) break;
                if (((memcmp(m_pClientList[i]->m_cLocation, "arehunter", 9) == 0) || (memcmp(m_pClientList[i]->m_cLocation, "elvhunter", 9) == 0)) &&
                    ((strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "2ndmiddle") == 0) || (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "middleland") == 0)))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                    RequestTeleportHandler(i, "1   ");
                }

                if (m_bIsApocalypseMode == TRUE)
                {
                    if (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "abaddon", 7) == 0)
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_APOCGATEOPEN, 167, 169, NULL, m_pClientList[i]->m_cMapName);
                    else if (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "icebound", 8) == 0)
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_APOCGATEOPEN, 89, 31, NULL, m_pClientList[i]->m_cMapName);
                }

                if (m_pClientList[i] == NULL) break;
                if ((m_bIsApocalypseMode == TRUE) &&
                    (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "icebound", 8) == 0) &&
                    ((m_pClientList[i]->m_sX == 89 && m_pClientList[i]->m_sY == 31) ||
                        (m_pClientList[i]->m_sX == 89 && m_pClientList[i]->m_sY == 32) ||
                        (m_pClientList[i]->m_sX == 90 && m_pClientList[i]->m_sY == 31) ||
                        (m_pClientList[i]->m_sX == 90 && m_pClientList[i]->m_sY == 32)))
                {
                    RequestTeleportHandler(i, "2   ", "druncncity", -1, -1);
                }

                if (m_pClientList[i] == NULL) break;
                if ((memcmp(m_pClientList[i]->m_cLocation, "are", 3) == 0) &&
                    (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "elvfarm") == 0) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                    RequestTeleportHandler(i, "0   ");
                }

                if (m_pClientList[i] == NULL) break;
                if ((memcmp(m_pClientList[i]->m_cLocation, "elv", 3) == 0) &&
                    (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "arefarm") == 0) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                    RequestTeleportHandler(i, "0   ");
                }

                if (m_pClientList[i] == NULL) break;
                if ((strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "middleland") == 0)
                    && (strcmp(m_pClientList[i]->m_cLocation, "NONE") == 0) &&
                    (m_pClientList[i]->m_iAdminUserLevel < 1))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                    RequestTeleportHandler(i, "0   ");
                }

                if ((m_pClientList[i]->m_bInRecallImpossibleMap == TRUE) && (m_pClientList[i]->m_iAdminUserLevel < 1))
                {
                    m_pClientList[i]->m_iTimeLeft_ForceRecall--;
                    if (m_pClientList[i]->m_iTimeLeft_ForceRecall <= 0)
                    {
                        m_pClientList[i]->m_iTimeLeft_ForceRecall = 0;
                        m_pClientList[i]->m_bInRecallImpossibleMap = FALSE;
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_TOBERECALLED, NULL, NULL, NULL, NULL);
                        RequestTeleportHandler(i, "0   ");
                    }
                }

                if (m_pClientList[i] == NULL) break;
                m_pClientList[i]->m_iSuperAttackCount++;
                if (m_pClientList[i]->m_iSuperAttackCount > 12)
                {

                    m_pClientList[i]->m_iSuperAttackCount = 0;
                    iMaxSuperAttack = (m_pClientList[i]->m_iLevel / 10);
                    if (m_pClientList[i]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[i]->m_iSuperAttackLeft++;

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_SUPERATTACKLEFT, NULL, NULL, NULL, NULL);
                }

                m_pClientList[i]->m_iTimeLeft_FirmStaminar--;
                if (m_pClientList[i]->m_iTimeLeft_FirmStaminar < 0) m_pClientList[i]->m_iTimeLeft_FirmStaminar = 0;

                if (m_pClientList[i] == NULL) break;
                if (m_pClientList[i]->m_bIsSendingMapStatus == TRUE) _SendMapStatus(i);

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

int CGame::_iComposePlayerDataFileContents(int iClientH, char * pData)
{
    SYSTEMTIME SysTime;
    char  cTxt[120], cTmp[30];
    POINT TempItemPosList[DEF_MAXITEMS]{};
    int   i, iPos;

    if (m_pClientList[iClientH] == NULL) return 0;

    GetLocalTime(&SysTime);
    strcat(pData, "[FILE-DATE]\n\n");

    wsprintf(cTxt, "file-saved-date: %d %d %d %d %d\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    strcat(pData, "[NAME-ACCOUNT]\n\n");
    strcat(pData, "character-name     = ");
    strcat(pData, m_pClientList[iClientH]->m_cCharName);
    strcat(pData, "\n");
    strcat(pData, "account-name       = ");
    strcat(pData, m_pClientList[iClientH]->m_cAccountName);
    strcat(pData, "\n\n");

    strcat(pData, "[STATUS]\n\n");
    strcat(pData, "character-profile 	=");
    if (strlen(m_pClientList[iClientH]->m_cProfile) == 0)
    {
        strcat(pData, "__________");
    }
    else strcat(pData, m_pClientList[iClientH]->m_cProfile);
    strcat(pData, "\n");

    strcat(pData, "character-location   = ");
    strcat(pData, m_pClientList[iClientH]->m_cLocation);
    strcat(pData, "\n");

    strcat(pData, "character-guild-name = ");
    if (m_pClientList[iClientH]->m_iGuildRank != -1)
    {
        strcat(pData, m_pClientList[iClientH]->m_cGuildName);
    }
    else strcat(pData, "NONE");
    strcat(pData, "\n");

    strcat(pData, "character-guild-GUID = ");
    if (m_pClientList[iClientH]->m_iGuildRank != -1)
    {
        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "%d", m_pClientList[iClientH]->m_iGuildGUID);
        strcat(pData, cTxt);
    }
    else strcat(pData, "-1");
    strcat(pData, "\n");

    strcat(pData, "character-guild-rank = ");
    _itoa(m_pClientList[iClientH]->m_iGuildRank, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    strcat(pData, "character-loc-map = ");
    strcat(pData, m_pClientList[iClientH]->m_cMapName);
    strcat(pData, "\n");
    strcat(pData, "character-loc-x   = ");
    _itoa(m_pClientList[iClientH]->m_sX, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "character-loc-y   = ");
    _itoa(m_pClientList[iClientH]->m_sY, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n\n");
    if (m_pClientList[iClientH]->m_iHP <= 0)
        m_pClientList[iClientH]->m_iHP = 30;

    wsprintf(cTxt, "character-HP       = %d", m_pClientList[iClientH]->m_iHP);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-MP       = %d", m_pClientList[iClientH]->m_iMP);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    if (m_pClientList[iClientH]->m_iSP < 0) m_pClientList[iClientH]->m_iSP = 0;
    wsprintf(cTxt, "character-SP       = %d", m_pClientList[iClientH]->m_iSP);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-LEVEL    = %d", m_pClientList[iClientH]->m_iLevel);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-RATING   = %d", m_pClientList[iClientH]->m_iRating);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-STR      = %d", m_pClientList[iClientH]->m_iStr);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-INT      = %d", m_pClientList[iClientH]->m_iInt);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-VIT      = %d", m_pClientList[iClientH]->m_iVit);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-DEX      = %d", m_pClientList[iClientH]->m_iDex);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-MAG      = %d", m_pClientList[iClientH]->m_iMag);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-CHARISMA = %d", m_pClientList[iClientH]->m_iCharisma);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-LUCK     = %d", m_pClientList[iClientH]->m_iLuck);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-EXP      = %d", m_pClientList[iClientH]->m_iExp);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-LU_Pool  = %d", m_pClientList[iClientH]->m_iLU_Pool);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-EK-Count = %d", m_pClientList[iClientH]->m_iEnemyKillCount);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-PK-Count = %d", m_pClientList[iClientH]->m_iPKCount);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-reward-gold = %d", m_pClientList[iClientH]->m_iRewardGold);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-downskillindex = %d", m_pClientList[iClientH]->m_iDownSkillIndex);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-IDnum1 = %d", m_pClientList[iClientH]->m_sCharIDnum1);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-IDnum2 = %d", m_pClientList[iClientH]->m_sCharIDnum2);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-IDnum3 = %d", m_pClientList[iClientH]->m_sCharIDnum3);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    strcat(pData, "sex-status       = ");
    _itoa(m_pClientList[iClientH]->m_cSex, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "skin-status      = ");
    _itoa(m_pClientList[iClientH]->m_cSkin, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "hairstyle-status = ");
    _itoa(m_pClientList[iClientH]->m_cHairStyle, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "haircolor-status = ");
    _itoa(m_pClientList[iClientH]->m_cHairColor, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "underwear-status = ");
    _itoa(m_pClientList[iClientH]->m_cUnderwear, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "hunger-status    = %d", m_pClientList[iClientH]->m_iHungerStatus);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "timeleft-shutup  = %d", m_pClientList[iClientH]->m_iTimeLeft_ShutUp);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "timeleft-rating  = %d", m_pClientList[iClientH]->m_iTimeLeft_Rating);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "timeleft-force-recall  = %d", m_pClientList[iClientH]->m_iTimeLeft_ForceRecall);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "timeleft-firm-staminar = %d", m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "admin-user-level = %d", m_pClientList[iClientH]->m_iAdminUserLevel);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "penalty-block-date = %d %d %d", m_pClientList[iClientH]->m_iPenaltyBlockYear, m_pClientList[iClientH]->m_iPenaltyBlockMonth, m_pClientList[iClientH]->m_iPenaltyBlockDay);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-quest-number = %d", m_pClientList[iClientH]->m_iQuest);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-quest-ID     = %d", m_pClientList[iClientH]->m_iQuestID);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "current-quest-count    = %d", m_pClientList[iClientH]->m_iCurQuestCount);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "quest-reward-type      = %d", m_pClientList[iClientH]->m_iQuestRewardType);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "quest-reward-amount    = %d", m_pClientList[iClientH]->m_iQuestRewardAmount);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-contribution = %d", m_pClientList[iClientH]->m_iContribution);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-war-contribution = %d", m_pClientList[iClientH]->m_iWarContribution);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "character-quest-completed = %d", (int)m_pClientList[iClientH]->m_bIsQuestCompleted);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "special-event-id = %d", (int)m_pClientList[iClientH]->m_iSpecialEventID);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "super-attack-left = %d", (int)m_pClientList[iClientH]->m_iSuperAttackLeft);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "reserved-fightzone-id = %d %d %d", m_pClientList[iClientH]->m_iFightzoneNumber, m_pClientList[iClientH]->m_iReserveTime, m_pClientList[iClientH]->m_iFightZoneTicketNumber);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "special-ability-time = %d", m_pClientList[iClientH]->m_iSpecialAbilityTime);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "locked-map-name = %s", m_pClientList[iClientH]->m_cLockedMapName);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "locked-map-time = %d", m_pClientList[iClientH]->m_iLockedMapTime);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "crusade-job = %d", m_pClientList[iClientH]->m_iCrusadeDuty);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "crusade-GUID = %d", m_pClientList[iClientH]->m_dwCrusadeGUID);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "construct-point = %d", m_pClientList[iClientH]->m_iConstructionPoint);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "dead-penalty-time = %d", m_pClientList[iClientH]->m_iDeadPenaltyTime);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "party-id = %d", m_pClientList[iClientH]->m_iPartyID);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    wsprintf(cTxt, "gizon-item-upgade-left = %d", m_pClientList[iClientH]->m_iGizonItemUpgradeLeft);
    strcat(pData, cTxt);
    strcat(pData, "\n");

    strcat(pData, "\n\n");

    strcat(pData, "appr1 = ");
    _itoa(m_pClientList[iClientH]->m_sAppr1, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "appr2 = ");
    _itoa(m_pClientList[iClientH]->m_sAppr2, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "appr3 = ");
    _itoa(m_pClientList[iClientH]->m_sAppr3, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "appr4 = ");
    _itoa(m_pClientList[iClientH]->m_sAppr4, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n");
    strcat(pData, "appr-color = ");
    _itoa(m_pClientList[iClientH]->m_iApprColor, cTxt, 10);
    strcat(pData, cTxt);
    strcat(pData, "\n\n");

    strcat(pData, "[ITEMLIST]\n\n");

    for (i = 0; i < DEF_MAXITEMS; i++)
    {
        TempItemPosList[i].x = 40;
        TempItemPosList[i].y = 30;
    }
    iPos = 0;

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
        {
            TempItemPosList[iPos].x = m_pClientList[iClientH]->m_ItemPosList[i].x;
            TempItemPosList[iPos].y = m_pClientList[iClientH]->m_ItemPosList[i].y;
            iPos++;

            strcat(pData, "character-item = ");
            memset(cTmp, ' ', 21);
            strcpy(cTmp, m_pClientList[iClientH]->m_pItemList[i]->m_cName);
            cTmp[strlen(m_pClientList[iClientH]->m_pItemList[i]->m_cName)] = (char)' ';
            cTmp[20] = NULL;
            strcat(pData, cTmp);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_dwCount, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectType, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue1, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue2, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue3, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_cItemColor, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue1, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue2, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue3, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemList[i]->m_dwAttribute, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, "\n");
        }
    strcat(pData, "\n\n");

    for (i = 0; i < DEF_MAXITEMS; i++)
    {
        m_pClientList[iClientH]->m_ItemPosList[i].x = TempItemPosList[i].x;
        m_pClientList[iClientH]->m_ItemPosList[i].y = TempItemPosList[i].y;
    }

    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] != NULL)
        {
            strcat(pData, "character-bank-item = ");
            memset(cTmp, ' ', 21);
            strcpy(cTmp, m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName);
            cTmp[strlen(m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName)] = (char)' ';
            cTmp[20] = NULL;
            strcat(pData, cTmp);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwCount, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectType, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue1, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue2, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue3, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemColor, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue1, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue2, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue3, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_wCurLifeSpan, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, " ");
            _itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwAttribute, cTxt, 10);
            strcat(pData, cTxt);
            strcat(pData, "\n");
        }
    strcat(pData, "\n\n");


    strcat(pData, "[MAGIC-SKILL-MASTERY]\n\n");

    strcat(pData, "//------------------012345678901234567890123456789012345678901234567890");
    strcat(pData, "\n");

    strcat(pData, "magic-mastery     = ");
    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
    {
        wsprintf(cTxt, "%d", (int)m_pClientList[iClientH]->m_cMagicMastery[i]);
        strcat(pData, cTxt);
    }
    strcat(pData, "\n");

    strcat(pData, "skill-mastery     = ");


    for (i = 0; i < 60; i++)
    {
        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_cSkillMastery[i]);

        strcat(pData, cTxt);
    }
    strcat(pData, "\n");

    strcat(pData, "skill-SSN     = ");
    for (i = 0; i < 60; i++)
    {
        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_iSkillSSN[i]);

        strcat(pData, cTxt);
    }
    strcat(pData, "\n");

    strcat(pData, "[ITEM-EQUIP-STATUS]\n\n");
    strcat(pData, "item-equip-status = ");

    ZeroMemory(cTxt, sizeof(cTxt));
    strcpy(cTxt, "00000000000000000000000000000000000000000000000000");

    int iEP = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
        {
            if ((m_pClientList[iClientH]->m_bIsItemEquipped[i] == TRUE) &&
                (m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_EQUIP))
            {
                cTxt[iEP] = '1';
            }
            iEP++;
        }
    strcat(pData, cTxt);
    strcat(pData, "\n");


    strcat(pData, "item-position-x = ");
    for (i = 0; i < DEF_MAXITEMS; i++)
    {
        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_ItemPosList[i].x);
        strcat(pData, cTxt);
    }
    strcat(pData, "\n");

    strcat(pData, "item-position-y = ");
    for (i = 0; i < DEF_MAXITEMS; i++)
    {
        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_ItemPosList[i].y);
        strcat(pData, cTxt);
    }
    strcat(pData, "\n\n");

    strcat(pData, "[EOF]");
    strcat(pData, "\n\n\n\n");

    return strlen(pData);
}

BOOL CGame::_bInitItemAttr(CItem * pItem, char * pItemName)
{
    int i;
    char cTmpName[30];

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pItemName);

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != NULL)
        {
            if (memcmp(cTmpName, m_pItemConfigList[i]->m_cName, 20) == 0)
            {
                ZeroMemory(pItem->m_cName, sizeof(pItem->m_cName));
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

                return TRUE;
            }
        }

    return FALSE;
}

BOOL CGame::_bGetIsStringIsNumber(char * pStr)
{
    int i;
    for (i = 0; i < (int)strlen(pStr); i++)
        if ((pStr[i] != '-') && ((pStr[i] < (char)'0') || (pStr[i] > (char)'9'))) return FALSE;

    return TRUE;
}

BOOL CGame::_bReadMapInfoFiles(int iMapIndex)
{
    if (__bReadMapInfo(iMapIndex) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CGame::bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType)
{
    int i, iIndex, iMapIndex, iFollowIndex;
    char cTmpName[11], cFollowSide;

    ZeroMemory(cTmpName, sizeof(cTmpName));
    iMapIndex = -1;
    iFollowIndex = -1;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if ((m_pNpcList[i] != NULL) && (memcmp(m_pNpcList[i]->m_cName, pName, 5) == 0))
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
            if ((m_pNpcList[i] != NULL) && (memcmp(m_pNpcList[i]->m_cName, pFollowName, 5) == 0))
            {
                if (m_pNpcList[i]->m_cMapIndex != iMapIndex) return FALSE;
                iFollowIndex = i;
                cFollowSide = m_pNpcList[i]->m_cSide;
                goto NEXT_STEP_SNFM2;
            }
        break;

    case DEF_OWNERTYPE_PLAYER:
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, pFollowName, 10) == 0))
            {
                if (m_pClientList[i]->m_cMapIndex != iMapIndex) return FALSE;
                iFollowIndex = i;
                cFollowSide = m_pClientList[i]->m_cSide;
                goto NEXT_STEP_SNFM2;
            }
        break;
    }

    NEXT_STEP_SNFM2:;

    if ((iIndex == -1) || (iFollowIndex == -1)) return FALSE;

    m_pNpcList[iIndex]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
    m_pNpcList[iIndex]->m_cFollowOwnerType = cFollowOwnerType;
    m_pNpcList[iIndex]->m_iFollowOwnerIndex = iFollowIndex;
    m_pNpcList[iIndex]->m_cSide = cFollowSide;


    return TRUE;
}

int CGame::bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType, int * poX, int * poY, char * pWaypointList, RECT * pArea, int iSpotMobIndex, char cChangeSide, BOOL bHideGenMode, BOOL bIsSummoned, BOOL bFirmBerserk, BOOL bIsMaster, int iGuildGUID)
{
    int i, t, j, k, iMapIndex;
    char  cTmpName[11], cTxt[120];
    short sX, sY, sRange;
    BOOL  bFlag;
    SYSTEMTIME SysTime;

    if (strlen(pName) == 0)   return FALSE;
    if (strlen(pNpcName) == 0) return FALSE;
    GetLocalTime(&SysTime);
    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pMapName);
    iMapIndex = -1;

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL)
        {
            if (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0)
                iMapIndex = i;
        }

    if (iMapIndex == -1) return FALSE;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] == NULL)
        {
            m_pNpcList[i] = new CNpc(pName);
            if (_bInitNpcAttr(m_pNpcList[i], pNpcName, sClass, cSA) == FALSE)
            {
                wsprintf(cTxt, "(!) Not existing NPC creation request! (%s) Ignored.", pNpcName);
                log->info(cTxt);

                delete m_pNpcList[i];
                m_pNpcList[i] = NULL;
                return FALSE;
            }

            if (m_pNpcList[i]->m_cDayOfWeekLimit < 10)
            {
                if (m_pNpcList[i]->m_cDayOfWeekLimit != SysTime.wDayOfWeek)
                {
                    delete m_pNpcList[i];
                    m_pNpcList[i] = NULL;
                    return FALSE;
                }
            }

            switch (cMoveType)
            {
            case DEF_MOVETYPE_GUARD:
            case DEF_MOVETYPE_RANDOM:
                if ((poX != NULL) && (poY != NULL) && (*poX != NULL) && (*poY != NULL))
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

                        bFlag = TRUE;
                        for (k = 0; k < DEF_MAXMGAR; k++)
                            if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left != -1)
                            {
                                if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left) &&
                                    (sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].right) &&
                                    (sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].top) &&
                                    (sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].bottom))
                                {
                                    bFlag = FALSE;
                                }
                            }
                        if (bFlag == TRUE) goto GET_VALIDLOC_SUCCESS;

                    }
                    delete m_pNpcList[i];
                    m_pNpcList[i] = NULL;
                    return FALSE;

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
                if ((poX != NULL) && (poY != NULL) && (*poX != NULL) && (*poY != NULL))
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

            if (bGetEmptyPosition(&sX, &sY, iMapIndex) == FALSE)
            {

                delete m_pNpcList[i];
                m_pNpcList[i] = NULL;
                return FALSE;
            }

            if ((bHideGenMode == TRUE) && (_iGetPlayerNumberOnSpot(sX, sY, iMapIndex, 7) != 0))
            {
                delete m_pNpcList[i];
                m_pNpcList[i] = NULL;
                return FALSE;
            }

            if ((poX != NULL) && (poY != NULL))
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

            if (pArea != NULL)
            {
                SetRect(&m_pNpcList[i]->m_rcRandomArea, pArea->left, pArea->top, pArea->right, pArea->bottom);
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

            m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
            m_pNpcList[i]->m_iTargetIndex = NULL;
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
                m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | ((rand() % 13) << 4);
                m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | (rand() % 9);
                break;

            case 36:
            case 37:
            case 38:
            case 39:
                m_pNpcList[i]->m_sAppr2 = 3;
                break;

            case 64:
                m_pNpcList[i]->m_sAppr2 = 1;
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
            if (bIsSummoned == TRUE)
                m_pNpcList[i]->m_dwSummonedTime = timeGetTime();

            if (bFirmBerserk == TRUE)
            {
                m_pNpcList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = 1;
                m_pNpcList[i]->m_iStatus = m_pNpcList[i]->m_iStatus | 0x20;
            }

            if (cChangeSide != -1) m_pNpcList[i]->m_cSide = cChangeSide;

            m_pNpcList[i]->m_cBravery = (rand() % 3) + m_pNpcList[i]->m_iMinBravery;
            m_pNpcList[i]->m_iSpotMobIndex = iSpotMobIndex;
            m_pNpcList[i]->m_iGuildGUID = iGuildGUID;

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

            case 64:
                m_pMapList[iMapIndex]->bAddCropsTotalSum();
                break;
            }

            SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, NULL, NULL, NULL);
            return TRUE;
        }

    return FALSE;
}

void CGame::NpcProcess()
{
    int i, iMaxHP;
    DWORD dwTime, dwActionTime;

    dwTime = timeGetTime();

    for (i = 1; i < DEF_MAXNPCS; i++)
    {

        if (m_pNpcList[i] != NULL)
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

        if ((m_pNpcList[i] != NULL) && ((dwTime - m_pNpcList[i]->m_dwTime) > dwActionTime))
        {
            m_pNpcList[i]->m_dwTime = dwTime;

            if (abs(m_pNpcList[i]->m_cMagicLevel) > 0)
            {
                if ((dwTime - m_pNpcList[i]->m_dwMPupTime) > DEF_MPUPTIME)
                {
                    m_pNpcList[i]->m_dwMPupTime = dwTime;

                    m_pNpcList[i]->m_iMana += iDice(1, (m_pNpcList[i]->m_iMaxMana / 5));

                    if (m_pNpcList[i]->m_iMana > m_pNpcList[i]->m_iMaxMana)
                        m_pNpcList[i]->m_iMana = m_pNpcList[i]->m_iMaxMana;
                }
            }

            if (((dwTime - m_pNpcList[i]->m_dwHPupTime) > DEF_HPUPTIME) && (m_pNpcList[i]->m_bIsKilled == FALSE))
            {
                m_pNpcList[i]->m_dwHPupTime = dwTime;

                iMaxHP = iDice(m_pNpcList[i]->m_iHitDice, 8) + m_pNpcList[i]->m_iHitDice;
                if (m_pNpcList[i]->m_iHP < iMaxHP)
                {

                    if (m_pNpcList[i]->m_bIsSummoned == FALSE)
                        m_pNpcList[i]->m_iHP += iDice(1, m_pNpcList[i]->m_iHitDice);

                    if (m_pNpcList[i]->m_iHP > iMaxHP) m_pNpcList[i]->m_iHP = iMaxHP;
                    if (m_pNpcList[i]->m_iHP <= 0)     m_pNpcList[i]->m_iHP = 1;
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

            if ((m_pNpcList[i] != NULL) && (m_pNpcList[i]->m_iHP != 0) && (m_pNpcList[i]->m_bIsSummoned == TRUE))
            {
                switch (m_pNpcList[i]->m_sType)
                {
                case 29:
                    if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 90)
                        NpcKilledHandler(NULL, NULL, i, 0);
                    break;

                default:
                    if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > DEF_SUMMONTIME)
                        NpcKilledHandler(NULL, NULL, i, 0);
                    break;
                }
            }
        }
    }
}

void CGame::ChatMsgHandlerGSM(int iMsgType, int iV1, char * pName, char * pData, DWORD dwMsgSize)
{
    int i, iRet;
    DWORD * dwp;
    WORD * wp;
    short * sp;
    char * cp, cTemp[256], cSendMode = NULL;

    ZeroMemory(cTemp, sizeof(cTemp));

    dwp = (DWORD *)cTemp;
    *dwp = MSGID_COMMAND_CHATMSG;

    wp = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
    *wp = NULL;

    cp = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
    sp = (short *)cp;
    *sp = NULL;
    cp += 2;

    sp = (short *)cp;
    *sp = NULL;
    cp += 2;

    memcpy(cp, pName, 10);
    cp += 10;

    *cp = (char)iMsgType;
    cp++;

    memcpy(cp, pData, dwMsgSize);
    cp += dwMsgSize;

    switch (iMsgType)
    {
    case 1:
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if (m_pClientList[i] != NULL)
            {
                if (m_pClientList[i]->m_bIsInitComplete == FALSE) break;
                if ((m_pClientList[i]->m_iGuildGUID == iV1) && (m_pClientList[i]->m_iGuildGUID != 0))
                {
                    iRet = m_pClientList[i]->iSendMsg(cTemp, dwMsgSize + 22);
                }
            }
        break;

    case 2:
    case 10:
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if (m_pClientList[i] != NULL)
            {
                iRet = m_pClientList[i]->iSendMsg(cTemp, dwMsgSize + 22);
            }
        break;
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
            if (m_pMapList[cMapIndex]->bGetMoveable(dX + aX, dY + aY) == TRUE) return cTmpDir;
        }

    if (cTurn == 1)
        for (i = cDir; i >= cDir - 7; i--)
        {
            cTmpDir = i;
            if (cTmpDir < 1) cTmpDir += 8;
            aX = _tmp_cTmpDirX[cTmpDir];
            aY = _tmp_cTmpDirY[cTmpDir];
            if (m_pMapList[cMapIndex]->bGetMoveable(dX + aX, dY + aY) == TRUE) return cTmpDir;
        }

    return 0;
}

BOOL CGame::bGetEmptyPosition(short * pX, short * pY, char cMapIndex)
{
    int i;
    short sX, sY;

    if (cMapIndex == -1) return FALSE;

    for (i = 0; i < 25; i++)
        if ((m_pMapList[cMapIndex]->bGetMoveable(*pX + _tmp_cEmptyPosX[i], *pY + _tmp_cEmptyPosY[i]) == TRUE) &&
            (m_pMapList[cMapIndex]->bGetIsTeleport(*pX + _tmp_cEmptyPosX[i], *pY + _tmp_cEmptyPosY[i]) == FALSE))
        {
            sX = *pX + _tmp_cEmptyPosX[i];
            sY = *pY + _tmp_cEmptyPosY[i];
            *pX = sX;
            *pY = sY;
            return TRUE;
        }


    GetMapInitialPoint(cMapIndex, &sX, &sY);
    *pX = sX;
    *pY = sY;

    return FALSE;
}

void CGame::TargetSearch(int iNpcH, short * pTarget, char * pTargetType)
{
    int ix{}, iy{}, iPKCount{};
    short sX{}, sY{}, rX{}, rY{}, dX{}, dY{};
    short sOwner{}, sTargetOwner{}, sDistance{}, sTempDistance{};
    char  cOwnerType{}, cTargetType{}, cTargetSide{};
    int   iInv{};

    sTargetOwner = NULL;
    cTargetType = NULL;
    sDistance = 100;

    sX = m_pNpcList[iNpcH]->m_sX;
    sY = m_pNpcList[iNpcH]->m_sY;

    rX = m_pNpcList[iNpcH]->m_sX - m_pNpcList[iNpcH]->m_cTargetSearchRange;
    rY = m_pNpcList[iNpcH]->m_sY - m_pNpcList[iNpcH]->m_cTargetSearchRange;

    for (ix = rX; ix < rX + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; ix++)
        for (iy = rY; iy < rY + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; iy++)
        {

            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
            if (sOwner != NULL)
            {
                if ((sOwner == iNpcH) && (cOwnerType == DEF_OWNERTYPE_NPC)) break;

                iPKCount = 0;
                switch (cOwnerType)
                {
                case DEF_OWNERTYPE_PLAYER:
                    if (m_pClientList[sOwner] == NULL)
                    {
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(5, sOwner, DEF_OWNERTYPE_PLAYER, ix, iy);
                    }
                    else
                    {
                        if (m_pClientList[sOwner]->m_iAdminUserLevel > 0) goto SKIP_SEARCH;

                        dX = m_pClientList[sOwner]->m_sX;
                        dY = m_pClientList[sOwner]->m_sY;
                        cTargetSide = m_pClientList[sOwner]->m_cSide;
                        iPKCount = m_pClientList[sOwner]->m_iPKCount;
                        iInv = m_pClientList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];
                    }
                    break;

                case DEF_OWNERTYPE_NPC:
                    if (m_pNpcList[sOwner] == NULL)
                    {
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(6, sOwner, DEF_OWNERTYPE_NPC, ix, iy);
                    }
                    else
                    {
                        dX = m_pNpcList[sOwner]->m_sX;
                        dY = m_pNpcList[sOwner]->m_sY;
                        cTargetSide = m_pNpcList[sOwner]->m_cSide;
                        iPKCount = 0;
                        iInv = m_pNpcList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];

                        if (m_pNpcList[iNpcH]->m_sType == 21)
                        {
                            if (_iCalcPlayerNum(m_pNpcList[sOwner]->m_cMapIndex, dX, dY, 2) != 0)
                            {
                                sOwner = NULL;
                                cOwnerType = NULL;
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
    DWORD dwTime = timeGetTime();

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != NULL)
        {
            if ((m_pNpcList[i]->m_iGuildGUID != NULL) && (cTargetType == DEF_OWNERTYPE_PLAYER) &&
                (m_pClientList[sTargetH]->m_iGuildGUID == m_pNpcList[i]->m_iGuildGUID))
            {

                if (m_pNpcList[i]->m_cActionLimit == 0)
                {
                    m_pNpcList[i]->m_bIsSummoned = TRUE;
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
                        m_pNpcList[i]->m_iTargetIndex = NULL;
                        m_pNpcList[i]->m_cTargetType = NULL;
                    }
                    break;

                default:
                    m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                    m_pNpcList[i]->m_iTargetIndex = NULL;
                    m_pNpcList[i]->m_cTargetType = NULL;
                    break;
                }
            }
        }
}

int CGame::iGetDangerValue(int iNpcH, short dX, short dY)
{
    int ix, iy, iDangerValue;
    short sOwner, sDOType;
    char  cOwnerType;
    DWORD dwRegisterTime;

    if (m_pNpcList[iNpcH] == NULL) return FALSE;

    iDangerValue = 0;

    for (ix = dX - 2; ix <= dX + 2; ix++)
        for (iy = dY - 2; iy <= dY + 2; iy++)
        {
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sDOType, &dwRegisterTime);

            if (sDOType == 1) iDangerValue++;

            switch (cOwnerType)
            {
            case NULL:
                break;
            case DEF_OWNERTYPE_PLAYER:
                if (m_pClientList[sOwner] == NULL) break;
                if (m_pNpcList[iNpcH]->m_cSide != m_pClientList[sOwner]->m_cSide)
                    iDangerValue++;
                else iDangerValue--;
                break;
            case DEF_OWNERTYPE_NPC:
                if (m_pNpcList[sOwner] == NULL) break;
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
    char * pData{};
    char cFrom{}, cKey{};
    DWORD    dwMsgSize{}, * dwpMsgID;
    WORD * wpMsgType{}, * wp{};
    int      i;
    char   m_msgBuff[1000];

    if ((m_bF5pressed == TRUE) && (m_bF1pressed == TRUE))
    {
        log->info("(XXX) RELOADING CONFIGS MANUALY...");
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
            {
                if (!save_player_data(m_pClientList[i]->shared_from_this()))
                {
                    log->error("Save player data error for [{}]", m_pClientList[i]->m_cCharName);
                }
                //bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA, i);
            }
        bInit();
    }

    if ((m_bF1pressed == TRUE) && (m_bF4pressed == TRUE) && (m_bOnExitProcess == FALSE))
    {
        m_cShutDownCode = 2;
        m_bOnExitProcess = TRUE;
        m_dwExitProcessTime = timeGetTime();
        log->info("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Local command)!!!");
        //bSendMsgToLS(MSGID_GAMESERVERSHUTDOWNED, NULL);
        if (m_iMiddlelandMapIndex > 0)
        {
            SaveOccupyFlagData();
            //bSendMsgToLS(MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA, NULL, NULL);
            //bSendMsgToLS(MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA, NULL, NULL);
        }

        return;
    }

    m_iCurMsgs = 0;
    std::unique_lock<std::mutex> l(packet_mtx, std::defer_lock);
    while (!packet_queue.empty())
    {
        l.lock();
        std::unique_ptr<socket_message> msg = std::move(packet_queue.front());
        socket_message & sm = *msg;

        CClient * client = sm.connection_state->client.get();
        int iClientH = sm.connection_state->client_handle;

        stream_read & sr = *sm.sr;

        packet_queue.pop_front();
        l.unlock();

        pData = sr.data;
        dwMsgSize = sr.size;

        dwpMsgID = (DWORD *)(sr.data + DEF_INDEX4_MSGID);

        if (client == nullptr || client->currentstatus == client_status::login_screen)
        {
            handle_login_server_message(sm);
            continue;
        }

        if (m_pClientList[iClientH] == nullptr)
        {
            log->error(std::format("ClientList is null - Game Packet [{:X}]", *dwpMsgID));
            sm.websocket.close();
            continue;
        }

        if (client->currentstatus != client_status::in_game)
        {
            log->error(std::format("Client's status is not in game - Game Packet [{:X}]", *dwpMsgID));
            continue;
        }

        //log->info(std::format("Game Packet [{:X}]", *dwpMsgID));
        m_pClientList[iClientH]->m_dwTime = timeGetTime();
        switch (*dwpMsgID)
        {

            case DEF_REQUEST_RESURRECTPLAYER_YES:
                RequestResurrectPlayer(iClientH, TRUE);
                break;

            case DEF_REQUEST_RESURRECTPLAYER_NO:
                RequestResurrectPlayer(iClientH, FALSE);
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
                ClientCommonHandler(iClientH, pData);
                break;

            case MSGID_SCREEN_SETTINGS:
                ScreenSettingsHandler(iClientH, pData, dwMsgSize);
                break;

            case MSGID_COMMAND_MOTION:
                ClientMotionHandler(iClientH, pData);
                break;

            case MSGID_COMMAND_CHECKCONNECTION:
                CheckConnectionHandler(iClientH, pData);
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

            case MSGID_REQUEST_HELDENIANTELEPORT:
                RequestHeldenianTeleport(iClientH, pData, dwMsgSize);
                break;


            case MSGID_REQUEST_CITYHALLTELEPORT:
                if (memcmp(m_pClientList[iClientH]->m_cLocation, "aresden", 7) == 0)
                {
                    RequestTeleportHandler(iClientH, "2   ", "dglv2", 263, 258);
                }
                else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvine", 6) == 0)
                {
                    RequestTeleportHandler(iClientH, "2   ", "dglv2", 209, 258);
                }
                break;

            default:
                wsprintf(m_msgBuff, "Unknown message received! (0x%.8X) Delete Client", *dwpMsgID);
                log->info(m_msgBuff);
                //DeleteClient(iClientH, TRUE, TRUE);
                break;
        }
    }
}

void CGame::PutMsgQueue(std::unique_ptr<socket_message> & sm)
{
    packet_queue.push_back(std::move(sm));
}

BOOL CGame::_bAddClientItemList(int iClientH, CItem * pItem, int * pDelReq)
{
    int i;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (pItem == NULL) return FALSE;

    if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW))
    {
        if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount)) > (DWORD)_iCalcMaxLoad(iClientH))
            return FALSE;
    }
    else
    {
        if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, 1)) > (DWORD)_iCalcMaxLoad(iClientH))
            return FALSE;
    }

    if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW))
    {
        for (i = 0; i < DEF_MAXITEMS; i++)
            if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) &&
                (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, pItem->m_cName, 20) == 0))
            {
                m_pClientList[iClientH]->m_pItemList[i]->m_dwCount += pItem->m_dwCount;
                *pDelReq = 1;

                update_db_bag_item(
                    m_pClientList[iClientH]->m_pItemList[i],
                    m_pClientList[iClientH]->m_ItemPosList[i].x,
                    m_pClientList[iClientH]->m_ItemPosList[i].y,
                    m_pClientList[iClientH]->id,
                    m_pClientList[iClientH]->m_bIsItemEquipped[i]
                );

                iCalcTotalWeight(iClientH);

                return TRUE;
            }
    }

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] == NULL)
        {

            m_pClientList[iClientH]->m_pItemList[i] = pItem;
            m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
            m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

            *pDelReq = 0;

            if (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)
                m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

            update_db_bag_item(
                m_pClientList[iClientH]->m_pItemList[i],
                m_pClientList[iClientH]->m_ItemPosList[i].x,
                m_pClientList[iClientH]->m_ItemPosList[i].y,
                m_pClientList[iClientH]->id,
                m_pClientList[iClientH]->m_bIsItemEquipped[i]
            );

            iCalcTotalWeight(iClientH);

            return TRUE;
        }

    return FALSE;
}

void CGame::SendEventToNearClient_TypeB(DWORD dwMsgID, WORD wMsgType, char cMapIndex, short sX, short sY, short sV1, short sV2, short sV3, short sV4)
{
    int i, iRet, iShortCutIndex;
    char * cp, cData[100];
    DWORD * dwp, dwTime;
    WORD * wp;
    short * sp;
    BOOL bFlag;

    ZeroMemory(cData, sizeof(cData));

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = dwMsgID;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
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

    bFlag = TRUE;
    iShortCutIndex = 0;
    while (bFlag == TRUE)
    {
        i = m_iClientShortCut[iShortCutIndex];
        iShortCutIndex++;
        if (i == 0) bFlag = FALSE;

        if ((bFlag == TRUE) && (m_pClientList[i] != NULL))
        {
            if ((m_pClientList[i]->m_cMapIndex == cMapIndex) &&
                (m_pClientList[i]->m_sX >= sX - 10) &&
                (m_pClientList[i]->m_sX <= sX + 10) &&
                (m_pClientList[i]->m_sY >= sY - 8) &&
                (m_pClientList[i]->m_sY <= sY + 8))
            {

                iRet = m_pClientList[i]->iSendMsg(cData, 18);
            }
        }
    }
}

void CGame::SendNotifyMsg(int iFromH, int iToH, WORD wMsgType, DWORD sV1, DWORD sV2, DWORD sV3, char * pString, DWORD sV4, DWORD sV5, DWORD sV6, DWORD sV7, DWORD sV8, DWORD sV9, char * pString2)
{
    char cData[1000];
    DWORD * dwp;
    WORD * wp;
    char * cp;
    short * sp;
    int * ip, i;
    std::size_t iRet;

    if (m_pClientList[iToH] == NULL) return;

    ZeroMemory(cData, sizeof(cData));

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_NOTIFY;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    switch (wMsgType)
    {
    case DEF_NOTIFY_HELDENIANCOUNT:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV3;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV4;
        cp += 2;

        cp += 14;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 14);

        break;

    case DEF_NOTIFY_NOMOREAGRICULTURE:
    case DEF_NOTIFY_AGRICULTURESKILLLIMIT:
    case DEF_NOTIFY_AGRICULTURENOAREA:
        iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
        break;

    case DEF_NOTIFY_SPAWNEVENT:
        *cp = (char)sV3;
        cp++;

        sp = (short *)cp;
        *cp = sV1;
        cp += 2;

        sp = (short *)cp;
        *cp = sV2;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 11);
        break;

    case DEF_NOTIFY_QUESTCOUNTER:
        ip = (int *)cp;
        *ip = sV1;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 24);
        break;

    case DEF_NOTIFY_APOCGATECLOSE:
    case DEF_NOTIFY_APOCGATEOPEN:
        ip = (int *)cp;
        *ip = sV1;
        cp += 4;

        ip = (int *)cp;
        *ip = sV2;
        cp += 4;

        memcpy(cp, pString, 10);
        cp += 10;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 24);
        break;

    case DEF_NOTIFY_ABADDONKILLED:
        memcpy(cp, m_pClientList[iFromH]->m_cCharName, 10);
        cp += 10;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 24);
        break;

    case DEF_NOTIFY_APOCFORCERECALLPLAYERS:
    case DEF_NOTIFY_APOCGATESTARTMSG:
    case DEF_NOTIFY_APOCGATEENDMSG:
    case DEF_NOTIFY_NORECALL:
        iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
        break;

    case DEF_NOTIFY_FORCERECALLTIME:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_MONSTERCOUNT:
    case DEF_NOTIFY_SLATE_STATUS:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_0BE8:
    case DEF_NOTIFY_HELDENIANTELEPORT:
    case DEF_NOTIFY_HELDENIANEND:
    case DEF_NOTIFY_RESURRECTPLAYER:
    case DEF_NOTIFY_SLATE_EXP:
    case DEF_NOTIFY_SLATE_MANA:
    case DEF_NOTIFY_SLATE_INVINCIBLE:
        iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
        break;

    case DEF_NOTIFY_SLATE_CREATEFAIL:
        iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
        break;

    case DEF_NOTIFY_SLATE_CREATESUCCESS:
        dwp = (DWORD *)cp;
        *dwp = sV1;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
        break;

    case DEF_NOTIFY_PARTY:
        switch (sV1)
        {
        case 4:
        case 6:
            wp = (WORD *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (WORD *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            wp = (WORD *)cp;
            *wp = (WORD)sV3;
            cp += 2;

            memcpy(cp, pString, 10);
            cp += 10;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 22);
            break;
        case 5:
            wp = (WORD *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (WORD *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            wp = (WORD *)cp;
            *wp = (WORD)sV3;
            cp += 2;

            memcpy(cp, pString, sV3 * 11);
            cp += sV3 * 11;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 12 + (sV3 * 11));
            break;
        default:
            wp = (WORD *)cp;
            *wp = (WORD)sV1;
            cp += 2;

            wp = (WORD *)cp;
            *wp = (WORD)sV2;
            cp += 2;

            wp = (WORD *)cp;
            *wp = (WORD)sV3;
            cp += 2;

            wp = (WORD *)cp;
            *wp = (WORD)sV4;
            cp += 2;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
            break;
        }
        break;

    case DEF_NOTIFY_REQGUILDNAMEANSWER:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        memcpy(cp, pString, 20);
        cp += 20;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 30);
        break;

    case DEF_NOTIFY_ITEMUPGRADEFAIL:
        sp = (short *)cp;
        *sp = (short)sV1;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_ITEMATTRIBUTECHANGE:
    case DEF_NOTIFY_GIZONITEMUPGRADELEFT:
        sp = (short *)cp;
        *sp = sV1;
        cp += 2;

        dwp = (DWORD *)cp;
        *dwp = sV2;
        cp += 4;

        dwp = (DWORD *)cp;
        *dwp = sV3;
        cp += 4;

        dwp = (DWORD *)cp;
        *dwp = sV4;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 20);
        break;

    case DEF_NOTIFY_GIZONITEMCANGE:
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

        dwp = (DWORD *)cp;
        *dwp = sV8;
        cp += 4;

        memcpy(cp, pString, 20);
        cp += 20;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 41);
        break;

    case DEF_NOTIFY_CHANGEPLAYMODE:
        memcpy(cp, pString, 10);
        cp += 10;
        iRet = m_pClientList[iToH]->iSendMsg(cData, 16);
        break;

    case DEF_NOTIFY_TCLOC:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        memcpy(cp, pString, 10);
        cp += 10;

        wp = (WORD *)cp;
        *wp = (WORD)sV4;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV5;
        cp += 2;

        memcpy(cp, pString2, 10);
        cp += 10;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 34);
        break;

    case DEF_NOTIFY_GRANDMAGICRESULT:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV3;
        cp += 2;

        memcpy(cp, pString, 10);
        cp += 10;

        wp = (WORD *)cp;
        *wp = (WORD)sV4;
        cp += 2;

        if (sV9 > 0)
        {
            memcpy(cp, pString2, (sV9 * 2) + 2);
            cp += (sV9 * 2) + 2;
        }
        else
        {
            sp = (short *)cp;
            *sp = 0;
            cp += 2;
        }
        iRet = m_pClientList[iToH]->iSendMsg(cData, (sV9 * 2) + 26);
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

        if (pString2 != NULL) memcpy(cp, pString2, 20);
        cp += 20;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 44);
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

        dwp = (DWORD *)cp;
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

        dwp = (DWORD *)cp;
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

            dwp = (DWORD *)cp;
            *dwp = m_pNpcList[sV2]->m_dwDeadTime;
            cp += 4;

            dwp = (DWORD *)cp;
            *dwp = m_pNpcList[sV2]->m_dwRegenTime;
            cp += 4;

            ip = (int *)cp;
            *ip = (int)m_pNpcList[sV2]->m_bIsKilled;
            cp += 4;

            iRet = m_pClientList[iToH]->iSendMsg(cData, 26 + 12);
            break;
        }
        break;

    case DEF_NOTIFY_HELDENIANSTART:
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

        if (pString != NULL) memcpy(cp, pString, 20);
        cp += 20;

        if (pString2 != NULL) memcpy(cp, pString2, 20);
        cp += 20;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 64);
        break;

    case DEF_NOTIFY_PORTIONSUCCESS:
    case DEF_NOTIFY_LOWPORTIONSKILL:
    case DEF_NOTIFY_PORTIONFAIL:
    case DEF_NOTIFY_NOMATCHINGPORTION:
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
        dwp = (DWORD *)cp;
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

    case DEF_NOTIFY_FISHCANCELED:
    case DEF_NOTIFY_FISHSUCCESS:
    case DEF_NOTIFY_FISHFAIL:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_DEBUGMSG:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_FISHCHANCE:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_ENERGYSPHEREGOALIN:
    case DEF_NOTIFY_EVENTFISHMODE:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV3;
        cp += 2;

        memcpy(cp, pString, 20);
        cp += 20;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 32);
        break;

    case DEF_NOTIFY_NOTICEMSG:
        memcpy(cp, pString, strlen(pString));
        cp += strlen(pString);

        *cp = NULL;
        cp++;

        iRet = m_pClientList[iToH]->iSendMsg(cData, strlen(pString) + 7);
        break;

    case DEF_NOTIFY_CANNOTRATING:
        wp = (WORD *)cp;
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
        wp = (WORD *)cp;
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
        *cp = NULL;
        cp++;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 7 + strlen(pString));
        break;

    case DEF_NOTIFY_WHISPERMODEON:
    case DEF_NOTIFY_WHISPERMODEOFF:
    case DEF_NOTIFY_PLAYERNOTONGAME:
        memcpy(cp, pString, 10);
        cp += 10;

        memcpy(cp, "          ", 10);
        cp += 10;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 27);
        break;

    case DEF_NOTIFY_PLAYERONGAME:
        memcpy(cp, pString, 10);
        cp += 10;

        if (pString[0] != 0)
        {
            memcpy(cp, pString2, 14);
            cp += 14;
        }

        iRet = m_pClientList[iToH]->iSendMsg(cData, 31);
        break;

    case DEF_NOTIFY_ITEMSOLD:
    case DEF_NOTIFY_ITEMREPAIRED:
        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV1;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV2;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
        break;

    case DEF_NOTIFY_REPAIRITEMPRICE:
    case DEF_NOTIFY_SELLITEMPRICE:
        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV1;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV2;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV3;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV4;
        cp += 4;

        memcpy(cp, pString, 20);
        cp += 20;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 42);
        break;

    case DEF_NOTIFY_CANNOTREPAIRITEM:
    case DEF_NOTIFY_CANNOTSELLITEM:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        memcpy(cp, pString, 20);
        cp += 20;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 30);

        break;

    case DEF_NOTIFY_SHOWMAP:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
        break;

    case DEF_NOTIFY_SKILLUSINGEND:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_TOTALUSERS:
        wp = (WORD *)cp;
        *wp = (WORD)m_iTotalGameServerClients;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 8);
        break;

    case DEF_NOTIFY_MAGICEFFECTOFF:
    case DEF_NOTIFY_MAGICEFFECTON:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV2;
        cp += 4;

        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV3;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 16);
        break;

    case DEF_NOTIFY_CANNOTITEMTOBANK:
        iRet = m_pClientList[iToH]->iSendMsg(cData, 6);
        break;

    case DEF_NOTIFY_SERVERCHANGE:
        memcpy(cp, m_pClientList[iToH]->m_cMapName, 10);
        cp += 10;

        memcpy(cp, m_cLogServerAddr, 15);
        cp += 15;

        ip = (int *)cp;
        *ip = m_iLogServerPort;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 16 + 19);
        break;

    case DEF_NOTIFY_SKILL:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
        break;

    case DEF_NOTIFY_SETITEMCOUNT:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        dwp = (DWORD *)cp;
        *dwp = (DWORD)sV2;
        cp += 4;

        *cp = (char)sV3;
        cp++;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 13);
        break;

    case DEF_NOTIFY_ITEMDEPLETED_ERASEITEM:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
        break;

    case DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        ip = (int *)cp;
        *ip = (int)sV2;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 12);
        break;

    case DEF_NOTIFY_DROPITEMFIN_ERASEITEM:
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;

        ip = (int *)cp;
        *ip = (int)sV2;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 12);
        break;

    case DEF_NOTIFY_CANNOTGIVEITEM:
    case DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED:
        wp = (WORD *)cp;
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
        wp = (WORD *)cp;
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
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iExp;
        cp += 4;
        dwp = (DWORD *)cp;
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
        wp = (WORD *)cp;
        *wp = (WORD)sV1;
        cp += 2;
        wp = (WORD *)cp;
        *wp = (WORD)sV2;
        cp += 2;
        memcpy(cp, pString, 10);
        cp += 10;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iRewardGold;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = m_pClientList[iToH]->m_iExp;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 28);
        break;

    case DEF_NOTIFY_PKPENALTY:
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iExp;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iStr;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iVit;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iDex;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iInt;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iMag;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iCharisma;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iPKCount;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 38);
        break;

    case DEF_NOTIFY_TRAVELERLIMITEDLEVEL:
    case DEF_NOTIFY_LIMITEDLEVEL:
        dwp = (DWORD *)cp;
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
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iExp;
        cp += 4;

        ip = (int *)cp;
        *ip = m_pClientList[iToH]->m_iRating;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
        break;

    case DEF_NOTIFY_HP:
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iHP;
        cp += 4;
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iMP;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 14);
        break;

    case DEF_NOTIFY_MP:
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iMP;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
        break;

    case DEF_NOTIFY_SP:
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iSP;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
        break;

    case DEF_NOTIFY_CHARISMA:
        dwp = (DWORD *)cp;
        *dwp = (DWORD)m_pClientList[iToH]->m_iCharisma;
        cp += 4;

        iRet = m_pClientList[iToH]->iSendMsg(cData, 10);
        break;

    case DEF_NOTIFY_STATECHANGE_FAILED:
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
        if (m_pClientList[iFromH] != NULL)
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
        if (m_pClientList[iFromH] != NULL)
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
    }

    switch (iRet)
    {
    case DEF_XSOCKEVENT_QUENEFULL:
    case DEF_XSOCKEVENT_SOCKETERROR:
    case DEF_XSOCKEVENT_CRITICALERROR:
    case DEF_XSOCKEVENT_SOCKETCLOSED:
        //DeleteClient(iToH, TRUE, TRUE);
        return;
    }
}

DWORD CGame::dwGetItemCount(int iClientH, char * pName)
{
    char cTmpName[30];

    if (m_pClientList[iClientH] == NULL) return NULL;

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pName);

    for (int i = 0; i < DEF_MAXITEMS; i++)
        if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0))
            return m_pClientList[iClientH]->m_pItemList[i]->m_dwCount;

    return 0;
}

int CGame::SetItemCount(int iClientH, char * pItemName, DWORD dwCount)
{
    char cTmpName[30];
    WORD wWeight;

    if (m_pClientList[iClientH] == NULL) return -1;

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pItemName);

    for (int i = 0; i < DEF_MAXITEMS; i++)
        if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0))
        {
            wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[i], 1);

            if (dwCount == 0)
            {
                ItemDepleteHandler(iClientH, i, FALSE);
            }
            else
            {
                m_pClientList[iClientH]->m_pItemList[i]->m_dwCount = dwCount;
                update_db_bag_item(
                    m_pClientList[iClientH]->m_pItemList[i],
                    m_pClientList[iClientH]->m_ItemPosList[i].x,
                    m_pClientList[iClientH]->m_ItemPosList[i].y,
                    m_pClientList[iClientH]->id,
                    m_pClientList[iClientH]->m_bIsItemEquipped[i]
                );
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETITEMCOUNT, i, dwCount, (char)TRUE, NULL);
            }

            return wWeight;
        }

    return -1;
}

int CGame::SetItemCount(int iClientH, int iItemIndex, DWORD dwCount)
{
    WORD wWeight;

    if (m_pClientList[iClientH] == NULL) return -1;
    if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return -1;

    wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1);

    if (dwCount == 0)
    {
        ItemDepleteHandler(iClientH, iItemIndex, FALSE);
    }
    else
    {
        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount = dwCount;
        update_db_bag_item(
            m_pClientList[iClientH]->m_pItemList[iItemIndex],
            m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x,
            m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y,
            m_pClientList[iClientH]->id,
            m_pClientList[iClientH]->m_bIsItemEquipped[iItemIndex]
        );
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETITEMCOUNT, iItemIndex, dwCount, (char)TRUE, NULL);
    }

    return wWeight;
}

void CGame::EKAnnounce(int iClientH, char * pMsg)
{
    char * cp, cTemp[256];
    DWORD * dwp, dwMsgSize;
    WORD * wp;
    short * sp;

    ZeroMemory(cTemp, sizeof(cTemp));

    dwp = (DWORD *)cTemp;
    *dwp = MSGID_COMMAND_CHATMSG;


    wp = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
    *wp = NULL;

    cp = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
    sp = (short *)cp;
    *sp = NULL;
    cp += 2;

    sp = (short *)cp;
    *sp = NULL;
    cp += 2;

    memcpy(cp, "Announcer", 9);
    cp += 10;

    *cp = 10;
    cp++;

    dwMsgSize = strlen(pMsg);
    memcpy(cp, pMsg, dwMsgSize);
    cp += dwMsgSize;

    m_pClientList[iClientH]->iSendMsg(cTemp, dwMsgSize + 22);
}

BOOL CGame::_bInitNpcAttr(CNpc * pNpc, char * pNpcName, short sClass, char cSA)
{
    int i, iTemp;
    char cTmpName[30];
    int sTemp;
    double dV1, dV2, dV3;

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pNpcName);

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        if (m_pNpcConfigList[i] != NULL)
        {
            if (memcmp(cTmpName, m_pNpcConfigList[i]->m_cNpcName, 20) == 0)
            {
                ZeroMemory(pNpc->m_cNpcName, sizeof(pNpc->m_cNpcName));
                memcpy(pNpc->m_cNpcName, m_pNpcConfigList[i]->m_cNpcName, 20);

                pNpc->m_sType = m_pNpcConfigList[i]->m_sType;

                if (m_pNpcConfigList[i]->m_iHitDice <= 5)
                    pNpc->m_iHP = abs(iDice(m_pNpcConfigList[i]->m_iHitDice, 4) + m_pNpcConfigList[i]->m_iHitDice);
                else pNpc->m_iHP = abs((m_pNpcConfigList[i]->m_iHitDice * 4) + m_pNpcConfigList[i]->m_iHitDice + iDice(1, m_pNpcConfigList[i]->m_iHitDice));
                if (pNpc->m_iHP == 0) pNpc->m_iHP = 1;

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
                pNpc->m_iMaxMana = m_pNpcConfigList[i]->m_iMaxMana;
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
                    pNpc->m_iAttackStrategy = DEF_ATTACKAI_NORMAL;
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
                        pNpc->m_cSpecialAbility = NULL;
                        cSA = NULL;
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
                        pNpc->m_cSpecialAbility = NULL;
                        cSA = NULL;
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

                pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFF0FF;
                sTemp = cSA;
                sTemp = sTemp << 8;
                pNpc->m_iStatus = pNpc->m_iStatus | sTemp;

                pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFFFF0;
                pNpc->m_iStatus = pNpc->m_iStatus | (sClass);

                return TRUE;
            }
        }

    return FALSE;
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

void CGame::CalculateGuildEffect(int iVictimH, char cVictimType, short sAttackerH)
{
    int ix, iy, iExp;
    short sOwnerH;
    char cOwnerType;

    // ### ERROR POINT!
    return;

    if (m_pClientList[sAttackerH] == NULL) return;

    switch (cVictimType)
    {

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[iVictimH] == NULL) return;

        for (ix = m_pNpcList[iVictimH]->m_sX - 2; ix <= m_pNpcList[iVictimH]->m_sX + 2; ix++)
            for (iy = m_pNpcList[iVictimH]->m_sY - 2; iy <= m_pNpcList[iVictimH]->m_sY + 2; iy++)
            {

                m_pMapList[m_pNpcList[iVictimH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);

                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (sOwnerH != NULL) && (sOwnerH != sAttackerH) &&
                    (memcmp(m_pClientList[sAttackerH]->m_cGuildName, m_pClientList[sOwnerH]->m_cGuildName, 20) == 0) &&
                    (m_pClientList[sAttackerH]->m_iLevel >= m_pClientList[sOwnerH]->m_iLevel))
                {
                    if (iDice(1, 3) == 2)
                    {
                        iExp = (m_pNpcList[iVictimH]->m_iExp / 3);
                        if (m_pClientList[sAttackerH]->m_iLevel >= m_iPlayerMaxLevel) iExp = 0;

                        if (iExp > 0)
                        {

                            m_pClientList[sOwnerH]->m_iExp += iExp;
                            if (bCheckLimitedUser(sOwnerH) == FALSE)
                            {
                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_EXP, NULL, NULL, NULL, NULL);
                            }
                            bCheckLevelUp(sOwnerH);
                        }
                    }
                }
            }
        break;
    }
}

void CGame::TimeManaPointsUp(int iClientH)
{
    int iMaxMP, iTotal;
    double dV1, dV2, dV3;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == TRUE) return;

    iMaxMP = (2 * m_pClientList[iClientH]->m_iMag) + (2 * m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iInt / 2);
    if (m_pClientList[iClientH]->m_iMP < iMaxMP)
    {
        iTotal = iDice(1, (m_pClientList[iClientH]->m_iMag));
        if (m_pClientList[iClientH]->m_iAddMP != 0)
        {
            dV2 = (double)iTotal;
            dV3 = (double)m_pClientList[iClientH]->m_iAddMP;
            dV1 = (dV3 / 100.0f) * dV2;
            iTotal += (int)dV1;
        }

        m_pClientList[iClientH]->m_iMP += iTotal;

        if (m_pClientList[iClientH]->m_iMP > iMaxMP)
            m_pClientList[iClientH]->m_iMP = iMaxMP;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MP, NULL, NULL, NULL, NULL);
    }
}

void CGame::TimeStaminarPointsUp(int iClientH)
{
    int iMaxSP, iTotal = 0;
    double dV1, dV2, dV3;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == TRUE) return;

    iMaxSP = (2 * m_pClientList[iClientH]->m_iStr) + (2 * m_pClientList[iClientH]->m_iLevel);
    if (m_pClientList[iClientH]->m_iSP < iMaxSP)
    {

        iTotal = iDice(1, (m_pClientList[iClientH]->m_iVit / 3));
        if (m_pClientList[iClientH]->m_iAddSP != 0)
        {
            dV2 = (double)iTotal;
            dV3 = (double)m_pClientList[iClientH]->m_iAddSP;
            dV1 = (dV3 / 100.0f) * dV2;
            iTotal += (int)dV1;
        }

        if (m_pClientList[iClientH]->m_iLevel <= 20)
        {
            iTotal += 15;
        }
        else if (m_pClientList[iClientH]->m_iLevel <= 40)
        {
            iTotal += 10;
        }
        else if (m_pClientList[iClientH]->m_iLevel <= 60)
        {
            iTotal += 5;
        }

        m_pClientList[iClientH]->m_iSP += iTotal;
        if (m_pClientList[iClientH]->m_iSP > iMaxSP)
            m_pClientList[iClientH]->m_iSP = iMaxSP;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
    }
}

void CGame::DelayEventProcess()
{

}

void CGame::SendGuildMsg(int iClientH, WORD wNotifyMsgType, short sV1, short sV2, char * pString)
{
    char cData[500];
    DWORD * dwp;
    WORD * wp;
    char * cp;
    int i, iRet;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) &&
            (memcmp(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName, 20) == 0))
        {
            ZeroMemory(cData, sizeof(cData));

            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = wNotifyMsgType;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

            switch (wNotifyMsgType)
            {
            case DEF_NOTIFY_GUILDDISBANDED:
                if (i == iClientH) break;
                memcpy(cp, m_pClientList[iClientH]->m_cGuildName, 20);
                cp += 20;

                iRet = m_pClientList[i]->iSendMsg(cData, 26);
                ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
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
                DeleteClient(i, TRUE, TRUE);
                return;
            }
        }

}

void CGame::ReleaseFollowMode(short sOwnerH, char cOwnerType)
{
    int i;

    for (i = 0; i < DEF_MAXNPCS; i++)
        if ((i != sOwnerH) && (m_pNpcList[i] != NULL))
        {
            if ((m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW) &&
                (m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH) &&
                (m_pNpcList[i]->m_cFollowOwnerType == cOwnerType))
            {

                m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOMWAYPOINT;
            }
        }
}

int CGame::_iGetMagicNumber(char * pMagicName, int * pReqInt, int * pCost)
{
    int i;
    char cTmpName[31];

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pMagicName);

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        if (m_pMagicConfigList[i] != NULL)
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

void CGame::TrainSkillResponse(BOOL bSuccess, int iClientH, int iSkillNum, int iSkillLevel)
{
    char * cp, cData[100]{};
    DWORD * dwp;
    WORD * wp;
    int   iRet;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((iSkillNum < 0) || (iSkillNum > 100)) return;
    if ((iSkillLevel < 0) || (iSkillLevel > 100)) return;

    if (bSuccess == TRUE)
    {
        if (m_pClientList[iClientH]->m_cSkillMastery[iSkillNum] != 0) return;

        m_pClientList[iClientH]->m_cSkillMastery[iSkillNum] = iSkillLevel;
        bCheckTotalSkillMasteryPoints(iClientH, iSkillNum);

        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_SKILLTRAINSUCCESS;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        *cp = iSkillNum;
        cp++;

        *cp = iSkillLevel;
        cp++;

        wsprintf(G_cTxt, "Skill Improve: (%s) increased (%s) at %s (%d,%d).", m_pClientList[iClientH]->m_cCharName, m_pSkillConfigList[iSkillNum]->m_cName, m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
        log->info(G_cTxt);

        if (m_pSkillConfigList[iSkillNum]->m_cName != NULL)
            _bItemLog(DEF_ITEMLOG_SKILLLEARN, iClientH, m_pSkillConfigList[iSkillNum]->m_cName, NULL);

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 8);
        switch (iRet)
        {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            DeleteClient(iClientH, TRUE, TRUE);
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
    char cTmpName[30];

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pSkillName);

    for (i = 1; i < DEF_MAXSKILLTYPE; i++)
        if (m_pSkillConfigList[i] != NULL)
        {
            if (memcmp(cTmpName, m_pSkillConfigList[i]->m_cName, 20) == 0)
            {
                return i;
            }
        }

    return 0;
}

BOOL CGame::bPlayerItemToBank(int iClientH, short sItemIndex)
{
    int i, iIndex;

    if (m_pClientList[iClientH] == NULL) return FALSE;

    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] == NULL)
        {
            iIndex = i;
            goto NEXT_STEP_PLTB;
        }
    return FALSE;

    NEXT_STEP_PLTB:;

    ReleaseItemHandler(iClientH, sItemIndex, TRUE);

    m_pClientList[iClientH]->m_pItemInBankList[iIndex] = m_pClientList[iClientH]->m_pItemList[sItemIndex];
    m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
    m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;

    for (i = 1; i < DEF_MAXITEMS; i++)
        if ((m_pClientList[iClientH]->m_pItemList[i - 1] == NULL) && (m_pClientList[iClientH]->m_pItemList[i] != NULL))
        {
            m_pClientList[iClientH]->m_pItemList[i - 1] = m_pClientList[iClientH]->m_pItemList[i];
            m_pClientList[iClientH]->m_bIsItemEquipped[i - 1] = m_pClientList[iClientH]->m_bIsItemEquipped[i];
            m_pClientList[iClientH]->m_pItemList[i] = NULL;
            m_pClientList[iClientH]->m_bIsItemEquipped[i] = FALSE;
        }

    return TRUE;
}

BOOL CGame::bBankItemToPlayer(int iClientH, short sItemIndex)
{
    int i, iIndex;

    if (m_pClientList[iClientH] == NULL) return FALSE;

    if (m_pClientList[iClientH]->m_pItemInBankList[sItemIndex] == NULL) return FALSE;

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] == NULL)
        {
            iIndex = i;
            goto NEXT_STEP_PLTB;
        }
    return FALSE;

    NEXT_STEP_PLTB:;

    m_pClientList[iClientH]->m_pItemList[iIndex] = m_pClientList[iClientH]->m_pItemInBankList[sItemIndex];

    m_pClientList[iClientH]->m_pItemInBankList[sItemIndex] = NULL;

    return TRUE;
}

void CGame::Quit()
{
    int i;

    Sleep(300);

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != NULL) delete m_pClientList[i];

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != NULL) delete m_pNpcList[i];

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL) delete m_pMapList[i];

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != NULL) delete m_pItemConfigList[i];

    for (i = 0; i < DEF_MAXNPCTYPES; i++)
        if (m_pNpcConfigList[i] != NULL) delete m_pNpcConfigList[i];

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        if (m_pMagicConfigList[i] != NULL) delete m_pMagicConfigList[i];

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        if (m_pSkillConfigList[i] != NULL) delete m_pSkillConfigList[i];

    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        if (m_pQuestConfigList[i] != NULL) delete m_pQuestConfigList[i];

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != NULL) delete m_pDynamicObjectList[i];

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] != NULL) delete m_pDelayEventList[i];

//     for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
//         if (m_pNoticeMsgList[i] != NULL) delete m_pNoticeMsgList[i];

    for (i = 0; i < DEF_MAXFISHS; i++)
        if (m_pFish[i] != NULL) delete m_pFish[i];

    for (i = 0; i < DEF_MAXMINERALS; i++)
        if (m_pMineral[i] != NULL) delete m_pMineral[i];

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        if (m_pPortionConfigList[i] != NULL) delete m_pPortionConfigList[i];

    //	for (i = 0; i < DEF_MAXTELEPORTTYPE; i++)
    //	if (m_pTeleportConfigList[i] != NULL) delete m_pTeleportConfigList[i];

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        if (m_pBuildItemList[i] != NULL) delete m_pBuildItemList[i];

    for (i = 0; i < DEF_MAXDUPITEMID; i++)
        if (m_pDupItemIDList[i] != NULL) delete m_pDupItemIDList[i];

    if (m_pNoticementData != NULL) delete m_pNoticementData;

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
        iRet = (iLevel * 2);
    }

    return iRet;
}

BOOL CGame::bCheckLevelUp(int iClientH)
{
    BOOL bStr, bVit, bDex, bInt, bMag, bChr;
    char cLoopCnt;
    int bobdole;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_iLevel >= 180)
    {
        if (m_pClientList[iClientH]->m_iExp >= m_pClientList[iClientH]->m_iNextLevelExp)
        {
            AddGizon(iClientH);
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 1, NULL, NULL);
        }
        return FALSE;
    }

    if (m_pClientList[iClientH]->m_iExp < m_pClientList[iClientH]->m_iNextLevelExp) return FALSE;

    bStr = bVit = bDex = bInt = bMag = bChr = FALSE;
    cLoopCnt = 0;
    while (1)
    {
        if (cLoopCnt++ > 100) return FALSE;
        if (m_pClientList[iClientH]->m_iExp >= m_pClientList[iClientH]->m_iNextLevelExp)
        {
            m_pClientList[iClientH]->m_iLevel++;
            m_pClientList[iClientH]->m_iLU_Pool += 3;

            if (m_pClientList[iClientH]->m_iStr > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iStr = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iDex > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iDex = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iVit > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iVit = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iInt > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iInt = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iMag > DEF_CHARPOINTLIMIT)      m_pClientList[iClientH]->m_iMag = DEF_CHARPOINTLIMIT;
            if (m_pClientList[iClientH]->m_iCharisma > DEF_CHARPOINTLIMIT) m_pClientList[iClientH]->m_iCharisma = DEF_CHARPOINTLIMIT;

            if (m_pClientList[iClientH]->m_iLevel > 100)
                if (m_pClientList[iClientH]->m_bIsPlayerCivil == TRUE)
                    ForceChangePlayMode(iClientH, TRUE);

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LEVELUP, NULL, NULL, NULL, NULL);

            m_pClientList[iClientH]->m_iNextLevelExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel + 1];

            CalcTotalItemEffect(iClientH, -1, FALSE);

            wsprintf(G_cTxt, "(!) Level up: Player (%s) Level (%d) Experience(%d) Next Level Experience(%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iLevel, m_pClientList[iClientH]->m_iExp, m_pClientList[iClientH]->m_iNextLevelExp);
            log->info(G_cTxt);
        }
        else { return TRUE; }
    }
    bobdole = 3;
    return FALSE;
}

BOOL CGame::bCheckMagicInt(int iClientH)
{

    for (int i = 0; i < DEF_MAXMAGICTYPE; i++)
    {
        if (m_pMagicConfigList[i] != NULL)
            if (m_pMagicConfigList[i]->m_sIntLimit > m_pClientList[iClientH]->m_iInt)
            {
                m_pClientList[iClientH]->m_cMagicMastery[i] = 0;
            }
    }

    return TRUE;
}

BOOL CGame::bChangeState(char cStateChange, char * cStr, char * cVit, char * cDex, char * cInt, char * cMag, char * cChar)
{
    if (cStateChange == DEF_STR)
    {
        *cStr += 1;
    }
    else if (cStateChange == DEF_VIT)
    {
        *cInt += 1;
		// if(m_pClientList[iClientH]->m_iStr - cVit < 10 || m_pClientList[iClientH]->cVit - cVit < DEF_CHARPOINTLIMIT )
		// 	return 0;
    }
    else if (cStateChange == DEF_DEX)
    {
        *cVit += 1;
    }
    else if (cStateChange == DEF_INT)
    {
        *cDex += 1;
    }
    else if (cStateChange == DEF_MAG)
    {
        *cMag += 1;
    }
    else if (cStateChange == DEF_CHR)
    {
        *cChar += 1;
    }
    else
    {
        return 0;
    }

    return cStateChange;
}

BOOL CGame::bCheckLimitedUser(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return FALSE;

    if ((memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) &&
        (m_pClientList[iClientH]->m_iExp >= m_iLevelExp20))
    {
        m_pClientList[iClientH]->m_iExp = m_iLevelExp20 - 1;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_TRAVELERLIMITEDLEVEL, NULL, NULL, NULL, NULL);
        return TRUE;
    }


    return FALSE;
}

BOOL CGame::bSetItemToBankItem(int iClientH, short sItemIndex)
{
    int i, iRet;
    DWORD * dwp;
    WORD * wp;
    char * cp;
    short * sp;
    char cData[100]{};
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return FALSE;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_bIsInsideWarehouse == FALSE) return FALSE;

    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] == NULL)
        {
            m_pClientList[iClientH]->m_pItemInBankList[i] = m_pClientList[iClientH]->m_pItemList[sItemIndex];
            pItem = m_pClientList[iClientH]->m_pItemInBankList[i];
            m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;

            iCalcTotalWeight(iClientH);

            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_ITEMTOBANK;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

            *cp = i;
            cp++;

            *cp = 1;
            cp++;

            memcpy(cp, pItem->m_cName, 20);
            cp += 20;

            dwp = (DWORD *)cp;
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

            wp = (WORD *)cp;
            *wp = pItem->m_wCurLifeSpan;
            cp += 2;

            wp = (WORD *)cp;
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

            sp = (short *)cp;
            *sp = pItem->m_sItemEffectValue2;
            cp += 2;

            dwp = (DWORD *)cp;
            *dwp = pItem->m_dwAttribute;
            cp += 4;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 55);
            switch (iRet)
            {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:
                // DeleteClient(iClientH, TRUE, TRUE);
                return TRUE;
            }

            return TRUE;
        }

    return FALSE;
}

void CGame::ApplyPKpenalty(short sAttackerH, short sVictumH)
{
    int iV1, iV2;

    if (m_pClientList[sAttackerH] == NULL) return;
    if (m_pClientList[sVictumH] == NULL) return;
    if ((m_pClientList[sAttackerH]->m_bIsSafeAttackMode == TRUE) && (m_pClientList[sAttackerH]->m_iPKCount == 0)) return;
    if ((strcmp(m_pClientList[sVictumH]->m_cLocation, "aresden") != 0) && (strcmp(m_pClientList[sVictumH]->m_cLocation, "elvine") != 0) && (strcmp(m_pClientList[sVictumH]->m_cLocation, "elvhunter") != 0) && (strcmp(m_pClientList[sVictumH]->m_cLocation, "arehunter") != 0))
    {
        return;
    }

    m_pClientList[sAttackerH]->m_iPKCount++;

    _bPKLog(DEF_PKLOG_BYPK, sAttackerH, sVictumH, NULL);

    iV1 = iDice((m_pClientList[sVictumH]->m_iLevel / 2) + 1, 50);
    iV2 = iDice((m_pClientList[sAttackerH]->m_iLevel / 2) + 1, 50);

    m_pClientList[sAttackerH]->m_iExp -= iV1;
    m_pClientList[sAttackerH]->m_iExp -= iV2;
    if (m_pClientList[sAttackerH]->m_iExp < 0) m_pClientList[sAttackerH]->m_iExp = 0;

    SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_PKPENALTY, NULL, NULL, NULL, NULL);

    SendEventToNearClient_TypeA(sAttackerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);

    wsprintf(G_cTxt, "(!) PK-penalty: Attacker(%s) Exp Lost(%d) Remaining Exp(%d) ", m_pClientList[sAttackerH]->m_cCharName, iV1+iV2, m_pClientList[sAttackerH]->m_iExp);
    log->info(G_cTxt);

    m_stCityStatus[m_pClientList[sAttackerH]->m_cSide].iCrimes++;

    m_pClientList[sAttackerH]->m_iRating -= 10;
    if (m_pClientList[sAttackerH]->m_iRating > 10000)  m_pClientList[sAttackerH]->m_iRating = 10000;
    if (m_pClientList[sAttackerH]->m_iRating < -10000) m_pClientList[sAttackerH]->m_iRating = -10000;


    if (strcmp(m_pClientList[sAttackerH]->m_cLocation, "aresden") == 0)
    {
        if ((strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "arebrk11") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "arebrk12") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "arebrk21") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "arebrk22") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "aresden") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "huntzone2") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "areuni") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "arefarm") == 0))
        {
            ZeroMemory(m_pClientList[sAttackerH]->m_cLockedMapName, sizeof(m_pClientList[sAttackerH]->m_cLockedMapName));
            strcpy(m_pClientList[sAttackerH]->m_cLockedMapName, "arejail");
            m_pClientList[sAttackerH]->m_iLockedMapTime = 60 * 3;
            RequestTeleportHandler(sAttackerH, "2   ", "arejail", -1, -1);
            return;
        }
    }

    if (strcmp(m_pClientList[sAttackerH]->m_cLocation, "elvine") == 0)
    {
        if ((strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvbrk11") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvbrk12") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvbrk21") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvbrk22") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvine") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "huntzone1") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvuni") == 0) ||
            (strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvfarm") == 0))
        {
            ZeroMemory(m_pClientList[sAttackerH]->m_cLockedMapName, sizeof(m_pClientList[sAttackerH]->m_cLockedMapName));
            strcpy(m_pClientList[sAttackerH]->m_cLockedMapName, "elvjail");
            m_pClientList[sAttackerH]->m_iLockedMapTime = 60 * 3;
            RequestTeleportHandler(sAttackerH, "2   ", "elvjail", -1, -1);
            return;
        }
    }
}

void CGame::ApplyCombatKilledPenalty(int iClientH, int cPenaltyLevel, BOOL bIsSAattacked)
{
    int iExp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if (m_bIsCrusadeMode == TRUE)
    {
        if (m_pClientList[iClientH]->m_iPKCount > 0)
        {
            m_pClientList[iClientH]->m_iPKCount--;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PKPENALTY, NULL, NULL, NULL, NULL);
            _bPKLog(DEF_PKLOG_REDUCECRIMINAL, NULL, iClientH, NULL);

        }
        return;
    }
    else
    {
        if (m_pClientList[iClientH]->m_iPKCount > 0)
        {
            m_pClientList[iClientH]->m_iPKCount--;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PKPENALTY, NULL, NULL, NULL, NULL);
            _bPKLog(DEF_PKLOG_REDUCECRIMINAL, NULL, iClientH, NULL);
        }

        iExp = iDice(1, (5 * cPenaltyLevel * m_pClientList[iClientH]->m_iLevel));

        if (m_pClientList[iClientH]->m_bIsNeutral == TRUE) iExp = iExp / 3;

        // if (m_pClientList[iClientH]->m_iLevel == DEF_PLAYERMAXLEVEL) iExp = 0;

        m_pClientList[iClientH]->m_iExp -= iExp;
        if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, NULL, NULL, NULL, NULL);

        if (m_pClientList[iClientH]->m_bIsNeutral != TRUE)
        {
            if (m_pClientList[iClientH]->m_iLevel < 80)
            {
                cPenaltyLevel--;
                if (cPenaltyLevel <= 0) cPenaltyLevel = 1;
                _PenaltyItemDrop(iClientH, cPenaltyLevel, bIsSAattacked);
            }
            else _PenaltyItemDrop(iClientH, cPenaltyLevel, bIsSAattacked);
        }
    }
}

void CGame::_PenaltyItemDrop(int iClientH, int iTotal, BOOL bIsSAattacked)
{
    int i, j, iRemainItem;
    char cItemIndexList[DEF_MAXITEMS], cItemIndex;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if ((m_pClientList[iClientH]->m_iAlterItemDropIndex != -1) && (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex] != NULL))
    {
        if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP)
        {
            if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan > 0)
                m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan--;

            DropItemHandler(iClientH, m_pClientList[iClientH]->m_iAlterItemDropIndex, -1, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_cName);

            m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
        }
        else
        {
            log->info("Alter Drop Item Index Error1");
            for (i = 0; i < DEF_MAXITEMS; i++)
                if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) && (m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP))
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
        ZeroMemory(cItemIndexList, sizeof(cItemIndexList));

        for (j = 0; j < DEF_MAXITEMS; j++)
            if (m_pClientList[iClientH]->m_pItemList[j] != NULL)
            {
                cItemIndexList[iRemainItem] = j;
                iRemainItem++;
            }

        if (iRemainItem == 0) return;
        cItemIndex = cItemIndexList[iDice(1, iRemainItem) - 1];


        if ((m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectType != 0) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectValue1 == m_pClientList[iClientH]->m_sCharIDnum1) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectValue2 == m_pClientList[iClientH]->m_sCharIDnum2) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sTouchEffectValue3 == m_pClientList[iClientH]->m_sCharIDnum3))
        {
        }

        else if (
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sIDnum >= 400) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sIDnum != 402) &&
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sIDnum <= 428))
        {
        }

        else if (((m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) ||
            (m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)) &&
            (bIsSAattacked == FALSE))
        {
        }

        else if ((m_pClientList[iClientH]->m_bIsLuckyEffect == TRUE) && (iDice(1, 10) == 5))
        {
        }

        else DropItemHandler(iClientH, cItemIndex, -1, m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_cName);
    }
}

int CGame::_iCalcMaxLoad(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return 0;

    return (m_pClientList[iClientH]->m_iStr * 500 + m_pClientList[iClientH]->m_iLevel * 500);
}

int CGame::iAddDynamicObjectList(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, DWORD dwLastTime, int iV1)
{
    int i;
    short sPreType;
    DWORD dwTime, dwRegisterTime;

    m_pMapList[cMapIndex]->bGetDynamicObject(sX, sY, &sPreType, &dwRegisterTime);
    if (sPreType != NULL) return NULL;

    switch (sType)
    {
    case DEF_DYNAMICOBJECT_FIRE3:
    case DEF_DYNAMICOBJECT_FIRE:
        if (m_pMapList[cMapIndex]->bGetIsMoveAllowedTile(sX, sY) == FALSE)
            return NULL;
        if (dwLastTime != NULL)
        {
            switch (m_pMapList[cMapIndex]->m_cWhetherStatus)
            {
            case 1:	dwLastTime = dwLastTime - (dwLastTime / 2);       break;
            case 2:	dwLastTime = (dwLastTime / 2) - (dwLastTime / 3); break;
            case 3:	dwLastTime = (dwLastTime / 3) - (dwLastTime / 4); break;
            }

            if (dwLastTime == NULL) dwLastTime = 1000;
        }
        break;

    case DEF_DYNAMICOBJECT_FISHOBJECT:
    case DEF_DYNAMICOBJECT_FISH:
        if (m_pMapList[cMapIndex]->bGetIsWater(sX, sY) == FALSE)
            return NULL;
        break;

    case DEF_DYNAMICOBJECT_ARESDENFLAG1:
    case DEF_DYNAMICOBJECT_ELVINEFLAG1:
    case DEF_DYNAMICOBJECT_MINERAL1:
    case DEF_DYNAMICOBJECT_MINERAL2:
        if (m_pMapList[cMapIndex]->bGetMoveable(sX, sY) == FALSE)
            return NULL;
        m_pMapList[cMapIndex]->SetTempMoveAllowedFlag(sX, sY, FALSE);
        break;

    }

    for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] == NULL)
        {
            dwTime = timeGetTime();

            if (dwLastTime != NULL)
                dwLastTime += (iDice(1, 4) * 1000);

            m_pDynamicObjectList[i] = new CDynamicObject(sOwner, cOwnerType, sType, cMapIndex, sX, sY, dwTime, dwLastTime, iV1);
            m_pMapList[cMapIndex]->SetDynamicObject(i, sType, sX, sY, dwTime);
            SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_CONFIRM, cMapIndex, sX, sY, sType, i, NULL);

            return i;
        }
    return NULL;
}

void CGame::CheckDynamicObjectList()
{
    int i;
    DWORD dwTime = timeGetTime(), dwRegisterTime;
    short sType;

    for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)
    {
        if ((m_pDynamicObjectList[i] != NULL) && (m_pDynamicObjectList[i]->m_dwLastTime != NULL))
        {

            switch (m_pDynamicObjectList[i]->m_sType)
            {
            case DEF_DYNAMICOBJECT_FIRE3:
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
        if ((m_pDynamicObjectList[i] != NULL) && (m_pDynamicObjectList[i]->m_dwLastTime != NULL) &&
            ((dwTime - m_pDynamicObjectList[i]->m_dwRegisterTime) >= m_pDynamicObjectList[i]->m_dwLastTime))
        {

            m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, &sType, &dwRegisterTime);

            if (dwRegisterTime == m_pDynamicObjectList[i]->m_dwRegisterTime)
            {
                SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[i]->m_cMapIndex, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, m_pDynamicObjectList[i]->m_sType, i, NULL);
                m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->SetDynamicObject(NULL, NULL, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, dwTime);
            }

            switch (sType)
            {
            case DEF_DYNAMICOBJECT_FISHOBJECT:
            case DEF_DYNAMICOBJECT_FISH:
                bDeleteFish(m_pDynamicObjectList[i]->m_sOwner, 2);
                break;
            }

            delete m_pDynamicObjectList[i];
            m_pDynamicObjectList[i] = NULL;
        }
    }
}

void CGame::CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue)
{
    short sSkillIndex;
    int   iOldSSN, iSSNpoint, iWeaponIndex;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_pItemList[sWeaponIndex] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;

    sSkillIndex = m_pClientList[iClientH]->m_pItemList[sWeaponIndex]->m_sRelatedSkill;
    if ((sSkillIndex < 0) || (sSkillIndex >= DEF_MAXSKILLTYPE)) return;
    if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] == 0) return;

    iOldSSN = m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex];
    m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] += iValue;

    iSSNpoint = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] + 1];

    if ((m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] < 100) &&
        (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] > iSSNpoint))
    {

        m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]++;

        switch (sSkillIndex)
        {
        case 0:  // Mining
        case 5:  // Hand-Attack
        case 13: // Manufacturing
            if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iStr * 2))
            {
                m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
            }
            else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
            break;

        case 3: // Magic-Resistance
            if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iLevel * 2))
            {
                m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
            }
            else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
            break;

        case 4:  // Magic
        case 21: // Staff-Attack
            if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iMag * 2))
            {
                m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
            }
            else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
            break;

        case 1:  // Fishing
        case 6:  // Archery
        case 7:  // Short-Sword
        case 8:  // Long-Sword
        case 9:  // Fencing 
        case 10: // Axe-Attack
        case 11: // Shield        	
        case 14: // Hammer 
            if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iDex * 2))
            {
                m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
            }
            else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
            break;

        case 2:	 // Farming
        case 12: // Alchemy
        case 15: 
        case 19: // Pretend-Corpse
            if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iInt * 2))
            {
                m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
            }
            else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
            break;

        case 23: // Poison-Resistance
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
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], NULL, NULL);
        }
    }
}

void CGame::CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue)
{
    int   iOldSSN, iSSNpoint, iWeaponIndex;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((sSkillIndex < 0) || (sSkillIndex >= DEF_MAXSKILLTYPE)) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;

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
            if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iDex * 2))
            {
                m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
                m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
            }
            else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
            break;

        case 2:
        case 12:
        case 14:
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

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], NULL, NULL);
        }
    }
}

void CGame::MobGenerator()
{
    int i = {}, x = {}, j = {}, iNamingValue = {}, iResult = {}, iTotalMob = {};
    char cNpcName[30] = {}, cName_Master[11] = {}, cName_Slave[11] = {}, cWaypoint[11] = {};
    char cSA = {};
    int  pX = {}, pY = {}, iMapLevel = {}, iProbSA = {}, iKindSA = {}, iResultNum = {}, iNpcID = {};
    BOOL bMaster = {}, bFirmBerserk = {}, bIsSpecialEvent = {};

    if (m_bOnExitProcess == TRUE) return;

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        // Random Mob Generator

        //if ( (m_pMapList[i] != NULL) && (m_pMapList[i]->m_bRandomMobGenerator == TRUE) && 
        //	 ((m_pMapList[i]->m_iMaximumObject - 30) > m_pMapList[i]->m_iTotalActiveObject) ) {

        if (m_pMapList[i] != NULL)
        {
            //if (m_bIsCrusadeMode == TRUE) 
            //	 iResultNum = (m_pMapList[i]->m_iMaximumObject - 30) / 3;
            //else iResultNum = (m_pMapList[i]->m_iMaximumObject - 30);
            iResultNum = (m_pMapList[i]->m_iMaximumObject - 30);
        }

        if ((m_pMapList[i] != NULL) && (m_pMapList[i]->m_bRandomMobGenerator == TRUE) && (iResultNum > m_pMapList[i]->m_iTotalActiveObject))
        {
            if ((m_iMiddlelandMapIndex != -1) && (m_iMiddlelandMapIndex == i) && (m_bIsCrusadeMode == TRUE)) break;

            iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
            if (iNamingValue != -1)
            {
                ZeroMemory(cName_Master, sizeof(cName_Master));
                wsprintf(cName_Master, "XX%d", iNamingValue);
                cName_Master[0] = '_';
                cName_Master[1] = i + 65;

                ZeroMemory(cNpcName, sizeof(cNpcName));

                bFirmBerserk = FALSE;
                iResult = iDice(1, 100);
                switch (m_pMapList[i]->m_cRandomMobGeneratorLevel)
                {

                case 1: // arefarm, elvfarm, aresden, elvine
                    if ((iResult >= 1) && (iResult < 20))
                    {
                        iResult = 1; // Slime
                    }
                    else if ((iResult >= 20) && (iResult < 40))
                    {
                        iResult = 2; // Giant-Ant
                    }
                    else if ((iResult >= 40) && (iResult < 85))
                    {
                        iResult = 24; // Rabbit
                    }
                    else if ((iResult >= 85) && (iResult < 95))
                    {
                        iResult = 25; // Cat
                    }
                    else if ((iResult >= 95) && (iResult <= 100))
                    {
                        iResult = 3; // Orc
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
                        case 1: iResult = 3;  break;
                        case 2: iResult = 4;  break;
                        }
                    }
                    else if ((iResult >= 20) && (iResult < 25))
                    {
                        iResult = 30;
                    }
                    else if ((iResult >= 25) && (iResult < 50))
                    {
                        switch (iDice(1, 3))
                        {
                        case 1: iResult = 5;  break;
                        case 2: iResult = 6;  break;
                        case 3:	iResult = 7;  break;
                        }
                    }
                    else if ((iResult >= 50) && (iResult < 75))
                    {
                        switch (iDice(1, 7))
                        {
                        case 1:
                        case 2:	iResult = 8;  break;
                        case 3:	iResult = 11; break;
                        case 4: iResult = 12; break;
                        case 5:	iResult = 18; break;
                        case 6:	iResult = 26; break;
                        case 7: iResult = 28; break;
                        }
                    }
                    else if ((iResult >= 75) && (iResult <= 100))
                    {
                        switch (iDice(1, 5))
                        {
                        case 1:
                        case 2: iResult = 9;  break;
                        case 3:	iResult = 13; break;
                        case 4: iResult = 14; break;
                        case 5:	iResult = 27; break;
                        }
                    }
                    iMapLevel = 3;
                    break;

                case 4:
                    if ((iResult >= 1) && (iResult < 50))
                    {
                        switch (iDice(1, 2))
                        {
                        case 1:	iResult = 2;  break;
                        case 2: iResult = 10; break;
                        }
                    }
                    else if ((iResult >= 50) && (iResult < 80))
                    {
                        switch (iDice(1, 2))
                        {
                        case 1: iResult = 8;  break;
                        case 2: iResult = 11; break;
                        }
                    }
                    else if ((iResult >= 80) && (iResult <= 100))
                    {
                        switch (iDice(1, 2))
                        {
                        case 1: iResult = 14; break;
                        case 2:	iResult = 9;  break;
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
                        case 5: iResult = 2;  break;
                        }
                    }
                    else if ((iResult >= 30) && (iResult < 60))
                    {
                        switch (iDice(1, 2))
                        {
                        case 1: iResult = 3;  break;
                        case 2: iResult = 4;  break;
                        }
                    }
                    else if ((iResult >= 60) && (iResult < 80))
                    {
                        switch (iDice(1, 2))
                        {
                        case 1: iResult = 5;  break;
                        case 2: iResult = 7;  break;
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
                        case 3: iResult = 9;  break;
                        }
                    }
                    iMapLevel = 3;
                    break;

                case 6: // huntzone3, huntzone4
                    if ((iResult >= 1) && (iResult < 60))
                    {
                        switch (iDice(1, 4))
                        {
                        case 1: iResult = 5;  break; // Skeleton
                        case 2:	iResult = 6;  break; // Orc-Mage
                        case 3: iResult = 12; break; // Cyclops
                        case 4: iResult = 11; break; // Troll
                        }
                    }
                    else if ((iResult >= 60) && (iResult < 90))
                    {
                        switch (iDice(1, 5))
                        {
                        case 1:
                        case 2: iResult = 8;  break; // Stone-Golem
                        case 3:	iResult = 11; break; // Troll
                        case 4:	iResult = 12; break; // Cyclops 
                        case 5:	iResult = 43; break; // Tentocle
                        }
                    }
                    else if ((iResult >= 90) && (iResult <= 100))
                    {
                        switch (iDice(1, 9))
                        {
                        case 1:	iResult = 26; break;
                        case 2:	iResult = 9;  break;
                        case 3: iResult = 13; break;
                        case 4: iResult = 14; break;
                        case 5:	iResult = 18; break;
                        case 6:	iResult = 28; break;
                        case 7: iResult = 27; break;
                        case 8: iResult = 29; break;
                        }
                    }
                    iMapLevel = 4;
                    break;

                case 7: // areuni, elvuni
                    if ((iResult >= 1) && (iResult < 50))
                    {
                        switch (iDice(1, 5))
                        {
                        case 1: iResult = 3;  break; // Orc
                        case 2: iResult = 6;  break; // Orc-Mage
                        case 3: iResult = 10; break; // Amphis
                        case 4: iResult = 3;  break; // Orc
                        case 5: iResult = 50; break; // Giant-Tree
                        }
                    }
                    //else if ((iResult >= 50) && (iResult < 60)) { 
                    //	iResult = 29; // Rudolph
                    //}
                    else if ((iResult >= 50) && (iResult < 85))
                    {
                        switch (iDice(1, 4))
                        {
                        case 1: iResult = 50; break; // Giant-Tree
                        case 2:
                        case 3: iResult = 6;  break; // Orc-Mage
                        case 4: iResult = 12; break; // Troll
                        }
                    }
                    else if ((iResult >= 85) && (iResult <= 100))
                    {
                        switch (iDice(1, 4))
                        {
                        case 1: iResult = 12;  break; // Troll
                        case 2:
                        case 3:
                            if (iDice(1, 100) < 3)
                                iResult = 17; // Unicorn
                            else iResult = 12; // Troll
                            break;
                        case 4: iResult = 29;  break; // Cannibal-Plant
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
                    else if ((iResult >= 70) && (iResult < 95))
                    {
                        switch (iDice(1, 2))
                        {
                        case 1: iResult = 8;  break;
                        case 2: iResult = 11; break;
                        }
                    }
                    else if ((iResult >= 95) && (iResult <= 100))
                    {
                        iResult = 14; break;
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
                    else if ((iResult >= 95) && (iResult <= 100))
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

                    if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = TRUE;
                    iMapLevel = 5;
                    break;

                case 10:
                    if ((iResult >= 1) && (iResult < 70))
                    {
                        switch (iDice(1, 3))
                        {
                        case 1:	iResult = 9; break;
                        case 2: iResult = 5; break;
                        case 3: iResult = 8; break;
                        }
                    }
                    else if ((iResult >= 70) && (iResult < 95))
                    {
                        switch (iDice(1, 3))
                        {
                        case 1:
                        case 2:	iResult = 13; break;
                        case 3: iResult = 14; break;
                        }
                    }
                    else if ((iResult >= 95) && (iResult <= 100))
                    {
                        switch (iDice(1, 3))
                        {
                        case 1:
                        case 2: iResult = 14; break;
                        case 3: iResult = 15; break;
                        }
                    }
                    // Demon Berserk
                    if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = TRUE;
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

                case 12:
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
                    else if ((iResult >= 85) && (iResult <= 100))
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
                        bFirmBerserk = TRUE;
                        iTotalMob = 4 - (iDice(1, 2) - 1);
                        break;
                    }
                    else if ((iResult >= 15) && (iResult < 40))
                    {
                        iResult = 14;
                        bFirmBerserk = TRUE;
                        iTotalMob = 4 - (iDice(1, 2) - 1);
                        break;
                    }
                    else if ((iResult >= 40) && (iResult < 60))
                    {
                        iResult = 9;
                        bFirmBerserk = TRUE;
                        iTotalMob = 4 - (iDice(1, 2) - 1);
                        break;
                    }
                    else if ((iResult >= 60) && (iResult < 75))
                    {
                        iResult = 13;
                        bFirmBerserk = TRUE;
                        iTotalMob = 4 - (iDice(1, 2) - 1);
                        break;
                    }
                    else if ((iResult >= 75) && (iResult < 95))
                    {
                        iResult = 23;
                    }
                    else if ((iResult >= 95) && (iResult <= 100))
                    {
                        iResult = 22;
                    }
                    iMapLevel = 5;
                    break;

                case 14: // icebound
                    if ((iResult >= 1) && (iResult < 30))
                    {
                        iResult = 23; // Dark-Elf
                    }
                    else if ((iResult >= 30) && (iResult < 50))
                    {
                        iResult = 31; // Ice-Golem
                    }
                    else if ((iResult >= 50) && (iResult < 70))
                    {
                        iResult = 22; // Beholder
                        bFirmBerserk = TRUE;
                        iTotalMob = 4 - (iDice(1, 2) - 1);
                    }
                    else if ((iResult >= 70) && (iResult < 90))
                    {
                        iResult = 32; // DireBoar
                    }
                    else if ((iResult >= 90) && (iResult <= 100))
                    {
                        iResult = 33; // Frost
                    }
                    iMapLevel = 5;
                    break;

                case 15:
                    if ((iResult >= 1) && (iResult < 35))
                    {
                        iResult = 23;
                        bFirmBerserk = TRUE;
                    }
                    else if ((iResult >= 35) && (iResult < 50))
                    {
                        iResult = 22;
                        bFirmBerserk = TRUE;
                    }
                    else if ((iResult >= 50) && (iResult < 80))
                    {
                        iResult = 15;
                    }
                    else if ((iResult >= 80) && (iResult <= 100))
                    {
                        iResult = 21;
                    }
                    iMapLevel = 4;
                    break;

                case 16: // 2ndmiddle, huntzone1, huntzone2, 
                    if ((iResult >= 1) && (iResult < 40))
                    {
                        switch (iDice(1, 3))
                        {
                        case 1:	iResult = 7;  break; // Scorpion
                        case 2: iResult = 2;  break; // Giant-Ant
                        case 3: iResult = 10; break; // Amphis
                        }
                    }
                    else if ((iResult >= 40) && (iResult < 50))
                    {
                        iResult = 30; // Rudolph
                    }
                    else if ((iResult >= 50) && (iResult < 85))
                    {
                        switch (iDice(1, 2))
                        {
                        case 1: iResult = 5;  break; // Skeleton
                        case 2: iResult = 4;  break; // Zombie
                        }
                    }
                    else if ((iResult >= 85) && (iResult <= 100))
                    {
                        switch (iDice(1, 3))
                        {
                        case 1: iResult = 8;  break; // Stone-Golem
                        case 2: iResult = 11; break; // Clay-Golem
                        case 3: iResult = 7;  break; // Scorpion
                        }
                    }
                    iMapLevel = 1;
                    break;

                case 17:
                    if ((iResult >= 1) && (iResult < 30))
                    {
                        switch (iDice(1, 4))
                        {
                        case 1:	iResult = 22;  break; // Giant-Frog
                        case 2: iResult = 8;   break; // Stone-Golem
                        case 3: iResult = 24;  break; // Rabbit
                        case 4: iResult = 5;   break;
                        }
                    }
                    else if ((iResult >= 30) && (iResult < 40))
                    {
                        iResult = 30;
                    }
                    else if ((iResult >= 40) && (iResult < 70))
                    {
                        iResult = 32;
                    }
                    else if ((iResult >= 70) && (iResult < 90))
                    {
                        iResult = 31;
                        if (iDice(1, 5) == 1)
                        {
                            bFirmBerserk = TRUE;
                        }
                    }
                    else if ((iResult >= 90) && (iResult <= 100))
                    {
                        iResult = 33;
                    }
                    iMapLevel = 1;
                    break;

                case 18: // druncncity
                    if ((iResult >= 1) && (iResult < 2))
                    {
                        iResult = 39; // Tentocle
                    }
                    else if ((iResult >= 2) && (iResult < 12))
                    {
                        iResult = 44; // ClawTurtle
                    }
                    else if ((iResult >= 12) && (iResult < 50))
                    {
                        iResult = 48; // Nizie
                    }
                    else if ((iResult >= 50) && (iResult < 80))
                    {
                        iResult = 45; // Giant-Crayfish
                    }
                    else if ((iResult >= 80) && (iResult < 90))
                    {
                        iResult = 34; // Stalker
                    }
                    else if ((iResult >= 90) && (iResult <= 100))
                    {
                        iResult = 26; // Giant-Frog
                    }
                    iMapLevel = 4;
                    break;

                case 19:
                    if ((iResult >= 1) && (iResult < 15))
                    {
                        iResult = 44;
                    }
                    else if ((iResult >= 15) && (iResult < 25))
                    {
                        iResult = 46;
                    }
                    else if ((iResult >= 25) && (iResult < 35))
                    {
                        iResult = 21;
                    }
                    else if ((iResult >= 35) && (iResult < 60))
                    {
                        iResult = 43;
                    }
                    else if ((iResult >= 60) && (iResult < 85))
                    {
                        iResult = 23;
                    }
                    else if ((iResult >= 85) && (iResult <= 100))
                    {
                        iResult = 22;
                    }
                    iMapLevel = 4;
                    break;

                case 20:
                    if ((iResult >= 1) && (iResult < 2))
                    {
                        iResult = 41;
                    }
                    else if ((iResult >= 2) && (iResult < 3))
                    {
                        iResult = 40;
                    }
                    else if ((iResult >= 3) && (iResult < 8))
                    {
                        iResult = 53;
                    }
                    else if ((iResult >= 8) && (iResult < 9))
                    {
                        iResult = 39;
                    }
                    else if ((iResult >= 9) && (iResult < 20))
                    {
                        iResult = 21;
                    }
                    else if ((iResult >= 20) && (iResult < 35))
                    {
                        iResult = 16;
                    }
                    else if ((iResult >= 35) && (iResult < 45))
                    {
                        iResult = 44;
                    }
                    else if ((iResult >= 45) && (iResult < 55))
                    {
                        iResult = 45;
                    }
                    else if ((iResult >= 55) && (iResult < 75))
                    {
                        iResult = 28;
                    }
                    else if ((iResult >= 75) && (iResult < 95))
                    {
                        iResult = 43;
                    }
                    else if ((iResult >= 95) && (iResult < 100))
                    {
                        iResult = 22;
                    }
                    iMapLevel = 4;
                    break;

                case 21:
                    if ((iResult >= 1) && (iResult < 94))
                    {
                        iResult = 17; // Unicorn
                        bFirmBerserk = TRUE;
                    }
                    else if ((iResult >= 94) && (iResult < 95))
                    {
                        iResult = 36; // Wyvern
                    }
                    else if ((iResult >= 95) && (iResult < 96))
                    {
                        iResult = 37; // Fire-Wyvern
                    }
                    else if ((iResult >= 96) && (iResult < 97))
                    {
                        iResult = 47; // MasterMage-Orc
                    }
                    else if ((iResult >= 97) && (iResult < 98))
                    {
                        iResult = 35; // Hellclaw
                    }
                    else if ((iResult >= 98) && (iResult < 99))
                    {
                        iResult = 49; // Tigerworm
                    }
                    else if ((iResult >= 99) && (iResult <= 100))
                    {
                        iResult = 51; // Abaddon
                    }
                    iMapLevel = 4;
                    break;

                }

                pX = NULL;
                pY = NULL;

                if ((m_bIsSpecialEventTime == TRUE) && (iDice(1, 10) == 3)) bIsSpecialEvent = TRUE;

                if (bIsSpecialEvent == TRUE)
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

                            if (m_bIsCrusadeMode == TRUE)
                            {
                                if (strcmp(m_pMapList[i]->m_cName, "aresden") == 0)
                                    switch (iDice(1, 6))
                                    {
                                    case 1: iResult = 20; break;
                                    case 2: iResult = 53; break;
                                    case 3: iResult = 55; break;
                                    case 4: iResult = 57; break;
                                    case 5: iResult = 59; break;
                                    case 6: iResult = 61; break;
                                    }
                                else if (strcmp(m_pMapList[i]->m_cName, "elvine") == 0)
                                    switch (iDice(1, 6))
                                    {
                                    case 1: iResult = 19; break;
                                    case 2: iResult = 52; break;
                                    case 3: iResult = 54; break;
                                    case 4: iResult = 56; break;
                                    case 5: iResult = 58; break;
                                    case 6: iResult = 60; break;
                                    }
                            }
                            wsprintf(G_cTxt, "(!) Mob-Event Map(%s)[%d (%d,%d)]", m_pMapList[i]->m_cName, iResult, pX, pY);
                        }
                        break;

                    case 2:
                        if (iDice(1, 3) == 2)
                        {
                            if ((memcmp(m_pMapList[i]->m_cLocationName, "aresden", 7) == 0) ||
                                (memcmp(m_pMapList[i]->m_cLocationName, "middled1n", 9) == 0) ||
                                (memcmp(m_pMapList[i]->m_cLocationName, "arefarm", 7) == 0) ||
                                (memcmp(m_pMapList[i]->m_cLocationName, "elvfarm", 7) == 0) ||
                                (memcmp(m_pMapList[i]->m_cLocationName, "elvine", 6) == 0))
                            {
                                if (iDice(1, 30) == 5)
                                    iResult = 16;
                                else iResult = 5;
                            }
                            else iResult = 16;
                        }
                        else iResult = 17;

                        m_bIsSpecialEventTime = FALSE;
                        break;
                    }
                }

                ZeroMemory(cNpcName, sizeof(cNpcName));
                //Random Monster Spawns
                switch (iResult)
                {
                    case 1:  strcpy(cNpcName, "Slime");				iNpcID = 10; iProbSA = 5;  iKindSA = 1; break;
                    case 2:  strcpy(cNpcName, "Giant-Ant");			iNpcID = 16; iProbSA = 10; iKindSA = 2; break;
                    case 3:  strcpy(cNpcName, "Orc");				iNpcID = 14; iProbSA = 15; iKindSA = 1; break;
                    case 4:  strcpy(cNpcName, "Zombie");			iNpcID = 18; iProbSA = 15; iKindSA = 3; break;
                    case 5:  strcpy(cNpcName, "Skeleton");			iNpcID = 11; iProbSA = 35; iKindSA = 8; break;
                    case 6:  strcpy(cNpcName, "Orc-Mage");			iNpcID = 14; iProbSA = 30; iKindSA = 7; break;
                    case 7:  strcpy(cNpcName, "Scorpion");			iNpcID = 17; iProbSA = 15; iKindSA = 3; break;
                    case 8:  strcpy(cNpcName, "Stone-Golem");		iNpcID = 12; iProbSA = 25; iKindSA = 5; break;
                    case 9:  strcpy(cNpcName, "Cyclops");			iNpcID = 13; iProbSA = 35; iKindSA = 8; break;
                    case 10: strcpy(cNpcName, "Amphis");			iNpcID = 22; iProbSA = 20; iKindSA = 3; break;
                    case 11: strcpy(cNpcName, "Clay-Golem");		iNpcID = 23; iProbSA = 20; iKindSA = 5; break;
                    case 12: strcpy(cNpcName, "Troll");				iNpcID = 28; iProbSA = 25; iKindSA = 3; break;
                    case 13: strcpy(cNpcName, "Orge");				iNpcID = 29; iProbSA = 25; iKindSA = 1; break;
                    case 14: strcpy(cNpcName, "Hellbound");			iNpcID = 27; iProbSA = 25; iKindSA = 8; break;
                    case 15: strcpy(cNpcName, "Liche");				iNpcID = 30; iProbSA = 30; iKindSA = 8; break;
                    case 16: strcpy(cNpcName, "Demon");				iNpcID = 31; iProbSA = 20; iKindSA = 8; break;
                    case 17: strcpy(cNpcName, "Unicorn");			iNpcID = 32; iProbSA = 35; iKindSA = 7; break;
                    case 18: strcpy(cNpcName, "WereWolf");			iNpcID = 33; iProbSA = 25; iKindSA = 1; break;
                    case 19: strcpy(cNpcName, "YB-Aresden");		iNpcID = -1;  iProbSA = 15; iKindSA = 1; break;
                    case 20: strcpy(cNpcName, "YB-Elvine");			iNpcID = -1;  iProbSA = 15; iKindSA = 1; break;
                    case 21: strcpy(cNpcName, "Gagoyle");			iNpcID = 52; iProbSA = 20; iKindSA = 8; break;
                    case 22: strcpy(cNpcName, "Beholder");			iNpcID = 53; iProbSA = 20; iKindSA = 5; break;
                    case 23: strcpy(cNpcName, "Dark-Elf");			iNpcID = 54; iProbSA = 20; iKindSA = 3; break;
                    case 24: strcpy(cNpcName, "Rabbit");			iNpcID = -1; iProbSA = 5;  iKindSA = 1; break;
                    case 25: strcpy(cNpcName, "Cat");				iNpcID = -1; iProbSA = 10; iKindSA = 2; break;
                    case 26: strcpy(cNpcName, "Giant-Frog");		iNpcID = 57; iProbSA = 10; iKindSA = 2; break;
                    case 27: strcpy(cNpcName, "Mountain-Giant");	iNpcID = 58; iProbSA = 25; iKindSA = 1; break;
                    case 28: strcpy(cNpcName, "Ettin");				iNpcID = 59; iProbSA = 20; iKindSA = 8; break;
                    case 29: strcpy(cNpcName, "Cannibal-Plant");	iNpcID = 60; iProbSA = 20; iKindSA = 5; break;
                    case 30: strcpy(cNpcName, "Rudolph");			iNpcID = -1; iProbSA = 20; iKindSA = 5; break;
                    case 31: strcpy(cNpcName, "Ice-Golem");			iNpcID = 65; iProbSA = 35; iKindSA = 8; break;
                    case 32: strcpy(cNpcName, "DireBoar");			iNpcID = 62; iProbSA = 20; iKindSA = 5; break;
                    case 33: strcpy(cNpcName, "Frost");				iNpcID = 63; iProbSA = 30; iKindSA = 8; break;
                    case 34: strcpy(cNpcName, "Stalker");           iNpcID = 48; iProbSA = 20; iKindSA = 1; break;
                    case 35: strcpy(cNpcName, "Hellclaw");			iNpcID = 49; iProbSA = 20; iKindSA = 1; break;
                    case 36: strcpy(cNpcName, "Wyvern");			iNpcID = 66; iProbSA = 20; iKindSA = 1; break;
                    case 37: strcpy(cNpcName, "Fire-Wyvern");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 38: strcpy(cNpcName, "Barlog");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 39: strcpy(cNpcName, "Tentocle");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 40: strcpy(cNpcName, "Centaurus");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 41: strcpy(cNpcName, "Giant-Lizard");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 42: strcpy(cNpcName, "Minotaurs");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 43: strcpy(cNpcName, "Tentocle");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 44: strcpy(cNpcName, "Claw-Turtle");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 45: strcpy(cNpcName, "Giant-Crayfish");	iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 46: strcpy(cNpcName, "Giant-Plant");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 47: strcpy(cNpcName, "MasterMage-Orc");	iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 48: strcpy(cNpcName, "Nizie");				iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 49: strcpy(cNpcName, "Tigerworm");			iNpcID = 50; iProbSA = 20; iKindSA = 1; break;
                    case 50: strcpy(cNpcName, "Giant-Plant");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 51: strcpy(cNpcName, "Abaddon");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
                    case 52: strcpy(cNpcName, "YW-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 53: strcpy(cNpcName, "YW-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 54: strcpy(cNpcName, "YY-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 55: strcpy(cNpcName, "YY-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 56: strcpy(cNpcName, "XB-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 57: strcpy(cNpcName, "XB-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 58: strcpy(cNpcName, "XW-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 59: strcpy(cNpcName, "XW-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 60: strcpy(cNpcName, "XY-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    case 61: strcpy(cNpcName, "XY-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
                    default: strcpy(cNpcName, "Orc");				iNpcID = 14; iProbSA = 15; iKindSA = 1; break;
                }

                cSA = 0;
                if (iDice(1, 100) <= iProbSA)
                {
                    cSA = _cGetSpecialAbility(iKindSA);
                }

                if ((bMaster = bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, bFirmBerserk, TRUE)) == FALSE)
                {
                    m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
                }
                else
                {

                }
            }

            switch (iResult)
            {
            case 1:	 iTotalMob = iDice(1, 5) - 1; break;
            case 2:	 iTotalMob = iDice(1, 5) - 1; break;
            case 3:	 iTotalMob = iDice(1, 5) - 1; break;
            case 4:	 iTotalMob = iDice(1, 3) - 1; break;
            case 5:	 iTotalMob = iDice(1, 3) - 1; break;

            case 6:  iTotalMob = iDice(1, 3) - 1; break;
            case 7:  iTotalMob = iDice(1, 3) - 1; break;
            case 8:  iTotalMob = iDice(1, 2) - 1; break;
            case 9:  iTotalMob = iDice(1, 2) - 1; break;
            case 10: iTotalMob = iDice(1, 5) - 1; break;
            case 11: iTotalMob = iDice(1, 3) - 1; break;
            case 12: iTotalMob = iDice(1, 5) - 1; break;
            case 13: iTotalMob = iDice(1, 3) - 1; break;
            case 14: iTotalMob = iDice(1, 2) - 1; break;
            case 15: iTotalMob = iDice(1, 3) - 1; break;
            case 16: iTotalMob = iDice(1, 2) - 1; break;
            case 17: iTotalMob = iDice(1, 2) - 1; break;

            case 18: iTotalMob = iDice(1, 5) - 1; break;
            case 19: iTotalMob = iDice(1, 2) - 1; break;
            case 20: iTotalMob = iDice(1, 2) - 1; break;
            case 21: iTotalMob = iDice(1, 5) - 1; break;
            case 22: iTotalMob = iDice(1, 2) - 1; break;
            case 23: iTotalMob = iDice(1, 2) - 1; break;

            case 24: iTotalMob = iDice(1, 4) - 1; break;
            case 25: iTotalMob = iDice(1, 2) - 1; break;
            case 26: iTotalMob = iDice(1, 3) - 1; break;
            case 27: iTotalMob = iDice(1, 3) - 1; break;

            case 28: iTotalMob = iDice(1, 3) - 1; break;
            case 29: iTotalMob = iDice(1, 5) - 1; break;
            case 30: iTotalMob = iDice(1, 3) - 1; break;
            case 31: iTotalMob = iDice(1, 3) - 1; break;

            case 32: iTotalMob = 1; break;
            case 33: iTotalMob = 1; break;
            case 34: iTotalMob = 1; break;
            case 35: iTotalMob = 1; break;
            case 36: iTotalMob = 1; break;

            case 37: iTotalMob = 1; break;
            case 38: iTotalMob = 1; break;
            case 39: iTotalMob = 1; break;
            case 40: iTotalMob = 1; break;
            case 41: iTotalMob = 1; break;

            case 42: iTotalMob = iDice(1, 3) - 1; break;
            case 43: iTotalMob = 1; break;
            case 44: iTotalMob = iDice(1, 3) - 1; break;
            case 45: iTotalMob = 1; break;
            default: iTotalMob = 0; break;
            }

            if (bMaster == FALSE) iTotalMob = 0;

            if (iTotalMob > 2)
            {
                switch (iResult)
                {
                case 1:  // Slime 
                case 2:  // Giant-Ant
                case 3:  // Orc
                case 4:  // Zombie
                case 5:  // Skeleton
                case 6:  // Orc-Mage
                case 7:  // Scorpion
                case 8:  // Stone-Golem
                case 9:  // Cyclops
                case 10: // Amphis
                case 11: // Clay-Golem
                case 12: // Troll
                case 13: // Orge
                case 14: // Hellbound
                case 15: // Liche
                case 16: // Demon
                case 17: // Unicorn
                case 18: // WereWolf
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 32:
                    if (iDice(1, 5) == 1) iTotalMob = 0;  // 75%
                    break;

                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                    if (iDice(1, 5) != 1) iTotalMob = 0;  // 75%
                    break;
                }
            }

            if (bIsSpecialEvent == TRUE)
            {
                switch (m_cSpecialEventType)
                {
                case 1:
                    if ((iResult != 35) && (iResult != 36) && (iResult != 37) && (iResult != 49)
                        && (iResult != 51) && (iResult != 15) && (iResult != 16) && (iResult != 21)) iTotalMob = 12;
                    for (x = 1; x < DEF_MAXCLIENTS; x++)
                        if ((iNpcID != -1) && (m_pClientList[x] != NULL) && (m_pClientList[x]->m_bIsInitComplete == TRUE))
                        {
                            SendNotifyMsg(NULL, x, DEF_NOTIFY_SPAWNEVENT, pX, pY, iNpcID, NULL, NULL, NULL);
                        }
                    break;

                case 2:
                    if ((memcmp(m_pMapList[i]->m_cLocationName, "aresden", 7) == 0) ||
                        (memcmp(m_pMapList[i]->m_cLocationName, "elvine", 6) == 0) ||
                        (memcmp(m_pMapList[i]->m_cLocationName, "elvfarm", 7) == 0) ||
                        (memcmp(m_pMapList[i]->m_cLocationName, "arefarm", 7) == 0))
                    {
                        iTotalMob = 0;
                    }
                    break;
                }
                m_bIsSpecialEventTime = FALSE;
            }

            for (j = 0; j < iTotalMob; j++)
            {
                iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
                if (iNamingValue != -1)
                {
                    ZeroMemory(cName_Slave, sizeof(cName_Slave));
                    wsprintf(cName_Slave, "XX%d", iNamingValue);
                    cName_Slave[0] = 95;
                    cName_Slave[1] = i + 65;

                    cSA = 0;

                    if (iDice(1, 100) <= iProbSA)
                    {
                        cSA = _cGetSpecialAbility(iKindSA);
                    }

                    if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, bFirmBerserk) == FALSE)
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

        if ((m_pMapList[i] != NULL) && ((m_pMapList[i]->m_iMaximumObject) > m_pMapList[i]->m_iTotalActiveObject))
        {
            for (j = 1; j < DEF_MAXSPOTMOBGENERATOR; j++)
                if ((iDice(1, 3) == 2) && (m_pMapList[i]->m_stSpotMobGenerator[j].bDefined == TRUE) &&
                    (m_pMapList[i]->m_stSpotMobGenerator[j].iMaxMobs > m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs))
                {
                    iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
                    if (iNamingValue != -1)
                    {

                        ZeroMemory(cNpcName, sizeof(cNpcName));
                        switch (m_pMapList[i]->m_stSpotMobGenerator[j].iMobType)
                        {
                            // spot-mob-generator
                            case 10:  strcpy(cNpcName, "Slime");				iProbSA = 5;  iKindSA = 1;  break;
                            case 16:  strcpy(cNpcName, "Giant-Ant");			iProbSA = 10; iKindSA = 2;  break;
                            case 14:  strcpy(cNpcName, "Orc");				iProbSA = 15; iKindSA = 1;  break;
                            case 18:  strcpy(cNpcName, "Zombie");			iProbSA = 15; iKindSA = 3;  break;
                            case 11:  strcpy(cNpcName, "Skeleton");			iProbSA = 35; iKindSA = 8;  break;
                            case 6:   strcpy(cNpcName, "Orc-Mage");			iProbSA = 30; iKindSA = 7;  break;
                            case 17:  strcpy(cNpcName, "Scorpion");			iProbSA = 15; iKindSA = 3;  break;
                            case 12:  strcpy(cNpcName, "Stone-Golem");		iProbSA = 25; iKindSA = 5;  break;
                            case 13:  strcpy(cNpcName, "Cyclops");			iProbSA = 35; iKindSA = 8;  break;
                            case 22:  strcpy(cNpcName, "Amphis");			iProbSA = 20; iKindSA = 3;  break;
                            case 23:  strcpy(cNpcName, "Clay-Golem");		iProbSA = 20; iKindSA = 5;  break;
                            case 24:  strcpy(cNpcName, "Guard-Aresden");		iProbSA = 20; iKindSA = 1;  break;
                            case 25:  strcpy(cNpcName, "Guard-Elvine");		iProbSA = 20; iKindSA = 1;  break;
                            case 26:  strcpy(cNpcName, "Guard-Neutral");		iProbSA = 20; iKindSA = 1;  break;
                            case 27:  strcpy(cNpcName, "Hellbound");			iProbSA = 20; iKindSA = 1;  break;
                            case 29:  strcpy(cNpcName, "Orge");              iProbSA = 20; iKindSA = 1;  break;
                            case 30:  strcpy(cNpcName, "Liche");				iProbSA = 30; iKindSA = 8;  break;
                            case 31:  strcpy(cNpcName, "Demon");				iProbSA = 20; iKindSA = 8;  break;
                            case 32:  strcpy(cNpcName, "Unicorn");			iProbSA = 35; iKindSA = 7;  break;
                            case 33:  strcpy(cNpcName, "WereWolf");			iProbSA = 25; iKindSA = 1;  break;
                            case 34:  strcpy(cNpcName, "Dummy");				iProbSA = 5;  iKindSA = 1;  break;
                            case 35:  strcpy(cNpcName, "Attack-Dummy");		iProbSA = 5;  iKindSA = 1;  break;
                            case 48:  strcpy(cNpcName, "Stalker");			iProbSA = 20; iKindSA = 3;  break;
                            case 49:  strcpy(cNpcName, "Hellclaw");			iProbSA = 20; iKindSA = 8;  break;
                            case 50:  strcpy(cNpcName, "Tigerworm");			iProbSA = 20; iKindSA = 8;  break;
                            case 54:  strcpy(cNpcName, "Dark-Elf");			iProbSA = 20; iKindSA = 8;  break;
                            case 53:  strcpy(cNpcName, "Beholder");			iProbSA = 20; iKindSA = 8;  break;
                            case 52:  strcpy(cNpcName, "Gagoyle");			iProbSA = 20; iKindSA = 8;  break;
                            case 57:  strcpy(cNpcName, "Giant-Frog");		iProbSA = 10; iKindSA = 2;  break;
                            case 58:  strcpy(cNpcName, "Mountain-Giant");	iProbSA = 25; iKindSA = 1;  break;
                            case 59:  strcpy(cNpcName, "Ettin");				iProbSA = 20; iKindSA = 8;  break;
                            case 60:  strcpy(cNpcName, "Cannibal-Plant");	iProbSA = 20; iKindSA = 5;  break;
                            case 61:  strcpy(cNpcName, "Rudolph");			iProbSA = 20; iKindSA = 1;  break;
                            case 62:  strcpy(cNpcName, "DireBoar");			iProbSA = 20; iKindSA = 1;  break;
                            case 63:  strcpy(cNpcName, "Frost");				iProbSA = 20; iKindSA = 8;  break;
                            case 65:  strcpy(cNpcName, "Ice-Golem");			iProbSA = 20; iKindSA = 8;  break;
                            case 66:  strcpy(cNpcName, "Wyvern");			iProbSA = 20; iKindSA = 1;  break;
                            case 55:  strcpy(cNpcName, "Rabbit");			iProbSA = 20; iKindSA = 1;  break;
                            case 67:  strcpy(cNpcName, "McGaffin");			iProbSA = 20; iKindSA = 1;  break;
                            case 68:  strcpy(cNpcName, "Perry");				iProbSA = 20; iKindSA = 1;  break;
                            case 69:  strcpy(cNpcName, "Devlin");			iProbSA = 20; iKindSA = 1;  break;
                            case 73:  strcpy(cNpcName, "Fire-Wyvern");		iProbSA = 20; iKindSA = 1;  break;
                            case 70:  strcpy(cNpcName, "Barlog");			iProbSA = 20; iKindSA = 1;  break;
                            case 80:  strcpy(cNpcName, "Tentocle");			iProbSA = 20; iKindSA = 1;  break;
                            case 71:  strcpy(cNpcName, "Centaurus");			iProbSA = 20; iKindSA = 1;  break;
                            case 75:  strcpy(cNpcName, "Giant-Lizard");		iProbSA = 20; iKindSA = 1;  break;
                            case 78:  strcpy(cNpcName, "Minotaurs");			iProbSA = 20; iKindSA = 1;  break;
                            case 81:  strcpy(cNpcName, "Abaddon");			iProbSA = 20; iKindSA = 1;  break;
                            case 72:  strcpy(cNpcName, "Claw-Turtle");		iProbSA = 20; iKindSA = 1;  break;
                            case 74:  strcpy(cNpcName, "Giant-Crayfish");	iProbSA = 20; iKindSA = 1;  break;
                            case 76:  strcpy(cNpcName, "Giant-Plant");		iProbSA = 20; iKindSA = 1;  break;
                            case 77:  strcpy(cNpcName, "MasterMage-Orc");	iProbSA = 20; iKindSA = 1;  break;
                            case 79:  strcpy(cNpcName, "Nizie");				iProbSA = 20; iKindSA = 1; break;
                            default:
                                strcpy(cNpcName, "Orc");
                                iProbSA = 15;
                                iKindSA = 1;
                                break;
                        }
                        /* NPCs not spawning in pits:
                        case 56:  strcpy(cNpcName,"Cat");				iProbSA = 15; iKindSA = 6;  break;
                        case 28:  strcpy(cNpcName, "Troll");			iProbSA = 25; iKindSA = 3; break;

                        // 15 ShopKeeper-W
                        // 19 Gandlf
                        // 20 Howard
                        // 36 Arrow Gaurd Tower Kit - Aresden, Elvine
                        // 37 Cannon Gaurd Tower Kit - Aresden, Elvine
                        // 38 Mana Collector Kit - Aresden, Elvine
                        // 39 Detector Constructor Kit - Aresden, Elvine
                        // 40 Energy Shield Generator - Aresden, Elvine
                        // 41 Grand Master Generator - Aresden Elvine
                        // 43 Light War Beetle - Aresden, Elvine
                        // 44 God's Hand Knight
                        // 45 Mounted God's Hand Knight
                        // 46 Temple Knight
                        // 47 Battle Golem
                        // 51 Catapult
                        // 64 Crops
                        */
                        bFirmBerserk = FALSE;
                        if ((iMapLevel == 5) && (iDice(1, 3) == 1)) bFirmBerserk = TRUE;

                        ZeroMemory(cName_Master, sizeof(cName_Master));
                        wsprintf(cName_Master, "XX%d", iNamingValue);
                        cName_Master[0] = 95;
                        cName_Master[1] = i + 65;

                        cSA = 0;
                        if ((m_pMapList[i]->m_stSpotMobGenerator[j].iMobType != 34) && (iDice(1, 100) <= iProbSA))
                        {
                            cSA = _cGetSpecialAbility(iKindSA);
                        }

                        switch (m_pMapList[i]->m_stSpotMobGenerator[j].cType)
                        {
                        case 1:
                            if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMAREA, &pX, &pY, cWaypoint, &m_pMapList[i]->m_stSpotMobGenerator[j].rcRect, j, -1, FALSE, FALSE, bFirmBerserk) == FALSE)
                            {
                                m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
                            }
                            else
                            {
                                m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
                            }
                            break;

                        case 2:
                            if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMWAYPOINT, NULL, NULL, m_pMapList[i]->m_stSpotMobGenerator[j].cWaypoint, NULL, j, -1, FALSE, FALSE, bFirmBerserk) == FALSE)
                            {
                                m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
                            }
                            else
                            {
                                m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
                            }
                            break;
                        }
                    }
                }
        }
    }
}

void CGame::CalcNextWayPointDestination(int iNpcH)
{
    short sRange, sX, sY;
    int i, j, iMapIndex;
    BOOL bFlag;

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

            bFlag = TRUE;
            for (j = 0; j < DEF_MAXMGAR; j++)
                if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left != -1)
                {
                    if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left) &&
                        (sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].right) &&
                        (sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].top) &&
                        (sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].bottom))
                    {
                        // Avoid Rect
                        bFlag = FALSE;
                    }
                }
            if (bFlag == TRUE) goto CNW_GET_VALIDLOC_SUCCESS;
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
    int  i, iNamingValue, iNumItem, iItemID, iItemIDs[MAX_NPCITEMDROP], iSlateID;
    char cTmp[30], cItemName[30];
    CItem * pItem, * pItem2;
    DWORD dwCount, dwTime;
    POINT ItemPositions[MAX_NPCITEMDROP];
    char cTemp[256];
    SYSTEMTIME SysTime;
    if (m_pNpcList[iNpcH] == NULL) return;

    dwTime = timeGetTime();

    //Init number of items to 1 unless its a multidrop;
    iNumItem = 0;
    iItemID = 0;

    SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, NULL, NULL, NULL);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(11, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

    ZeroMemory(cTmp, sizeof(cTmp));
    strcpy(cTmp, (char *)(m_pNpcList[iNpcH]->m_cName + 2));
    iNamingValue = atoi(cTmp);

    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalActiveObject--;

    if (m_pNpcList[iNpcH]->m_iSpotMobIndex != NULL)
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
    case 64: m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum(); break;

    }

    bRemoveFromDelayEventList(iNpcH, DEF_OWNERTYPE_NPC, NULL);
    if ((m_pNpcList[iNpcH]->m_bIsSummoned == FALSE) && (m_pNpcList[iNpcH]->m_bIsUnsummoned == FALSE))
    {
        pItem = new CItem;
        ZeroMemory(cItemName, sizeof(cItemName));
        switch (m_pNpcList[iNpcH]->m_sType)
        {

        case 10: // Slime
            if (iDice(1, 25) == 1) iItemID = 220; break; // SlimeJelly 
            break;

        case 11: // Skeleton
            switch (iDice(1, 2))
            {
            case 1:if (iDice(1, 20) == 1) iItemID = 219; break; // SkeletonBones
            case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 12: // Stone-Golem
            switch (iDice(1, 2))
            {
            case 1:	if (iDice(1, 30) == 1) iItemID = 221; break; // StoneGolemPiece
            case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 13: // Cyclops
            switch (iDice(1, 6))
            {
            case 1:	if (iDice(1, 36) == 1) iItemID = 194; break; // CyclopsEye
            case 2:	if (iDice(1, 40) == 1) iItemID = 195; break; // CyclopsHandEdge
            case 3:	if (iDice(1, 30) == 1) iItemID = 196; break; // CyclopsHeart
            case 4:	if (iDice(1, 22) == 1) iItemID = 197; break; // CyclopsMeat
            case 5:	if (iDice(1, 40) == 1) iItemID = 198; break; // CyclopsLeather
            case 6:	bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 14: // Orc, Orc-Mage
            switch (iDice(1, 4))
            {
            case 1:	if (iDice(1, 11) == 1) iItemID = 206; break; // OrcMeat
            case 2:	if (iDice(1, 20) == 1) iItemID = 207; break; // OrcLeather
            case 3:	if (iDice(1, 21) == 1) iItemID = 208; break; // OrcTeeth
            case 4: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 16: //Giant-Ant
            switch (iDice(1, 3))
            {
            case 1:	if (iDice(1, 9) == 1) iItemID = 192; break; // AntLeg
            case 2:	if (iDice(1, 10) == 1) iItemID = 193; break; // AntFeeler
            case 3:	bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 17: //Scorpion
            switch (iDice(1, 5))
            {
            case 1:	if (iDice(1, 50) == 1) iItemID = 215; break; // ScorpionPincers
            case 2:	if (iDice(1, 20) == 1) iItemID = 216; break; // ScorpionMeat
            case 3: if (iDice(1, 50) == 1) iItemID = 217; break; // ScorpionSting
            case 4: if (iDice(1, 40) == 1) iItemID = 218; break; // ScorpionSkin
            case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 18: //Zombie
            bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            break;

        case 22: //Amphis
            switch (iDice(1, 5))
            {
            case 1: if (iDice(1, 15) == 1) iItemID = 188; break; // SnakeMeat
            case 2:	if (iDice(1, 16) == 1) iItemID = 189; break; // SnakeSkin
            case 3:	if (iDice(1, 16) == 1) iItemID = 190; break; // SnakeTeeth
            case 4:	if (iDice(1, 17) == 1) iItemID = 191; break; // SnakeTongue
            case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 23: //Clay-Golem
            switch (iDice(1, 2))
            {
            case 1: if (iDice(1, 30) == 1) iItemID = 205; break; // LumpofClay
            case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;


        case 27: //Hellbound
            switch (iDice(1, 7))
            {
            case 1:	if (iDice(1, 40) == 1) iItemID = 199; break; // HelboundHeart
            case 2:	if (iDice(1, 38) == 1) iItemID = 200; break; // HelboundLeather
            case 3:	if (iDice(1, 38) == 1) iItemID = 201; break; // HelboundTail
            case 4:	if (iDice(1, 36) == 1) iItemID = 202; break; // HelboundTeeth
            case 5:	if (iDice(1, 36) == 1) iItemID = 203; break; // HelboundClaw
            case 6:	if (iDice(1, 50) == 1) iItemID = 204; break; // HelboundTongue
            case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 28: //Troll
            switch (iDice(1, 5))
            {
            case 1:	if (iDice(1, 35) == 1) iItemID = 222; break; // TrollHeart
            case 2:	if (iDice(1, 23) == 1) iItemID = 223; break; // TrollMeat
            case 3:	if (iDice(1, 25) == 1) iItemID = 224; break; // TrollLeather
            case 4:	if (iDice(1, 27) == 1) iItemID = 225; break; // TrollClaw
            case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 29: //Orge
            switch (iDice(1, 7))
            {
            case 1:	if (iDice(1, 20) == 1) iItemID = 209; break; // OgreHair
            case 2:	if (iDice(1, 22) == 1) iItemID = 210; break; // OgreHeart
            case 3:	if (iDice(1, 25) == 1) iItemID = 211; break; // OgreMeat
            case 4:	if (iDice(1, 25) == 1) iItemID = 212; break; // OgreLeather
            case 5:	if (iDice(1, 28) == 1) iItemID = 213; break; // OgreTeeth
            case 6:	if (iDice(1, 28) == 1) iItemID = 214; break; // OgreClaw
            case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 30: //Liche
            bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            break;

        case 31: //Demon
            switch (iDice(1, 5))
            {
            case 1:	if (iDice(1, 400) == 123) iItemID = 541; break; // DemonHeart
            case 2:	if (iDice(1, 1000) == 123) iItemID = 542; break; // DemonMeat
            case 3:	if (iDice(1, 200) == 123) iItemID = 543; break; // DemonLeather
            case 4:	if (iDice(1, 300) == 123) iItemID = 540; break; // DemonEye
            case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 32: //Unicorn
            switch (iDice(1, 5))
            {
            case 1:	if (iDice(1, 3000) == 396) iItemID = 544; break; // UnicornHeart
            case 2:	if (iDice(1, 500) == 3) iItemID = 545; break; // UnicornHorn
            case 3:	if (iDice(1, 100) == 3) iItemID = 546; break; // UnicornMeat
            case 4:	if (iDice(1, 200) == 3) iItemID = 547; break; // UnicornLeather
            case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 33: //WereWolf
            switch (iDice(1, 8))
            {
            case 1: if (iDice(1, 30) == 3) iItemID = 551; break; // WerewolfTail
            case 2:	if (iDice(1, 28) == 3) iItemID = 548; break; // WerewolfHeart
            case 3:	if (iDice(1, 25) == 3) iItemID = 550; break; // WerewolfMeat
            case 4:	if (iDice(1, 35) == 3) iItemID = 553; break; // WerewolfLeather
            case 5:	if (iDice(1, 28) == 3) iItemID = 552; break; // WerewolfTeeth
            case 6:	if (iDice(1, 28) == 3) iItemID = 554; break; // WerewolfClaw
            case 7:	if (iDice(1, 38) == 3) iItemID = 549; break; // WerewolfNail
            case 8: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;
            default: break;
            }
            break;

        case 48: //Stalker
        case 49: //Hellclaw
        case 50: //Tigerworm
        case 52: //Gagoyle
        case 53: //Beholder
        case 54: //Dark-Elf
        case 55: //Rabbit
        case 56: //Cat
        case 57: //Giant-Frog
        case 58: //Mountain-Giant
        case 59: //Ettin
        case 60: //Cannibal-Plant
        case 61: //Rudolph
        case 62: //DireBoar
        case 63: //Frost
        case 65: //Ice
        case 70: //Barlog
            bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType);
            break;

        case 66: // Wyvern
            bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,
                50,
                5,
                15,
                m_pNpcList[iNpcH]->m_sX,
                m_pNpcList[iNpcH]->m_sY,
                DEF_ITEMSPREAD_FIXED,
                4,
                iItemIDs,
                ItemPositions,
                &iNumItem);
            break;

        case 73: // Fire-Wyvern
            bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,
                50,
                5,
                15,
                m_pNpcList[iNpcH]->m_sX,
                m_pNpcList[iNpcH]->m_sY,
                2,
                4,
                iItemIDs,
                ItemPositions,
                &iNumItem);
            break;
        case 81:
            bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,
                50,
                12,
                20,
                m_pNpcList[iNpcH]->m_sX,
                m_pNpcList[iNpcH]->m_sY,
                DEF_ITEMSPREAD_FIXED,
                65,
                iItemIDs,
                ItemPositions,
                &iNumItem);
            break;
        }

        dwCount = 1;
        if (iNumItem > 0)
        {
            GetLocalTime(&SysTime);
            wsprintf(cTemp, "%d%02d%", SysTime.wMonth, SysTime.wDay);
            for (int j = 0; j < iNumItem; j++)
            {
                if (pItem == NULL)
                {
                    pItem = new CItem;
                }
                if (_bInitItemAttr(pItem, iItemIDs[j]) == FALSE ||
                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetIsMoveAllowedTile(ItemPositions[j].x, ItemPositions[j].y) == FALSE)
                {
                    delete pItem;
                    pItem = NULL;
                }
                else
                {
                    if (iItemIDs[j] == 90) // Gold
                        pItem->m_dwCount = iDice(10, 15000);
                    else
                        pItem->m_dwCount = dwCount;

                    pItem->m_sTouchEffectType = DEF_ITET_ID;
                    pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                    pItem->m_sTouchEffectValue2 = iDice(1, 100000);
                    pItem->m_sTouchEffectValue3 = (short)timeGetTime();
                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(ItemPositions[j].x, ItemPositions[j].y, pItem);
                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
                        ItemPositions[j].x, ItemPositions[j].y, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
                    _bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, m_pNpcList[iNpcH]->m_cNpcName, pItem);
                    pItem = NULL;
                }
            }
        }
        else
        {
            if (_bInitItemAttr(pItem, iItemID) == FALSE)
            {
                delete pItem;
                pItem = NULL;
            }
            else
            {
                pItem->m_dwCount = dwCount;

                pItem->m_sTouchEffectType = DEF_ITET_ID;
                pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                pItem->m_sTouchEffectValue2 = iDice(1, 100000);
                pItem->m_sTouchEffectValue3 = (short)timeGetTime();
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem);
                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
                    m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
                _bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, m_pNpcList[iNpcH]->m_cNpcName, pItem);
            }
        }

        if (iDice(1, 100000) < 10)
        {
            pItem2 = new CItem;
            switch (iDice(1, 4))
            {
            case 1:	iSlateID = 868; break;
            case 2: iSlateID = 869; break;
            case 3: iSlateID = 870; break;
            case 4: iSlateID = 871; break;
            }
            if (_bInitItemAttr(pItem2, iSlateID) == FALSE)
            {
                delete pItem2;
                pItem2 = NULL;
            }
            else
            {
                pItem2->m_dwCount = 1;

                pItem2->m_sTouchEffectType = DEF_ITET_ID;
                pItem2->m_sTouchEffectValue1 = iDice(1, 100000);
                pItem2->m_sTouchEffectValue2 = iDice(1, 100000);
                pItem2->m_sTouchEffectValue3 = (short)timeGetTime();

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem2);
                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
                    m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem2->m_sSprite, pItem2->m_sSpriteFrame, pItem2->m_cItemColor);
                _bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, m_pNpcList[iNpcH]->m_cNpcName, pItem2);
            }
        }
    }

    delete m_pNpcList[iNpcH];
    m_pNpcList[iNpcH] = NULL;
}

int CGame::_iGetArrowItemIndex(int iClientH)
{
    int i;

    if (m_pClientList[iClientH] == NULL) return -1;

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
        {
            if ((m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_ARROW) &&
                (m_pClientList[iClientH]->m_pItemList[i]->m_dwCount > 0))
                return i;
        }

    return -1;
}

BOOL CGame::bCheckResistingMagicSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio)
{
    double dTmp1{}, dTmp2{}, dTmp3{};
    int    iTargetMagicResistRatio{}, iDestHitRatio{}, iResult{};
    char   cTargetDir{}, cProtect{};

    switch (cTargetType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sTargetH] == NULL) return FALSE;
        if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) return FALSE;
        if (m_pClientList[sTargetH]->m_iAdminUserLevel > 0) return TRUE;
        if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return TRUE;
        cTargetDir = m_pClientList[sTargetH]->m_cDir;
        iTargetMagicResistRatio = m_pClientList[sTargetH]->m_cSkillMastery[3] + m_pClientList[sTargetH]->m_iAddMR;
        if (m_pClientList[sTargetH]->m_iMag > 50)
            iTargetMagicResistRatio += (m_pClientList[sTargetH]->m_iMag - 50);
        iTargetMagicResistRatio += m_pClientList[sTargetH]->m_iAddResistMagic;
        cProtect = m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT];
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sTargetH] == NULL) return FALSE;
        cTargetDir = m_pNpcList[sTargetH]->m_cDir;
        iTargetMagicResistRatio = m_pNpcList[sTargetH]->m_cResistMagic;
        cProtect = m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT];
        break;
    }

    if (cProtect == 5) return TRUE;

    if ((iHitRatio < 1000) && (cProtect == 2)) return TRUE;
    if (iHitRatio >= 10000) iHitRatio -= 10000;
    if (iTargetMagicResistRatio < 1) iTargetMagicResistRatio = 1;
    if ((cAttackerDir != 0) && (m_pClientList[sTargetH] != NULL) && (m_pClientList[sTargetH]->m_cHeroArmorBonus == 2))
    {
        iHitRatio += 50;
    }

    dTmp1 = (double)(iHitRatio);
    dTmp2 = (double)(iTargetMagicResistRatio);
    dTmp3 = (dTmp1 / dTmp2) * 50.0f;
    iDestHitRatio = (int)(dTmp3);

    if (iDestHitRatio < DEF_MINIMUMHITRATIO) iDestHitRatio = DEF_MINIMUMHITRATIO;
    if (iDestHitRatio > DEF_MAXIMUMHITRATIO) iDestHitRatio = DEF_MAXIMUMHITRATIO;
    if (iDestHitRatio >= 100) return FALSE;

    iResult = iDice(1, 100);
    if (iResult <= iDestHitRatio) return FALSE;

    if (cTargetType == DEF_OWNERTYPE_PLAYER)
        CalculateSSN_SkillIndex(sTargetH, 3, 1);
    return TRUE;
}

BOOL CGame::bCheckResistingIceSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio)
{
    int    iTargetIceResistRatio, iResult;

    switch (cTargetType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sTargetH] == NULL) return FALSE;
        if (m_pClientList[sTargetH]->m_iAdminUserLevel > 0) return TRUE;
        iTargetIceResistRatio = m_pClientList[sTargetH]->m_iAddAbsWater * 2;
        if (m_pClientList[sTargetH]->m_dwWarmEffectTime == NULL)
        {
        }
        else if ((timeGetTime() - m_pClientList[sTargetH]->m_dwWarmEffectTime) < 1000 * 30) return TRUE;
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sTargetH] == NULL) return FALSE;
        iTargetIceResistRatio = (m_pNpcList[sTargetH]->m_cResistMagic) - (m_pNpcList[sTargetH]->m_cResistMagic / 3);
        break;
    }

    if (iTargetIceResistRatio < 1) iTargetIceResistRatio = 1;

    iResult = iDice(1, 100);
    if (iResult <= iTargetIceResistRatio) return TRUE;

    return FALSE;
}

BOOL CGame::bSetItemToBankItem(int iClientH, CItem * pItem)
{
    int i, iRet;
    DWORD * dwp;
    WORD * wp;
    char * cp;
    short * sp;
    char cData[100]{};

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (pItem == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_bIsInsideWarehouse == FALSE) return FALSE;

    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] == NULL)
        {

            m_pClientList[iClientH]->m_pItemInBankList[i] = pItem;

            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_ITEMTOBANK;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

            *cp = i;
            cp++;

            *cp = 1;
            cp++;

            memcpy(cp, pItem->m_cName, 20);
            cp += 20;

            dwp = (DWORD *)cp;
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

            wp = (WORD *)cp;
            *wp = pItem->m_wCurLifeSpan;
            cp += 2;

            wp = (WORD *)cp;
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

            sp = (short *)cp;
            *sp = pItem->m_sItemEffectValue2;
            cp += 2;

            dwp = (DWORD *)cp;
            *dwp = pItem->m_dwAttribute;
            cp += 4;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 55);
            switch (iRet)
            {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:
                // DeleteClient(iClientH, TRUE, TRUE);
                return TRUE;
            }

            return TRUE;
        }

    return FALSE;
}

BOOL CGame::bCheckTotalSkillMasteryPoints(int iClientH, int iSkill)
{
    int i;
    int iRemainPoint, iTotalPoints, iWeaponIndex, iDownSkillSSN, iDownPoint;
    short sDownSkillIndex;

    if (m_pClientList[iClientH] == NULL) return FALSE;

    iTotalPoints = 0;
    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        iTotalPoints += m_pClientList[iClientH]->m_cSkillMastery[i];

    iRemainPoint = iTotalPoints - DEF_MAXSKILLPOINTS;

    if (iRemainPoint > 0)
    {
        while (iRemainPoint > 0)
        {

            sDownSkillIndex = -1;
            if (m_pClientList[iClientH]->m_iDownSkillIndex != -1)
            {
                switch (m_pClientList[iClientH]->m_iDownSkillIndex)
                {
                case 3:

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

                if (m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] <= 20)
                    iDownPoint = m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex];
                else iDownPoint = 1;

                m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] -= iDownPoint;
                m_pClientList[iClientH]->m_iSkillSSN[sDownSkillIndex] = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] + 1] - 1;
                iRemainPoint -= iDownPoint;

                if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
                {
                    iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                    if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sDownSkillIndex)
                    {
                        m_pClientList[iClientH]->m_iHitRatio -= iDownPoint;
                        if (m_pClientList[iClientH]->m_iHitRatio < 0) m_pClientList[iClientH]->m_iHitRatio = 0;
                    }
                }

                if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
                {
                    iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                    if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sDownSkillIndex)
                    {
                        m_pClientList[iClientH]->m_iHitRatio -= iDownPoint;
                        if (m_pClientList[iClientH]->m_iHitRatio < 0) m_pClientList[iClientH]->m_iHitRatio = 0;
                    }
                }
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, sDownSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex], NULL, NULL);
            }
            else
            {
                return FALSE;
            }
        }
        return TRUE;
    }

    return FALSE;
}

int CGame::iGetFollowerNumber(short sOwnerH, char cOwnerType)
{
    int i, iTotal;

    iTotal = 0;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if ((m_pNpcList[i] != NULL) && (m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW))
        {

            if ((m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH) && (m_pNpcList[i]->m_cFollowOwnerType == cOwnerType))
                iTotal++;
        }

    return iTotal;
}

BOOL CGame::bRegisterDelayEvent(int iDelayType, int iEffectType, DWORD dwLastTime, int iTargetH, char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3)
{
    for (int i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] == NULL)
        {
            m_pDelayEventList[i] = new CDelayEvent;
            m_pDelayEventList[i]->m_iDelayType = iDelayType;
            m_pDelayEventList[i]->m_iEffectType = iEffectType;
            m_pDelayEventList[i]->m_cMapIndex = cMapIndex;
            m_pDelayEventList[i]->m_dX = dX;
            m_pDelayEventList[i]->m_dY = dY;
            m_pDelayEventList[i]->m_iTargetH = iTargetH;
            m_pDelayEventList[i]->m_cTargetType = cTargetType;
            m_pDelayEventList[i]->m_iV1 = iV1;
            m_pDelayEventList[i]->m_iV2 = iV2;
            m_pDelayEventList[i]->m_iV3 = iV3;
            m_pDelayEventList[i]->m_dwTriggerTime = dwLastTime;
            return TRUE;
        }
    return FALSE;
}

void CGame::DelayEventProcessor()
{
    int i, iSkillNum, iResult;
    DWORD dwTime = timeGetTime();
    int iTemp;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if ((m_pDelayEventList[i] != NULL) && (m_pDelayEventList[i]->m_dwTriggerTime < dwTime))
        {
            switch (m_pDelayEventList[i]->m_iDelayType)
            {

            case DEF_DELAYEVENTTYPE_ANCIENT_TABLET:
                if ((m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iStatus & 0x400000) != 0)
                {
                    iTemp = 1;
                }
                else if ((m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iStatus & 0x800000) != 0)
                {
                    iTemp = 3;
                }
                else if ((m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iStatus & 0x10000) != 0)
                {
                    iTemp = 4;
                }

                SendNotifyMsg(NULL, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_SLATE_STATUS, iTemp, NULL, NULL, NULL);
                SetSlateFlag(m_pDelayEventList[i]->m_iTargetH, iTemp, FALSE);
                break;

            case DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT:
                CalcMeteorStrikeEffectHandler(m_pDelayEventList[i]->m_cMapIndex);
                break;

            case DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE:
                DoMeteorStrikeDamageHandler(m_pDelayEventList[i]->m_cMapIndex);
                break;

            case DEF_DELAYEVENTTYPE_METEORSTRIKE:
                MeteorStrikeHandler(m_pDelayEventList[i]->m_cMapIndex);
                break;

            case DEF_DELAYEVENTTYPE_USEITEM_SKILL:
                switch (m_pDelayEventList[i]->m_cTargetType)
                {
                case DEF_OWNERTYPE_PLAYER:
                    iSkillNum = m_pDelayEventList[i]->m_iEffectType;

                    if (m_pClientList[m_pDelayEventList[i]->m_iTargetH] == NULL) break;
                    if (m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[iSkillNum] == FALSE) break;
                    if (m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[iSkillNum] != m_pDelayEventList[i]->m_iV2) break;

                    m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[iSkillNum] = FALSE;
                    m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[iSkillNum] = NULL;

                    iResult = iCalculateUseSkillItemEffect(m_pDelayEventList[i]->m_iTargetH, m_pDelayEventList[i]->m_cTargetType,
                        m_pDelayEventList[i]->m_iV1, iSkillNum, m_pDelayEventList[i]->m_cMapIndex, m_pDelayEventList[i]->m_dX, m_pDelayEventList[i]->m_dY);

                    SendNotifyMsg(NULL, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_SKILLUSINGEND, iResult, NULL, NULL, NULL);
                    break;
                }
                break;

            case DEF_DELAYEVENTTYPE_DAMAGEOBJECT:
                break;

            case DEF_DELAYEVENTTYPE_MAGICRELEASE:
                // Removes the aura after time
                switch (m_pDelayEventList[i]->m_cTargetType)
                {
                case DEF_OWNERTYPE_PLAYER:
                    if (m_pClientList[m_pDelayEventList[i]->m_iTargetH] == NULL) break;

                    SendNotifyMsg(NULL, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_MAGICEFFECTOFF,
                        m_pDelayEventList[i]->m_iEffectType, m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_cMagicEffectStatus[m_pDelayEventList[i]->m_iEffectType], NULL, NULL);

                    m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_cMagicEffectStatus[m_pDelayEventList[i]->m_iEffectType] = NULL;

                    // Inbitition casting 
                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INHIBITION)
                        m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bInhibition = FALSE;

                    // Invisibility
                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INVISIBILITY)
                        SetInvisibilityFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE);

                    // Berserk
                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_BERSERK)
                        SetBerserkFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE);

                    // Confusion
                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_CONFUSE)
                        switch (m_pDelayEventList[i]->m_iV1)
                        {
                        case 3: SetIllusionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE); break;
                        case 4: SetIllusionMovementFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE); break;
                        }

                    // Protection Magic
                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_PROTECT)
                    {
                        switch (m_pDelayEventList[i]->m_iV1)
                        {
                        case 1:
                            SetProtectionFromArrowFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE);
                            break;
                        case 2:
                        case 5:
                            SetMagicProtectionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE);
                            break;
                        case 3:
                        case 4:
                            SetDefenseShieldFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE);
                            break;
                        }
                    }


                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_POLYMORPH)
                    {
                        m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sType = m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sOriginalType;
                        SendEventToNearClient_TypeA(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                    }

                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_ICE)
                        SetIceFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, FALSE);
                    break;

                case DEF_OWNERTYPE_NPC:
                    if (m_pNpcList[m_pDelayEventList[i]->m_iTargetH] == NULL) break;

                    m_pNpcList[m_pDelayEventList[i]->m_iTargetH]->m_cMagicEffectStatus[m_pDelayEventList[i]->m_iEffectType] = NULL;

                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INVISIBILITY)
                        SetInvisibilityFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, FALSE);

                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_BERSERK)
                        SetBerserkFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, FALSE);

                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_POLYMORPH)
                    {
                        m_pNpcList[m_pDelayEventList[i]->m_iTargetH]->m_sType = m_pNpcList[m_pDelayEventList[i]->m_iTargetH]->m_sOriginalType;
                        SendEventToNearClient_TypeA(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                    }

                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_ICE)
                        SetIceFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, FALSE);

                    // Illusion
                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_CONFUSE)
                        SetIllusionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, FALSE);

                    // Protection Magic
                    if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_PROTECT)
                    {
                        switch (m_pDelayEventList[i]->m_iV1)
                        {
                        case 1:
                            SetProtectionFromArrowFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, FALSE);
                            break;
                        case 2:
                        case 5:
                            SetMagicProtectionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, FALSE);
                            break;
                        case 3:
                        case 4:
                            SetDefenseShieldFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, FALSE);
                            break;
                        }
                    }
                    break;
                }
                break;
            }

            delete m_pDelayEventList[i];
            m_pDelayEventList[i] = NULL;
        }
}

BOOL CGame::bRemoveFromDelayEventList(int iH, char cType, int iEffectType)
{
    int i;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] != NULL)
        {

            if (iEffectType == NULL)
            {
                if ((m_pDelayEventList[i]->m_iTargetH == iH) && (m_pDelayEventList[i]->m_cTargetType == cType))
                {
                    delete m_pDelayEventList[i];
                    m_pDelayEventList[i] = NULL;
                }
            }
            else
            {
                if ((m_pDelayEventList[i]->m_iTargetH == iH) && (m_pDelayEventList[i]->m_cTargetType == cType) &&
                    (m_pDelayEventList[i]->m_iEffectType == iEffectType))
                {
                    delete m_pDelayEventList[i];
                    m_pDelayEventList[i] = NULL;
                }
            }
        }

    return TRUE;
}

void CGame::SendObjectMotionRejectMsg(int iClientH)
{
    char * cp, cData[30]{};
    DWORD * dwp;
    WORD * wp;
    short * sp;
    int     iRet;

    m_pClientList[iClientH]->m_bIsMoveBlocked = TRUE;

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
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
        DeleteClient(iClientH, TRUE, TRUE);
        return;
    }
    return;
}

int CGame::_iGetTotalClients()
{
    int i, iTotal;

    iTotal = 0;
    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != NULL) iTotal++;

    return iTotal;
}

void CGame::DynamicObjectEffectProcessor()
{
    int i, ix, iy, iIndex;
    short sOwnerH, iDamage, sType;
    char  cOwnerType;
    DWORD dwTime = timeGetTime(), dwRegisterTime;

    for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != NULL)
        {
            switch (m_pDynamicObjectList[i]->m_sType)
            {

            case DEF_DYNAMICOBJECT_PCLOUD_BEGIN:
                for (ix = m_pDynamicObjectList[i]->m_sX - 1; ix <= m_pDynamicObjectList[i]->m_sX + 1; ix++)
                    for (iy = m_pDynamicObjectList[i]->m_sY - 1; iy <= m_pDynamicObjectList[i]->m_sY + 1; iy++)
                    {

                        m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (sOwnerH != NULL)
                        {
                            // Poison Damage
                            switch (cOwnerType)
                            {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) break;
                                if (m_pClientList[sOwnerH]->m_bIsKilled == TRUE) break;
                                //if ((m_pClientList[sOwnerH]->m_bIsNeutral == TRUE) && (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) == 0) break;

                                if (m_pDynamicObjectList[i]->m_iV1 < 20)
                                    iDamage = iDice(1, 6);
                                else iDamage = iDice(1, 8);

                                if (m_pClientList[sOwnerH]->m_iAdminUserLevel == 0)
                                    m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                {
                                    ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                }
                                else
                                {
                                    if (iDamage > 0)
                                    {
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);

                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                        {
                                            // 1: Hold-Person 
                                            // 2: Paralyze
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);

                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                                            bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                                        }
                                    }

                                    if ((bCheckResistingMagicSuccess(1, sOwnerH, DEF_OWNERTYPE_PLAYER, 100) == FALSE) &&
                                        (m_pClientList[sOwnerH]->m_bIsPoisoned == FALSE))
                                    {

                                        m_pClientList[sOwnerH]->m_bIsPoisoned = TRUE;
                                        m_pClientList[sOwnerH]->m_iPoisonLevel = m_pDynamicObjectList[i]->m_iV1;
                                        m_pClientList[sOwnerH]->m_dwPoisonTime = dwTime;
                                        SetPoisonFlag(sOwnerH, cOwnerType, TRUE);// poison aura appears from dynamic objects
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pClientList[sOwnerH]->m_iPoisonLevel, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) break;

                                if (m_pDynamicObjectList[i]->m_iV1 < 20)
                                    iDamage = iDice(1, 6);
                                else iDamage = iDice(1, 8);

                                switch (m_pNpcList[sOwnerH]->m_sType)
                                {
                                case 40: // ESG
                                case 41: // GMG
                                case 67: // McGaffin
                                case 68: // Perry
                                case 69: // Devlin
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
                                    NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0);
                                }
                                else
                                {
                                    if (iDice(1, 3) == 2)
                                        m_pNpcList[sOwnerH]->m_dwTime = dwTime;

                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                                    }

                                    SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, NULL);
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
                        if (sOwnerH != NULL)
                        {
                            switch (cOwnerType)
                            {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) break;
                                if (m_pClientList[sOwnerH]->m_bIsKilled == TRUE) break;

                                iDamage = iDice(3, 3) + 5;

                                if (m_pClientList[sOwnerH]->m_iAdminUserLevel == 0)
                                    m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                {
                                    ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                }
                                else
                                {
                                    if (iDamage > 0)
                                    {

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);

                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 1)
                                        {

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);

                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                                            bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                                        }
                                    }

                                    if ((bCheckResistingIceSuccess(1, sOwnerH, DEF_OWNERTYPE_PLAYER, m_pDynamicObjectList[i]->m_iV1) == FALSE) &&
                                        (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0))
                                    {

                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (20 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) break;

                                iDamage = iDice(3, 3) + 5;

                                switch (m_pNpcList[sOwnerH]->m_sType)
                                {
                                case 40: // ESG
                                case 41: // GMG
                                case 67: // McGaffin
                                case 68: // Perry
                                case 69: // Devlin
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

                                if (m_pNpcList[sOwnerH]->m_iHP <= 0)
                                {
                                    NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0);
                                }
                                else
                                {
                                    if (iDice(1, 3) == 2)
                                        m_pNpcList[sOwnerH]->m_dwTime = dwTime;

                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                                    }

                                    SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, NULL);

                                    if ((bCheckResistingIceSuccess(1, sOwnerH, DEF_OWNERTYPE_NPC, m_pDynamicObjectList[i]->m_iV1) == FALSE) &&
                                        (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0))
                                    {

                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (20 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    }
                                }
                                break;
                            }
                        }

                        m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
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
                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                                }
                            }
                        }

                        m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sType, &dwRegisterTime, &iIndex);
                        if (((sType == DEF_DYNAMICOBJECT_FIRE) || (sType == DEF_DYNAMICOBJECT_FIRE3)) && (m_pDynamicObjectList[iIndex] != NULL))
                            m_pDynamicObjectList[iIndex]->m_dwLastTime = m_pDynamicObjectList[iIndex]->m_dwLastTime - (m_pDynamicObjectList[iIndex]->m_dwLastTime / 10);
                    }
                break;

            case DEF_DYNAMICOBJECT_FIRE3:
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
                        if (sOwnerH != NULL)
                        {
                            switch (cOwnerType)
                            {

                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) break;
                                if (m_pClientList[sOwnerH]->m_bIsKilled == TRUE) break;
                                //if ((m_pClientList[sOwnerH]->m_bIsNeutral == TRUE) && (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) == 0) break;

                                iDamage = iDice(1, 6);
                                if (m_pClientList[sOwnerH]->m_iAdminUserLevel == 0)
                                    m_pClientList[sOwnerH]->m_iHP -= iDamage;

                                if (m_pClientList[sOwnerH]->m_iHP <= 0)
                                {
                                    ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);
                                }
                                else
                                {
                                    if (iDamage > 0)
                                    {
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);

                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                        {
                                            // 1: Hold-Person 
                                            // 2: Paralyze	
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);

                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                                            bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                                        }
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) break;

                                iDamage = iDice(1, 6);

                                switch (m_pNpcList[sOwnerH]->m_sType)
                                {
                                case 40: // ESG
                                case 41: // GMG
                                case 67: // McGaffin
                                case 68: // Perry
                                case 69: // Devlin
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
                                    NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0);
                                }
                                else
                                {
                                    if (iDice(1, 3) == 2)
                                        m_pNpcList[sOwnerH]->m_dwTime = dwTime;

                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                                    }

                                    SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, NULL);
                                }
                                break;
                            }
                        }

                        m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
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
                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                                }
                            }
                        }

                        m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sType, &dwRegisterTime, &iIndex);
                        if ((sType == DEF_DYNAMICOBJECT_ICESTORM) && (m_pDynamicObjectList[iIndex] != NULL))
                            m_pDynamicObjectList[iIndex]->m_dwLastTime = m_pDynamicObjectList[iIndex]->m_dwLastTime - (m_pDynamicObjectList[iIndex]->m_dwLastTime / 10);
                    }
                break;
            }
        }
}

char _tmp_cCorpseX[] = { 0,  1, 1, 1, 0, -1, -1, -1, 0, 0, 0, 0 };
char _tmp_cCorpseY[] = { -1, -1, 0, 1, 1,  1,  0, -1, 0, 0, 0 };

void CGame::ClearSkillUsingStatus(int iClientH)
{
    int i;
    short tX, fX, tY, fY;

    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == TRUE)
    {
        tX = m_pClientList[iClientH]->m_sX;
        tY = m_pClientList[iClientH]->m_sY;
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(tX, tY, NULL) == FALSE)
        {
            fX = m_pClientList[iClientH]->m_sX + _tmp_cCorpseX[m_pClientList[iClientH]->m_cDir];
            fY = m_pClientList[iClientH]->m_sY + _tmp_cCorpseY[m_pClientList[iClientH]->m_cDir];
            if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(fX, fY, NULL) == FALSE)
            {
                m_pClientList[iClientH]->m_cDir = iDice(1, 8);
                fX = m_pClientList[iClientH]->m_sX + _tmp_cCorpseX[m_pClientList[iClientH]->m_cDir];
                fY = m_pClientList[iClientH]->m_sY + _tmp_cCorpseY[m_pClientList[iClientH]->m_cDir];
                if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(fX, fY, NULL) == FALSE)
                {
                    return;
                }
            }
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DAMAGEMOVE, m_pClientList[iClientH]->m_cDir, NULL, NULL, NULL);
        }
    }
    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
    {
        m_pClientList[iClientH]->m_bSkillUsingStatus[i] = FALSE;
        m_pClientList[iClientH]->m_iSkillUsingTimeID[i] = NULL;
    }

    if (m_pClientList[iClientH]->m_iAllocatedFish != NULL)
    {
        if (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] != NULL)
            m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;

        m_pClientList[iClientH]->m_iAllocatedFish = NULL;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FISHCANCELED, NULL, NULL, NULL, NULL);
    }

}

int CGame::iCalculateUseSkillItemEffect(int iOwnerH, char cOwnerType, char cOwnerSkill, int iSkillNum, char cMapIndex, int dX, int dY)
{
    CItem * pItem;
    char  cItemName[30];
    short lX, lY;
    int   iResult, iFish;

    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[iOwnerH] == NULL) return 0;
        if (m_pClientList[iOwnerH]->m_cMapIndex != cMapIndex) return 0;
        lX = m_pClientList[iOwnerH]->m_sX;
        lY = m_pClientList[iOwnerH]->m_sY;
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[iOwnerH] == NULL) return 0;
        if (m_pNpcList[iOwnerH]->m_cMapIndex != cMapIndex) return 0;
        lX = m_pNpcList[iOwnerH]->m_sX;
        lY = m_pNpcList[iOwnerH]->m_sY;
        break;
    }

    if (cOwnerSkill == 0) return 0;

    iResult = iDice(1, 105);
    if (cOwnerSkill <= iResult)	return 0;

    if (m_pMapList[cMapIndex]->bGetIsWater(dX, dY) == FALSE) return 0;

    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
        CalculateSSN_SkillIndex(iOwnerH, iSkillNum, 1);

    switch (m_pSkillConfigList[iSkillNum]->m_sType)
    {
    case DEF_SKILLEFFECTTYPE_TAMING:
        _TamingHandler(iOwnerH, iSkillNum, cMapIndex, dX, dY);
        break;

    case DEF_SKILLEFFECTTYPE_GET:
        ZeroMemory(cItemName, sizeof(cItemName));
        switch (m_pSkillConfigList[iSkillNum]->m_sValue1)
        {
        case 1:
            wsprintf(cItemName, "Meat");
            break;

        case 2:
            //if (m_pMapList[cMapIndex]->bGetIsWater(dX, dY) == FALSE) return 0; 

            if (cOwnerType == DEF_OWNERTYPE_PLAYER)
            {
                iFish = iCheckFish(iOwnerH, cMapIndex, dX, dY);
                if (iFish == NULL) wsprintf(cItemName, "Fish");
            }
            else wsprintf(cItemName, "Fish");
            break;
        }

        if (strlen(cItemName) != 0)
        {
            if (memcmp(cItemName, "Fish", 6) == 0)
            {
                SendNotifyMsg(NULL, iOwnerH, DEF_NOTIFY_FISHSUCCESS, NULL, NULL, NULL, NULL);
                m_pClientList[iOwnerH]->m_iExpStock += iDice(1, 2);
            }

            pItem = new CItem;
            if (pItem == NULL) return 0;
            if (_bInitItemAttr(pItem, cItemName) == TRUE)
            {
                m_pMapList[cMapIndex]->bSetItem(lX, lY, pItem);

                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, cMapIndex,
                    lX, lY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
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

    if (m_pClientList[iClientH] == NULL) return 0;

    m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
    for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
        if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL)
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
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
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
    BOOL bFlag;
    char cTxt[120];
    return;
    if (m_pClientList[iClientH] == NULL) return;

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
            bFlag = FALSE;

            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iStr < 10))
            {
                m_pClientList[iClientH]->m_iStr++;
                iToBeRestoredPoint--;
                bFlag = TRUE;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iMag < 10))
            {
                m_pClientList[iClientH]->m_iMag++;
                iToBeRestoredPoint--;
                bFlag = TRUE;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iInt < 10))
            {
                m_pClientList[iClientH]->m_iInt++;
                iToBeRestoredPoint--;
                bFlag = TRUE;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iDex < 10))
            {
                m_pClientList[iClientH]->m_iDex++;
                iToBeRestoredPoint--;
                bFlag = TRUE;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iVit < 10))
            {
                m_pClientList[iClientH]->m_iVit++;
                iToBeRestoredPoint--;
                bFlag = TRUE;
            }
            if ((iToBeRestoredPoint > 0) && (m_pClientList[iClientH]->m_iCharisma < 10))
            {
                m_pClientList[iClientH]->m_iCharisma++;
                iToBeRestoredPoint--;
                bFlag = TRUE;
            }

            if (bFlag == FALSE)          break;
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
            bFlag = FALSE;
            if (m_pClientList[iClientH]->m_iStr > DEF_CHARPOINTLIMIT)
            {
                bFlag = TRUE;
                m_pClientList[iClientH]->m_iStr--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iDex > DEF_CHARPOINTLIMIT)
            {
                bFlag = TRUE;
                m_pClientList[iClientH]->m_iDex--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iVit > DEF_CHARPOINTLIMIT)
            {
                bFlag = TRUE;
                m_pClientList[iClientH]->m_iVit--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iInt > DEF_CHARPOINTLIMIT)
            {
                bFlag = TRUE;
                m_pClientList[iClientH]->m_iInt--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iMag > DEF_CHARPOINTLIMIT)
            {
                bFlag = TRUE;
                m_pClientList[iClientH]->m_iMag--;
                iToBeRestoredPoint++;
            }

            if (m_pClientList[iClientH]->m_iCharisma > DEF_CHARPOINTLIMIT)
            {
                bFlag = TRUE;
                m_pClientList[iClientH]->m_iCharisma--;
                iToBeRestoredPoint++;
            }

            if (bFlag == FALSE)	break;
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

        }
        else
        {
            wsprintf(cTxt, "(^o^) RestorePlayerCharacteristics(Over) SUCCESS! Player(%s)-(%d/%d)", m_pClientList[iClientH]->m_cCharName, iVerifyPoint, iOriginalPoint);
            log->info(cTxt);
        }
    }
}

void CGame::CalcTotalItemEffect(int iClientH, int iEquipItemID, BOOL bNotify)
{
    short sItemIndex;
    int  i, iArrowIndex, iPrevSAType, iTemp;
    char cEquipPos;
    double dV1, dV2, dV3;
    DWORD  dwSWEType, dwSWEValue;
    short  sTemp;

    if (m_pClientList[iClientH] == NULL) return;

    if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) &&
        (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1))
    {

        if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] != NULL)
        {
            m_pClientList[iClientH]->m_bIsItemEquipped[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] = FALSE;
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

    m_pClientList[iClientH]->m_bIsLuckyEffect = FALSE;
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

    iPrevSAType = m_pClientList[iClientH]->m_iSpecialAbilityType;

    m_pClientList[iClientH]->m_iSpecialAbilityType = 0;
    m_pClientList[iClientH]->m_iSpecialAbilityLastSec = 0;
    m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = 0;

    m_pClientList[iClientH]->m_iAddTransMana = 0;
    m_pClientList[iClientH]->m_iAddChargeCritical = 0;

    m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
    for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
        if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL)
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
        if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL) &&
            (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == TRUE))
        {

            cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

            switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
            {

            case DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE:
                m_pClientList[iClientH]->m_iMagicDamageSaveItemIndex = sItemIndex;
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

                m_pClientList[iClientH]->m_iAddPhysicalDamage += iTemp;
                m_pClientList[iClientH]->m_iAddMagicalDamage += iTemp;

                m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];

                //m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSM_HitRatio;
                //m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L  += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sL_HitRatio;
                m_pClientList[iClientH]->m_sUsingWeaponSkill = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill;

                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != NULL)
                {
                    m_pClientList[iClientH]->m_iCustomItemValue_Attack += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
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

                        if (m_pClientList[iClientH]->m_iMinAP_SM < 1) m_pClientList[iClientH]->m_iMinAP_SM = 1;
                        if (m_pClientList[iClientH]->m_iMinAP_L < 1)  m_pClientList[iClientH]->m_iMinAP_L = 1;

                        if (m_pClientList[iClientH]->m_iMinAP_SM > (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
                            m_pClientList[iClientH]->m_iMinAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);

                        if (m_pClientList[iClientH]->m_iMinAP_L > (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
                            m_pClientList[iClientH]->m_iMinAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);
                    }
                    else if (m_pClientList[iClientH]->m_iCustomItemValue_Attack < 0)
                    {
                        dV2 = (double)m_pClientList[iClientH]->m_iCustomItemValue_Attack;
                        dV1 = (dV2 / 100.0f) * (5.0f);
                        m_pClientList[iClientH]->m_iMaxAP_SM = m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM
                            + m_pClientList[iClientH]->m_cAttackBonus_SM + (int)dV1;

                        m_pClientList[iClientH]->m_iMaxAP_L = m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L
                            + m_pClientList[iClientH]->m_cAttackBonus_L + (int)dV1;

                        if (m_pClientList[iClientH]->m_iMaxAP_SM < 1) m_pClientList[iClientH]->m_iMaxAP_SM = 1;
                        if (m_pClientList[iClientH]->m_iMaxAP_L < 1)  m_pClientList[iClientH]->m_iMaxAP_L = 1;

                        if (m_pClientList[iClientH]->m_iMaxAP_SM < (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
                            m_pClientList[iClientH]->m_iMaxAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);

                        if (m_pClientList[iClientH]->m_iMaxAP_L < (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
                            m_pClientList[iClientH]->m_iMaxAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);
                    }
                }

                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
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

                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != NULL)
                {
                    dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
                    dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;

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

                    if ((bNotify == TRUE) && (iEquipItemID == (int)sItemIndex))
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, NULL);
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
                        m_pClientList[iClientH]->m_bIsLuckyEffect = TRUE;
                    else m_pClientList[iClientH]->m_bIsLuckyEffect = FALSE;
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

                case 12:
                    m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                    break;
                }
                break;

            case DEF_ITEMEFFECTTYPE_ATTACK_ARROW:
                if ((m_pClientList[iClientH]->m_cArrowIndex != -1) &&
                    (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cArrowIndex] == NULL))
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
                    m_pClientList[iClientH]->m_cAttackDiceThrow_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                    m_pClientList[iClientH]->m_cAttackDiceRange_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                    m_pClientList[iClientH]->m_cAttackBonus_SM = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
                    m_pClientList[iClientH]->m_cAttackDiceThrow_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
                    m_pClientList[iClientH]->m_cAttackDiceRange_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5;
                    m_pClientList[iClientH]->m_cAttackBonus_L = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
                }

                m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];
                break;

            case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
            case DEF_ITEMEFFECTTYPE_DEFENSE:
                m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;

                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != NULL)
                {
                    m_pClientList[iClientH]->m_iCustomItemValue_Defense += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;

                    dV2 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
                    dV3 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                    dV1 = (double)(dV2 / 100.0f) * dV3;

                    dV1 = dV1 / 2.0f;
                    m_pClientList[iClientH]->m_iDefenseRatio += (int)dV1;
                    if (m_pClientList[iClientH]->m_iDefenseRatio <= 0) m_pClientList[iClientH]->m_iDefenseRatio = 1;
                }

                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
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

                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != NULL)
                {
                    dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
                    dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;

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

                    if ((bNotify == TRUE) && (iEquipItemID == (int)sItemIndex))
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, NULL);
                    break;
                }
                break;
            }
        }

    if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType == 0) && (bNotify == TRUE))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 4, NULL, NULL, NULL);
        if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == TRUE)
        {
            m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = FALSE;
            m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xFF0F;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;
            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        }
    }

    if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType != 0) &&
        (iPrevSAType != m_pClientList[iClientH]->m_iSpecialAbilityType) && (bNotify == TRUE))
    {
        if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == TRUE)
        {
            SendNotifyMsg(NULL, i, DEF_NOTIFY_SPECIALABILITYSTATUS, 3, NULL, NULL, NULL);
            m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = FALSE;
            m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xFF0F;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;
            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
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
            if ((sOwnerH != NULL) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
                iSum++;
        }

    return iSum;
}

BOOL CGame::bAnalyzeCriminalAction(int iClientH, short dX, short dY, BOOL bIsCheck)
{
    int   iNamingValue, tX, tY;
    short sOwnerH;
    char  cOwnerType, cName[11], cNpcName[30];
    char  cNpcWaypoint[11];

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return FALSE;
    if (m_bIsCrusadeMode == TRUE) return FALSE;

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL))
    {
        if (_bGetIsPlayerHostile(iClientH, sOwnerH) != TRUE)
        {
            if (bIsCheck == TRUE) return TRUE;

            ZeroMemory(cNpcName, sizeof(cNpcName));
            if (strcmp(m_pClientList[iClientH]->m_cMapName, "aresden") == 0)
                strcpy(cNpcName, "Guard-Aresden");
            else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvine") == 0)
                strcpy(cNpcName, "Guard-Elvine");
            else  if (strcmp(m_pClientList[iClientH]->m_cMapName, "default") == 0)
                strcpy(cNpcName, "Guard-Neutral");

            iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
            if (iNamingValue == -1)
            {

            }
            else
            {
                ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
                ZeroMemory(cName, sizeof(cName));
                wsprintf(cName, "XX%d", iNamingValue);
                cName[0] = '_';
                cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

                tX = (int)m_pClientList[iClientH]->m_sX;
                tY = (int)m_pClientList[iClientH]->m_sY;
                if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM,
                    &tX, &tY, cNpcWaypoint, NULL, NULL, -1, FALSE, TRUE) == FALSE)
                {
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                }
                else
                {
                    bSetNpcAttackMode(cName, iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
                }
            }
        }
    }
    return FALSE;
}

BOOL CGame::_bGetIsPlayerHostile(int iClientH, int sOwnerH)
{
    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (m_pClientList[sOwnerH] == NULL) return FALSE;

    if (iClientH == sOwnerH) return TRUE;

    if (m_pClientList[iClientH]->m_cSide == 0)
    {
        if (m_pClientList[sOwnerH]->m_iPKCount != 0)
            return TRUE;
        else return FALSE;
    }
    else
    {
        if (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide)
        {
            if (m_pClientList[sOwnerH]->m_cSide == 0)
            {
                if (m_pClientList[sOwnerH]->m_iPKCount != 0)
                    return TRUE;
                else return FALSE;
            }
            else return TRUE;
        }
        else
        {
            if (m_pClientList[sOwnerH]->m_iPKCount != 0)
                return TRUE;
            else return FALSE;
        }
    }

    return FALSE;
}

void CGame::bSetNpcAttackMode(char * cName, int iTargetH, char cTargetType, BOOL bIsPermAttack)
{
    int i, iIndex;

    for (i = 1; i < DEF_MAXNPCS; i++)
        if ((m_pNpcList[i] != NULL) && (memcmp(m_pNpcList[i]->m_cName, cName, 5) == 0))
        {
            iIndex = i;
            goto NEXT_STEP_SNAM1;
        }

    return;

    NEXT_STEP_SNAM1:;

    switch (cTargetType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[iTargetH] == NULL) return;
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[iTargetH] == NULL) return;
        break;
    }

    m_pNpcList[iIndex]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
    m_pNpcList[iIndex]->m_sBehaviorTurnCount = 0;
    m_pNpcList[iIndex]->m_iTargetIndex = iTargetH;
    m_pNpcList[iIndex]->m_cTargetType = cTargetType;

    m_pNpcList[iIndex]->m_bIsPermAttackMode = bIsPermAttack;
}

void CGame::PoisonEffect(int iClientH, int iV1)
{
    int iPoisonLevel, iDamage, iPrevHP, iProb;

    if (m_pClientList[iClientH] == NULL)     return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    iPoisonLevel = m_pClientList[iClientH]->m_iPoisonLevel;

    iDamage = iDice(1, iPoisonLevel);

    iPrevHP = m_pClientList[iClientH]->m_iHP;
    m_pClientList[iClientH]->m_iHP -= iDamage;
    if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 1;

    if (iPrevHP != m_pClientList[iClientH]->m_iHP)
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);

    iProb = m_pClientList[iClientH]->m_cSkillMastery[23] - 10 + m_pClientList[iClientH]->m_iAddPR;
    if (iProb <= 10) iProb = 10;
    if (iDice(1, 100) <= iProb)
    {
        m_pClientList[iClientH]->m_bIsPoisoned = FALSE;
        SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE); // remove poison aura after effect complete
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, NULL, NULL, NULL);
    }
}

BOOL CGame::bCheckResistingPoisonSuccess(short sOwnerH, char cOwnerType)
{
    int iResist, iResult;

    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return FALSE;
        iResist = m_pClientList[sOwnerH]->m_cSkillMastery[23] + m_pClientList[sOwnerH]->m_iAddPR;
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return FALSE;
        iResist = 0;
        break;
    }

    iResult = iDice(1, 100);
    if (iResult >= iResist)
        return FALSE;

    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
        CalculateSSN_SkillIndex(sOwnerH, 23, 1);

    return TRUE;
}

BOOL CGame::bCheckBadWord(char * pString)
{
    char * cp;

    cp = pString;
    while (*cp != NULL)
    {

        cp++;
    }

    return FALSE;
}

void CGame::CheckDayOrNightMode()
{
    SYSTEMTIME SysTime;
    char cPrevMode;
    int  i;

    if (m_bManualTime = TRUE) return;

    cPrevMode = m_cDayOrNight;

    GetLocalTime(&SysTime);
    if (SysTime.wMinute >= DEF_NIGHTTIME)
        m_cDayOrNight = 2;
    else m_cDayOrNight = 1;

    if (cPrevMode != m_cDayOrNight)
    {
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
            {
                if ((m_pClientList[i]->m_cMapIndex >= 0) &&
                    (m_pMapList[m_pClientList[i]->m_cMapIndex] != NULL) &&
                    (m_pMapList[m_pClientList[i]->m_cMapIndex]->m_bIsFixedDayMode == FALSE))
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TIMECHANGE, m_cDayOrNight, NULL, NULL, NULL);
            }
    }
}

void CGame::ResponseSavePlayerDataReplyHandler(char * pData, DWORD dwMsgSize)
{
    char * cp, cCharName[11];
    int i;

    ZeroMemory(cCharName, sizeof(cCharName));

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memcpy(cCharName, cp, 10);

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != NULL)
        {
            if (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0)
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_SERVERCHANGE, NULL, NULL, NULL, NULL);
            }
        }
}

void CGame::CalcExpStock(int iClientH)
{
    BOOL bIsLevelUp;
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_iExpStock <= 0) return;
    //if ((m_pClientList[iClientH]->m_iLevel >= m_iPlayerMaxLevel) && (m_pClientList[iClientH]->m_iExp >= m_iLevelExpTable[m_iPlayerMaxLevel])) return;

    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD)
    {
        m_pClientList[iClientH]->m_iExpStock = 0;
        return;
    }

    m_pClientList[iClientH]->m_iExp += m_pClientList[iClientH]->m_iExpStock;
    m_pClientList[iClientH]->m_iAutoExpAmount += m_pClientList[iClientH]->m_iExpStock;
    m_pClientList[iClientH]->m_iExpStock = 0;

    if (bCheckLimitedUser(iClientH) == FALSE)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, NULL, NULL, NULL, NULL);
    }
    bIsLevelUp = bCheckLevelUp(iClientH);

    if ((bIsLevelUp == TRUE) && (m_pClientList[iClientH]->m_iLevel <= 5))
    {
        pItem = new CItem;
        if (_bInitItemAttr(pItem, "Gold") == FALSE)
        {
            delete pItem;
            return;
        }
        else pItem->m_dwCount = (DWORD)100000;
        bAddItem(iClientH, pItem, NULL);
    }

    if ((bIsLevelUp == TRUE) && (m_pClientList[iClientH]->m_iLevel > 5) && (m_pClientList[iClientH]->m_iLevel <= 20))
    {
        pItem = new CItem;
        if (_bInitItemAttr(pItem, "Gold") == FALSE)
        {
            delete pItem;
            return;
        }
        else pItem->m_dwCount = (DWORD)100000;
        bAddItem(iClientH, pItem, NULL);
    }
}

void CGame::___RestorePlayerRating(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_iRating < -10000) m_pClientList[iClientH]->m_iRating = 0;
    if (m_pClientList[iClientH]->m_iRating > 10000) m_pClientList[iClientH]->m_iRating = 0;
}

int CGame::iGetExpLevel(int iExp)
{
    int i;

    for (i = 1; i < 200; i++)
        if ((m_iLevelExpTable[i] <= iExp) && (m_iLevelExpTable[i + 1] > iExp)) return i;

    return 0;
}

int CGame::iCreateFish(char cMapIndex, short sX, short sY, short sType, CItem * pItem, int iDifficulty, DWORD dwLastTime)
{
    int i, iDynamicHandle;

    if ((cMapIndex < 0) || (cMapIndex >= DEF_MAXMAPS)) return NULL;
    if (m_pMapList[cMapIndex] == NULL) return NULL;
    if (m_pMapList[cMapIndex]->bGetIsWater(sX, sY) == FALSE) return NULL;

    for (i = 1; i < DEF_MAXFISHS; i++)
        if (m_pFish[i] == NULL)
        {
            m_pFish[i] = new CFish(cMapIndex, sX, sY, sType, pItem, iDifficulty);
            if (m_pFish[i] == NULL) return NULL;

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
                iDynamicHandle = iAddDynamicObjectList(i, NULL, DEF_DYNAMICOBJECT_FISH, cMapIndex, sX, sY, dwLastTime);
                break;
            default:
                iDynamicHandle = iAddDynamicObjectList(i, NULL, DEF_DYNAMICOBJECT_FISHOBJECT, cMapIndex, sX, sY, dwLastTime);
                break;
            }

            if (iDynamicHandle == NULL)
            {
                delete m_pFish[i];
                m_pFish[i] = NULL;
                return NULL;
            }
            m_pFish[i]->m_sDynamicObjectHandle = iDynamicHandle;
            m_pMapList[cMapIndex]->m_iCurFish++;

            return i;
        }

    return NULL;
}

BOOL CGame::bDeleteFish(int iHandle, int iDelMode)
{
    int i, iH;
    DWORD dwTime;

    if (m_pFish[iHandle] == NULL) return FALSE;

    dwTime = timeGetTime();

    iH = m_pFish[iHandle]->m_sDynamicObjectHandle;

    if (m_pDynamicObjectList[iH] != NULL)
    {
        SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iH]->m_cMapIndex, m_pDynamicObjectList[iH]->m_sX, m_pDynamicObjectList[iH]->m_sY, m_pDynamicObjectList[iH]->m_sType, iH, NULL);
        m_pMapList[m_pDynamicObjectList[iH]->m_cMapIndex]->SetDynamicObject(NULL, NULL, m_pDynamicObjectList[iH]->m_sX, m_pDynamicObjectList[iH]->m_sY, dwTime);
        m_pMapList[m_pDynamicObjectList[iH]->m_cMapIndex]->m_iCurFish--;

        delete m_pDynamicObjectList[iH];
        m_pDynamicObjectList[iH] = NULL;
    }

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE) &&
            (m_pClientList[i]->m_iAllocatedFish == iHandle))
        {
            SendNotifyMsg(NULL, i, DEF_NOTIFY_FISHCANCELED, iDelMode, NULL, NULL, NULL);
            ClearSkillUsingStatus(i);
        }
    }

    delete m_pFish[iHandle];
    m_pFish[iHandle] = NULL;

    return TRUE;
}

int CGame::iCheckFish(int iClientH, char cMapIndex, short dX, short dY)
{
    int i;
    short sDistX, sDistY;

    if (m_pClientList[iClientH] == NULL) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

    if ((cMapIndex < 0) || (cMapIndex >= DEF_MAXMAPS)) return 0;

    for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)
        if (m_pDynamicObjectList[i] != NULL)
        {
            sDistX = abs(m_pDynamicObjectList[i]->m_sX - dX);
            sDistY = abs(m_pDynamicObjectList[i]->m_sY - dY);

            if ((m_pDynamicObjectList[i]->m_cMapIndex == cMapIndex) &&
                ((m_pDynamicObjectList[i]->m_sType == DEF_DYNAMICOBJECT_FISH) || (m_pDynamicObjectList[i]->m_sType == DEF_DYNAMICOBJECT_FISHOBJECT)) &&
                (sDistX <= 2) && (sDistY <= 2))
            {
                if (m_pFish[m_pDynamicObjectList[i]->m_sOwner] == NULL) return 0;
                if (m_pFish[m_pDynamicObjectList[i]->m_sOwner]->m_sEngagingCount >= DEF_MAXENGAGINGFISH) return 0;

                if (m_pClientList[iClientH]->m_iAllocatedFish != NULL) return 0;
                if (m_pClientList[iClientH]->m_cMapIndex != cMapIndex) return 0;
                m_pClientList[iClientH]->m_iAllocatedFish = m_pDynamicObjectList[i]->m_sOwner;
                m_pClientList[iClientH]->m_iFishChance = 1;
                m_pClientList[iClientH]->m_bSkillUsingStatus[1] = TRUE;

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
        if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE) &&
            (m_pClientList[i]->m_iAllocatedFish != NULL))
        {

            if (m_pFish[m_pClientList[i]->m_iAllocatedFish] == NULL) break;

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

                SendNotifyMsg(NULL, i, DEF_NOTIFY_FISHCHANCE, m_pClientList[i]->m_iFishChance, NULL, NULL, NULL);
            }
            else if (iSkillLevel < iResult)
            {
                m_pClientList[i]->m_iFishChance -= iChangeValue;
                if (m_pClientList[i]->m_iFishChance < 1) m_pClientList[i]->m_iFishChance = 1;

                SendNotifyMsg(NULL, i, DEF_NOTIFY_FISHCHANCE, m_pClientList[i]->m_iFishChance, NULL, NULL, NULL);
            }
        }
    }
}

void CGame::FishGenerator()
{
    int i, iP, tX, tY, iRet;
    char  cItemName[30];
    short sDifficulty;
    DWORD dwLastTime;
    CItem * pItem;

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        if ((iDice(1, 10) == 5) && (m_pMapList[i] != NULL) &&
            (m_pMapList[i]->m_iCurFish < m_pMapList[i]->m_iMaxFish))
        {

            iP = iDice(1, m_pMapList[i]->m_iTotalFishPoint) - 1;
            if ((m_pMapList[i]->m_FishPointList[iP].x == -1) || (m_pMapList[i]->m_FishPointList[iP].y == -1)) break;

            tX = m_pMapList[i]->m_FishPointList[iP].x + (iDice(1, 3) - 2);
            tY = m_pMapList[i]->m_FishPointList[iP].y + (iDice(1, 3) - 2);

            pItem = new CItem;
            if (pItem == NULL) break;

            ZeroMemory(cItemName, sizeof(cItemName));
            switch (iDice(1, 9))
            {
            case 1:   strcpy(cItemName, "RedCarp"); sDifficulty = iDice(1, 10) + 20; break;
            case 2:   strcpy(cItemName, "GreenCarp"); sDifficulty = iDice(1, 5) + 10; break;
            case 3:   strcpy(cItemName, "GoldCarp"); sDifficulty = iDice(1, 10) + 1;  break;
            case 4:   strcpy(cItemName, "CrucianCarp"); sDifficulty = 1;  break;
            case 5:   strcpy(cItemName, "BlueSeaBream"); sDifficulty = iDice(1, 15) + 1;  break;
            case 6:   strcpy(cItemName, "RedSeaBream"); sDifficulty = iDice(1, 18) + 1;  break;
            case 7:   strcpy(cItemName, "Salmon"); sDifficulty = iDice(1, 12) + 1;  break;
            case 8:   strcpy(cItemName, "GrayMullet"); sDifficulty = iDice(1, 10) + 1;  break;
            case 9:
                switch (iDice(1, 150))
                {
                case 1:
                case 2:
                case 3:
                    strcpy(cItemName, "PowerGreenPotion");
                    sDifficulty = iDice(5, 4) + 30;
                    break;

                case 10:
                case 11:
                    strcpy(cItemName, "SuperPowerGreenPotion");
                    sDifficulty = iDice(5, 4) + 50;
                    break;

                case 20:
                    strcpy(cItemName, "Dagger+2");
                    sDifficulty = iDice(5, 4) + 30;
                    break;

                case 30:
                    strcpy(cItemName, "LongSword+2");
                    sDifficulty = iDice(5, 4) + 40;
                    break;

                case 40:
                    strcpy(cItemName, "Scimitar+2");
                    sDifficulty = iDice(5, 4) + 50;
                    break;

                case 50:
                    strcpy(cItemName, "Rapier+2");
                    sDifficulty = iDice(5, 4) + 60;
                    break;

                case 60:
                    strcpy(cItemName, "Flameberge+2");
                    sDifficulty = iDice(5, 4) + 60;
                    break;

                case 70:
                    strcpy(cItemName, "WarAxe+2");
                    sDifficulty = iDice(5, 4) + 50;
                    break;

                case 90:
                    strcpy(cItemName, "Ruby");
                    sDifficulty = iDice(5, 4) + 40;
                    break;

                case 95:
                    strcpy(cItemName, "Diamond");
                    sDifficulty = iDice(5, 4) + 40;
                    break;
                }
                break;
            }
            dwLastTime = (60000 * 10) + (iDice(1, 3) - 1) * (60000 * 10);

            if (_bInitItemAttr(pItem, cItemName) == TRUE)
            {
                iRet = iCreateFish(i, tX, tY, 1, pItem, sDifficulty, dwLastTime);
            }
            else
            {
                delete pItem;
                pItem = NULL;
            }
        }
    }
}

int CGame::_iCalcPlayerNum(char cMapIndex, short dX, short dY, char cRadius)
{
    int ix, iy, iRet;
    CTile * pTile;

    if ((cMapIndex < 0) || (cMapIndex > DEF_MAXMAPS)) return 0;
    if (m_pMapList[cMapIndex] == NULL) return 0;

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
                if ((pTile->m_sOwner != NULL) && (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER))
                    iRet++;
            }
        }

    return iRet;
}

void CGame::WhetherProcessor()
{
    char cPrevMode;
    int i, j;
    DWORD dwTime;

    dwTime = timeGetTime();

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        if ((m_pMapList[i] != NULL) && (m_pMapList[i]->m_bIsFixedDayMode == FALSE))
        {
            cPrevMode = m_pMapList[i]->m_cWhetherStatus;
            if (m_pMapList[i]->m_cWhetherStatus != NULL)
            {
                if ((dwTime - m_pMapList[i]->m_dwWhetherStartTime) > m_pMapList[i]->m_dwWhetherLastTime)
                    m_pMapList[i]->m_cWhetherStatus = NULL;
            }
            else
            {
                if (iDice(1, 300) == 13)
                {
                    m_pMapList[i]->m_cWhetherStatus = iDice(1, 3);
                    m_pMapList[i]->m_dwWhetherStartTime = dwTime;
                    m_pMapList[i]->m_dwWhetherLastTime = 60000 * 3 + 60000 * iDice(1, 7);
                }
            }

            if (m_pMapList[i]->m_bIsSnowEnabled == TRUE)
            {
                m_pMapList[i]->m_cWhetherStatus = iDice(1, 3) + 3;
                m_pMapList[i]->m_dwWhetherStartTime = dwTime;
                m_pMapList[i]->m_dwWhetherLastTime = 60000 * 3 + 60000 * iDice(1, 7);
            }

            if (cPrevMode != m_pMapList[i]->m_cWhetherStatus)
            {
                for (j = 1; j < DEF_MAXCLIENTS; j++)
                    if ((m_pClientList[j] != NULL) && (m_pClientList[j]->m_bIsInitComplete == TRUE) && (m_pClientList[j]->m_cMapIndex == i))
                        SendNotifyMsg(NULL, j, DEF_NOTIFY_WHETHERCHANGE, m_pMapList[i]->m_cWhetherStatus, NULL, NULL, NULL);
            }
        }
    }
}

void CGame::FightzoneReserveProcessor()
{
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
            iWheatherBonus = 1;
            break;

        case 20:
        case 30:
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

    if (m_pClientList[iClientH] == NULL) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

    if (m_pClientList[iOpponentH] == NULL) return 0;
    if (m_pClientList[iOpponentH]->m_bIsInitComplete == FALSE) return 0;

    iRet = 0;

    if (m_pClientList[iClientH]->m_iPKCount != 0)
    {
        if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iOpponentH]->m_cLocation, 10) == 0) &&
            (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) && (memcmp(m_pClientList[iOpponentH]->m_cLocation, "NONE", 4) != 0))
        {
            iRet = 7;
        }
        else iRet = 2; 
    }
    else if (m_pClientList[iOpponentH]->m_iPKCount != 0)
    {
        if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iOpponentH]->m_cLocation, 10) == 0) &&
            (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0))
            iRet = 6;
        else iRet = 2;
    }
    else
    {
        if (m_pClientList[iClientH]->m_cSide != m_pClientList[iOpponentH]->m_cSide)
        {
            if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[iOpponentH]->m_cSide != 0))
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
                if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iOpponentH]->m_cLocation, 10) == 0) &&
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

int CGame::iGetPlayerABSStatus(int iWhatH, int iRecvH)
{
    int iRet;

    if (m_pClientList[iWhatH] == NULL) return 0;
    if (m_pClientList[iRecvH] == NULL) return 0;

    iRet = 0;

    /*if (m_pMapList[m_pClientList[iRecvH]->m_cMapIndex]->m_bIsFightZone == TRUE &&
        m_pClientList[iWhatH]->m_iGuildRank != -1 && m_pClientList[iRecvH]->m_iGuildRank != -1 &&
        memcmp(m_pClientList[iWhatH]->m_cGuildName,m_pClientList[iRecvH]->m_cGuildName,20) != 0) {
            iRet = 8;
        }*/

    if (m_pClientList[iWhatH]->m_iPKCount != 0)
    {
        iRet = 8;
    }

    if (m_pClientList[iWhatH]->m_cSide != 0)
    {
        iRet = iRet | 4;
    }

    if (m_pClientList[iWhatH]->m_cSide == 1)
    {
        iRet = iRet | 2;
    }

    if (m_pClientList[iWhatH]->m_bIsPlayerCivil == TRUE)
        iRet = iRet | 1;

    return iRet;
}

int CGame::iGetNpcRelationship(int iWhatH, int iRecvH)
{
    int iRet;

    if (m_pClientList[iRecvH] == NULL) return 0;
    if (m_pNpcList[iWhatH] == NULL) return 0;

    iRet = 0;
    switch (m_pNpcList[iWhatH]->m_cSide)
    {
        case 10: iRet |= 8;
        case 1:  iRet = (iRet | 4) | 2;
        case 2:  iRet |= 4;
    }

    return iRet;
}

int CGame::iGetNpcRelationship_SendEvent(int iNpcH, int iOpponentH)
{
    int iRet;

    if (m_pClientList[iOpponentH] == NULL) return 0;
    if (m_pClientList[iOpponentH]->m_bIsInitComplete == FALSE) return 0;

    if (m_pNpcList[iNpcH] == NULL) return 0;

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

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pMapName);

    iMapIndex = -1;
    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL)
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
        if (m_pClientList[i] != NULL)
        {
            if (m_pClientList[i]->m_bIsInitComplete == TRUE)
                DeleteClient(i, TRUE, TRUE);
            else DeleteClient(i, FALSE, FALSE);
            iCnt++;
            if (iCnt >= iNum) break;
        }

    return iCnt;
}

void CGame::NpcRequestAssistance(int iNpcH)
{
    int ix, iy, sX, sY;
    short sOwnerH;
    char  cOwnerType;

    if (m_pNpcList[iNpcH] == NULL) return;

    sX = m_pNpcList[iNpcH]->m_sX;
    sY = m_pNpcList[iNpcH]->m_sY;

    for (ix = sX - 8; ix <= sX + 8; ix++)
        for (iy = sY - 8; iy <= sY + 8; iy++)
        {
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
            if ((sOwnerH != NULL) && (m_pNpcList[sOwnerH] != NULL) && (cOwnerType == DEF_OWNERTYPE_NPC) &&
                (iNpcH != sOwnerH) && (m_pNpcList[sOwnerH]->m_cSide == m_pNpcList[iNpcH]->m_cSide) &&
                (m_pNpcList[sOwnerH]->m_bIsPermAttackMode == FALSE) && (m_pNpcList[sOwnerH]->m_cBehavior == DEF_BEHAVIOR_MOVE))
            {
                m_pNpcList[sOwnerH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                m_pNpcList[sOwnerH]->m_sBehaviorTurnCount = 0;
                m_pNpcList[sOwnerH]->m_iTargetIndex = m_pNpcList[iNpcH]->m_iTargetIndex;
                m_pNpcList[sOwnerH]->m_cTargetType = m_pNpcList[iNpcH]->m_cTargetType;

                return;
            }
        }
}

void CGame::ForceDisconnectAccount(char * pAccountName, WORD wCount)
{
    int i;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cAccountName, pAccountName, 10) == 0))
        {
            wsprintf(G_cTxt, "<%d> Force disconnect account: CharName(%s) AccntName(%s) Count(%d)", i, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cAccountName, wCount);
            log->info(G_cTxt);

            //DeleteClient(i, TRUE, TRUE);

            SendNotifyMsg(NULL, i, DEF_NOTIFY_FORCEDISCONN, wCount, NULL, NULL, NULL);
        }
}

BOOL CGame::bOnClose()
{
    if (m_bIsServerShutdowned == FALSE)
        if (MessageBox(NULL, "Player data not saved! Shutdown server now?", m_cServerName, MB_ICONEXCLAMATION | MB_YESNO) == IDYES) return TRUE;
        else return FALSE;
    else return TRUE;

    return FALSE;
}

void CGame::_CheckAttackType(int iClientH, short * spType)
{
    WORD wType;

    if (m_pClientList[iClientH] == NULL) return;
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

void CGame::LocalSavePlayerData(int iClientH)
{
    char * pData, * cp, cFn[256], cDir[256], cTxt[256], cCharDir[256];
    int    iSize;
    FILE * pFile;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == NULL) return;

    pData = new char[30000];
    if (pData == NULL) return;
    ZeroMemory(pData, 30000);

    cp = (char *)(pData);
    iSize = _iComposePlayerDataFileContents(iClientH, cp);

    GetLocalTime(&SysTime);
    ZeroMemory(cCharDir, sizeof(cDir));
    wsprintf(cCharDir, "Character_%d_%d_%d_%d", SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);

    ZeroMemory(cDir, sizeof(cDir));
    ZeroMemory(cFn, sizeof(cFn));
    strcat(cFn, cCharDir);
    strcat(cFn, "\\");
    strcat(cFn, "\\");
    wsprintf(cTxt, "MeC77%d", (unsigned char)m_pClientList[iClientH]->m_cCharName[0]);
    strcat(cFn, cTxt);
    strcpy(cDir, cFn);
    strcat(cFn, "\\");
    strcat(cFn, "\\");
    strcat(cFn, m_pClientList[iClientH]->m_cCharName);
    strcat(cFn, ".txt");

    _mkdir(cCharDir);
    _mkdir(cDir);

    if (iSize == 0)
    {
        log->info("(!) Character data body empty: Cannot create & save temporal player data file.");
        delete pData;
        return;
    }

    pFile = fopen(cFn, "wt");
    if (pFile == NULL)
    {
        wsprintf(cTxt, "(!) Cannot create temporal player data file : Name(%s)", cFn);
        log->info(cTxt);
    }
    else
    {
        wsprintf(cTxt, "(!) temporal player data file saved : Name(%s)", cFn);
        log->info(cTxt);
        fwrite(cp, iSize, 1, pFile);
    }

    if (pFile != NULL) fclose(pFile);
    delete pData;
}

void CGame::MineralGenerator()
{
    int i, iP, tX, tY, iRet;

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        if ((iDice(1, 4) == 1) && (m_pMapList[i] != NULL) &&
            (m_pMapList[i]->m_bMineralGenerator == TRUE) &&
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

    if ((cMapIndex < 0) || (cMapIndex >= DEF_MAXMAPS)) return NULL;
    if (m_pMapList[cMapIndex] == NULL) return NULL;

    for (i = 1; i < DEF_MAXMINERALS; i++)
        if (m_pMineral[i] == NULL)
        {
            iMineralType = iDice(1, cLevel);
            m_pMineral[i] = new CMineral(iMineralType, cMapIndex, tX, tY, 1);
            if (m_pMineral[i] == NULL) return NULL;

            iDynamicHandle = NULL;
            switch (iMineralType)
            {
            case 1:
            case 2:
            case 3:
            case 4:
                iDynamicHandle = iAddDynamicObjectList(NULL, NULL, DEF_DYNAMICOBJECT_MINERAL1, cMapIndex, tX, tY, NULL, i);
                break;

            case 5:
            case 6:
                iDynamicHandle = iAddDynamicObjectList(NULL, NULL, DEF_DYNAMICOBJECT_MINERAL2, cMapIndex, tX, tY, NULL, i);
                break;

            default:
                iDynamicHandle = iAddDynamicObjectList(NULL, NULL, DEF_DYNAMICOBJECT_MINERAL1, cMapIndex, tX, tY, NULL, i);
                break;
            }

            if (iDynamicHandle == NULL)
            {
                delete m_pMineral[i];
                m_pMineral[i] = NULL;
                return NULL;
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

    return NULL;
}

void CGame::_CheckMiningAction(int iClientH, int dX, int dY)
{
    short sType;
    DWORD dwRegisterTime;
    int   iDynamicIndex, iSkillLevel, iResult, iItemID;
    CItem * pItem;
    WORD  wWeaponType;

    iItemID = 0;

    if (m_pClientList[iClientH] == NULL)  return;

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetDynamicObject(dX, dY, &sType, &dwRegisterTime, &iDynamicIndex);

    if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
    {
        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
    }

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

        if (m_pDynamicObjectList[iDynamicIndex] == NULL) break;
        iSkillLevel -= m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_iDifficulty;
        if (iSkillLevel <= 0) iSkillLevel = 1;

        iResult = iDice(1, 100);
        if (iResult <= iSkillLevel)
        {
            CalculateSSN_SkillIndex(iClientH, 0, 1);

            switch (m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_cType)
            {
            case 1:
                switch (iDice(1, 5))
                {
                case 1:
                case 2:
                case 3:
                    iItemID = 355; // Coal
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 4:
                    iItemID = 357; // IronOre
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 5:
                    iItemID = 507; // BlondeStone
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                }
                break;

            case 2:
                switch (iDice(1, 5))
                {
                case 1:
                case 2:
                    iItemID = 355; // Coal
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 3:
                case 4:
                    iItemID = 357; // IronOre
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 5:
                    if (iDice(1, 3) == 2)
                    {
                        iItemID = 356; // SilverNugget
                        m_pClientList[iClientH]->m_iExpStock += iDice(1, 4);
                    }
                    else
                    {
                        iItemID = 507; // BlondeStone
                        m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    }
                    break;
                }
                break;

            case 3:
                switch (iDice(1, 6))
                {
                case 1:
                    iItemID = 355; // Coal
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                    iItemID = 357; // IronOre
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 6:
                    if (iDice(1, 8) == 3)
                    {
                        if (iDice(1, 2) == 1)
                        {
                            iItemID = 356; // SilverNugget
                            m_pClientList[iClientH]->m_iExpStock += iDice(1, 4);
                        }
                        else
                        {
                            iItemID = 357; // IronOre
                            m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                        }
                        break;
                    }
                    else
                    {
                        iItemID = 357; // IronOre
                        m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    }
                    break;
                }
                break;

            case 4:
                switch (iDice(1, 6))
                {
                case 1:
                    iItemID = 355; // Coal
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 2:
                    if (iDice(1, 3) == 2)
                    {
                        iItemID = 356; // SilverNugget
                        m_pClientList[iClientH]->m_iExpStock += iDice(1, 4);
                    }
                    break;
                case 3:
                case 4:
                case 5:
                    iItemID = 357; // IronOre
                    m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                    break;
                case 6:
                    if (iDice(1, 8) == 3)
                    {
                        if (iDice(1, 4) == 3)
                        {
                            if (iDice(1, 4) < 3)
                            {
                                iItemID = 508; // Mithral
                                m_pClientList[iClientH]->m_iExpStock += iDice(1, 15);
                            }
                            else
                            {
                                iItemID = 354; // GoldNugget
                                m_pClientList[iClientH]->m_iExpStock += iDice(1, 5);
                            }
                            break;
                        }
                        else
                        {
                            iItemID = 356; // SilverNugget
                            m_pClientList[iClientH]->m_iExpStock += iDice(1, 4);
                        }
                        break;
                    }
                    else
                    {
                        if (iDice(1, 2) == 1)
                        {
                            iItemID = 354; // GoldNugget
                            m_pClientList[iClientH]->m_iExpStock += iDice(1, 5);
                        }
                        else
                        {
                            iItemID = 357;  // IronOre
                            m_pClientList[iClientH]->m_iExpStock += iDice(1, 3);
                        }
                        break;
                    }
                    break;
                }
                break;

            case 5:
                switch (iDice(1, 19))
                {
                case 3:
                    iItemID = 352; // Sapphire
                    m_pClientList[iClientH]->m_iExpStock += iDice(2, 3);
                    break;
                default:
                    iItemID = 358; // Crystal
                    m_pClientList[iClientH]->m_iExpStock += iDice(2, 3);
                    break;
                }
                break;

            case 6:
                switch (iDice(1, 5))
                {
                case 1:
                    if (iDice(1, 6) == 3)
                    {
                        iItemID = 353; // Emerald 
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 4);
                    }
                    else
                    {
                        iItemID = 358; // Crystal
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 3);
                    }
                    break;
                case 2:
                    if (iDice(1, 6) == 3)
                    {
                        iItemID = 352; // Saphire
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 4);
                    }
                    else
                    {
                        iItemID = 358; // Crystal
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 3);
                    }
                    break;
                case 3:
                    if (iDice(1, 6) == 3)
                    {
                        iItemID = 351; // Ruby
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 4);
                    }
                    else
                    {
                        iItemID = 358; // Crystal 
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 3);
                    }
                    break;
                case 4:
                    iItemID = 358; // Crystal 
                    m_pClientList[iClientH]->m_iExpStock += iDice(2, 3);
                    break;
                case 5:
                    if (iDice(1, 12) == 3)
                    {
                        iItemID = 350; // Diamond
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 5);
                    }
                    else
                    {
                        iItemID = 358; // Crystal
                        m_pClientList[iClientH]->m_iExpStock += iDice(2, 3);
                    }
                    break;
                }
                break;

            }

            pItem = new CItem;
            if (_bInitItemAttr(pItem, iItemID) == FALSE)
            {
                delete pItem;
            }
            else
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                    m_pClientList[iClientH]->m_sY, pItem);
                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                    m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                    pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
            }

            m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_iRemain--;
            if (m_pMineral[m_pDynamicObjectList[iDynamicIndex]->m_iV1]->m_iRemain <= 0)
            {
                bDeleteMineral(m_pDynamicObjectList[iDynamicIndex]->m_iV1);

                delete m_pDynamicObjectList[iDynamicIndex];
                m_pDynamicObjectList[iDynamicIndex] = NULL;
            }
        }
        break;

    default:
        break;
    }
}

BOOL CGame::bDeleteMineral(int iIndex)
{
    int iDynamicIndex;
    DWORD dwTime;

    dwTime = timeGetTime();

    if (m_pMineral[iIndex] == NULL) return FALSE;
    iDynamicIndex = m_pMineral[iIndex]->m_sDynamicObjectHandle;
    if (m_pDynamicObjectList[iDynamicIndex] == NULL) return FALSE;

    SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iDynamicIndex]->m_cMapIndex,
        m_pDynamicObjectList[iDynamicIndex]->m_sX, m_pDynamicObjectList[iDynamicIndex]->m_sY,
        m_pDynamicObjectList[iDynamicIndex]->m_sType, iDynamicIndex, NULL);
    m_pMapList[m_pDynamicObjectList[iDynamicIndex]->m_cMapIndex]->SetDynamicObject(NULL, NULL, m_pDynamicObjectList[iDynamicIndex]->m_sX, m_pDynamicObjectList[iDynamicIndex]->m_sY, dwTime);
    m_pMapList[m_pMineral[iIndex]->m_cMapIndex]->SetTempMoveAllowedFlag(m_pDynamicObjectList[iDynamicIndex]->m_sX, m_pDynamicObjectList[iDynamicIndex]->m_sY, TRUE);

    m_pMapList[m_pMineral[iIndex]->m_cMapIndex]->m_iCurMineral--;

    delete m_pMineral[iIndex];
    m_pMineral[iIndex] = NULL;

    return TRUE;
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
                if (pItem != NULL) delete pItem;
                iAddDynamicObjectList(NULL, NULL, DEF_DYNAMICOBJECT_FIRE, cMapIndex, ix, iy, 6000);

                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, cMapIndex,
                    ix, iy, sSpr, sSprFrame, cItemColor);
                break;
            }
        }
}

int CGame::_iGetWeaponSkillType(int iClientH)
{
    WORD wWeaponType;

    if (m_pClientList[iClientH] == NULL) return 0;

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

BOOL CGame::_bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID)
{
    BOOL bRet;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return FALSE;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return FALSE;

    switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
    {
    case DEF_ITEMEFFECTTYPE_OCCUPYFLAG:
        bRet = __bSetOccupyFlag(m_pClientList[iClientH]->m_cMapIndex, dX, dY,
            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,
            NULL, iClientH, NULL);
        if (bRet == TRUE)
        {
            GetExp(iClientH, (iDice(m_pClientList[iClientH]->m_iLevel, 10)));
        }
        else
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTFLAGSPOT, NULL, NULL, NULL, NULL);
        }
        return bRet;

    case DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT:
        bRet = __bSetConstructionKit(m_pClientList[iClientH]->m_cMapIndex, dX, dY,
            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,
            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,
            iClientH);
        if (bRet == TRUE)
        {
        }
        else
        {
        }
        return bRet;

    case DEF_ITEMEFFECTTYPE_DYE:
        if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS))
        {
            if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != NULL)
            {
                if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 11) ||
                    (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 12))
                {
                    m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, NULL, NULL);
                    return TRUE;
                }
                else
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, NULL, NULL);
                    return FALSE;
                }
            }
        }
        break;

    case DEF_ITEMEFFECTTYPE_ARMORDYE:
        if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS))
        {
            if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != NULL)
            {
                if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 6) ||
                    (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 15) ||
                    (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 13))
                {
                    m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, NULL, NULL);
                    return TRUE;
                }
                else
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, NULL, NULL);
                    return FALSE;
                }
            }
        }
        break;

    case DEF_ITEMEFFECTTYPE_WEAPONDYE:
        if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS))
        {
            if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != NULL)
            {
                if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 1) ||
                    (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 3) ||
                    (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 8))
                {
                    m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, NULL, NULL);
                    return TRUE;
                }
                else
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, NULL, NULL);
                    return FALSE;
                }
            }
        }
        break;

    case DEF_ITEMEFFECTTYPE_FARMING:
        bRet = bPlantSeedBag(m_pClientList[iClientH]->m_cMapIndex, dX, dY,
            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,
            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,
            iClientH);
        return bRet;

    default:
        break;
    }

    return TRUE;
}

BOOL CGame::__bSetOccupyFlag(char cMapIndex, int dX, int dY, int iSide, int iEKNum, int iClientH, BOOL bAdminFlag)
{
    int ix, iy;
    int   iDynamicObjectIndex, iIndex;
    CTile * pTile;
    DWORD dwTime;

    dwTime = timeGetTime();

    if (m_pMapList[cMapIndex] == NULL) return FALSE;
    if (((m_bIsHeldenianMode == FALSE) || (m_bIsHeldenianMode != m_cHeldenianType)) && (m_bHeldenianInitiated == 1)) return FALSE;
    if ((m_cHeldenianType == 1) && (m_iBTFieldMapIndex == -1)) return FALSE;
    if ((m_cHeldenianType == 2) && (m_iGodHMapIndex == -1)) return FALSE;
    if ((m_pClientList[iClientH]->m_iGuildRank == 0)) return FALSE;

    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + dX + dY * m_pMapList[cMapIndex]->m_sSizeY);
    if (pTile->m_iAttribute != NULL) return FALSE;
    iSide = m_sLastHeldenianWinner;
    if ((dX < 25) || (dX >= m_pMapList[cMapIndex]->m_sSizeX - 25) ||
        (dY < 25) || (dY >= m_pMapList[cMapIndex]->m_sSizeY - 25)) return FALSE;

    if ((iClientH > 0) && (m_pClientList[iClientH] != NULL))
    {
        if ((bAdminFlag == FALSE) && (m_pClientList[iClientH]->m_cSide != iSide)) return FALSE;
    }

    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + dX + dY * m_pMapList[cMapIndex]->m_sSizeY);
    if (pTile->m_iOccupyFlagIndex != NULL) return FALSE;
    if (pTile->m_bIsMoveAllowed == FALSE)  return FALSE;

    for (ix = dX - 3; ix <= dX + 3; ix++)
        for (iy = dY - 3; iy <= dY + 3; iy++)
        {
            if ((ix == dX) && (iy == dY))
            {

            }
            else
            {
                pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy * m_pMapList[cMapIndex]->m_sSizeY);
                if ((pTile->m_iOccupyFlagIndex != NULL) && (pTile->m_iOccupyFlagIndex > 0) &&
                    (pTile->m_iOccupyFlagIndex < DEF_MAXOCCUPYFLAG) && (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex] != NULL))
                {
                    if (m_pMapList[cMapIndex]->m_pOccupyFlag[pTile->m_iOccupyFlagIndex]->m_cSide == iSide) return FALSE;
                }
            }
        }

    if (m_pMapList[cMapIndex]->m_iTotalOccupyFlags >= DEF_MAXOCCUPYFLAG)
    {
        return FALSE;
    }

    switch (iSide)
    {
    case 1:	iDynamicObjectIndex = iAddDynamicObjectList(NULL, NULL, DEF_DYNAMICOBJECT_ARESDENFLAG1, cMapIndex, dX, dY, NULL, NULL);	break;
    case 2:	iDynamicObjectIndex = iAddDynamicObjectList(NULL, NULL, DEF_DYNAMICOBJECT_ELVINEFLAG1, cMapIndex, dX, dY, NULL, NULL);	break;
    default: iDynamicObjectIndex = 0;
    }

    iEKNum = 1;
    iIndex = m_pMapList[cMapIndex]->iRegisterOccupyFlag(dX, dY, iSide, iEKNum, iDynamicObjectIndex);
    if (iIndex < 0)
    {
        if (iDynamicObjectIndex > DEF_MAXGUILDS)
            return TRUE;
    }

    pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + dX + dY * m_pMapList[cMapIndex]->m_sSizeY);
    pTile->m_iOccupyFlagIndex = iIndex;

    m_pMapList[cMapIndex]->m_iTotalOccupyFlags++;

    if (m_cHeldenianType == 1)
    {
        for (ix = dX - 3; ix <= dX + 3; ix++)
            for (iy = dY - 3; iy <= dY + 3; iy++)
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
    }

    if (m_cHeldenianType == 2)
    {
        if (iSide == m_sLastHeldenianWinner)
        {
            m_cHeldenianVictoryType = iSide;
        }
    }
    return TRUE;
}

void CGame::SetSummonMobAction(int iClientH, int iMode, DWORD dwMsgSize, char * pData)
{
    int i, iTargetIndex;
    char   seps[] = "= \t\n";
    char * token, cTargetName[11], cBuff[256];
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_cSide == 0) return;

    switch (iMode)
    {
    case 0:
    case 1:
        for (i = 0; i < DEF_MAXNPCS; i++)
            if (m_pNpcList[i] != NULL)
            {
                if ((m_pNpcList[i]->m_bIsSummoned == TRUE) &&
                    (m_pNpcList[i]->m_iFollowOwnerIndex == iClientH) &&
                    (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER))
                {

                    m_pNpcList[i]->m_iSummonControlMode = iMode;
                    m_pNpcList[i]->m_bIsPermAttackMode = FALSE;
                    m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                    m_pNpcList[i]->m_sBehaviorTurnCount = 0;
                    m_pNpcList[i]->m_iTargetIndex = NULL;
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
        if (token != NULL)
        {
            if (strlen(token) > 10)
                memcpy(cTargetName, token, 10);
            else memcpy(cTargetName, token, strlen(token));

            for (i = 1; i < DEF_MAXCLIENTS; i++)
            {
                if ((m_pClientList[i] != NULL) &&
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
                if (m_pNpcList[i] != NULL)
                {
                    if ((m_pNpcList[i]->m_bIsSummoned == TRUE) &&
                        (m_pNpcList[i]->m_iFollowOwnerIndex == iClientH) &&
                        (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER))
                    {

                        m_pNpcList[i]->m_iSummonControlMode = iMode;
                        m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                        m_pNpcList[i]->m_sBehaviorTurnCount = 0;
                        m_pNpcList[i]->m_iTargetIndex = iTargetIndex;
                        m_pNpcList[i]->m_cTargetType = DEF_OWNERTYPE_PLAYER;
                        m_pNpcList[i]->m_bIsPermAttackMode = TRUE;
                    }
                }
        }
        break;
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

    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pMapList[m_iMiddlelandMapIndex]->m_pOccupyFlag[i] != NULL)
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

    if (m_pClientList[iClientH] == NULL) return;

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
        {
            if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER) &&
                (m_pClientList[iClientH]->m_bIsItemEquipped[i] == TRUE))
            {

                if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue1 == m_pClientList[iClientH]->m_sCharIDnum1) &&
                    (m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue2 == m_pClientList[iClientH]->m_sCharIDnum2) &&
                    (m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue3 == m_pClientList[iClientH]->m_sCharIDnum3))
                {
                }
                else
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos, i, NULL, NULL);
                    ReleaseItemHandler(iClientH, i, TRUE);
                    iDamage = iDice(10, 10);
                    m_pClientList[iClientH]->m_iHP -= iDamage;
                    if (m_pClientList[iClientH]->m_iHP <= 0)
                    {
                        ClientKilledHandler(iClientH, NULL, NULL, iDamage);
                    }
                }
            }
        }
}

void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount)
{
    int iExH;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
    if (m_pClientList[iClientH]->iExchangeCount > 4) return;

    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0))
    {
        _ClearExchangeStatus(m_pClientList[iClientH]->m_iExchangeH);
        _ClearExchangeStatus(iClientH);
    }


    if ((m_pClientList[iClientH]->m_bIsExchangeMode == TRUE) && (m_pClientList[iClientH]->m_iExchangeH != NULL))
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;
        if ((m_pClientList[iExH] == NULL) || (memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0))
        {
        }
        else
        {
            if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
            if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return;
            if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount < iAmount) return;

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

            //ZeroMemory(m_pClientList[iClientH]->m_cExchangeItemName, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
            memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->iExchangeCount], m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, 20);

            //m_pClientList[iClientH]->m_cExchangeItemIndex  = iItemIndex;
            //m_pClientList[iClientH]->m_iExchangeItemAmount = iAmount;
            //ZeroMemory(m_pClientList[iClientH]->m_cExchangeItemName, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
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
    int iExH, i;
    int iItemWeightA, iItemWeightB, iWeightLeftA, iWeightLeftB, iAmountLeft;
    CItem * pItemA[4]{}, * pItemB[4]{}, * pItemAcopy[4]{}, * pItemBcopy[4]{};

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;


    if ((m_pClientList[iClientH]->m_bIsExchangeMode == TRUE) && (m_pClientList[iClientH]->m_iExchangeH != NULL))
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;

        if (iClientH == iExH) return;

        if (m_pClientList[iExH] != NULL)
        {
            if ((memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0) ||
                (m_pClientList[iExH]->m_bIsExchangeMode != TRUE) ||
                (memcmp(m_pClientList[iExH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName, 10) != 0))
            {
                _ClearExchangeStatus(iClientH);
                _ClearExchangeStatus(iExH);
                return;
            }
            else
            {
                m_pClientList[iClientH]->m_bIsExchangeConfirm = TRUE;
                if (m_pClientList[iExH]->m_bIsExchangeConfirm == TRUE)
                {
                    //Check all items
                    for (int i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
                    {
                        if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] == NULL) ||
                            (memcmp(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName, m_pClientList[iClientH]->m_cExchangeItemName[i], 20) != 0))
                        {
                            _ClearExchangeStatus(iClientH);
                            _ClearExchangeStatus(iExH);
                            return;
                        }
                    }
                    for (int i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
                    {
                        if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] == NULL) ||
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
                    for (i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
                    {
                        iItemWeightA = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]],
                            m_pClientList[iClientH]->m_iExchangeItemAmount[i]);
                    }
                    iItemWeightB = 0;
                    for (i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
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

                    for (i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
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

                    for (i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
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

                    for (i = 0; i < m_pClientList[iExH]->iExchangeCount; i++)
                    {
                        bAddItem(iClientH, pItemB[i], NULL);
                        _bItemLog(DEF_ITEMLOG_EXCHANGE, iExH, iClientH, pItemBcopy[i]);
                        delete pItemBcopy[i];
                        pItemBcopy[i] = NULL;
                        if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                            (m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW))
                        {
                            iAmountLeft = (int)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iExH]->m_iExchangeItemAmount[i];
                            if (iAmountLeft < 0) iAmountLeft = 0;
                            SetItemCount(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], iAmountLeft);
                            // m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex]->m_cName, iAmountLeft);
                        }
                        else
                        {
                            ReleaseItemHandler(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], TRUE);
                            SendNotifyMsg(NULL, iExH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iExH]->m_cExchangeItemIndex[i], m_pClientList[iExH]->m_iExchangeItemAmount[i], NULL, m_pClientList[iClientH]->m_cCharName);
                            m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] = NULL;
                        }
                    }

                    for (i = 0; i < m_pClientList[iClientH]->iExchangeCount; i++)
                    {
                        bAddItem(iExH, pItemA[i], NULL);
                        _bItemLog(DEF_ITEMLOG_EXCHANGE, iClientH, iExH, pItemAcopy[i]);
                        delete pItemAcopy[i];
                        pItemAcopy[i] = NULL;

                        if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                            (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW))
                        {
                            iAmountLeft = (int)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iClientH]->m_iExchangeItemAmount[i];
                            if (iAmountLeft < 0) iAmountLeft = 0;
                            SetItemCount(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], iAmountLeft);
                            // m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex]->m_cName, iAmountLeft);
                        }
                        else
                        {
                            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], TRUE);
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iClientH]->m_cExchangeItemIndex[i], m_pClientList[iClientH]->m_iExchangeItemAmount[i], NULL, m_pClientList[iExH]->m_cCharName);
                            m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] = NULL;
                        }
                    }

                    m_pClientList[iClientH]->m_bIsExchangeMode = FALSE;
                    m_pClientList[iClientH]->m_bIsExchangeConfirm = FALSE;
                    ZeroMemory(m_pClientList[iClientH]->m_cExchangeName, sizeof(m_pClientList[iClientH]->m_cExchangeName));
                    m_pClientList[iClientH]->m_iExchangeH = NULL;
                    m_pClientList[iClientH]->iExchangeCount = 0;

                    m_pClientList[iExH]->m_bIsExchangeMode = FALSE;
                    m_pClientList[iExH]->m_bIsExchangeConfirm = FALSE;
                    ZeroMemory(m_pClientList[iExH]->m_cExchangeName, sizeof(m_pClientList[iExH]->m_cExchangeName));
                    m_pClientList[iExH]->m_iExchangeH = NULL;
                    m_pClientList[iExH]->iExchangeCount = 0;

                    for (i = 0; i < 4; i++)
                    {
                        m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
                        m_pClientList[iExH]->m_cExchangeItemIndex[i] = -1;
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, NULL, NULL, NULL, NULL);
                    SendNotifyMsg(NULL, iExH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, NULL, NULL, NULL, NULL);

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

int CGame::_iGetItemSpaceLeft(int iClientH)
{
    int i, iTotalItem;

    iTotalItem = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL) iTotalItem++;

    return (DEF_MAXITEMS - iTotalItem);
}

void CGame::ScreenSettingsHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    if (m_pClientList[iClientH] == NULL) return;

    CClient * client = m_pClientList[iClientH];
    stream_read sr(pData, dwMsgSize);

    sr.read<uint32_t>();

    client->screenwidth_v = sr.read<uint16_t>();
    client->screenheight_v = sr.read<uint16_t>();
    client->screenwidth = sr.read<uint16_t>();
    client->screenheight = sr.read<uint16_t>();
    client->screen_size_x = client->screenwidth_v / 32;
    client->screen_size_y = client->screenheight_v / 32;

    log->info("ScreenSettingsHandler: client {} screenwidth_v {} screenheight_v {} screenwidth {} screenheight {} screen_size_x {} screen_size_y {}", iClientH, client->screenwidth_v, client->screenheight_v, client->screenwidth, client->screenheight, client->screen_size_x, client->screen_size_y);
}

BOOL CGame::bAddItem(int iClientH, CItem * pItem, char cMode)
{
    char * cp, cData[256];
    DWORD * dwp;
    WORD * wp;
    short * sp;
    int iRet, iEraseReq;

    ZeroMemory(cData, sizeof(cData));
    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
    {
        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        *cp = 1;
        cp++;

        memcpy(cp, pItem->m_cName, 20);
        cp += 20;

        dwp = (DWORD *)cp;
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

        wp = (WORD *)cp;
        *wp = pItem->m_wCurLifeSpan;
        cp += 2;

        wp = (WORD *)cp;
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

        *cp = (char)pItem->m_sItemSpecEffectValue2;
        cp++;

        dwp = (DWORD *)cp;
        *dwp = pItem->m_dwAttribute;
        cp += 4;
        /*
        *cp = (char)(pItem->m_dwAttribute & 0x00000001);
        cp++;
        */

        if (iEraseReq == 1)
        {
            delete pItem;
            pItem = NULL;
        }

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

        return TRUE;
    }
    else
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY,
            pItem);

        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);

        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);

        return TRUE;
    }

    return FALSE;
}

int CGame::__iSearchForQuest(int iClientH, int iWho, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    int i, iQuestList[DEF_MAXQUESTTYPE]{}, iIndex, iQuest, iReward, iQuestIndex;

    if (m_pClientList[iClientH] == NULL) return -1;

    iIndex = 0;
    for (i = 0; i < DEF_MAXQUESTTYPE; i++)
        iQuestList[i] = -1;

    for (i = 1; i < DEF_MAXQUESTTYPE; i++)
        if (m_pQuestConfigList[i] != NULL)
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

            if ((m_bIsCrusadeMode == TRUE) && (m_pQuestConfigList[i]->m_iAssignType != 1)) goto SFQ_SKIP;
            if ((m_bIsCrusadeMode == FALSE) && (m_pQuestConfigList[i]->m_iAssignType == 1)) goto SFQ_SKIP;

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
    char cTargetName[30];

    if (m_pClientList[iClientH] == NULL) return;

    iIndex = m_pClientList[iClientH]->m_iQuest;
    if (iIndex == NULL)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCONTENTS, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }
    else
    {
        iWho = m_pQuestConfigList[iIndex]->m_iFrom;
        iQuestType = m_pQuestConfigList[iIndex]->m_iType;
        iContribution = m_pQuestConfigList[iIndex]->m_iContribution;
        iTargetType = m_pQuestConfigList[iIndex]->m_iTargetType;
        iTargetCount = m_pQuestConfigList[iIndex]->m_iMaxCount;
        iX = m_pQuestConfigList[iIndex]->m_sX;
        iY = m_pQuestConfigList[iIndex]->m_sY;
        iRange = m_pQuestConfigList[iIndex]->m_iRange;
        ZeroMemory(cTargetName, sizeof(cTargetName));
        memcpy(cTargetName, m_pQuestConfigList[iIndex]->m_cTargetName, 20);
        iQuestCompleted = (int)m_pClientList[iClientH]->m_bIsQuestCompleted;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCONTENTS, iWho, iQuestType, iContribution, NULL,
            iTargetType, iTargetCount, iX, iY, iRange, iQuestCompleted, cTargetName);
    }
}

void CGame::_CheckQuestEnvironment(int iClientH)
{
    int iIndex;
    char cTargetName[30];

    if (m_pClientList[iClientH] == NULL) return;

    iIndex = m_pClientList[iClientH]->m_iQuest;
    if (iIndex == NULL) return;
    if (m_pQuestConfigList[iIndex] == NULL) return;

    if (iIndex >= 35 && iIndex <= 40)
    {
        m_pClientList[iClientH]->m_iQuest = 0;
        m_pClientList[iClientH]->m_iQuestID = NULL;
        m_pClientList[iClientH]->m_iQuestRewardAmount = NULL;
        m_pClientList[iClientH]->m_iQuestRewardType = NULL;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTABORTED, NULL, NULL, NULL, NULL);
        return;
    }

    if (m_pQuestConfigList[iIndex]->m_iQuestID != m_pClientList[iClientH]->m_iQuestID)
    {
        m_pClientList[iClientH]->m_iQuest = NULL;
        m_pClientList[iClientH]->m_iQuestID = NULL;
        m_pClientList[iClientH]->m_iQuestRewardAmount = NULL;
        m_pClientList[iClientH]->m_iQuestRewardType = NULL;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTABORTED, NULL, NULL, NULL, NULL);
        return;
    }

    switch (m_pQuestConfigList[iIndex]->m_iType)
    {
    case DEF_QUESTTYPE_MONSTERHUNT:
    case DEF_QUESTTYPE_GOPLACE:
        ZeroMemory(cTargetName, sizeof(cTargetName));
        memcpy(cTargetName, m_pQuestConfigList[iIndex]->m_cTargetName, 20);
        if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, cTargetName, 10) == 0)
            m_pClientList[iClientH]->m_bQuestMatchFlag_Loc = TRUE;
        else m_pClientList[iClientH]->m_bQuestMatchFlag_Loc = FALSE;
        break;
    }

}

BOOL CGame::_bCheckIsQuestCompleted(int iClientH)
{
    int iQuestIndex;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_bIsQuestCompleted == TRUE) return FALSE;
    iQuestIndex = m_pClientList[iClientH]->m_iQuest;
    if (iQuestIndex == NULL) return FALSE;

    if (m_pQuestConfigList[iQuestIndex] != NULL)
    {
        switch (m_pQuestConfigList[iQuestIndex]->m_iType)
        {
        case DEF_QUESTTYPE_MONSTERHUNT:
            if ((m_pClientList[iClientH]->m_bQuestMatchFlag_Loc == TRUE) &&
                (m_pClientList[iClientH]->m_iCurQuestCount >= m_pQuestConfigList[iQuestIndex]->m_iMaxCount))
            {
                m_pClientList[iClientH]->m_bIsQuestCompleted = TRUE;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCOMPLETED, NULL, NULL, NULL, NULL);
                return TRUE;
            }
            break;

        case DEF_QUESTTYPE_GOPLACE:
            if ((m_pClientList[iClientH]->m_bQuestMatchFlag_Loc == TRUE) &&
                (m_pClientList[iClientH]->m_sX >= m_pQuestConfigList[iQuestIndex]->m_sX - m_pQuestConfigList[iQuestIndex]->m_iRange) &&
                (m_pClientList[iClientH]->m_sX <= m_pQuestConfigList[iQuestIndex]->m_sX + m_pQuestConfigList[iQuestIndex]->m_iRange) &&
                (m_pClientList[iClientH]->m_sY >= m_pQuestConfigList[iQuestIndex]->m_sY - m_pQuestConfigList[iQuestIndex]->m_iRange) &&
                (m_pClientList[iClientH]->m_sY <= m_pQuestConfigList[iQuestIndex]->m_sY + m_pQuestConfigList[iQuestIndex]->m_iRange))
            {
                m_pClientList[iClientH]->m_bIsQuestCompleted = TRUE;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCOMPLETED, NULL, NULL, NULL, NULL);
                return TRUE;
            }
            break;
        }
    }

    return FALSE;
}

void CGame::SendItemNotifyMsg(int iClientH, WORD wMsgType, CItem * pItem, int iV1)
{
    char * cp, cData[512]{};
    DWORD * dwp;
    WORD * wp;
    short * sp;
    int     iRet;

    if (m_pClientList[iClientH] == NULL) return;

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_NOTIFY;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = wMsgType;
    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    switch (wMsgType)
    {
    case DEF_NOTIFY_ITEMOBTAINED:
        *cp = 1;
        cp++;

        memcpy(cp, pItem->m_cName, 20);
        cp += 20;

        dwp = (DWORD *)cp;
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

        wp = (WORD *)cp;
        *wp = pItem->m_wCurLifeSpan;
        cp += 2;

        wp = (WORD *)cp;
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

        *cp = (char)pItem->m_sItemSpecEffectValue2;
        cp++;

        dwp = (DWORD *)cp;
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

        dwp = (DWORD *)cp;
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

        wp = (WORD *)cp;
        *wp = pItem->m_wCurLifeSpan;
        cp += 2;

        wp = (WORD *)cp;
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

        wp = (WORD *)cp;
        *wp = iV1;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 48);
        break;

    case DEF_NOTIFY_CANNOTCARRYMOREITEM:
        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
        break;
    }
}

BOOL CGame::_bCheckItemReceiveCondition(int iClientH, CItem * pItem)
{
    int i;

    if (m_pClientList[iClientH] == NULL) return FALSE;

    if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount) > (DWORD)_iCalcMaxLoad(iClientH))
        return FALSE;

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] == NULL) return TRUE;

    return FALSE;
}

void CGame::_ClearQuestStatus(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;

    m_pClientList[iClientH]->m_iQuest = NULL;
    m_pClientList[iClientH]->m_iQuestID = NULL;
    m_pClientList[iClientH]->m_iQuestRewardType = NULL;
    m_pClientList[iClientH]->m_iQuestRewardAmount = NULL;
    m_pClientList[iClientH]->m_bIsQuestCompleted = FALSE;
}

int CGame::iGetMaxHP(int iClientH)
{
    int iRet;

    if (m_pClientList[iClientH] == NULL) return 0;

    iRet = m_pClientList[iClientH]->m_iVit * 3 + m_pClientList[iClientH]->m_iLevel * 2 + m_pClientList[iClientH]->m_iStr / 2;

    if (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown != 0)
        iRet = iRet - (iRet / m_pClientList[iClientH]->m_iSideEffect_MaxHPdown);

    return iRet;
}

int CGame::iGetMaxMP(int iClientH)
{
    int iRet;

    if (m_pClientList[iClientH] == NULL) return 0;

    iRet = (2 * m_pClientList[iClientH]->m_iMag) + (2 * m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iInt / 2);

    return iRet;
}

int CGame::iGetMaxSP(int iClientH)
{
    int iRet;

    if (m_pClientList[iClientH] == NULL) return 0;

    iRet = (2 * m_pClientList[iClientH]->m_iStr) + (2 * m_pClientList[iClientH]->m_iLevel);

    return iRet;
}

void CGame::GetMapInitialPoint(int iMapIndex, short * pX, short * pY, char * pPlayerLocation)
{
    int i, iTotalPoint;
    POINT  pList[DEF_MAXINITIALPOINT]{};

    if (m_pMapList[iMapIndex] == NULL) return;

    
    iTotalPoint = 0;
    for (i = 0; i < DEF_MAXINITIALPOINT; i++)
        if (m_pMapList[iMapIndex]->m_pInitialPoint[i].x != -1)
        {
            pList[iTotalPoint].x = m_pMapList[iMapIndex]->m_pInitialPoint[i].x;
            pList[iTotalPoint].y = m_pMapList[iMapIndex]->m_pInitialPoint[i].y;
            iTotalPoint++;
        }

    if (iTotalPoint == 0) return;

    
    if ((pPlayerLocation != NULL) && (memcmp(pPlayerLocation, "NONE", 4) == 0))
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
        if (m_pMapList[cMapIndex]->m_pStrategicPointList[i] != NULL)
        {

            iSide = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iSide;
            iValue = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iValue;
            iX = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iX;
            iY = m_pMapList[cMapIndex]->m_pStrategicPointList[i]->m_iY;

            pTile = (CTile *)(m_pMapList[cMapIndex]->m_pTile + iX + iY * m_pMapList[cMapIndex]->m_sSizeY);

            
            m_iStrategicStatus += pTile->m_iOccupyStatus * iValue;
        }
}

char CGame::_cGetSpecialAbility(int iKindSA)
{
    char cSA;

    switch (iKindSA)
    {
    case 1:
        // Slime, Orc, Orge, WereWolf, YB-, Rabbit, Mountain-Giant, Stalker, Hellclaw, 
        // Wyvern, Fire-Wyvern, Barlog, Tentocle, Centaurus, Giant-Lizard, Minotaurus,
        // Abaddon, Claw-Turtle, Giant-Cray-Fish, Giant-Plant, MasterMage-Orc, Nizie,
        // Tigerworm
        switch (iDice(1, 2))
        {
        case 1: cSA = 3; break; // Anti-Physical Damage
        case 2: cSA = 4; break; // Anti-Magic Damage
        }
        break;

    case 2:
        // Giant-Ant, Cat, Giant-Frog, 
        switch (iDice(1, 3))
        {
        case 1: cSA = 3; break; // Anti-Physical Damage
        case 2: cSA = 4; break; // Anti-Magic Damage
        case 3: cSA = 5; break; // Poisonous
        }
        break;

    case 3:
        // Zombie, Scorpion, Amphis, Troll, Dark-Elf
        switch (iDice(1, 4))
        {
        case 1: cSA = 3; break; // Anti-Physical Damage
        case 2: cSA = 4; break; // Anti-Magic Damage
        case 3: cSA = 5; break; // Poisonous
        case 4: cSA = 6; break; // Critical Poisonous
        }
        break;

    case 4:
        // no linked Npc
        switch (iDice(1, 3))
        {
        case 1: cSA = 3; break; // Anti-Physical Damage
        case 2: cSA = 4; break; // Anti-Magic Damage
        case 3: cSA = 7; break; // Explosive
        }
        break;

    case 5:
        // Stone-Golem, Clay-Golem, Beholder, Cannibal-Plant, Rudolph, DireBoar
        switch (iDice(1, 4))
        {
        case 1: cSA = 3; break; // Anti-Physical Damage
        case 2: cSA = 4; break; // Anti-Magic Damage
        case 3: cSA = 7; break; // Explosive
        case 4: cSA = 8; break; // Critical-Explosive
        }
        break;

    case 6:
        // no linked Npc
        switch (iDice(1, 3))
        {
        case 1: cSA = 3; break; // Anti-Physical Damage
        case 2: cSA = 4; break; // Anti-Magic Damage
        case 3: cSA = 5; break; // Poisonous
        }
        break;

    case 7:
        // Orc-Mage, Unicorn
        switch (iDice(1, 3))
        {
        case 1: cSA = 1; break; // Clairvoyant
        case 2: cSA = 2; break; // Distruction of Magic Protection
        case 3: cSA = 4; break; // Anti-Magic Damage
        }
        break;

    case 8:
        // Frost, Ice-Golem, Ettin, Gagoyle, Demon, Liche, Hellbound, Cyclops, 
        // Skeleton
        switch (iDice(1, 5))
        {
        case 1: cSA = 1; break; // Clairvoyant
        case 2: cSA = 2; break; // Distruction of Magic Protection
        case 3: cSA = 4; break; // Anti-Magic Damage
        case 4: cSA = 3; break; // Anti-Physical Damage
        case 5: cSA = 8; break; // Critical-Explosive
        }
        break;

    case 9:
        // no linked Npc
        cSA = iDice(1, 8); // All abilities available
        break;
    }

    return cSA;
}

void CGame::CheckSpecialEvent(int iClientH)
{
    CItem * pItem;
    char  cItemName[30];
    int   iEraseReq;

    if (m_pClientList[iClientH] == NULL) return;

    
    if (m_pClientList[iClientH]->m_iSpecialEventID == 200081)
    {

        
        if (m_pClientList[iClientH]->m_iLevel < 11)
        {
            m_pClientList[iClientH]->m_iSpecialEventID = 0;
            return;
        }

        ZeroMemory(cItemName, sizeof(cItemName));
        strcpy(cItemName, "MemorialRing");

        pItem = new CItem;
        if (_bInitItemAttr(pItem, cItemName) == FALSE)
        {
            
            delete pItem;
        }
        else
        {
            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
            {
                
                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

                
                wsprintf(G_cTxt, "(*) Get MemorialRing  : Char(%s)", m_pClientList[iClientH]->m_cCharName);
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

BOOL CGame::_bCheckDupItemID(CItem * pItem)
{
    int i;

    for (i = 0; i < DEF_MAXDUPITEMID; i++)
        if (m_pDupItemIDList[i] != NULL)
        {
            if ((pItem->m_sTouchEffectType == m_pDupItemIDList[i]->m_sTouchEffectType) &&
                (pItem->m_sTouchEffectValue1 == m_pDupItemIDList[i]->m_sTouchEffectValue1) &&
                (pItem->m_sTouchEffectValue2 == m_pDupItemIDList[i]->m_sTouchEffectValue2) &&
                (pItem->m_sTouchEffectValue3 == m_pDupItemIDList[i]->m_sTouchEffectValue3))
            {
                
                pItem->m_wPrice = m_pDupItemIDList[i]->m_wPrice;
                return TRUE;
            }
        }

    return FALSE;
}

void CGame::_AdjustRareItemValue(CItem * pItem)
{
    DWORD dwSWEType, dwSWEValue;
    double dV1, dV2, dV3;

    if ((pItem->m_dwAttribute & 0x00F00000) != NULL)
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

void CGame::_DeleteRandomOccupyFlag(int iMapIndex)
{
    int i, iCount, iTotalFlags, iTargetFlag, iDynamicObjectIndex;
    int tx, ty, fx, fy, iLocalSide, iLocalEKNum, iPrevStatus;
    CTile * pTile;
    DWORD dwTime;

    if (m_pMapList[iMapIndex] == NULL) return;

    dwTime = timeGetTime();

    
    iTotalFlags = 0;
    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != NULL)
        {
            iTotalFlags++;
        }

    
    iTargetFlag = iDice(1, iTotalFlags);

    iCount = 0;
    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != NULL)
        {
            iCount++;
            if ((iCount == iTotalFlags) && (m_pMapList[iMapIndex]->m_pOccupyFlag[i] != NULL))
            {
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
                    m_pDynamicObjectList[iDynamicObjectIndex]->m_sType, iDynamicObjectIndex, NULL);
                
                m_pMapList[m_pDynamicObjectList[iDynamicObjectIndex]->m_cMapIndex]->SetDynamicObject(NULL, NULL, m_pDynamicObjectList[iDynamicObjectIndex]->m_sX, m_pDynamicObjectList[iDynamicObjectIndex]->m_sY, dwTime);

                
                delete m_pMapList[iMapIndex]->m_pOccupyFlag[i];
                m_pMapList[iMapIndex]->m_pOccupyFlag[i] = NULL;

                
                pTile->m_iOccupyFlagIndex = NULL;

                
                delete m_pDynamicObjectList[iDynamicObjectIndex];
                m_pDynamicObjectList[iDynamicObjectIndex] = NULL;

                
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
                return;
            }
        }
}

BOOL CGame::bCheckEnergySphereDestination(int iNpcH, short sAttackerH, char cAttackerType)
{
    int i, sX, sY, dX, dY, iGoalMapIndex;
    char cResult;

    if (m_pNpcList[iNpcH] == NULL) return FALSE;
    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iCurEnergySphereGoalPointIndex == -1) return FALSE;

    if (m_pNpcList[iNpcH]->m_cMapIndex != m_iMiddlelandMapIndex)
    {
        
        iGoalMapIndex = m_pNpcList[iNpcH]->m_cMapIndex;

        sX = m_pNpcList[iNpcH]->m_sX;
        sY = m_pNpcList[iNpcH]->m_sY;

        cResult = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].cResult;
        dX = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenX;
        dY = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenY;
        if ((sX >= dX - 2) && (sX <= dX + 2) && (sY >= dY - 2) && (sY <= dY + 2))
        {
            
            
            m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 1)
                {
                    m_pClientList[sAttackerH]->m_iContribution += 5;
                    
                    wsprintf(G_cTxt, "(!) EnergySphere Hit By Aresden Player (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {
                    
                    m_pClientList[sAttackerH]->m_iContribution -= 10;
                }

                
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
                    {
                        
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 2, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }
            return TRUE;
        }

        dX = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineX;
        dY = m_pMapList[iGoalMapIndex]->m_stEnergySphereGoalList[m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineY;
        if ((sX >= dX - 2) && (sX <= dX + 2) && (sY >= dY - 2) && (sY <= dY + 2))
        {
            
            
            m_pMapList[iGoalMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 2)
                {
                    m_pClientList[sAttackerH]->m_iContribution += 5;
                    
                    wsprintf(G_cTxt, "(!) EnergySphere Hit By Elvine Player (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {
                    
                    m_pClientList[sAttackerH]->m_iContribution -= 10;
                }

                
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
                    {
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 1, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }
        }
        return FALSE;
    }
    else
    {
        

        
        sX = m_pNpcList[iNpcH]->m_sX;
        sY = m_pNpcList[iNpcH]->m_sY;

        cResult = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].cResult;
        dX = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenX;
        dY = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].aresdenY;
        if ((sX >= dX - 4) && (sX <= dX + 4) && (sY >= dY - 4) && (sY <= dY + 4))
        {
            
            
            m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 1)
                {
                    m_pClientList[sAttackerH]->m_iContribution += 5;
                    
                    wsprintf(G_cTxt, "(!) EnergySphere Hit By Aresden Player (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {
                    
                    m_pClientList[sAttackerH]->m_iContribution -= 10;
                }

                
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
                    {
                        
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 2, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }
            return TRUE;
        }

        dX = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineX;
        dY = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex].elvineY;
        if ((sX >= dX - 4) && (sX <= dX + 4) && (sY >= dY - 4) && (sY <= dY + 4))
        {
            
            
            m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex = -1;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
            {
                if (m_pClientList[sAttackerH]->m_cSide == 2)
                {
                    m_pClientList[sAttackerH]->m_iContribution += 5;
                    
                    wsprintf(G_cTxt, "(!) EnergySphere Hit By Aresden Player (%s)", m_pClientList[sAttackerH]->m_cCharName);
                    log->info(G_cTxt);
                }
                else
                {
                    
                    m_pClientList[sAttackerH]->m_iContribution -= 10;
                }

                
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
                    {
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHEREGOALIN, cResult, m_pClientList[sAttackerH]->m_cSide, 1, m_pClientList[sAttackerH]->m_cCharName);
                    }
            }
            return TRUE;
        }
        return FALSE;
    }
}

void CGame::EnergySphereProcessor(BOOL bIsAdminCreate, int iClientH)
{
    int i, iNamingValue, iCIndex, iTemp, pX, pY;
    char cSA, cName_Internal[31], cWaypoint[31];

    if (bIsAdminCreate != TRUE)
    {

        if (m_iMiddlelandMapIndex < 0) return;
        if (m_pMapList[m_iMiddlelandMapIndex] == NULL) return;
        
        if (iDice(1, 2000) != 123) return;
        
        if (m_iTotalGameServerClients < 500) return;

        
        if (m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex >= 0) return;

        
        iCIndex = iDice(1, m_pMapList[m_iMiddlelandMapIndex]->m_iTotalEnergySphereCreationPoint);

        
        if (m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereCreationList[iCIndex].cType == NULL) return;

        
        cSA = 0;
        pX = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereCreationList[iCIndex].sX;
        pY = m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereCreationList[iCIndex].sY;
        ZeroMemory(cWaypoint, sizeof(cWaypoint));

        iNamingValue = m_pMapList[m_iMiddlelandMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue != -1)
        {
            ZeroMemory(cName_Internal, sizeof(cName_Internal));
            wsprintf(cName_Internal, "XX%d", iNamingValue);
            cName_Internal[0] = '_';
            cName_Internal[1] = m_iMiddlelandMapIndex + 65;

            if ((bCreateNewNpc("Energy-Sphere", cName_Internal, m_pMapList[m_iMiddlelandMapIndex]->m_cName, (rand() % 5), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, FALSE)) == FALSE)
            {
                m_pMapList[m_iMiddlelandMapIndex]->SetNamingValueEmpty(iNamingValue);
                return;
            }
        }

        
        iTemp = iDice(1, m_pMapList[m_iMiddlelandMapIndex]->m_iTotalEnergySphereGoalPoint);
        if (m_pMapList[m_iMiddlelandMapIndex]->m_stEnergySphereGoalList[iTemp].cResult == NULL) return;

        
        m_pMapList[m_iMiddlelandMapIndex]->m_iCurEnergySphereGoalPointIndex = iTemp;

        
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHERECREATED, pX, pY, NULL, NULL);
            }

        wsprintf(G_cTxt, "(!) Energy Sphere Created! (%d, %d)", pX, pY);
        log->info(G_cTxt);
    }
    else
    {
        
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iCurEnergySphereGoalPointIndex >= 0) return;

        
        iCIndex = iDice(1, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalEnergySphereCreationPoint);

        
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stEnergySphereCreationList[iCIndex].cType == NULL) return;

        
        cSA = 0;
        pX = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stEnergySphereCreationList[iCIndex].sX;
        pY = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stEnergySphereCreationList[iCIndex].sY;
        ZeroMemory(cWaypoint, sizeof(cWaypoint));

        iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue != -1)
        {
            ZeroMemory(cName_Internal, sizeof(cName_Internal));
            wsprintf(cName_Internal, "XX%d", iNamingValue);
            cName_Internal[0] = '_';
            cName_Internal[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

            if ((bCreateNewNpc("Energy-Sphere", cName_Internal, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 5), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, FALSE)) == FALSE)
            {
                
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                return;
            }
        }

        
        iTemp = iDice(1, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalEnergySphereGoalPoint);
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stEnergySphereGoalList[iTemp].cResult == NULL) return;

        
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iCurEnergySphereGoalPointIndex = iTemp;

        
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_ENERGYSPHERECREATED, pX, pY, NULL, NULL);
            }

        wsprintf(G_cTxt, "(!) Admin Energy Sphere Created! (%d, %d)", pX, pY);
        log->info(G_cTxt);
    }
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

void CGame::UpdateMapSectorInfo()
{
    int i, ix, iy;
    int iMaxNeutralActivity, iMaxAresdenActivity, iMaxElvineActivity, iMaxMonsterActivity, iMaxPlayerActivity;

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL)
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
        if (m_pMapList[i] != NULL)
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

    log->info("(!) Middleland OccupyFlag data saved.");

    pData = new char[1000000 + 1];
    if (pData == NULL) return;
    ZeroMemory(pData, 1000000);

    iSize = _iComposeFlagStatusContents(pData);

    _mkdir("GameData");

    pFile = fopen("GameData\\OccupyFlag.txt", "wt");
    if (pFile == NULL) return;

    fwrite(pData, 1, iSize, pFile);

    delete pData;
    fclose(pFile);
}

void CGame::_SendMapStatus(int iClientH)
{
    int i, iDataSize;
    char * cp, cData[DEF_MAXCRUSADESTRUCTURES * 6];
    short * sp;

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)(cData);

    memcpy(cp, m_pClientList[iClientH]->m_cSendingMapName, 10);
    cp += 10;

    sp = (short *)cp;
    *sp = (short)m_pClientList[iClientH]->m_iCSIsendPoint;
    cp += 2;

    
    cp++;

    if (m_pClientList[iClientH]->m_iCSIsendPoint == NULL)
        m_pClientList[iClientH]->m_bIsSendingMapStatus = TRUE;

    
    iDataSize = 0;
    for (i = 0; i < 100; i++)
    {
        if (m_pClientList[iClientH]->m_iCSIsendPoint >= DEF_MAXCRUSADESTRUCTURES) goto SMS_ENDOFDATA;
        if (m_pClientList[iClientH]->m_stCrusadeStructureInfo[m_pClientList[iClientH]->m_iCSIsendPoint].cType == NULL) goto SMS_ENDOFDATA;

        *cp = m_pClientList[iClientH]->m_stCrusadeStructureInfo[m_pClientList[iClientH]->m_iCSIsendPoint].cType;
        cp++;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_stCrusadeStructureInfo[m_pClientList[iClientH]->m_iCSIsendPoint].sX;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_stCrusadeStructureInfo[m_pClientList[iClientH]->m_iCSIsendPoint].sY;
        cp += 2;
        *cp = m_pClientList[iClientH]->m_stCrusadeStructureInfo[m_pClientList[iClientH]->m_iCSIsendPoint].cSide;
        cp++;

        iDataSize += 6;
        m_pClientList[iClientH]->m_iCSIsendPoint++;
    }

    
    cp = (char *)(cData + 12);
    *cp = (iDataSize / 6);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAPSTATUSNEXT, iDataSize + 13, NULL, NULL, cData);
    return;

    SMS_ENDOFDATA:;

    
    cp = (char *)(cData + 12);
    *cp = (iDataSize / 6);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAPSTATUSLAST, iDataSize + 13, NULL, NULL, cData);
    m_pClientList[iClientH]->m_bIsSendingMapStatus = FALSE;

    return;
}

BOOL CGame::bStockMsgToGateServer(char * pData, DWORD dwSize)
{
    char * cp;

    if ((m_iIndexGSS + dwSize) >= DEF_MAXGATESERVERSTOCKMSGSIZE - 10) return FALSE;

    cp = (char *)(m_cGateServerStockMsg + m_iIndexGSS);
    memcpy(cp, pData, dwSize);

    m_iIndexGSS += dwSize;

    return TRUE;
}

void CGame::SendStockMsgToGateServer()
{
    DWORD * dwp;
    WORD * wp;
    char * cp;

    
    if (m_iIndexGSS > 6)
    {
        //SendMsgToGateServer(MSGID_SERVERSTOCKMSG, NULL, m_cGateServerStockMsg);
        
        ZeroMemory(m_cGateServerStockMsg, sizeof(m_cGateServerStockMsg));
        
        cp = (char *)m_cGateServerStockMsg;
        dwp = (DWORD *)cp;
        *dwp = MSGID_SERVERSTOCKMSG;
        cp += 4;
        wp = (WORD *)cp;
        *wp = DEF_MSGTYPE_CONFIRM;
        cp += 2;

        m_iIndexGSS = 6;
    }
}

void CGame::GSM_RequestFindCharacter(WORD wReqServerID, WORD wReqClientH, char * pName, char * pFinder)
{
    char * cp, cTemp[120];
    WORD * wp;
    int i;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cCharName, pName) == 0))
        {
            
            ZeroMemory(cTemp, sizeof(cTemp));
            cp = (char *)(cTemp);
            *cp = GSM_RESPONSE_FINDCHARACTER;
            cp++;

            wp = (WORD *)cp;
            *wp = wReqServerID;
            cp += 2;

            wp = (WORD *)cp;
            *wp = wReqClientH;
            cp += 2;

            memcpy(cp, pName, 10);
            cp += 10;

            memcpy(cp, pFinder, 10);
            cp += 10;

            memcpy(cp, m_pClientList[i]->m_cMapName, 10);
            cp += 10;

            wp = (WORD *)cp;
            *wp = m_pClientList[i]->m_sX;
            cp += 2;

            wp = (WORD *)cp;
            *wp = m_pClientList[i]->m_sY;
            cp += 2;

            bStockMsgToGateServer(cTemp, 39);
            return;
        }
}

void CGame::SyncMiddlelandMapInfo()
{
    int i;
    char * cp;
    short * sp;

    
    if (m_iMiddlelandMapIndex != -1)
    {
        
        for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
        {
            m_stMiddleCrusadeStructureInfo[i].cType = NULL;
            m_stMiddleCrusadeStructureInfo[i].cSide = NULL;
            m_stMiddleCrusadeStructureInfo[i].sX = NULL;
            m_stMiddleCrusadeStructureInfo[i].sY = NULL;
        }
        
        m_iTotalMiddleCrusadeStructures = m_pMapList[m_iMiddlelandMapIndex]->m_iTotalCrusadeStructures;
        ZeroMemory(G_cData50000, sizeof(G_cData50000));
        cp = (char *)G_cData50000;
        *cp = GSM_MIDDLEMAPSTATUS;
        cp++;

        sp = (short *)cp;
        *sp = (short)m_iTotalMiddleCrusadeStructures;
        cp += 2;

        for (i = 0; i < m_iTotalMiddleCrusadeStructures; i++)
        {
            m_stMiddleCrusadeStructureInfo[i].cType = m_pMapList[m_iMiddlelandMapIndex]->m_stCrusadeStructureInfo[i].cType;
            m_stMiddleCrusadeStructureInfo[i].cSide = m_pMapList[m_iMiddlelandMapIndex]->m_stCrusadeStructureInfo[i].cSide;
            m_stMiddleCrusadeStructureInfo[i].sX = m_pMapList[m_iMiddlelandMapIndex]->m_stCrusadeStructureInfo[i].sX;
            m_stMiddleCrusadeStructureInfo[i].sY = m_pMapList[m_iMiddlelandMapIndex]->m_stCrusadeStructureInfo[i].sY;

            *cp = m_stMiddleCrusadeStructureInfo[i].cType;
            cp++;
            *cp = m_stMiddleCrusadeStructureInfo[i].cSide;
            cp++;
            sp = (short *)cp;
            *sp = (short)m_stMiddleCrusadeStructureInfo[i].sX;
            cp += 2;
            sp = (short *)cp;
            *sp = (short)m_stMiddleCrusadeStructureInfo[i].sY;
            cp += 2;
        }

        
        if (m_iTotalMiddleCrusadeStructures != 0)
        {
            //bStockMsgToGateServer(G_cData50000, 3 + m_iTotalMiddleCrusadeStructures * 6);
        }
    }
}

void CGame::GSM_SetGuildConstructLoc(int iGuildGUID, int dX, int dY, char * pMapName)
{
    int i, iIndex;
    DWORD dwTemp, dwTime;

    wsprintf(G_cTxt, "SetGuildConstructLoc: %d %s %d %d", iGuildGUID, pMapName, dX, dY);
    log->info(G_cTxt);

    dwTime = timeGetTime();

    
    for (i = 0; i < DEF_MAXGUILDS; i++)
        if (m_pGuildTeleportLoc[i].m_iV1 == iGuildGUID)
        {
            
            if ((m_pGuildTeleportLoc[i].m_sDestX2 == dX) && (m_pGuildTeleportLoc[i].m_sDestY2 == dY) && (strcmp(m_pGuildTeleportLoc[i].m_cDestMapName2, pMapName) == 0))
            {
                
                m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;
                return;
            }
            else
            {
                
                m_pGuildTeleportLoc[i].m_sDestX2 = dX;
                m_pGuildTeleportLoc[i].m_sDestY2 = dY;
                ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName2, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName2));
                strcpy(m_pGuildTeleportLoc[i].m_cDestMapName2, pMapName);
                m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;
                return;
            }
        }

    
    dwTemp = 0;
    iIndex = -1;
    for (i = 0; i < DEF_MAXGUILDS; i++)
    {
        if (m_pGuildTeleportLoc[i].m_iV1 == NULL)
        {

            m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
            m_pGuildTeleportLoc[i].m_sDestX2 = dX;
            m_pGuildTeleportLoc[i].m_sDestY2 = dY;
            ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName2, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
            strcpy(m_pGuildTeleportLoc[i].m_cDestMapName2, pMapName);
            m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;
            return;
        }
        else
        {
            
            if (dwTemp < (dwTime - m_pGuildTeleportLoc[i].m_dwTime2))
            {
                dwTemp = (dwTime - m_pGuildTeleportLoc[i].m_dwTime2);
                iIndex = i;
            }
        }
    }

    
    if (iIndex == -1) return;

    log->info("(X) No more GuildConstLocuct Space! Replaced.");

    m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
    m_pGuildTeleportLoc[i].m_sDestX2 = dX;
    m_pGuildTeleportLoc[i].m_sDestY2 = dY;
    ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName2, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName2));
    strcpy(m_pGuildTeleportLoc[i].m_cDestMapName2, pMapName);
    m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;
}

void CGame::GSM_ConstructionPoint(int iGuildGUID, int iPoint)
{
    int i;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iCrusadeDuty == 3) &&
            (m_pClientList[i]->m_iGuildGUID == iGuildGUID))
        {
            
            m_pClientList[i]->m_iConstructionPoint += iPoint;
            m_pClientList[i]->m_iWarContribution += iPoint / 10;

            if (m_pClientList[i]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                m_pClientList[i]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

            if (m_pClientList[i]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                m_pClientList[i]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

            SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, NULL, NULL);
            wsprintf(G_cTxt, "GSM_ConstructionPoint: %d %d", iGuildGUID, iPoint);
            log->info(G_cTxt);
            return;
        }
}

BOOL CGame::bAddClientShortCut(int iClientH)
{
    int i;

    

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_iClientShortCut[i] == iClientH) return FALSE;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if (m_iClientShortCut[i] == 0)
        {
            m_iClientShortCut[i] = iClientH;
            return TRUE;
        }

    return FALSE;
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

    
    //m_iClientShortCut[i] = m_iClientShortCut[m_iTotalClients+1];
    //m_iClientShortCut[m_iTotalClients+1] = 0;
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
    if (strcmp(pMapName, "elvine") == 0) return 4;
    if (strcmp(pMapName, "arebrk11") == 0) return 3;
    if (strcmp(pMapName, "elvbrk11") == 0) return 4;

    if (strcmp(pMapName, "cityhall_1") == 0) return 1;
    if (strcmp(pMapName, "cityhall_2") == 0) return 2;
    if (strcmp(pMapName, "cath_1") == 0) return 1;
    if (strcmp(pMapName, "cath_2") == 0) return 2;
    if (strcmp(pMapName, "gshop_1") == 0) return 1;
    if (strcmp(pMapName, "gshop_2") == 0) return 2;
    if (strcmp(pMapName, "bsmith_1") == 0) return 1;
    if (strcmp(pMapName, "bsmith_2") == 0) return 2;
    if (strcmp(pMapName, "wrhus_1") == 0) return 1;
    if (strcmp(pMapName, "wrhus_2") == 0) return 2;
    if (strcmp(pMapName, "gldhall_1") == 0) return 1;
    if (strcmp(pMapName, "gldhall_2") == 0) return 2;
    if (strcmp(pMapName, "wzdtwr_1") == 0) return 1;
    if (strcmp(pMapName, "wzdtwr_2") == 0) return 2;
    if (strcmp(pMapName, "arefarm") == 0) return 1;
    if (strcmp(pMapName, "elvfarm") == 0) return 2;
    if (strcmp(pMapName, "arewrhus") == 0) return 1;
    if (strcmp(pMapName, "elvwrhus") == 0) return 2;
    if (strcmp(pMapName, "cmdhall_1") == 0) return 1;
    if (strcmp(pMapName, "Cmdhall_2") == 0) return 2;

    return 0;
}

BOOL CGame::bCopyItemContents(CItem * pCopy, CItem * pOriginal)
{
    if (pOriginal == NULL) return FALSE;
    if (pCopy == NULL) return FALSE;

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

    return TRUE;
}

void CGame::SendMsg(short sOwnerH, char cOwnerType, BOOL bStatus, long lPass)
{
    if (m_pClientList[sOwnerH] == NULL) return;
    //SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_LEVELUP, NULL, NULL, NULL, NULL);

    SendNotifyMsg(NULL, sOwnerH, lPass, NULL, NULL, NULL, NULL);
}

void CGame::RequestChangePlayMode(int iClientH)
{

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iPKCount > 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "cityhall", 8) != 0) return;

    if (m_pClientList[iClientH]->m_iLevel < 100 ||
        m_pClientList[iClientH]->m_bIsPlayerCivil == TRUE)
    {
        if (memcmp(m_pClientList[iClientH]->m_cLocation, "aresden", 7) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "arehunter");
        else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvine", 6) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "elvhunter");
        else if (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "aresden");
        else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "elvine");

        if (m_pClientList[iClientH]->m_bIsPlayerCivil == TRUE)
            m_pClientList[iClientH]->m_bIsPlayerCivil = FALSE;
        else m_pClientList[iClientH]->m_bIsPlayerCivil = TRUE;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CHANGEPLAYMODE, NULL, NULL, NULL, m_pClientList[iClientH]->m_cLocation);
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, 100, NULL, NULL, NULL);
    }
}

void CGame::SetInvisibilityFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000010;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFFEF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000010;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFFEF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetInhibitionCastingFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00100000;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFEFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00100000;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFEFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetBerserkFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000020;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFFDF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000020;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFFDF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetIceFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000040;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFFBF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000040;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFFBF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetPoisonFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000080;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFF7F;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000080;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFF7F;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetIllusionFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x01000000;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFEFFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x01000000;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFEFFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetHeroFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00020000;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFDFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00020000;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFDFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetDefenseShieldFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x02000000;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFDFFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x02000000;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFDFFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetMagicProtectionFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x04000000;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFBFFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x04000000;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFBFFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetProtectionFromArrowFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x08000000;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xF7FFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;

    case DEF_OWNERTYPE_NPC:
        if (m_pNpcList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x08000000;
        else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xF7FFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

void CGame::SetIllusionMovementFlag(short sOwnerH, char cOwnerType, BOOL bStatus)
{
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[sOwnerH] == NULL) return;
        if (bStatus == TRUE)
            m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00200000;
        else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFDFFFFF;
        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        break;
    }
}

BOOL CGame::_bItemLog(int iAction, int iGiveH, int iRecvH, CItem * pItem, BOOL bForceItemLog)
{
    char  cTxt[1024], cTemp1[120], cTemp2[120];
    int iItemCount;
    if (pItem == NULL) return FALSE;

    
    if (m_pClientList[iGiveH]->m_cCharName == NULL) return FALSE;

    if (iAction == DEF_ITEMLOG_DUPITEMID)
    {
        
        if (m_pClientList[iGiveH] == NULL) return FALSE;
        if (m_pClientList[iGiveH]->m_cCharName == NULL) return FALSE;
        wsprintf(G_cTxt, "(!) Delete-DupItem(%s %d %d %d %d) Owner(%s)", pItem->m_cName, pItem->m_dwCount, pItem->m_sTouchEffectValue1,
            pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3,
            m_pClientList[iGiveH]->m_cCharName);
        log->info(G_cTxt);
        //bSendMsgToLS(MSGID_GAMEITEMLOG, iGiveH, NULL, G_cTxt);
        return TRUE;
    }

    ZeroMemory(cTxt, sizeof(cTxt));
    ZeroMemory(cTemp1, sizeof(cTemp1));
    ZeroMemory(cTemp2, sizeof(cTemp2));

    switch (iAction)
    {

    case DEF_ITEMLOG_EXCHANGE:
        if (m_pClientList[iRecvH]->m_cCharName == NULL) return FALSE;
        wsprintf(cTxt, "(%s) PC(%s)\tExchange\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_GIVE:
        if (m_pClientList[iRecvH]->m_cCharName == NULL) return FALSE;
        wsprintf(cTxt, "(%s) PC(%s)\tGive\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_DROP:
        wsprintf(cTxt, "(%s) PC(%s)\tDrop\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_GET:
        wsprintf(cTxt, "(%s) PC(%s)\tGet\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_MAKE:
        wsprintf(cTxt, "(%s) PC(%s)\tMake\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_DEPLETE:
        wsprintf(cTxt, "(%s) PC(%s)\tDeplete\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_BUY:
        iItemCount = iRecvH;
        wsprintf(cTxt, "(%s) PC(%s)\tBuy\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, iItemCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_SELL:
        wsprintf(cTxt, "(%s) PC(%s)\tSell\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_RETRIEVE:
        wsprintf(cTxt, "(%s) PC(%s)\tRetrieve\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_DEPOSIT:
        wsprintf(cTxt, "(%s) PC(%s)\tDeposit\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_UPGRADEFAIL:
        wsprintf(cTxt, "(%s) PC(%s)\tUpgrade Fail\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
            pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;

    case DEF_ITEMLOG_UPGRADESUCCESS:
        wsprintf(cTxt, "(%s) PC(%s)\tUpgrade Success\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
            pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
            m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
        log->info(cTxt);
        break;
    default:
        return FALSE;
    }
    //bSendMsgToLS(MSGID_GAMEITEMLOG, iGiveH, NULL, cTxt);
    return TRUE;
}

BOOL CGame::_bItemLog(int iAction, int iClientH, char * cName, CItem * pItem)
{
    if (pItem == NULL) return FALSE;
    if (_bCheckGoodItem(pItem) == FALSE) return FALSE;
    if (iAction != DEF_ITEMLOG_NEWGENDROP)
    {
        if (m_pClientList[iClientH] == NULL) return FALSE;
    }
    char  cTxt[200], cTemp1[120];
    
    ZeroMemory(cTxt, sizeof(cTxt));
    ZeroMemory(cTemp1, sizeof(cTemp1));
    //if (m_pClientList[iClientH] != NULL) m_pClientList[iClientH]->iGetPeerAddress(cTemp1);

    switch (iAction)
    {

    case DEF_ITEMLOG_NEWGENDROP:
        if (pItem == NULL) return FALSE;
        wsprintf(cTxt, "NPC(%s)\tDrop\t%s(%d %d %d %d)", cName, pItem->m_cName, pItem->m_dwCount,
            pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3);
        log->info(cTxt);
        break;
    case DEF_ITEMLOG_SKILLLEARN:
    case DEF_ITEMLOG_MAGICLEARN:
        if (cName == NULL) return FALSE;
        if (m_pClientList[iClientH] == NULL) return FALSE;
        wsprintf(cTxt, "PC(%s)\tLearn\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
            m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
        log->info(cTxt);
        break;
    case DEF_ITEMLOG_SUMMONMONSTER:
        if (cName == NULL) return FALSE;
        if (m_pClientList[iClientH] == NULL) return FALSE;
        wsprintf(cTxt, "PC(%s)\tSummon\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
            m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
        break;
    case DEF_ITEMLOG_POISONED:
        if (m_pClientList[iClientH] == NULL) return FALSE;
        wsprintf(cTxt, "PC(%s)\tBe Poisoned\t \t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName,
            m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
        break;

    case DEF_ITEMLOG_REPAIR:
        if (cName == NULL) return FALSE;
        if (m_pClientList[iClientH] == NULL) return FALSE;
        wsprintf(cTxt, "PC(%s)\tRepair\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
            m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
        break;

    default:
        return FALSE;
    }
    //bSendMsgToLS(MSGID_GAMEITEMLOG, iClientH, NULL, cTxt);
    return TRUE;
}

BOOL CGame::_bCheckGoodItem(CItem * pItem)
{
    if (pItem == NULL) return FALSE;

    if (pItem->m_sIDnum == 90)
    {
        if (pItem->m_dwCount > 10000) return TRUE;  
        else return FALSE;
    }
    switch (pItem->m_sIDnum)
    {
        
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

    case 650:
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
    case 924:

        return TRUE;  
        break;
    default:
        
        if ((pItem->m_dwAttribute & 0xF0F0F001) == NULL) return FALSE;  
        else if (pItem->m_sIDnum > 30) return TRUE;  
        else return FALSE;  
    }
}

void CGame::GetExp(int iClientH, int iExp, BOOL bIsAttackerOwn)
{
    double dV1, dV2, dV3;
    int i, iH, iUnitValue;
    DWORD dwTime = timeGetTime();
    int iTotalPartyMembers;

    if (m_pClientList[iClientH] == NULL) return;
    if (iExp <= 0) return;

    if (m_pClientList[iClientH]->m_iLevel <= 80)
    {
        dV1 = (double)(80 - m_pClientList[iClientH]->m_iLevel);
        dV2 = dV1 * 0.025f;
        dV3 = (double)iExp;
        dV1 = (dV2 + 1.025f) * dV3;
        iExp = (int)dV1;
    }
    else
    {
         //Lower exp
        if ((m_pClientList[iClientH]->m_iLevel >= 80) && ((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresdend1") == 0)
            || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvined1") == 0)))
        {
            iExp = (iExp / 10);
        }
        else if ((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresdend1") == 0)
            || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvined1") == 0))
        {
            iExp = (iExp * 1 / 4);
        }
    }

    //Check for party status, else give exp to player
    if ((m_pClientList[iClientH]->m_iPartyID != NULL) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM))
    {
        //Only divide exp if >= 1 person and exp > 10
        if (iExp >= 10 && m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers > 0)
        {

            //Calc total ppl in party
            iTotalPartyMembers = 0;
            for (i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++)
            {
                iH = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];
                if ((m_pClientList[iH] != NULL) && (m_pClientList[iH]->m_iHP > 0))
                {
                    //Newly added, Only players on same map get exp :}
                    if ((strlen(m_pMapList[m_pClientList[iH]->m_cMapIndex]->m_cName)) == (strlen(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName)))
                    {
                        if (memcmp(m_pMapList[m_pClientList[iH]->m_cMapIndex]->m_cName,
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName,
                            strlen(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName)) == 0)
                        {
                            iTotalPartyMembers++;
                        }
                    }
                }
            }

            //Check for party bug
            if (iTotalPartyMembers > 8)
            {
                wsprintf(G_cTxt, "(X) Party Bug !! partyMember %d XXXXXXXXXX", iTotalPartyMembers);
                log->info(G_cTxt);
                iTotalPartyMembers = 8;
            }

            //Figure out how much exp a player gets
            dV1 = (double)iExp;

            switch (iTotalPartyMembers)
            {
            case 1:
                dV2 = dV1;
                break;
            case 2:
                dV2 = (dV1 + (dV1 * 2.0e-2)) / 2.0;
                break;
            case 3:
                dV2 = (dV1 + (dV1 * 5.0e-2)) / 3.0;
                break;
            case 4:
                dV2 = (dV1 + (dV1 * 7.000000000000001e-2)) / 4.0;
                break;
            case 5:
                dV2 = (dV1 + (dV1 * 1.0e-1)) / 5.0;
                break;
            case 6:
                dV2 = (dV1 + (dV1 * 1.4e-1)) / 6.0;
                break;
            case 7:
                dV2 = (dV1 + (dV1 * 1.7e-1)) / 7.0;
                break;
            case 8:
                dV2 = (dV1 + (dV1 * 2.0e-1)) / 8.0;
                break;
            }

            dV3 = dV2 + 5.0e-1;

            //Divide exp among party members
            for (i = 0; i < iTotalPartyMembers; i++)
            {
                iUnitValue = (int)dV3;
                iH = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];
                if ((m_pClientList[iH] != NULL) && (m_pClientList[iH]->m_bSkillUsingStatus[19] != 1) && (m_pClientList[iH]->m_iHP > 0))
                {
                    // Is player alive ??
                    if (m_pClientList[iH]->m_iLevel < 81)   iUnitValue *= 2;
                    else if (m_pClientList[iH]->m_iLevel < 101)   iUnitValue *= 2;
                    else if (m_pClientList[iH]->m_iLevel < 151)  iUnitValue *= 2;
                    else if (m_pClientList[iH]->m_iLevel < 500)  iUnitValue *= 2;
                    if ((m_pClientList[iH]->m_iStatus & 0x10000) != 0)  iUnitValue *= 3;
                    m_pClientList[iH]->m_iExpStock += iUnitValue;
                }
            }
        }
    }
    else
    {
        if ((m_pClientList[iClientH] != NULL) && (m_pClientList[iClientH]->m_bSkillUsingStatus[19] != 1) && (m_pClientList[iClientH]->m_iHP > 0))
        {
            // Is player alive ??
            if (m_pClientList[iClientH]->m_iLevel < 81)   iExp *= 1.5;
            else if (m_pClientList[iClientH]->m_iLevel < 101)   iExp *= 1.5;
            else if (m_pClientList[iClientH]->m_iLevel < 151)  iExp *= 1.5;
            else if (m_pClientList[iClientH]->m_iLevel < 500)  iExp *= 1.5;
            if ((m_pClientList[iClientH]->m_iStatus & 0x10000) != 0)  iExp *= 3;
            m_pClientList[iClientH]->m_iExpStock += iExp;
        }
    }
}

BOOL CGame::bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex)
{
    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return FALSE;

    switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
    {
    case 4:  // Dagger +1
    case 9:  // Short Sword +1
    case 13: // Main Gauge +1
    case 16: // Gradius +1
    case 18: // Long Sword +1
    case 19: // Long Sword +2
    case 21: // Excaliber +1
    case 24: // Sabre +1
    case 26: // Scimitar +1
    case 27: // Scimitar +2
    case 29: // Falchoin +1
    case 30: // Falchion +2
    case 32: // Esterk +1
    case 33: // Esterk +2
    case 35: // Rapier +1
    case 36: // Rapier +2
    case 39: // Broad Sword +1
    case 40: // Broad Sword +2
    case 43: // Bastad Sword +1
    case 44: // Bastad Sword +2
    case 47: // Claymore +1
    case 48: // Claymore +2
    case 51: // Great Sword +1
    case 52: // Great Sword +2
    case 55: // Flameberge +1
    case 56: // Flameberge +2
    case 60: // Light Axe +1
    case 61: // Light Axe +2
    case 63: // Tomahoc +1
    case 64: // Tomohoc +2
    case 66: // Sexon Axe +1
    case 67: // Sexon Axe +2
    case 69: // Double Axe +1
    case 70: // Double Axe +2
    case 72: // War Axe +1
    case 73: // War Axe +2

    case 580: // Battle Axe +1
    case 581: // Battle Axe +2
    case 582: // Sabre +2
        return TRUE;
        break;
    }
    return FALSE;
}

void CGame::ArmorLifeDecrement(int iAttackerH, int iTargetH, char cOwnerType, int iValue)
{
    int iTemp;

    if (m_pClientList[iAttackerH] == NULL) return;
    
    switch (cOwnerType)
    {
    case DEF_OWNERTYPE_PLAYER:
        if (m_pClientList[iTargetH] == NULL) return;
        break;

    case DEF_OWNERTYPE_NPC:	return;
    default: return;
    }

    
    if (m_pClientList[iAttackerH]->m_cSide == m_pClientList[iTargetH]->m_cSide) return;

    
    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL))
    {
        
        if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if ((m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) || (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 64000))
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
            
            
            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
            
            ReleaseItemHandler(iTargetH, iTemp, TRUE);  
        }
    }

    
    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL))
    {

        
        if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if ((m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) || (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 64000))
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
            
            
            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
            
            ReleaseItemHandler(iTargetH, iTemp, TRUE);  
        }
    }

    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL))
    {

        
        if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if ((m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) || (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 64000))
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
            
            
            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
            
            ReleaseItemHandler(iTargetH, iTemp, TRUE);  
        }
    }

    
    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL))
    {

        
        if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if ((m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) || (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 64000))
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
            
            
            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
            
            ReleaseItemHandler(iTargetH, iTemp, TRUE);  
        }
    }

    
    iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
    if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != NULL))
    {

        
        if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;

        if ((m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) || (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 64000))
        {
            m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
            
            
            SendNotifyMsg(NULL, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
            
            ReleaseItemHandler(iTargetH, iTemp, TRUE);  
        }
    }
}

BOOL CGame::_bCrusadeLog(int iAction, int iClientH, int iData, char * cName)
{
    char  cTxt[200];

    
    ZeroMemory(cTxt, sizeof(cTxt));

    switch (iAction)
    {

    case DEF_CRUSADELOG_ENDCRUSADE:
        if (cName == NULL) return FALSE;
        wsprintf(cTxt, "\tEnd Crusade\t%s", cName);
        break;

    case DEF_CRUSADELOG_SELECTDUTY:
        if (cName == NULL) return FALSE;
        if (m_pClientList[iClientH] == NULL) return FALSE;
        wsprintf(cTxt, "PC(%s)\tSelect Duty\t(%s)\t \t(%s)", m_pClientList[iClientH]->m_cCharName, cName, m_pClientList[iClientH]->m_cGuildName);
        break;

    case DEF_CRUSADELOG_GETEXP:
        if (m_pClientList[iClientH] == NULL) return FALSE;
        wsprintf(cTxt, "PC(%s)\tGet Exp\t(%d)\t \t(%s)", m_pClientList[iClientH]->m_cCharName, iData, m_pClientList[iClientH]->m_cGuildName);
        break;

    case DEF_CRUSADELOG_STARTCRUSADE:
        wsprintf(cTxt, "\tStart Crusade");
        break;

    default:
        return FALSE;
    }

    //bSendMsgToLS(MSGID_GAMECRUSADELOG, iClientH, NULL, cTxt);
    return TRUE;
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

    if (m_pClientList[iClientH]->m_bIsPlayerCivil == TRUE)
        iRet = iRet | 1;

    return iRet;
}

BOOL CGame::_bInitItemAttr(CItem * pItem, int iItemID)
{
    int i;

    for (i = 0; i < DEF_MAXITEMTYPES; i++)
        if (m_pItemConfigList[i] != NULL)
        {
            if (m_pItemConfigList[i]->m_sIDnum == iItemID)
            {
                
                ZeroMemory(pItem->m_cName, sizeof(pItem->m_cName));
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

                return TRUE;
            }
        }
    return FALSE;
}

void CGame::SetSlateFlag(int iClientH, short sType, bool bFlag)
{
    if (m_pClientList[iClientH] == NULL) return;

    if (sType == DEF_NOTIFY_SLATECLEAR)
    {
        m_pClientList[iClientH]->m_iStatus &= 0xFFBFFFFF;
        m_pClientList[iClientH]->m_iStatus &= 0xFF7FFFFF;
        m_pClientList[iClientH]->m_iStatus &= 0xFFFEFFFF;
        return;
    }

    if (bFlag == TRUE)
    {
        if (sType == 1)
        {
            // Invincible slate
            m_pClientList[iClientH]->m_iStatus |= 0x400000;
        }
        else if (sType == 3)
        {
            // Mana slate
            m_pClientList[iClientH]->m_iStatus |= 0x800000;
        }
        else if (sType == 4)
        {
            // Exp slate
            m_pClientList[iClientH]->m_iStatus |= 0x10000;
        }
    }
    else
    {
        if ((m_pClientList[iClientH]->m_iStatus & 0x400000) != 0)
        {
            m_pClientList[iClientH]->m_iStatus &= 0xFFBFFFFF;
        }
        else if ((m_pClientList[iClientH]->m_iStatus & 0x800000) != 0)
        {
            m_pClientList[iClientH]->m_iStatus &= 0xFF7FFFFF;
        }
        else if ((m_pClientList[iClientH]->m_iStatus & 0x10000) != 0)
        {
            m_pClientList[iClientH]->m_iStatus &= 0xFFFEFFFF;
        }
    }

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
}

void CGame::SetPlayingStatus(int iClientH)
{
    char cMapName[11], cLocation[11];

    if (m_pClientList[iClientH] == NULL) return;

    ZeroMemory(cMapName, sizeof(cMapName));
    ZeroMemory(cLocation, sizeof(cLocation));

    strcpy(cLocation, m_pClientList[iClientH]->m_cLocation);
    strcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);

    m_pClientList[iClientH]->m_cSide = 0;
    m_pClientList[iClientH]->m_bIsOwnLocation = FALSE;
    m_pClientList[iClientH]->m_bIsPlayerCivil = FALSE;

    if (memcmp(cLocation, cMapName, 3) == 0)
    {
        m_pClientList[iClientH]->m_bIsOwnLocation = TRUE;
    }

    if (memcmp(cLocation, "are", 3) == 0)
        m_pClientList[iClientH]->m_cSide = 1;
    else if (memcmp(cLocation, "elv", 3) == 0)
        m_pClientList[iClientH]->m_cSide = 2;
    else
    {
        if (strcmp(cMapName, "elvine") == 0 || strcmp(cMapName, "aresden") == 0)
        {
            m_pClientList[iClientH]->m_bIsOwnLocation = TRUE;
        }
        m_pClientList[iClientH]->m_bIsNeutral = TRUE;
    }

    if (memcmp(cLocation, "arehunter", 9) == 0 || memcmp(cLocation, "elvhunter", 9) == 0)
    {
        m_pClientList[iClientH]->m_bIsPlayerCivil = TRUE;
    }

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "bisle", 5) == 0)
    {
        m_pClientList[iClientH]->m_bIsPlayerCivil = FALSE;
    }

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "bsmith", 6) == 0 ||
        memcmp(m_pClientList[iClientH]->m_cMapName, "gldhall", 7) == 0 ||
        memcmp(m_pClientList[iClientH]->m_cMapName, "gshop", 5) == 0)
        m_pClientList[iClientH]->m_pIsProcessingAllowed = TRUE;
    else
        m_pClientList[iClientH]->m_pIsProcessingAllowed = FALSE;

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "wrhus", 5) == 0 ||
        memcmp(m_pClientList[iClientH]->m_cMapName, "arewrhus", 8) == 0 ||
        memcmp(m_pClientList[iClientH]->m_cMapName, "elvwrhus", 8) == 0)
        m_pClientList[iClientH]->m_bIsInsideWarehouse = TRUE;
    else
        m_pClientList[iClientH]->m_bIsInsideWarehouse = FALSE;

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "wzdtwr", 6) == 0)
        m_pClientList[iClientH]->m_bIsInsideWizardTower = TRUE;
    else
        m_pClientList[iClientH]->m_bIsInsideWizardTower = FALSE;
}

void CGame::ForceChangePlayMode(int iClientH, bool bNotify)
{
    if (m_pClientList[iClientH] == NULL) return;

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0)
        strcpy(m_pClientList[iClientH]->m_cLocation, "aresden");
    else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0)
        strcpy(m_pClientList[iClientH]->m_cLocation, "elvine");

    if (m_pClientList[iClientH]->m_bIsPlayerCivil == TRUE)
        m_pClientList[iClientH]->m_bIsPlayerCivil = FALSE;

    if (bNotify)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CHANGEPLAYMODE, NULL, NULL, NULL, m_pClientList[iClientH]->m_cLocation);
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
    }
}

void CGame::ShowVersion(int iClientH)
{
    char cVerMessage[256];

    ZeroMemory(cVerMessage, sizeof(cVerMessage));
    wsprintf(cVerMessage, "Helbreath.io %d.%d.%d - https://helbreath.io", UPPER_VERSION, LOWER_VERSION, PATCH_VERSION);
    ShowClientMsg(iClientH, cVerMessage);
}

void CGame::GSM_RequestShutupPlayer(char * pGMName, WORD wReqServerID, WORD wReqClientH, WORD wTime, char * pPlayer)
{
    char * cp, cTemp[120];
    WORD * wp;
    int i;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cCharName, pPlayer) == 0))
        {
            
            ZeroMemory(cTemp, sizeof(cTemp));
            cp = (char *)(cTemp);
            *cp = GSM_RESPONSE_SHUTUPPLAYER;
            cp++;

            wp = (WORD *)cp;
            *wp = wReqServerID;
            cp += 2;

            wp = (WORD *)cp;
            *wp = wReqClientH;
            cp += 2;

            memcpy(cp, pGMName, 10);
            cp += 10;

            wp = (WORD *)cp;
            *wp = (WORD)wTime;
            cp += 2;

            memcpy(cp, pPlayer, 10);
            cp += 10;

            
            m_pClientList[i]->m_iTimeLeft_ShutUp = wTime * 20; 

            SendNotifyMsg(NULL, i, DEF_NOTIFY_PLAYERSHUTUP, wTime, NULL, NULL, pPlayer);

            bStockMsgToGateServer(cTemp, 27);
            return;
        }
}

BOOL CGame::_bPKLog(int iAction, int iAttackerH, int iVictumH, char * pNPC)
{
    char  cTxt[1024], cTemp1[120], cTemp2[120];

    
    ZeroMemory(cTxt, sizeof(cTxt));
    ZeroMemory(cTemp1, sizeof(cTemp1));
    ZeroMemory(cTemp2, sizeof(cTemp2));

    if (m_pClientList[iVictumH] == NULL) return FALSE;

    switch (iAction)
    {

    case DEF_PKLOG_REDUCECRIMINAL:
        wsprintf(cTxt, "(%s) PC(%s)\tReduce\tCC(%d)\t%s(%d %d)\t", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, m_pClientList[iVictumH]->m_iPKCount,
            m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY);
        break;

    case DEF_PKLOG_BYPLAYER:
        if (m_pClientList[iAttackerH] == NULL) return FALSE;
        wsprintf(cTxt, "(%s) PC(%s)\tKilled by PC\t \t%s(%d %d)\t(%s) PC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName,
            m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, m_pClientList[iAttackerH]->m_cIPaddress, m_pClientList[iAttackerH]->m_cCharName);
        break;
    case DEF_PKLOG_BYPK:
        if (m_pClientList[iAttackerH] == NULL) return FALSE;
        wsprintf(cTxt, "(%s) PC(%s)\tKilled by PK\tCC(%d)\t%s(%d %d)\t(%s) PC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, m_pClientList[iAttackerH]->m_iPKCount,
            m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, m_pClientList[iAttackerH]->m_cIPaddress, m_pClientList[iAttackerH]->m_cCharName);
        break;
    case DEF_PKLOG_BYENERMY:
        if (m_pClientList[iAttackerH] == NULL) return FALSE;
        wsprintf(cTxt, "(%s) PC(%s)\tKilled by Enemy\t \t%s(%d %d)\t(%s) PC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName,
            m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, m_pClientList[iAttackerH]->m_cIPaddress, m_pClientList[iAttackerH]->m_cCharName);
        break;
    case DEF_PKLOG_BYNPC:
        if (pNPC == NULL) return FALSE;
        wsprintf(cTxt, "(%s) PC(%s)\tKilled by NPC\t \t%s(%d %d)\tNPC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName,
            m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY, pNPC);
        break;
    case DEF_PKLOG_BYOTHER:
        wsprintf(cTxt, "(%s) PC(%s)\tKilled by Other\t \t%s(%d %d)\tUnknown", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName,
            m_pClientList[iVictumH]->m_cMapName, m_pClientList[iVictumH]->m_sX, m_pClientList[iVictumH]->m_sY);
        break;
    default:
        return FALSE;
    }
    log->info(cTxt);
    return TRUE;
}

void CGame::RequestResurrectPlayer(int iClientH, bool bResurrect)
{
    short sX, sY;
    char buff[100];

    if (m_pClientList[iClientH] == NULL) return;

    sX = m_pClientList[iClientH]->m_sX;
    sY = m_pClientList[iClientH]->m_sY;

    if (bResurrect == FALSE)
    {
        m_pClientList[iClientH]->m_bIsBeingResurrected = FALSE;
        return;
    }

    if (m_pClientList[iClientH]->m_bIsBeingResurrected == FALSE)
    {
        try
        {
            wsprintf(buff, "(!!!) Player(%s) Tried To Use Resurrection Hack", m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE, TRUE, TRUE);
        }
        catch (...)
        {
        }
        return;
    }

    wsprintf(buff, "(*) Resurrect Player! %s", m_pClientList[iClientH]->m_cCharName);
    log->info(buff);


    m_pClientList[iClientH]->m_bIsKilled = FALSE;
    // Player's HP becomes half of the Max HP. 
    m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH) / 2;
    // Player's MP
    m_pClientList[iClientH]->m_iMP = ((m_pClientList[iClientH]->m_iMag * 2) + (m_pClientList[iClientH]->m_iLevel / 2)) + m_pClientList[iClientH]->m_iInt / 2;
    // Player's SP
    m_pClientList[iClientH]->m_iSP = (m_pClientList[iClientH]->m_iStr * 2) + (m_pClientList[iClientH]->m_iLevel / 2);
    // Player's Hunger
    m_pClientList[iClientH]->m_iHungerStatus = 100;

    m_pClientList[iClientH]->m_bIsBeingResurrected = FALSE;

    
    RequestTeleportHandler(iClientH, "2   ", m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
}

BOOL CGame::bCheckClientAttackFrequency(int iClientH, DWORD dwClientTime)
{
    DWORD dwTimeGap;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return FALSE;

    if (m_pClientList[iClientH]->m_dwAttackFreqTime == NULL)
        m_pClientList[iClientH]->m_dwAttackFreqTime = dwClientTime;
    else
    {
        dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwAttackFreqTime;
        m_pClientList[iClientH]->m_dwAttackFreqTime = dwClientTime;

        if (dwTimeGap < 450)
        {
            try
            {
                wsprintf(G_cTxt, "Swing Hack: (%s) Player: (%s) - attacking with weapon at irregular rates.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                log->info(G_cTxt);
                DeleteClient(iClientH, TRUE, TRUE);
            }
            catch (...)
            {
            }

            return FALSE;
        }
    }

    return FALSE;
}

BOOL CGame::bCheckClientMagicFrequency(int iClientH, DWORD dwClientTime)
{
    DWORD dwTimeGap;

    if (m_pClientList[iClientH] == NULL) return FALSE;

    if (m_pClientList[iClientH]->m_dwMagicFreqTime == NULL)
        m_pClientList[iClientH]->m_dwMagicFreqTime = dwClientTime;
    else
    {
        dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwMagicFreqTime;
        m_pClientList[iClientH]->m_dwMagicFreqTime = dwClientTime;

        if ((dwTimeGap < 1500) && (m_pClientList[iClientH]->m_bMagicConfirm == TRUE))
        {
            try
            {
                wsprintf(G_cTxt, "Speed Cast: (%s) Player: (%s) - casting magic at irregular rates. ", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                log->info(G_cTxt);
                DeleteClient(iClientH, TRUE, TRUE);
            }
            catch (...)
            {
            }
            return FALSE;
        }

        m_pClientList[iClientH]->m_iSpellCount--;
        m_pClientList[iClientH]->m_bMagicConfirm = FALSE;
        m_pClientList[iClientH]->m_bMagicPauseTime = FALSE;
    }

    return FALSE;
}

BOOL CGame::bCheckClientMoveFrequency(int iClientH, DWORD dwClientTime)
{
    DWORD dwTimeGap;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return FALSE;

    if (m_pClientList[iClientH]->m_dwMoveFreqTime == NULL)
        m_pClientList[iClientH]->m_dwMoveFreqTime = dwClientTime;
    else
    {
        if (m_pClientList[iClientH]->m_bIsMoveBlocked == TRUE)
        {
            m_pClientList[iClientH]->m_dwMoveFreqTime = NULL;
            m_pClientList[iClientH]->m_bIsMoveBlocked = FALSE;
            return FALSE;
        }

        if (m_pClientList[iClientH]->m_bIsAttackModeChange == TRUE)
        {
            m_pClientList[iClientH]->m_dwMoveFreqTime = NULL;
            m_pClientList[iClientH]->m_bIsAttackModeChange = FALSE;
            return FALSE;
        }

        dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwMoveFreqTime;
        m_pClientList[iClientH]->m_dwMoveFreqTime = dwClientTime;

        if ((dwTimeGap < 200) && (dwTimeGap >= 0))
        {
            try
            {
                wsprintf(G_cTxt, "Speed Hack: (%s) Player: (%s) - running too fast.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                log->info(G_cTxt);
                DeleteClient(iClientH, TRUE, TRUE);
            }
            catch (...)
            {
            }
            return FALSE;
        }
    }

    return FALSE;
}

void CGame::CrusadeWarStarter()
{
    SYSTEMTIME SysTime;
    int i;

    if (m_bIsCrusadeMode == TRUE) return;
    if (m_bIsCrusadeWarStarter == FALSE) return;

    GetLocalTime(&SysTime);

    for (i = 0; i < DEF_MAXSCHEDULE; i++)
        if ((m_stCrusadeWarSchedule[i].iDay == SysTime.wDayOfWeek) &&
            (m_stCrusadeWarSchedule[i].iHour == SysTime.wHour) &&
            (m_stCrusadeWarSchedule[i].iMinute == SysTime.wMinute))
        {
            log->info("(!) Automated crusade is being initiated!");
            GlobalStartCrusadeMode();
            return;
        }
}

void CGame::OnTimer()
{
    DWORD dwTime;

    dwTime = timeGetTime();

    if ((dwTime - m_dwGameTime1) > 200) {
        GameProcess();
        m_dwGameTime1 = dwTime;
    }


    if ((dwTime - m_dwGameTime2) > 3000)
    {
        CheckClientResponseTime();
        //SendMsgToGateServer(MSGID_GAMESERVERALIVE, NULL);
        CheckDayOrNightMode();
        m_dwGameTime2 = dwTime;

        if ((m_bIsGameStarted == FALSE) && (m_bIsItemAvailable == TRUE) &&
            (m_bIsNpcAvailable == TRUE) && (m_bIsMagicAvailable == TRUE) &&
            (m_bIsSkillAvailable == TRUE) && (m_bIsPortionAvailable == TRUE) &&
            (m_bIsQuestAvailable == TRUE) && (m_bIsBuildItemAvailable == TRUE))
        {
            
            log->info("Starting server.");
            m_bIsGameStarted = TRUE;
        }
    }
    if ((dwTime - m_dwGameTime6) > 1000)
    {
        DelayEventProcessor();
        SendStockMsgToGateServer();
        m_dwGameTime6 = dwTime;

        if (m_iFinalShutdownCount != 0)
        {
            m_iFinalShutdownCount--;
            wsprintf(G_cTxt, "Final Shutdown...%d", m_iFinalShutdownCount);
            log->info(G_cTxt);
            if (m_iFinalShutdownCount <= 1)
            {
                
                SendMessage(m_hWnd, WM_CLOSE, NULL, NULL);
                return;

            }
        }
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
            //bSendMsgToLS(MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA, NULL, NULL);
            //bSendMsgToLS(MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA, NULL, NULL);
        }
        m_dwGameTime5 = dwTime;

        
        srand((unsigned)time(NULL));
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

    if ((m_bHeldenianRunning == TRUE) && (m_bIsHeldenianMode == TRUE))
    {
        SetHeldenianMode();
    }
    
    if ((dwTime - m_dwCanFightzoneReserveTime) > 7200000)
    {
        FightzoneReserveProcessor();
        m_dwCanFightzoneReserveTime = dwTime;
    }

    if ((m_bIsServerShutdowned == FALSE) && (m_bOnExitProcess == TRUE) && ((dwTime - m_dwExitProcessTime) > 1000 * 2))
    {
        if (_iForcePlayerDisconect(15) == 0)
        {
            log->info("(!) GAME SERVER SHUTDOWN PROCESS COMPLETED! All players are disconnected.");
            m_bIsServerShutdowned = TRUE;

            
            if ((m_cShutDownCode == 3) || (m_cShutDownCode == 4))
            {
                log->info("(!!!) AUTO-SERVER-REBOOTING!");
                bInit();
            }
            else
            {
                
                if (m_iFinalShutdownCount == 0)	m_iFinalShutdownCount = 20;
            }
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

void CGame::OnStartGameSignal()
{
    int i;

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL)
            _bReadMapInfoFiles(i);

    bReadCrusadeStructureConfigFile("..\\GameConfigs\\Crusade.cfg");
    _LinkStrikePointMapIndex();
    bReadScheduleConfigFile("..\\GameConfigs\\Schedule.cfg");

    bReadCrusadeGUIDFile("GameData\\CrusadeGUID.txt");
    bReadApocalypseGUIDFile("GameData\\ApocalypseGUID.txt");
    bReadHeldenianGUIDFile("GameData\\HeldenianGUID.txt");

    log->info("");
    log->info("(!) Game Server Activated.");

}

void CGame::GSM_SetGuildTeleportLoc(int iGuildGUID, int dX, int dY, char * pMapName)
{
    int i, iIndex;
    DWORD dwTemp, dwTime;

    wsprintf(G_cTxt, "SetGuildTeleportLoc: %d %s %d %d", iGuildGUID, pMapName, dX, dY);
    log->info(G_cTxt);

    dwTime = timeGetTime();

    
    for (i = 0; i < DEF_MAXGUILDS; i++)
        if (m_pGuildTeleportLoc[i].m_iV1 == iGuildGUID)
        {
            if ((m_pGuildTeleportLoc[i].m_sDestX == dX) && (m_pGuildTeleportLoc[i].m_sDestY == dY) && (strcmp(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName) == 0))
            {
                m_pGuildTeleportLoc[i].m_dwTime = dwTime;
                return;
            }
            else
            {
                m_pGuildTeleportLoc[i].m_sDestX = dX;
                m_pGuildTeleportLoc[i].m_sDestY = dY;
                ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
                strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
                m_pGuildTeleportLoc[i].m_dwTime = dwTime;
                return;
            }
        }

    dwTemp = 0;
    iIndex = -1;
    for (i = 0; i < DEF_MAXGUILDS; i++)
    {
        if (m_pGuildTeleportLoc[i].m_iV1 == NULL)
        {

            m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
            m_pGuildTeleportLoc[i].m_sDestX = dX;
            m_pGuildTeleportLoc[i].m_sDestY = dY;
            ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
            strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
            m_pGuildTeleportLoc[i].m_dwTime = dwTime;
            return;
        }
        else
        {
            
            if (dwTemp < (dwTime - m_pGuildTeleportLoc[i].m_dwTime))
            {
                dwTemp = (dwTime - m_pGuildTeleportLoc[i].m_dwTime);
                iIndex = i;
            }
        }
    }

    
    if (iIndex == -1) return;

    log->info("(X) No more GuildTeleportLoc Space! Replaced.");

    m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
    m_pGuildTeleportLoc[i].m_sDestX = dX;
    m_pGuildTeleportLoc[i].m_sDestY = dY;
    ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
    strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
    m_pGuildTeleportLoc[i].m_dwTime = dwTime;
}

void CGame::_ClearExchangeStatus(int iToH)
{
    if ((iToH <= 0) || (iToH >= DEF_MAXCLIENTS)) return;
    if (m_pClientList[iToH] == NULL) return;

    if (m_pClientList[iToH]->m_cExchangeName != FALSE)
        SendNotifyMsg(NULL, iToH, DEF_NOTIFY_CANCELEXCHANGEITEM, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL);

    // m_pClientList[iToH]->m_cExchangeName    = FALSE;
    m_pClientList[iToH]->m_dwInitCCTime = FALSE;
    m_pClientList[iToH]->m_iAlterItemDropIndex = 0;
    // m_pClientList[iToH]->m_cExchangeItemIndex = -1;
    m_pClientList[iToH]->m_iExchangeH = NULL;

    m_pClientList[iToH]->m_bIsExchangeMode = FALSE;

    ZeroMemory(m_pClientList[iToH]->m_cExchangeName, sizeof(m_pClientList[iToH]->m_cExchangeName));

}

void CGame::CancelExchangeItem(int iClientH)
{
    int iExH;

    
    iExH = m_pClientList[iClientH]->m_iExchangeH;
    _ClearExchangeStatus(iExH);
    _ClearExchangeStatus(iClientH);
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

void CGame::SetForceRecallTime(int iClientH)
{
    int iTL_ = 0;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == NULL) return;

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
            case 1:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeMonday; break;
            case 2:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeTuesday; break;
            case 3:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeWednesday; break;
            case 4:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeThursday; break;
            case 5:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeFriday; break;
            case 6:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeSaturday; break;
            case 0:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeSunday; break;
            }
        }
    }
    else
    {
        // if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0) 
        if (m_sForceRecallTime > 0)
        {
            iTL_ = 20 * m_sForceRecallTime;
        }
        else
        {

            
            GetLocalTime(&SysTime);
            switch (SysTime.wDayOfWeek)
            {
                case 1:	iTL_ = 20 * m_sRaidTimeMonday; break;
                case 2:	iTL_ = 20 * m_sRaidTimeTuesday; break;
                case 3:	iTL_ = 20 * m_sRaidTimeWednesday; break;
                case 4:	iTL_ = 20 * m_sRaidTimeThursday; break;
                case 5:	iTL_ = 20 * m_sRaidTimeFriday; break;
                case 6:	iTL_ = 20 * m_sRaidTimeSaturday; break;
                case 0:	iTL_ = 20 * m_sRaidTimeSunday; break;
            }
        }

        if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > iTL_)
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;

    }

    m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
    return;
}

void CGame::CheckForceRecallTime(int iClientH)
{
    SYSTEMTIME SysTime;
    int iTL_;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return;

    if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0)
    {
        // has admin set a recall time ??
        if (m_sForceRecallTime > 0)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sForceRecallTime * 20;
        }
        // use standard recall time calculations
        else
        {
            GetLocalTime(&SysTime);
            switch (SysTime.wDayOfWeek)
            {
            case 1:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeMonday; break;
            case 2:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeTuesday; break;
            case 3:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeWednesday; break;
            case 4:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeThursday; break;
            case 5:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeFriday; break;
            case 6:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeSaturday; break;
            case 0:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * m_sRaidTimeSunday; break;
            }
        }
    }
    else
    {
        // has admin set a recall time ??
        if (m_sForceRecallTime > 0)
        {
            iTL_ = m_sForceRecallTime * 20;
        }
        // use standard recall time calculations
        else
        {
            GetLocalTime(&SysTime);
            switch (SysTime.wDayOfWeek)
            {
                case 1:	iTL_ = 20 * m_sRaidTimeMonday; break;
                case 2:	iTL_ = 20 * m_sRaidTimeTuesday; break;
                case 3:	iTL_ = 20 * m_sRaidTimeWednesday; break;
                case 4:	iTL_ = 20 * m_sRaidTimeThursday; break;
                case 5:	iTL_ = 20 * m_sRaidTimeFriday; break;
                case 6:	iTL_ = 20 * m_sRaidTimeSaturday; break;
                case 0:	iTL_ = 20 * m_sRaidTimeSunday; break;
            }
        }
        if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > iTL_)
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = iTL_;
    }

    m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
    return;

}

int ITEMSPREAD_FIXED_COORD[25][2] =
{
    { 0,  0},	//1
    { 1,  0},	//2
    { 1,  1},	//3
    { 0,  1},	//4
    {-1,  1},	//5
    {-1,  0},	//6
    {-1, -1},	//7
    { 0, -1},	//8
    { 1, -1},	//9
    { 2, -1},	//10
    { 2,  0},	//11
    { 2,  1},	//12
    { 2,  2},	//13
    { 1,  2},	//14
    { 0,  2},	//15
    {-1,  2},	//16
    {-2,  2},	//17
    {-2,  1},	//18
    {-2,  0},	//19
    {-2, -1},	//20
    {-2, -2},	//21
    {-1, -2},	//22
    { 0, -2},	//23
    { 1, -2},	//24
    { 2, -2},	//25
};

BOOL CGame::bGetMultipleItemNamesWhenDeleteNpc(short sNpcType, int iProbability, int iMin, int iMax, short sBaseX, short sBaseY,
    int iItemSpreadType, int iSpreadRange,
    int * iItemIDs, POINT * BasePos, int * iNumItem)
{
    int		iProb = 100;
    float	fProb, fProbA, fProbB, fProbC;
    int		iItemID;
    int		iNum = 0;

    for (int i = 0; i < iMax; i++)
    {
        if (i > iMin) iProb = iProbability;

        fProb = (float)(100 - iProb) / 10.0;	
        if (fProb < 1.0) fProb = 1.0;

        fProbA = fProbB = fProbC = fProb;

        fProbA = fProbA * 8.0f;
        fProbB = fProbB * 4.0f;
        fProbC = fProbC;

        iItemID = 0;

        switch (sNpcType)
        {
        case 69: // Wyvern
            
            switch (iDice(1, 4))
            {
            case 1: if (iDice(1, (6000 * fProbA)) == 3) iItemID = 634; break; // RingofWizard
            case 2: if (iDice(1, (5000 * fProbA)) == 3) iItemID = 636; break; // RingofGrandMage
            case 3: if (iDice(1, (3000 * fProbA)) == 2) iItemID = 614; break; // SwordofIceElemental
            case 4: if (iDice(1, (4500 * fProbA)) == 3) iItemID = 380; break; // IceStormManual
            }

            if (iItemID == 0)
            {
                switch (iDice(1, 6))
                {
                case  1: if (iDice(1, (500 * fProbB)) == 2) iItemID = 642; break; // KnecklaceOfIcePro
                case  2: if (iDice(1, (2000 * fProbB)) == 2) iItemID = 643; break; // KnecklaceOfIceEle
                case  3: if (iDice(1, (1000 * fProbB)) == 3) iItemID = 943; break; // IceAxe
                case  4: if (iDice(1, (1500 * fProbB)) == 3) iItemID = 734; break; // RingOfArcmage
                case  5: if (iDice(1, (500 * fProbB)) == 3) iItemID = 942; break; // IceHammer
                case  6: if (iDice(1, (500 * fProbB)) == 2) iItemID = 738; break; // BerserkWand(MS.20)
                }
            }
            break;

        case 73: // Fire-Wyvern
            
            switch (iDice(1, 7))
            {
            case  1: if (iDice(1, (5000 * fProbA)) == 3) iItemID = 860; break; // NecklaceOfXelima
            case  2: if (iDice(1, (3000 * fProbA)) == 2) iItemID = 630; break; // RingoftheXelima
            case  3: if (iDice(1, (3000 * fProbA)) == 2) iItemID = 738; break; // BerserkWand(MS.20)
            case  4: if (iDice(1, (3000 * fProbA)) == 2) iItemID = 735; break; // RingOfDragonpower
            case  5: if (iDice(1, (3000 * fProbA)) == 2) iItemID = 20; break; // Excaliber
            case  6: if (iDice(1, (3000 * fProbA)) == 3) iItemID = 382; break; // BloodyShockW.Manual
            case  7: if (iDice(1, (3000 * fProbA)) == 3) iItemID = 381; break; // MassFireStrikeManual
            }

            if (iItemID == 0)
            {
                switch (iDice(1, 9))
                {
                case  1: if (iDice(1, (1000 * fProbB)) == 2) iItemID = 645; break; // KnecklaceOfEfreet
                case  2: if (iDice(1, (500 * fProbB)) == 2) iItemID = 638; break; // KnecklaceOfFirePro
                case  3: if (iDice(1, (1000 * fProbB)) == 3) iItemID = 636; break; // RingofGrandMage
                case  4: if (iDice(1, (800 * fProbB)) == 3) iItemID = 734; break; // RingOfArcmage
                case  5: if (iDice(1, (500 * fProbB)) == 3) iItemID = 634; break; // RingofWizard
                case  6: if (iDice(1, (500 * fProbB)) == 2) iItemID = 290; break; // Flameberge+3(LLF)
                case  7: if (iDice(1, (500 * fProbB)) == 3) iItemID = 490; break; // BloodSword
                case  8: if (iDice(1, (500 * fProbB)) == 3) iItemID = 491; break; // BloodAxe
                case  9: if (iDice(1, (500 * fProbB)) == 3) iItemID = 492; break; // BloodRapier
                }
            }

            break;

        case 81: // Abaddon
            switch (iDice(1, 6))
            {
            case 1: if (iDice(1, (100 * fProbA)) == 3) iItemID = 20;  break; // Excaliber
            case 2: if (iDice(1, (100 * fProbA)) == 3) iItemID = 647; break; // NecklaceOfStoneGol
            case 3: if (iDice(1, (100 * fProbA)) == 3) iItemID = 860; break; // NecklaceOfXelima
            case 4: if (iDice(1, (100 * fProbA)) == 3) iItemID = 936; break; // MerienHat
            case 5: if (iDice(1, (100 * fProbA)) == 2) iItemID = 631; break; // RingoftheAbaddon
            case 6: if (iDice(1, (100 * fProbA)) == 2) iItemID = 937; break; // MerienHelm
            }

            if (iItemID == 0)
            {
                switch (iDice(1, 15))
                {
                case  1: if (iDice(1, (4 * fProbB)) == 3) iItemID = 650; break; // ZemstoneOfSacrifice
                case  2: if (iDice(1, (4 * fProbB)) == 3) iItemID = 490; break; // BloodSword
                case  3: if (iDice(1, (4 * fProbB)) == 3) iItemID = 491; break; // BloodAxe
                case  4: if (iDice(1, (4 * fProbB)) == 3) iItemID = 492; break; // BloodRapier
                case  5: if (iDice(1, (4 * fProbB)) == 3) iItemID = 611; break; // XelimaAxe
                case  6: if (iDice(1, (4 * fProbB)) == 3) iItemID = 610; break; // XelimaBlade
                case  7: if (iDice(1, (4 * fProbB)) == 3) iItemID = 612; break; // XelimaRapier
                case 10: if (iDice(1, (4 * fProbB)) == 3) iItemID = 645; break; // KnecklaceOfEfreet
                case 11: if (iDice(1, (4 * fProbB)) == 3) iItemID = 638; break; // BloodKlonessAxe
                case 12: if (iDice(1, (4 * fProbB)) == 3) iItemID = 382; break; // BloodyShockW.Manual
                case 13: if (iDice(1, (4 * fProbB)) == 3) iItemID = 381; break; // MassFireStrikeManual
                case 14: if (iDice(1, (4 * fProbB)) == 3) iItemID = 259; break; // MagicWand(M.Shield)
                case 15: if (iDice(1, (4 * fProbB)) == 3) iItemID = 947; break; // DragonStaff(MS.40)
                }
            }
            break;
        }

        if (iItemID == 0)
        {
            switch (iDice(1, 24))
            {
            case  1: if (iDice(1, (2 * fProbC)) == 2) iItemID = 740; break; // BagOfGold-medium
            case  2: if (iDice(1, (2 * fProbC)) == 2) iItemID = 741; break; // BagOfGold-large
            case  3: if (iDice(1, (2 * fProbC)) == 2) iItemID = 742; break; // BagOfGold-largest
            case  4:
            case  5:
            case  6:
            case  7: if (iDice(1, (2 * fProbC)) == 2) iItemID = 650; break; // ZemstoneOfSacrifice
            case  8:
            case  9: if (iDice(1, (2 * fProbC)) == 2) iItemID = 656; break; // StoneOfXelima
            case 10:
            case 11:
            case 12: if (iDice(1, (2 * fProbC)) == 2) iItemID = 657; break; // StoneOfMerien
            case 13:
            case 14:
            case 15: if (iDice(1, (2 * fProbC)) == 2) iItemID = 335; break; // EmeraldRing
            case 16:
            case 17:
            case 18: if (iDice(1, (2 * fProbC)) == 2) iItemID = 290; break; // Flameberge+3(LLF)
            case 19:
            case 20: if (iDice(1, (2 * fProbC)) == 2) iItemID = 259; break; // MagicWand(M.Shield)
            case 21: if (iDice(1, (2 * fProbC)) == 2) iItemID = 300; break; // MagicNecklace(RM10)
            case 22: if (iDice(1, (2 * fProbC)) == 2) iItemID = 311; break; // MagicNecklace(DF+10)
            case 23: if (iDice(1, (2 * fProbC)) == 2) iItemID = 305; break; // MagicNecklace(DM+1)
            case 24: if (iDice(1, (2 * fProbC)) == 2) iItemID = 308; break; // MagicNecklace(MS10)
            }
        }

        if (iItemID == 0 && iProb == 100) iItemID = 90; // Gold

        if (iItemID != 0)
        {
            iItemIDs[iNum] = iItemID;

            switch (iItemSpreadType)
            {
            case DEF_ITEMSPREAD_RANDOM:
                BasePos[iNum].x = sBaseX + iSpreadRange - iDice(1, iSpreadRange * 2);
                BasePos[iNum].y = sBaseY + iSpreadRange - iDice(1, iSpreadRange * 2);
                break;

            case DEF_ITEMSPREAD_FIXED:
                BasePos[iNum].x = sBaseX + ITEMSPREAD_FIXED_COORD[iNum][0];
                BasePos[iNum].y = sBaseY + ITEMSPREAD_FIXED_COORD[iNum][1];
                break;
            }
            iNum++;
        }

    }

    *iNumItem = iNum;

    return TRUE;
}

void CGame::SendCollectedMana()
{
    char * cp, cData[120];
    WORD * wp;

    if ((m_iCollectedMana[1] == 0) && (m_iCollectedMana[2] == 0)) return;

    wsprintf(G_cTxt, "Sending Collected Mana: %d %d", m_iCollectedMana[1], m_iCollectedMana[2]);
    log->info(G_cTxt);

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)(cData);
    *cp = GSM_COLLECTEDMANA;
    cp++;

    wp = (WORD *)cp;
    *wp = (WORD)m_iCollectedMana[1];
    cp += 2;

    wp = (WORD *)cp;
    *wp = (WORD)m_iCollectedMana[2];
    cp += 2;

    CollectedManaHandler(m_iCollectedMana[1], m_iCollectedMana[2]);

    //bStockMsgToGateServer(cData, 5);

    m_iCollectedMana[0] = 0;
    m_iCollectedMana[1] = 0;
    m_iCollectedMana[2] = 0;
}

void CGame::_LinkStrikePointMapIndex()
{
    int i, z, x;

    for (i = 0; i < DEF_MAXMAPS; i++)
        if ((m_pMapList[i] != NULL) && (m_pMapList[i]->m_iTotalStrikePoints != 0))
        {
            for (z = 0; z < DEF_MAXSTRIKEPOINTS; z++)
                if (strlen(m_pMapList[i]->m_stStrikePoint[z].cRelatedMapName) != 0)
                {
                    for (x = 0; x < DEF_MAXMAPS; x++)
                        if ((m_pMapList[x] != NULL) && (strcmp(m_pMapList[x]->m_cName, m_pMapList[i]->m_stStrikePoint[z].cRelatedMapName) == 0))
                        {
                            m_pMapList[i]->m_stStrikePoint[z].iMapIndex = x;
                            wsprintf(G_cTxt, "(!) Map(%s) Strike Point(%d) Related Map(%s) Index(%d)", m_pMapList[i]->m_cName, z, m_pMapList[i]->m_stStrikePoint[z].cRelatedMapName, x);
                            log->info(G_cTxt);

                            goto LSPMI_LOOPBREAK;
                        }
                    LSPMI_LOOPBREAK:;
                }
        }
}

BOOL CGame::_bRegisterMap(char * pName)
{
    int i;
    char cTmpName[11], cTxt[120];

    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, pName);
    for (i = 0; i < DEF_MAXMAPS; i++)
        if ((m_pMapList[i] != NULL) && (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0))
        {
            wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map (%s) is already installed! cannot add.", cTmpName);
            log->info(cTxt);
            return FALSE;
        }

    for (i = 1; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] == NULL)
        {
            m_pMapList[i] = new CMap(this, i);
            wsprintf(cTxt, "(*) Add map (%s)   - Loading map info files...", pName);
            log->info(cTxt);
            if (m_pMapList[i]->bInit(pName) == FALSE)
            {
                wsprintf(cTxt, "(!!!) Data file loading fail!", pName);
                log->info(cTxt);
                return FALSE;
            };

            log->info("(*) Data file loading success.");

            if ((m_iMiddlelandMapIndex == -1) && (strcmp("middleland", pName) == 0))
                m_iMiddlelandMapIndex = i;

            if ((m_iAresdenMapIndex == -1) && (strcmp("aresden", pName) == 0))
                m_iAresdenMapIndex = i;

            if ((m_iElvineMapIndex == -1) && (strcmp("elvine", pName) == 0))
                m_iElvineMapIndex = i;

            if ((m_iBTFieldMapIndex == -1) && (strcmp("BtField", pName) == 0))
                m_iBTFieldMapIndex = i;

            if ((m_iGodHMapIndex == -1) && (strcmp("GodH", pName) == 0))
                m_iGodHMapIndex = i;

            m_iTotalMaps++;
            return TRUE;
        }

    wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map (%s) canot be added - no more map space.", pName);
    log->info(cTxt);
    return FALSE;
}

BOOL CGame::bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH, BOOL bBonus)
{
    int iValue, iProb, iResult;

    if (m_pClientList[iClientH]->m_pItemList[iSomH] == NULL) return FALSE;

    iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x0F0000000) >> 28;

    switch (iValue)
    {
    case 0: iProb = 30; break;  // +1 :90%     +1~+2
    case 1: iProb = 25; break;  // +2 :80%      +3
    case 2: iProb = 20; break;  // +3 :48%      +4 
    case 3: iProb = 15; break;  // +4 :24%      +5
    case 4: iProb = 10; break;  // +5 :9.6%     +6
    case 5: iProb = 10; break;  // +6 :2.8%     +7
    case 6: iProb = 8; break;  // +7 :0.57%    +8
    case 7: iProb = 8; break;  // +8 :0.05%    +9
    case 8: iProb = 5; break;  // +9 :0.004%   +10
    case 9: iProb = 3; break;  // +10:0.00016%
    default: iProb = 1; break;
    }

    if (((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 > 100))
    {
        if (iProb > 20)
            iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 10);
        else if (iProb > 7)
            iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 20);
        else
            iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 40);
    }
    if (bBonus == TRUE) iProb *= 2;

    iProb *= 100;
    iResult = iDice(1, 10000);

    if (iProb >= iResult)
    {
        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
        return TRUE;
    }

    _bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);

    return FALSE;
}

void CGame::ShowClientMsg(int iClientH, char * pMsg)
{
    char * cp, cTemp[256];
    DWORD * dwp;
    std::size_t dwMsgSize;
    WORD * wp;
    short * sp;

    ZeroMemory(cTemp, sizeof(cTemp));

    dwp = (DWORD *)cTemp;
    *dwp = MSGID_COMMAND_CHATMSG;

    wp = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
    *wp = NULL;

    cp = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
    sp = (short *)cp;
    *sp = NULL;
    cp += 2;

    sp = (short *)cp;
    *sp = NULL;
    cp += 2;

    memcpy(cp, "HGServer", 8);
    cp += 10;

    *cp = 10;
    cp++;

    dwMsgSize = strlen(pMsg);
    if (dwMsgSize > 50) dwMsgSize = 50;
    memcpy(cp, pMsg, dwMsgSize);
    cp += dwMsgSize;

    m_pClientList[iClientH]->iSendMsg(cTemp, dwMsgSize + 22);
}

void CGame::Command_YellowBall(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cPlayerName[11], cMapName[32];
    CStrTok * pStrTok;
    char * cp;
    WORD * wp;
    int iSoxH, iSoX, i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    iSoX = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
        {
            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
            {
            case 653: iSoX++; iSoxH = i; break;
            }
        }
    if (iSoX > 0)
    {

        ZeroMemory(cPlayerName, sizeof(cPlayerName));
        ZeroMemory(cBuff, sizeof(cBuff));
        memcpy(cBuff, pData, dwMsgSize);

        pStrTok = new CStrTok(cBuff, seps);
        token = pStrTok->pGet();

        token = pStrTok->pGet();
        if (token == NULL)
        {
            delete pStrTok;
            return;
        }

        if (strlen(token) > 10)
        {
            memcpy(cPlayerName, token, 10);
        }
        else
        {
            memcpy(cPlayerName, token, strlen(token));
        }

        for (int i = 1; i < DEF_MAXCLIENTS; i++)
        {
            if (m_pClientList[i] != NULL)
            {
                if (memcmp(cPlayerName, m_pClientList[i]->m_cCharName, 10) == 0)
                {
                    if ((m_pClientList[iClientH]->m_cLocation) != (m_pClientList[i]->m_cLocation)) return;
                    ZeroMemory(cMapName, sizeof(cMapName));
                    strcpy(cMapName, m_pClientList[i]->m_cMapName);
                    wsprintf(G_cTxt, "(%s) Player: (%s) - YellowBall MapName(%s)(%d %d)", m_pClientList[i]->m_cIPaddress, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cMapName, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                    log->info(G_cTxt);
                    ItemDepleteHandler(iClientH, iSoxH, TRUE);
                    RequestTeleportHandler(iClientH, "2   ", cMapName, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                    delete pStrTok;
                    return;
                }
            }
        }
        m_pClientList[iClientH]->m_bIsAdminOrderGoto = TRUE;

        ZeroMemory(cBuff, sizeof(cBuff));

        cp = (char *)cBuff;
        *cp = GSM_REQUEST_FINDCHARACTER;
        cp++;

        wp = (WORD *)cp;
        *wp = m_wServerID_GSS;
        cp += 2;

        wp = (WORD *)cp;
        *wp = iClientH;
        cp += 2;

        memcpy(cp, cPlayerName, 10);
        cp += 10;

        memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
        cp += 10;

        bStockMsgToGateServer(cBuff, 25);

        delete pStrTok;
    }
}

void CGame::Command_RedBall(int iClientH, char * pData, DWORD dwMsgSize)
{
    char seps[] = "= \t\n", cName[30], cNpcName[30], cNpcWaypoint[11];
    int iNamingValue, tX, tY, i, x, iNpcID;
    int iSoxH, iSoX;

    if (m_pClientList[iClientH] == NULL) return;
    if ((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone1", 9) != 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone2", 9) != 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone3", 9) != 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone4", 9) != 0)) return;

    iSoX = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
        {
            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
            {
            case 652: iSoX++; iSoxH = i; break;
            }
        }
    if (iSoX > 0)
    {
        iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue == -1)
        {

        }
        else
        {

            ZeroMemory(cNpcName, sizeof(cNpcName));
            switch (iDice(1, 5))
            {
            case 1: strcpy(cNpcName, "Wyvern"); iNpcID = 66; break;
            case 2: strcpy(cNpcName, "Hellclaw"); iNpcID = 49; break;
            case 3: strcpy(cNpcName, "Fire-Wyvern"); iNpcID = 73; break;
            case 4: strcpy(cNpcName, "Tigerworm"); iNpcID = 50; break;
            case 5: strcpy(cNpcName, "Gagoyle"); iNpcID = 52; break;
            }
            ZeroMemory(cName, sizeof(cName));
            wsprintf(cName, "XX%d", iNamingValue);
            cName[0] = '_';
            cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

            ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));

            tX = (int)m_pClientList[iClientH]->m_sX;
            tY = (int)m_pClientList[iClientH]->m_sY;
            if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, (rand() % 9),
                DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, NULL, NULL, -1, FALSE, FALSE) == FALSE)
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
            }
            else
            {
                wsprintf(G_cTxt, "(%s) Player: (%s) - RedBall used %s [%s(%d, %d)] ", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, cNpcName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, tX, tY);
                log->info(G_cTxt);
            }
        }

        for (x = 1; x < DEF_MAXCLIENTS; x++)
            if ((m_pClientList[x] != NULL) && (m_pClientList[x]->m_bIsInitComplete == TRUE))
            {
                SendNotifyMsg(NULL, x, DEF_NOTIFY_SPAWNEVENT, tX, tY, iNpcID, NULL, NULL, NULL);
            }
        ItemDepleteHandler(iClientH, iSoxH, TRUE);
    }
}

void CGame::Command_BlueBall(int iClientH, char * pData, DWORD dwMsgSize)

{
    char seps[] = "= \t\n";
    char   cName_Master[10], cName_Slave[10], cNpcName[256], cWaypoint[11], cSA;
    int    pX, pY, j, iNum, iNamingValue, iNpcID;
    int i, x;
    BOOL   bMaster;
    int iSoxH, iSoX;

    if (m_pClientList[iClientH] == NULL) return;
    if ((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone1", 9) != 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone2", 9) != 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone3", 9) != 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "huntzone4", 9) != 0)) return;


    iSoX = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
        {
            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
            {
            case 654: iSoX++; iSoxH = i; break;
            }
        }
    if (iSoX > 0)
    {
        iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue == -1)
        {

        }
        else
        {

            ZeroMemory(cNpcName, sizeof(cNpcName));

            switch (iDice(1, 38))
            {
            case 1: strcpy(cNpcName, "Slime");			iNpcID = 10; break;
            case 2: strcpy(cNpcName, "Giant-Ant");		iNpcID = 15; break;
            case 3: strcpy(cNpcName, "Zombie");			iNpcID = 17; break;
            case 4: strcpy(cNpcName, "Scorpion");		iNpcID = 16; break;
            case 5: strcpy(cNpcName, "Skeleton");		iNpcID = 11; break;
            case 6: strcpy(cNpcName, "Orc-Mage");		iNpcID = 14; break;
            case 7: strcpy(cNpcName, "Clay-Golem");		iNpcID = 23; break;
            case 8: strcpy(cNpcName, "Stone-Golem");	iNpcID = 12; break;
            case 9: strcpy(cNpcName, "Hellbound");		iNpcID = 27; break;
            case 10: strcpy(cNpcName, "Giant-Frog");	iNpcID = 57; break;
            case 11: strcpy(cNpcName, "Troll");			iNpcID = 28; break;
            case 12: strcpy(cNpcName, "Cyclops");		iNpcID = 13; break;
            case 13: strcpy(cNpcName, "Ice-Golem");		iNpcID = 65; break;
            case 14: strcpy(cNpcName, "Beholder");		iNpcID = 53; break;
            case 15: strcpy(cNpcName, "Cannibal-Plant"); iNpcID = 60; break;
            case 16: strcpy(cNpcName, "Orge");			iNpcID = 29; break;
            case 17: strcpy(cNpcName, "Mountain-Giant"); iNpcID = 58; break;
            case 18: strcpy(cNpcName, "DireBoar");		iNpcID = 62; break;
            case 19: strcpy(cNpcName, "Liche");			iNpcID = 30; break;
            case 20: strcpy(cNpcName, "Stalker");		iNpcID = 48; break;
            case 21: strcpy(cNpcName, "WereWolf");		iNpcID = 33; break;
            case 22: strcpy(cNpcName, "Dark-Elf");		iNpcID = 54; break;
            case 23: strcpy(cNpcName, "Frost");			iNpcID = 63; break;
            case 24: strcpy(cNpcName, "Orc");			iNpcID = 14; break;
            case 25: strcpy(cNpcName, "Ettin");			iNpcID = 59; break;
            case 26: strcpy(cNpcName, "Tentocle");		iNpcID = 80; break;
            case 27: strcpy(cNpcName, "Giant-Crayfish"); iNpcID = 74; break;
            case 28: strcpy(cNpcName, "Giant-Plant");	iNpcID = 76; break;
            case 29: strcpy(cNpcName, "Rudolph");		iNpcID = 61; break;
            case 30: strcpy(cNpcName, "Claw-Turtle");	iNpcID = 72; break;
            case 31: strcpy(cNpcName, "Centaurus");		iNpcID = 71; break;
            case 32: strcpy(cNpcName, "Barlog");		iNpcID = 70; break;
            case 33: strcpy(cNpcName, "Giant-Lizard");	iNpcID = 75; break;
            case 34: strcpy(cNpcName, "MasterMage-Orc"); iNpcID = 77; break;
            case 35: strcpy(cNpcName, "Minotaurs");		iNpcID = 78; break;
            case 36: strcpy(cNpcName, "Unicorn");		iNpcID = 32; break;
            case 37: strcpy(cNpcName, "Nizie");			iNpcID = 79; break;
            }

            iNum = 10;
            cSA = 0;
            pX = m_pClientList[iClientH]->m_sX;
            pY = m_pClientList[iClientH]->m_sY;

            wsprintf(G_cTxt, "(!) BlueBallEvent: SummonMob (%s)-(%d)", cNpcName, iNum);
            log->info(G_cTxt);

            iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
            if (iNamingValue != -1)
            {

                ZeroMemory(cName_Master, sizeof(cName_Master));
                wsprintf(cName_Master, "XX%d", iNamingValue);
                cName_Master[0] = '_';
                cName_Master[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

                if ((bMaster = bCreateNewNpc(cNpcName, cName_Master, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, FALSE, TRUE)) == FALSE)
                {

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                }
            }

            for (j = 0; j < (iNum - 1); j++)
            {
                iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
                if (iNamingValue != -1)
                {
                    // Slave Mob
                    ZeroMemory(cName_Slave, sizeof(cName_Slave));
                    wsprintf(cName_Slave, "XX%d", iNamingValue);
                    cName_Slave[0] = '_';
                    cName_Slave[1] = m_pClientList[iClientH]->m_cMapIndex + 65;



                    if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, FALSE) == FALSE)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                    }
                    else
                    {
                        // Slave
                        bSetNpcFollowMode(cName_Slave, cName_Master, DEF_OWNERTYPE_NPC);
                    }
                }
            }
        }
    }

    for (x = 1; x < DEF_MAXCLIENTS; x++)
        if ((m_pClientList[x] != NULL) && (m_pClientList[x]->m_bIsInitComplete == TRUE))
        {
            SendNotifyMsg(NULL, x, DEF_NOTIFY_SPAWNEVENT, pX, pY, iNpcID, NULL, NULL, NULL);
        }
    ItemDepleteHandler(iClientH, iSoxH, TRUE);
}

void CGame::TimeHitPointsUp(int iClientH)
{
    int iMaxHP, iTemp, iTotal;
    double dV1, dV2, dV3;

    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;

    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;

    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == TRUE) return;

    iMaxHP = iGetMaxHP(iClientH);

    if (m_pClientList[iClientH]->m_iHP < iMaxHP)
    {

        iTemp = iDice(1, (m_pClientList[iClientH]->m_iVit));

        if (iTemp < (m_pClientList[iClientH]->m_iVit / 2)) iTemp = (m_pClientList[iClientH]->m_iVit / 2);

        if (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown != 0)

            iTemp -= (iTemp / m_pClientList[iClientH]->m_iSideEffect_MaxHPdown);

        iTotal = iTemp + m_pClientList[iClientH]->m_iHPstock;

        if (m_pClientList[iClientH]->m_iAddHP != 0)
        {
            dV2 = (double)iTotal;
            dV3 = (double)m_pClientList[iClientH]->m_iAddHP;
            dV1 = (dV3 / 100.0f) * dV2;
            iTotal += (int)dV1;
        }

        m_pClientList[iClientH]->m_iHP += iTotal;

        if (m_pClientList[iClientH]->m_iHP > iMaxHP) m_pClientList[iClientH]->m_iHP = iMaxHP;

        if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 0;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
    }
    m_pClientList[iClientH]->m_iHPstock = 0;
}

char CGame::_cCheckHeroItemEquipped(int iClientH)
{
    short sHeroLeggings, sHeroHauberk, sHeroArmor, sHeroHelm;

    if (m_pClientList[iClientH] == NULL) return 0;

    sHeroHelm = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
    sHeroArmor = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
    sHeroHauberk = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
    sHeroLeggings = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];

    if ((sHeroHelm < 0) || (sHeroLeggings < 0) || (sHeroArmor < 0) || (sHeroHauberk < 0)) return 0;

    if (m_pClientList[iClientH]->m_pItemList[sHeroHelm] == NULL) return 0;
    if (m_pClientList[iClientH]->m_pItemList[sHeroLeggings] == NULL) return 0;
    if (m_pClientList[iClientH]->m_pItemList[sHeroArmor] == NULL) return 0;
    if (m_pClientList[iClientH]->m_pItemList[sHeroHauberk] == NULL) return 0;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 403) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 411) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 419) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 423)) return 1;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 407) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 415) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 419) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 423)) return 2;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 404) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 412) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 420) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 424)) return 1;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 408) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 416) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 420) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 424)) return 2;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 405) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 413) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 421) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 425)) return 1;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 409) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 417) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 421) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 425)) return 2;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 406) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 414) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 422) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 426)) return 1;

    if ((m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 410) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 418) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 422) &&
        (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 426)) return 2;

    return 0;
}

BOOL CGame::bPlantSeedBag(int iMapIndex, int dX, int dY, int iItemEffectValue1, int iItemEffectValue2, int iClientH)
{
    int iNamingValue, tX, tY;
    short sOwnerH;
    char cOwnerType, cNpcName[30], cName[30], cNpcWaypointIndex[11];
    BOOL bRet;

    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAgriculture >= 200)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOMOREAGRICULTURE, NULL, NULL, NULL, NULL);
        return FALSE;
    }

    if (iItemEffectValue2 > m_pClientList[iClientH]->m_cSkillMastery[2])
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_AGRICULTURESKILLLIMIT, NULL, NULL, NULL, NULL);
        return FALSE;
    }

    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();

    if (iNamingValue == -1)
    {
    }
    else
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
        if (sOwnerH != NULL && sOwnerH == DEF_OWNERTYPE_NPC && m_pNpcList[sOwnerH]->m_cActionLimit == 5)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_AGRICULTURENOAREA, NULL, NULL, NULL, NULL);
            return FALSE;
        }
        else
        {
            if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetIsFarm(dX, dY) == FALSE)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_AGRICULTURENOAREA, NULL, NULL, NULL, NULL);
                return FALSE;
            }

            ZeroMemory(cNpcName, sizeof(cNpcName));
            strcpy(cNpcName, "Crops");
            ZeroMemory(cName, sizeof(cName));
            wsprintf(cName, "XX%d", iNamingValue);
            cName[0] = '_';
            cName[1] = iMapIndex + 65;

            ZeroMemory(cNpcWaypointIndex, sizeof(cNpcWaypointIndex));
            tX = dX;
            tY = dY;

            bRet = bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypointIndex, NULL, NULL, 0, FALSE, TRUE);
            if (bRet == FALSE)
            {
                m_pMapList[iMapIndex]->SetNamingValueEmpty(iNamingValue);
            }
            else
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                if (m_pNpcList[sOwnerH] == NULL) return 0;
                m_pNpcList[sOwnerH]->m_cCropType = iItemEffectValue1;
                switch (iItemEffectValue1)
                {
                case 1: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 2: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 3: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 4: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 5: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 6: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 7: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 8: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 9: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 10: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 11: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 12: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                case 13: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
                default: m_pNpcList[sOwnerH]->m_cCropSkill = 100; break;
                }
                m_pNpcList[sOwnerH]->m_sAppr2 = 1;
                SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, NULL, NULL, NULL);
                wsprintf(G_cTxt, "(skill:%d type:%d)plant(%s) Agriculture begin(%d,%d) sum(%d)!", m_pNpcList[sOwnerH]->m_cCropSkill, m_pNpcList[sOwnerH]->m_cCropType, cNpcName, tX, tY, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAgriculture);
                log->info(G_cTxt);
                return TRUE;
            }
        }
    }
    return FALSE;
}

void CGame::_CheckFarmingAction(short sAttackerH, short sTargetH, BOOL bType)
{
    char cCropType;
    int iItemID;
    CItem * pItem;

    iItemID = 0;
    cCropType = 0;

    cCropType = m_pNpcList[sTargetH]->m_cCropType;
    switch (cCropType)
    {
    case 1: GetExp(sAttackerH, iDice(3, 10)); iItemID = 820; break; // WaterMelon
    case 2: GetExp(sAttackerH, iDice(3, 10)); iItemID = 821; break; // Pumpkin
    case 3: GetExp(sAttackerH, iDice(4, 10)); iItemID = 822; break; // Garlic
    case 4: GetExp(sAttackerH, iDice(4, 10)); iItemID = 823; break; // Barley
    case 5: GetExp(sAttackerH, iDice(5, 10)); iItemID = 824; break; // Carrot
    case 6: GetExp(sAttackerH, iDice(5, 10)); iItemID = 825; break; // Radish
    case 7: GetExp(sAttackerH, iDice(6, 10)); iItemID = 826; break; // Corn
    case 8: GetExp(sAttackerH, iDice(6, 10)); iItemID = 827; break; // ChineseBellflower
    case 9: GetExp(sAttackerH, iDice(7, 10)); iItemID = 828; break; // Melone
    case 10: GetExp(sAttackerH, iDice(7, 10)); iItemID = 829; break; // Tommato
    case 11: GetExp(sAttackerH, iDice(8, 10)); iItemID = 830; break; // Grapes
    case 12: GetExp(sAttackerH, iDice(8, 10)); iItemID = 831; break; // BlueGrapes
    case 13: GetExp(sAttackerH, iDice(9, 10)); iItemID = 832; break; // Mushroom
    default: GetExp(sAttackerH, iDice(10, 10)); iItemID = 721; break; // Ginseng

    }

    pItem = new CItem;
    if (_bInitItemAttr(pItem, iItemID) == FALSE)
    {
        delete pItem;
    }
    if (bType == 0)
    {
        m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->bSetItem(m_pClientList[sAttackerH]->m_sX, m_pClientList[sAttackerH]->m_sY, pItem);
        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[sAttackerH]->m_cMapIndex,
            m_pClientList[sAttackerH]->m_sX, m_pClientList[sAttackerH]->m_sY, pItem->m_sSprite,
            pItem->m_sSpriteFrame, pItem->m_cItemColor, FALSE);
    }
    else if (bType == 1)
    {
        m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bSetItem(m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY, pItem);
        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[sTargetH]->m_cMapIndex,
            m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY, pItem->m_sSprite,
            pItem->m_sSpriteFrame, pItem->m_cItemColor, FALSE);
    }

}

/*void CGame::CalculateEnduranceDecrement(short sTargetH, short sAttackerH, char cTargetType, int iArmorType)
{
 short sItemIndex;
 int iDownValue = 1, iHammerChance = 100;

    if (m_pClientList[sTargetH] == NULL) return;

    if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL )) {
        if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_cSide != m_pClientList[sAttackerH]->m_cSide)) {
            switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill) {
                case 14:
                    if ((31 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4)) || (32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))) {
                        sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                        if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL)) {
                            if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 761) { // BattleHammer
                                iDownValue = 30;
                                break;
                            }
                            if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 762) { // GiantBattleHammer
                                iDownValue = 35;
                                break;
                            }
                            if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 843) { // BarbarianHammer
                                iDownValue = 30;
                                break;
                            }
                            if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 745) { // BarbarianHammer
                                iDownValue = 30;
                                break;
                            }
                        }
                    }
                    else {
                        iDownValue = 20; break;
                    }
                case 10: iDownValue = 3; break;
                default: iDownValue = 1; break;
                }

                if (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == TRUE) {
                    switch (m_pClientList[sTargetH]->m_iSpecialAbilityType)
                        case 52: iDownValue = 0; iHammerChance = 0;
                }
            }
        }

        if ((m_pClientList[sTargetH]->m_cSide != 0) && (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan > 0)) {
                m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan -= iDownValue;
        }

        if ((m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan <= 0) || (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan > 64000)) {
            m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan = 0;
            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_cEquipPos, iArmorType, NULL, NULL);
            ReleaseItemHandler(sTargetH, iArmorType, TRUE);
            return;
        }

    /*try
    {
        if (m_pClientList[sAttackerH] != NULL) {
            if (cTargetType == DEF_OWNERTYPE_PLAYER) {
                sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL)) {
                    if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 617) || (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 618) || (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 619) || (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 873) || (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 874) || (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 75) || (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 76)) {
                        m_pClientList[sAttackerH]->m_sUsingWeaponSkill = 6;
                        return;
                    }
                }
            }
        }*/

        /*if (m_pClientList[sAttackerH] != NULL) {
        if (cTargetType == DEF_OWNERTYPE_PLAYER) {
        if ((m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14) && (iHammerChance == 100)) {
            if (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wMaxLifeSpan < 2000) {
                iHammerChance = iDice(6, (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan));
            }
            else {
                iHammerChance = iDice(4, (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan));
            }

            if ((31 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4)) || (32 == ((m_pClientList[sAttackerH]->m_sAppr2  & 0x0FF0) >> 4))) {
                sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL)) {
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 761) { // BattleHammer
                        iHammerChance = iHammerChance/2;
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 762) { // GiantBattleHammer
                        iHammerChance = ((iHammerChance*10)/9);
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 843) { // GiantBattleHammer
                        iHammerChance = ((iHammerChance*10)/9);
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 745) { // GiantBattleHammer
                        iHammerChance = ((iHammerChance*10)/9);
                    }
                }
            }
            if ((m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_sIDnum == 622) || (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_sIDnum == 621)) {
                iHammerChance = 0;
            }
            if (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan < iHammerChance) {
                wsprintf(G_cTxt, "(iHammerChance (%d), target armor endurance (%d)!", iHammerChance, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan);
                log->info(G_cTxt);
                ReleaseItemHandler(sTargetH, iArmorType, TRUE);
                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_cEquipPos, iArmorType, NULL, NULL);
                return;
            }
        }
        }
        }
    //}
    //catch(...)
    {

    }
}*/

BOOL CGame::bCalculateEnduranceDecrement(short sTargetH, short sAttackerH, char cTargetType, int iArmorType)
{
    int iDownValue = 1, iHammerChance = 100, iItemIndex;
    WORD wWeaponType;

    if (m_pClientList[sTargetH] == NULL) return FALSE;
    if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] == NULL)) return FALSE;
    wWeaponType = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
    if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_cSide != m_pClientList[sAttackerH]->m_cSide))
    {
        switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill)
        {
        case 14:
            if ((wWeaponType == 31) || (wWeaponType == 32))
            {
                iItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                if ((iItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[iItemIndex] != NULL))
                {
                    if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 761) // BattleHammer 
                    {
                        iDownValue = 30;
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 762) // GiantBattleHammer
                    {
                        iDownValue = 35;
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 843) // BarbarianHammer
                    {
                        iDownValue = 30;
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 745) // MasterBattleHammer
                    {
                        iDownValue = 30;
                    }
                    break;
                }
            }
            else
            {
                iDownValue = 20;
            }
            break;
        case 10:
            iDownValue = 3;
            break;
        default:
            iDownValue = 1;
        }
    }
    if (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == TRUE)
    {
        switch (m_pClientList[sTargetH]->m_iSpecialAbilityType)
    case 52:
        iDownValue = 0;
        iHammerChance = 0;
    }
    if ((m_pClientList[sTargetH]->m_cSide != 0) && (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan > 0))
    {
        m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan -= iDownValue;
    }
    if (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan <= 0)
    {
        m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan = 0;
        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_cEquipPos, iArmorType, NULL, NULL);
        ReleaseItemHandler(sTargetH, iArmorType, TRUE);
        return TRUE;
    }
    if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14) && (iHammerChance == 100))
    {
        if (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wMaxLifeSpan < 2000)
        {
            iHammerChance = iDice(6, (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan));
        }
        else
        {
            iHammerChance = iDice(4, (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wMaxLifeSpan - m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan));
        }
        if ((wWeaponType == 31) || (wWeaponType == 32))
        {
            iItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
            if ((iItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[iItemIndex] != NULL))
            {
                if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 761) // BattleHammer 
                {
                    iHammerChance -= iHammerChance >> 1;
                }
                if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 762) // GiantBattleHammer
                {
                    iHammerChance = (((iHammerChance * 5) + 7) >> 3);
                }
                if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 843) // BarbarianHammer
                {
                    iHammerChance = (((iHammerChance * 5) + 7) >> 3);
                }
                if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 745) // MasterBattleHammer
                {
                    iHammerChance = (((iHammerChance * 5) + 7) >> 3);
                }
            }
            else
            {
                iHammerChance = ((iHammerChance + 3) >> 2);
            }
            switch (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_sIDnum)
            {
            case 621:
            case 622:
                iHammerChance = 0;
            }
            if (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan < iHammerChance)
            {
                ReleaseItemHandler(sTargetH, iArmorType, TRUE);
                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_cEquipPos, iArmorType, NULL, NULL);
            }
        }
    }
    return TRUE;
}

int CGame::iCalculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, BOOL bNearAttack, BOOL bIsDash, BOOL bArrowUse)
{
    int    iAP_SM, iAP_L, iAttackerHitRatio{}, iTargetDefenseRatio, iDestHitRatio, iResult, iAP_Abs_Armor, iAP_Abs_Shield;
    char   cAttackerName[30], cAttackerDir, cAttackerSide, cTargetDir, cProtect, cCropSkill, cFarmingSkill;
    short  sWeaponIndex, sAttackerWeapon, dX, dY, sX, sY, sAtkX, sAtkY, sTgtX, sTgtY;
    DWORD  dwTime;
    WORD   wWeaponType;
    double dTmp1, dTmp2, dTmp3;
    BOOL   bKilled;
    BOOL   bNormalMissileAttack;
    BOOL   bIsAttackerBerserk;
    int    iKilledDice, iDamage, iExp, iWepLifeOff, iSideCondition, iMaxSuperAttack, iWeaponSkill, iComboBonus, iTemp;
    int    iAttackerHP, iMoveDamage, iRepDamage;
    char   cAttackerSA;
    int    iAttackerSAvalue, iHitPoint{};
    char   cDamageMoveDir;
    int    iPartyID, iConstructionPoint, iWarContribution, tX, tY, iDst1, iDst2;
    short sItemIndex;
    short sSkillUsed;

    dwTime = timeGetTime();
    bKilled = FALSE;
    iExp = 0;
    iPartyID = 0;
    bNormalMissileAttack = FALSE;
    ZeroMemory(cAttackerName, sizeof(cAttackerName));
    cAttackerSA = NULL;
    iAttackerSAvalue = NULL;
    wWeaponType = NULL;

    switch (cAttackerType)
    {
    case DEF_OWNERTYPE_PLAYER:

        if (m_pClientList[sAttackerH] == NULL) return 0;
        if ((m_bAdminSecurity == TRUE) && (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0)) return 0;
        if ((m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) && (m_pClientList[sAttackerH]->m_iAdminUserLevel == 0)) return 0;
        if ((m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] == NULL) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsHeldenianMap == TRUE) && (m_bIsHeldenianMode == TRUE)) return 0;
        if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == TRUE) && (cTargetType == DEF_OWNERTYPE_PLAYER)) return 0;

        if ((m_pClientList[sAttackerH]->m_iStatus & 0x10) != 0)
        {
            SetInvisibilityFlag(sAttackerH, DEF_OWNERTYPE_PLAYER, FALSE);
            bRemoveFromDelayEventList(sAttackerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
            m_pClientList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
        }

        if ((m_pClientList[sAttackerH]->m_sAppr2 & 0xF000) == 0) return 0;

        iAP_SM = 0;
        iAP_L = 0;

        wWeaponType = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);

        sSkillUsed = m_pClientList[sAttackerH]->m_sUsingWeaponSkill;
        if ((bIsDash == TRUE) && (m_pClientList[sAttackerH]->m_cSkillMastery[sSkillUsed] != 100) && (wWeaponType != 25) && (wWeaponType != 27))
        {
            try
            {
                wsprintf(G_cTxt, "TSearch Fullswing Hack: (%s) Player: (%s) - dashing with only (%d) weapon skill.", m_pClientList[sAttackerH]->m_cIPaddress, m_pClientList[sAttackerH]->m_cCharName, m_pClientList[sAttackerH]->m_cSkillMastery[sSkillUsed]);
                log->info(G_cTxt);
                DeleteClient(sAttackerH, TRUE, TRUE);
            }
            catch (...)
            {

            }
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
        }
        else if (wWeaponType >= 40)
        {
            iAP_SM = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_SM, m_pClientList[sAttackerH]->m_cAttackDiceRange_SM);
            iAP_L = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_L, m_pClientList[sAttackerH]->m_cAttackDiceRange_L);

            iAP_SM += m_pClientList[sAttackerH]->m_cAttackBonus_SM;
            iAP_L += m_pClientList[sAttackerH]->m_cAttackBonus_L;

            iAttackerHitRatio = m_pClientList[sAttackerH]->m_iHitRatio;
            bNormalMissileAttack = TRUE;

            iAP_SM += iDice(1, (m_pClientList[sAttackerH]->m_iStr / 20));
            iAP_L += iDice(1, (m_pClientList[sAttackerH]->m_iStr / 20));
        }
        else
        {
            iAttackerHitRatio = 0;
        }

        iAttackerHitRatio += 50;
        if (iAP_SM <= 0) iAP_SM = 1;
        if (iAP_L <= 0) iAP_L = 1;

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

        if (m_pClientList[sAttackerH]->m_cHeroArmorBonus == 1)
        {
            iAttackerHitRatio += 100;
            iAP_SM += 5;
            iAP_L += 5;
        }

        sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
        if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL))
        {
            if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 851) || // KlonessEsterk 
                (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 863) || // KlonessWand(MS.20)
                (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 864)) // KlonessWand(MS.10) 
            {
                if (m_pClientList[sAttackerH]->m_iRating > 0)
                {
                    iRepDamage = m_pClientList[sAttackerH]->m_iRating / 100;
                    if (iRepDamage < 5) iRepDamage = 5;
                    if (iRepDamage > 15) iRepDamage = 15;
                    iAP_SM += iRepDamage;
                    iAP_L += iRepDamage;
                }
                if (cTargetType == DEF_OWNERTYPE_PLAYER)
                {
                    if (m_pClientList[sTargetH] == NULL) return 0;
                    if (m_pClientList[sTargetH]->m_iRating < 0)
                    {
                        iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 10);
                        if (iRepDamage > 10) iRepDamage = 10;
                        iAP_SM += iRepDamage;
                        iAP_L += iRepDamage;
                    }
                }
            }
            if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 732) || // BerserkWand(MS.20)
                (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 738)) // BerserkWand(MS.10)
            {
                iAP_SM += 1;
                iAP_L += 1;
            }
        }

        sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
        if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL))
        {
            if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 847) &&
                (m_cDayOrNight == 2))
            {
                iAP_SM += 4;
                iAP_L += 4;
            }
            if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 848) &&
                (m_cDayOrNight == 1))
            {
                iAP_SM += 4;
                iAP_L += 4;
            }
            if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 849) || // KlonessBlade 
                (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 850)) // KlonessAxe
            {
                if (m_pClientList[sAttackerH]->m_iRating > 0)
                {
                    iRepDamage = m_pClientList[sAttackerH]->m_iRating / 100;
                    if (iRepDamage < 5) iRepDamage = 5;
                    if (iRepDamage > 15) iRepDamage = 15;
                    iAP_SM += iRepDamage;
                    iAP_L += iRepDamage;
                }
                if (cTargetType == DEF_OWNERTYPE_PLAYER)
                {
                    if (m_pClientList[sTargetH] == NULL) return 0;
                    if (m_pClientList[sTargetH]->m_iRating < 0)
                    {
                        iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 10);
                        if (iRepDamage > 10) iRepDamage = 10;
                        iAP_SM += iRepDamage;
                        iAP_L += iRepDamage;
                    }
                }
            }
        }

        sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_NECK];
        if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL))
        {
            if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859) // NecklaceOfKloness  
            {
                if (cTargetType == DEF_OWNERTYPE_PLAYER)
                {
                    if (m_pClientList[sTargetH] == NULL) return 0;
                    iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 20);
                    if (iRepDamage > 5) iRepDamage = 5;
                    iAP_SM += iRepDamage;
                    iAP_L += iRepDamage;
                }
            }
        }

        cAttackerDir = m_pClientList[sAttackerH]->m_cDir;
        strcpy(cAttackerName, m_pClientList[sAttackerH]->m_cCharName);

        if (m_pClientList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0)
            bIsAttackerBerserk = TRUE;
        else bIsAttackerBerserk = FALSE;

        if ((bArrowUse != TRUE) && (m_pClientList[sAttackerH]->m_iSuperAttackLeft > 0) && (iAttackMode >= 20))
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

            case 7:  iAP_SM *= 1; iAP_L *= 1; break;

            case 8:  iAP_SM += (iAP_SM / 10); iAP_L += (iAP_L / 10); iAttackerHitRatio += 30; break;
            case 10:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5);                           break;
            case 14:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); iAttackerHitRatio += 20; break;
            case 21:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); iAttackerHitRatio += 50; break;
            default: break;
            }
            iAttackerHitRatio += 100;
            iAttackerHitRatio += m_pClientList[sAttackerH]->m_iCustomItemValue_Attack;
        }

        if (bIsDash == TRUE)
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

        iAttackerHP = m_pClientList[sAttackerH]->m_iHP;
        iAttackerHitRatio += m_pClientList[sAttackerH]->m_iAddAR;

        sAtkX = m_pClientList[sAttackerH]->m_sX;
        sAtkY = m_pClientList[sAttackerH]->m_sY;
        iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
        break;

    case DEF_OWNERTYPE_NPC:

        if (m_pNpcList[sAttackerH] == NULL) return 0;
        if (m_pMapList[m_pNpcList[sAttackerH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) return 0;

        if ((m_pNpcList[sAttackerH]->m_iStatus & 0x10) != 0)
        {
            SetInvisibilityFlag(sAttackerH, DEF_OWNERTYPE_NPC, FALSE);
            bRemoveFromDelayEventList(sAttackerH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_INVISIBILITY);
            m_pNpcList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
        }

        cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
        iAP_SM = 0;
        iAP_L = 0;

        if (m_pNpcList[sAttackerH]->m_cAttackDiceThrow != 0)
            iAP_L = iAP_SM = iDice(m_pNpcList[sAttackerH]->m_cAttackDiceThrow, m_pNpcList[sAttackerH]->m_cAttackDiceRange);

        iAttackerHitRatio = m_pNpcList[sAttackerH]->m_iHitRatio;

        cAttackerDir = m_pNpcList[sAttackerH]->m_cDir;
        memcpy(cAttackerName, m_pNpcList[sAttackerH]->m_cNpcName, 20);

        if (m_pNpcList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0)
            bIsAttackerBerserk = TRUE;
        else bIsAttackerBerserk = FALSE;

        iAttackerHP = m_pNpcList[sAttackerH]->m_iHP;
        cAttackerSA = m_pNpcList[sAttackerH]->m_cSpecialAbility;

        sAtkX = m_pNpcList[sAttackerH]->m_sX;
        sAtkY = m_pNpcList[sAttackerH]->m_sY;
        break;
    }

    switch (cTargetType)
    {
    case DEF_OWNERTYPE_PLAYER:

        if (m_pClientList[sTargetH] == NULL) return 0;
        if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return 0;
        if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return 0;

        if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == FALSE) &&
            (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == TRUE)) return 0;

        if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == TRUE) &&
            (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsOwnLocation == TRUE)) return 0;

        if ((m_pClientList[sTargetH]->m_sX != tdX) || (m_pClientList[sTargetH]->m_sY != tdY)) return 0;

        if (m_pClientList[sTargetH]->m_iAdminUserLevel > 0) return 0;

        if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == TRUE)
            && (m_pClientList[sTargetH]->m_iPKCount == 0)) return 0;

        if ((m_pClientList[sTargetH]->m_iPartyID != NULL) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return 0;

        cTargetDir = m_pClientList[sTargetH]->m_cDir;
        iTargetDefenseRatio = m_pClientList[sTargetH]->m_iDefenseRatio;
        m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;
        if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == TRUE))
        {
            iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
            if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
            {
                iAP_SM = iAP_SM / 2;
                iAP_L = iAP_L / 2;
            }
            else
            {
                if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
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

        iTargetDefenseRatio += m_pClientList[sTargetH]->m_iAddDR;



        sTgtX = m_pClientList[sTargetH]->m_sX;
        sTgtY = m_pClientList[sTargetH]->m_sY;
        break;

    case DEF_OWNERTYPE_NPC:

        if (m_pNpcList[sTargetH] == NULL) return 0;
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

            if ((wWeaponType == 25) && (m_pNpcList[sTargetH]->m_cActionLimit == 5) && (m_pNpcList[sTargetH]->m_iBuildCount > 0))
            {
                if ((m_pClientList[sAttackerH]->m_iCrusadeDuty != 2) && (m_pClientList[sAttackerH]->m_iAdminUserLevel == 0)) break;

                switch (m_pNpcList[sTargetH]->m_sType)
                {
                case 36:
                case 37:
                case 38:
                case 39:
                    if (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0)
                    {
                        m_pNpcList[sTargetH]->m_sAppr2 = 0;
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                    }

                    switch (m_pNpcList[sTargetH]->m_iBuildCount)
                    {
                    case 1:
                        m_pNpcList[sTargetH]->m_sAppr2 = 0;
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
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
                        wsprintf(G_cTxt, "Construction Complete! WarContribution: +%d", iWarContribution);
                        log->info(G_cTxt);
                        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, NULL, NULL);
                        break;
                    case 5:
                        m_pNpcList[sTargetH]->m_sAppr2 = 1;
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                        break;
                    case 10:
                        m_pNpcList[sTargetH]->m_sAppr2 = 2;
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
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
            if ((wWeaponType == 27) && (m_pNpcList[sTargetH]->m_cCropType != 0) && (m_pNpcList[sTargetH]->m_cActionLimit == 5) && (m_pNpcList[sTargetH]->m_iBuildCount > 0))
            {
                cFarmingSkill = m_pClientList[sAttackerH]->m_cSkillMastery[2];
                cCropSkill = m_pNpcList[sTargetH]->m_cCropSkill;
                if (cFarmingSkill < 20) return 0;
                if (m_pClientList[sAttackerH]->m_iLevel < 20) return 0;
                switch (m_pNpcList[sTargetH]->m_sType)
                {
                case 64:
                    switch (m_pNpcList[sTargetH]->m_iBuildCount)
                    {
                    case 1:
                        m_pNpcList[sTargetH]->m_sAppr2 = 3;
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                        CalculateSSN_SkillIndex(sAttackerH, 2, cFarmingSkill <= cCropSkill + 10);
                        _CheckFarmingAction(sAttackerH, sTargetH, 1);
                        DeleteNpc(sTargetH);
                        return 0;
                    case 8:
                        m_pNpcList[sTargetH]->m_sAppr2 = 3;
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                        CalculateSSN_SkillIndex(sAttackerH, 2, cFarmingSkill <= cCropSkill + 10);
                        _CheckFarmingAction(sAttackerH, sTargetH, 0);
                        break;
                    case 18:
                        m_pNpcList[sTargetH]->m_sAppr2 = 2;
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                        CalculateSSN_SkillIndex(sAttackerH, 2, cFarmingSkill <= cCropSkill + 10);
                        _CheckFarmingAction(sAttackerH, sTargetH, 0);
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
        }

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
        case 1:	iAttackerHitRatio -= (iAttackerHitRatio / 20); break;
        case 2:	iAttackerHitRatio -= (iAttackerHitRatio / 10); break;
        case 3:	iAttackerHitRatio -= (iAttackerHitRatio / 4);  break;
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
        if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
        {
            if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]] == NULL)
            {
                m_pClientList[sAttackerH]->m_bIsItemEquipped[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]] = FALSE;
                DeleteClient(sAttackerH, TRUE, TRUE);
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
                    if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex] == NULL)
                        return 0;

                    if (bArrowUse != TRUE)
                        m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex]->m_dwCount--;
                    if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex]->m_dwCount <= 0)
                    {

                        ItemDepleteHandler(sAttackerH, m_pClientList[sAttackerH]->m_cArrowIndex, FALSE);
                        m_pClientList[sAttackerH]->m_cArrowIndex = _iGetArrowItemIndex(sAttackerH);
                    }
                    else
                    {
                        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_SETITEMCOUNT, m_pClientList[sAttackerH]->m_cArrowIndex, m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex]->m_dwCount, FALSE, NULL);
                        iCalcTotalWeight(sAttackerH);
                    }
                }
                if (cProtect == 1) return 0;
            }
            else
            {
                switch (cProtect)
                {
                case 3: iTargetDefenseRatio += 40;  break;
                case 4: iTargetDefenseRatio += 100; break;
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
        case 3: iTargetDefenseRatio += 40;  break;
        case 4: iTargetDefenseRatio += 100; break;
        }
        if (iTargetDefenseRatio < 0) iTargetDefenseRatio = 1;
    }

    if (cAttackerDir == cTargetDir) iTargetDefenseRatio = iTargetDefenseRatio / 2;
    if (iTargetDefenseRatio < 1)   iTargetDefenseRatio = 1;

    dTmp1 = (double)(iAttackerHitRatio);
    dTmp2 = (double)(iTargetDefenseRatio);
    dTmp3 = (dTmp1 / dTmp2) * 50.0f;
    iDestHitRatio = (int)(dTmp3);

    if (iDestHitRatio < DEF_MINIMUMHITRATIO) iDestHitRatio = DEF_MINIMUMHITRATIO;
    if (iDestHitRatio > DEF_MAXIMUMHITRATIO) iDestHitRatio = DEF_MAXIMUMHITRATIO;

    if ((bIsAttackerBerserk == TRUE) && (iAttackMode < 20))
    {
        iAP_SM = iAP_SM * 2;
        iAP_L = iAP_L * 2;
    }

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        iAP_SM += m_pClientList[sAttackerH]->m_iAddPhysicalDamage;
        iAP_L += m_pClientList[sAttackerH]->m_iAddPhysicalDamage;
    }

    if (bNearAttack == TRUE)
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

    if (iResult <= iDestHitRatio)
    {
        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        {

            if (((m_pClientList[sAttackerH]->m_iHungerStatus <= 10) || (m_pClientList[sAttackerH]->m_iSP <= 0)) && (iDice(1, 10) == 5)) return FALSE;
            m_pClientList[sAttackerH]->m_iComboAttackCount++;
            if (m_pClientList[sAttackerH]->m_iComboAttackCount < 0) m_pClientList[sAttackerH]->m_iComboAttackCount = 0;
            if (m_pClientList[sAttackerH]->m_iComboAttackCount > 4) m_pClientList[sAttackerH]->m_iComboAttackCount = 1;
            iWeaponSkill = _iGetWeaponSkillType(sAttackerH);
            iComboBonus = iGetComboAttackBonus(iWeaponSkill, m_pClientList[sAttackerH]->m_iComboAttackCount);

            if ((m_pClientList[sAttackerH]->m_iComboAttackCount > 1) && (m_pClientList[sAttackerH]->m_iAddCD != NULL))
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

            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
            {
                iAP_SM += iAP_SM / 3;
                iAP_L += iAP_L / 3;
            }

            if (bCheckHeldenianMap(sAttackerH, m_iBTFieldMapIndex, DEF_OWNERTYPE_PLAYER) == 1)
            {
                iAP_SM += iAP_SM / 3;
                iAP_L += iAP_L / 3;
            }

            if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == TRUE) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
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
                    iAP_SM += iAP_SM / 3;
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
                        if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != NULL))
                        {
                            if ((m_pClientList[sTargetH]->m_cSide != 0) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0))
                                m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan--;
                            if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan == 0)
                            {
                                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, NULL, NULL);
                                ReleaseItemHandler(sTargetH, iTemp, TRUE);
                            }
                        }
                    }
                }

                iAP_SM = iAP_SM - (iAP_Abs_Armor + iAP_Abs_Shield);
                if (iAP_SM <= 0) iAP_SM = 1;

                if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL) && (m_pClientList[sAttackerH]->m_bIsSpecialAbilityEnabled == TRUE))
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
                            SetIceFlag(sTargetH, DEF_OWNERTYPE_PLAYER, TRUE);
                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + 30000,
                                sTargetH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 1, NULL, NULL);
                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                        }
                        break;
                    case 3:
                        if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 0)
                        {
                            m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 2;
                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + 10000,
                                sTargetH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 10, NULL, NULL);
                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_HOLDOBJECT, 10, NULL, NULL);
                        }
                        break;
                    case 4:
                        iAP_SM = (m_pClientList[sTargetH]->m_iHP);
                        break;
                    case 5:
                        m_pClientList[sAttackerH]->m_iHP += iAP_SM;
                        if (iGetMaxHP(sAttackerH) < m_pClientList[sAttackerH]->m_iHP) m_pClientList[sAttackerH]->m_iHP = iGetMaxHP(sAttackerH);
                        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        break;
                    }
                }

                if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == TRUE))
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

                if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == TRUE) &&
                    (iDice(1, 10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iAP_SM))
                {
                    iAP_SM = m_pClientList[sTargetH]->m_iHP - 1;
                }

                switch (iHitPoint)
                {
                case 1:
                    iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
                    if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != NULL))
                    {
                        bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
                    }
                    break;

                case 2:
                    iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS];
                    if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != NULL))
                    {
                        bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
                    }
                    else
                    {
                        iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
                        if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != NULL))
                        {
                            bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
                        }
                    }
                    break;

                case 3:
                    iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
                    if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != NULL))
                    {
                        bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
                    }
                    break;

                case 4:
                    iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
                    if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != NULL))
                    {
                        bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
                    }
                    break;
                }

                if ((cAttackerSA == 2) && (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0))
                {
                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_PROTECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT], NULL, NULL);
                    switch (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT])
                    {
                    case 1:
                        SetProtectionFromArrowFlag(sTargetH, DEF_OWNERTYPE_PLAYER, FALSE);
                        break;
                    case 2:
                    case 5:
                        SetMagicProtectionFlag(sTargetH, DEF_OWNERTYPE_PLAYER, FALSE);
                        break;
                    case 3:
                    case 4:
                        SetDefenseShieldFlag(sTargetH, DEF_OWNERTYPE_PLAYER, FALSE);
                        break;
                    }
                    m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = NULL;
                    bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_PROTECT);
                }

                if ((m_pClientList[sTargetH]->m_bIsPoisoned == FALSE) &&
                    ((cAttackerSA == 5) || (cAttackerSA == 6) || (cAttackerSA == 61)))
                {
                    if (bCheckResistingPoisonSuccess(sTargetH, DEF_OWNERTYPE_PLAYER) == FALSE)
                    {
                        m_pClientList[sTargetH]->m_bIsPoisoned = TRUE;
                        if (cAttackerSA == 5)		m_pClientList[sTargetH]->m_iPoisonLevel = 15;
                        else if (cAttackerSA == 6)  m_pClientList[sTargetH]->m_iPoisonLevel = 40;
                        else if (cAttackerSA == 61) m_pClientList[sTargetH]->m_iPoisonLevel = iAttackerSAvalue;

                        m_pClientList[sTargetH]->m_dwPoisonTime = dwTime;
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pClientList[sTargetH]->m_iPoisonLevel, NULL, NULL);
                        SetPoisonFlag(sTargetH, DEF_OWNERTYPE_PLAYER, TRUE);
                    }
                }

                m_pClientList[sTargetH]->m_iHP -= iAP_SM;
                if (m_pClientList[sTargetH]->m_iHP <= 0)
                {
                    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                        bAnalyzeCriminalAction(sAttackerH, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                    ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iAP_SM);
                    bKilled = TRUE;
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
                                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, NULL, NULL, NULL, NULL);
                            }
                        }

                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);

                        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                            sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
                        else sAttackerWeapon = 1;

                        if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE))
                            iMoveDamage = 60;
                        else iMoveDamage = 40;

                        if (iAP_SM >= iMoveDamage)
                        {
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

                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iAP_SM, sAttackerWeapon, NULL);
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
                                SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iAP_SM, sAttackerWeapon, NULL);
                        }

                        if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 1)
                        {
                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);
                            m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                            bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                        }

                        m_pClientList[sTargetH]->m_iSuperAttackCount++;
                        if (m_pClientList[sTargetH]->m_iSuperAttackCount > 14)
                        {
                            m_pClientList[sTargetH]->m_iSuperAttackCount = 0;
                            iMaxSuperAttack = (m_pClientList[sTargetH]->m_iLevel / 10);
                            if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;
                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH]->m_cBehavior == DEF_BEHAVIOR_DEAD) return 0;
            if (m_pNpcList[sTargetH]->m_bIsKilled == TRUE) return 0;
            if (m_bIsCrusadeMode == TRUE)
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
                else if ((m_pNpcList[sTargetH]->m_sType == 31) && (cAttackerType == 1) && (m_pClientList[sAttackerH] != NULL) && (m_pClientList[sAttackerH]->m_iSpecialAbilityType == 7))
                    iDamage += iDice(3, 2);
            }


            if ((cAttackerSA == 2) && (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0))
            {
                switch (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT])
                {
                case 1:
                    SetProtectionFromArrowFlag(sTargetH, DEF_OWNERTYPE_NPC, FALSE);
                    break;
                case 2:
                case 5:
                    SetMagicProtectionFlag(sTargetH, DEF_OWNERTYPE_NPC, FALSE);
                    break;
                case 3:
                case 4:
                    SetDefenseShieldFlag(sTargetH, DEF_OWNERTYPE_NPC, FALSE);
                    break;
                }
                m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = NULL;
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
                bKilled = TRUE;
                iKilledDice = m_pNpcList[sTargetH]->m_iHitDice;
            }
            else
            {
                if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
                    && (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) goto CAE_SKIPCOUNTERATTACK;

                if (m_pNpcList[sTargetH]->m_cActionLimit != 0) goto CAE_SKIPCOUNTERATTACK;
                if (m_pNpcList[sTargetH]->m_bIsPermAttackMode == TRUE) goto CAE_SKIPCOUNTERATTACK;
                if ((m_pNpcList[sTargetH]->m_bIsSummoned == TRUE) && (m_pNpcList[sTargetH]->m_iSummonControlMode == 1)) goto CAE_SKIPCOUNTERATTACK;
                if (m_pNpcList[sTargetH]->m_sType == 51) goto CAE_SKIPCOUNTERATTACK;

                if (iDice(1, 3) == 2)
                {
                    if (m_pNpcList[sTargetH]->m_cBehavior == DEF_BEHAVIOR_ATTACK)
                    {
                        tX = tY = 0;
                        switch (m_pNpcList[sTargetH]->m_cTargetType)
                        {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex] != NULL)
                            {
                                tX = m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sX;
                                tY = m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sY;
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[m_pNpcList[sTargetH]->m_iTargetIndex] != NULL)
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
                            if (m_pClientList[sAttackerH] != NULL)
                            {
                                tX = m_pClientList[sAttackerH]->m_sX;
                                tY = m_pClientList[sAttackerH]->m_sY;
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sAttackerH] != NULL)
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

                    if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, NULL) == FALSE)
                    {
                        cDamageMoveDir = iDice(1, 8);
                        dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                        dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, NULL) == FALSE) goto CAE_SKIPDAMAGEMOVE2;
                    }

                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);
                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
                    m_pNpcList[sTargetH]->m_sX = dX;
                    m_pNpcList[sTargetH]->m_sY = dY;
                    m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;

                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, NULL, NULL, NULL);

                    dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                    dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                    if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, NULL) == FALSE)
                    {
                        cDamageMoveDir = iDice(1, 8);
                        dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                        dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];
                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, NULL) == FALSE) goto CAE_SKIPDAMAGEMOVE2;
                    }

                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);
                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
                    m_pNpcList[sTargetH]->m_sX = dX;
                    m_pNpcList[sTargetH]->m_sY = dY;
                    m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;

                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, NULL, NULL, NULL);

                    if (bCheckEnergySphereDestination(sTargetH, sAttackerH, cAttackerType) == TRUE)
                    {
                        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                        {
                            iExp = (m_pNpcList[sTargetH]->m_iExp / 3);
                            if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0)
                                iExp += m_pNpcList[sTargetH]->m_iNoDieRemainExp;

                            if (m_pClientList[sAttackerH]->m_iAddExp != NULL)
                            {
                                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                dTmp2 = (double)iExp;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                iExp += (int)dTmp3;
                            }

                            if ((m_bIsCrusadeMode == TRUE) && (iExp > 10)) iExp = 10;

                            GetExp(sAttackerH, iExp);

                            DeleteNpc(sTargetH);
                            return FALSE;
                        }
                    }

                    CAE_SKIPDAMAGEMOVE2:;
                }
                else
                {
                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, sAttackerWeapon, NULL);
                }

                if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 1)
                {
                    m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                    bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                }
                else if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] == 2)
                {
                    if ((m_pNpcList[sTargetH]->m_iHitDice > 50) && (iDice(1, 10) == 5))
                    {
                        m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }

                if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != TRUE) &&
                    (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
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

                    if (m_pClientList[sAttackerH]->m_iAddExp != NULL)
                    {
                        dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                        dTmp2 = (double)iExp;
                        dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                        iExp += (int)dTmp3;
                    }

                    if (m_bIsCrusadeMode == TRUE) iExp = iExp / 3;

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

        if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        {
            if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
                sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
            else sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];

            if ((sWeaponIndex != -1) && (bArrowUse != TRUE))
            {
                if ((m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex] != NULL) &&
                    (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_sIDnum != 231))
                {
                    if (bKilled == FALSE)
                        CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, 1);
                    else
                    {
                        if (m_pClientList[sAttackerH]->m_iHP <= 3)
                            CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, iDice(1, iKilledDice) * 2);
                        else CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, iDice(1, iKilledDice));
                    }
                }

                if ((m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex] != NULL) &&
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
                        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_cEquipPos, sWeaponIndex, NULL, NULL);
                        ReleaseItemHandler(sAttackerH, sWeaponIndex, TRUE);
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

BOOL CGame::bGetItemNameWhenDeleteNpc(int & iItemID, short sNpcType)
{
    int iResult;

    switch (sNpcType)
    {
    case 49: // Hellclaw 
        iResult = iDice(1, 20000);
        if ((iResult >= 1) && (iResult <= 5000)) iItemID = 308;	    // MagicNecklace(MS10)
        else if ((iResult > 5000) && (iResult <= 10000))  iItemID = 259;  // MagicWand(M.Shield)
        else if ((iResult > 10000) && (iResult <= 13000))  iItemID = 337;  // RubyRing
        else if ((iResult > 13000) && (iResult <= 15000))  iItemID = 335;  // EmeraldRing
        else if ((iResult > 15000) && (iResult <= 17500))  iItemID = 300;  // MagicNecklace(RM10)
        else if ((iResult > 17500) && (iResult <= 18750))  iItemID = 311;  // MagicNecklace(DF+10)
        else if ((iResult > 18750) && (iResult <= 19000))  iItemID = 305;  // MagicNecklace(DM+1)
        else if ((iResult > 19000) && (iResult <= 19700))  iItemID = 634;  // RingofWizard
        else if ((iResult > 19700) && (iResult <= 19844))  iItemID = 635;  // RingofMage
        else if ((iResult > 19844) && (iResult <= 19922))  iItemID = 614;  // SwordofIceElemental
        else if ((iResult > 19922) && (iResult <= 19961))  iItemID = 640;  // KnecklaceOfSufferent
        else if ((iResult > 19961) && (iResult <= 19981))  iItemID = 637;  // KnecklaceOfLightPro
        else if ((iResult > 19981) && (iResult <= 19991))  iItemID = 620;  // MerienShield
        else if ((iResult > 19991) && (iResult <= 19996))  iItemID = 644;  // KnecklaceOfAirEle
        else if ((iResult > 19996) && (iResult <= 19999))  iItemID = 643;  // KnecklaceOfIceEle
        else if ((iResult > 19999) && (iResult <= 20000))  iItemID = 636;  // RingofGrandMage
        return TRUE;

    case 50: // Tigerworm
        iResult = iDice(1, 10000);
        if ((iResult >= 1) && (iResult <= 4999))
        {
            if (iDice(1, 2) == 1)
                iItemID = 311;  // MagicNecklace(DF+10)
            else iItemID = 305;      // MagicNecklace(DM+1)
        }
        else if ((iResult > 5000) && (iResult <= 7499))  iItemID = 614;  // SwordofIceElemental
        else if ((iResult > 7500) && (iResult <= 8749))  iItemID = 290;  // Flameberge+3(LLF)
        else if ((iResult > 8750) && (iResult <= 9374))  iItemID = 633;  // RingofDemonpower
        else if ((iResult > 9375) && (iResult <= 9687))  iItemID = 492;  // BloodRapier
        else if ((iResult > 9688) && (iResult <= 9843))  iItemID = 490;  // BloodSword
        else if ((iResult > 9844) && (iResult <= 9921))  iItemID = 491;  // BloodAxe
        else if ((iResult > 9922) && (iResult <= 9960))  iItemID = 947;  // DragonWand(MS40)
        else if ((iResult > 9961) && (iResult <= 9980))  iItemID = 643;  // KnecklaceOfIceEle
        else if ((iResult > 9981) && (iResult <= 9990))  iItemID = 612;  // XelimaRapier
        else if ((iResult > 9991) && (iResult <= 9996))  iItemID = 610;  // XelimaBlade
        else if ((iResult > 9996) && (iResult <= 9998))  iItemID = 611;  // XelimaAxe
        else if ((iResult > 9999) && (iResult <= 10000)) iItemID = 631;  // RingoftheAbaddon
        return TRUE;

    default:
        break;
    }

    if (iDice(1, 45) == 13)
    {
        switch (sNpcType)
        {
        case 11: if (iDice(1, 465) != 11) return FALSE; break;	  // Skeleton   2 * 100	
        case 12: if (iDice(1, 340) != 11) return FALSE; break;	  // Stone-Golem 2 * 100	
        case 13: if (iDice(1, 85) != 11) return FALSE; break;	  // Cyclops  6 * 100	
        case 14: if (iDice(1, 595) != 11) return FALSE; break;	  // Orc 4 * 100	
        case 17: if (iDice(1, 510) != 11) return FALSE; break;	  // Scorpoin 5 * 100	
        case 18: if (iDice(1, 720) != 11) return FALSE; break;	  // Zombie 1 * 100	
        case 22: if (iDice(1, 510) != 11) return FALSE; break;	  // Amphis 5 * 100	
        case 23: if (iDice(1, 340) != 11) return FALSE; break;	  // Clay-Golem 2 * 100	
        case 27: if (iDice(1, 85) != 11) return FALSE; break;	  // Hellhound 7 * 100	
        case 28: if (iDice(1, 85) != 11) return FALSE; break;	  // Troll 5 * 100	
        case 29: if (iDice(1, 125) != 11) return FALSE; break;	  // Orge  7 * 100	
        case 30: if (iDice(1, 100) != 11) return FALSE; break;	  // Liche 1 * 100   
        case 31: if (iDice(1, 120) != 11) return FALSE; break;	  // Demon 5 * 100
        case 32: if (iDice(1, 170) != 11) return FALSE; break;	  // Unicorn 5 * 100	
        case 33: if (iDice(1, 255) != 11) return FALSE; break;	  // WereWolf 7 * 100
        case 48: if (iDice(1, 80) != 11) return FALSE; break;	  // Stalker 
        case 52: if (iDice(1, 255) != 11) return FALSE; break;    // Gagoyle
        case 53: if (iDice(1, 425) != 11) return FALSE; break;	  // Beholder
        case 54: if (iDice(1, 200) != 11) return FALSE; break;	  // Dark-Elf
        case 57: if (iDice(1, 400) != 11) return FALSE; break;	  // Giant-Frog
        case 63: if (iDice(1, 300) != 11) return FALSE; break;	  // Frost
        case 79: if (iDice(1, 170) != 11) return FALSE; break;	  // Nizie
        case 70: if (iDice(1, 170) != 11) return FALSE; break;	  // Barlog
        case 71: if (iDice(1, 170) != 11) return FALSE; break;	  // Centaurus
        case 74: if (iDice(1, 170) != 11) return FALSE; break;	  // Giant-Crayfish
        case 72: if (iDice(1, 170) != 11) return FALSE; break;	  // Claw-Turtle
        case 75: if (iDice(1, 170) != 11) return FALSE; break;	  // Giant-Lizard
        case 77: if (iDice(1, 170) != 11) return FALSE; break;	  // MasterMage-Orc
        case 78: if (iDice(1, 170) != 11) return FALSE; break;	  // Minotaurs
        case 59: if (iDice(1, 120) != 11) return FALSE; break;	  // Ettin
        default: return FALSE;
        }
    }
    else return FALSE;

    switch (sNpcType)
    {
    case 11: // Skeleton
    case 17: // Scorpoin
    case 14: // Orc
    case 28: // Troll
    case 57: // Giant-Frog
        switch (iDice(1, 7))
        {
        case 1: iItemID = 334; break; // LuckyGoldRing
        case 2: iItemID = 336; break; // SapphireRing
        case 3: if (iDice(1, 15) == 3) iItemID = 335; break; // EmeraldRing
        case 4: iItemID = 337; break; // RubyRing
        case 5: iItemID = 333; break; // PlatinumRing
        case 6: if (iDice(1, 15) == 3) iItemID = 634; break; // RingofWizard
        case 7: if (iDice(1, 25) == 3) iItemID = 635; break; // RingofMage
        }
        break;

    case 13: // Cyclops
    case 27: // Hellhound
    case 29: // Orge
        switch (iDice(1, 7))
        {
        case 1: iItemID = 311; break; // MagicNecklace(DF+10)
        case 2: if (iDice(1, 15) == 13) iItemID = 308; break; // MagicNecklace(MS10)
        case 3: if (iDice(1, 5) == 3) iItemID = 305; break; // MagicNecklace(DM+1)
        case 4: iItemID = 300; break; // MagicNecklace(RM10)
        case 5: if (iDice(1, 25) == 13) iItemID = 632; break; // RingofOgrepower
        case 6: if (iDice(1, 25) == 13) iItemID = 637; break; // KnecklaceOfLightPro
        case 7: if (iDice(1, 25) == 13) iItemID = 638; break; // KnecklaceOfFirePro
        }
        break;

    case 18: // Zombie
    case 22: // Amphis
        switch (iDice(1, 4))
        {
        case 1: if (iDice(1, 65) == 13) iItemID = 613; break; // SwordofMedusa
        case 2: if (iDice(1, 15) == 13) iItemID = 639; break; // KnecklaceOfPoisonPro
        case 3: if (iDice(1, 30) == 13) iItemID = 641; break; // KnecklaceOfMedusa
        case 4: if (iDice(1, 25) == 13) iItemID = 640; break; // KnecklaceOfSufferent
        }
        break;

    case 12: // Stone-Golem
        switch (iDice(1, 5))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 738; break; // BerserkWand(MS.10)
        case 2: if (iDice(1, 30) == 13) iItemID = 621; break; // MerienPlateMailM
        case 3: if (iDice(1, 30) == 13) iItemID = 622; break; // MerienPlateMailW
        case 4: if (iDice(1, 15) == 13) iItemID = 644; break; // KnecklaceOfAirEle
        case 5: if (iDice(1, 15) == 13) iItemID = 647; break; // KnecklaceOfStoneGolem
        }
        break;

    case 23: // Clay-Golem
        switch (iDice(1, 4))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 738; break; // BerserkWand(MS.10)	
        case 2: if (iDice(1, 30) == 13) iItemID = 621; break; // MerienPlateMailM
        case 3: if (iDice(1, 30) == 13) iItemID = 622; break; // MerienPlateMailW
        case 4: if (iDice(1, 15) == 13) iItemID = 647; break; // KnecklaceOfStoneGolem
        }
        break;

    case 32: // Unicorn
        switch (iDice(1, 4))
        {
        case 1: if (iDice(1, 30) == 13) iItemID = 620; break; // MerienShield	
        case 2: if (iDice(1, 30) == 13) iItemID = 621; break; // MerienPlateMailM
        case 3: if (iDice(1, 30) == 13) iItemID = 622; break; // MerienPlateMailW
        case 4: if (iDice(1, 15) == 13) iItemID = 644; break; // KnecklaceOfAirEle
        }
        break;

    case 33: // WereWolf
    case 48: // Stalker
        switch (iDice(1, 4))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 852; break; // CancelManual
        case 2: if (iDice(1, 20) == 13) iItemID = 857; break; // I.M.CManual
        case 3: if (iDice(1, 20) == 13) iItemID = 853; break; // E.S.W Manual
        case 4: iItemID = 620; break; // MerienShield
        }
        break;

    case 30: // Liche
        switch (iDice(1, 6))
        {
        case 1: if (iDice(1, 15) == 13) iItemID = 852; break; // CancelManual
        case 2: iItemID = 380; break; // IceStormManual
        case 3: if (iDice(1, 15) == 13) iItemID = 853; break; // E.S.W Manual
        case 4: if (iDice(1, 30) == 13) iItemID = 643; break; // KnecklaceOfIceEle	
        case 5: if (iDice(1, 15) == 13) iItemID = 648; break; // NecklaceOfLiche
        case 6: if (iDice(1, 20) == 13) iItemID = 734; break; // RingOfArcmage
        }
        break;

    case 31: // Demon 
        switch (iDice(1, 8))
        {
        case 1: if (iDice(1, 5) == 3) iItemID = 382; break; // BloodyShockW.Manual
        case 2: iItemID = 491; break; // BloodAxe
        case 3: if (iDice(1, 5) == 3) iItemID = 490; break; // BloodSword
        case 4: iItemID = 492; break; // BloodRapier
        case 5: if (iDice(1, 5) == 3) iItemID = 381; break; // MassFireStrikeManual
        case 6: if (iDice(1, 15) == 3) iItemID = 633; break; // RingofDemonpower
        case 7: if (iDice(1, 10) == 3) iItemID = 645; break; // KnecklaceOfEfreet
        case 8: if (iDice(1, 15) == 3) iItemID = 616; break; // DemonSlayer
        }
        break;

    case 52: // Gagoyle
        switch (iDice(1, 11))
        {
        case 1: if (iDice(1, 5) == 3) iItemID = 382; break; // BloodyShockW.Manual	
        case 2: if (iDice(1, 15) == 13) iItemID = 610; break; // XelimaBlade	
        case 3: if (iDice(1, 15) == 13) iItemID = 611; break; // XelimaAxe	
        case 4: if (iDice(1, 15) == 13) iItemID = 612; break; // XelimaRapier
        case 5: if (iDice(1, 5) == 3) iItemID = 381; break; // MassFireStrikeManual
        case 6: if (iDice(1, 15) == 13) iItemID = 633; break; // RingofDemonpower
        case 7: if (iDice(1, 10) == 3) iItemID = 645; break; // KnecklaceOfEfreet
        case 8: if (iDice(1, 30) == 13) iItemID = 630; break; // RingoftheXelima	
        case 9: if (iDice(1, 40) == 13) iItemID = 631; break; // RingoftheAbaddon
        case 10: if (iDice(1, 20) == 13) iItemID = 735; break; // RingOfDragonpower
        case 11: if (iDice(1, 30) == 13) iItemID = 20; break; // Excalibur
        }
        break;

    case 53: // Beholder
        if (iDice(1, 10) == 11) iItemID = 380; break; // IceStormManual	
        break;

    case 54: // Dark-Elf
        switch (iDice(1, 4))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 618; break; // DarkElfBow	
        case 2: if (iDice(1, 15) == 13) iItemID = 958; break; // DrowBoots
        case 3: if (iDice(1, 15) == 11) iItemID = 956; break; // DrowChainM
        case 4: if (iDice(1, 15) == 11) iItemID = 957; break; // DrowChainW
        }
        break;

    case 63: // Frost
        switch (iDice(1, 3))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 943; break; // IceAxe	
        case 2: if (iDice(1, 20) == 13) iItemID = 942; break; // IceHammer
        case 3: if (iDice(1, 30) == 11) iItemID = 732; break; // BerserkWand(MS.20)
        }
        break;

    case 79: // Nizie
        switch (iDice(1, 3))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 943; break; // IceAxe	
        case 2: if (iDice(1, 20) == 13) iItemID = 942; break; // IceHammer
        case 3: if (iDice(1, 30) == 11) iItemID = 732; break; // BerserkWand(MS.20)
        }
        break;

    case 70: // Barlog
        switch (iDice(1, 3))
        {
        case 1: if (iDice(1, 5) == 3) iItemID = 382; break; // BloodyShockW.Manual
        case 2: if (iDice(1, 5) == 3) iItemID = 381; break; // MassFireStrikeManual
        case 3: if (iDice(1, 30) == 13) iItemID = 732; break; // BerserkWand(MS.20)
        }
        break;

    case 71: // Centaurus
        switch (iDice(1, 2))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 735; break; // RingOfDragonpower
        case 2: if (iDice(1, 30) == 13) iItemID = 732; break; // BerserkWand(MS.20)
        }
        break;

    case 59: // Ettin
        switch (iDice(1, 3))
        {
        case 1: if (iDice(1, 20) == 13) iItemID = 735; break; // RingOfDragonpower
        case 2: if (iDice(1, 10) == 13) iItemID = 853; break; // E.S.W.Manual
        case 3: if (iDice(1, 7) == 3) iItemID = 382; break; // BloodyShockW.Manual
        }
        break;

    }

    if (iItemID == 0)
        return FALSE;
    else return TRUE;
}

BOOL CGame::_bCheckCharacterData(int iClientH)
{
    int i;

    if ((m_pClientList[iClientH]->m_iStr > m_sCharStatLimit) || (m_pClientList[iClientH]->m_iVit > m_sCharStatLimit) || (m_pClientList[iClientH]->m_iDex > m_sCharStatLimit) ||
        (m_pClientList[iClientH]->m_iMag > m_sCharStatLimit) || (m_pClientList[iClientH]->m_iInt > m_sCharStatLimit) || (m_pClientList[iClientH]->m_iCharisma > m_sCharStatLimit))
    {
        try
        {
            wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) stat points are greater then server accepts.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            return FALSE;
        }
        catch (...)
        {

        }
    }

    if ((m_pClientList[iClientH]->m_iLevel > m_sMaxPlayerLevel) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        try
        {
            wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) level above max server level.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            return FALSE;
        }
        catch (...)
        {

        }
    }

    if (m_pClientList[iClientH]->m_iExp < 0)
    {
        try
        {
            wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) experience is below 0 - (Exp:%d).", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iExp);
            log->info(G_cTxt);
            return FALSE;
        }
        catch (...)
        {

        }
    }

    if ((m_pClientList[iClientH]->m_iHP > iGetMaxHP(iClientH)) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        try
        {
            if (m_pClientList[iClientH]->m_pItemList[(m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND])] != NULL)
            {
                if ((m_pClientList[iClientH]->m_pItemList[(m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND])]->m_sIDnum == 492) || (m_pClientList[iClientH]->m_pItemList[(m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND])]->m_sIDnum == 491))
                {
                    if (m_pClientList[iClientH]->m_iHP > (4 * (iGetMaxHP(iClientH) / 5)))
                    {

                    }
                }
            }
            else if (m_pClientList[iClientH]->m_pItemList[(m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND])] != NULL)
            {
                if ((m_pClientList[iClientH]->m_pItemList[(m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND])]->m_sIDnum == 490))
                {
                    if (m_pClientList[iClientH]->m_iHP > (4 * (iGetMaxHP(iClientH) / 5)))
                    {

                    }
                }
            }
            else
            {
                wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) HP: current/maximum (%d/%d).", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iHP, iGetMaxHP(iClientH));
                log->info(G_cTxt);
                return FALSE;
            }
        }
        catch (...)
        {

        }
    }

    if ((m_pClientList[iClientH]->m_iMP > iGetMaxMP(iClientH)) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        try
        {
            wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) MP: current/maximum (%d/%d).", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iMP, iGetMaxMP(iClientH));
            log->info(G_cTxt);
            return FALSE;
        }
        catch (...)
        {

        }
    }

    if ((m_pClientList[iClientH]->m_iSP > iGetMaxSP(iClientH)) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        try
        {
            wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) SP: current/maximum (%d/%d).", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iSP, iGetMaxSP(iClientH));
            log->info(G_cTxt);
            return FALSE;
        }
        catch (...)
        {

        }
    }

    try
    {
        for (i = 0; i < DEF_MAXBANNED; i++)
        {
            if (strlen(m_stBannedList[i].m_cBannedIPaddress) == 0) break;
            if ((strlen(m_stBannedList[i].m_cBannedIPaddress)) == (strlen(m_pClientList[iClientH]->m_cIPaddress)))
            {
                if (memcmp(m_stBannedList[i].m_cBannedIPaddress, m_pClientList[iClientH]->m_cIPaddress, strlen(m_pClientList[iClientH]->m_cIPaddress)) == 0)
                {
                    wsprintf(G_cTxt, "Client Rejected: Banned: (%s)", m_pClientList[iClientH]->m_cIPaddress);
                    log->info(G_cTxt);
                    return FALSE;
                }
                else
                {

                }
            }
        }
    }
    catch (...)
    {

    }

    return TRUE;
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

//in stat change, check skillpoints
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

void CGame::SetSkillAll(int iClientH, char * pData, DWORD dwMsgSize)
{
    if (m_pClientList[iClientH] == NULL) return;
    //Magic
    if (m_pClientList[iClientH]->m_cSkillMastery[4] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[4] = m_pClientList[iClientH]->m_iMag * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[4] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[4] = 100;
        }
        if (m_pClientList[iClientH]->m_iMag > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[4] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 4, m_pClientList[iClientH]->m_cSkillMastery[4], NULL, NULL);

    }
    //LongSword
    if (m_pClientList[iClientH]->m_cSkillMastery[8] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[8] = m_pClientList[iClientH]->m_iDex * 2;

        if (m_pClientList[iClientH]->m_cSkillMastery[8] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[8] = 100;
        }
        if (m_pClientList[iClientH]->m_iDex > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[8] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 8, m_pClientList[iClientH]->m_cSkillMastery[8], NULL, NULL);

    }
    //Hammer
    if (m_pClientList[iClientH]->m_cSkillMastery[14] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[14] = m_pClientList[iClientH]->m_iDex * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[14] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[14] = 100;
        }
        if (m_pClientList[iClientH]->m_iDex > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[14] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 14, m_pClientList[iClientH]->m_cSkillMastery[14], NULL, NULL);

    }
    //Axes
    if (m_pClientList[iClientH]->m_cSkillMastery[10] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[10] = m_pClientList[iClientH]->m_iDex * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[10] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[10] = 100;
        }
        if (m_pClientList[iClientH]->m_iDex > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[10] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 10, m_pClientList[iClientH]->m_cSkillMastery[10], NULL, NULL);

    }
    //hand attack
    if (m_pClientList[iClientH]->m_cSkillMastery[5] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[5] = m_pClientList[iClientH]->m_iStr * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[5] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[5] = 100;
        }
        if (m_pClientList[iClientH]->m_iStr > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[5] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 5, m_pClientList[iClientH]->m_cSkillMastery[5], NULL, NULL);

    }
    //ShortSword
    if (m_pClientList[iClientH]->m_cSkillMastery[7] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[7] = m_pClientList[iClientH]->m_iDex * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[7] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[7] = 100;
        }
        if (m_pClientList[iClientH]->m_iDex > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[7] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 7, m_pClientList[iClientH]->m_cSkillMastery[7], NULL, NULL);

    }
    //archery
    if (m_pClientList[iClientH]->m_cSkillMastery[6] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[6] = m_pClientList[iClientH]->m_iDex * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[6] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[6] = 100;
        }
        if (m_pClientList[iClientH]->m_iDex > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[6] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 6, m_pClientList[iClientH]->m_cSkillMastery[6], NULL, NULL);

    }
    //Fencing
    if (m_pClientList[iClientH]->m_cSkillMastery[9] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[9] = m_pClientList[iClientH]->m_iDex * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[9] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[9] = 100;
        }
        if (m_pClientList[iClientH]->m_iDex > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[9] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 9, m_pClientList[iClientH]->m_cSkillMastery[9], NULL, NULL);

    }
    //Staff Attack
    if (m_pClientList[iClientH]->m_cSkillMastery[21] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[21] = m_pClientList[iClientH]->m_iInt * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[21] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[21] = 100;
        }
        if (m_pClientList[iClientH]->m_iInt > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[21] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 21, m_pClientList[iClientH]->m_cSkillMastery[21], NULL, NULL);

    }
    //shield
    if (m_pClientList[iClientH]->m_cSkillMastery[11] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[11] = m_pClientList[iClientH]->m_iDex * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[11] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[11] = 100;
        }
        if (m_pClientList[iClientH]->m_iDex > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[11] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 11, m_pClientList[iClientH]->m_cSkillMastery[11], NULL, NULL);

    }
    //mining
    if (m_pClientList[iClientH]->m_cSkillMastery[0] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[0] = 100;
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 0, m_pClientList[iClientH]->m_cSkillMastery[0], NULL, NULL);

    }
    //fishing
    if (m_pClientList[iClientH]->m_cSkillMastery[1] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[1] = 100;
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 1, m_pClientList[iClientH]->m_cSkillMastery[1], NULL, NULL);

    }
    //farming
    if (m_pClientList[iClientH]->m_cSkillMastery[2] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[2] = 100;
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 2, m_pClientList[iClientH]->m_cSkillMastery[2], NULL, NULL);

    }
    //alchemy
    if (m_pClientList[iClientH]->m_cSkillMastery[12] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[12] = 100;
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 12, m_pClientList[iClientH]->m_cSkillMastery[12], NULL, NULL);

    }
    //manufacturing
    if (m_pClientList[iClientH]->m_cSkillMastery[13] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[13] = 100;
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 13, m_pClientList[iClientH]->m_cSkillMastery[13], NULL, NULL);

    }
    //poison resistance
    if (m_pClientList[iClientH]->m_cSkillMastery[23] < 20)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[23] = 20;
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 23, m_pClientList[iClientH]->m_cSkillMastery[23], NULL, NULL);

    }
    //pretend corpse
    if (m_pClientList[iClientH]->m_cSkillMastery[19] < 100)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[19] = m_pClientList[iClientH]->m_iInt * 2;
        if (m_pClientList[iClientH]->m_cSkillMastery[19] > 100)
        {
            m_pClientList[iClientH]->m_cSkillMastery[19] = 100;
        }
        if (m_pClientList[iClientH]->m_iInt > 50)
        {
            m_pClientList[iClientH]->m_cSkillMastery[19] = 100;
        }
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 19, m_pClientList[iClientH]->m_cSkillMastery[19], NULL, NULL);

    }
    //magic resistance
    if (m_pClientList[iClientH]->m_cSkillMastery[3] < 20)
    {
        // now we add skills
        m_pClientList[iClientH]->m_cSkillMastery[3] = 20;
        //Send a notify to update the client
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 3, m_pClientList[iClientH]->m_cSkillMastery[3], NULL, NULL);

    }
}
