//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include "Map.h"

extern char G_cTxt[512];
extern char	G_cData50000[50000];

void CGame::GlobalStartCrusadeMode()
{
    uint32_t dwCrusadeGUID;
    SYSTEMTIME SysTime;


    GetLocalTime(&SysTime);
    if (m_iLatestCrusadeDayOfWeek != -1)
    {
        if (m_iLatestCrusadeDayOfWeek == SysTime.wDayOfWeek) return;
    }
    else m_iLatestCrusadeDayOfWeek = SysTime.wDayOfWeek;

    dwCrusadeGUID = timeGetTime();

    LocalStartCrusadeMode(dwCrusadeGUID);
}

void CGame::GrandMagicResultHandler(char * cMapName, int iCrashedStructureNum, int iStructureDamageAmount, int iCasualities, int iActiveStructure, int iSTcount, char * pData)
{
    int i;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != 0)
        {
            SendNotifyMsg(NULL, i, DEF_NOTIFY_GRANDMAGICRESULT, iCrashedStructureNum, iStructureDamageAmount, iCasualities, cMapName, iActiveStructure, 0, 0, 0, 0, iSTcount, pData);
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
            m_stMiddleCrusadeStructureInfo[i].cType = 0;
            m_stMiddleCrusadeStructureInfo[i].cSide = 0;
            m_stMiddleCrusadeStructureInfo[i].sX = 0;
            m_stMiddleCrusadeStructureInfo[i].sY = 0;
        }

        m_iTotalMiddleCrusadeStructures = m_pMapList[m_iMiddlelandMapIndex]->m_iTotalCrusadeStructures;
        memset(G_cData50000, 0, sizeof(G_cData50000));
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
            //testcode
            //wsprintf(G_cTxt, "m_iTotalMiddleCrusadeStructures: %d", m_iTotalMiddleCrusadeStructures);
            //log->info(G_cTxt);
        }
    }
}

void CGame::CrusadeWarStarter()
{
    SYSTEMTIME SysTime;
    int i;

    if (m_bIsCrusadeMode == true) return;
    if (m_bIsCrusadeWarStarter == false) return;


    GetLocalTime(&SysTime);

    for (i = 0; i < DEF_MAXSCHEDULE; i++)
        if (m_stCrusadeWarSchedule[i].iDay == SysTime.wDayOfWeek)
        {
            if ((m_stCrusadeWarSchedule[i].iHour == SysTime.wHour) &&
                (m_stCrusadeWarSchedule[i].iMinute == SysTime.wMinute))
            {

                GlobalStartCrusadeMode();
                return;
            }
        }
}

void CGame::KillCrusadeObjects()
{
    int i;


    for (i = 1; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0)
        {
            switch (m_pNpcList[i]->m_sType)
            {
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 51:
                    NpcKilledHandler(NULL, 0, i, 0);
                    break;
            }
        }
}

void CGame::ManualEndCrusadeMode(int iWinnerSide)
{
    if (m_bIsCrusadeMode == false) return;

    LocalEndCrusadeMode(iWinnerSide);
}

