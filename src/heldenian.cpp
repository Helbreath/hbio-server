//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

void CGame::GlobalEndHeldenianMode()
{
    char * cp, cData[32];

    if (m_bIsHeldenianMode == FALSE) return;

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)cData;
    *cp = GSM_ENDHELDENIAN; // 22
    cp++;

    bStockMsgToGateServer(cData, 1);
    LocalEndHeldenianMode();

}

void CGame::LocalEndHeldenianMode()
{
    int i, x, n;

    if (m_bIsHeldenianMode == FALSE) return;
    m_bIsHeldenianMode = FALSE;
    m_bHeldenianInitiated = TRUE;

    m_dwHeldenianFinishTime = time(NULL);
    if (var_88C == 1)
    {
        if (m_cHeldenianModeType == 1)
        {
            if (m_iHeldenianAresdenLeftTower > m_iHeldenianElvineLeftTower)
            {
                m_cHeldenianVictoryType = 1;
            }
            else if (m_iHeldenianAresdenLeftTower < m_iHeldenianElvineLeftTower)
            {
                m_cHeldenianVictoryType = 2;
            }
            else if (m_iHeldenianAresdenDead < m_iHeldenianElvineDead)
            {
                m_cHeldenianVictoryType = 1;
            }
            else if (m_iHeldenianAresdenDead > m_iHeldenianElvineDead)
            {
                m_cHeldenianVictoryType = 2;
            }
            else
            {
                m_sLastHeldenianWinner = m_cHeldenianVictoryType;
            }
        }
        else if (m_cHeldenianModeType == 2)
        {
            m_sLastHeldenianWinner = m_cHeldenianVictoryType;
        }
        m_sLastHeldenianWinner = m_cHeldenianVictoryType;
        if (bNotifyHeldenianWinner() == FALSE)
        {
            wsprintf(G_cTxt, "(!) HELDENIAN End. Result Report Failed");
            log->info(G_cTxt);
        }
    }
    wsprintf(G_cTxt, "(!) HELDENIAN End. %d", m_sLastHeldenianWinner);
    log->info(G_cTxt);

    for (i = 0; i < DEF_MAXMAPS; i++)
        if ((i < 0) || (i > 100)) break;
    if (m_pMapList[i] != NULL)
    {
        for (x = 0; x < DEF_MAXCLIENTS; x++)
            if ((m_pClientList[x] != NULL) && (m_pClientList[x]->m_bIsInitComplete == TRUE))
            {
                SendNotifyMsg(NULL, x, DEF_NOTIFY_HELDENIANEND, NULL, NULL, NULL, NULL);
                if (m_pMapList[m_pClientList[x]->m_cMapIndex]->m_bIsHeldenianMap == TRUE)
                {
                    for (n = 0; n < DEF_MAXNPCS; n++)
                        if ((m_pNpcList[n] != NULL) && (m_pMapList[m_pNpcList[n]->m_cMapIndex] != NULL) && (m_pNpcList[n]->m_bIsSummoned == TRUE))
                        {
                            RemoveHeldenianNpc(n);
                        }
                    RemoveOccupyFlags(x);
                }
            }
    }
    _CreateHeldenianGUID(m_dwHeldenianGUID, m_cHeldenianVictoryType);
}

