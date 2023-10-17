//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

extern char G_cTxt[512];

void CGame::_GrandMagicLaunchMsgSend(int iType, char cAttackerSide)
{
    char * cp, cBuff[120];
    WORD * wp;

    ZeroMemory(cBuff, sizeof(cBuff));
    cp = (char *)cBuff;
    *cp = GSM_GRANDMAGICLAUNCH;
    cp++;

    wp = (WORD *)cp;
    *wp = (WORD)iType;
    cp += 2;

    wp = (WORD *)cp;
    *wp = (WORD)cAttackerSide;
    cp += 2;

    bStockMsgToGateServer(cBuff, 5);
}

void CGame::CheckCrusadeResultCalculation(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_cVar == 1) return;

    if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[iClientH]->m_dwCrusadeGUID != 0))
    {
        if (m_pClientList[iClientH]->m_iWarContribution > DEF_MAXWARCONTRIBUTION) m_pClientList[iClientH]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;
        if (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID)
        {
            if (m_iCrusadeWinnerSide == 0)
            {
                m_pClientList[iClientH]->m_iExpStock += (m_pClientList[iClientH]->m_iWarContribution / 6);
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, m_pClientList[iClientH]->m_iWarContribution, NULL);
                _bCrusadeLog(DEF_CRUSADELOG_GETEXP, iClientH, (m_pClientList[iClientH]->m_iWarContribution / 6), NULL);
            }
            else
            {
                if (m_iCrusadeWinnerSide == m_pClientList[iClientH]->m_cSide)
                {
                    if (m_pClientList[iClientH]->m_iLevel <= 80)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 100;
                    }
                    else if (m_pClientList[iClientH]->m_iLevel <= 100)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 40;
                    }
                    else m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel;
                    m_pClientList[iClientH]->m_iExpStock += m_pClientList[iClientH]->m_iWarContribution;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, m_pClientList[iClientH]->m_iWarContribution, NULL);
                    _bCrusadeLog(DEF_CRUSADELOG_GETEXP, iClientH, m_pClientList[iClientH]->m_iWarContribution, NULL);
                }
                else if (m_iCrusadeWinnerSide != m_pClientList[iClientH]->m_cSide)
                {
                    if (m_pClientList[iClientH]->m_iLevel <= 80)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 100;
                    }
                    else if (m_pClientList[iClientH]->m_iLevel <= 100)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 40;
                    }
                    else m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel;
                    m_pClientList[iClientH]->m_iExpStock += m_pClientList[iClientH]->m_iWarContribution / 10;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, -1 * m_pClientList[iClientH]->m_iWarContribution, NULL);
                    _bCrusadeLog(DEF_CRUSADELOG_GETEXP, iClientH, (m_pClientList[iClientH]->m_iWarContribution / 10), NULL);
                }
            }
        }
        else
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, 0, NULL, -1);
        }
        m_pClientList[iClientH]->m_iCrusadeDuty = 0;
        m_pClientList[iClientH]->m_iWarContribution = 0;
        m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
        m_pClientList[iClientH]->m_dwSpeedHackCheckTime = timeGetTime();
        m_pClientList[iClientH]->m_iSpeedHackCheckExp = m_pClientList[iClientH]->m_iExp;
    }
}

void CGame::GlobalStartCrusadeMode()
{
    char * cp, cData[120];
    DWORD * dwp, dwCrusadeGUID;
    SYSTEMTIME SysTime;

    GetLocalTime(&SysTime);
    if (m_iLatestCrusadeDayOfWeek != -1)
    {
        if (m_iLatestCrusadeDayOfWeek == SysTime.wDayOfWeek) return;
    }
    else m_iLatestCrusadeDayOfWeek = SysTime.wDayOfWeek;

    dwCrusadeGUID = timeGetTime();

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)cData;
    *cp = GSM_BEGINCRUSADE;
    cp++;
    dwp = (DWORD *)cp;
    *dwp = dwCrusadeGUID;
    cp += 4;

    bStockMsgToGateServer(cData, 5);

    LocalStartCrusadeMode(dwCrusadeGUID);
}

void CGame::RemoveCrusadeNpcs(void)
{
    for (int i = 0; i < DEF_MAXNPCS; i++)
    {
        if (m_pNpcList[i] != NULL)
        {
            if ((m_pNpcList[i]->m_sType >= 43 && m_pNpcList[i]->m_sType <= 47) || m_pNpcList[i]->m_sType == 51)
            {
                NpcKilledHandler(NULL, NULL, i, NULL);
            }
        }
    }
}