bool CGame::bReadCrusadeScheduleConfigFile(char * pFn)
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

        log->info("(!) Cannot open Crusade Schedule file.");
        return false;
    }
    else
    {
        log->info("(!) Reading Crusade Schedule file...");
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
                            log->info("(!) This server will schedule crusade total war.");
                            m_bIsCrusadeWarStarter = true;
                        }
                        cReadModeA = 0;
                        break;

                    case 2:
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
                }
            }
            else
            {
                if (memcmp(token, "game-server-name", 16) == 0)
                {
                    cReadModeA = 1;
                    cReadModeB = 1;
                }
                if (memcmp(token, "schedule", 8) == 0)
                {
                    cReadModeA = 2;
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

void CGame::_CreateCrusadeGUID(uint32_t dwCrusadeGUID, int iWinnerSide)
{
    char * cp, cTxt[256], cFn[256], cTemp[1024];
    FILE * pFile;

    _mkdir("GameData");
    memset(cFn, 0, sizeof(cFn));

    strcat(cFn, "GameData");
    strcat(cFn, "\\");
    strcat(cFn, "\\");
    strcat(cFn, "CrusadeGUID.Txt");

    pFile = fopen(cFn, "wt");
    if (pFile == 0)
    {

        wsprintf(cTxt, "(!) Cannot create CrusadeGUID(%d) file", dwCrusadeGUID);
        log->info(cTxt);
    }
    else
    {
        memset(cTemp, 0, sizeof(cTemp));

        memset(cTxt, 0, sizeof(cTxt));
        wsprintf(cTxt, "CrusadeGUID = %d\n", dwCrusadeGUID);
        strcat(cTemp, cTxt);

        memset(cTxt, 0, sizeof(cTxt));
        wsprintf(cTxt, "winner-side = %d\n", iWinnerSide);
        strcat(cTemp, cTxt);

        cp = (char *)cTemp;
        fwrite(cp, strlen(cp), 1, pFile);

        wsprintf(cTxt, "(O) CrusadeGUID(%d) file created", dwCrusadeGUID);
        log->info(cTxt);
    }
    if (pFile != 0) fclose(pFile);
}

void CGame::CheckCommanderConstructionPoint(int iClientH)
{
    char * cp, cData[120];
    int * ip, i;

    if (m_pClientList[iClientH] == 0) return;
    if (m_bIsCrusadeMode == false) return;
    if (m_pClientList[iClientH]->m_iConstructionPoint <= 0) return;

    switch (m_pClientList[iClientH]->m_iCrusadeDuty)
    {
        case 1:
        case 2:
            for (i = 0; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iCrusadeDuty == 3) &&
                    (m_pClientList[i]->m_iGuildGUID == m_pClientList[iClientH]->m_iGuildGUID))
                {

                    m_pClientList[i]->m_iConstructionPoint += m_pClientList[iClientH]->m_iConstructionPoint;

                    m_pClientList[i]->m_iWarContribution += (m_pClientList[iClientH]->m_iConstructionPoint / 6);

                    if (m_pClientList[i]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                        m_pClientList[i]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                    if (m_pClientList[i]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                        m_pClientList[i]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, 0, 0);
                    m_pClientList[iClientH]->m_iConstructionPoint = 0;
                    return;
                }

            m_pClientList[iClientH]->m_iConstructionPoint = 0;
            break;

        case 3:

            break;
    }
}

bool CGame::__bSetConstructionKit(int iMapIndex, int dX, int dY, int iType, int iTimeCost, int iClientH)
{
    int iNamingValue, ix, iy, tX, tY;
    char cNpcName[21], cName[21], cNpcWaypoint[11], cOwnerType;
    short sOwnerH;


    if ((m_bIsCrusadeMode == false) || (m_pClientList[iClientH]->m_iCrusadeDuty != 2)) return false;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalCrusadeStructures >= DEF_MAXCRUSADESTRUCTURES)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOMORECRUSADESTRUCTURE, 0, 0, 0, 0);
        return false;
    }


    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue == -1)
    {

    }
    else
    {


        for (ix = dX - 3; ix <= dX + 5; ix++)
            for (iy = dY - 3; iy <= dY + 5; iy++)
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_NPC) && (m_pNpcList[sOwnerH]->m_cActionLimit == 5)) return false;
            }


        memset(cNpcName, 0, sizeof(cNpcName));
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
        else return false;

        memset(cName, 0, sizeof(cName));
        wsprintf(cName, "XX%d", iNamingValue);
        cName[0] = '_';
        cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        memset(cNpcWaypoint, 0, sizeof(cNpcWaypoint));

        tX = (int)dX;
        tY = (int)dY;
        if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, (rand() % 9),
            DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, 0, 0, -1, false, false) == false)
        {

            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
        else
        {

            wsprintf(G_cTxt, "Structure(%s) construction begin(%d,%d)!", cNpcName, tX, tY);
            log->info(G_cTxt);
            return true;
        }
    }

    return false;
}


