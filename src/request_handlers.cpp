//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

extern char G_cTxt[512];

void CGame::RequestInitDataHandler(int iClientH, char * pData, char cKey)
{
    char * pBuffer = NULL;
    short * sp;
    DWORD * dwp;
    WORD * wp;
    char * cp;
    int sSummonPoints;
    int * ip, i, iTotalItemA, iTotalItemB, iSize, iRet, iStats;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == NULL) return;

    pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
    ZeroMemory(pBuffer, DEF_MSGBUFFERSIZE + 1);

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    dwp = (DWORD *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_PLAYERCHARACTERCONTENTS;
    wp = (WORD *)(pBuffer + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iHP;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iMP;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iSP;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iDefenseRatio;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iHitRatio;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iLevel;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iStr;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iInt;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iVit;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iDex;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iMag;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iCharisma;
    cp += 4;

    iStats = (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iDex + m_pClientList[iClientH]->m_iVit +
        m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma);

    m_pClientList[iClientH]->m_iLU_Pool = (m_pClientList[iClientH]->m_iLevel - 1) * 3 - (iStats - 70);
    wp = (WORD *)cp;
    *wp = m_pClientList[iClientH]->m_iLU_Pool;
    cp += 2;

    *cp = m_pClientList[iClientH]->m_cVar;
    cp++;

    *cp = 0;
    cp++;

    *cp = 0;
    cp++;

    *cp = 0;
    cp++;

    *cp = 0;
    cp++;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iExp;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iEnemyKillCount;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iPKCount;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iRewardGold;
    cp += 4;

    memcpy(cp, m_pClientList[iClientH]->m_cLocation, 10);
    cp += 10;

    memcpy(cp, m_pClientList[iClientH]->m_cGuildName, 20);
    cp += 20;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iGuildRank;
    cp += 4;

    *cp = (char)m_pClientList[iClientH]->m_iSuperAttackLeft;
    cp++;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iFightzoneNumber;
    cp += 4;

    m_pClientList[iClientH]->isForceSet = FALSE;
    m_pClientList[iClientH]->m_iPartyID = NULL;
    m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
    m_pClientList[iClientH]->m_iReqJoinPartyClientH = NULL;

    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 118);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            if (pBuffer != NULL) delete pBuffer;
            DeleteClient(iClientH, TRUE, TRUE);
            return;
    }

    dwp = (DWORD *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_PLAYERITEMLISTCONTENTS;
    wp = (WORD *)(pBuffer + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    iTotalItemA = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
            iTotalItemA++;

    cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

    *cp = iTotalItemA;
    cp++;

    for (i = 0; i < iTotalItemA; i++)
    {
        if (m_pClientList[iClientH]->m_pItemList[i] == NULL)
        {
            wsprintf(G_cTxt, "RequestInitDataHandler error: Client(%s) Item(%d)", m_pClientList[iClientH]->m_cCharName, i);
            log->info(G_cTxt);

            DeleteClient(iClientH, FALSE, TRUE);
            if (pBuffer != NULL) delete pBuffer;
            return;
        }
        memcpy(cp, m_pClientList[iClientH]->m_pItemList[i]->m_cName, 20);
        cp += 20;
        dwp = (DWORD *)cp;
        *dwp = m_pClientList[iClientH]->m_pItemList[i]->m_dwCount;
        cp += 4;
        *cp = m_pClientList[iClientH]->m_pItemList[i]->m_cItemType;
        cp++;
        *cp = m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos;
        cp++;
        *cp = (char)m_pClientList[iClientH]->m_bIsItemEquipped[i];
        cp++;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemList[i]->m_sLevelLimit;
        cp += 2;
        *cp = m_pClientList[iClientH]->m_pItemList[i]->m_cGenderLimit;
        cp++;
        wp = (WORD *)cp;
        *wp = m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan;
        cp += 2;
        wp = (WORD *)cp;
        *wp = m_pClientList[iClientH]->m_pItemList[i]->m_wWeight;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemList[i]->m_sSprite;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemList[i]->m_sSpriteFrame;
        cp += 2;
        *cp = m_pClientList[iClientH]->m_pItemList[i]->m_cItemColor;
        cp++;
        *cp = (char)m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue2;
        cp++;
        dwp = (DWORD *)cp;
        *dwp = m_pClientList[iClientH]->m_pItemList[i]->m_dwAttribute;
        cp += 4;
    }

    iTotalItemB = 0;
    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] != NULL)
            iTotalItemB++;

    *cp = iTotalItemB;
    cp++;

    for (i = 0; i < iTotalItemB; i++)
    {
        if (m_pClientList[iClientH]->m_pItemInBankList[i] == NULL)
        {
            wsprintf(G_cTxt, "RequestInitDataHandler error: Client(%s) Bank-Item(%d)", m_pClientList[iClientH]->m_cCharName, i);
            log->info(G_cTxt);

            DeleteClient(iClientH, FALSE, TRUE);
            if (pBuffer != NULL) delete pBuffer;
            return;
        }
        memcpy(cp, m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName, 20);
        cp += 20;
        dwp = (DWORD *)cp;
        *dwp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwCount;
        cp += 4;
        *cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemType;
        cp++;
        *cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cEquipPos;
        cp++;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sLevelLimit;
        cp += 2;
        *cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cGenderLimit;
        cp++;
        wp = (WORD *)cp;
        *wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wCurLifeSpan;
        cp += 2;
        wp = (WORD *)cp;
        *wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wWeight;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sSprite;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sSpriteFrame;
        cp += 2;
        *cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemColor;
        cp++;
        *cp = (char)m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue2;
        cp++;
        dwp = (DWORD *)cp;
        *dwp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwAttribute;
        cp += 4;
    }

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
    {
        *cp = m_pClientList[iClientH]->m_cMagicMastery[i];
        cp++;
    }

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
    {
        *cp = m_pClientList[iClientH]->m_cSkillMastery[i];
        cp++;
    }

    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 6 + 1 + iTotalItemA * 44 + iTotalItemB * 43 + DEF_MAXMAGICTYPE + DEF_MAXSKILLTYPE);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            DeleteClient(iClientH, TRUE, TRUE);
            if (pBuffer != NULL) delete pBuffer;
            return;
    }

    dwp = (DWORD *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_INITDATA;
    wp = (WORD *)(pBuffer + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

    if (m_pClientList[iClientH]->m_bIsObserverMode == FALSE)
        bGetEmptyPosition(&m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cMapIndex);
    else GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY);

    wp = (WORD *)cp;
    *wp = iClientH;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sX;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sY;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sType;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr1;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr2;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr3;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr4;
    cp += 2;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iApprColor;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iStatus;
    cp += 4;

    memcpy(cp, m_pClientList[iClientH]->m_cMapName, 10);
    cp += 10;

    memcpy(cp, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10);
    cp += 10;

    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == TRUE)
        *cp = 1;
    else *cp = m_cDayOrNight;
    cp++;

    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == TRUE)
        *cp = NULL;
    else *cp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus;
    cp++;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iContribution;
    cp += 4;


    if (m_pClientList[iClientH]->m_bIsObserverMode == FALSE)
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH,
            DEF_OWNERTYPE_PLAYER,
            m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY);
    }

    *cp = (char)m_pClientList[iClientH]->m_bIsObserverMode;
    cp++;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iRating;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iHP;
    cp += 4;

    *cp = 0;
    cp++;

    iSize = iComposeInitMapData(m_pClientList[iClientH]->m_sX - 10, m_pClientList[iClientH]->m_sY - 7, iClientH, cp);

    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 46 + iSize + 4 + 4 + 1 + 4 + 4 + 3);

    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            DeleteClient(iClientH, TRUE, TRUE);
            if (pBuffer != NULL) delete pBuffer;
            return;
    }

    if (pBuffer != NULL) delete pBuffer;

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, NULL, NULL, NULL);

    if ((memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
        SetForceRecallTime(iClientH);
    }
    else if ((memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = TRUE;

        SetForceRecallTime(iClientH);
    }
    else if (((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arejail", 7) == 0) ||
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvjail", 7) == 0))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();

        if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 5;
        }
        else if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 20 * 5)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 5;
        }
    }
    else if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == TRUE) &&
        (m_iFightzoneNoForceRecall == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = TRUE;

        GetLocalTime(&SysTime);
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2 * 60 * 20 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 2 * 20;
    }
    else
    {
        m_pClientList[iClientH]->m_bIsWarLocation = FALSE;
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 0;
        SetForceRecallTime(iClientH);
    }

    if ((m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0) && (m_pClientList[iClientH]->m_bIsWarLocation == TRUE) && IsEnemyZone(iClientH))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, NULL, NULL, NULL);
        wsprintf(G_cTxt, "(!) Game Server Force Recall Time  %d (%d)min", m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall / 20);
        log->info(G_cTxt);
    }

    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft < 0)
    {
        m_pClientList[iClientH]->m_iGizonItemUpgradeLeft = 0;
    }

    int iMapside, iMapside2;

    iMapside = iGetMapLocationSide(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);
    if (iMapside > 3) iMapside2 = iMapside - 2;
    else iMapside2 = iMapside;
    m_pClientList[iClientH]->m_bIsInsideOwnTown = FALSE;
    if ((m_pClientList[iClientH]->m_cSide != iMapside2) && (iMapside != 0))
    {
        if ((iMapside <= 2) && (m_pClientList[iClientH]->m_iAdminUserLevel < 1))
        {
            if (m_pClientList[iClientH]->m_cSide != 0)
            {
                m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
                m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
                m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
                m_pClientList[iClientH]->m_bIsInsideOwnTown = TRUE;
            }
        }
    }
    else
    {
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == TRUE &&
            m_iFightzoneNoForceRecall == FALSE &&
            m_pClientList[iClientH]->m_iAdminUserLevel == 0)
        {
            m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
            m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
            GetLocalTime(&SysTime);
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2 * 60 * 20 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 2 * 20;
        }
        else
        {
            if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arejail", 7) == 0 ||
                memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvjail", 7) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
                {
                    m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
                    m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
                    if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0)
                        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 100;
                    else if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 100)
                        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 100;
                }
            }
        }
    }

    /*if ((m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0) &&
        (m_pClientList[iClientH]->m_bIsWarLocation == TRUE)) {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, NULL, NULL, NULL);
    }*/

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SAFEATTACKMODE, NULL, NULL, NULL, NULL);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, NULL, NULL, NULL);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMPOSLIST, NULL, NULL, NULL, NULL);

    _SendQuestContents(iClientH);
    _CheckQuestEnvironment(iClientH);

    if (m_pClientList[iClientH]->m_iSpecialAbilityTime == 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYENABLED, NULL, NULL, NULL, NULL);
    }

    if (m_bIsCrusadeMode == TRUE)
    {
        if (m_pClientList[iClientH]->m_dwCrusadeGUID == 0)
        {
            m_pClientList[iClientH]->m_iCrusadeDuty = 0;
            m_pClientList[iClientH]->m_iConstructionPoint = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = m_dwCrusadeGUID;
        }
        else if (m_pClientList[iClientH]->m_dwCrusadeGUID != m_dwCrusadeGUID)
        {
            m_pClientList[iClientH]->m_iCrusadeDuty = 0;
            m_pClientList[iClientH]->m_iConstructionPoint = 0;
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = m_dwCrusadeGUID;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, 0, NULL, -1);
        }
        m_pClientList[iClientH]->m_cVar = 1;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, NULL, NULL);
    }
    else if (m_bIsHeldenianMode == TRUE)
    {
        sSummonPoints = m_pClientList[iClientH]->m_iCharisma * 300;
        if (sSummonPoints > DEF_MAXSUMMONPOINTS) sSummonPoints = DEF_MAXSUMMONPOINTS;
        if (m_pClientList[iClientH]->m_dwHeldenianGUID == NULL)
        {
            m_pClientList[iClientH]->m_dwHeldenianGUID = m_dwHeldenianGUID;
            m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
        }
        else if (m_pClientList[iClientH]->m_dwHeldenianGUID != m_dwHeldenianGUID)
        {
            m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwHeldenianGUID = m_dwHeldenianGUID;
        }
        m_pClientList[iClientH]->m_cVar = 2;
        if (m_bIsHeldenianMode == TRUE)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, NULL, NULL, NULL, NULL);
            if (m_bHeldenianInitiated == FALSE)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANSTART, NULL, NULL, NULL, NULL);
            }
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, NULL, NULL);
            UpdateHeldenianStatus();
        }
    }
    else if ((m_pClientList[iClientH]->m_cVar == 1) && (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID))
    {
        m_pClientList[iClientH]->m_iCrusadeDuty = 0;
        m_pClientList[iClientH]->m_iConstructionPoint = 0;
    }
    else
    {
        if (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID)
        {
            if (m_pClientList[iClientH]->m_cVar == 1)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, NULL, NULL, -1);
            }
        }
        else
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, 0, NULL, -1);
            m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
        }
    }

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "fightzone", 9) == 0)
    {
        wsprintf(G_cTxt, "Char(%s)-Enter(%s) Observer(%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_bIsObserverMode);
        log->info(G_cTxt);
    }

    if (m_bIsHeldenianMode == TRUE) SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANTELEPORT, NULL, NULL, NULL, NULL, NULL);
    if (m_bHeldenianInitiated == TRUE) SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANSTART, NULL, NULL, NULL, NULL, NULL);

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 1, NULL);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);

    if ((m_bIsApocalypseMode == TRUE) && (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsApocalypseMap == TRUE))
    {
        RequestTeleportHandler(iClientH, "1   ");
    }

    if (m_bIsApocalypseMode == TRUE)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_APOCGATESTARTMSG, NULL, NULL, NULL, NULL, NULL);
    }
}

void CGame::ClientMotionHandler(int iClientH, char * pData)
{
    DWORD * dwp, dwClientTime;
    WORD * wp, wCommand, wTargetObjectID;
    short * sp, sX, sY, dX, dY, wType;
    char * cp, cDir;
    int   iRet, iTemp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;

    /*m_pClientList[iClientH]->m_cConnectionCheck++;
    if (m_pClientList[iClientH]->m_cConnectionCheck > 50) {
        wsprintf(G_cTxt, "Hex: (%s) Player: (%s) - removed 03203203h, vital to hack detection.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
        log->info(G_cTxt);
        DeleteClient(iClientH, TRUE, TRUE);
        return;
    }*/
    wp = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
    wCommand = *wp;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    sp = (short *)cp;
    sX = *sp;
    cp += 2;

    sp = (short *)cp;
    sY = *sp;
    cp += 2;

    cDir = *cp;
    cp++;

    sp = (short *)cp;
    dX = *sp;
    cp += 2;

    sp = (short *)cp;
    dY = *sp;
    cp += 2;

    sp = (short *)cp;
    wType = *sp;
    cp += 2;

    if ((wCommand == DEF_OBJECTATTACK) || (wCommand == DEF_OBJECTATTACKMOVE))
    {
        wp = (WORD *)cp;
        wTargetObjectID = *wp;
        cp += 2;
    }

    dwp = (DWORD *)cp;
    dwClientTime = *dwp;
    cp += 4;

    switch (wCommand)
    {
        case DEF_OBJECTSTOP:
            iRet = iClientMotion_Stop_Handler(iClientH, sX, sY, cDir);
            if (iRet == 1)
            {
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTSTOP, NULL, NULL, NULL);
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            break;

        case DEF_OBJECTRUN:
            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 1);
            if (iRet == 1)
            {
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTRUN, NULL, NULL, NULL);
            }
            bCheckClientMoveFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTMOVE:
            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 2);
            if (iRet == 1)
            {
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, NULL, NULL, NULL);
            }
            if ((m_pClientList[iClientH] != NULL) && (m_pClientList[iClientH]->m_iHP <= 0)) ClientKilledHandler(iClientH, NULL, NULL, 1);
            bCheckClientMoveFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTDAMAGEMOVE:
            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 0);
            if (iRet == 1)
            {
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGEMOVE, m_pClientList[iClientH]->m_iLastDamage, NULL, NULL);
            }
            if ((m_pClientList[iClientH] != NULL) && (m_pClientList[iClientH]->m_iHP <= 0)) ClientKilledHandler(iClientH, NULL, NULL, 1);
            break;

        case DEF_OBJECTATTACKMOVE:
            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 0);
            if ((iRet == 1) && (m_pClientList[iClientH] != NULL))
            {
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTATTACKMOVE, NULL, NULL, NULL);
                iClientMotion_Attack_Handler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, dX, dY, wType, cDir, wTargetObjectID, FALSE, TRUE);
            }
            if ((m_pClientList[iClientH] != NULL) && (m_pClientList[iClientH]->m_iHP <= 0)) ClientKilledHandler(iClientH, NULL, NULL, 1);
            bCheckClientAttackFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTATTACK:
            _CheckAttackType(iClientH, &wType);
            iRet = iClientMotion_Attack_Handler(iClientH, sX, sY, dX, dY, wType, cDir, wTargetObjectID);
            if (iRet == 1)
            {
                if (wType >= 20)
                {
                    m_pClientList[iClientH]->m_iSuperAttackLeft--;
                    if (m_pClientList[iClientH]->m_iSuperAttackLeft < 0) m_pClientList[iClientH]->m_iSuperAttackLeft = 0;
                }

                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, wType);
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            bCheckClientAttackFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTGETITEM:
            iRet = iClientMotion_GetItem_Handler(iClientH, sX, sY, cDir);
            if (iRet == 1)
            {
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTGETITEM, NULL, NULL, NULL);
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            break;

        case DEF_OBJECTMAGIC:
            iRet = iClientMotion_Magic_Handler(iClientH, sX, sY, cDir);
            if (iRet == 1)
            {
                if (m_pClientList[iClientH]->m_bMagicPauseTime == FALSE)
                {
                    m_pClientList[iClientH]->m_bMagicPauseTime = TRUE;
                    iTemp = 10;
                    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTMAGIC, dX, iTemp, NULL);
                    m_pClientList[iClientH]->m_iSpellCount++;
                    bCheckClientMagicFrequency(iClientH, dwClientTime);
                }
                else if (m_pClientList[iClientH]->m_bMagicPauseTime == TRUE)
                {
                    try
                    {
                        wsprintf(G_cTxt, "Cast Delay Hack: (%s) Player: (%s) - player casting too fast.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                        log->info(G_cTxt);
                        DeleteClient(iClientH, TRUE, TRUE);
                    }
                    catch (...)
                    {
                    }
                }
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            break;

        default:
            break;
    }
}

