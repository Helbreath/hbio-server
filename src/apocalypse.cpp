//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

extern char G_cTxt[512];

/*void CGame::OpenApocalypseGate(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;

    //m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAliveObject;
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_APOCGATEOPEN, 95, 31, NULL, m_pClientList[iClientH]->m_cMapName);
}*/

void CGame::GlobalEndApocalypseMode()
{
    char * cp, cData[120];

    if (m_bIsApocalypseMode == FALSE) return;

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)cData;
    *cp = GSM_ENDAPOCALYPSE;
    cp++;

    LocalEndApocalypse();

    bStockMsgToGateServer(cData, 5);
}

void CGame::GlobalUpdateConfigs(char cConfigType)
{
    char * cp, cData[120];

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)cData;
    *cp = GSM_UPDATECONFIGS;
    cp++;

    *cp = (char)cConfigType;
    cp++;

    LocalUpdateConfigs(cConfigType);

    bStockMsgToGateServer(cData, 5);
}

void CGame::LocalUpdateConfigs(char cConfigType)
{
    if (cConfigType == 1)
    {
        bReadSettingsConfigFile("..\\GameConfigs\\Settings.cfg");
        log->info("(!!!) Settings.cfg updated successfully!");
    }
    if (cConfigType == 2)
    {
        bReadAdminListConfigFile("..\\GameConfigs\\AdminList.cfg");
        log->info("(!!!) AdminList.cfg updated successfully!");
    }
    if (cConfigType == 3)
    {
        bReadBannedListConfigFile("..\\GameConfigs\\BannedList.cfg");
        log->info("(!!!) BannedList.cfg updated successfully!");
    }
    if (cConfigType == 4)
    {
        bReadAdminSetConfigFile("..\\GameConfigs\\AdminSettings.cfg");
        log->info("(!!!) AdminSettings.cfg updated successfully!");
    }
}

void CGame::LocalEndApocalypse()
{
    int i;

    m_bIsApocalypseMode = FALSE;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != NULL)
        {
            SendNotifyMsg(NULL, i, DEF_NOTIFY_APOCGATEENDMSG, NULL, NULL, NULL, NULL);
        }
    }
    wsprintf(G_cTxt, "(!)Apocalypse Mode OFF.");
    log->info(G_cTxt);
}

void CGame::LocalStartApocalypse(DWORD dwApocalypseGUID)
{
    int i;
    //DWORD dwApocalypse;

    m_bIsApocalypseMode = TRUE;

    if (dwApocalypseGUID != NULL)
    {
        _CreateApocalypseGUID(dwApocalypseGUID);
        //m_dwApocalypseGUID = dwApocalypse;
    }

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != NULL)
        {
            SendNotifyMsg(NULL, i, DEF_NOTIFY_APOCGATESTARTMSG, NULL, NULL, NULL, NULL);
            //RequestTeleportHandler(i, "0   ");
            //SendNotifyMsg(NULL, i, DEF_NOTIFY_APOCFORCERECALLPLAYERS, NULL, NULL, NULL, NULL);
        }
    }
    wsprintf(G_cTxt, "(!)Apocalypse Mode ON.");
    log->info(G_cTxt);
}

void CGame::_CreateApocalypseGUID(DWORD dwApocalypseGUID)
{
    char * cp, cTxt[256], cFn[256], cTemp[1024];
    FILE * pFile;

    _mkdir("GameData");
    ZeroMemory(cFn, sizeof(cFn));

    strcat(cFn, "GameData");
    strcat(cFn, "\\");
    strcat(cFn, "\\");
    strcat(cFn, "ApocalypseGUID.Txt");

    pFile = fopen(cFn, "wt");
    if (pFile == NULL)
    {
        wsprintf(cTxt, "(!) Cannot create ApocalypseGUID(%d) file", dwApocalypseGUID);
        log->info(cTxt);
    }
    else
    {
        ZeroMemory(cTemp, sizeof(cTemp));

        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "ApocalypseGUID = %d\n", dwApocalypseGUID);
        strcat(cTemp, cTxt);

        cp = (char *)cTemp;
        fwrite(cp, strlen(cp), 1, pFile);

        wsprintf(cTxt, "(O) ApocalypseGUID(%d) file created", dwApocalypseGUID);
        log->info(cTxt);
    }
    if (pFile != NULL) fclose(pFile);
}

/*void CGame::ApocalypseStarter()
{
 SYSTEMTIME SysTime;
 int i;

    if (m_bIsApocalypseMode == TRUE) return;
    if (m_bIsApocalypseStarter == FALSE) return;

    GetLocalTime(&SysTime);

    for (i = 0; i < DEF_MAXAPOCALYPSE; i++)
    if	((m_stApocalypseScheduleStart[i].iDay == SysTime.wDayOfWeek) &&
        (m_stApocalypseScheduleStart[i].iHour == SysTime.wHour) &&
        (m_stApocalypseScheduleStart[i].iMinute == SysTime.wMinute)) {
            log->info("(!) Automated apocalypse is initiated!");
            GlobalStartApocalypseMode();
            return;
    }
}*/

void CGame::ApocalypseEnder()
{
    SYSTEMTIME SysTime;
    int i;

    if (m_bIsApocalypseMode == FALSE) return;
    if (m_bIsApocalypseStarter == FALSE) return;

    GetLocalTime(&SysTime);

    for (i = 0; i < DEF_MAXAPOCALYPSE; i++)
        if ((m_stApocalypseScheduleEnd[i].iDay == SysTime.wDayOfWeek) &&
            (m_stApocalypseScheduleEnd[i].iHour == SysTime.wHour) &&
            (m_stApocalypseScheduleEnd[i].iMinute == SysTime.wMinute))
        {
            log->info("(!) Automated apocalypse is concluded!");
            GlobalEndApocalypseMode();
            return;
        }
}

/*void CGame::GlobalStartApocalypseMode(int iClientH, char *pData, DWORD dwMsgSize)
{
 char * cp, cData[120], cBuff[256];
 DWORD * dwp, dwApocalypseGUID;
 SYSTEMTIME SysTime;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
    }
    if (m_bIsApocalypseMode == TRUE) return;
    if (m_bIsHeldenianMode == TRUE) return;
    if (m_bIsCrusadeMode == TRUE) return;
    if (dwMsgSize != NULL) && (pData != NULL) {
        m_bIsApocalypseGateOpen = TRUE;
        GetLocalTime(&SysTime);
        ZeroMemory(cBuff, sizeof(cBuff));
        memcpy(cBuff, pData, dwMsgSize);

        pStrTok = new CStrTok(cBuff, seps);
        token = pStrTok->pGet();
        token = pStrTok->pGet();
        if (token != NULL) {
            var_124 = atoi(token);
            m_dwApocalypseStartHour = SysTime.wHour;
            m_dwApocalypseStartMinute = SysTime.wMinute;
        }
        else {
            return;
        }
    }
    sub_4AD0E0();
    wsprintf(cTemp, "(%s) GM Order(%s): beginapocalypse", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
    delete pStrTok;
}*/