void CGame::RemoveCrusadeRecallTime(void)
{
    for (int i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != NULL)
        {
            if (m_pClientList[i]->m_bIsWarLocation == TRUE &&
                m_pClientList[i]->m_bIsPlayerCivil == TRUE &&
                m_pClientList[i]->m_bIsInitComplete == TRUE)
            {
                m_pClientList[i]->m_iTimeLeft_ForceRecall = 0;
            }
        }
    }
}

void CGame::_CreateCrusadeGUID(DWORD dwCrusadeGUID, int iWinnerSide)
{
    char * cp, cTxt[256], cFn[256], cTemp[1024];
    FILE * pFile;

    _mkdir("GameData");
    ZeroMemory(cFn, sizeof(cFn));

    strcat(cFn, "GameData");
    strcat(cFn, "\\");
    strcat(cFn, "\\");
    strcat(cFn, "CrusadeGUID.Txt");

    pFile = fopen(cFn, "wt");
    if (pFile == NULL)
    {
        wsprintf(cTxt, "(!) Cannot create CrusadeGUID(%d) file", dwCrusadeGUID);
        log->info(cTxt);
    }
    else
    {
        ZeroMemory(cTemp, sizeof(cTemp));

        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "CrusadeGUID = %d\n", dwCrusadeGUID);
        strcat(cTemp, cTxt);

        ZeroMemory(cTxt, sizeof(cTxt));
        wsprintf(cTxt, "winner-side = %d\n", iWinnerSide);
        strcat(cTemp, cTxt);

        cp = (char *)cTemp;
        fwrite(cp, strlen(cp), 1, pFile);

        wsprintf(cTxt, "(O) CrusadeGUID(%d) file created", dwCrusadeGUID);
        log->info(cTxt);
    }
    if (pFile != NULL) fclose(pFile);
}

void CGame::ManualEndCrusadeMode(int iWinnerSide)
{
    char * cp, cData[256];
    WORD * wp;

    if (m_bIsCrusadeMode == FALSE) return;

    LocalEndCrusadeMode(iWinnerSide);

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)(cData);
    *cp = GSM_ENDCRUSADE;
    cp++;

    *cp = (char)iWinnerSide;
    cp++;

    wp = (WORD *)cp;
    *wp = NULL;
    cp += 2;

    wp = (WORD *)cp;
    *wp = NULL;
    cp += 2;

    wp = (WORD *)cp;
    *wp = NULL;
    cp += 2;

    cp += 10;

    bStockMsgToGateServer(cData, 18);
}

void CGame::RemoveCrusadeStructures()
{
    int i;

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != NULL)
        {
            switch (m_pNpcList[i]->m_sType)
            {
                case 36:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                    DeleteNpc(i);
                    break;
            }
        }
}

void CGame::CheckCommanderConstructionPoint(int iClientH)
{
    char * cp, cData[120];
    int * ip, i;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_bIsCrusadeMode == FALSE) return;
    if (m_pClientList[iClientH]->m_iConstructionPoint <= 0) return;

    switch (m_pClientList[iClientH]->m_iCrusadeDuty)
    {
        case 1:
        case 2:
            for (i = 0; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iCrusadeDuty == 3) &&
                    (m_pClientList[i]->m_iGuildGUID == m_pClientList[iClientH]->m_iGuildGUID))
                {
                    m_pClientList[i]->m_iConstructionPoint += m_pClientList[iClientH]->m_iConstructionPoint;
                    m_pClientList[i]->m_iWarContribution += (m_pClientList[iClientH]->m_iConstructionPoint / 10);

                    if (m_pClientList[i]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                        m_pClientList[i]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                    if (m_pClientList[i]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                        m_pClientList[i]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, NULL, NULL);
                    m_pClientList[iClientH]->m_iConstructionPoint = 0;
                    return;
                }

            ZeroMemory(cData, sizeof(cData));
            cp = (char *)cData;
            *cp = GSM_CONSTRUCTIONPOINT;
            cp++;
            ip = (int *)cp;
            *ip = m_pClientList[iClientH]->m_iGuildGUID;
            cp += 4;
            ip = (int *)cp;
            *ip = m_pClientList[iClientH]->m_iConstructionPoint;
            cp += 4;
            bStockMsgToGateServer(cData, 9);

            m_pClientList[iClientH]->m_iConstructionPoint = 0;
            break;

        case 3:

            break;
    }
}