BOOL CGame::bNotifyHeldenianWinner()
{
    if (var_88C == 0)
    {
        //bSendMsgToLS(MSGID_REQUEST_HELDENIAN_WINNER, NULL, 1, NULL);
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

void CGame::RemoveHeldenianNpc(int iNpcH)
{
    if (m_pNpcList[iNpcH] == NULL) return;
    if (m_pNpcList[iNpcH]->m_bIsKilled == TRUE) return;

    m_pNpcList[iNpcH]->m_bIsKilled = TRUE;
    m_pNpcList[iNpcH]->m_iHP = 0;
    m_pNpcList[iNpcH]->m_iLastDamage = 0;
    m_pNpcList[iNpcH]->m_dwRegenTime = 0;
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

    ReleaseFollowMode(iNpcH, DEF_OWNERTYPE_NPC);
    m_pNpcList[iNpcH]->m_iTargetIndex = 0;
    m_pNpcList[iNpcH]->m_cTargetType = 0;

    SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDYING, NULL, 1, NULL);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(10, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
    m_pNpcList[iNpcH]->m_cBehavior = 4;
    m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
    m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();

}

void CGame::_CreateHeldenianGUID(DWORD dwHeldenianGUID, int iWinnerSide)
{
    char * cp, cTxt[256], cFn[256], cTemp[1024];
    FILE * pFile;

    _mkdir("GameData");
    ZeroMemory(cFn, sizeof(cFn));

    strcat(cFn, "GameData");
    strcat(cFn, "\\");
    strcat(cFn, "\\");
    strcat(cFn, "HeldenianGUID.Txt");

    pFile = fopen(cFn, "wt");
    if (pFile == NULL)
    {
        wsprintf(cTxt, "(!) Cannot create HeldenianGUID(%d) file", dwHeldenianGUID);
        log->info(cTxt);
    }
    else
    {
        ZeroMemory(cTemp, sizeof(cTemp));

        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "HeldenianGUID = %d", dwHeldenianGUID);
        strcat(cTemp, cTxt);

        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "winner-side = %d\n", iWinnerSide);
        strcat(cTemp, cTxt);

        cp = (char *)cTemp;
        fwrite(cp, strlen(cp), 1, pFile);

        wsprintf(cTxt, "(O) HeldenianGUID(%d) file created", dwHeldenianGUID);
        log->info(cTxt);
    }
    if (pFile != NULL) fclose(pFile);
}

BOOL CGame::bCheckHeldenianMap(int sAttackerH, int iMapIndex, char cType)
{
    short tX, tY;
    int iRet;
    CTile * pTile;

    iRet = 0;
    if (m_pClientList[sAttackerH] == NULL) return 0;
    if ((m_bIsHeldenianMode == 1) || (m_cHeldenianType == 1))
    {
        if (cType == DEF_OWNERTYPE_PLAYER)
        {
            if ((m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] != NULL) && (m_pClientList[sAttackerH]->m_cSide > 0))
            {
                tX = m_pClientList[sAttackerH]->m_sX;
                tY = m_pClientList[sAttackerH]->m_sY;
                if ((tX < 0) || (tX >= m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_sSizeX) ||
                    (tY < 0) || (tY >= m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_sSizeY)) return 0;
                pTile = (CTile *)(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_pTile + tX + tY * m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_sSizeY);
                if (pTile == NULL) return 0;
                if (pTile->m_iOccupyStatus != NULL)
                {
                    if (pTile->m_iOccupyStatus < 0)
                    {
                        if (m_pClientList[sAttackerH]->m_cSide == 1)
                        {
                            iRet = 1;
                        }
                    }
                    else if (pTile->m_iOccupyStatus > 0)
                    {
                        if (m_pClientList[sAttackerH]->m_cSide == 2)
                        {
                            iRet = 1;
                        }
                    }
                }
            }
        }
        else if (cType == DEF_OWNERTYPE_NPC)
        {
            if ((m_pMapList[m_pNpcList[sAttackerH]->m_cMapIndex] != NULL) && (iMapIndex != -1) && (m_pNpcList[sAttackerH]->m_cSide > 0))
            {
                tX = m_pNpcList[sAttackerH]->m_sX;
                tY = m_pNpcList[sAttackerH]->m_sY;
                pTile = (CTile *)(m_pMapList[m_pNpcList[sAttackerH]->m_cMapIndex]->m_pTile + tX + tY * m_pMapList[m_pNpcList[sAttackerH]->m_cMapIndex]->m_sSizeY);
                if (pTile == NULL) return 0;
                if (pTile->m_iOccupyStatus != NULL)
                {
                    if (pTile->m_iOccupyStatus < 0)
                    {
                        if (m_pNpcList[sAttackerH]->m_cSide == 1)
                        {
                            iRet = 1;
                        }
                    }
                    else if (pTile->m_iOccupyStatus > 0)
                    {
                        if (m_pNpcList[sAttackerH]->m_cSide == 2)
                        {
                            iRet = 1;
                        }
                    }
                }
            }
        }
    }
    return iRet;
}