void CGame::CheckCrusadeResultCalculation(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;


    if ((m_bIsCrusadeMode == false) && (m_pClientList[iClientH]->m_dwCrusadeGUID != 0))
    {

        if (m_pClientList[iClientH]->m_iWarContribution > 300000) m_pClientList[iClientH]->m_iWarContribution = 300000;

        if (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID)
        {
            if (m_iCrusadeWinnerSide == 0)
            {

                m_pClientList[iClientH]->m_iExpStock += (m_pClientList[iClientH]->m_iWarContribution / 6);
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, m_pClientList[iClientH]->m_iWarContribution, 0);

                _bCrusadeLog(DEF_CRUSADELOG_GETEXP, iClientH, (m_pClientList[iClientH]->m_iWarContribution / 6), NULL);
            }
            else
            {
                if (m_iCrusadeWinnerSide == m_pClientList[iClientH]->m_cSide)
                {




                    if (m_pClientList[iClientH]->m_iLevel <= 80)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 200;
                    }
                    else if (m_pClientList[iClientH]->m_iLevel <= 100)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 100;
                    }
                    else m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 30;


                    m_pClientList[iClientH]->m_iExpStock += (m_pClientList[iClientH]->m_iWarContribution) * (1.2);

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, m_pClientList[iClientH]->m_iWarContribution, 0);
                    _bCrusadeLog(DEF_CRUSADELOG_GETEXP, iClientH, m_pClientList[iClientH]->m_iWarContribution, NULL);
                }
                else if (m_iCrusadeWinnerSide != m_pClientList[iClientH]->m_cSide)
                {




                    if (m_pClientList[iClientH]->m_iLevel <= 80)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 200;
                    }
                    else if (m_pClientList[iClientH]->m_iLevel <= 100)
                    {
                        m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 100;
                    }
                    else m_pClientList[iClientH]->m_iWarContribution += m_pClientList[iClientH]->m_iLevel * 30;

                    m_pClientList[iClientH]->m_iExpStock += m_pClientList[iClientH]->m_iWarContribution / 5;

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, -1 * m_pClientList[iClientH]->m_iWarContribution, 0);

                    _bCrusadeLog(DEF_CRUSADELOG_GETEXP, iClientH, (m_pClientList[iClientH]->m_iWarContribution / 10), NULL);
                }
            }
        }
        else
        {

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, 0, 0, -1);
        }

        m_pClientList[iClientH]->m_iCrusadeDuty = 0;
        m_pClientList[iClientH]->m_iWarContribution = 0;
        m_pClientList[iClientH]->m_dwCrusadeGUID = 0;

        //m_pClientList[iClientH]->m_dwSpeedHackCheckTime = timeGetTime();
        //m_pClientList[iClientH]->m_iSpeedHackCheckExp = m_pClientList[iClientH]->m_iExp;
    }
}

void CGame::LocalStartCrusadeMode(uint32_t dwCrusadeGUID)
{
    int i;

    if (m_bIsCrusadeMode == true) return;
    m_bIsCrusadeMode = true;
    m_iCrusadeWinnerSide = 0;

    if (dwCrusadeGUID != 0)
    {

        _CreateCrusadeGUID(dwCrusadeGUID, 0);
        m_dwCrusadeGUID = dwCrusadeGUID;
    }

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
        {

            m_pClientList[i]->m_iCrusadeDuty = 0;
            m_pClientList[i]->m_iConstructionPoint = 0;
            m_pClientList[i]->m_dwCrusadeGUID = m_dwCrusadeGUID;
            SendNotifyMsg(NULL, i, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[i]->m_iCrusadeDuty, 0, 0);
        }


    for (i = 0; i < DEF_MAXMAPS; i++)
        if (m_pMapList[i] != 0) m_pMapList[i]->RestoreStrikePoints();


    CreateCrusadeStructures();

    log->info("(!)Crusade Mode ON.");


    _bCrusadeLog(DEF_CRUSADELOG_STARTCRUSADE, NULL, NULL, NULL);
}