void CGame::ChatMsgHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    int i;
    std::size_t iRet;
    WORD * wp;
    int * ip{};
    char * cp, * cp2{};
    char   cBuffer[256], cTemp[256], cSendMode = NULL;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (dwMsgSize > 83 + 30) return;

    if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) return;

    if (memcmp((pData + 10), m_pClientList[iClientH]->m_cCharName, strlen(m_pClientList[iClientH]->m_cCharName)) != 0) return;

    if ((m_pClientList[iClientH]->m_bIsObserverMode == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) return;

    int iStX, iStY;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != NULL)
    {
        iStX = m_pClientList[iClientH]->m_sX / 20;
        iStY = m_pClientList[iClientH]->m_sY / 20;
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iPlayerActivity++;

        switch (m_pClientList[iClientH]->m_cSide)
        {
            case 0: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iNeutralActivity++; break;
            case 1: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iAresdenActivity++; break;
            case 2: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iElvineActivity++;  break;
        }
    }
    cp = (char *)(pData + 21);

    switch (m_bLogChatOption)
    {
        case 1:
            if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
            {
                ZeroMemory(cTemp, sizeof(cTemp));
                pData[dwMsgSize - 1] = 0;
                wsprintf(cTemp, "Loc(%s) IP(%s) PC(%s):\"%s\"", m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, cp);
                //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
            }
            break;

        case 2:
            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
            {
                ZeroMemory(cTemp, sizeof(cTemp));
                pData[dwMsgSize - 1] = 0;
                wsprintf(cTemp, "Loc(%s) IP(%s) GM(%s):\"%s\"", m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, cp);
                //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
            }
            break;

        case 3:
            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
            {
                ZeroMemory(cTemp, sizeof(cTemp));
                pData[dwMsgSize - 1] = 0;
                wsprintf(cTemp, "Loc(%s) IP(%s) GM(%s):\"%s\"", m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, cp);
                //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
            }
            else
            {
                ZeroMemory(cTemp, sizeof(cTemp));
                pData[dwMsgSize - 1] = 0;
                wsprintf(cTemp, "Loc(%s) IP(%s) PC(%s):\"%s\"", m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, cp);
                //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
            }
            break;

        case 4:

            break;
    }

    switch (*cp)
    {
        case '@':
            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 1) &&
                (m_pClientList[iClientH]->m_iSP >= 3))
            {
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[iClientH]->m_iSP -= 3;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                }
                cSendMode = 1;
            }
            else cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = NULL;
            break;

        case '$':
            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iSP >= 3))
            {
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[iClientH]->m_iSP -= 3;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                }
                cSendMode = 4;
            }
            else
            {
                cSendMode = NULL;
            }

            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0)
            {
                cSendMode = NULL;
            }
            break;

        case '^':
            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 10) &&
                (m_pClientList[iClientH]->m_iSP > 5) && m_pClientList[iClientH]->m_iGuildRank != -1)
            {
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[iClientH]->m_iSP -= 3;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                }
                cSendMode = 1;
            }
            else cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iHP < 0) cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) cSendMode = 10;
            break;

        case '!':
            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 10) &&
                (m_pClientList[iClientH]->m_iSP >= 5))
            {
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[iClientH]->m_iSP -= 5;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                }
                cSendMode = 2;
            }
            else cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) cSendMode = 10;
            break;

        case '~':
            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 1) &&
                (m_pClientList[iClientH]->m_iSP >= 3))
            {
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[iClientH]->m_iSP -= 3;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                }
                cSendMode = 3;
            }
            else cSendMode = NULL;

            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = NULL;
            if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = NULL;
            break;

        case '/':
            ZeroMemory(cBuffer, sizeof(cBuffer));
            memcpy(cBuffer, cp, dwMsgSize - 21);
            cp = (char *)(cBuffer);

            if (memcmp(cp, "/version", 8) == 0)
            {
                ShowVersion(iClientH);
                return;
            }

            if (memcmp(cp, "/begincrusadetotalwar", 21) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 3)
                {
                    GlobalStartCrusadeMode();
                    wsprintf(cTemp, "(%s) GM Order(%s): begincrusadetotalwar", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
                }
                return;
            }

            if (memcmp(cp, "/endcrusadetotalwar", 19) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 3)
                {
                    ManualEndCrusadeMode(0);
                    wsprintf(cTemp, "(%s) GM Order(%s): endcrusadetotalwar", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
                }
                return;
            }

            if (memcmp(cp, "/goto ", 6) == 0)
            {
                AdminOrder_GoTo(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/unsummonboss", 13) == 0)
            {
                AdminOrder_UnsummonBoss(iClientH);
                return;
            }

            if (memcmp(cp, "/clearnpc", 9) == 0)
            {
                AdminOrder_ClearNpc(iClientH);
                return;
            }

            if (memcmp(cp, "/setforcerecalltime ", 20) == 0)
            {
                AdminOrder_SetForceRecallTime(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/enableadmincommand ", 20) == 0)
            {
                AdminOrder_EnableAdminCommand(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/monstercount", 13) == 0)
            {
                AdminOrder_MonsterCount(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/createparty", 12) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 1)
                {
                    RequestCreatePartyHandler(iClientH);
                }
                return;
            }

            if (memcmp(cp, "/joinparty ", 11) == 0)
            {
                RequestJoinPartyHandler(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/dismissparty", 13) == 0)
            {
                RequestDismissPartyHandler(iClientH);
                return;
            }

            if (memcmp(cp, "/blueball", 9) == 0)
            {
                Command_BlueBall(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/redball", 8) == 0)
            {
                Command_RedBall(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/yellowball ", 12) == 0)
            {
                Command_YellowBall(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/getpartyinfo", 13) == 0)
            {
                GetPartyInfoHandler(iClientH);
                return;
            }

            if (memcmp(cp, "/deleteparty", 12) == 0)
            {
                RequestDeletePartyHandler(iClientH);
                return;
            }

            if (memcmp(cp, "/who", 4) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel >= m_iAdminLevelWho)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_TOTALUSERS, NULL, NULL, NULL, NULL);
                }
                return;
            }

            if (memcmp(cp, "/fi ", 4) == 0)
            {
                CheckAndNotifyPlayerConnection(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/to", 3) == 0)
            {
                ToggleWhisperPlayer(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/setpf ", 7) == 0)
            {
                SetPlayerProfile(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/weather", 8) == 0)
            {
                AdminOrder_Weather(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/pf ", 4) == 0)
            {
                GetPlayerProfile(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/shutup ", 8) == 0)
            {
                ShutUpPlayer(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/rep+ ", 6) == 0)
            {
                SetPlayerReputation(iClientH, cp, 1, dwMsgSize - 21);
                return;
            }


            if (memcmp(cp, "/time ", 6) == 0)
            {
                AdminOrder_Time(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/getskills", 10) == 0)
            {
                SetSkillAll(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/checkrep", 9) == 0)
            {
                AdminOrder_CheckRep(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/checkstatus ", 13) == 0)
            {
                AdminOrder_CheckStats(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/send ", 5) == 0)
            {
                AdminOrder_Pushplayer(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/rep- ", 6) == 0)
            {
                SetPlayerReputation(iClientH, cp, 0, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/hold", 5) == 0)
            {
                SetSummonMobAction(iClientH, 1, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/tgt ", 5) == 0)
            {
                SetSummonMobAction(iClientH, 2, dwMsgSize - 21, cp);
                return;
            }

            if (memcmp(cp, "/free", 5) == 0)
            {
                SetSummonMobAction(iClientH, 0, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/summonall ", 11) == 0)
            {
                AdminOrder_SummonAll(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/summonguild", 12) == 0)
            {
                if ((!m_bIsCrusadeMode) || (m_pClientList[iClientH]->m_iGuildRank == 0))
                {
                    if (m_pClientList[iClientH]->m_iGuildRank == 0)
                    {
                        if (dwGetItemCount(iClientH, "Gold") >= 100000)
                        {
                            SetItemCount(iClientH, "Gold", dwGetItemCount(iClientH, "Gold") - 50000);
                            AdminOrder_SummonGuild(iClientH, cp, dwMsgSize - 21);
                            return;
                        }
                    }
                }
            }

            if (memcmp(cp, "/summonplayer ", 14) == 0)
            {
                AdminOrder_SummonPlayer(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/storm ", 7) == 0)
            {
                AdminOrder_SummonStorm(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/summondeath ", 13) == 0)
            {
                AdminOrder_SummonDeath(iClientH);
                return;
            }

            if (memcmp(cp, "/kill ", 6) == 0)
            {
                AdminOrder_Kill(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/revive ", 8) == 0)
            {
                AdminOrder_Revive(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/closeconn ", 11) == 0)
            {
                AdminOrder_CloseConn(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/ban", 4) == 0)
            {
                UserCommand_BanGuildsman(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/reservefightzone", 17) == 0)
            {
                AdminOrder_ReserveFightzone(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/dissmiss ", 9) == 0)
            {
                UserCommand_DissmissGuild(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/attack ", 8) == 0)
            {
                AdminOrder_CallGuard(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/createfish ", 12) == 0)
            {
                AdminOrder_CreateFish(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/teleport ", 10) == 0 || memcmp(cp, "/tp ", 4) == 0)
            {
                AdminOrder_Teleport(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/summondemon ", 13) == 0)
            {
                AdminOrder_SummonDemon(iClientH);
                return;
            }

            if (memcmp(cp, "/unsummonall ", 13) == 0)
            {
                AdminOrder_UnsummonAll(iClientH);
                return;
            }

            if (memcmp(cp, "/unsummondemon ", 15) == 0)
            {
                AdminOrder_UnsummonDemon(iClientH);
                return;
            }

            if (memcmp(cp, "/checkip ", 9) == 0)
            {
                AdminOrder_CheckIP(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/polymorph ", 11) == 0)
            {
                AdminOrder_Polymorph(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/setinvi ", 9) == 0)
            {
                AdminOrder_SetInvi(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/gns ", 4) == 0)
            {
                AdminOrder_GetNpcStatus(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/setattackmode ", 15) == 0)
            {
                AdminOrder_SetAttackMode(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/summon ", 8) == 0)
            {
                AdminOrder_Summon(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/setzerk ", 9) == 0)
            {
                AdminOrder_SetZerk(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/setfreeze ", 11) == 0)
            {
                AdminOrder_SetFreeze(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/setstatus ", 11) == 0)
            {
                AdminOrder_SetStatus(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/disconnectall ", 15) == 0)
            {
                AdminOrder_DisconnectAll(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/createitem ", 12) == 0)
            {
                AdminOrder_CreateItem(iClientH, cp, dwMsgSize - 21);
                return;
            }

            if (memcmp(cp, "/energysphere ", 14) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel >= m_iAdminLevelEnergySphere) EnergySphereProcessor(TRUE, iClientH);
                return;
            }

            if ((memcmp(cp, "/shutdownthisserverrightnow ", 28) == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel >= m_iAdminLevelShutdown))
            {
                m_cShutDownCode = 2;
                m_bOnExitProcess = TRUE;
                m_dwExitProcessTime = timeGetTime();
                log->info("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Admin-Command)!!!");
                //bSendMsgToLS(MSGID_GAMESERVERSHUTDOWNED, NULL);
                if (m_iMiddlelandMapIndex > 0)
                {
                    SaveOccupyFlagData();
                    //bSendMsgToLS(MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA, NULL, NULL);
                    //bSendMsgToLS(MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA, NULL, NULL);
                }
                return;
            }

            if ((memcmp(cp, "/setobservermode ", 17) == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel >= m_iAdminLevelObserver))
            {
                AdminOrder_SetObserverMode(iClientH);
                return;
            }

            if ((memcmp(cp, "/getticket ", 11) == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel >= 2))
            {
                AdminOrder_GetFightzoneTicket(iClientH);
                return;
            }

            if (memcmp(cp, "/beginheldenian ", 16) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 2)
                {
                    ManualStartHeldenianMode(iClientH, cp, dwMsgSize - 21);
                }
                return;
            }

            if (memcmp(cp, "/endheldenian ", 14) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 2)
                {
                    ManualEndHeldenianMode(iClientH, cp, dwMsgSize - 21);
                }
                return;
            }

            return;
    }

    pData[dwMsgSize - 1] = NULL;

    if ((m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] == 1) && (iDice(1, 3) != 2))
    {
        cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 17);

        while (*cp != NULL)
        {
            if ((cp[0] != NULL) && (cp[0] != ' ') && (cp[1] != NULL) && (cp[1] != ' '))
            {
                switch (iDice(1, 3))
                {
                    case 1:	memcpy(cp, "Â¿Ã¶", 2); break;
                    case 2:	memcpy(cp, "Â¿Ã¬", 2); break;
                    case 3:	memcpy(cp, "Â¿Ã¹", 2); break;
                }
                cp += 2;
            }
            else cp++;
        }
    }

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 17);

    if ((cSendMode == NULL) && (m_pClientList[iClientH]->m_iWhisperPlayerIndex != -1))
    {
        cSendMode = 20;

        if (*cp == '#') cSendMode = NULL;

        if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = NULL;
    }

    wp = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
    *wp = (WORD)iClientH;
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 16);
    *cp = cSendMode;

    if (cSendMode != 20)
    {
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if (m_pClientList[i] != NULL)
            {
                switch (cSendMode)
                {
                    case NULL:
                        if (m_pClientList[i]->m_bIsInitComplete == FALSE) break;

                        if ((m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex) &&
                            (m_pClientList[i]->m_sX > m_pClientList[iClientH]->m_sX - 10) &&
                            (m_pClientList[i]->m_sX < m_pClientList[iClientH]->m_sX + 10) &&
                            (m_pClientList[i]->m_sY > m_pClientList[iClientH]->m_sY - 7) &&
                            (m_pClientList[i]->m_sY < m_pClientList[iClientH]->m_sY + 7))
                        {

                            if (m_bIsCrusadeMode == TRUE)
                            {
                                if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[i]->m_cSide != 0) &&
                                    (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide))
                                {
                                }
                                else iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                            }
                            else iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                        }
                        break;

                    case 1:
                        if (m_pClientList[i]->m_bIsInitComplete == FALSE) break;

                        if ((memcmp(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName, 20) == 0) &&
                            (memcmp(m_pClientList[i]->m_cGuildName, "NONE", 4) != 0))
                        {

                            if (m_bIsCrusadeMode == TRUE)
                            {
                                if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[i]->m_cSide != 0) &&
                                    (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide))
                                {
                                }
                                else iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                            }
                            else iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                        }
                        break;

                    case 2:
                    case 10:
                        if (m_bIsCrusadeMode == TRUE)
                        {
                            if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[i]->m_cSide != 0) &&
                                (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide))
                            {
                            }
                            else iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                        }
                        else iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                        break;

                    case 3:
                        if (m_pClientList[i]->m_bIsInitComplete == FALSE) break;

                        if ((m_pClientList[i]->m_cSide == m_pClientList[iClientH]->m_cSide))
                            iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                        break;

                    case 4:
                        if (m_pClientList[i]->m_bIsInitComplete == FALSE) break;
                        if ((m_pClientList[i]->m_iPartyID != NULL) && (m_pClientList[i]->m_iPartyID == m_pClientList[iClientH]->m_iPartyID))
                            iRet = m_pClientList[i]->iSendMsg(pData, dwMsgSize);
                        break;
                }

                switch (iRet)
                {
                    case DEF_XSOCKEVENT_QUENEFULL:
                    case DEF_XSOCKEVENT_SOCKETERROR:
                    case DEF_XSOCKEVENT_CRITICALERROR:
                    case DEF_XSOCKEVENT_SOCKETCLOSED:
                        DeleteClient(i, TRUE, TRUE);
                        break;
                }
            }
    }
    else
    {
        iRet = m_pClientList[iClientH]->iSendMsg(pData, dwMsgSize);
        if (m_pClientList[iClientH]->m_iWhisperPlayerIndex == 10000)
        {
            ZeroMemory(cBuffer, sizeof(cBuffer));
            cp = (char *)cBuffer;
            *cp = GSM_WHISFERMSG;
            cp++;
            memcpy(cp, m_pClientList[iClientH]->m_cWhisperPlayerName, 10);
            cp += 10;

            wp = (WORD *)cp;
            *wp = (WORD)dwMsgSize;
            cp += 2;
            memcpy(cp, pData, dwMsgSize);
            cp += dwMsgSize;
            bStockMsgToGateServer(cBuffer, dwMsgSize + 13);
        }
        else
        {
            if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex] != NULL &&
                strcmp(m_pClientList[iClientH]->m_cWhisperPlayerName, m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_cCharName) == 0)
            {
                iRet = m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->iSendMsg(pData, dwMsgSize);
                switch (m_bLogChatOption)
                {

                    case 1:
                        if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_iAdminUserLevel == 0)
                        {
                            ZeroMemory(cTemp, sizeof(cTemp));
                            wsprintf(cTemp, "GM Whisper   (%s):\"%s\"\tto Player(%s)", m_pClientList[iClientH]->m_cCharName, pData + 21, m_pClientList[iClientH]->m_cWhisperPlayerName);
                            //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
                        }
                        break;

                    case 2:
                        if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_iAdminUserLevel > 0)
                        {
                            ZeroMemory(cTemp, sizeof(cTemp));
                            wsprintf(cTemp, "GM Whisper   (%s):\"%s\"\tto GM(%s)", m_pClientList[iClientH]->m_cCharName, pData + 21, m_pClientList[iClientH]->m_cWhisperPlayerName);
                            //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
                        }
                        break;

                    case 3:
                        if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_iAdminUserLevel > 0)
                        {
                            ZeroMemory(cTemp, sizeof(cTemp));
                            wsprintf(cTemp, "GM Whisper   (%s):\"%s\"\tto GM(%s)", m_pClientList[iClientH]->m_cCharName, pData + 21, m_pClientList[iClientH]->m_cWhisperPlayerName);
                            //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
                        }
                        else
                        {
                            ZeroMemory(cTemp, sizeof(cTemp));
                            wsprintf(cTemp, "Player Whisper   (%s):\"%s\"\tto Player(%s)", m_pClientList[iClientH]->m_cCharName, pData + 21, m_pClientList[iClientH]->m_cWhisperPlayerName);
                            //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cTemp);
                        }
                        break;

                    case 4:
                        break;
                }
            }
        }

        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:
                //DeleteClient(i, TRUE, TRUE);
                break;
        }
    }
}

void CGame::RequestCreateNewGuildHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    char * cp, cGuildName[30], cTxt[120], cData[100];
    DWORD * dwp;
    WORD * wp;
    int     iRet;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_bIsCrusadeMode == TRUE) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    cp += 10;
    cp += 10;
    cp += 10;

    ZeroMemory(cGuildName, sizeof(cGuildName));
    memcpy(cGuildName, cp, 20);
    cp += 20;

    if (m_pClientList[iClientH]->m_iGuildRank != -1)
    {
        wsprintf(cTxt, "(!)Cannot create guild! Already guild member.: CharName(%s)", m_pClientList[iClientH]->m_cCharName);
        log->info(cTxt);
    }
    else
    {
        if ((m_pClientList[iClientH]->m_iLevel < 20) || (m_pClientList[iClientH]->m_iCharisma < 20) ||
            (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) ||
            (memcmp(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10) != 0))
        {
            ZeroMemory(cData, sizeof(cData));

            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_CREATENEWGUILD;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_MSGTYPE_REJECT;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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
            ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
            strcpy(m_pClientList[iClientH]->m_cGuildName, cGuildName);

            ZeroMemory(m_pClientList[iClientH]->m_cLocation, sizeof(m_pClientList[iClientH]->m_cLocation));
            strcpy(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName);

            GetLocalTime(&SysTime);
            m_pClientList[iClientH]->m_iGuildGUID = (int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());

            //bSendMsgToLS(MSGID_REQUEST_CREATENEWGUILD, iClientH);
        }
    }
}

void CGame::RequestDisbandGuildHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    char * cp, cGuildName[30], cTxt[120];

    if (m_bIsCrusadeMode == TRUE) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    ZeroMemory(cGuildName, sizeof(cGuildName));

    cp += 10;
    cp += 10;
    cp += 10;

    memcpy(cGuildName, cp, 20);
    cp += 20;

    if ((m_pClientList[iClientH]->m_iGuildRank != 0) || (memcmp(m_pClientList[iClientH]->m_cGuildName, cGuildName, 20) != 0))
    {
        wsprintf(cTxt, "(!)Cannot Disband guild! Not guildmaster.: CharName(%s)", m_pClientList[iClientH]->m_cCharName);
        log->info(cTxt);
    }
    else
    {
        //bSendMsgToLS(MSGID_REQUEST_DISBANDGUILD, iClientH);
    }
}

void CGame::RequestPurchaseItemHandler(int iClientH, char * pItemName, int iNum)
{
    CItem * pItem;
    char * cp, cItemName[30], cData[100];
    short * sp;
    DWORD * dwp, dwGoldCount, dwItemCount;
    WORD * wp, wTempPrice;
    int   i, iRet, iEraseReq, iGoldWeight;
    int   iCost, iDiscountRatio, iDiscountCost;
    double dTmp1, dTmp2, dTmp3;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0)
    {
        if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0)
        {
            if ((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0) ||
                (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arefarm", 7) == 0))
            {

            }
            else return;
        }

        if (memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0)
        {
            if ((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0) ||
                (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvfarm", 7) == 0))
            {

            }
            else return;
        }
    }

    ZeroMemory(cData, sizeof(cData));
    ZeroMemory(cItemName, sizeof(cItemName));

    if (m_pClientList[iClientH]->m_pIsProcessingAllowed == FALSE) return;

    if (memcmp(pItemName, "10Arrows", 8) == 0)
    {
        strcpy(cItemName, "Arrow");
        dwItemCount = 10;
    }
    else if (memcmp(pItemName, "100Arrows", 9) == 0)
    {
        strcpy(cItemName, "Arrow");
        dwItemCount = 100;
    }
    else
    {
        memcpy(cItemName, pItemName, 20);
        dwItemCount = 1;
    }

    for (i = 1; i <= iNum; i++)
    {

        pItem = new CItem;
        if (_bInitItemAttr(pItem, cItemName) == FALSE)
        {
            delete pItem;
        }
        else
        {

            if (pItem->m_bIsForSale == FALSE)
            {
                delete pItem;
                return;
            }

            pItem->m_dwCount = dwItemCount;

            iCost = pItem->m_wPrice * pItem->m_dwCount;


            dwGoldCount = dwGetItemCount(iClientH, "Gold");

            iDiscountRatio = ((m_pClientList[iClientH]->m_iCharisma - 10) / 4);

            dTmp1 = (double)(iDiscountRatio);
            dTmp2 = dTmp1 / 100.0f;
            dTmp1 = (double)iCost;
            dTmp3 = dTmp1 * dTmp2;
            iDiscountCost = (int)dTmp3;

            if (iDiscountCost >= (iCost / 2)) iDiscountCost = (iCost / 2) - 1;

            if (dwGoldCount < (DWORD)(iCost - iDiscountCost))
            {
                delete pItem;

                dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_NOTENOUGHGOLD;
                cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
                *cp = -1;
                cp++;

                iRet = m_pClientList[iClientH]->iSendMsg(cData, 7);
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

            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
            {
                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

                dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_ITEMPURCHASED;

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

                wp = (WORD *)cp;
                *wp = (iCost - iDiscountCost);
                wTempPrice = (iCost - iDiscountCost);
                cp += 2;

                if (iEraseReq == 1) delete pItem;

                iRet = m_pClientList[iClientH]->iSendMsg(cData, 48);

                iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - wTempPrice);
                iCalcTotalWeight(iClientH);

                m_stCityStatus[m_pClientList[iClientH]->m_cSide].iFunds += wTempPrice;

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
                delete pItem;

                iCalcTotalWeight(iClientH);

                dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

                iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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
        }
    }
}

void CGame::RequestTeleportHandler(int iClientH, char * pData, char * cMapName, int dX, int dY)
{
    char * pBuffer, cTempMapName[30];
    DWORD * dwp;
    WORD * wp;
    char * cp, cDestMapName[11], cDir, cMapIndex, cQuestRemain;
    short * sp, sX, sY, sSummonPoints;
    int * ip, i, iRet, iSize, iDestX, iDestY, iExH, iMapSide;
    BOOL    bRet, bIsLockedMapNotify;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;
    if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == TRUE) return;
    if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsRecallImpossible == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0) &&
        (m_pClientList[iClientH]->m_bIsKilled == FALSE) && (m_bIsApocalypseMode == TRUE) && (m_pClientList[iClientH]->m_iHP > 0))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NORECALL, NULL, NULL, NULL, NULL);
        return;
    }
    if ((memcmp(m_pClientList[iClientH]->m_cLocation, "elvine", 6) == 0)
        && (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0)
        && (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
        && ((pData[0] == '1') || (pData[0] == '3'))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
        && (m_bIsCrusadeMode == FALSE)) return;

    if ((memcmp(m_pClientList[iClientH]->m_cLocation, "aresden", 7) == 0)
        && (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0)
        && (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
        && ((pData[0] == '1') || (pData[0] == '3'))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
        && (m_bIsCrusadeMode == FALSE)) return;

    bIsLockedMapNotify = FALSE;

    if (m_pClientList[iClientH]->m_bIsExchangeMode == TRUE)
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;
        _ClearExchangeStatus(iExH);
        _ClearExchangeStatus(iClientH);
    }

    if ((memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) && (pData[0] == '1'))
        return;

    RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(13, iClientH, DEF_OWNERTYPE_PLAYER,
        m_pClientList[iClientH]->m_sX,
        m_pClientList[iClientH]->m_sY);

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, NULL, NULL, NULL);

    sX = m_pClientList[iClientH]->m_sX;
    sY = m_pClientList[iClientH]->m_sY;

    ZeroMemory(cDestMapName, sizeof(cDestMapName));
    bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSearchTeleportDest(sX, sY, cDestMapName, &iDestX, &iDestY, &cDir);

    if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0))
    {
        iMapSide = iGetMapLocationSide(cDestMapName);
        if (iMapSide > 3) iMapSide -= 2;
        if ((iMapSide != 0) && (m_pClientList[iClientH]->m_cSide == iMapSide))
        {
        }
        else
        {
            iDestX = -1;
            iDestY = -1;
            bIsLockedMapNotify = TRUE;
            ZeroMemory(cDestMapName, sizeof(cDestMapName));
            strcpy(cDestMapName, m_pClientList[iClientH]->m_cLockedMapName);
        }
    }

    if ((bRet == TRUE) && (cMapName == NULL))
    {
        for (i = 0; i < DEF_MAXMAPS; i++)
            if (m_pMapList[i] != NULL)
            {
                if (memcmp(m_pMapList[i]->m_cName, cDestMapName, 10) == 0)
                {
                    m_pClientList[iClientH]->m_sX = iDestX;
                    m_pClientList[iClientH]->m_sY = iDestY;
                    m_pClientList[iClientH]->m_cDir = cDir;
                    m_pClientList[iClientH]->m_cMapIndex = i;
                    ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
                    memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[i]->m_cName, 10);
                    goto RTH_NEXTSTEP;
                }
            }

        m_pClientList[iClientH]->m_sX = iDestX;
        m_pClientList[iClientH]->m_sY = iDestY;
        m_pClientList[iClientH]->m_cDir = cDir;
        ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
        memcpy(m_pClientList[iClientH]->m_cMapName, cDestMapName, 10);

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_CONFUSE,
            m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE], NULL, NULL);
        SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, FALSE);

        //bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, iClientH, FALSE);
        m_pClientList[iClientH]->m_bIsOnServerChange = TRUE;
        m_pClientList[iClientH]->m_bIsOnWaitingProcess = TRUE;
        return;
    }
    else
    {
        switch (pData[0])
        {
            case '0':
                ZeroMemory(cTempMapName, sizeof(cTempMapName));
                if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0)
                {
                    strcpy(cTempMapName, "default");
                }
                else if (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0)
                {
                    strcpy(cTempMapName, "arefarm");
                }
                else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0)
                {
                    strcpy(cTempMapName, "elvfarm");
                }
                else strcpy(cTempMapName, m_pClientList[iClientH]->m_cLocation);

                if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0))
                {
                    bIsLockedMapNotify = TRUE;
                    ZeroMemory(cTempMapName, sizeof(cTempMapName));
                    strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
                }

                for (i = 0; i < DEF_MAXMAPS; i++)
                    if (m_pMapList[i] != NULL)
                    {
                        if (memcmp(m_pMapList[i]->m_cName, cTempMapName, 10) == 0)
                        {
                            GetMapInitialPoint(i, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cLocation);

                            m_pClientList[iClientH]->m_cMapIndex = i;
                            ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
                            memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);
                            goto RTH_NEXTSTEP;
                        }
                    }

                m_pClientList[iClientH]->m_sX = -1;
                m_pClientList[iClientH]->m_sY = -1;

                ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
                memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_CONFUSE,
                    m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE], NULL, NULL);
                SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, FALSE);

                //bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, iClientH, FALSE);

                m_pClientList[iClientH]->m_bIsOnServerChange = TRUE;
                m_pClientList[iClientH]->m_bIsOnWaitingProcess = TRUE;
                return;

            case '1':
                // if (memcmp(m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_cName, "resurr", 6) == 0) return;

                ZeroMemory(cTempMapName, sizeof(cTempMapName));
                if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0)
                {
                    strcpy(cTempMapName, "default");
                }
                else
                {
                    if (m_pClientList[iClientH]->m_iLevel > 80)
                        if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0)
                            strcpy(cTempMapName, "aresden");
                        else strcpy(cTempMapName, "elvine");
                    else
                    {
                        if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0)
                            strcpy(cTempMapName, "arefarm");
                        else strcpy(cTempMapName, "elvfarm");
                    }
                }
                if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0))
                {
                    bIsLockedMapNotify = TRUE;
                    ZeroMemory(cTempMapName, sizeof(cTempMapName));
                    strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
                }

                for (i = 0; i < DEF_MAXMAPS; i++)
                    if (m_pMapList[i] != NULL)
                    {
                        if (memcmp(m_pMapList[i]->m_cName, cTempMapName, 10) == 0)
                        {
                            GetMapInitialPoint(i, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cLocation);

                            m_pClientList[iClientH]->m_cMapIndex = i;
                            ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
                            memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[i]->m_cName, 10);
                            goto RTH_NEXTSTEP;
                        }
                    }

                m_pClientList[iClientH]->m_sX = -1;
                m_pClientList[iClientH]->m_sY = -1;

                ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
                memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_CONFUSE,
                    m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE], NULL, NULL);
                SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, FALSE);

                //bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, iClientH, FALSE);
                m_pClientList[iClientH]->m_bIsOnServerChange = TRUE;
                m_pClientList[iClientH]->m_bIsOnWaitingProcess = TRUE;
                return;

            case '2':
                if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0))
                {
                    dX = -1;
                    dY = -1;
                    bIsLockedMapNotify = TRUE;
                    ZeroMemory(cTempMapName, sizeof(cTempMapName));
                    strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
                }
                else
                {
                    ZeroMemory(cTempMapName, sizeof(cTempMapName));
                    strcpy(cTempMapName, cMapName);
                }

                cMapIndex = iGetMapIndex(cTempMapName);
                if (cMapIndex == -1)
                {
                    m_pClientList[iClientH]->m_sX = dX;
                    m_pClientList[iClientH]->m_sY = dY;

                    ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
                    memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_CONFUSE,
                        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE], NULL, NULL);
                    SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, FALSE);

                    //bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, iClientH, FALSE);
                    m_pClientList[iClientH]->m_bIsOnServerChange = TRUE;
                    m_pClientList[iClientH]->m_bIsOnWaitingProcess = TRUE;
                    return;
                }

                m_pClientList[iClientH]->m_sX = dX;
                m_pClientList[iClientH]->m_sY = dY;
                m_pClientList[iClientH]->m_cMapIndex = cMapIndex;

                ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
                memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[cMapIndex]->m_cName, 10);
                break;
        }
    }

    RTH_NEXTSTEP:;

    SetPlayingStatus(iClientH);
    int iTemp, iTemp2;
    iTemp = m_pClientList[iClientH]->m_iStatus;
    iTemp = 0x0FFFFFFF & iTemp;
    iTemp2 = iGetPlayerABSStatus(iClientH);
    iTemp = iTemp | (iTemp2 << 28);
    m_pClientList[iClientH]->m_iStatus = iTemp;

    if (bIsLockedMapNotify == TRUE) SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LOCKEDMAP, m_pClientList[iClientH]->m_iLockedMapTime, NULL, NULL, m_pClientList[iClientH]->m_cLockedMapName);

    pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
    ZeroMemory(pBuffer, DEF_MSGBUFFERSIZE + 1);

    dwp = (DWORD *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_INITDATA;
    wp = (WORD *)(pBuffer + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

    if (m_pClientList[iClientH]->m_bIsObserverMode == FALSE)
        bGetEmptyPosition(&m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cMapIndex);
    else GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY);

    sp = (short *)cp;
    *sp = iClientH;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sX;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sY;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sType;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr1;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr2;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr3;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sAppr4;
    cp += 2;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iApprColor;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iStatus;
    cp += 4;

    memcpy(cp, m_pClientList[iClientH]->m_cMapName, 10);
    cp += 10;

    memcpy(cp, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10);
    cp += 10;

    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == TRUE)
        *cp = 1;
    else *cp = m_cDayOrNight;
    cp++;

    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == TRUE)
        *cp = NULL;
    else *cp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus;
    cp++;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iContribution;
    cp += 4;

    if (m_pClientList[iClientH]->m_bIsObserverMode == FALSE)
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH,
            DEF_OWNERTYPE_PLAYER,
            m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY);
    }

    *cp = (char)m_pClientList[iClientH]->m_bIsObserverMode;
    cp++;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iRating;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iHP;
    cp += 4;

    *cp = 0;
    cp++;

    iSize = iComposeInitMapData(m_pClientList[iClientH]->m_sX - 10, m_pClientList[iClientH]->m_sY - 7, iClientH, cp);
    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 46 + iSize + 4 + 4 + 1 + 4 + 4 + 3);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            DeleteClient(iClientH, TRUE, TRUE);
            if (pBuffer != NULL) delete pBuffer;
            return;
    }

    if (pBuffer != NULL) delete pBuffer;

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, NULL, NULL, NULL);

    if ((memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0) &&
        (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
        CheckForceRecallTime(iClientH);
    }
    else if ((memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0) &&
        (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = TRUE;

        CheckForceRecallTime(iClientH);
    }
    else if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == TRUE)
    {
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
        SetForceRecallTime(iClientH);

        GetLocalTime(&SysTime);
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2 * 20 * 60 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 2 * 20;

    }
    else
    {
        m_pClientList[iClientH]->m_bIsWarLocation = FALSE;
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 0;
        SetForceRecallTime(iClientH);
    }

    int iMapside, iMapside2;

    iMapside = iGetMapLocationSide(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);
    if (iMapside > 3) iMapside2 = iMapside - 2;
    else iMapside2 = iMapside;
    m_pClientList[iClientH]->m_bIsInsideOwnTown = FALSE;
    if ((m_pClientList[iClientH]->m_cSide != iMapside2) && (iMapside != 0))
    {
        if ((iMapside <= 2) && (m_pClientList[iClientH]->m_iAdminUserLevel < 1))
        {
            if (m_pClientList[iClientH]->m_cSide != 0)
            {
                m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
                m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
                m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
                m_pClientList[iClientH]->m_bIsInsideOwnTown = TRUE;
            }
        }
    }
    else
    {
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == TRUE &&
            m_iFightzoneNoForceRecall == FALSE &&
            m_pClientList[iClientH]->m_iAdminUserLevel == 0)
        {
            m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
            m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
            GetLocalTime(&SysTime);
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2 * 60 * 20 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 2 * 20;
        }
        else
        {
            if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arejail", 7) == 0 ||
                memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvjail", 7) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
                {
                    m_pClientList[iClientH]->m_bIsWarLocation = TRUE;
                    m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
                    if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0)
                        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 100;
                    else if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 100)
                        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 100;
                }
            }
        }
    }

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SAFEATTACKMODE, NULL, NULL, NULL, NULL);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, NULL, NULL, NULL);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMPOSLIST, NULL, NULL, NULL, NULL);
    _SendQuestContents(iClientH);
    _CheckQuestEnvironment(iClientH);

    if (m_pClientList[iClientH]->m_iSpecialAbilityTime == 0)
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYENABLED, NULL, NULL, NULL, NULL);

    if (m_bIsCrusadeMode == TRUE)
    {
        if (m_pClientList[iClientH]->m_dwCrusadeGUID == 0)
        {
            m_pClientList[iClientH]->m_iCrusadeDuty = 0;
            m_pClientList[iClientH]->m_iConstructionPoint = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = m_dwCrusadeGUID;
        }
        else if (m_pClientList[iClientH]->m_dwCrusadeGUID != m_dwCrusadeGUID)
        {
            m_pClientList[iClientH]->m_iCrusadeDuty = 0;
            m_pClientList[iClientH]->m_iConstructionPoint = 0;
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = m_dwCrusadeGUID;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, 0, NULL, -1);
        }
        m_pClientList[iClientH]->m_cVar = 1;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, NULL, NULL);
    }
    else if (m_bIsHeldenianMode == TRUE)
    {
        sSummonPoints = m_pClientList[iClientH]->m_iCharisma * 300;
        if (sSummonPoints > DEF_MAXSUMMONPOINTS) sSummonPoints = DEF_MAXSUMMONPOINTS;
        if (m_pClientList[iClientH]->m_dwHeldenianGUID == NULL)
        {
            m_pClientList[iClientH]->m_dwHeldenianGUID = m_dwHeldenianGUID;
            m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
        }
        else if (m_pClientList[iClientH]->m_dwHeldenianGUID != m_dwHeldenianGUID)
        {
            m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwHeldenianGUID = m_dwHeldenianGUID;
        }
        m_pClientList[iClientH]->m_cVar = 2;
        if (m_bIsHeldenianMode == TRUE)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANTELEPORT, NULL, NULL, NULL, NULL);
        }
        if (m_bHeldenianInitiated == TRUE)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANSTART, NULL, NULL, NULL, NULL);
        }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, NULL, NULL);
        UpdateHeldenianStatus();
    }
    else if ((m_pClientList[iClientH]->m_cVar == 1) && (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID))
    {
        m_pClientList[iClientH]->m_iCrusadeDuty = 0;
        m_pClientList[iClientH]->m_iConstructionPoint = 0;
    }
    else
    {
        if (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID)
        {
            if (m_pClientList[iClientH]->m_cVar == 1)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, NULL, NULL, NULL, -1);
            }
        }
        else
        {
            m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
        }
    }

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 5) == 0)
    {
        wsprintf(G_cTxt, "Char(%s)-Enter(%s) Observer(%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_bIsObserverMode);
        log->info(G_cTxt);
    }

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 1, NULL);

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);

    if (m_bIsHeldenianMode == TRUE)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANTELEPORT, NULL, NULL, NULL, NULL);
        if (m_bHeldenianInitiated == TRUE)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELDENIANSTART, NULL, NULL, NULL, NULL);
        }
        else
        {
            UpdateHeldenianStatus();
        }
    }

    if (m_pClientList[iClientH]->m_iQuest != NULL)
    {
        cQuestRemain = (m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iMaxCount - m_pClientList[iClientH]->m_iCurQuestCount);
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCOUNTER, cQuestRemain, NULL, NULL, NULL);
        _bCheckIsQuestCompleted(iClientH);
    }

}