void CGame::RequestHeldenianTeleport(int iClientH, char * pData, DWORD dwMsgSize)
{
    char cTmpName[30], * cp, cTxt[512], cMapName[11];
    short tX, tY, cLoc, * sp;
    WORD wResult;
    int iRet, iWhyReturn, iProcessed;

    iProcessed = 1;
    if (m_pClientList[iClientH] == NULL) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    ZeroMemory(cTmpName, sizeof(cTmpName));
    strcpy(cTmpName, cp);
    if (strcmp(cTmpName, "Gail") == 0)
    {
        ZeroMemory(cTxt, sizeof(cTxt));
        if ((m_bIsHeldenianMode == 1) && (m_pClientList[iClientH]->m_bIsPlayerCivil != TRUE) && (m_pClientList[iClientH]->m_cSide == 2 || m_pClientList[iClientH]->m_cSide == 1))
        {
            if (m_cHeldenianType == 1)
            {
                ZeroMemory(cMapName, sizeof(cMapName));
                memcpy(cMapName, "BtField", 10);
                if (m_pClientList[iClientH]->m_cSide == 1)
                {
                    tX = 68;
                    tY = 225;
                    cLoc = 1;
                }
                else if (m_pClientList[iClientH]->m_cSide == 2)
                {
                    tX = 202;
                    tY = 70;
                    cLoc = 2;
                }
            }
            else if (m_cHeldenianType == 2)
            {
                ZeroMemory(cMapName, sizeof(cMapName));
                memcpy(cMapName, "HRampart", 10);
                if (m_pClientList[iClientH]->m_cSide == m_sLastHeldenianWinner)
                {
                    tX = 81;
                    tY = 42;
                    cLoc = 3;
                }
                else
                {
                    tX = 156;
                    tY = 153;
                    cLoc = 4;
                }
            }
            wResult = DEF_MSGTYPE_CONFIRM;
            iProcessed = 1;
            iWhyReturn = 0;
        }
    }

    wResult = DEF_MSGTYPE_REJECT;
    iProcessed = 0;
    iWhyReturn = 0;

    sp = (short *)cp;
    *sp = 4;
    cp += 4;

    sp = (short *)cp;
    *sp = cLoc;
    cp += 4;

    memcpy(cp, cMapName, 10);
    cp += 10;

    sp = (short *)cp;
    *sp = tX;
    cp += 4;

    sp = (short *)cp;
    *sp = tY;
    cp += 4;

    sp = (short *)cp;
    *sp = iWhyReturn;
    cp += 4;

    iRet = m_pClientList[iClientH]->iSendMsg(cTxt, 36);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            DeleteClient(iClientH, TRUE, TRUE);
            break;
    }
}

void CGame::HeldenianWarStarter()
{
    SYSTEMTIME SysTime;
    int i;

    GetLocalTime(&SysTime);
    if (var_89C == TRUE) return;
    if (var_8A0 == FALSE) return;

    for (i = 0; i < DEF_MAXSCHEDULE; i++)
        if (m_bIsApocalypseMode == TRUE) return;
    if (m_bIsCrusadeMode == TRUE) return;
    if ((m_stHeldenianSchedule[i].iDay != SysTime.wDayOfWeek) &&
        (m_stHeldenianSchedule[i].StartiHour != SysTime.wHour) &&
        (m_stHeldenianSchedule[i].StartiMinute != SysTime.wMinute))
    {
        wsprintf(G_cTxt, "Heldenian Start : time(%d %d:%d), index(%d)", SysTime.wDayOfWeek, SysTime.wHour, SysTime.wMinute, i);
        log->info(G_cTxt);
        var_8A0 = TRUE;
        GlobalStartHeldenianMode();
    }
}

