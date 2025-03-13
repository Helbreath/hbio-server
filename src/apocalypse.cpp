//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"

extern char G_cTxt[512];

/*void CGame::OpenApocalypseGate(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;

    //m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAliveObject;
    SendNotifyMsg(0, iClientH, DEF_NOTIFY_APOCGATEOPEN, 95, 31, 0, m_pClientList[iClientH]->m_cMapName);
}*/

void CGame::GlobalEndApocalypseMode()
{
    char * cp, cData[120];

    if (m_bIsApocalypseMode == false) return;

    memset(cData, 0, sizeof(cData));
    cp = (char *)cData;
    *cp = GSM_ENDAPOCALYPSE;
    cp++;

    LocalEndApocalypse();
}

void CGame::LocalEndApocalypse()
{
    int i;

    m_bIsApocalypseMode = false;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != 0)
        {
            SendNotifyMsg(0, i, DEF_NOTIFY_APOCGATEENDMSG, 0, 0, 0, 0);
        }
    }
    wsprintf(G_cTxt, "(!)Apocalypse Mode OFF.");
    log->info(G_cTxt);
}

void CGame::LocalStartApocalypse(uint32_t dwApocalypseGUID)
{
    int i;
    //uint32_t dwApocalypse;

    m_bIsApocalypseMode = true;

    if (dwApocalypseGUID != 0)
    {
        _CreateApocalypseGUID(dwApocalypseGUID);
        //m_dwApocalypseGUID = dwApocalypse;
    }

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != 0)
        {
            SendNotifyMsg(0, i, DEF_NOTIFY_APOCGATESTARTMSG, 0, 0, 0, 0);
            //RequestTeleportHandler(i, "0   ");
            //SendNotifyMsg(0, i, DEF_NOTIFY_APOCFORCERECALLPLAYERS, 0, 0, 0, 0);
        }
    }
    wsprintf(G_cTxt, "(!)Apocalypse Mode ON.");
    log->info(G_cTxt);
}

void CGame::_CreateApocalypseGUID(uint32_t dwApocalypseGUID)
{
    char * cp, cTxt[256], cFn[256], cTemp[1024];
    FILE * pFile;

    _mkdir("GameData");
    memset(cFn, 0, sizeof(cFn));

    strcat(cFn, "GameData");
    strcat(cFn, "\\");
    strcat(cFn, "\\");
    strcat(cFn, "ApocalypseGUID.Txt");

    pFile = fopen(cFn, "wt");
    if (pFile == 0)
    {
        wsprintf(cTxt, "(!) Cannot create ApocalypseGUID(%d) file", dwApocalypseGUID);
        log->info(cTxt);
    }
    else
    {
        memset(cTemp, 0, sizeof(cTemp));

        memset(cTxt, 0, sizeof(cTxt));
        wsprintf(cTxt, "ApocalypseGUID = %d\n", dwApocalypseGUID);
        strcat(cTemp, cTxt);

        cp = (char *)cTemp;
        fwrite(cp, strlen(cp), 1, pFile);

        wsprintf(cTxt, "(O) ApocalypseGUID(%d) file created", dwApocalypseGUID);
        log->info(cTxt);
    }
    if (pFile != 0) fclose(pFile);
}

/*void CGame::ApocalypseStarter()
{
 SYSTEMTIME SysTime;
 int i;

    if (m_bIsApocalypseMode == true) return;
    if (m_bIsApocalypseStarter == false) return;

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

    if (m_bIsApocalypseMode == false) return;
    if (m_bIsApocalypseStarter == false) return;

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

/*void CGame::GlobalStartApocalypseMode(int iClientH, char *pData, uint32_t dwMsgSize)
{
 char * cp, cData[120], cBuff[256];
 uint32_t * dwp, dwApocalypseGUID;
 SYSTEMTIME SysTime;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
    }
    if (m_bIsApocalypseMode == true) return;
    if (m_bIsHeldenianMode == true) return;
    if (m_bIsCrusadeMode == true) return;
    if (dwMsgSize != 0) && (pData != 0) {
        m_bIsApocalypseGateOpen = true;
        GetLocalTime(&SysTime);
        memset(cBuff, 0, sizeof(cBuff));
        memcpy(cBuff, pData, dwMsgSize);

        pStrTok = new CStrTok(cBuff, seps);
        token = pStrTok->pGet();
        token = pStrTok->pGet();
        if (token != 0) {
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
    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
    delete pStrTok;
}*/