void CGame::RequestStudyMagicHandler(int iClientH, char * pName, BOOL bIsPurchase)
{
    char * cp, cMagicName[31], cData[100];
    DWORD * dwp, dwGoldCount;
    WORD * wp;
    int * ip, iReqInt, iCost, iRet;
    BOOL bMagic = TRUE;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    ZeroMemory(cData, sizeof(cData));

    ZeroMemory(cMagicName, sizeof(cMagicName));
    memcpy(cMagicName, pName, 30);

    iRet = _iGetMagicNumber(cMagicName, &iReqInt, &iCost);
    if (iRet == -1)
    {
    }
    else
    {
        if (bIsPurchase == TRUE)
        {
            if (m_pMagicConfigList[iRet]->m_iGoldCost < 0) bMagic = FALSE;
            dwGoldCount = dwGetItemCount(iClientH, "Gold");
            if ((DWORD)iCost > dwGoldCount)  bMagic = FALSE;
        }
        if (m_pClientList[iClientH]->m_bIsInsideWizardTower == FALSE && bIsPurchase) return;
        if (m_pClientList[iClientH]->m_cMagicMastery[iRet] != 0) return;

        if ((iReqInt <= m_pClientList[iClientH]->m_iInt) && (bMagic == TRUE))
        {

            if (bIsPurchase == TRUE) SetItemCount(iClientH, "Gold", dwGoldCount - iCost);

            iCalcTotalWeight(iClientH);

            m_pClientList[iClientH]->m_cMagicMastery[iRet] = 1;

            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_MAGICSTUDYSUCCESS;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

            *cp = iRet;
            cp++;

            memcpy(cp, cMagicName, 30);
            cp += 30;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 37);

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
            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_MAGICSTUDYFAIL;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
            *cp = 1;
            cp++;

            *cp = iRet;
            cp++;

            memcpy(cp, cMagicName, 30);
            cp += 30;

            ip = (int *)cp;
            *ip = iCost;
            cp += 4;

            ip = (int *)cp;
            *ip = iReqInt;
            cp += 4;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 46);
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
    }
}