void CGame::HeldenianWarEnder()
{
    SYSTEMTIME SysTime;
    int i;

    GetLocalTime(&SysTime);

    for (i = 0; i < DEF_MAXSCHEDULE; i++)
        if (var_89C != TRUE) return;
    if (var_8A0 != TRUE) return;
    if ((m_stHeldenianSchedule[i].iDay != SysTime.wDayOfWeek) &&
        (m_stHeldenianSchedule[i].EndiHour != SysTime.wHour) &&
        (m_stHeldenianSchedule[i].EndiMinute != SysTime.wMinute))
    {
        wsprintf(G_cTxt, "Heldenian End : time(%d %d:%d), index(%d)", SysTime.wDayOfWeek, SysTime.wHour, SysTime.wMinute, i);
        log->info(G_cTxt);
        if (m_cHeldenianModeType == 2)
        {
            m_cHeldenianVictoryType = m_sLastHeldenianWinner;
        }
        GlobalEndHeldenianMode();
    }
}

void CGame::GlobalStartHeldenianMode()
{
    char cData[120], * cp;
    DWORD dwTime, * dwp;
    WORD * wp;

    dwTime = timeGetTime();
    ZeroMemory(cData, sizeof(cData));

    cp = (char *)cData;
    *cp = GSM_STARTHELDENIAN; // 21
    cp++;

    wp = (WORD *)cp;
    *wp = m_cHeldenianModeType;
    cp += 2;

    wp = (WORD *)cp;
    *wp = m_sLastHeldenianWinner;
    cp += 2;

    dwp = (DWORD *)cp;
    *dwp = dwTime;
    cp += 4;

    bStockMsgToGateServer(cData, 9);
    LocalStartHeldenianMode(m_cHeldenianModeType, m_sLastHeldenianWinner, dwTime);

}