void CGame::LocalEndCrusadeMode(int iWinnerSide)
{
    int i;

    //testcode
    wsprintf(G_cTxt, "LocalEndCrusadeMode(%d)", iWinnerSide);
    log->info(G_cTxt);

    if (m_bIsCrusadeMode == false) return;
    m_bIsCrusadeMode = false;

    log->info("(!)Crusade Mode OFF.");


    RemoveCrusadeStructures();

    KillCrusadeObjects();


    _CreateCrusadeGUID(m_dwCrusadeGUID, iWinnerSide);
    m_iWinnerSide = m_iCrusadeWinnerSide = iWinnerSide;  //v2.19 2002-11-15

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
        {

            m_pClientList[i]->m_iCrusadeDuty = 0;
            m_pClientList[i]->m_iConstructionPoint = 0;
            SendNotifyMsg(NULL, i, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, 0, 0, m_iCrusadeWinnerSide);
        }


    RecallHunterPlayer();


    if (iWinnerSide == 2)
        _bCrusadeLog(DEF_CRUSADELOG_ENDCRUSADE, NULL, NULL, "Elvine Win!");
    else if (iWinnerSide == 1)
        _bCrusadeLog(DEF_CRUSADELOG_ENDCRUSADE, NULL, NULL, "Aresden Win!");
    else
        _bCrusadeLog(DEF_CRUSADELOG_ENDCRUSADE, NULL, NULL, "Drawn!");
}

void CGame::CreateCrusadeStructures()
{
    int i, z, tX, tY, iNamingValue, iTotalMS, iMSIndex[100]{}, iMSSelected[DEF_MAXCRUSADESTRUCTURES]{}, iMSLeft;
    char cName[6], cNpcName[21], cNpcWayPoint[11];
    bool bFlag;

    memset(cName, 0, sizeof(cName));
    memset(cNpcName, 0, sizeof(cNpcName));
    memset(cNpcWayPoint, 0, sizeof(cNpcWayPoint));


    for (i = 0; i < 100; i++) iMSIndex[i] = -1;

    iTotalMS = 0;
    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
        if (m_stCrusadeStructures[i].cType == 42)
        {
            iMSIndex[iTotalMS] = i;
            iTotalMS++;
            iMSSelected[i] = -1;
        }

    if (iTotalMS <= 3)
    {

        iMSSelected[iMSIndex[0]] = 1;
        iMSSelected[iMSIndex[1]] = 1;
        iMSSelected[iMSIndex[2]] = 1;
    }
    else
    {

        iMSLeft = 3;
        bFlag = false;
        while (bFlag == false)
        {
            i = iDice(1, iTotalMS) - 1;
            if (iMSIndex[i] != -1)
            {
                iMSSelected[iMSIndex[i]] = 1;
                iMSIndex[i] = -1;
                iMSLeft--;
                if (iMSLeft <= 0) bFlag = true;
            }
        }
    }


    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
        if (m_stCrusadeStructures[i].cType != 0)
        {
            for (z = 0; z < DEF_MAXMAPS; z++)
                if ((m_pMapList[z] != 0) && (strcmp(m_pMapList[z]->m_cName, m_stCrusadeStructures[i].cMapName) == 0))
                {

                    iNamingValue = m_pMapList[z]->iGetEmptyNamingValue();
                    if (iNamingValue == -1)
                    {


                    }
                    else
                    {
                        memset(cName, 0, sizeof(cName));
                        memset(cNpcName, 0, sizeof(cNpcName));


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
                                if (iMSSelected[i] == 1) strcpy(cNpcName, "ManaStone");
                                break;

                            default:
                                strcpy(cNpcName, m_pNpcConfigList[m_stCrusadeStructures[i].cType]->m_cNpcName);
                                break;
                        }

                        tX = (int)m_stCrusadeStructures[i].dX;
                        tY = (int)m_stCrusadeStructures[i].dY;
                        if (bCreateNewNpc(cNpcName, cName, m_pMapList[z]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM,
                            &tX, &tY, cNpcWayPoint, 0, 0, -1, false) == false)
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


void CGame::SendCollectedMana()
{
    if ((m_iCollectedMana[1] == 0) && m_iCollectedMana[2] == 0) return;

    m_iCollectedMana[0] = 0;
    m_iCollectedMana[1] = 0;
    m_iCollectedMana[2] = 0;
}


void CGame::CollectedManaHandler(uint16_t wAresdenMana, uint16_t wElvineMana)
{
    //wsprintf(G_cTxt, "Collected Mana Handler %d %d      %d %d",  wAresdenMana, wElvineMana, m_iAresdenMapIndex, m_iElvineMapIndex);
    //log->info(G_cTxt);

    if (m_iAresdenMapIndex != -1)
    {
        m_iAresdenMana += wAresdenMana;
        //testcode
        if (wAresdenMana > 0)
        {
            wsprintf(G_cTxt, "Aresden Mana: %d Total:%d", wAresdenMana, m_iAresdenMana);
            log->info(G_cTxt);
        }
    }

    if (m_iElvineMapIndex != -1)
    {
        m_iElvineMana += wElvineMana;
        //testcode
        if (wAresdenMana > 0)
        {
            wsprintf(G_cTxt, "Elvine Mana: %d Total:%d", wElvineMana, m_iElvineMana);
            log->info(G_cTxt);
        }
    }
}



void CGame::MeteorStrikeMsgHandler(char cAttackerSide)
{
    int i;
    uint32_t dwTime = timeGetTime();

    switch (cAttackerSide)
    {
        case 1:

            if (m_iElvineMapIndex != -1)
            {

                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
                    {

                        if (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "elvine") == 0)
                        {

                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 1, 0, 0, 0);
                        }
                        else
                        {

                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 2, 0, 0, 0);
                        }
                    }

                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_METEORSTRIKE, 0, dwTime + 1000 * 5, 0, 0, m_iElvineMapIndex, 0, 0, 0, 0, 0);
            }
            else
            {
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if (m_pClientList[i] != 0)
                    {

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 2, 0, 0, 0);
                    }
            }
            break;

        case 2:

            if (m_iAresdenMapIndex != -1)
            {

                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))
                    {

                        if (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "aresden") == 0)
                        {

                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 3, 0, 0, 0);
                        }
                        else
                        {

                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 4, 0, 0, 0);
                        }
                    }

                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_METEORSTRIKE, 0, dwTime + 1000 * 5, 0, 0, m_iAresdenMapIndex, 0, 0, 0, 0, 0);
            }
            else
            {
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if (m_pClientList[i] != 0)
                    {

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 4, 0, 0, 0);
                    }
            }
            break;
    }
}