void CGame::StateChangeHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    char * cp, cStateChange1, cStateChange2, cStateChange3;
    char cStr, cVit, cDex, cInt, cMag, cChar;
    char cStateTxt[512];
    int iOldStr, iOldVit, iOldDex, iOldInt, iOldMag, iOldChar;
    int iTotalSetting = 0;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) return;

    cStr = cVit = cDex = cInt = cMag = cChar = 0;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    cStateChange1 = *cp;
    cp++;

    cStateChange2 = *cp;
    cp++;

    cStateChange3 = *cp;
    cp++;

    iOldStr = m_pClientList[iClientH]->m_iStr;
    iOldVit = m_pClientList[iClientH]->m_iVit;
    iOldDex = m_pClientList[iClientH]->m_iDex;
    iOldInt = m_pClientList[iClientH]->m_iInt;
    iOldMag = m_pClientList[iClientH]->m_iMag;
    iOldChar = m_pClientList[iClientH]->m_iCharisma;

    try
    {
        wsprintf(G_cTxt, "(*) Char(%s) Str(%d) Vit(%d) Dex(%d) Int(%d) Mag(%d) Chr(%d) ", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iStr, m_pClientList[iClientH]->m_iVit, m_pClientList[iClientH]->m_iDex, m_pClientList[iClientH]->m_iInt, m_pClientList[iClientH]->m_iMag, m_pClientList[iClientH]->m_iCharisma);
        log->info(G_cTxt);
    }
    catch (...)
    {
    }

    if (!bChangeState(cStateChange1, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }
    if (!bChangeState(cStateChange2, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }
    if (!bChangeState(cStateChange3, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    try
    {
        wsprintf(G_cTxt, "(*) Char(%s) St1(%d) St2(%d) St3(%d) Str(%d) Vit(%d) Dex(%d) Int(%d) Mag(%d) Chr(%d) ", m_pClientList[iClientH]->m_cCharName, cStateChange1, cStateChange2, cStateChange3, cStr, cVit, cDex, cInt, cMag, cChar);
        log->info(G_cTxt);
    }
    catch (...)
    {
    }

    if (m_pClientList[iClientH]->m_iGuildRank == 0)
    {
        if (m_pClientList[iClientH]->m_iCharisma - cChar < 20)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
            return;
        }
    }

    if (iOldStr + iOldVit + iOldDex + iOldInt + iOldMag + iOldChar != (179 * 3 + 70))
    {
        return;
    }


    if (cStr < 0 || cVit < 0 || cDex < 0 || cInt < 0 || cMag < 0 || cChar < 0
        || cStr + cVit + cDex + cInt + cMag + cChar != 3)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if ((m_pClientList[iClientH]->m_iStr - cStr > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iStr - cStr < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if ((m_pClientList[iClientH]->m_iDex - cDex > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iDex - cDex < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if ((m_pClientList[iClientH]->m_iInt - cInt > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iInt - cInt < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if ((m_pClientList[iClientH]->m_iVit - cVit > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iVit - cVit < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if ((m_pClientList[iClientH]->m_iMag - cMag > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iMag - cMag < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if ((m_pClientList[iClientH]->m_iCharisma - cChar > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iCharisma - cChar < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if (m_pClientList[iClientH]->m_iLU_Pool < 3) m_pClientList[iClientH]->m_iLU_Pool = 3;

    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft--;

    m_pClientList[iClientH]->m_iHP -= (cStr + 3 * cVit);
    m_pClientList[iClientH]->m_iMP -= (cInt + 2 * cMag);
    m_pClientList[iClientH]->m_iSP -= (2 * cStr);
    m_pClientList[iClientH]->m_iStr -= cStr;
    m_pClientList[iClientH]->m_iVit -= cVit;
    m_pClientList[iClientH]->m_iDex -= cDex;
    m_pClientList[iClientH]->m_iInt -= cInt;

    if (cInt > 0)
        bCheckMagicInt(iClientH);
    m_pClientList[iClientH]->m_iMag -= cMag;
    m_pClientList[iClientH]->m_iCharisma -= cChar;
    //SkillCheck(iClientH);

    try
    {
        ZeroMemory(cStateTxt, sizeof(cStateTxt));

        wsprintf(cStateTxt, "STR(%d->%d)VIT(%d->%d)DEX(%d->%d)INT(%d->%d)MAG(%d->%d)CHARISMA(%d->%d)",
            iOldStr, m_pClientList[iClientH]->m_iStr,
            iOldVit, m_pClientList[iClientH]->m_iVit,
            iOldDex, m_pClientList[iClientH]->m_iDex,
            iOldInt, m_pClientList[iClientH]->m_iInt,
            iOldMag, m_pClientList[iClientH]->m_iMag,
            iOldChar, m_pClientList[iClientH]->m_iCharisma
        );
    }
    catch (...)
    {

    }

    //bCheckSkillState(iClientH);

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_SUCCESS, NULL, NULL, NULL, NULL);
}

void CGame::LevelUpSettingsHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    stream_read sr{ pData, dwMsgSize };
    int16_t cStr, cVit, cDex, cInt, cMag, cChar;
    int iTotalSetting = 0;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_iLU_Pool <= 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    sr.read_uint32();
    sr.read_uint16();
    cStr = sr.read_int16();
    cVit = sr.read_int16();
    cDex = sr.read_int16();
    cInt = sr.read_int16();
    cMag = sr.read_int16();
    cChar = sr.read_int16();

    try
    {
        std::string str = std::format("(*) Char({}) , STR ({}), DEX ({}), INT ({}), MAG ({}), VIT({}), CHR ({})", m_pClientList[iClientH]->m_cCharName, cStr, cDex, cInt, cMag, cVit, cChar);
        log->info(G_cTxt);
    }
    catch (...)
    {
    }

    if ((cStr + cVit + cDex + cInt + cMag + cChar) > m_pClientList[iClientH]->m_iLU_Pool)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if ((m_pClientList[iClientH]->m_iStr + cStr > DEF_CHARPOINTLIMIT) || (cStr < 0))
        return;

    if ((m_pClientList[iClientH]->m_iDex + cDex > DEF_CHARPOINTLIMIT) || (cDex < 0))
        return;

    if ((m_pClientList[iClientH]->m_iInt + cInt > DEF_CHARPOINTLIMIT) || (cInt < 0))
        return;

    if ((m_pClientList[iClientH]->m_iVit + cVit > DEF_CHARPOINTLIMIT) || (cVit < 0))
        return;

    if ((m_pClientList[iClientH]->m_iMag + cMag > DEF_CHARPOINTLIMIT) || (cMag < 0))
        return;

    if ((m_pClientList[iClientH]->m_iCharisma + cChar > DEF_CHARPOINTLIMIT) || (cChar < 0))
        return;

    iTotalSetting = m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iDex + m_pClientList[iClientH]->m_iVit +
        m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma;

    if (iTotalSetting + m_pClientList[iClientH]->m_iLU_Pool - 3 > ((m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70))
    {
        m_pClientList[iClientH]->m_iLU_Pool = 3 + (m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70 - iTotalSetting;

        if (m_pClientList[iClientH]->m_iLU_Pool < 3)
            m_pClientList[iClientH]->m_iLU_Pool = 3;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    if (iTotalSetting + (cStr + cVit + cDex + cInt + cMag + cChar)
        > ((m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, NULL, NULL, NULL, NULL);
        return;
    }

    m_pClientList[iClientH]->m_iLU_Pool = m_pClientList[iClientH]->m_iLU_Pool - (cStr + cVit + cDex + cInt + cMag + cChar);

    m_pClientList[iClientH]->m_iStr += cStr;
    m_pClientList[iClientH]->m_iVit += cVit;
    m_pClientList[iClientH]->m_iDex += cDex;
    m_pClientList[iClientH]->m_iInt += cInt;
    m_pClientList[iClientH]->m_iMag += cMag;
    m_pClientList[iClientH]->m_iCharisma += cChar;

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_SUCCESS, NULL, NULL, NULL, NULL);

}

void CGame::FightzoneReserveHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    char cData[100];
    int iFightzoneNum, * ip, iEnableReserveTime;
    DWORD * dwp, dwGoldCount;
    WORD * wp, wResult;
    int     iRet, iResult = 1, iCannotReserveDay;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    GetLocalTime(&SysTime);

    iEnableReserveTime = 2 * 20 * 60 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 5 * 20;

    dwGoldCount = dwGetItemCount(iClientH, "Gold");

    ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
    iFightzoneNum = *ip;

    if ((iFightzoneNum < 1) || (iFightzoneNum > DEF_MAXFIGHTZONE)) return;

    iCannotReserveDay = (SysTime.wDay + m_pClientList[iClientH]->m_cSide + iFightzoneNum) % 2;
    if (iEnableReserveTime <= 0)
    {
        wResult = DEF_MSGTYPE_REJECT;
        iResult = 0;
    }
    else if (m_iFightZoneReserve[iFightzoneNum - 1] != 0)
    {
        wResult = DEF_MSGTYPE_REJECT;
        iResult = -1;
    }
    else if (dwGoldCount < 1500)
    {
        wResult = DEF_MSGTYPE_REJECT;
        iResult = -2;
    }
    else if (iCannotReserveDay)
    {
        wResult = DEF_MSGTYPE_REJECT;
        iResult = -3;
    }
    else if (m_pClientList[iClientH]->m_iFightzoneNumber != 0)
    {
        wResult = DEF_MSGTYPE_REJECT;
        iResult = -4;
    }
    else
    {
        wResult = DEF_MSGTYPE_CONFIRM;

        SetItemCount(iClientH, "Gold", dwGoldCount - 1500);
        iCalcTotalWeight(iClientH);

        m_iFightZoneReserve[iFightzoneNum - 1] = iClientH;

        m_pClientList[iClientH]->m_iFightzoneNumber = iFightzoneNum;
        m_pClientList[iClientH]->m_iReserveTime = SysTime.wMonth * 10000 + SysTime.wDay * 100 + SysTime.wHour;

        if (SysTime.wHour % 2)	m_pClientList[iClientH]->m_iReserveTime += 1;
        else					m_pClientList[iClientH]->m_iReserveTime += 2;
        wsprintf(G_cTxt, "(*) Reserve FIGHTZONETICKET : Char(%s) TICKENUMBER (%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iReserveTime);
        log->info(G_cTxt);
        log->info(G_cTxt);

        m_pClientList[iClientH]->m_iFightZoneTicketNumber = 50;
        iResult = 1;
    }

    ZeroMemory(cData, sizeof(cData));

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_FIGHTZONE_RESERVE;

    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = wResult;

    ip = (int *)(cData + DEF_INDEX2_MSGTYPE + 2);
    *ip = iResult;
    ip += 4;

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
}

void CGame::RequestCivilRightHandler(int iClientH, char * pData)
{
    char * cp, cData[100]{};
    DWORD * dwp;
    WORD * wp, wResult;
    int  iRet;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) wResult = 0;
    else wResult = 1;

    if (m_pClientList[iClientH]->m_iLevel < 5) wResult = 0;

    if (wResult == 1)
    {
        ZeroMemory(m_pClientList[iClientH]->m_cLocation, sizeof(m_pClientList[iClientH]->m_cLocation));
        strcpy(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName);
    }

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0)
        m_pClientList[iClientH]->m_cSide = 1;

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0)
        m_pClientList[iClientH]->m_cSide = 2;

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_CIVILRIGHT;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = wResult;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
    memcpy(cp, m_pClientList[iClientH]->m_cLocation, 10);
    cp += 10;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 16);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            DeleteClient(iClientH, TRUE, TRUE);
            return;
    }
    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
}

void CGame::RequestRetrieveItemHandler(int iClientH, char * pData)
{
    char * cp, cBankItemIndex, cMsg[100];
    int i, j, iRet, iItemWeight;
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    cBankItemIndex = *cp;

    if (m_pClientList[iClientH]->m_bIsInsideWarehouse == FALSE) return;

    if ((cBankItemIndex < 0) || (cBankItemIndex >= DEF_MAXBANKITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] == NULL)
    {
        ZeroMemory(cMsg, sizeof(cMsg));

        dwp = (DWORD *)(cMsg + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_RETRIEVEITEM;
        wp = (WORD *)(cMsg + DEF_INDEX2_MSGTYPE);
        *wp = DEF_MSGTYPE_REJECT;

        iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 8);
    }
    else
    {
        iItemWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex], m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);

        if ((iItemWeight + m_pClientList[iClientH]->m_iCurWeightLoad) > _iCalcMaxLoad(iClientH))
        {
            ZeroMemory(cMsg, sizeof(cMsg));

            dwp = (DWORD *)(cMsg + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cMsg + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

            iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 6);
            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:
                    DeleteClient(iClientH, TRUE, TRUE);
                    break;
            }
            return;
        }

        if ((m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
            (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW))
        {
            for (i = 0; i < DEF_MAXITEMS; i++)
                if ((m_pClientList[iClientH]->m_pItemList[i] != NULL) &&
                    (m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType) &&
                    (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cName, 20) == 0))
                {
                    SetItemCount(iClientH, i, m_pClientList[iClientH]->m_pItemList[i]->m_dwCount + m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);

                    delete m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
                    m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = NULL;

                    for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++)
                    {
                        if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != NULL) && (m_pClientList[iClientH]->m_pItemInBankList[j] == NULL))
                        {
                            m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

                            m_pClientList[iClientH]->m_pItemInBankList[j + 1] = NULL;
                        }
                    }

                    ZeroMemory(cMsg, sizeof(cMsg));

                    dwp = (DWORD *)(cMsg + DEF_INDEX4_MSGID);
                    *dwp = MSGID_RESPONSE_RETRIEVEITEM;
                    wp = (WORD *)(cMsg + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_MSGTYPE_CONFIRM;

                    cp = (char *)(cMsg + DEF_INDEX2_MSGTYPE + 2);
                    *cp = cBankItemIndex;
                    cp++;
                    *cp = i;
                    cp++;

                    iCalcTotalWeight(iClientH);
                    m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

                    iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 8);
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

            goto RRIH_NOQUANTITY;
        }
        else
        {
            RRIH_NOQUANTITY:;
            for (i = 0; i < DEF_MAXITEMS; i++)
                if (m_pClientList[iClientH]->m_pItemList[i] == NULL)
                {
                    m_pClientList[iClientH]->m_pItemList[i] = m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
                    m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
                    m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

                    m_pClientList[iClientH]->m_bIsItemEquipped[i] = FALSE;

                    m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = NULL;

                    for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++)
                    {
                        if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != NULL) && (m_pClientList[iClientH]->m_pItemInBankList[j] == NULL))
                        {
                            m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

                            m_pClientList[iClientH]->m_pItemInBankList[j + 1] = NULL;
                        }
                    }

                    ZeroMemory(cMsg, sizeof(cMsg));

                    dwp = (DWORD *)(cMsg + DEF_INDEX4_MSGID);
                    *dwp = MSGID_RESPONSE_RETRIEVEITEM;
                    wp = (WORD *)(cMsg + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_MSGTYPE_CONFIRM;

                    cp = (char *)(cMsg + DEF_INDEX2_MSGTYPE + 2);
                    *cp = cBankItemIndex;
                    cp++;
                    *cp = i;
                    cp++;

                    iCalcTotalWeight(iClientH);

                    m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

                    iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 8);
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
            ZeroMemory(cMsg, sizeof(cMsg));

            dwp = (DWORD *)(cMsg + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_RETRIEVEITEM;
            wp = (WORD *)(cMsg + DEF_INDEX2_MSGTYPE);
            *wp = DEF_MSGTYPE_REJECT;

            iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 8);
        }
    }

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

void CGame::RequestFullObjectData(int iClientH, char * pData)
{
    DWORD * dwp;
    WORD * wp, wObjectID;
    char * cp, cData[100];
    short * sp, sX, sY;
    int     sTemp, sTemp2;
    int * ip, iRet;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    wp = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
    wObjectID = *wp;

    ZeroMemory(cData, sizeof(cData));
    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_EVENT_MOTION;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTSTOP;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    if (wObjectID < 10000)
    {
        if ((wObjectID == 0) || (wObjectID >= DEF_MAXCLIENTS)) return;
        if (m_pClientList[wObjectID] == NULL) return;

        wp = (WORD *)cp;
        *wp = wObjectID;
        cp += 2;
        sp = (short *)cp;
        sX = m_pClientList[wObjectID]->m_sX;
        *sp = sX;
        cp += 2;
        sp = (short *)cp;
        sY = m_pClientList[wObjectID]->m_sY;
        *sp = sY;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[wObjectID]->m_sType;
        cp += 2;
        *cp = m_pClientList[wObjectID]->m_cDir;
        cp++;
        memcpy(cp, m_pClientList[wObjectID]->m_cCharName, 10);
        cp += 10;
        sp = (short *)cp;
        *sp = m_pClientList[wObjectID]->m_sAppr1;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[wObjectID]->m_sAppr2;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[wObjectID]->m_sAppr3;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[wObjectID]->m_sAppr4;
        cp += 2;
        ip = (int *)cp;
        *ip = m_pClientList[wObjectID]->m_iApprColor;
        cp += 4;

        ip = (int *)cp;

        sTemp = m_pClientList[wObjectID]->m_iStatus;
        sTemp = 0x0FFFFFFF & sTemp;
        sTemp2 = iGetPlayerABSStatus(wObjectID, iClientH);
        sTemp = (sTemp | (sTemp2 << 28));

        *ip = sTemp;
        cp += 4;

        if (m_pClientList[wObjectID]->m_bIsKilled == TRUE)
            *cp = 1;
        else *cp = 0;
        cp++;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 41);
    }
    else
    {
        if (((wObjectID - 10000) == 0) || ((wObjectID - 10000) >= DEF_MAXNPCS)) return;
        if (m_pNpcList[wObjectID - 10000] == NULL) return;

        wp = (WORD *)cp;
        *wp = wObjectID;
        cp += 2;

        wObjectID -= 10000;

        sp = (short *)cp;
        sX = m_pNpcList[wObjectID]->m_sX;
        *sp = sX;
        cp += 2;
        sp = (short *)cp;
        sY = m_pNpcList[wObjectID]->m_sY;
        *sp = sY;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pNpcList[wObjectID]->m_sType;
        cp += 2;
        *cp = m_pNpcList[wObjectID]->m_cDir;
        cp++;
        memcpy(cp, m_pNpcList[wObjectID]->m_cName, 5);
        cp += 5;
        sp = (short *)cp;
        *sp = m_pNpcList[wObjectID]->m_sAppr2;
        cp += 2;

        ip = (int *)cp;

        sTemp = m_pNpcList[wObjectID]->m_iStatus;
        sTemp = 0x0FFFFFFF & sTemp;

        sTemp2 = iGetNpcRelationship(wObjectID, iClientH);
        sTemp = (sTemp | (sTemp2 << 28));
        *ip = sTemp;
        cp += 4;

        if (m_pNpcList[wObjectID]->m_bIsKilled == TRUE)
            *cp = 1;
        else *cp = 0;
        cp++;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 27);
    }

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

void CGame::CheckAndNotifyPlayerConnection(int iClientH, char * pMsg, DWORD dwSize)
{
    char   seps[] = "= \t\n";
    char * token, * cp, cName[11], cBuff[256], cPlayerLocation[120];
    CStrTok * pStrTok;
    int i;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (dwSize <= 0) return;

    ZeroMemory(cPlayerLocation, sizeof(cPlayerLocation));
    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    if (strlen(token) > 10)
        memcpy(cName, token, 10);
    else memcpy(cName, token, strlen(token));

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(cName, m_pClientList[i]->m_cCharName, 10) == 0))
        {
            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
            {
                cp = (char *)cPlayerLocation;

                memcpy(cp, m_pClientList[i]->m_cMapName, 10);
                cp += 10;

                wp = (WORD *)cp;
                *wp = m_pClientList[i]->m_sX;
                cp += 2;

                wp = (WORD *)cp;
                *wp = m_pClientList[i]->m_sY;
                cp += 2;
            }
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERONGAME, NULL, NULL, NULL, m_pClientList[i]->m_cCharName, NULL, NULL, NULL, NULL, NULL, NULL, cPlayerLocation);

            delete pStrTok;
            return;
        }

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

    memcpy(cp, cName, 10);
    cp += 10;

    memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
    cp += 10;

    bStockMsgToGateServer(cBuff, 25);

    delete pStrTok;
}

void CGame::ToggleWhisperPlayer(int iClientH, char * pMsg, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;
    char * cp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (dwMsgSize <= 0) return;

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == NULL)
    {
        m_pClientList[iClientH]->m_iWhisperPlayerIndex = -1;
        ZeroMemory(m_pClientList[iClientH]->m_cWhisperPlayerName, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
        m_pClientList[iClientH]->m_bIsCheckingWhisperPlayer = FALSE;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_WHISPERMODEOFF, NULL, NULL, NULL, cName);
    }
    else
    {
        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));

        m_pClientList[iClientH]->m_iWhisperPlayerIndex = -1;

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {
                if (i == iClientH)
                {
                    delete pStrTok;
                    return;
                }
                m_pClientList[iClientH]->m_iWhisperPlayerIndex = i;
                ZeroMemory(m_pClientList[iClientH]->m_cWhisperPlayerName, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
                strcpy(m_pClientList[iClientH]->m_cWhisperPlayerName, cName);
                break;
            }

        if (m_pClientList[iClientH]->m_iWhisperPlayerIndex == -1)
        {
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

            memcpy(cp, cName, 10);
            cp += 10;

            memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
            cp += 10;

            bStockMsgToGateServer(cBuff, 25);

            ZeroMemory(m_pClientList[iClientH]->m_cWhisperPlayerName, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
            strcpy(m_pClientList[iClientH]->m_cWhisperPlayerName, cName);
            m_pClientList[iClientH]->m_bIsCheckingWhisperPlayer = TRUE;
        }
        else
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_WHISPERMODEON, NULL, NULL, NULL, m_pClientList[iClientH]->m_cWhisperPlayerName);
        }
    }

    delete pStrTok;
}

void CGame::SetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize)
{
    char cTemp[256];
    int i;


    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize - 7) <= 0) return;

    ZeroMemory(cTemp, sizeof(cTemp));
    memcpy(cTemp, (pMsg + 7), dwMsgSize - 7);

    for (i = 0; i < 256; i++)
        if (cTemp[i] == ' ') cTemp[i] = '_';

    cTemp[255] = NULL;

    ZeroMemory(m_pClientList[iClientH]->m_cProfile, sizeof(m_pClientList[iClientH]->m_cProfile));
    strcpy(m_pClientList[iClientH]->m_cProfile, cTemp);
}

void CGame::GetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256], cBuff2[500];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {

                ZeroMemory(cBuff2, sizeof(cBuff2));
                wsprintf(cBuff2, "%s Profile: %s", cName, m_pClientList[i]->m_cProfile);
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERPROFILE, NULL, NULL, NULL, cBuff2);

                delete pStrTok;
                return;
            }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cName);
    }

    delete pStrTok;
    return;
}

void CGame::RequestAdminUserMode(int iClientH, char * pData)
{

}