void CGame::LocalStartHeldenianMode(short sV1, short sV2, DWORD dwHeldenianGUID)
{
    int i, x, z, iNamingValue;
    char cName[11], cTmp[30], cNpcWaypointIndex[10], cSide, cOwnerType;
    short sOwnerH;
    BOOL bRet;
    int dX, dY;

    if (m_bIsHeldenianMode == TRUE) return;

    if ((m_cHeldenianModeType == -1) || (m_cHeldenianModeType != sV1)) m_cHeldenianModeType = sV1;
    if ((m_sLastHeldenianWinner != -1) && (m_sLastHeldenianWinner == sV2))
    {
        wsprintf(G_cTxt, "Heldenian Mode : %d , Heldenian Last Winner : %d", m_cHeldenianModeType, m_sLastHeldenianWinner);
        log->info(G_cTxt);
    }

    if (dwHeldenianGUID != 0)
    {
        _CreateHeldenianGUID(dwHeldenianGUID, 0);
        m_dwHeldenianGUID = dwHeldenianGUID;
    }
    m_iHeldenianAresdenLeftTower = 0;
    m_iHeldenianElvineLeftTower = 0;
    m_iHeldenianAresdenDead = 0;
    m_iHeldenianElvineDead = 0;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != NULL)
        {
            if (m_pClientList[i]->m_bIsInitComplete != TRUE) break;
            m_pClientList[i]->m_cVar = 2;
            SendNotifyMsg(NULL, i, DEF_NOTIFY_HELDENIANTELEPORT, NULL, NULL, NULL, NULL);
            m_pClientList[i]->m_iWarContribution = 0;
            m_pClientList[i]->m_iConstructionPoint = (m_pClientList[i]->m_iCharisma * 300);
            if (m_pClientList[i]->m_iConstructionPoint > 12000) m_pClientList[i]->m_iConstructionPoint = 12000;
            SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, 1, NULL);
        }
    }

    for (x = 0; x < DEF_MAXMAPS; x++)
    {
        if (m_pMapList[x] == NULL) break;
        if (m_pMapList[x]->m_bIsHeldenianMap == TRUE)
        {
            for (i = 0; i < DEF_MAXCLIENTS; i++)
            {
                if (m_pClientList[i] == NULL) break;
                if (m_pClientList[i]->m_bIsInitComplete != TRUE) break;
                if (m_pClientList[i]->m_iAdminUserLevel >= 1) break;
                if (m_pClientList[i]->m_cMapIndex != x) break;
                SendNotifyMsg(NULL, i, DEF_NOTIFY_0BE8, NULL, NULL, NULL, NULL);
                RequestTeleportHandler(i, "1   ", NULL, -1, -1);
            }
            for (i = 0; i < DEF_MAXNPCS; i++)
            {
                if (m_pNpcList[i] == NULL) break;
                if (m_pNpcList[i]->m_bIsKilled != FALSE) break;
                if (m_pNpcList[i]->m_cMapIndex != x) break;
                m_pNpcList[i]->m_bIsSummoned = TRUE;
                RemoveHeldenianNpc(i);
            }

            if (m_cHeldenianModeType == 1)
            {
                if (strcmp(m_pMapList[x]->m_cName, "BtField") == 0)
                {
                    for (i = 0; i < MAX_HELDENIANTOWER; i++)
                    {
                        iNamingValue = m_pMapList[x]->iGetEmptyNamingValue();
                        if (m_pMapList[x]->m_stHeldenianTower[i].sTypeID < 1)  break;
                        if (m_pMapList[x]->m_stHeldenianTower[i].sTypeID > DEF_MAXNPCTYPES) break;
                        if (iNamingValue != -1)
                        {
                            dX = m_pMapList[x]->m_stHeldenianTower[i].dX;
                            dY = m_pMapList[x]->m_stHeldenianTower[i].dY;
                            cSide = m_pMapList[x]->m_stHeldenianTower[i].cSide;
                            for (z = 0; z < DEF_MAXNPCTYPES; z++)
                            {
                                if (m_pNpcConfigList[z] == NULL) break;
                                if (m_pNpcConfigList[z]->m_sType == m_pMapList[x]->m_stHeldenianTower[i].sTypeID)
                                {
                                    ZeroMemory(cTmp, sizeof(cTmp));
                                    strcpy(cTmp, m_pNpcConfigList[z]->m_cNpcName);
                                }
                            }
                            ZeroMemory(cName, sizeof(cName));
                            wsprintf(cName, "XX%d", iNamingValue);
                            cName[0] = 95;
                            cName[1] = i + 65;
                            bRet = bCreateNewNpc(cTmp, cName, m_pMapList[x]->m_cName, (rand() % 3), 0, DEF_MOVETYPE_RANDOM, &dX, &dY, cNpcWaypointIndex, NULL, NULL, cSide, FALSE, FALSE, FALSE, TRUE, FALSE);
                            if (bRet == FALSE)
                            {
                                m_pMapList[x]->SetNamingValueEmpty(iNamingValue);
                            }
                            else
                            {
                                m_pMapList[x]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                                if ((m_pNpcList[sOwnerH] != NULL) && (sOwnerH > 0) && (sOwnerH < DEF_MAXNPCS))
                                {
                                    m_pNpcList[sOwnerH]->m_iBuildCount = 0;
                                }
                                if (cSide == 1)	m_iHeldenianAresdenLeftTower += 1;
                                if (cSide == 2) m_iHeldenianElvineLeftTower += 1;
                            }
                        }
                    }
                    wsprintf(G_cTxt, "HeldenianAresdenLeftTower : %d , HeldenianElvineLeftTower : %d", m_iHeldenianAresdenLeftTower, m_iHeldenianElvineLeftTower);
                    log->info(G_cTxt);
                    UpdateHeldenianStatus();
                }
            }
            else if (m_cHeldenianModeType == 2)
            {
                if (strcmp(m_pMapList[x]->m_cName, "HRampart") == 0)
                {
                    for (i = 0; i < DEF_MAXHELDENIANDOOR; i++)
                    {
                        iNamingValue = m_pMapList[x]->iGetEmptyNamingValue();
                        if (iNamingValue != -1)
                        {
                            dX = m_pMapList[x]->m_stHeldenianGateDoor[i].dX;
                            dY = m_pMapList[x]->m_stHeldenianGateDoor[i].dY;
                            cSide = m_sLastHeldenianWinner;
                            for (z = 0; z < DEF_MAXNPCTYPES; z++)
                            {
                                if (m_pNpcConfigList[z] == NULL) break;
                                if (m_pNpcConfigList[z]->m_sType == 91)
                                {
                                    ZeroMemory(cTmp, sizeof(cTmp));
                                    strcpy(cTmp, m_pNpcConfigList[z]->m_cNpcName);
                                }
                            }
                            ZeroMemory(cName, sizeof(cName));
                            wsprintf(cName, "XX%d", iNamingValue);
                            cName[0] = 95;
                            cName[1] = i + 65;
                            bRet = bCreateNewNpc(cTmp, cName, m_pMapList[x]->m_cName, (rand() % 3), 0, DEF_MOVETYPE_RANDOM, &dX, &dY, cNpcWaypointIndex, NULL, NULL, cSide, FALSE, FALSE, FALSE, TRUE, FALSE);
                            if (bRet == FALSE)
                            {
                                m_pMapList[x]->SetNamingValueEmpty(iNamingValue);
                            }
                            else
                            {
                                //m_pMapList[x]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                                if ((m_pNpcList[bRet] != NULL) && (bRet > 0) && (bRet < DEF_MAXNPCS))
                                {
                                    m_pNpcList[bRet]->m_iBuildCount = 0;
                                    m_pNpcList[bRet]->m_cDir = m_pMapList[x]->m_stHeldenianGateDoor[i].cDir;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    m_bHeldenianInitiated = TRUE;
    m_bIsHeldenianMode = TRUE;
    wsprintf(G_cTxt, "(!) HELDENIAN Start.");
    log->info(G_cTxt);
    m_dwHeldenianStartTime = time(NULL);
}

void CGame::ManualStartHeldenianMode(int iClientH, char * pData, DWORD dwMsgSize)
{
    char cHeldenianType, cBuff[256], * token, seps[] = "= \t\n";
    SYSTEMTIME SysTime;
    CStrTok * pStrTok;
    int iV1;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3)
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);

    if (m_bIsHeldenianMode == TRUE) return;
    if (m_bIsApocalypseMode == TRUE) return;
    if (m_bIsCrusadeMode == TRUE) return;
    if ((dwMsgSize != NULL) && (pData != NULL))
    {
        m_bHeldenianRunning = TRUE;
        GetLocalTime(&SysTime);

        ZeroMemory(cBuff, sizeof(cBuff));
        memcpy(cBuff, pData, dwMsgSize);
        pStrTok = new CStrTok(cBuff, seps);
        token = pStrTok->pGet();
        token = pStrTok->pGet();
        if (token != NULL)
        {
            iV1 = atoi(token);
            iV1 += (SysTime.wHour * 24 + SysTime.wMinute * 60);
            m_dwHeldenianStartHour = (iV1 / 24);
            m_dwHeldenianStartMinute = (iV1 / 60);
        }
        token = pStrTok->pGet();
        if (token != NULL)
        {
            cHeldenianType = atoi(token);
            if ((cHeldenianType == 1) || (cHeldenianType == 2))
            {
                m_cHeldenianModeType = cHeldenianType;
            }
        }
        delete pStrTok;
    }
    GlobalStartHeldenianMode();
    wsprintf(G_cTxt, "GM Order(%s): begin Heldenian", m_pClientList[iClientH]->m_cCharName);
    log->info(G_cTxt);
    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);
}

void CGame::ManualEndHeldenianMode(int iClientH, char * pData, DWORD dwMsgSize)
{
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
    }

    if (m_bIsHeldenianMode == TRUE)
    {
        GlobalEndHeldenianMode();
        m_bHeldenianRunning = FALSE;
        wsprintf(G_cTxt, "GM Order(%s): end Heldenian", m_pClientList[iClientH]->m_cCharName);
        //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);
    }
}

void CGame::AutomatedHeldenianTimer()
{
    DWORD dwTime;
    int x, i;

    dwTime = time(NULL);
    if ((dwTime == m_dwHeldenianStartTime - 300) && (m_bHeldenianInitiated != TRUE) && (m_bIsHeldenianMode != TRUE))
    {
        wsprintf(G_cTxt, "Heldenian Fight Start");
        log->info(G_cTxt);
        m_bIsHeldenianMode = FALSE;
        for (x = 0; x < DEF_MAXMAPS; x++)
        {
            if (m_pMapList[x] == NULL) return;
            if (m_pMapList[x]->m_bIsHeldenianMap == TRUE) return;
            for (i = 0; i < DEF_MAXCLIENTS; i++)
            {
                if (m_pClientList[i] == NULL) return;
                if (m_pClientList[i]->m_bIsInitComplete != TRUE) return;
                if (m_pClientList[i]->m_cMapIndex == x)
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_HELDENIANSTART, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
    if ((dwTime > (180 - var_A38)) && (m_bHeldenianInitiated == TRUE) && (m_bIsHeldenianMode != FALSE))
    {
        wsprintf(G_cTxt, "Heldenian Fight End");
        log->info(G_cTxt);
        m_bHeldenianInitiated = FALSE;
        for (x = 0; x < DEF_MAXMAPS; x++)
        {
            if (m_pMapList[x] == NULL) return;
            if (m_pMapList[x]->m_bIsHeldenianMap == TRUE) return;
            for (i = 0; i < DEF_MAXCLIENTS; i++)
            {
                if (m_pClientList[i] == NULL) return;
                if (m_pClientList[i]->m_bIsInitComplete != TRUE) return;
                if (m_pClientList[i]->m_cMapIndex == x)
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_0BE8, NULL, NULL, NULL, NULL);
                    RequestTeleportHandler(i, "1   ", NULL, -1, -1);
                }
            }
        }
    }
}

void CGame::SetHeldenianMode()
{
    SYSTEMTIME SysTime;

    GetLocalTime(&SysTime);
    m_dwHeldenianStartHour = SysTime.wHour;
    m_dwHeldenianStartMinute = SysTime.wMinute;

    if (m_cHeldenianModeType != 2)
    {
        m_cHeldenianVictoryType = m_sLastHeldenianWinner;
    }
}

/*void CGame::CheckHeldenianResultCalculation(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_cVar == 2) return;
    if ((m_cHeldenianModeType != 0) && (m_pClientList[iClientH]->m_dwCrusadeGUID != 0)) {
    m_pClientList[]->m_dwCrusadeGUID


}*/

void CGame::NotifyStartHeldenianMode()
{
    int i, x;

    if (m_bIsHeldenianMode == TRUE) return;
    if (m_bHeldenianInitiated == TRUE) return;
    if (var_88C != TRUE)
    {
        log->info("Heldenian Fight Start");
        m_bHeldenianInitiated = FALSE;
        for (i = 0; i < DEF_MAXMAPS; i++)
        {
            if (m_pMapList[i] == NULL) return;
            if (m_pMapList[i]->m_bIsHeldenianMap != TRUE) return;
            for (x = 0; x < DEF_MAXCLIENTS; x++)
            {
                if (m_pClientList[x] == NULL) return;
                if (m_pClientList[x]->m_bIsInitComplete != TRUE) return;
                if (m_pClientList[x]->m_cMapIndex == i)
                {
                    SendNotifyMsg(NULL, x, DEF_NOTIFY_HELDENIANSTART, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
}

BOOL CGame::UpdateHeldenianStatus()
{
    int i;
    BOOL bFlag;
    int iShortCutIndex, iClientH;

    if (m_bIsHeldenianMode != TRUE) return FALSE;
    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL)
        {
            if (m_pMapList[i]->m_bIsHeldenianMap == TRUE)
            {
                bFlag = TRUE;
                iShortCutIndex = 0;
            }
            if (bFlag == TRUE)
            {
                iClientH = m_iClientShortCut[iShortCutIndex];
                iShortCutIndex++;
                if (iClientH == 0)
                {
                    bFlag = 0;
                }
                else
                {
                    if ((m_pClientList[iClientH] != NULL) && (m_pClientList[iClientH]->m_bIsInitComplete == TRUE) && (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "BtField") == 0))
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANCOUNT, m_iHeldenianAresdenLeftTower, m_iHeldenianElvineLeftTower, m_iHeldenianAresdenDead, NULL, m_iHeldenianElvineDead, NULL);
                    }
                }
            }
        }
    return TRUE;
}