void CGame::_LinkStrikePointMapIndex()
{
    int i, z, x;


    for (i = 0; i < DEF_MAXMAPS; i++)
        if ((m_pMapList[i] != 0) && (m_pMapList[i]->m_iTotalStrikePoints != 0))
        {

            for (z = 0; z < DEF_MAXSTRIKEPOINTS; z++)
                if (strlen(m_pMapList[i]->m_stStrikePoint[z].cRelatedMapName) != 0)
                {
                    for (x = 0; x < DEF_MAXMAPS; x++)
                        if ((m_pMapList[x] != 0) && (strcmp(m_pMapList[x]->m_cName, m_pMapList[i]->m_stStrikePoint[z].cRelatedMapName) == 0))
                        {

                            m_pMapList[i]->m_stStrikePoint[z].iMapIndex = x;
                            //testcode
                            wsprintf(G_cTxt, "(!) Map(%s) Strike Point(%d) Related Map(%s) Index(%d)", m_pMapList[i]->m_cName, z, m_pMapList[i]->m_stStrikePoint[z].cRelatedMapName, x);
                            log->info(G_cTxt);

                            goto LSPMI_LOOPBREAK;
                        }
                    LSPMI_LOOPBREAK:;
                }
        }
}


void CGame::MeteorStrikeHandler(int iMapIndex)
{
    int i, ix, iy, dX, dY, iIndex, iTargetIndex, iTotalESG, iEffect;
    int iTargetArray[DEF_MAXSTRIKEPOINTS]{};
    short sOwnerH;
    char  cOwnerType;
    uint32_t dwTime = timeGetTime();

    log->info("(!) Beginning Meteor Strike Procedure...");

    if (iMapIndex == -1)
    {
        log->info("(X) MeteorStrikeHandler Error! MapIndex -1!");
        return;
    }

    if (m_pMapList[iMapIndex] == 0)
    {
        log->info("(X) MeteorStrikeHandler Error! 0 Map!");
        return;
    }

    if (m_pMapList[iMapIndex]->m_iTotalStrikePoints == 0)
    {
        log->info("(X) MeteorStrikeHandler Error! No Strike Points!");
        return;
    }

    for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++) iTargetArray[i] = -1;

    iIndex = 0;
    for (i = 1; i <= m_pMapList[iMapIndex]->m_iTotalStrikePoints; i++)
    {
        if (m_pMapList[iMapIndex]->m_stStrikePoint[i].iHP > 0)
        {
            iTargetArray[iIndex] = i;
            iIndex++;
        }
    }

    //testcode
    wsprintf(G_cTxt, "(!) Map(%s) has %d available strike points", m_pMapList[iMapIndex]->m_cName, iIndex);
    log->info(G_cTxt);


    m_stMeteorStrikeResult.iCasualties = 0;
    m_stMeteorStrikeResult.iCrashedStructureNum = 0;
    m_stMeteorStrikeResult.iStructureDamageAmount = 0;

    if (iIndex == 0)
    {

        log->info("(!) No strike points!");
        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT, 0, dwTime + 6000, 0, 0, iMapIndex, 0, 0, 0, 0, 0);
    }
    else
    {


        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) && (m_pClientList[i]->m_cMapIndex == iMapIndex))
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKEHIT, 0, 0, 0, 0);
            }

        for (i = 0; i < iIndex; i++)
        {

            iTargetIndex = iTargetArray[i];

            if (iTargetIndex == -1)
            {
                log->info("(X) Strike Point MapIndex: -1!");
                goto MSH_SKIP_STRIKE;
            }

            dX = m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].dX;
            dY = m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].dY;

            iTotalESG = 0;
            for (ix = dX - 10; ix <= dX + 10; ix++)
                for (iy = dY - 10; iy <= dY + 10; iy++)
                {
                    m_pMapList[iMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                    if ((cOwnerType == DEF_OWNERTYPE_NPC) && (m_pNpcList[sOwnerH] != 0) && (m_pNpcList[sOwnerH]->m_sType == 40))
                    {
                        iTotalESG++;
                    }
                }

            wsprintf(G_cTxt, "(!) Meteor Strike Target(%d, %d) ESG(%d)", dX, dY, iTotalESG);
            log->info(G_cTxt);

            if (iTotalESG < 2)
            {
                m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iHP -= (2 - iTotalESG);
                if (m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iHP <= 0)
                {
                    m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iHP = 0;

                    m_pMapList[m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iMapIndex]->m_bIsDisabled = true;
                    m_stMeteorStrikeResult.iCrashedStructureNum++;
                }
                else
                {
                    m_stMeteorStrikeResult.iStructureDamageAmount += (2 - iTotalESG);

                    iEffect = iDice(1, 5) - 1;
                    iAddDynamicObjectList(NULL, DEF_OWNERTYPE_PLAYER_INDIRECT, DEF_DYNAMICOBJECT_FIRE2, iMapIndex,
                        m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iEffectX[iEffect] + (iDice(1, 3) - 2),
                        m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iEffectY[iEffect] + (iDice(1, 3) - 2), 60 * 1000 * 50);
                }
            }
            MSH_SKIP_STRIKE:;
        }

        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE, 0, dwTime + 1000, 0, 0, iMapIndex, 0, 0, 0, 0, 0);

        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE, 0, dwTime + 4000, 0, 0, iMapIndex, 0, 0, 0, 0, 0);

        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT, 0, dwTime + 6000, 0, 0, iMapIndex, 0, 0, 0, 0, 0);
    }
}