void CGame::ShutUpPlayer(int iClientH, char * pMsg, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256];
    CStrTok * pStrTok;
    int i, iTime;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelShutup)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));

        token = pStrTok->pGet();
        if (token == NULL)
            iTime = 0;
        else iTime = atoi(token);

        if (iTime < 0) iTime = 0;

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {

                m_pClientList[i]->m_iTimeLeft_ShutUp = iTime * 20;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERSHUTUP, iTime, NULL, NULL, cName);
                SendNotifyMsg(NULL, i, DEF_NOTIFY_PLAYERSHUTUP, iTime, NULL, NULL, cName);

                wsprintf(G_cTxt, "GM Order(%s): Shutup PC(%s) (%d)Min", m_pClientList[iClientH]->m_cCharName,
                    m_pClientList[i]->m_cCharName, iTime);
                log->info(G_cTxt);

                delete pStrTok;
                return;
            }
        ZeroMemory(cBuff, sizeof(cBuff));

        char * cp;
        WORD * wp;

        cp = (char *)cBuff;

        *cp = GSM_REQUEST_SHUTUPPLAYER;
        cp++;

        wp = (WORD *)cp;
        *wp = m_wServerID_GSS;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)iClientH;
        cp += 2;

        memcpy(cp, cName, 10);
        cp += 10;

        wp = (WORD *)cp;
        *wp = iTime;
        cp += 2;

        memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
        cp += 10;

        bStockMsgToGateServer(cBuff, 27);
    }

    delete pStrTok;
    return;
}

void CGame::SetPlayerReputation(int iClientH, char * pMsg, char cValue, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH]->m_iLevel < 40) return;

    if ((m_pClientList[iClientH]->m_iTimeLeft_Rating != 0) || (m_pClientList[iClientH]->m_iPKCount != 0))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTRATING, m_pClientList[iClientH]->m_iTimeLeft_Rating, NULL, NULL, NULL);
        return;
    }
    else if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTRATING, 0, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {

                if (i != iClientH)
                {
                    if (cValue == 0)
                        m_pClientList[i]->m_iRating--;
                    else if (cValue == 1)
                        m_pClientList[i]->m_iRating++;

                    if (m_pClientList[i]->m_iRating > 500)  m_pClientList[i]->m_iRating = 500;
                    if (m_pClientList[i]->m_iRating < -500) m_pClientList[i]->m_iRating = -500;
                    m_pClientList[iClientH]->m_iTimeLeft_Rating = 20 * 60;

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_RATINGPLAYER, cValue, NULL, NULL, cName);
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RATINGPLAYER, cValue, NULL, NULL, cName);

                    delete pStrTok;
                    return;
                }
            }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cName);
    }

    delete pStrTok;
    return;
}

void CGame::ReqCreatePortionHandler(int iClientH, char * pData)
{
    DWORD * dwp;
    WORD * wp;
    char * cp, cI[6]{}, cPortionName[30], cData[120];
    int    iRet, i, j, iEraseReq, iSkillLimit, iSkillLevel, iResult, iDifficulty;
    short * sp, sItemIndex[6]{}, sTemp;
    short  sItemNumber[6]{}, sItemArray[12]{};
    BOOL   bDup, bFlag;
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return;
    m_pClientList[iClientH]->m_iSkillMsgRecvCount++;

    for (i = 0; i < 6; i++)
    {
        cI[i] = -1;
        sItemIndex[i] = -1;
        sItemNumber[i] = 0;
    }

    cp = (char *)(pData + 11);
    cI[0] = *cp;
    cp++;
    cI[1] = *cp;
    cp++;
    cI[2] = *cp;
    cp++;
    cI[3] = *cp;
    cp++;
    cI[4] = *cp;
    cp++;
    cI[5] = *cp;
    cp++;

    for (i = 0; i < 6; i++)
    {
        if (cI[i] >= DEF_MAXITEMS) return;
        if ((cI[i] >= 0) && (m_pClientList[iClientH]->m_pItemList[cI[i]] == NULL)) return;
    }

    for (i = 0; i < 6; i++)
        if (cI[i] >= 0)
        {
            bDup = FALSE;
            for (j = 0; j < 6; j++)
                if (sItemIndex[j] == cI[i])
                {
                    sItemNumber[j]++;
                    bDup = TRUE;
                }
            if (bDup == FALSE)
            {
                for (j = 0; j < 6; j++)
                    if (sItemIndex[j] == -1)
                    {
                        sItemIndex[j] = cI[i];
                        sItemNumber[j]++;
                        goto RCPH_LOOPBREAK;
                    }
                RCPH_LOOPBREAK:;
            }
        }

    for (i = 0; i < 6; i++)
        if (sItemIndex[i] != -1)
        {
            if (sItemIndex[i] < 0) return;
            if ((sItemIndex[i] >= 0) && (sItemIndex[i] >= DEF_MAXITEMS)) return;
            if (m_pClientList[iClientH]->m_pItemList[sItemIndex[i]] == NULL) return;
            if (m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_dwCount < sItemNumber[i]) return;
        }

    bFlag = TRUE;
    while (bFlag == TRUE)
    {
        bFlag = FALSE;
        for (i = 0; i < 5; i++)
            if ((sItemIndex[i] != -1) && (sItemIndex[i + 1] != -1))
            {
                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_sIDnum) <
                    (m_pClientList[iClientH]->m_pItemList[sItemIndex[i + 1]]->m_sIDnum))
                {
                    sTemp = sItemIndex[i + 1];
                    sItemIndex[i + 1] = sItemIndex[i];
                    sItemIndex[i] = sTemp;
                    sTemp = sItemNumber[i + 1];
                    sItemNumber[i + 1] = sItemNumber[i];
                    sItemNumber[i] = sTemp;
                    bFlag = TRUE;
                }
            }
    }

    j = 0;
    for (i = 0; i < 6; i++)
    {
        if (sItemIndex[i] != -1)
            sItemArray[j] = m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_sIDnum;
        else sItemArray[j] = sItemIndex[i];
        sItemArray[j + 1] = sItemNumber[i];
        j += 2;
    }

    ZeroMemory(cPortionName, sizeof(cPortionName));

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        if (m_pPortionConfigList[i] != NULL)
        {
            bFlag = FALSE;
            for (j = 0; j < 12; j++)
                if (m_pPortionConfigList[i]->m_sArray[j] != sItemArray[j]) bFlag = TRUE;

            if (bFlag == FALSE)
            {
                ZeroMemory(cPortionName, sizeof(cPortionName));
                memcpy(cPortionName, m_pPortionConfigList[i]->m_cName, 20);
                iSkillLimit = m_pPortionConfigList[i]->m_iSkillLimit;
                iDifficulty = m_pPortionConfigList[i]->m_iDifficulty;
            }
        }

    if (strlen(cPortionName) == 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOMATCHINGPORTION, NULL, NULL, NULL, NULL);
        return;
    }

    iSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[12];
    if (iSkillLimit > iSkillLevel)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LOWPORTIONSKILL, NULL, NULL, NULL, cPortionName);
        return;
    }

    iSkillLevel -= iDifficulty;
    if (iSkillLevel <= 0) iSkillLevel = 1;

    iResult = iDice(1, 100);
    if (iResult > iSkillLevel)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PORTIONFAIL, NULL, NULL, NULL, cPortionName);
        return;
    }

    CalculateSSN_SkillIndex(iClientH, 12, 1);

    if (strlen(cPortionName) != 0)
    {
        pItem = NULL;
        pItem = new CItem;
        if (pItem == NULL) return;

        for (i = 0; i < 6; i++)
            if (sItemIndex[i] != -1)
            {
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME)
                    SetItemCount(iClientH, sItemIndex[i], m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_dwCount - sItemNumber[i]);
                else ItemDepleteHandler(iClientH, sItemIndex[i], FALSE);
            }

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PORTIONSUCCESS, NULL, NULL, NULL, cPortionName);
        m_pClientList[iClientH]->m_iExpStock += iDice(1, (iDifficulty / 3));

        if ((_bInitItemAttr(pItem, cPortionName) == TRUE))
        {
            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
            {
                ZeroMemory(cData, sizeof(cData));
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

                if (iEraseReq == 1) delete pItem;

                iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);
                switch (iRet)
                {
                    case DEF_XSOCKEVENT_QUENEFULL:
                    case DEF_XSOCKEVENT_SOCKETERROR:
                    case DEF_XSOCKEVENT_CRITICALERROR:
                    case DEF_XSOCKEVENT_SOCKETCLOSED:
                        DeleteClient(iClientH, TRUE, TRUE);
                        break;
                }

                //if ((pItem->m_wPrice * pItem->m_dwCount) > 1000) 
                //	SendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA, iClientH);
            }
            else
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                    m_pClientList[iClientH]->m_sY, pItem);

                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                    m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                    pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);

                dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

                iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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
        }
        else
        {
            delete pItem;
            pItem = NULL;
        }
    }
}

void CGame::_SetItemPos(int iClientH, char * pData)
{
    char * cp, cItemIndex;
    short * sp, sX, sY;

    if (m_pClientList[iClientH] == NULL) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    cItemIndex = *cp;
    cp++;

    sp = (short *)cp;
    sX = *sp;
    cp += 2;

    sp = (short *)cp;
    sY = *sp;
    cp += 2;

    if (sY < -10) sY = -10;

    if ((cItemIndex < 0) || (cItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemIndex] != NULL)
    {
        m_pClientList[iClientH]->m_ItemPosList[cItemIndex].x = sX;
        m_pClientList[iClientH]->m_ItemPosList[cItemIndex].y = sY;
    }
}

void CGame::BuildItemHandler(int iClientH, char * pData)
{
    char * cp, cName[30], cElementItemID[6];
    int    i, x, z, iMatch, iCount, iPlayerSkillLevel, iResult, iTotalValue, iResultValue, iTemp, iItemCount[DEF_MAXITEMS]{};
    CItem * pItem;
    BOOL   bFlag, bItemFlag[6]{};
    double dV1, dV2, dV3;
    DWORD  dwTemp, dwTemp2;
    WORD   wTemp;

    if (m_pClientList[iClientH] == NULL) return;
    m_pClientList[iClientH]->m_iSkillMsgRecvCount++;

    cp = (char *)(pData + 11);
    ZeroMemory(cName, sizeof(cName));
    memcpy(cName, cp, 20);
    cp += 20;

    ZeroMemory(cElementItemID, sizeof(cElementItemID));
    cElementItemID[0] = *cp;
    cp++;
    cElementItemID[1] = *cp;
    cp++;
    cElementItemID[2] = *cp;
    cp++;
    cElementItemID[3] = *cp;
    cp++;
    cElementItemID[4] = *cp;
    cp++;
    cElementItemID[5] = *cp;
    cp++;

    bFlag = TRUE;
    while (bFlag == TRUE)
    {
        bFlag = FALSE;
        for (i = 0; i <= 4; i++)
            if ((cElementItemID[i] == -1) && (cElementItemID[i + 1] != -1))
            {
                cElementItemID[i] = cElementItemID[i + 1];
                cElementItemID[i + 1] = -1;
                bFlag = TRUE;
            }
    }

    for (i = 0; i < 6; i++) bItemFlag[i] = FALSE;

    iPlayerSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[13];
    iResult = iDice(1, 100);

    if (iResult > iPlayerSkillLevel)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMFAIL, NULL, NULL, NULL, NULL);
        return;
    }

    for (i = 0; i < 6; i++)
        if (cElementItemID[i] != -1)
        {
            if ((cElementItemID[i] < 0) || (cElementItemID[i] > DEF_MAXITEMS)) return;
            if (m_pClientList[iClientH]->m_pItemList[cElementItemID[i]] == NULL) return;
        }

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        if (m_pBuildItemList[i] != NULL)
        {
            if (memcmp(m_pBuildItemList[i]->m_cName, cName, 20) == 0)
            {
                if (m_pBuildItemList[i]->m_iSkillLimit > m_pClientList[iClientH]->m_cSkillMastery[13]) return;

                for (x = 0; x < DEF_MAXITEMS; x++)
                    if (m_pClientList[iClientH]->m_pItemList[x] != NULL)
                        iItemCount[x] = m_pClientList[iClientH]->m_pItemList[x]->m_dwCount;
                    else iItemCount[x] = 0;

                iMatch = 0;
                iTotalValue = 0;

                for (x = 0; x < 6; x++)
                {
                    if (m_pBuildItemList[i]->m_iMaterialItemCount[x] == 0)
                    {
                        iMatch++;
                    }
                    else
                    {
                        for (z = 0; z < 6; z++)
                            if ((cElementItemID[z] != -1) && (bItemFlag[z] == FALSE))
                            {

                                if ((m_pClientList[iClientH]->m_pItemList[cElementItemID[z]]->m_sIDnum == m_pBuildItemList[i]->m_iMaterialItemID[x]) &&
                                    (m_pClientList[iClientH]->m_pItemList[cElementItemID[z]]->m_dwCount >= m_pBuildItemList[i]->m_iMaterialItemCount[x]) &&
                                    (iItemCount[cElementItemID[z]] > 0))
                                {
                                    iTemp = m_pClientList[iClientH]->m_pItemList[cElementItemID[z]]->m_sItemSpecEffectValue2;
                                    if (iTemp > m_pClientList[iClientH]->m_cSkillMastery[13])
                                    {
                                        iTemp = iTemp - (iTemp - m_pClientList[iClientH]->m_cSkillMastery[13]) / 2;
                                    }

                                    iTotalValue += (iTemp * m_pBuildItemList[i]->m_iMaterialItemValue[x]);
                                    iItemCount[cElementItemID[z]] -= m_pBuildItemList[i]->m_iMaterialItemCount[x];
                                    iMatch++;
                                    bItemFlag[z] = TRUE;

                                    goto BIH_LOOPBREAK;
                                }
                            }
                        BIH_LOOPBREAK:;
                    }
                }

                if (iMatch != 6)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMFAIL, NULL, NULL, NULL, NULL);
                    return;
                }

                dV2 = (double)m_pBuildItemList[i]->m_iMaxValue;
                if (iTotalValue <= 0)
                    dV3 = 1.0f;
                else dV3 = (double)iTotalValue;
                dV1 = (double)(dV3 / dV2) * 100.0f;

                iTotalValue = (int)dV1;

                pItem = new CItem;
                if (_bInitItemAttr(pItem, m_pBuildItemList[i]->m_cName) == FALSE)
                {
                    delete pItem;
                    return;
                }

                dwTemp = pItem->m_dwAttribute;
                dwTemp = dwTemp & 0xFFFFFFFE;
                dwTemp = dwTemp | 0x00000001;
                pItem->m_dwAttribute = dwTemp;

                if (pItem->m_cItemType == DEF_ITEMTYPE_MATERIAL)
                {
                    iTemp = iDice(1, (iPlayerSkillLevel / 2) + 1) - 1;
                    pItem->m_sItemSpecEffectValue2 = (iPlayerSkillLevel / 2) + iTemp;
                    pItem->m_sTouchEffectType = DEF_ITET_ID;
                    pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                    pItem->m_sTouchEffectValue2 = iDice(1, 100000);
                    pItem->m_sTouchEffectValue3 = timeGetTime();

                }
                else
                {
                    dwTemp = pItem->m_dwAttribute;
                    dwTemp = dwTemp & 0x0000FFFF;

                    dwTemp2 = (WORD)m_pBuildItemList[i]->m_wAttribute;
                    dwTemp2 = dwTemp2 << 16;

                    dwTemp = dwTemp | dwTemp2;
                    pItem->m_dwAttribute = dwTemp;

                    iResultValue = (iTotalValue - m_pBuildItemList[i]->m_iAverageValue);
                    if (iResultValue > 0)
                    {
                        dV2 = (double)iResultValue;
                        dV3 = (double)(100 - m_pBuildItemList[i]->m_iAverageValue);
                        dV1 = (dV2 / dV3) * 100.0f;
                        pItem->m_sItemSpecEffectValue2 = (int)dV1;
                    }
                    else if (iResultValue < 0)
                    {
                        dV2 = (double)(iResultValue);
                        dV3 = (double)(m_pBuildItemList[i]->m_iAverageValue);
                        dV1 = (dV2 / dV3) * 100.0f;
                        pItem->m_sItemSpecEffectValue2 = (int)dV1;
                    }
                    else pItem->m_sItemSpecEffectValue2 = 0;

                    dV2 = (double)pItem->m_sItemSpecEffectValue2;
                    dV3 = (double)pItem->m_wMaxLifeSpan;
                    dV1 = (dV2 / 100.0f) * dV3;

                    iTemp = (int)pItem->m_wMaxLifeSpan;
                    iTemp += (int)dV1;

                    pItem->m_sTouchEffectType = DEF_ITET_ID;
                    pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                    pItem->m_sTouchEffectValue2 = iDice(1, 100000);
                    pItem->m_sTouchEffectValue3 = timeGetTime();

                    if (iTemp <= 0)
                        wTemp = 1;
                    else wTemp = (WORD)iTemp;

                    if (wTemp <= pItem->m_wMaxLifeSpan * 2)
                    {
                        pItem->m_wMaxLifeSpan = wTemp;
                        pItem->m_sItemSpecEffectValue1 = (short)wTemp;
                        pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
                    }
                    else pItem->m_sItemSpecEffectValue1 = (short)pItem->m_wMaxLifeSpan;

                    pItem->m_cItemColor = 2;
                }

                wsprintf(G_cTxt, "Custom-Item(%s) Value(%d) Life(%d/%d)", pItem->m_cName, pItem->m_sItemSpecEffectValue2, pItem->m_wCurLifeSpan, pItem->m_wMaxLifeSpan);
                log->info(G_cTxt);

                bAddItem(iClientH, pItem, NULL);
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMSUCCESS, pItem->m_sItemSpecEffectValue2, pItem->m_cItemType, NULL, NULL); // Integer¸¦ Àü´ÞÇÏ±â À§ÇØ 

                for (x = 0; x < 6; x++)
                    if (cElementItemID[x] != -1)
                    {
                        if (m_pClientList[iClientH]->m_pItemList[cElementItemID[x]] == NULL)
                        {
                            wsprintf(G_cTxt, "(?) Char(%s) ElementItemID(%d)", m_pClientList[iClientH]->m_cCharName, cElementItemID[x]);
                            log->info(G_cTxt);
                        }
                        else
                        {
                            iCount = m_pClientList[iClientH]->m_pItemList[cElementItemID[x]]->m_dwCount - m_pBuildItemList[i]->m_iMaterialItemCount[x];
                            if (iCount < 0) iCount = 0;
                            SetItemCount(iClientH, cElementItemID[x], iCount);
                        }
                    }

                if (m_pBuildItemList[i]->m_iMaxSkill > m_pClientList[iClientH]->m_cSkillMastery[13])
                    CalculateSSN_SkillIndex(iClientH, 13, 1);

                GetExp(iClientH, iDice(1, (m_pBuildItemList[i]->m_iSkillLimit / 4)));
                return;
            }
        }

}

void CGame::RequestNoticementHandler(int iClientH, char * pData)
{
    char * cp, cData[120];
    int * ip, iRet, iClientSize;
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_dwNoticementDataSize < 10) return;

    ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
    iClientSize = *ip;

    if (iClientSize != m_dwNoticementDataSize)
    {
        cp = new char[m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2];
        ZeroMemory(cp, m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2);
        memcpy((cp + DEF_INDEX2_MSGTYPE + 2), m_pNoticementData, m_dwNoticementDataSize);

        dwp = (DWORD *)(cp + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_NOTICEMENT;
        wp = (WORD *)(cp + DEF_INDEX2_MSGTYPE);
        *wp = DEF_MSGTYPE_REJECT;

        iRet = m_pClientList[iClientH]->iSendMsg(cp, m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2);

        delete cp;
    }
    else
    {
        ZeroMemory(cData, sizeof(cData));

        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_NOTICEMENT;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_MSGTYPE_CONFIRM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
    }
}

void CGame::RequestCheckAccountPasswordHandler(char * pData, DWORD dwMsgSize)
{
    int * ip, i, iLevel;
    char * cp, cAccountName[11], cAccountPassword[11];

    cp = (char *)(pData + 6);

    ZeroMemory(cAccountName, sizeof(cAccountName));
    ZeroMemory(cAccountPassword, sizeof(cAccountPassword));

    memcpy(cAccountName, cp, 10);
    cp += 10;

    memcpy(cAccountPassword, cp, 10);
    cp += 10;

    ip = (int *)cp;
    iLevel = *ip;
    cp += 4;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cAccountName, cAccountName) == 0))
        {
            if ((strcmp(m_pClientList[i]->m_cAccountPassword, cAccountPassword) != 0) || (m_pClientList[i]->m_iLevel != iLevel))
            {
                wsprintf(G_cTxt, "(TestLog) Error! Account(%s)-Level(%d) password(or level) mismatch! Disconnect.", cAccountName, iLevel);
                log->info(G_cTxt);
                DeleteClient(i, FALSE, TRUE);
                return;
            }
        }
}