BOOL CGame::__bSetConstructionKit(int iMapIndex, int dX, int dY, int iType, int iTimeCost, int iClientH)
{
    int iNamingValue, ix, iy, tX, tY;
    char cNpcName[30], cName[30], cNpcWaypoint[11], cOwnerType;
    short sOwnerH;

    if ((m_bIsCrusadeMode == FALSE) || (m_pClientList[iClientH]->m_iCrusadeDuty != 2)) return FALSE;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalCrusadeStructures >= DEF_MAXCRUSADESTRUCTURES)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOMORECRUSADESTRUCTURE, NULL, NULL, NULL, NULL);
        return FALSE;
    }

    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue == -1)
    {
    }
    else
    {

        for (ix = dX - 3; ix <= dX + 5; ix++)
            for (iy = dY - 3; iy <= dX + 5; iy++)
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                if ((sOwnerH != NULL) && (cOwnerType == DEF_OWNERTYPE_NPC) && (m_pNpcList[sOwnerH]->m_cActionLimit == 5)) return FALSE;
            }

        ZeroMemory(cNpcName, sizeof(cNpcName));
        if (m_pClientList[iClientH]->m_cSide == 1)
        {
            switch (iType)
            {
                case 1: strcpy(cNpcName, "AGT-Aresden"); break;
                case 2: strcpy(cNpcName, "CGT-Aresen"); break;
                case 3: strcpy(cNpcName, "MS-Aresden"); break;
                case 4: strcpy(cNpcName, "DT-Aresden"); break;
            }
        }
        else if (m_pClientList[iClientH]->m_cSide == 2)
        {
            switch (iType)
            {
                case 1: strcpy(cNpcName, "AGT-Elvine"); break;
                case 2: strcpy(cNpcName, "CGT-Elvine"); break;
                case 3: strcpy(cNpcName, "MS-Elvine"); break;
                case 4: strcpy(cNpcName, "DT-Elvine"); break;
            }
        }
        else return FALSE;

        ZeroMemory(cName, sizeof(cName));
        wsprintf(cName, "XX%d", iNamingValue);
        cName[0] = '_';
        cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));

        tX = (int)dX;
        tY = (int)dY;
        if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, (rand() % 9),
            DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, NULL, NULL, -1, FALSE, FALSE) == FALSE)
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
        else
        {
            wsprintf(G_cTxt, "Structure(%s) construction begin(%d,%d)!", cNpcName, tX, tY);
            log->info(G_cTxt);
            return TRUE;
        }
    }

    return FALSE;
}

void CGame::LocalStartCrusadeMode(DWORD dwCrusadeGUID)
{
    int i;

    if (m_bIsCrusadeMode == TRUE) return;
    m_bIsCrusadeMode = TRUE;
    m_iCrusadeWinnerSide = 0;

    if (dwCrusadeGUID != NULL)
    {
        _CreateCrusadeGUID(dwCrusadeGUID, NULL);
        m_dwCrusadeGUID = dwCrusadeGUID;
    }

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
        {
            m_pClientList[i]->m_iCrusadeDuty = 0;
            m_pClientList[i]->m_iConstructionPoint = 0;
            m_pClientList[i]->m_dwCrusadeGUID = m_dwCrusadeGUID;
            SendNotifyMsg(NULL, i, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[i]->m_iCrusadeDuty, NULL, NULL);
        }

    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != NULL) m_pMapList[i]->RestoreStrikePoints();

    CreateCrusadeStructures();

    log->info("(!)Crusade Mode ON.");
    _bCrusadeLog(DEF_CRUSADELOG_STARTCRUSADE, NULL, NULL, NULL);
}