void CGame::MapStatusHandler(int iClientH, int iMode, char * pMapName)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;

    switch (iMode)
    {
        case 1:
            if (m_pClientList[iClientH]->m_iCrusadeDuty == 0) return;

            for (i = 0; i < DEF_MAXGUILDS; i++)
                if ((m_pGuildTeleportLoc[i].m_iV1 != 0) && (m_pGuildTeleportLoc[i].m_iV1 == m_pClientList[iClientH]->m_iGuildGUID))
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_TCLOC, m_pGuildTeleportLoc[i].m_sDestX, m_pGuildTeleportLoc[i].m_sDestY,
                        NULL, m_pGuildTeleportLoc[i].m_cDestMapName, m_pGuildTeleportLoc[i].m_sDestX2, m_pGuildTeleportLoc[i].m_sDestY2,
                        NULL, 0, 0, 0, m_pGuildTeleportLoc[i].m_cDestMapName2);

                    memset(m_pClientList[iClientH]->m_cConstructMapName, 0, sizeof(m_pClientList[iClientH]->m_cConstructMapName));
                    memcpy(m_pClientList[iClientH]->m_cConstructMapName, m_pGuildTeleportLoc[i].m_cDestMapName2, 10);
                    m_pClientList[iClientH]->m_iConstructLocX = m_pGuildTeleportLoc[i].m_sDestX2;
                    m_pClientList[iClientH]->m_iConstructLocY = m_pGuildTeleportLoc[i].m_sDestY2;
                    return;
                }


            break;

        case 3:
            //if (m_pClientList[iClientH]->m_iCrusadeDuty != 3) return;

            for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
            {
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = 0;
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = 0;
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = 0;
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = 0;
            }
            m_pClientList[iClientH]->m_iCSIsendPoint = 0;
            memset(m_pClientList[iClientH]->m_cSendingMapName, 0, sizeof(m_pClientList[iClientH]->m_cSendingMapName));

            if (strcmp(pMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName) == 0)
            {


                for (i = 0; i < m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalCrusadeStructures; i++)
                {

                    if (m_pClientList[iClientH]->m_iCrusadeDuty == 3)
                    {
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cType;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cSide;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sX;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sY;
                    }
                    else if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cType == 42)
                    {
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cType;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cSide;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sX;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sY;
                    }
                }
                memcpy(m_pClientList[iClientH]->m_cSendingMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 10);
            }
            else
            {

                if (strcmp(pMapName, "middleland") == 0)
                {
                    for (i = 0; i < m_iTotalMiddleCrusadeStructures; i++)
                    {

                        if (m_pClientList[iClientH]->m_iCrusadeDuty == 3)
                        {
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_stMiddleCrusadeStructureInfo[i].cType;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_stMiddleCrusadeStructureInfo[i].cSide;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_stMiddleCrusadeStructureInfo[i].sX;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_stMiddleCrusadeStructureInfo[i].sY;
                        }
                        else if (m_stMiddleCrusadeStructureInfo[i].cType == 42)
                        {
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_stMiddleCrusadeStructureInfo[i].cType;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_stMiddleCrusadeStructureInfo[i].cSide;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_stMiddleCrusadeStructureInfo[i].sX;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_stMiddleCrusadeStructureInfo[i].sY;
                        }
                    }
                    strcpy(m_pClientList[iClientH]->m_cSendingMapName, "middleland");
                }
                else
                {

                }
            }

            _SendMapStatus(iClientH);
            break;
    }
}