int CGame::iRequestPanningMapDataRequest(int iClientH, char * pData)
{
    char * cp, cDir, cData[3000]{};
    DWORD * dwp;
    WORD * wp;
    short * sp, dX, dY;
    int   iRet, iSize;

    if (m_pClientList[iClientH] == NULL) return 0;
    if (m_pClientList[iClientH]->m_bIsObserverMode == FALSE) return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

    dX = m_pClientList[iClientH]->m_sX;
    dY = m_pClientList[iClientH]->m_sY;

    cDir = *(pData + DEF_INDEX2_MSGTYPE + 2);
    if ((cDir <= 0) || (cDir > 8)) return 0;

    switch (cDir)
    {
        case 1:	dY--; break;
        case 2:	dX++; dY--;	break;
        case 3:	dX++; break;
        case 4:	dX++; dY++;	break;
        case 5: dY++; break;
        case 6:	dX--; dY++;	break;
        case 7:	dX--; break;
        case 8:	dX--; dY--;	break;
    }

    m_pClientList[iClientH]->m_sX = dX;
    m_pClientList[iClientH]->m_sY = dY;
    m_pClientList[iClientH]->m_cDir = cDir;

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_PANNING;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOVE_CONFIRM;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    sp = (short *)cp;
    *sp = (short)(dX);
    cp += 2;

    sp = (short *)cp;
    *sp = (short)(dY);
    cp += 2;

    *cp = cDir;
    cp++;

    iSize = 0;//iComposeMoveMapData((short)(dX - 10), (short)(dY - 7), iClientH, cDir, cp);

    iRet = m_pClientList[iClientH]->iSendMsg(cData, iSize + 12 + 1 + 4);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            DeleteClient(iClientH, TRUE, TRUE);
            return 0;
    }

    return 1;
}

void CGame::GetMagicAbilityHandler(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_cSkillMastery[4] != 0) return;

    m_pClientList[iClientH]->m_cSkillMastery[4] = 20;
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 4, m_pClientList[iClientH]->m_cSkillMastery[4], NULL, NULL);
    bCheckTotalSkillMasteryPoints(iClientH, 4);
}

void CGame::RequestRestartHandler(int iClientH)
{
    char  cTmpMap[32];

    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_bIsKilled == TRUE)
    {

        strcpy(cTmpMap, m_pClientList[iClientH]->m_cMapName);
        ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));

        if (strcmp(m_pClientList[iClientH]->m_cLocation, "NONE") == 0)
        {
            strcpy(m_pClientList[iClientH]->m_cMapName, "default");
        }
        else
        {
            if ((strcmp(m_pClientList[iClientH]->m_cLocation, "aresden") == 0) || (strcmp(m_pClientList[iClientH]->m_cLocation, "arehunter") == 0))
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
                        memcpy(m_pClientList[iClientH]->m_cMapName, "resurr1", 7);
                        m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10;
                    }
                }
                if (strcmp(cTmpMap, "elvine") == 0)
                {
                    memcpy(m_pClientList[iClientH]->m_cMapName, "elvjail", 7);
                    strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvjail");
                    m_pClientList[iClientH]->m_iLockedMapTime = 60 * 3;
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
                        memcpy(m_pClientList[iClientH]->m_cMapName, "resurr2", 7);
                        m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10;
                    }
                }
                if (strcmp(cTmpMap, "aresden") == 0)
                {
                    memcpy(m_pClientList[iClientH]->m_cMapName, "arejail", 7);
                    strcpy(m_pClientList[iClientH]->m_cLockedMapName, "arejail");
                    m_pClientList[iClientH]->m_iLockedMapTime = 60 * 3;

                }
                else if (m_pClientList[iClientH]->m_iLevel > 80)
                    memcpy(m_pClientList[iClientH]->m_cMapName, "resurr2", 7);
                else memcpy(m_pClientList[iClientH]->m_cMapName, "elvfarm", 7);
            }
        }

        m_pClientList[iClientH]->m_bIsKilled = FALSE;
        m_pClientList[iClientH]->m_iHP = (3 * m_pClientList[iClientH]->m_iVit) + (2 * m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iStr / 2);
        m_pClientList[iClientH]->m_iHungerStatus = 100;

        ZeroMemory(cTmpMap, sizeof(cTmpMap));
        strcpy(cTmpMap, m_pClientList[iClientH]->m_cMapName);
        RequestTeleportHandler(iClientH, "2   ", cTmpMap, -1, -1);
    }
}

void CGame::RequestSellItemListHandler(int iClientH, char * pData)
{
    int i, * ip, iAmount;
    char * cp, cIndex;
    struct
    {
        char cIndex;
        int  iAmount;
    } stTemp[12]{};

    if (m_pClientList[iClientH] == NULL) return;

    cp = (char *)(pData + 6);
    for (i = 0; i < 12; i++)
    {
        stTemp[i].cIndex = *cp;
        cp++;

        ip = (int *)cp;
        stTemp[i].iAmount = *ip;
        cp += 4;
    }

    for (i = 0; i < 12; i++)
    {
        cIndex = stTemp[i].cIndex;
        iAmount = stTemp[i].iAmount;

        if ((cIndex == -1) || (cIndex < 0) || (cIndex >= DEF_MAXITEMS)) return;
        if (m_pClientList[iClientH]->m_pItemList[cIndex] == NULL) return;

        ReqSellItemConfirmHandler(iClientH, cIndex, iAmount, NULL);
        if (m_pClientList[iClientH] == NULL) return;
    }
}

void CGame::CreateNewPartyHandler(int iClientH)
{
    BOOL bFlag;

    if (m_pClientList[iClientH] == NULL) return;

    bFlag = m_pClientList[iClientH]->bCreateNewParty();
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RESPONSE_CREATENEWPARTY, (int)bFlag, NULL, NULL, NULL);
}

void CGame::JoinPartyHandler(int iClientH, int iV1, char * pMemberName)
{
    char * cp, cData[120];
    short sAppr2;
    DWORD * dwp;
    WORD * wp;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;

    switch (iV1)
    {
        case 0:
            RequestDeletePartyHandler(iClientH);
            break;

        case 1:
            wsprintf(G_cTxt, "Join Party Req: %s(%d) ID(%d) Stat(%d) ReqJoinH(%d) ReqJoinName(%s)", m_pClientList[iClientH]->m_cCharName, iClientH,
                m_pClientList[iClientH]->m_iPartyID, m_pClientList[iClientH]->m_iPartyStatus, m_pClientList[iClientH]->m_iReqJoinPartyClientH,
                m_pClientList[iClientH]->m_cReqJoinPartyName);
            log->info(G_cTxt);

            if ((m_pClientList[iClientH]->m_iPartyID != NULL) || (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL))
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, NULL, NULL);
                m_pClientList[iClientH]->m_iReqJoinPartyClientH = NULL;
                ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
                m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                log->info("Join Party Reject (1)");
                return;
            }

            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cCharName, pMemberName) == 0))
                {
                    sAppr2 = (short)((m_pClientList[i]->m_sAppr2 & 0xF000) >> 12);
                    if (sAppr2 != 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, NULL, NULL);
                        log->info("Join Party Reject (2)");
                    }
                    else if (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, NULL, NULL);
                        log->info("Join Party Reject (3)");
                    }
                    else if (m_pClientList[i]->m_iPartyStatus == DEF_PARTYSTATUS_PROCESSING)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, NULL, NULL);
                        log->info("Join Party Reject (4)");
                        wsprintf(G_cTxt, "Party join reject(2) ClientH:%d ID:%d JoinName:%d", i, m_pClientList[i]->m_iPartyID, m_pClientList[i]->m_cReqJoinPartyName);
                        log->info(G_cTxt);

                        m_pClientList[iClientH]->m_iReqJoinPartyClientH = NULL;
                        ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
                        m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                    }
                    else
                    {
                        m_pClientList[i]->m_iReqJoinPartyClientH = iClientH;
                        ZeroMemory(m_pClientList[i]->m_cReqJoinPartyName, sizeof(m_pClientList[i]->m_cReqJoinPartyName));
                        strcpy(m_pClientList[i]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName);
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_QUERY_JOINPARTY, NULL, NULL, NULL, m_pClientList[iClientH]->m_cCharName);

                        m_pClientList[iClientH]->m_iReqJoinPartyClientH = i;
                        ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
                        strcpy(m_pClientList[iClientH]->m_cReqJoinPartyName, m_pClientList[i]->m_cCharName);
                        m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
                    }
                    return;
                }
            break;

        case 2:
            if (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM)
            {
                ZeroMemory(cData, sizeof(cData));
                cp = (char *)cData;
                dwp = (DWORD *)cp;
                *dwp = MSGID_PARTYOPERATION;
                cp += 4;
                wp = (WORD *)cp;
                *wp = 6;
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
            break;
    }
}

void CGame::RequestSetGuildConstructLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName)
{
    char * cp, cData[120];
    int i;
    int * ip, iIndex;
    DWORD dwTemp, dwTime;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)cData;
    *cp = GSM_SETGUILDCONSTRUCTLOC;
    cp++;

    ip = (int *)cp;
    *ip = iGuildGUID;
    cp += 4;

    ip = (int *)cp;
    *ip = dX;
    cp += 4;

    ip = (int *)cp;
    *ip = dY;
    cp += 4;

    memcpy(cp, pMapName, 10);
    cp += 10;

    dwTime = timeGetTime();

    wsprintf(G_cTxt, "SetGuildConstructLoc: %d %s %d %d", iGuildGUID, pMapName, dX, dY);
    log->info(G_cTxt);

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

                bStockMsgToGateServer(cData, 23);
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
            ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName2, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName2));
            strcpy(m_pGuildTeleportLoc[i].m_cDestMapName2, pMapName);
            m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;

            bStockMsgToGateServer(cData, 23);
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

    log->info("(X) No more GuildConstructLoc Space! Replaced.");

    m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
    m_pGuildTeleportLoc[i].m_sDestX2 = dX;
    m_pGuildTeleportLoc[i].m_sDestY2 = dY;
    ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName2, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName2));
    strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
    m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;

    //bStockMsgToGateServer(cData, 23);
}

void CGame::RequestSummonWarUnitHandler(int iClientH, int dX, int dY, char cType, char cNum, char cMode)
{
    char cName[6], cNpcName[30], cMapName[11], cNpcWayPoint[11], cOwnerType;
    int i, x;
    int iNamingValue, tX, tY, ix, iy;
    BOOL bRet;
    short sOwnerH;
    DWORD dwTime = timeGetTime();

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "toh3") == 0) || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "icebound") == 0)) && (m_pClientList[iClientH]->m_iAdminUserLevel < 1))
    {
        return;
    }

    ZeroMemory(cNpcWayPoint, sizeof(cNpcWayPoint));
    ZeroMemory(cNpcName, sizeof(cNpcName));
    ZeroMemory(cMapName, sizeof(cMapName));

    if (cType < 0) return;
    if (cType >= DEF_MAXNPCTYPES) return;
    if (cNum > 10) return;

    if (m_pClientList[iClientH]->m_iConstructionPoint < m_iNpcConstructionPoint[cType]) return;
    if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != NULL) && (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == TRUE)) return;

    cNum = 1;

    for (x = 1; x <= cNum; x++)
    {
        iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue == -1)
        {
        }
        else
        {
            ZeroMemory(cName, sizeof(cName));
            wsprintf(cName, "XX%d", iNamingValue);
            cName[0] = '_';
            cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

            switch (cType)
            {
                case 43: // Light War Beetle
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "LWB-Aresden"); break;
                        case 2: strcpy(cNpcName, "LWB-Elvine"); break;
                    }
                    break;

                case 36: // Arrow Guard Tower
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "AGT-Aresden"); break;
                        case 2: strcpy(cNpcName, "AGT-Elvine"); break;
                    }
                    break;

                case 37: // Cannon Guard Tower
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "CGT-Aresden"); break;
                        case 2: strcpy(cNpcName, "CGT-Elvine"); break;
                    }
                    break;

                case 38: // Mana Collector
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "MS-Aresden"); break;
                        case 2: strcpy(cNpcName, "MS-Elvine"); break;
                    }
                    break;

                case 39: // Detector
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "DT-Aresden"); break;
                        case 2: strcpy(cNpcName, "DT-Elvine"); break;
                    }
                    break;

                case 51: // Catapult
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "CP-Aresden"); break;
                        case 2: strcpy(cNpcName, "CP-Elvine"); break;
                    }
                    break;

                case 44:
                    strcpy(cNpcName, "GHK");
                    break;

                case 45:
                    strcpy(cNpcName, "GHKABS");
                    break;

                case 46:
                    strcpy(cNpcName, "TK");
                    break;

                case 47:
                    strcpy(cNpcName, "BG");
                    break;

                case 82:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "Sor-Aresden"); break;
                        case 2: strcpy(cNpcName, "Sor-Elvine"); break;
                    }
                    break;

                case 83:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "ATK-Aresden"); break;
                        case 2: strcpy(cNpcName, "ATK-Elvine"); break;
                    }
                    break;

                case 84:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "Elf-Aresden"); break;
                        case 2: strcpy(cNpcName, "Elf-Elvine"); break;
                    }
                    break;

                case 85:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "DSK-Aresden"); break;
                        case 2: strcpy(cNpcName, "DSK-Elvine"); break;
                    }
                    break;

                case 86:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "HBT-Aresden"); break;
                        case 2: strcpy(cNpcName, "HBT-Elvine"); break;
                    }
                    break;

                case 87:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "CT-Aresden"); break;
                        case 2: strcpy(cNpcName, "CT-Elvine"); break;
                    }
                    break;

                case 88:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "Bar-Aresden"); break;
                        case 2: strcpy(cNpcName, "Bar-Elvine"); break;
                    }
                    break;

                case 89:
                    switch (m_pClientList[iClientH]->m_cSide)
                    {
                        case 1: strcpy(cNpcName, "AGC-Aresden"); break;
                        case 2: strcpy(cNpcName, "AGC-Elvine"); break;
                    }
                    break;
            }

            wsprintf(G_cTxt, "(!) Request Summon War Unit (%d) (%s)", cType, cNpcName);
            log->info(G_cTxt);

            tX = (int)dX;
            tY = (int)dY;

            bRet = FALSE;
            switch (cType)
            {
                case 36:
                case 37:
                case 38:
                case 39:
                    if (strcmp(m_pClientList[iClientH]->m_cConstructMapName, m_pClientList[iClientH]->m_cMapName) != 0) bRet = TRUE;
                    if (abs(m_pClientList[iClientH]->m_sX - m_pClientList[iClientH]->m_iConstructLocX) > 10) bRet = TRUE;
                    if (abs(m_pClientList[iClientH]->m_sY - m_pClientList[iClientH]->m_iConstructLocY) > 10) bRet = TRUE;

                    if (bRet == TRUE)
                    {
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 2, NULL, NULL, NULL);
                        return;
                    }

                    for (i = 0; i < DEF_MAXGUILDS; i++)
                        if (m_pGuildTeleportLoc[i].m_iV1 == m_pClientList[iClientH]->m_iGuildGUID)
                        {
                            m_pGuildTeleportLoc[i].m_dwTime = dwTime;
                            if (m_pGuildTeleportLoc[i].m_iV2 >= DEF_MAXCONSTRUCTNUM)
                            {
                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 3, NULL, NULL, NULL);
                                return;
                            }
                            else
                            {
                                m_pGuildTeleportLoc[i].m_iV2++;
                                goto RSWU_LOOPBREAK;
                            }
                        }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 3, NULL, NULL, NULL);
                    return;

                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 51:
                    break;

                case 40:
                case 41:
                case 42:
                case 48:
                case 49:
                case 50:
                    break;
            }

            RSWU_LOOPBREAK:;

            bRet = FALSE;
            switch (cType)
            {
                case 36:
                case 37:
                    for (ix = tX - 2; ix <= tX + 2; ix++)
                        for (iy = tY - 2; iy <= tY + 2; iy++)
                        {
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if ((sOwnerH != NULL) && (cOwnerType == DEF_OWNERTYPE_NPC))
                            {
                                switch (m_pNpcList[sOwnerH]->m_sType)
                                {
                                    case 36:
                                    case 37:
                                        bRet = TRUE;
                                        break;
                                }
                            }
                        }

                    if ((dY <= 32) || (dY >= 783)) bRet = TRUE;
                    break;
            }

            if (bRet == TRUE)
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 1, NULL, NULL, NULL);
                return;
            }

            if (cMode == NULL)
            {
                bRet = bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_FOLLOW, &tX, &tY, cNpcWayPoint, NULL, NULL, -1, FALSE, FALSE, FALSE, FALSE, m_pClientList[iClientH]->m_iGuildGUID);
                bSetNpcFollowMode(cName, m_pClientList[iClientH]->m_cCharName, DEF_OWNERTYPE_PLAYER);
            }
            else bRet = bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_GUARD, &tX, &tY, cNpcWayPoint, NULL, NULL, -1, FALSE, FALSE, FALSE, FALSE, m_pClientList[iClientH]->m_iGuildGUID);

            if (bRet == FALSE)
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
            }
            else
            {
                m_pClientList[iClientH]->m_iConstructionPoint -= m_iNpcConstructionPoint[cType];
                if (m_pClientList[iClientH]->m_iConstructionPoint < 0) m_pClientList[iClientH]->m_iConstructionPoint = 0;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, NULL, NULL);
            }
        }
    }
}

void CGame::CheckConnectionHandler(int iClientH, char * pData)
{
    char * cp;
    DWORD * dwp, dwTimeRcv, dwTime, dwTimeGapClient, dwTimeGapServer;

    if (m_pClientList[iClientH] == NULL) return;
    //m_pClientList[iClientH]->m_cConnectionCheck = 0;

    dwTime = timeGetTime();
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    dwp = (DWORD *)cp;
    dwTimeRcv = *dwp;

    if (m_pClientList[iClientH]->m_dwInitCCTimeRcv == NULL)
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
            DeleteClient(iClientH, TRUE, TRUE);
            return;
        }
    }
}

void CGame::SelectCrusadeDutyHandler(int iClientH, int iDuty)
{

    if (m_pClientList[iClientH] == NULL) return;
    if ((m_pClientList[iClientH]->m_iGuildRank != 0) && (iDuty == 3)) return;

    if (m_iLastCrusadeWinner == m_pClientList[iClientH]->m_cSide &&
        m_pClientList[iClientH]->m_dwCrusadeGUID == 0 && iDuty == 3)
    {
        m_pClientList[iClientH]->m_iConstructionPoint = 3000;
    }
    m_pClientList[iClientH]->m_iCrusadeDuty = iDuty;

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, NULL, NULL);
    if (iDuty == 1)
        _bCrusadeLog(DEF_CRUSADELOG_SELECTDUTY, iClientH, NULL, "Fighter");
    else if (iDuty == 2)
        _bCrusadeLog(DEF_CRUSADELOG_SELECTDUTY, iClientH, NULL, "Constructor");
    else
        _bCrusadeLog(DEF_CRUSADELOG_SELECTDUTY, iClientH, NULL, "Commander");
}

void CGame::ReqCreateSlateHandler(int iClientH, char * pData)
{
    int i, iRet;
    short * sp;
    char cItemID[4]{}, ctr[4]{};
    char * cp, cSlateColor, cData[120];
    BOOL bIsSlatePresent = FALSE;
    CItem * pItem;
    int iSlateType, iEraseReq;
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;

    for (i = 0; i < 4; i++)
    {
        cItemID[i] = 0;
        ctr[i] = 0;
    }

    cp = (char *)pData;
    cp += 11;

    // 14% chance of creating slates
    if (iDice(1, 100) < m_sSlateSuccessRate) bIsSlatePresent = TRUE;

    try
    {
        // make sure slates really exist
        for (i = 0; i < 4; i++)
        {
            cItemID[i] = *cp;
            cp++;

            if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] == NULL || cItemID[i] > DEF_MAXITEMS)
            {
                bIsSlatePresent = FALSE;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, NULL, NULL, NULL, NULL);
                return;
            }

            //No duping
            if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 868) // LU
                ctr[0] = 1;
            else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 869) // LD
                ctr[1] = 1;
            else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 870) // RU
                ctr[2] = 1;
            else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 871) // RD
                ctr[3] = 1;
        }
    }
    catch (...)
    {
        //Crash Hacker Caught
        bIsSlatePresent = FALSE;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, NULL, NULL, NULL, NULL);
        wsprintf(G_cTxt, "TSearch Slate Hack: (%s) Player: (%s) - creating slates without correct item!", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
        log->info(G_cTxt);
        DeleteClient(iClientH, TRUE, TRUE);
        return;
    }

    // Are all 4 slates present ??
    if (ctr[0] != 1 || ctr[1] != 1 || ctr[2] != 1 || ctr[3] != 1)
    {
        bIsSlatePresent = FALSE;
        return;
    }

    if (m_pClientList[iClientH]->m_iAdminUserLevel > 3) bIsSlatePresent = TRUE;

    // if we failed, kill everything
    if (!bIsSlatePresent)
    {
        for (i = 0; i < 4; i++)
        {
            if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != NULL)
            {
                ItemDepleteHandler(iClientH, cItemID[i], FALSE);
            }
        }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, NULL, NULL, NULL, NULL);
        return;
    }

    // make the slates
    for (i = 0; i < 4; i++)
    {
        if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != NULL)
        {
            ItemDepleteHandler(iClientH, cItemID[i], FALSE);
        }
    }

    pItem = new CItem;

    i = iDice(1, 1000);

    if (i < 50)
    {
        // Hp slate
        iSlateType = 1;
        cSlateColor = 32;
    }
    else if (i < 250)
    {
        // Berserk slate
        iSlateType = 2;
        cSlateColor = 3;
    }
    else if (i < 750)
    {
        // Exp slate
        iSlateType = 4;
        cSlateColor = 7;
    }
    else if (i < 950)
    {
        // Mana slate
        iSlateType = 3;
        cSlateColor = 37;
    }
    else if (i < 1001)
    {
        // Hp slate
        iSlateType = 1;
        cSlateColor = 32;
    }

    // Notify client
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATESUCCESS, iSlateType, NULL, NULL, NULL);

    ZeroMemory(cData, sizeof(cData));

    // Create slates
    if (_bInitItemAttr(pItem, 867) == FALSE)
    {
        delete pItem;
        return;
    }
    else
    {
        pItem->m_sTouchEffectType = DEF_ITET_ID;
        pItem->m_sTouchEffectValue1 = iDice(1, 100000);
        pItem->m_sTouchEffectValue2 = iDice(1, 100000);
        pItem->m_sTouchEffectValue3 = (short)timeGetTime();

        _bItemLog(DEF_ITEMLOG_GET, iClientH, -1, pItem);

        pItem->m_sItemSpecEffectValue2 = iSlateType;
        pItem->m_cItemColor = cSlateColor;
        if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
        {
            ZeroMemory(cData, sizeof(cData));
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

            if (iEraseReq == 1) delete pItem;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);
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
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);
            SendEventToNearClient_TypeB(MSGID_MAGICCONFIGURATIONCONTENTS, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame,
                pItem->m_cItemColor);
            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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
    }
    return;
}