void CGame::LocalEndCrusadeMode(int iWinnerSide)
{
    int i;

    wsprintf(G_cTxt, "LocalEndCrusadeMode(%d)", iWinnerSide);
    log->info(G_cTxt);

    if (m_bIsCrusadeMode == FALSE) return;
    m_bIsCrusadeMode = FALSE;

    log->info("(!)Crusade Mode OFF.");

    RemoveCrusadeStructures();

    RemoveCrusadeNpcs();

    _CreateCrusadeGUID(m_dwCrusadeGUID, iWinnerSide);
    m_iCrusadeWinnerSide = iWinnerSide;
    m_iLastCrusadeWinner = iWinnerSide;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
        {
            m_pClientList[i]->m_iCrusadeDuty = 0;
            m_pClientList[i]->m_iConstructionPoint = 0;
            SendNotifyMsg(NULL, i, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, NULL, NULL, m_iCrusadeWinnerSide);
        }
    RemoveCrusadeRecallTime();

    if (iWinnerSide == 2)
    {
        _bCrusadeLog(DEF_CRUSADELOG_ENDCRUSADE, FALSE, FALSE, "Elvine Win!");
    }
    else if (iWinnerSide == 1)
    {
        _bCrusadeLog(DEF_CRUSADELOG_ENDCRUSADE, FALSE, FALSE, "Aresden Win!");
    }
    else
    {
        _bCrusadeLog(DEF_CRUSADELOG_ENDCRUSADE, FALSE, FALSE, "Drawn!");
    }

    if (m_iMiddlelandMapIndex != -1)
    {
    }
}

void CGame::CreateCrusadeStructures()
{
    int i, z, tX, tY, iNamingValue;
    char cName[6], cNpcName[30], cNpcWayPoint[11];

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cNpcName, sizeof(cNpcName));
    ZeroMemory(cNpcWayPoint, sizeof(cNpcWayPoint));

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
        if (m_stCrusadeStructures[i].cType != NULL)
        {
            for (z = 0; z < DEF_MAXMAPS; z++)
                if ((m_pMapList[z] != NULL) && (strcmp(m_pMapList[z]->m_cName, m_stCrusadeStructures[i].cMapName) == 0))
                {
                    iNamingValue = m_pMapList[z]->iGetEmptyNamingValue();
                    if (iNamingValue == -1)
                    {
                    }
                    else
                    {
                        wsprintf(cName, "XX%d", iNamingValue);
                        cName[0] = '_';
                        cName[1] = z + 65;

                        switch (m_stCrusadeStructures[i].cType)
                        {
                            case 36:
                                if (strcmp(m_pMapList[z]->m_cName, "aresden") == 0)
                                    strcpy(cNpcName, "AGT-Aresden");
                                else if (strcmp(m_pMapList[z]->m_cName, "elvine") == 0)
                                    strcpy(cNpcName, "AGT-Elvine");
                                break;

                            case 37:
                                if (strcmp(m_pMapList[z]->m_cName, "aresden") == 0)
                                    strcpy(cNpcName, "CGT-Aresden");
                                else if (strcmp(m_pMapList[z]->m_cName, "elvine") == 0)
                                    strcpy(cNpcName, "CGT-Elvine");
                                break;

                            case 40:
                                if (strcmp(m_pMapList[z]->m_cName, "aresden") == 0)
                                    strcpy(cNpcName, "ESG-Aresden");
                                else if (strcmp(m_pMapList[z]->m_cName, "elvine") == 0)
                                    strcpy(cNpcName, "ESG-Elvine");
                                break;

                            case 41:
                                if (strcmp(m_pMapList[z]->m_cName, "aresden") == 0)
                                    strcpy(cNpcName, "GMG-Aresden");
                                else if (strcmp(m_pMapList[z]->m_cName, "elvine") == 0)
                                    strcpy(cNpcName, "GMG-Elvine");
                                break;

                            case 42:
                                strcpy(cNpcName, "ManaStone");
                                break;

                            default:
                                strcpy(cNpcName, m_pNpcConfigList[m_stCrusadeStructures[i].cType]->m_cNpcName);
                                break;
                        }

                        tX = (int)m_stCrusadeStructures[i].dX;
                        tY = (int)m_stCrusadeStructures[i].dY;
                        if (bCreateNewNpc(cNpcName, cName, m_pMapList[z]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM,
                            &tX, &tY, cNpcWayPoint, NULL, NULL, -1, FALSE) == FALSE)
                        {
                            m_pMapList[z]->SetNamingValueEmpty(iNamingValue);
                        }
                        else
                        {
                            wsprintf(G_cTxt, "(!) Creating Crusade Structure(%s) at %s(%d, %d)", cNpcName, m_stCrusadeStructures[i].cMapName, tX, tY);
                            log->info(G_cTxt);
                        }
                    }
                }
        }
}