void CGame::_SendMapStatus(int iClientH)
{
    int i, iDataSize;
    char * cp, cData[DEF_MAXCRUSADESTRUCTURES * 6];
    short * sp;

    memset(cData, 0, sizeof(cData));
    cp = (char *)(cData);

    memcpy(cp, m_pClientList[iClientH]->m_cSendingMapName, 10);
    cp += 10;

    sp = (short *)cp;
    *sp = (short)m_pClientList[iClientH]->m_iCSIsendPoint;
    cp += 2;


    cp++;

    if (m_pClientList[iClientH]->m_iCSIsendPoint == 0)
        m_pClientList[iClientH]->m_bIsSendingMapStatus = true;


    iDataSize = 0;
    for (i = 0; i < 100; i++)
    {
        if (m_pClientList[iClientH]->m_iCSIsendPoint >= DEF_MAXCRUSADESTRUCTURES) goto SMS_ENDOFDATA;
        if (m_pClientList[iClientH]->m_stCrusadeStructureInfo[m_pClientList[iClientH]->m_iCSIsendPoint].cType == 0) goto SMS_ENDOFDATA;

        if ((m_pClientList[iClientH]->m_iCSIsendPoint >= 3) && (m_pClientList[iClientH]->m_iCrusadeDuty != 3))  goto SMS_ENDOFDATA;

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
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAPSTATUSNEXT, iDataSize + 13, 0, 0, cData);
    return;

    SMS_ENDOFDATA:;


    cp = (char *)(cData + 12);
    *cp = (iDataSize / 6);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAPSTATUSLAST, iDataSize + 13, 0, 0, cData);
    m_pClientList[iClientH]->m_bIsSendingMapStatus = false;

    return;
}