void CGame::ClientCommonHandler(int iClientH, char * pData)
{
    WORD * wp, wCommand;
    short * sp, sX, sY;
    int * ip, iV1, iV2, iV3, iV4;
    char * cp, cDir, * pString;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;

    wp = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
    wCommand = *wp;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    sp = (short *)cp;
    sX = *sp;
    cp += 2;

    sp = (short *)cp;
    sY = *sp;
    cp += 2;

    cDir = *cp;
    cp++;

    ip = (int *)cp;
    iV1 = *ip;
    cp += 4;

    ip = (int *)cp;
    iV2 = *ip;
    cp += 4;

    ip = (int *)cp;
    iV3 = *ip;
    cp += 4;

    pString = cp;
    cp += 30;

    ip = (int *)cp;
    iV4 = *ip;
    cp += 4;

    switch (wCommand)
    {

        case DEF_COMMONTYPE_REQ_CREATESLATE:
            ReqCreateSlateHandler(iClientH, pData);
            break;

        case DEF_COMMONTYPE_REQ_CHANGEPLAYMODE:
            RequestChangePlayMode(iClientH);
            break;

        case DEF_COMMONTYPE_SETGUILDTELEPORTLOC:
            RequestSetGuildTeleportLocHandler(iClientH, iV1, iV2, m_pClientList[iClientH]->m_iGuildGUID, "middleland");
            break;

        case DEF_COMMONTYPE_SETGUILDCONSTRUCTLOC:
            RequestSetGuildConstructLocHandler(iClientH, iV1, iV2, m_pClientList[iClientH]->m_iGuildGUID, pString);
            break;

        case DEF_COMMONTYPE_GUILDTELEPORT:
            RequestGuildTeleportHandler(iClientH);
            break;

        case DEF_COMMONTYPE_SUMMONWARUNIT:
            RequestSummonWarUnitHandler(iClientH, sX, sY, iV1, iV2, iV3);
            break;

        case DEF_COMMONTYPE_REQUEST_HELP:
            RequestHelpHandler(iClientH);
            break;

        case DEF_COMMONTYPE_REQUEST_MAPSTATUS:
            MapStatusHandler(iClientH, iV1, pString);
            break;

        case DEF_COMMONTYPE_REQUEST_SELECTCRUSADEDUTY:
            SelectCrusadeDutyHandler(iClientH, iV1);
            break;

        case DEF_COMMONTYPE_REQUEST_CANCELQUEST:
            CancelQuestHandler(iClientH);
            break;

        case DEF_COMMONTYPE_REQUEST_ACTIVATESPECABLTY:
            ActivateSpecialAbilityHandler(iClientH);
            break;

        case DEF_COMMONTYPE_REQUEST_JOINPARTY:
            JoinPartyHandler(iClientH, iV1, pString);
            break;

        case DEF_COMMONTYPE_GETMAGICABILITY:
            GetMagicAbilityHandler(iClientH);
            break;

        case DEF_COMMONTYPE_BUILDITEM:
            BuildItemHandler(iClientH, pData);
            break;

        case DEF_COMMONTYPE_QUESTACCEPTED:
            QuestAcceptedHandler(iClientH);
            break;

        case DEF_COMMONTYPE_CANCELEXCHANGEITEM:
            CancelExchangeItem(iClientH);
            break;

        case DEF_COMMONTYPE_CONFIRMEXCHANGEITEM:
            ConfirmExchangeItem(iClientH);
            break;

        case DEF_COMMONTYPE_SETEXCHANGEITEM:
            SetExchangeItem(iClientH, iV1, iV2);
            break;

        case DEF_COMMONTYPE_REQ_GETHEROMANTLE:
            GetHeroMantleHandler(iClientH, iV1, pString);
            break;

        case DEF_COMMONTYPE_REQ_GETOCCUPYFLAG:
            GetOccupyFlagHandler(iClientH);
            break;

        case DEF_COMMONTYPE_REQ_SETDOWNSKILLINDEX:
            SetDownSkillIndexHandler(iClientH, iV1);
            break;

        case DEF_COMMONTYPE_TALKTONPC:
            NpcTalkHandler(iClientH, iV1);
            break;

        case DEF_COMMONTYPE_REQ_CREATEPORTION:
            ReqCreatePortionHandler(iClientH, pData);
            break;

        case DEF_COMMONTYPE_REQ_GETFISHTHISTIME:
            ReqGetFishThisTimeHandler(iClientH);
            break;

        case DEF_COMMONTYPE_REQ_REPAIRITEMCONFIRM:
            ReqRepairItemCofirmHandler(iClientH, iV1, pString);
            break;

        case DEF_COMMONTYPE_REQ_REPAIRITEM:
            ReqRepairItemHandler(iClientH, iV1, iV2, pString);
            break;

        case DEF_COMMONTYPE_REQ_SELLITEMCONFIRM:
            ReqSellItemConfirmHandler(iClientH, iV1, iV2, pString);
            break;

        case DEF_COMMONTYPE_REQ_SELLITEM:
            ReqSellItemHandler(iClientH, iV1, iV2, iV3, pString);
            break;

        case DEF_COMMONTYPE_REQ_USESKILL:
            UseSkillHandler(iClientH, iV1, iV2, iV3);
            break;

        case DEF_COMMONTYPE_REQ_USEITEM:
            UseItemHandler(iClientH, iV1, iV2, iV3, iV4);
            break;

        case DEF_COMMONTYPE_REQ_GETREWARDMONEY:
            GetRewardMoneyHandler(iClientH);
            break;

        case DEF_COMMONTYPE_ITEMDROP:
            DropItemHandler(iClientH, iV1, iV2, pString, TRUE);
            break;

        case DEF_COMMONTYPE_EQUIPITEM:
            bEquipItemHandler(iClientH, iV1);
            break;

        case DEF_COMMONTYPE_REQ_PURCHASEITEM:
            RequestPurchaseItemHandler(iClientH, pString, iV1);
            break;

        case DEF_COMMONTYPE_REQ_STUDYMAGIC:
            RequestStudyMagicHandler(iClientH, pString);
            break;

        case DEF_COMMONTYPE_REQ_TRAINSKILL:
            //RequestTrainSkillHandler(iClientH, pString);
            break;

        case DEF_COMMONTYPE_GIVEITEMTOCHAR:
            GiveItemHandler(iClientH, cDir, iV1, iV2, iV3, iV4, pString);
            break;

        case DEF_COMMONTYPE_EXCHANGEITEMTOCHAR:
            ExchangeItemHandler(iClientH, cDir, iV1, iV2, iV3, iV4, pString);
            break;

        case DEF_COMMONTYPE_JOINGUILDAPPROVE:
            JoinGuildApproveHandler(iClientH, pString);
            break;

        case DEF_COMMONTYPE_JOINGUILDREJECT:
            JoinGuildRejectHandler(iClientH, pString);
            break;

        case DEF_COMMONTYPE_DISMISSGUILDAPPROVE:
            DismissGuildApproveHandler(iClientH, pString);
            break;

        case DEF_COMMONTYPE_DISMISSGUILDREJECT:
            DismissGuildRejectHandler(iClientH, pString);
            break;

        case DEF_COMMONTYPE_RELEASEITEM:
            ReleaseItemHandler(iClientH, iV1, TRUE);
            break;

        case DEF_COMMONTYPE_TOGGLECOMBATMODE:
            ToggleCombatModeHandler(iClientH);
            break;

        case DEF_COMMONTYPE_MAGIC:
            PlayerMagicHandler(iClientH, iV1, iV2, (iV3 - 100));
            break;

        case DEF_COMMONTYPE_TOGGLESAFEATTACKMODE:
            ToggleSafeAttackModeHandler(iClientH);
            break;

        case DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET:
            GetFightzoneTicketHandler(iClientH);
            break;

        case DEF_COMMONTYPE_UPGRADEITEM:
            RequestItemUpgradeHandler(iClientH, iV1);
            break;

        case DEF_COMMONTYPE_REQGUILDNAME:
            RequestGuildNameHandler(iClientH, iV1, iV2);
            break;

        case DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY:
            RequestAcceptJoinPartyHandler(iClientH, iV1);
            break;

        default:
            wsprintf(G_cTxt, "Unknown message received! (0x%.8X)", wCommand);
            log->info(G_cTxt);
            break;
    }
}

void CGame::JoinGuildApproveHandler(int iClientH, char * pName)
{
    int i;
    BOOL bIsExist = FALSE;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {
            if (memcmp(m_pClientList[i]->m_cLocation, m_pClientList[iClientH]->m_cLocation, 10) != 0) return;

            ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
            strcpy(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName);

            m_pClientList[i]->m_iGuildGUID = m_pClientList[iClientH]->m_iGuildGUID;

            ZeroMemory(m_pClientList[i]->m_cLocation, sizeof(m_pClientList[i]->m_cLocation));
            strcpy(m_pClientList[i]->m_cLocation, m_pClientList[iClientH]->m_cLocation);

            m_pClientList[i]->m_iGuildRank = DEF_GUILDSTARTRANK;
            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_JOINGUILDAPPROVE, NULL, NULL, NULL, NULL);
            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
            SendGuildMsg(i, DEF_NOTIFY_NEWGUILDSMAN, NULL, NULL, NULL);
            return;
        }
}

void CGame::JoinGuildRejectHandler(int iClientH, char * pName)
{
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {
            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_JOINGUILDREJECT, NULL, NULL, NULL, NULL);
            return;
        }
}

void CGame::DismissGuildApproveHandler(int iClientH, char * pName)
{
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    for (int i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {
            SendGuildMsg(i, DEF_NOTIFY_DISMISSGUILDSMAN, NULL, NULL, NULL);

            ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
            strcpy(m_pClientList[i]->m_cGuildName, "NONE");
            m_pClientList[i]->m_iGuildRank = -1;
            m_pClientList[i]->m_iGuildGUID = -1;

            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, NULL, NULL, NULL, NULL);

            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
            return;
        }
}

void CGame::DismissGuildRejectHandler(int iClientH, char * pName)
{
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    for (int i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {
            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_DISMISSGUILDREJECT, NULL, NULL, NULL, NULL);
            return;
        }
}

void CGame::GuildNotifyHandler(char * pData, DWORD dwMsgSize)
{
    char * cp, cCharName[11], cGuildName[30];

    ZeroMemory(cCharName, sizeof(cCharName));
    ZeroMemory(cGuildName, sizeof(cGuildName));

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    memcpy(cCharName, cp, 10);
    cp += 10;

    memcpy(cGuildName, cp, 20);
    cp += 20;
}

void CGame::ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char * pItemName)
{
    char cItemCategory, cItemName[30];
    short sRemainLife;
    int   iPrice;
    double d1, d2, d3;
    BOOL   bNeutral;
    DWORD  dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2, dwMul1, dwMul2;
    CItem * m_pGold;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;
    if (iNum <= 0) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

    iCalcTotalWeight(iClientH);

    m_pGold = new CItem;
    ZeroMemory(cItemName, sizeof(cItemName));
    wsprintf(cItemName, "Gold");
    _bInitItemAttr(m_pGold, cItemName);

    bNeutral = FALSE;
    if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) bNeutral = TRUE;
    switch (cSellToWhom)
    {
        case 15:
        case 24:
            cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;
            if ((cItemCategory >= 11) && (cItemCategory <= 50))
            {
                iPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) * iNum;
                sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

                if (bNeutral == TRUE) iPrice = iPrice / 2;
                if (iPrice <= 0)    iPrice = 1;
                if (iPrice > 1000000) iPrice = 1000000;

                if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH))
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
                }
                else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
            }
            else if ((cItemCategory >= 1) && (cItemCategory <= 10))
            {
                sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

                if (sRemainLife == 0)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 2, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
                }
                else
                {
                    d1 = (double)sRemainLife;
                    if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
                        d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
                    else d2 = 1.0f;
                    d3 = (d1 / d2) * 0.5f;
                    d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice;
                    d3 = d3 * d2;

                    iPrice = (int)d3;
                    iPrice = iPrice * iNum;

                    dwAddPrice1 = 0;
                    dwAddPrice2 = 0;
                    if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != NULL)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
                        dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;

                        switch (dwSWEType)
                        {
                            case 6: dwMul1 = 2; break;
                            case 8: dwMul1 = 2; break;
                            case 5: dwMul1 = 3; break;
                            case 1: dwMul1 = 4; break;
                            case 7: dwMul1 = 5; break;
                            case 2: dwMul1 = 6; break;
                            case 3: dwMul1 = 15; break;
                            case 9: dwMul1 = 20; break;
                            default: dwMul1 = 1; break;
                        }

                        d1 = (double)iPrice * dwMul1;
                        switch (dwSWEValue)
                        {
                            case 1: d2 = 10.0f; break;
                            case 2: d2 = 20.0f; break;
                            case 3: d2 = 30.0f; break;
                            case 4: d2 = 35.0f; break;
                            case 5: d2 = 40.0f; break;
                            case 6: d2 = 50.0f; break;
                            case 7: d2 = 100.0f; break;
                            case 8: d2 = 200.0f; break;
                            case 9: d2 = 300.0f; break;
                            case 10: d2 = 400.0f; break;
                            case 11: d2 = 500.0f; break;
                            case 12: d2 = 700.0f; break;
                            case 13: d2 = 900.0f; break;
                            default: d2 = 0.0f; break;
                        }
                        d3 = d1 * (d2 / 100.0f);

                        dwAddPrice1 = (int)(d1 + d3);
                    }

                    if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != NULL)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
                        dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;

                        switch (dwSWEType)
                        {
                            case 1:
                            case 12: dwMul2 = 2; break;

                            case 2:
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                            case 7: dwMul2 = 4; break;

                            case 8:
                            case 9:
                            case 10:
                            case 11: dwMul2 = 6; break;
                        }

                        d1 = (double)iPrice * dwMul2;
                        switch (dwSWEValue)
                        {
                            case 1: d2 = 10.0f; break;
                            case 2: d2 = 20.0f; break;
                            case 3: d2 = 30.0f; break;
                            case 4: d2 = 35.0f; break;
                            case 5: d2 = 40.0f; break;
                            case 6: d2 = 50.0f; break;
                            case 7: d2 = 100.0f; break;
                            case 8: d2 = 200.0f; break;
                            case 9: d2 = 300.0f; break;
                            case 10: d2 = 400.0f; break;
                            case 11: d2 = 500.0f; break;
                            case 12: d2 = 700.0f; break;
                            case 13: d2 = 900.0f; break;
                            default: d2 = 0.0f; break;
                        }
                        d3 = d1 * (d2 / 100.0f);

                        dwAddPrice2 = (int)(d1 + d3);
                    }

                    iPrice = iPrice + (dwAddPrice1 - (dwAddPrice1 / 3)) + (dwAddPrice2 - (dwAddPrice2 / 3));

                    if (bNeutral == TRUE) iPrice = iPrice / 2;
                    if (iPrice <= 0)    iPrice = 1;
                    if (iPrice > 1000000) iPrice = 1000000;

                    if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH))
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
                    }
                    else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
                }
            }
            else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 1, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
            break;


        default:
            break;
    }
    if (m_pGold != NULL) delete m_pGold;
}

void CGame::ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char * pString)
{
    CItem * pItemGold;
    short sRemainLife;
    int   iPrice;
    double d1, d2, d3;
    char * cp, cItemName[30], cData[120]{}, cItemCategory;
    DWORD * dwp, dwMul1, dwMul2, dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2;
    WORD * wp;
    int    iEraseReq, iRet;
    short * sp;
    BOOL   bNeutral;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;
    if (iNum <= 0) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

    if (m_pClientList[iClientH]->m_pIsProcessingAllowed == FALSE) return;

    iCalcTotalWeight(iClientH);
    cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

    bNeutral = FALSE;
    if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) bNeutral = TRUE;

    iPrice = 0;
    if ((cItemCategory >= 1) && (cItemCategory <= 10))
    {
        sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

        if (sRemainLife <= 0)
        {
            return;
        }
        else
        {
            d1 = (double)sRemainLife;
            if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
                d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
            else d2 = 1.0f;
            d3 = (d1 / d2) * 0.5f;
            d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice;
            d3 = d3 * d2;

            iPrice = (short)d3;
            iPrice = iPrice * iNum;

            dwAddPrice1 = 0;
            dwAddPrice2 = 0;
            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != NULL)
            {
                dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
                dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;

                switch (dwSWEType)
                {
                    case 6: dwMul1 = 2; break;
                    case 8: dwMul1 = 2; break;
                    case 5: dwMul1 = 3; break;
                    case 1: dwMul1 = 4; break;
                    case 7: dwMul1 = 5; break;
                    case 2: dwMul1 = 6; break;
                    case 3: dwMul1 = 15; break;
                    case 9: dwMul1 = 20; break;
                    default: dwMul1 = 1; break;
                }

                d1 = (double)iPrice * dwMul1;
                switch (dwSWEValue)
                {
                    case 1: d2 = 10.0f; break;
                    case 2: d2 = 20.0f; break;
                    case 3: d2 = 30.0f; break;
                    case 4: d2 = 35.0f; break;
                    case 5: d2 = 40.0f; break;
                    case 6: d2 = 50.0f; break;
                    case 7: d2 = 100.0f; break;
                    case 8: d2 = 200.0f; break;
                    case 9: d2 = 300.0f; break;
                    case 10: d2 = 400.0f; break;
                    case 11: d2 = 500.0f; break;
                    case 12: d2 = 700.0f; break;
                    case 13: d2 = 900.0f; break;
                    default: d2 = 0.0f; break;
                }
                d3 = d1 * (d2 / 100.0f);
                dwAddPrice1 = (int)(d1 + d3);
            }

            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != NULL)
            {
                dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
                dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;

                switch (dwSWEType)
                {
                    case 1:
                    case 12: dwMul2 = 2; break;

                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7: dwMul2 = 4; break;

                    case 8:
                    case 9:
                    case 10:
                    case 11: dwMul2 = 6; break;
                }

                d1 = (double)iPrice * dwMul2;
                switch (dwSWEValue)
                {
                    case 1: d2 = 10.0f; break;
                    case 2: d2 = 20.0f; break;
                    case 3: d2 = 30.0f; break;
                    case 4: d2 = 35.0f; break;
                    case 5: d2 = 40.0f; break;
                    case 6: d2 = 50.0f; break;
                    case 7: d2 = 100.0f; break;
                    case 8: d2 = 200.0f; break;
                    case 9: d2 = 300.0f; break;
                    case 10: d2 = 400.0f; break;
                    case 11: d2 = 500.0f; break;
                    case 12: d2 = 700.0f; break;
                    case 13: d2 = 900.0f; break;
                    default: d2 = 0.0f; break;
                }
                d3 = d1 * (d2 / 100.0f);
                dwAddPrice2 = (int)(d1 + d3);
            }

            iPrice = iPrice + (dwAddPrice1 - (dwAddPrice1 / 3)) + (dwAddPrice2 - (dwAddPrice2 / 3));

            if (bNeutral == TRUE) iPrice = iPrice / 2;
            if (iPrice <= 0) iPrice = 1;
            if (iPrice > 1000000) iPrice = 1000000;

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, NULL, NULL, NULL);

            _bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);

            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                (m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW))
            {
                SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
            }
            else ItemDepleteHandler(iClientH, cItemID, FALSE);
        }
    }
    else
        if ((cItemCategory >= 11) && (cItemCategory <= 50))
        {
            iPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
            iPrice = iPrice * iNum;

            if (bNeutral == TRUE) iPrice = iPrice / 2;
            if (iPrice <= 0) iPrice = 1;
            if (iPrice > 1000000) iPrice = 1000000;

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, NULL, NULL, NULL);

            _bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);

            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                (m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW))
            {
                SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
            }
            else ItemDepleteHandler(iClientH, cItemID, FALSE);
        }

    if (iPrice <= 0) return;

    pItemGold = new CItem;
    ZeroMemory(cItemName, sizeof(cItemName));
    wsprintf(cItemName, "Gold");
    _bInitItemAttr(pItemGold, cItemName);

    pItemGold->m_dwCount = iPrice;

    if (_bAddClientItemList(iClientH, pItemGold, &iEraseReq) == TRUE)
    {
        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        *cp = 1;
        cp++;

        memcpy(cp, pItemGold->m_cName, 20);
        cp += 20;

        dwp = (DWORD *)cp;
        *dwp = pItemGold->m_dwCount;
        cp += 4;

        *cp = pItemGold->m_cItemType;
        cp++;

        *cp = pItemGold->m_cEquipPos;
        cp++;

        *cp = (char)0;
        cp++;

        sp = (short *)cp;
        *sp = pItemGold->m_sLevelLimit;
        cp += 2;

        *cp = pItemGold->m_cGenderLimit;
        cp++;

        wp = (WORD *)cp;
        *wp = pItemGold->m_wCurLifeSpan;
        cp += 2;

        wp = (WORD *)cp;
        *wp = pItemGold->m_wWeight;
        cp += 2;

        sp = (short *)cp;
        *sp = pItemGold->m_sSprite;
        cp += 2;

        sp = (short *)cp;
        *sp = pItemGold->m_sSpriteFrame;
        cp += 2;

        *cp = pItemGold->m_cItemColor;
        cp++;

        *cp = (char)pItemGold->m_sItemSpecEffectValue2;
        cp++;

        dwp = (DWORD *)cp;
        *dwp = pItemGold->m_dwAttribute;
        cp += 4;
        /*
        *cp = (char)(pItemGold->m_dwAttribute & 0x00000001);
        cp++;
        */

        if (iEraseReq == 1)
            delete pItemGold;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

        iCalcTotalWeight(iClientH);

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
    else
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY, pItemGold);

        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItemGold->m_sSprite, pItemGold->m_sSpriteFrame, pItemGold->m_cItemColor);

        iCalcTotalWeight(iClientH);

        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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
}

void CGame::ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char * pString)
{
    char cItemCategory;
    short sRemainLife, sPrice;
    double d1, d2, d3;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;

    cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

    if ((cItemCategory >= 1) && (cItemCategory <= 10))
    {
        if (cRepairWhom != 24)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
            return;
        }

        sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
        if (sRemainLife == 0)
        {
            sPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
        }
        else
        {
            d1 = (double)sRemainLife;
            if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
                d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
            else d2 = 1.0f;
            d3 = (d1 / d2) * 0.5f;
            d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice;
            d3 = d3 * d2;

            sPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
        }

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REPAIRITEMPRICE, cItemID, sRemainLife, sPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
    }
    else if (((cItemCategory >= 43) && (cItemCategory <= 50)) || ((cItemCategory >= 11) && (cItemCategory <= 12)))
    {
        if (cRepairWhom != 15)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
            return;
        }

        sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
        if (sRemainLife == 0)
        {
            sPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
        }
        else
        {
            d1 = (double)sRemainLife;
            if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
                d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
            else d2 = 1.0f;
            d3 = (d1 / d2) * 0.5f;
            d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice;
            d3 = d3 * d2;

            sPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
        }

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REPAIRITEMPRICE, cItemID, sRemainLife, sPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
    }
    else
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 1, NULL, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
    }
}

void CGame::ReqRepairItemCofirmHandler(int iClientH, char cItemID, char * pString)
{
    short    sRemainLife, sPrice;
    char * cp, cItemCategory, cData[120]{};
    double   d1, d2, d3;
    DWORD * dwp, dwGoldCount;
    WORD * wp;
    int      iRet, iGoldWeight;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == NULL) return;

    if (m_pClientList[iClientH]->m_pIsProcessingAllowed == FALSE) return;

    cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

    if (((cItemCategory >= 1) && (cItemCategory <= 10)) || ((cItemCategory >= 43) && (cItemCategory <= 50)) ||
        ((cItemCategory >= 11) && (cItemCategory <= 12)))
    {
        sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
        if (sRemainLife == 0)
        {
            sPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
        }
        else
        {
            d1 = (double)abs(sRemainLife);
            if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
                d2 = (double)abs(m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan);
            else d2 = 1.0f;
            d3 = (d1 / d2) * 0.5f;
            d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice;
            d3 = d3 * d2;

            sPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
        }

        dwGoldCount = dwGetItemCount(iClientH, "Gold");

        if (dwGoldCount < (DWORD)sPrice)
        {
            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_NOTENOUGHGOLD;
            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
            *cp = cItemID;
            cp++;

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 7);
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
        else
        {
            m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMREPAIRED, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan, NULL, NULL);

            iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - sPrice);

            iCalcTotalWeight(iClientH);

            m_stCityStatus[m_pClientList[iClientH]->m_cSide].iFunds += sPrice;
        }
    }
    else
    {
    }
}

void CGame::ReqGetFishThisTimeHandler(int iClientH)
{
    int iResult, iFishH;
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_iAllocatedFish == NULL) return;
    if (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] == NULL) return;

    m_pClientList[iClientH]->m_bSkillUsingStatus[1] = FALSE;

    iResult = iDice(1, 100);
    if (m_pClientList[iClientH]->m_iFishChance >= iResult)
    {
        GetExp(iClientH, iDice(m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_iDifficulty, 5));
        CalculateSSN_SkillIndex(iClientH, 1, m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_iDifficulty);

        pItem = m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_pItem;
        m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_pItem = NULL;

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY,
            pItem);

        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FISHSUCCESS, NULL, NULL, NULL, NULL);
        iFishH = m_pClientList[iClientH]->m_iAllocatedFish;
        m_pClientList[iClientH]->m_iAllocatedFish = NULL;

        bDeleteFish(iFishH, 1);
        return;
    }

    m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FISHFAIL, NULL, NULL, NULL, NULL);

    m_pClientList[iClientH]->m_iAllocatedFish = NULL;
}

void CGame::RequestCreatePartyHandler(int iClientH)
{
    char * cp, cData[120];
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL)
    {
        return;
    }

    m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)cData;

    dwp = (DWORD *)cp;
    *dwp = MSGID_PARTYOPERATION;
    cp += 4;
    wp = (WORD *)cp;
    *wp = 1;
    cp += 2;

    wp = (WORD *)cp;
    *wp = iClientH;
    cp += 2;

    memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
    cp += 10;

    //SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);

    wsprintf(G_cTxt, "Request Create Party: %d", iClientH);
    log->info(G_cTxt);
}

void CGame::RequestJoinPartyHandler(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * cp, * token, cBuff[256], cData[120], cName[12];
    CStrTok * pStrTok;
    DWORD * dwp;
    WORD * wp;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL) return;
    if ((dwMsgSize) <= 0) return;
    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token != NULL)
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, token);
    }
    else return;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cCharName, cName) == 0))
        {
            if ((m_pClientList[i]->m_iPartyID == NULL) || (m_pClientList[i]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM))
            {
                return;
            }

            ZeroMemory(cData, sizeof(cData));

            cp = (char *)cData;
            dwp = (DWORD *)cp;
            *dwp = MSGID_PARTYOPERATION;
            cp += 4;
            wp = (WORD *)cp;
            *wp = 3;
            cp += 2;
            wp = (WORD *)cp;
            *wp = iClientH;
            cp += 2;
            memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
            cp += 10;
            wp = (WORD *)cp;
            *wp = m_pClientList[i]->m_iPartyID;
            cp += 2;
            //SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
            return;
        }

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cName);
}

void CGame::RequestDismissPartyHandler(int iClientH)
{
    char * cp, cData[120];
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;

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

    m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
}

void CGame::GetPartyInfoHandler(int iClientH)
{
    char * cp, cData[120];
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;

    ZeroMemory(cData, sizeof(cData));
    cp = (char *)cData;
    dwp = (DWORD *)cp;
    *dwp = MSGID_PARTYOPERATION;
    cp += 4;
    wp = (WORD *)cp;
    *wp = 5;
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

void CGame::RequestDeletePartyHandler(int iClientH)
{
    char * cp, cData[120];
    DWORD * dwp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
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
        m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
    }
}

void CGame::RequestAcceptJoinPartyHandler(int iClientH, int iResult)
{
    char * cp, cData[120];
    DWORD * dwp;
    WORD * wp;
    int iH;

    if (m_pClientList[iClientH] == NULL) return;

    switch (iResult)
    {
        case 0:
            iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
            if (m_pClientList[iH] == NULL)
            {
                return;
            }
            if (strcmp(m_pClientList[iH]->m_cCharName, m_pClientList[iClientH]->m_cReqJoinPartyName) != 0)
            {
                return;
            }
            if (m_pClientList[iH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING)
            {
                return;
            }
            if ((m_pClientList[iH]->m_iReqJoinPartyClientH != iClientH) || (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) != 0))
            {
                return;
            }

            SendNotifyMsg(NULL, iH, DEF_NOTIFY_PARTY, 7, 0, NULL, NULL);
            wsprintf(G_cTxt, "Party join reject(3) ClientH:%d ID:%d", iH, m_pClientList[iH]->m_iPartyID);
            log->info(G_cTxt);

            m_pClientList[iH]->m_iPartyID = NULL;
            m_pClientList[iH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
            m_pClientList[iH]->m_iReqJoinPartyClientH = NULL;
            ZeroMemory(m_pClientList[iH]->m_cReqJoinPartyName, sizeof(m_pClientList[iH]->m_cReqJoinPartyName));

            m_pClientList[iClientH]->m_iReqJoinPartyClientH = NULL;
            ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
            break;

        case 1:
            if ((m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM) && (m_pClientList[iClientH]->m_iPartyID != NULL))
            {
                iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
                if (m_pClientList[iH] == NULL)
                {
                    return;
                }
                if (strcmp(m_pClientList[iH]->m_cCharName, m_pClientList[iClientH]->m_cReqJoinPartyName) != 0)
                {
                    return;
                }
                if (m_pClientList[iH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING)
                {
                    return;
                }
                if ((m_pClientList[iH]->m_iReqJoinPartyClientH != iClientH) || (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) != 0))
                {
                    return;
                }

                ZeroMemory(cData, sizeof(cData));
                cp = (char *)cData;
                dwp = (DWORD *)cp;
                *dwp = MSGID_PARTYOPERATION;
                cp += 4;
                wp = (WORD *)cp;
                *wp = 3;
                cp += 2;
                wp = (WORD *)cp;
                *wp = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
                cp += 2;
                memcpy(cp, m_pClientList[iClientH]->m_cReqJoinPartyName, 10);
                cp += 10;
                wp = (WORD *)cp;
                *wp = m_pClientList[iClientH]->m_iPartyID;
                cp += 2;
                //SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
            }
            else
            {
                iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
                if (m_pClientList[iH] == NULL)
                {
                    return;
                }
                if (strcmp(m_pClientList[iH]->m_cCharName, m_pClientList[iClientH]->m_cReqJoinPartyName) != 0)
                {
                    return;
                }
                if (m_pClientList[iH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING)
                {
                    return;
                }
                if ((m_pClientList[iH]->m_iReqJoinPartyClientH != iClientH) || (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) != 0))
                {
                    return;
                }

                if (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_NULL)
                {
                    RequestCreatePartyHandler(iClientH);
                }
                else
                {
                }
            }
            break;

        case 2:
            if ((m_pClientList[iClientH]->m_iPartyID != NULL) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM))
            {
                RequestDismissPartyHandler(iClientH);
            }
            else
            {
                iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;

                if ((m_pClientList[iH] != NULL) && (m_pClientList[iH]->m_iReqJoinPartyClientH == iClientH) &&
                    (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) == 0))
                {
                    m_pClientList[iH]->m_iReqJoinPartyClientH = NULL;
                    ZeroMemory(m_pClientList[iH]->m_cReqJoinPartyName, sizeof(m_pClientList[iH]->m_cReqJoinPartyName));
                }

                m_pClientList[iClientH]->m_iPartyID = NULL;
                m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                m_pClientList[iClientH]->m_iReqJoinPartyClientH = NULL;
                ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
            }
            break;
    }
}

void CGame::RequestItemUpgradeHandler(int iClientH, int iItemIndex)
{
    int i, iItemX, iItemY, iSoM, iSoX, iSomH, iSoxH, iValue;
    DWORD dwTemp, dwSWEType;
    double dV1, dV2, dV3;
    short sItemUpgrade = 2;

    int bugint = 0;

    if (m_pClientList[iClientH] == NULL) return;
    if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == NULL) return;

    iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
    if (iValue >= 15 || iValue < 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, NULL, NULL, NULL);
        return;
    }

    switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cCategory)
    {
        case 1: // weapons upgrade
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
            {
                case 703: //
                case 709: // DarkKnightFlameberge 
                case 718: // DarkKnightGreatSword
                case 727: // DarkKnightFlamebergW
                case 736: //
                case 737: // DarkKnightAxe
                case 745: // DarkKnightHammer
                    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
                        return;
                    }

                    sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;

                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
                    {
                        if (iValue != 0)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                            return;
                        }
                    }

                    if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
                        return;
                    }

                    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);

                    if ((iValue == 0) && m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 703)
                    {
                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 709) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            return;
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;

                    }
                    else if ((iValue == 0) && ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 709) || (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 709)))
                    {

                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 709) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            return;
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);

                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;
                    }
                    else if ((iValue == 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 745))
                    {

                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 745) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            return;
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);

                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;
                    }
                    else if ((iValue == 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 737))
                    {

                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 737) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            return;
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);

                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;
                    }
                    else
                    {
                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                    }
                    break;

                default:
                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                        if (dwSWEType == 9)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                            return;
                        }
                    }
                    iSoX = iSoM = 0;
                    for (i = 0; i < DEF_MAXITEMS; i++)
                        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
                        {
                            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                            {
                                case 656: iSoX++; iSoxH = i; break;
                                case 657: iSoM++; iSomH = i; break;
                            }
                        }
                    if (iSoX > 0)
                    {
                        if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
                            if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE);
                            ItemDepleteHandler(iClientH, iSoxH, FALSE);
                            return;
                        }

                        if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL)
                        {
                            iValue++;
                            if (iValue > 10)
                                iValue = 10;
                            else
                            {
                                dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                                dwTemp = dwTemp & 0x0FFFFFFF;
                                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                                ItemDepleteHandler(iClientH, iSoxH, FALSE);
                            }
                        }
                        else
                        {
                            iValue++;
                            if (iValue > 7)
                                iValue = 7;
                            else
                            {
                                dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                                dwTemp = dwTemp & 0x0FFFFFFF;
                                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                                ItemDepleteHandler(iClientH, iSoxH, FALSE);
                            }
                        }
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                    break;
            }
            break;

        case 3:
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
            break;

        case 5:
            if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
            {
                dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                if (dwSWEType == 8)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                    return;
                }
            }
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
            {
                case 620:
                case 623:
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                    return;
                default: break;
            }


            iSoX = iSoM = 0;
            for (i = 0; i < DEF_MAXITEMS; i++)
                if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
                {
                    switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                    {
                        case 656: iSoX++; iSoxH = i; break;
                        case 657: iSoM++; iSomH = i; break;
                    }
                }

            if (iSoM > 0)
            {
                if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, TRUE) == FALSE)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                    iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
                    if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE);
                    ItemDepleteHandler(iClientH, iSomH, FALSE);
                    return;
                }

                iValue++;
                if (iValue > 10)
                    iValue = 10;
                else
                {
                    dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                    dwTemp = dwTemp & 0x0FFFFFFF;
                    m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL)
                    {
                        dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
                        dV2 = 0.2f * dV1;
                        dV3 = dV1 + dV2;
                    }
                    else
                    {
                        dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
                        dV2 = 0.15f * dV1;
                        dV3 = dV1 + dV2;
                    }
                    m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = (short)dV3;
                    if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 < 0)
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;

                    m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1;
                    ItemDepleteHandler(iClientH, iSomH, FALSE);
                }
            }
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, NULL, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);
            break;

        case 6:
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
            {
                case 621:
                case 622:

                case 700:
                case 701:
                case 702:
                case 704: 
                case 706:
                case 707:
                case 708:
                case 710:
                case 711:
                case 712:
                case 713:
                case 724:
                case 725:
                case 726:
                case 728:
                case 729:
                case 730:
                case 731:
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                    return;

                default:
                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != NULL)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                        if (dwSWEType == 8)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                            return;
                        }
                    }
                    iSoX = iSoM = 0;
                    for (i = 0; i < DEF_MAXITEMS; i++)
                        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
                        {
                            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                            {
                                case 656: iSoX++; iSoxH = i; break;
                                case 657: iSoM++; iSomH = i; break;
                            }
                        }
                    if (iSoM > 0)
                    {
                        if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, TRUE) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
                            if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE);
                            ItemDepleteHandler(iClientH, iSomH, FALSE);
                            return;
                        }
                        iValue++;
                        if (iValue > 10)
                            iValue = 10;
                        else
                        {
                            dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                            dwTemp = dwTemp & 0x0FFFFFFF;
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                            if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != NULL)
                            {
                                dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
                                dV2 = 0.2f * dV1;
                                dV3 = dV1 + dV2;
                            }
                            else
                            {
                                dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
                                dV2 = 0.15f * dV1;
                                dV3 = dV1 + dV2;
                            }
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = (short)dV3;
                            if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 < 0)
                                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;

                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1;
                            ItemDepleteHandler(iClientH, iSomH, FALSE);
                        }
                    }
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, NULL, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);
                    break;
            }
            break;

        case 8:
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
            {
                case 291:

                case 714:
                case 732:
                case 738:
                case 746:

                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
                    {
                        if (iValue != 0)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                            return;
                        }
                    }

                    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
                        return;
                    }
                    sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;

                    if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
                        return;
                    }

                    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, NULL, NULL, NULL);

                    if (iValue == 0)
                    {
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
                    }

                    if ((iValue == 11) && ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 714) || (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 738)))
                    {
                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 738) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            return;
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;


                    }
                    else if ((iValue == 15) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 738))
                    {
                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 746) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            return;
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;


                    }
                    else if ((iValue == 15) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 746))
                    {
                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 892) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            return;
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;


                    }
                    else
                    {
                        iValue += 1;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;
                    }

                default:
                    iSoX = iSoM = 0;
                    for (i = 0; i < DEF_MAXITEMS; i++)
                        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
                        {
                            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                            {
                                case 656: iSoX++; iSoxH = i; break;
                                case 657: iSoM++; iSomH = i; break;
                            }
                        }
                    if (iSoX > 0)
                    {
                        if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == FALSE)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                            iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
                            if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, FALSE);
                            ItemDepleteHandler(iClientH, iSoxH, FALSE);
                            return;
                        }

                        iValue++;
                        if (iValue > 7)
                            iValue = 7;
                        else
                        {
                            dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                            dwTemp = dwTemp & 0x0FFFFFFF;
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                            ItemDepleteHandler(iClientH, iSoxH, FALSE);
                        }
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);

                    break;
            }
            break;

        case 13:
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
            {
                case 400:
                case 401:
                    iSoX = iSoM = 0;
                    for (i = 0; i < DEF_MAXITEMS; i++)
                        if (m_pClientList[iClientH]->m_pItemList[i] != NULL)
                        {
                            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                            {
                                case 656: iSoX++; iSoxH = i; break;
                                case 657: iSoM++; iSomH = i; break;
                            }
                        }

                    if (iSoM < 1)
                    {
                        return;
                    }

                    bugint = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum;
                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, NULL, NULL, NULL);
                        return;
                    }

                    if ((m_pClientList[iClientH]->m_iContribution < 50) || (m_pClientList[iClientH]->m_iEnemyKillCount < 50))
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, NULL, NULL, NULL);
                        return;
                    }

                    m_pClientList[iClientH]->m_iContribution -= 50;
                    m_pClientList[iClientH]->m_iEnemyKillCount -= 50;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, NULL, NULL, NULL);

                    if (iValue == 0)
                    {
                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;

                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = NULL;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;

                        if (bugint == 400)
                        {
                            if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 427) == FALSE)
                            {
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                                return;
                            }
                        }
                        else
                        {
                            if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 428) == FALSE)
                            {
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
                                return;
                            }
                        }

                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        ItemDepleteHandler(iClientH, iSomH, FALSE);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;


                    }

                default: break;
            }
            break;

        default:
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, NULL, NULL);
            break;
    }
}