void CGame::RemoveCrusadeStructures()
{
    int i;

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0)
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

void CGame::RecallHunterPlayer()
{
    int i;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsWarLocation == true)
            && (m_pClientList[i]->m_bIsHunter == true) && (m_pClientList[i]->m_bIsInitComplete == true))
        {
            m_pClientList[i]->m_iTimeLeft_ForceRecall = 0;
        }
    }


}

void CGame::DoMeteorStrikeDamageHandler(int iMapIndex)
{
    int i, iDamage;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_cSide != 0) && (m_pClientList[i]->m_cMapIndex == iMapIndex))
        {
            if (m_pClientList[i]->m_iLevel < 80)
                iDamage = m_pClientList[i]->m_iLevel + iDice(1, 10);
            else iDamage = m_pClientList[i]->m_iLevel * 2 + iDice(1, 10);
            iDamage = iDice(1, m_pClientList[i]->m_iLevel) + m_pClientList[i]->m_iLevel;

            if (iDamage > 255) iDamage = 255;


            if (m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2) iDamage = iDamage / 2;


            if (m_pClientList[i]->m_iAdminUserLevel > 0) iDamage = 0;

            m_pClientList[i]->m_iHP -= iDamage;
            if (m_pClientList[i]->m_iHP <= 0)
            {

                ClientKilledHandler(i, 0, 0, iDamage);
                m_stMeteorStrikeResult.iCasualties++;
            }
            else
            {
                if (iDamage > 0)
                {

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_HP, 0, 0, 0, 0);

                    SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);


                    if (m_pClientList[i]->m_bSkillUsingStatus[19] != true)
                    {
                        m_pMapList[m_pClientList[i]->m_cMapIndex]->ClearOwner(0, i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                        m_pMapList[m_pClientList[i]->m_cMapIndex]->SetOwner(i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                    }

                    if (m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {

                        // 1: Hold-Person 
                        // 2: Paralyze
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], 0, 0);

                        m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                        bRemoveFromDelayEventList(i, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }
            }
        }
}

void CGame::CalcMeteorStrikeEffectHandler(int iMapIndex)
{
    int i, iActiveStructure, iStructureHP[DEF_MAXSTRIKEPOINTS]{};
    char cWinnerSide, cTempData[120];

    if (m_bIsCrusadeMode == false) return;


    for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++)
        iStructureHP[i] = 0;


    iActiveStructure = 0;
    for (i = 1; i <= m_pMapList[iMapIndex]->m_iTotalStrikePoints; i++)
    {
        if (m_pMapList[iMapIndex]->m_stStrikePoint[i].iHP > 0)
        {
            iActiveStructure++;
            iStructureHP[i] = m_pMapList[iMapIndex]->m_stStrikePoint[i].iHP;
        }
    }

    //testcode
    wsprintf(G_cTxt, "ActiveStructure:%d  MapIndex:%d AresdenMap:%d ElvineMap:%d", iActiveStructure, iMapIndex, m_iAresdenMapIndex, m_iElvineMapIndex);
    log->info(G_cTxt);

    if (iActiveStructure == 0)
    {
        if (iMapIndex == m_iAresdenMapIndex)
        {
            cWinnerSide = 2;
            LocalEndCrusadeMode(2);
        }
        else if (iMapIndex == m_iElvineMapIndex)
        {
            cWinnerSide = 1;
            LocalEndCrusadeMode(1);
        }
        else
        {
            cWinnerSide = 0;
            LocalEndCrusadeMode(0);
        }
    }
    else
    {
        GrandMagicResultHandler(m_pMapList[iMapIndex]->m_cName, m_stMeteorStrikeResult.iCrashedStructureNum, m_stMeteorStrikeResult.iStructureDamageAmount, m_stMeteorStrikeResult.iCasualties, iActiveStructure, m_pMapList[iMapIndex]->m_iTotalStrikePoints, cTempData);
    }

    m_stMeteorStrikeResult.iCasualties = 0;
    m_stMeteorStrikeResult.iCrashedStructureNum = 0;
    m_stMeteorStrikeResult.iStructureDamageAmount = 0;
}
