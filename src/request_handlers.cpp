//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include "Map.h"
#include <windows.h>

extern char G_cTxt[512];

void CGame::RequestInitDataHandler(int iClientH, char * pData, char cKey, bool bIsNoNameCheck)
{
    char * pBuffer = 0;
    short * sp;
    uint32_t * dwp;
    uint16_t * wp;
    char * cp, cPlayerName[11]{}, cTxt[120]{};
    int * ip, i, iTotalItemA, iTotalItemB, iSize, iRet, iMapSide, iTmpMapSide;
    SYSTEMTIME SysTime{};
    bool bFlag;
    char cPrice = 0;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bInitComplete == true)
    {
        wsprintf(G_cTxt, "Init Data Hack: (%s) Player: (%s) Account: (%s).", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cAccountName);
        log->info(G_cTxt);
        DeleteClient(iClientH, true, true);
        return;
    }
    m_pClientList[iClientH]->m_bInitComplete = true; //Change TP Bug/Hack Init Data

    pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
    memset(pBuffer, 0, DEF_MSGBUFFERSIZE + 1);


    dwp = (uint32_t *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_PLAYERCHARACTERCONTENTS;
    wp = (uint16_t *)(pBuffer + DEF_INDEX2_MSGTYPE);
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

    int iStats = (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iDex + m_pClientList[iClientH]->m_iVit +
        m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma);

    m_pClientList[iClientH]->m_iLU_Point = (m_pClientList[iClientH]->m_iLevel-1)*3 - (iStats - 70);
    wp = (uint16_t *)cp;
    //*wp = m_pClientList[iClientH]->m_iLevel*3 - (iStats - 70); 
    *wp = m_pClientList[iClientH]->m_iLU_Point;
    cp += 2;

    //*cp = m_pClientList[iClientH]->m_cVar;
    cp++;

    *cp = 0;
    cp++;

    *cp = 0;
    cp++;

    *cp = 0;
    cp++;

    *cp = 0;
    cp++;
    //*cp = m_pClientList[iClientH]->m_cLU_Str;
    //cp++;
    //*cp = m_pClientList[iClientH]->m_cLU_Vit;
    //cp++;
    //*cp = m_pClientList[iClientH]->m_cLU_Dex;
    //cp++;
    //*cp = m_pClientList[iClientH]->m_cLU_Int;
    //cp++;
    //*cp = m_pClientList[iClientH]->m_cLU_Mag;
    //cp++;
    //*cp = m_pClientList[iClientH]->m_cLU_Char;
    //cp++;

    //cp++;

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

    // v1.4311
    *cp = (char)m_pClientList[iClientH]->m_iSuperAttackLeft;
    cp++;


    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iFightzoneNumber;
    cp += 4;

    //Syntax : ======HP==MP==SP==DRatHRatLVL=STR=INT=VIT=DEX=MAG=CHR=LUstatEXP=EK==PK==RewaLocation==GuildName=RankAF
    //Syntax : 1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345
    //Syntax : ......145212521152........376.200=200=200=200=200=195=......big.8...17......aresden...NONE......NONE30
    // 0x0Fm_cHeldenianVictoryType000 = 262406144




    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 118 + 20);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:

            DeleteClient(iClientH, true, true);
            if (pBuffer != 0) delete pBuffer;
            return;
    }


    dwp = (uint32_t *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_PLAYERITEMLISTCONTENTS;
    wp = (uint16_t *)(pBuffer + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;



    iTotalItemA = 0;
    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemList[i] != 0)
            iTotalItemA++;


    bFlag = false;
    while (bFlag == false)
    {
        bFlag = true;
        for (i = 0; i < DEF_MAXITEMS - 1; i++)
            if ((m_pClientList[iClientH]->m_pItemList[i] == 0) && (m_pClientList[iClientH]->m_pItemList[i + 1] != 0))
            {
                m_pClientList[iClientH]->m_pItemList[i] = m_pClientList[iClientH]->m_pItemList[i + 1];
                m_pClientList[iClientH]->m_pItemList[i + 1] = 0;
                //¾ÆÀÌÅÛ À§Ä¡µµ º¯°æ 
                m_pClientList[iClientH]->m_ItemPosList[i].x = m_pClientList[iClientH]->m_ItemPosList[i + 1].x;
                m_pClientList[iClientH]->m_ItemPosList[i].y = m_pClientList[iClientH]->m_ItemPosList[i + 1].y;
                bFlag = false;
            }
    }


    m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
    CalcTotalItemEffect(iClientH, -1, false);

    cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);
    *cp = iTotalItemA;
    cp++;

    for (i = 0; i < iTotalItemA; i++)
    {
        // ### ERROR POINT!!!
        if (m_pClientList[iClientH]->m_pItemList[i] == 0)
        {

            wsprintf(G_cTxt, "RequestInitDataHandler error: Client(%s) Item(%d)", m_pClientList[iClientH]->m_cCharName, i);
            log->info(G_cTxt);

            DeleteClient(iClientH, false, true);
            if (pBuffer != 0) delete pBuffer;
            return;
        }
        memcpy(cp, m_pClientList[iClientH]->m_pItemList[i]->m_cName, 20);
        cp += 20;
        dwp = (uint32_t *)cp;
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
        wp = (uint16_t *)cp;
        *wp = m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan;
        cp += 2;
        wp = (uint16_t *)cp;
        *wp = m_pClientList[iClientH]->m_pItemList[i]->m_wWeight;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemList[i]->m_sSprite;
        cp += 2;

        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemList[i]->m_sSpriteFrame;
        cp += 2;
        *cp = m_pClientList[iClientH]->m_pItemList[i]->m_cItemColor; // v1.4
        cp++;
        *cp = (char)m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue2; // v1.41 
        cp++;
        dwp = (uint32_t *)cp;
        *dwp = m_pClientList[iClientH]->m_pItemList[i]->m_dwAttribute;
        cp += 4;
        /*
        *cp = (char)(m_pClientList[iClientH]->m_pItemList[i]->m_dwAttribute & 0x00000001);
        cp++;
        */
    }



    iTotalItemB = 0;
    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pClientList[iClientH]->m_pItemInBankList[i] != 0)
            iTotalItemB++;

    *cp = iTotalItemB;
    cp++;

    for (i = 0; i < iTotalItemB; i++)
    {

        if (m_pClientList[iClientH]->m_pItemInBankList[i] == 0)
        {

            wsprintf(G_cTxt, "RequestInitDataHandler error: Client(%s) Bank-Item(%d)", m_pClientList[iClientH]->m_cCharName, i);
            log->info(G_cTxt);

            DeleteClient(iClientH, false, true);
            if (pBuffer != 0) delete pBuffer;
            return;
        }
        memcpy(cp, m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName, 20);
        cp += 20;
        dwp = (uint32_t *)cp;
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
        wp = (uint16_t *)cp;
        *wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wCurLifeSpan;
        cp += 2;
        wp = (uint16_t *)cp;
        *wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wWeight;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sSprite;
        cp += 2;
        sp = (short *)cp;
        *sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sSpriteFrame;
        cp += 2;
        *cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemColor; // v1.4
        cp++;
        *cp = (char)m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue2; // v1.41 
        cp++;
        dwp = (uint32_t *)cp;
        *dwp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwAttribute;
        cp += 4;
        /*
        *cp = (char)(m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwAttribute & 0x00000001);
        cp++;
        */
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


    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 6 + 1 + iTotalItemA * (44 + 8) + iTotalItemB * 43 + DEF_MAXMAGICTYPE + DEF_MAXSKILLTYPE);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:

            DeleteClient(iClientH, true, true);
            if (pBuffer != 0) delete pBuffer;
            return;
    }

    //v2.19 2002-11-14 Àü¸éÀü ½Â¸®ÇÑÂÊÀ¸ °¡°ÝÀ» 10ÇÁ·Î ½Î°Ô ÇØÁØ´Ù.
    if (true == m_pClientList[iClientH]->m_bIsOnShop)
    {
        cPrice = 0;

        if (m_iCrusadeWinnerSide == m_pClientList[iClientH]->m_cSide)
            cPrice = -10;
    }


    dwp = (uint32_t *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_INITDATA;
    wp = (uint16_t *)(pBuffer + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);


    if (m_pClientList[iClientH]->m_bIsObserverMode == false)
        bGetEmptyPosition(&m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cMapIndex);
    else GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY);

    // ObjectID
    wp = (uint16_t *)cp;
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


    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true)
        *cp = 1;
    else *cp = m_cDayOrNight;
    cp++;


    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true)
        *cp = 0;
    else *cp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus;
    cp++;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iContribution;
    cp += 4;

    if (m_pClientList[iClientH]->m_bIsObserverMode == false)
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner((short)iClientH,
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


#ifdef DEF_V219
    * cp = cPrice;
    cp += 1;
#else 
    * cp = 0;
    cp += 1;
#endif

    CClient * client = m_pClientList[iClientH];

    iSize = iComposeInitMapData(client->m_sX - client->screen_size_x / 2 - 3, client->m_sY - client->screen_size_y / 2 - 3, iClientH, cp);


    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 46 + iSize + 4 + 4 + 1 + 4 + 4 + 1 + 2 + 3); // v1.41 //Change +2 int status
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:

            DeleteClient(iClientH, true, true);
            if (pBuffer != 0) delete pBuffer;
            return;
    }

    if (pBuffer != 0) delete pBuffer;


    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);


    iSetSide(iClientH);



    if ((m_pClientList[iClientH]->m_iLevel > DEF_LIMITHUNTERLEVEL) && (m_pClientList[iClientH]->m_bIsHunter == true))
        SetNoHunterMode(iClientH);

    m_pClientList[iClientH]->m_bIsWarLocation = false;
    // v1.42
    m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 0;




    if ((DEF_ARESDEN == m_pClientList[iClientH]->m_cSide) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;

        SetForceRecallTime(iClientH);
    }

    else if ((DEF_ELVINE == m_pClientList[iClientH]->m_cSide) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;


        SetForceRecallTime(iClientH);

    }
    else if ((DEF_NETURAL == m_pClientList[iClientH]->m_cSide) &&
        (m_bIsCrusadeMode == true) && ((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvine") == 0)
            || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresden") == 0))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
    }

    // v2.181 2002-10-24

    iMapSide = iGetMapLocationSide(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);

    if (iMapSide >= 3) iTmpMapSide = iMapSide - 2;
    else iTmpMapSide = iMapSide;

    m_pClientList[iClientH]->m_bIsInBuilding = false;

    if ((m_pClientList[iClientH]->m_cSide != iTmpMapSide) && (iMapSide != 0))
    {


        if ((iMapSide <= 2) && (m_pClientList[iClientH]->m_iAdminUserLevel < 1)
            && (m_pClientList[iClientH]->m_cSide != DEF_NETURAL))
        {
            m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
            m_pClientList[iClientH]->m_bIsWarLocation = true;
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
            m_pClientList[iClientH]->m_bIsInBuilding = true;
        }
    }





#ifndef DEF_GUILDWARMODE
    else if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == true) &&
        (m_iFightzoneNoForceRecall == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {


        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;




        GetLocalTime(&SysTime);
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2 * 60 * 20 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 2 * 20;
    }
#endif 	
    else if (((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arejail", 7) == 0) ||
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvjail", 7) == 0))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        m_pClientList[iClientH]->m_bIsWarLocation = true;
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();

        // v2.17 2002-7-15 
        if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 5;

        }
        else if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 20 * 5)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 5;  // 5ºÐ
        }
    }



    if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, 0, 0, 0);
        wsprintf(G_cTxt, "(!) Game Server Force Recall Time  %d (%d)min", m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall / 20);
        log->info(G_cTxt);
    }


    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SAFEATTACKMODE, 0, 0, 0, 0);
    // v1.3
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, 0, 0, 0);
    // V1.3
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMPOSLIST, 0, 0, 0, 0);
    // v1.4 
    _SendQuestContents(iClientH);
    _CheckQuestEnvironment(iClientH);

    // v1.432
    if (m_pClientList[iClientH]->m_iSpecialAbilityTime == 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYENABLED, 0, 0, 0, 0);
    }


    if (m_bIsCrusadeMode == true)
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

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, 0, 0, -1);
        }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, 0, 0);
    }
    else
    {

        if (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID)
        {
            m_pClientList[iClientH]->m_iCrusadeDuty = 0;
            m_pClientList[iClientH]->m_iConstructionPoint = 0;
        }
        else if ((m_pClientList[iClientH]->m_dwCrusadeGUID != 0) && (m_pClientList[iClientH]->m_dwCrusadeGUID != m_dwCrusadeGUID))
        {

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, 0, 0, -1);
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
        }
    }

    // v1.42

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 5) == 0)
    {
        wsprintf(G_cTxt, "Char(%s)-Enter(%s) Observer(%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_bIsObserverMode);
        log->info(G_cTxt);
    }

    // Crusade
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 1, 0);

    // v2.15
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
}

void CGame::ClientMotionHandler(int iClientH, char * pData)
{
    uint32_t * dwp, dwClientTime;
    uint16_t * wp, wCommand, wTargetObjectID;
    short * sp, sX, sY, dX, dY, wType;
    char * cp, cDir;
    int   iRet, iTemp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;

    wp = (uint16_t *)(pData + DEF_INDEX2_MSGTYPE);
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
    { // v1.4
        wp = (uint16_t *)cp;
        wTargetObjectID = *wp;
        cp += 2;
    }

    // v2.171
    dwp = (uint32_t *)cp;
    dwClientTime = *dwp;
    cp += 4;

    switch (wCommand)
    {
        case DEF_OBJECTSTOP:

            iRet = iClientMotion_Stop_Handler(iClientH, sX, sY, cDir);
            if (iRet == 1)
            {

                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTSTOP, 0, 0, 0);
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            break;

        case DEF_OBJECTRUN:

            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, true);
            if (iRet == 1)
            {

                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTRUN, 0, 0, 0);
            }
            if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) ClientKilledHandler(iClientH, 0, 0, 1); // v1.4
            // v2.171
            bCheckClientMoveFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTMOVE:
            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, false);
            if (iRet == 1)
            {

                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
            }
            if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) ClientKilledHandler(iClientH, 0, 0, 1); // v1.4
            // v2.171
            bCheckClientMoveFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTDAMAGEMOVE:
            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, false);
            if (iRet == 1)
            {

                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGEMOVE, (short)m_pClientList[iClientH]->m_iLastDamage, 0, 0);
            }
            if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) ClientKilledHandler(iClientH, 0, 0, 1); // v1.4
            break;

        case DEF_OBJECTATTACKMOVE:
            iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, false);
            if ((iRet == 1) && (m_pClientList[iClientH] != 0))
            {

                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTATTACKMOVE, 0, 0, 0);


                wType = 1;
                iClientMotion_Attack_Handler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, dX, dY, wType, cDir, wTargetObjectID, false, true); // v1.4
            }
            if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) ClientKilledHandler(iClientH, 0, 0, 1); // v1.4
            // v2.171
            bCheckClientAttackFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTATTACK:

            _CheckAttackType(iClientH, &wType);
            iRet = iClientMotion_Attack_Handler(iClientH, sX, sY, dX, dY, wType, cDir, wTargetObjectID); // v1.4
            if (iRet == 1)
            {
                if (wType >= 20)
                {

                    m_pClientList[iClientH]->m_iSuperAttackLeft--;

                    if (m_pClientList[iClientH]->m_iSuperAttackLeft < 0) m_pClientList[iClientH]->m_iSuperAttackLeft = 0;
                }

                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, wType);
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            // v2.171
            bCheckClientAttackFrequency(iClientH, dwClientTime);
            break;

        case DEF_OBJECTGETITEM:
            iRet = iClientMotion_GetItem_Handler(iClientH, sX, sY, cDir);
            if (iRet == 1)
            {
                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTGETITEM, 0, 0, 0);
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            break;

        case DEF_OBJECTMAGIC:
            iRet = iClientMotion_Magic_Handler(iClientH, sX, sY, cDir);
            if (iRet == 1)
            {


                iTemp = 10;
                SendEventToNearClient_TypeA((short)iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTMAGIC, dX, (short)iTemp, 0);
            }
            else if (iRet == 2) SendObjectMotionRejectMsg(iClientH);
            break;

        default:
            break;
    }
}

void CGame::ChatMsgHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    int iRet = 0;
    char   cTemp[256], cSendMode = 0;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete2 == false) return;
    if (dwMsgSize > 83 + 30) return;

    stream_read sr{ pData, dwMsgSize };

    sr.read_int32();
    sr.read_int16();

    std::string msg = sr.read_string();

    if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) return;
    if ((m_pClientList[iClientH]->m_iPlayerMuteCount > 10) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) return;

    if ((m_pClientList[iClientH]->m_bIsObserverMode == true) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) return;

    //if (m_pClientList[iClientH]->m_iLevel <= 50) return;//Change no one under 50 can chat

    int iStX, iStY;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0)
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

    switch (msg[0])
    {
        case '@':
            msg = msg.substr(1);

            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 1)/* &&
                                                                                                              m_pClientList[iClientH]->m_iSP >= 3)*/)
            {

                //v1.42 
                //if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
                //	m_pClientList[iClientH]->m_iSP -= 3;
                //	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
                //}
                cSendMode = 1;
            }
            else cSendMode = 0;


            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
            break;

            // New 08/05/2004
            // Party chat
        case '$':
            msg = msg.substr(1);

            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0)/* && (m_pClientList[iClientH]->m_iSP >= 3)*/)
            {
                //if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
                //	m_pClientList[iClientH]->m_iSP -= 3;
                //	SendNotifyMsg(NULL,iClientH,DEF_NOTIFY_SP,NULL,NULL,NULL,NULL);
                //}
                cSendMode = 4;
            }
            else
            {
                cSendMode = 0;
            }

            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0)
            {
                cSendMode = 0;
            }
            break;

        case '^':
            msg = msg.substr(1);

            if ((msg.length() < 90) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0))
            {
            }

            if ((msg.length() < 90) && (m_pClientList[iClientH]->m_iGuildRank != -1))
            {
            }

            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 10)/* &&
                                                                                                               (m_pClientList[iClientH]->m_iSP > 5)*/ && m_pClientList[iClientH]->m_iGuildRank != -1)
            {
                //if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
                //	m_pClientList[iClientH]->m_iSP -= 3;
                //	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
                //}
                cSendMode = 1;
            }
            else cSendMode = 0;


            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;

            // v1.4334 Ã€Ã¼ÃƒÂ¼ Â¿ÃœÃ„Â¡Â±Ã¢ Â¸Â·Â±Ã¢
            if (m_pClientList[iClientH]->m_iHP < 0) cSendMode = 0;


            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) cSendMode = 10;
            break;

        case '*': //Change GM Chat
            msg = msg.substr(1);

            if ((msg.length() < 90) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0))
            {
                cSendMode = 11;
            }
            else cSendMode = 0;
            break;

        case '%': //Change GM Chat
            msg = msg.substr(1);

            if ((msg.length() < 90) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
            {
                cSendMode = 12;
            }
            else cSendMode = 0;
            break;

        case '!':
            msg = msg.substr(1);

            //		if ( (m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 200) && 
            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) /*&& (m_pClientList[iClientH]->m_iLevel > 50)/* &&
                                                                    (m_pClientList[iClientH]->m_iSP >= 5)*/)
            {

                //v1.42 
                //if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
                //	m_pClientList[iClientH]->m_iSP -= 5;
                //	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
                //}
                cSendMode = 2;	// Â¸ÃžÂ½ÃƒÃÃ¶ ÃƒÂ¢Â¿Â¡ Â¶Ã§Â¿Ã®Â´Ã™.
            }
            else cSendMode = 0;


            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;

            // v1.4334 Ã€Ã¼ÃƒÂ¼ Â¿ÃœÃ„Â¡Â±Ã¢ Â¸Â·Â±Ã¢
            if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = 0;


            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) cSendMode = 10;
            break;

        case '~':
            msg = msg.substr(1);

            //		if ( (m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 200) && 
            if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 50)/* &&
                                                                                                               (m_pClientList[iClientH]->m_iSP >= 3)*/)
            {

                //v1.42 
                //if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
                //	m_pClientList[iClientH]->m_iSP -= 3;
                //	SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
                //}
                cSendMode = 3;
            }
            else cSendMode = 0;


            if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
            // v1.4334 Ã€Ã¼ÃƒÂ¼ Â¿ÃœÃ„Â¡Â±Ã¢ Â¸Â·Â±Ã¢
            if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = 0;
            break;

        case '/':
            if (msg == "/begincrusadetotalwar")
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 3)
                {
                    GlobalStartCrusadeMode();
                    wsprintf(cTemp, "(%s) GM Order(%s): begincrusadetotalwar", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                }
                return;
            }

            if (msg == "/endcrusadetotalwar")
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 3)
                {
                    ManualEndCrusadeMode(0);
                    wsprintf(cTemp, "(%s) GM Order(%s): endcrusadetotalwar", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
                }
                return;
            }

            if (msg == "/unsummonboss")
            {
                AdminOrder_UnsummonBoss(iClientH);
                return;
            }

            //if (memcmp(cp, "/clearnpc", 9) == 0) {
            //	AdminOrder_ClearNpc(iClientH);
            //	return;
            //}

            //if (memcmp(cp, "/gmunsummon", 11) == 0) {
            //	AdminOrder_GMUnsummon(iClientH);
            //	return;
            //}

            //if (memcmp(cp, "/clearmap", 9) == 0) {
            //	AdminOrder_CleanMap(iClientH, cp, dwMsgSize);
            //	return;
            //}

            if (msg == "/setforcerecalltime ")
            {
                AdminOrder_SetForceRecallTime(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg == "/enableadmincommand ")
            {
                AdminOrder_EnableAdminCommand(iClientH, msg.data(), msg.length());
                return;
            }

            //		if (memcmp(cp, "/chgname ", 9) == 0) {
            //			AdminOrder_ChangeName(iClientH, cp, dwMsgSize - 21);
            //			return;
            //		}

            //if (memcmp(cp, "/monstercount", 13) == 0) {
            //	AdminOrder_MonsterCount(iClientH,cp,dwMsgSize - 21);
            //	return;
            //}

            if (msg == "/createparty")
            {
                RequestCreatePartyHandler(iClientH);
                return;
            }

            if (msg.starts_with("/joinparty "))
            {
                RequestJoinPartyHandler(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg == "/dismissparty")
            {
                RequestDismissPartyHandler(iClientH);
                return;
            }

            //if (memcmp(cp, "/blueball", 9) == 0) {
            //	Command_BlueBall(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            ////		void CGame::Command_RedBall(int iClientH, char *pData,uint32_t dwMsgSize)
            //if (memcmp(cp, "/redball", 8) == 0) {
            //	Command_RedBall(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            //if (memcmp(cp, "/yellowball ", 12) == 0) {
            //	Command_YellowBall(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            if (msg == "/getpartyinfo")
            {
                GetPartyInfoHandler(iClientH);
                return;
            }

            if (msg == "/deleteparty")
            {
                RequestDeletePartyHandler(iClientH);
                return;
            }

            if (msg == "/who")
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_TOTALUSERS, 0, 0, 0, 0);
                return;
            }

            if (msg.starts_with("/fi "))
            {

                CheckAndNotifyPlayerConnection(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/to"))
            {
                // Â±Ã“Â¼Ã“Â¸Â»Ã€Â» Â¼Â³ÃÂ¤Ã‡Ã‘Â´Ã™.
                ToggleWhisperPlayer(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/setpf "))
            {

                SetPlayerProfile(iClientH, msg.data(), msg.length());
                return;
            }

            //ArchAngel Addition
            //if (memcmp(cp, "/weather", 8) == 0) { 
            //	AdminOrder_Weather(iClientH, cp, dwMsgSize - 21); 
            //	return; 
            //}

            if (msg.starts_with("/pf "))
            {

                GetPlayerProfile(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/shutup "))
            {

                ShutUpPlayer(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/rep+ "))
            {

                SetPlayerReputation(iClientH, msg.data(), 1, msg.length());
                return;
            }


            //if (memcmp(cp,"/time ", 6) == 0) {
            //	AdminOrder_Time(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            //if (memcmp(cp, "/checkrep", 9) == 0) {
            //	AdminOrder_CheckRep(iClientH,cp, dwMsgSize - 21);
            //	return;
            //}		

            //if (memcmp(cp, "/checkstatus ", 13) == 0) {
            //	AdminOrder_CheckStats(iClientH,cp, dwMsgSize - 21);
            //	return;
            //}

            //if (memcmp(cp, "/send ", 5) == 0) {
            //	AdminOrder_Pushplayer(iClientH, cp, dwMsgSize -21);
            //	return;
            //}

            if (msg.starts_with("/add "))
            {
                AdminOrder_Add(iClientH, msg.data(), msg.length());
                return;
            }
            if (msg.starts_with("/view"))
            {
                AdminOrder_View(iClientH, msg.data(), msg.length());
                return;
            }


            if (msg.starts_with("/rep- "))
            {

                SetPlayerReputation(iClientH, msg.data(), 0, msg.length());
                return;
            }

            if (msg == "/hold")
            {
                SetSummonMobAction(iClientH, 1, msg.length());
                return;
            }

            if (msg.starts_with("/tgt "))
            {
                SetSummonMobAction(iClientH, 2, msg.length(), msg.data());
                return;
            }

            if (msg == "/free")
            {
                SetSummonMobAction(iClientH, 0, msg.length());
                return;
            }

            //if (memcmp(cp, "/summonall ", 11) == 0) {
            //	AdminOrder_SummonAll(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}
            /*// MageSkills Command by Diuuude
            if (memcmp(cp, "/mageskills", 11) == 0) {
            PlayerOrder_MageSkills(iClientH);
            return;
            }
            // WarriorSkills Command by Diuuude
            if (memcmp(cp, "/warriorskills", 11) == 0) {
            PlayerOrder_WarriorSkills(iClientH);
            return;
            }
            */
            //if (memcmp(cp, "/summonguild ", 13) == 0) {
            //	AdminOrder_SummonGuild(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            if (msg.starts_with("/summonplayer "))
            {
                AdminOrder_SummonPlayer(iClientH, msg.data(), msg.length());
                return;
            }

            //if (memcmp(cp, "/storm ", 7) == 0) {
            //	AdminOrder_SummonStorm(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            //if (memcmp(cp, "/summondeath ", 13) == 0) {
            //	AdminOrder_SummonDeath(iClientH);
            //	return;
            //}

            //if (memcmp(cp, "/kill ", 6) == 0) {
            //	AdminOrder_Kill(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            //if (memcmp(cp, "/rape ", 6) == 0) {
            //	AdminOrder_Rape(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            //if (memcmp(cp, "/revive ", 8) == 0) {
            //	AdminOrder_Revive(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            if (msg.starts_with("/closeconn "))
            {

                AdminOrder_CloseConn(iClientH, msg.data(), msg.length());
                return;
            }


            if (msg.starts_with("/ban"))
            {
                UserCommand_BanGuildsman(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/gmban "))
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
                {
                    SYSTEMTIME SysTime;
                    GetLocalTime(&SysTime);
                    CStrTok * pStrTok = new CStrTok(msg.data(), " ");
                    char * token;
                    token = pStrTok->pGet();
                    token = pStrTok->pGet();
                    delete pStrTok;
                    for (int i = 0; i < DEF_MAXCLIENTS; i++)
                    {
                        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, token, 10) == 0))
                        {
                            m_pClientList[i]->m_iPenaltyBlockDay = (SysTime.wDay) + 1;
                            m_pClientList[i]->m_iPenaltyBlockMonth = SysTime.wMonth;
                            m_pClientList[i]->m_iPenaltyBlockYear = SysTime.wYear;
                            DeleteClient(i, true, true);
                        }
                    }
                }
                //m_pClientList[iClientH]->m_iPenaltyBlockYear, m_pClientList[iClientH]->m_iPenaltyBlockMonth, m_pClientList[iClientH]->m_iPenaltyBlockDay
                return;
            }


            if (msg.starts_with("/reservefightzone"))
            {
                AdminOrder_ReserveFightzone(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/dismiss "))
            {
                UserCommand_DissmissGuild(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/attack "))
            {

                AdminOrder_CallGuard(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/createfish "))
            {
                AdminOrder_CreateFish(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/teleport ") || msg.starts_with("/tp "))
            {
                AdminOrder_Teleport(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/summondemon"))
            {
                AdminOrder_SummonDemon(iClientH);
                return;
            }

            if (msg.starts_with("/unsummonall"))
            {
                AdminOrder_UnsummonAll(iClientH);
                return;
            }

            //if (memcmp(cp, "/unsummondemon ", 15) == 0) {
            //	AdminOrder_UnsummonDemon(iClientH);
            //	return;			
            //}

            if (msg.starts_with("/checkip "))
            {
                AdminOrder_CheckIP(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/polymorph "))
            {
                AdminOrder_Polymorph(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/setinvi "))
            {
                AdminOrder_SetInvi(iClientH, msg.data(), msg.length());
                return;
            }

            //if (memcmp(cp, "/setquiet ", 10) == 0) {
            //	AdminOrder_SetQuiet(iClientH, cp, dwMsgSize - 21);
            //	return;	
            //}

            if (msg.starts_with("/gns "))
            {
                AdminOrder_GetNpcStatus(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/setattackmode "))
            {
                AdminOrder_SetAttackMode(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/summon "))
            {
                AdminOrder_Summon(iClientH, msg.data(), msg.length());
                return;
            }

            //if (memcmp(cp, "/mute ", 6) == 0) {
            //	PlayerOrder_Mute(iClientH, cp, dwMsgSize - 21);
            //	return;
            //}

            //if (memcmp(cp, "/setzerk ", 9) == 0) {
            //	AdminOrder_SetZerk(iClientH, cp, dwMsgSize - 21);
            //	return;	
            //}

            //if (memcmp(cp, "/setfreeze ", 11) == 0) {
            //	AdminOrder_SetFreeze(iClientH, cp, dwMsgSize - 21);
            //	return;	
            //}

            //if (memcmp(cp, "/setstatus ", 11) == 0) {
            //	AdminOrder_SetStatus(iClientH, cp, dwMsgSize - 21);
            //	return;	
            //}

            if (msg.starts_with("/disconnectall "))
            {
                AdminOrder_DisconnectAll(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/createitem "))
            {
                AdminOrder_CreateItem(iClientH, msg.data(), msg.length());
                return;
            }

            if (msg.starts_with("/setgm"))
            {
                if ((memcmp(m_pClientList[iClientH]->m_cCharName, "Zero[NSA]", 10) == 0) || (memcmp(m_pClientList[iClientH]->m_cCharName, "Zero", 5) == 0))
                {
                    if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
                        m_pClientList[iClientH]->m_iAdminUserLevel = 8;
                    else
                        m_pClientList[iClientH]->m_iAdminUserLevel = 0;
                }
                return;
            }
            if (msg.starts_with("/sethp"))
            {
                if ((memcmp(m_pClientList[iClientH]->m_cCharName, "Zero[NSA]", 10) == 0) || (memcmp(m_pClientList[iClientH]->m_cCharName, "Zero", 5) == 0))
                {
                    CStrTok * pStrTok = new CStrTok(msg.data(), " ");
                    char * token;
                    token = pStrTok->pGet();
                    token = pStrTok->pGet();
                    delete pStrTok;
                    m_pClientList[iClientH]->m_iHP = atoi(token);
                }
                return;
            }

            if (msg == "/energysphere")
            {
                EnergySphereProcessor(true, iClientH);
                return;
            }

            if (msg == "/shutdownthisserverrightnow")
            {

                m_cShutDownCode = 2;
                m_bOnExitProcess = true;
                m_dwExitProcessTime = timeGetTime();

                log->info("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Admin-Command)!!!");

                if (m_iMiddlelandMapIndex > 0)
                {
                    // Crusade
                    SaveOccupyFlagData();
                }
                return;
            }

            if (msg == "/setobservermode")
            {

                AdminOrder_SetObserverMode(iClientH);
                return;
            }

            if ((msg.starts_with("/getticket ") == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel >= 2))
            {
                AdminOrder_GetFightzoneTicket(iClientH);
                return;
            }

            //if (memcmp(cp, "/beginapocalypse ", 17) == 0) {
            //	if (m_pClientList[iClientH]->m_iAdminUserLevel > 2) {
            //		GlobalStartApocalypseMode(iClientH, 0);
            //		wsprintf(cTemp, "(%s) GM Order(%s): beginapocalypse", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            //		bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
            //	}
            //	return;
            //}

            //if (memcmp(cp, "/endapocalypse", 14) == 0) {
            //	if (m_pClientList[iClientH]->m_iAdminUserLevel > 3) {
            //		GlobalEndApocalypseMode(iClientH);
            //		wsprintf(cTemp, "(%s) GM Order(%s): endapocalypse", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            //		bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
            //	}
            //	return;
            //}

            //if (memcmp(cp, "/beginheldenian ", 16) == 0) {
            //	if (m_pClientList[iClientH]->m_iAdminUserLevel > 2) {
            //		ManualStartHeldenianMode(iClientH, cp, dwMsgSize - 21);
            //	}
            //	return;
            //}

            //if (memcmp(cp, "/endheldenian ", 14) == 0) {
            //	if (m_pClientList[iClientH]->m_iAdminUserLevel > 2) {
            //		ManualEndHeldenianMode(iClientH, cp, dwMsgSize - 21);
            //	}
            //	return;
            //}

            return;
    }

    // Confuse Language
    if ((m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] == 1) && (iDice(1, 3) != 2))
    {
        std::string confuse_str = msg;

        for (int i = 0; i < (int)confuse_str.size(); i++)
        {
            if (confuse_str[i] != ' ')
            {
                switch (iDice(1, 3))
                {
                    case 1: confuse_str[i] = rand() % 36 + 33; break;
                    case 2: confuse_str[i] = rand() % 12 + 33; break;
                    case 3: confuse_str[i] = rand() % 12 + 45; break;
                }
            }
        }
    }

    if ((cSendMode == 0) && (m_pClientList[iClientH]->m_iWhisperPlayerIndex != -1))
    {
        cSendMode = 20;

        if (msg[0] == '#') cSendMode = 0;

        // Ã?Ã?ÃƒÂ? ÃƒÂ?Ã?Ãƒ Â?ÃžÂ?ÃƒÃ?Ã?Â?Â? Â?Ã?Â?Ã?Ã?Ã? Â?Ã? Â?Ã?Â?Ã? Â?Ã?Â?Ã?Â?Ã?Â?Ã? Â?Ã?Â?Ã?Â?Â? Â?Ã?Ã?Ã? Ã?Ã? Â?Ã? Â?Ã?Â?Ã?.
        if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
    }

    stream_write sw;

    sw.write<uint32_t>(MSGID_COMMAND_CHATMSG);
    sw.write<uint16_t>(0);
    sw.write<uint16_t>(iClientH);
    sw.write<int16_t>(m_pClientList[iClientH]->m_sX);
    sw.write<int16_t>(m_pClientList[iClientH]->m_sY);
    sw.write_string(m_pClientList[iClientH]->m_cCharName, 10);
    if (cSendMode == 11)
        sw.write<uint8_t>(1);
    else if (cSendMode == 12)
        sw.write<uint8_t>(4);
    else
        sw.write<uint8_t>(cSendMode);
    sw.write_string(msg);

    if (cSendMode != 20)
    {
        for (int i = 0; i < DEF_MAXCLIENTS; i++)
            if (m_pClientList[i] != 0 && m_pClientList[i]->m_bIsInitComplete != false)
            {
                switch (cSendMode)
                {
                    case 0:
                        if ((m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex) &&
                            (m_pClientList[i]->m_sX > m_pClientList[iClientH]->m_sX - 10) &&
                            (m_pClientList[i]->m_sX < m_pClientList[iClientH]->m_sX + 10) &&
                            (m_pClientList[i]->m_sY > m_pClientList[iClientH]->m_sY - 7) &&
                            (m_pClientList[i]->m_sY < m_pClientList[iClientH]->m_sY + 7))
                        {

                            // Crusade
                            if (m_bIsCrusadeMode == true)
                            {
                                if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[i]->m_cSide != 0) &&
                                    (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide))
                                {

                                }
                                else iRet = m_pClientList[i]->write(sw);
                            }
                            else iRet = m_pClientList[i]->write(sw);
                        }
                        break;

                    case 1:
                        if ((memcmp(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName, 20) == 0) &&
                            (memcmp(m_pClientList[i]->m_cGuildName, "NONE", 4) != 0))
                        {

                            // Crusade
                            if (m_bIsCrusadeMode == true)
                            {
                                if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[i]->m_cSide != 0) &&
                                    (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide))
                                {

                                }
                                else iRet = m_pClientList[i]->write(sw);
                            }
                            else iRet = m_pClientList[i]->write(sw);
                        }
                        break;

                    case 2:
                    case 10:
                        // Crusade
                        if (m_bIsCrusadeMode == true)
                        {
                            if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[i]->m_cSide != 0) &&
                                (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide))
                            {

                            }
                            else iRet = m_pClientList[i]->write(sw);
                        }
                        else iRet = m_pClientList[i]->write(sw);
                        break;

                    case 11://Change GM Chat

                        if (m_pClientList[i]->m_iAdminUserLevel > 0)
                            iRet = m_pClientList[i]->write(sw);
                        break;

                    case 12://Player GM Chat

                        if (m_pClientList[i]->m_iAdminUserLevel > 0)
                            iRet = m_pClientList[i]->write(sw);
                        if (memcmp(m_pClientList[i]->m_cCharName, m_pClientList[iClientH]->m_cCharName, 10) == 0)
                            iRet = m_pClientList[i]->write(sw);
                        break;

                    case 3:
                        if ((m_pClientList[i]->m_cSide == m_pClientList[iClientH]->m_cSide))
                            iRet = m_pClientList[i]->write(sw);
                        break;

                    case 4:
                        if ((m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == m_pClientList[iClientH]->m_iPartyID))
                            iRet = m_pClientList[i]->write(sw);
                        break;
                }
            }
    }
    else
    {
        iRet = m_pClientList[iClientH]->write(sw);
        if (m_pClientList[iClientH]->m_iWhisperPlayerIndex == 10000)
        {
            //testcode
            //wsprintf(G_cTxt, "Sending Whisper Msg: %s %d", m_pClientList[iClientH]->m_cWhisperPlayerName, (13 +dwMsgSize));
            //log->info(G_cTxt);
        }
        else
        {
            if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex] != 0 &&
                strcmp(m_pClientList[iClientH]->m_cWhisperPlayerName, m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_cCharName) == 0)
            {
                iRet = m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->write(sw);
                if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_iAdminUserLevel > 0)
                {
                    ZeroMemory(cTemp, sizeof(cTemp));
                    wsprintf(cTemp, "GM Whisper   (%s):\"%s\"\tto GM(%s)", m_pClientList[iClientH]->m_cCharName, msg.c_str(), m_pClientList[iClientH]->m_cWhisperPlayerName);
                }
                else
                {
                    ZeroMemory(cTemp, sizeof(cTemp));
                    wsprintf(cTemp, "Player Whisper   (%s):\"%s\"\tto Player(%s)", m_pClientList[iClientH]->m_cCharName, msg.c_str(), m_pClientList[iClientH]->m_cWhisperPlayerName);
                }
            }
        }
    }
}

void CGame::RequestCreateNewGuildHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char * cp, cGuildName[21], cTxt[120], cData[100]{};
    uint32_t * dwp;
    uint16_t * wp;
    int     iRet;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_bIsCrusadeMode == true) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    cp += 10;
    cp += 10;
    cp += 10;

    memset(cGuildName, 0, sizeof(cGuildName));
    memcpy(cGuildName, cp, 20);
    cp += 20;


    //////////////////////////////////////////////////////////////////////////

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_CREATENEWGUILD;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_REJECT;


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
    return;


    //////////////////////////////////////////////////////////////////////////


    if (m_pClientList[iClientH]->m_iGuildRank != -1)
    {

        wsprintf(cTxt, "(!)Cannot create guild! Already guild member.: CharName(%s)", m_pClientList[iClientH]->m_cCharName);
        log->info(cTxt);
    }
    else
    {
        if ((m_pClientList[iClientH]->m_iLevel < 20) || (m_pClientList[iClientH]->m_iCharisma < 20) ||
            (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) ||
            (m_pClientList[iClientH]->m_bIsHunter == true) ||
            (m_pClientList[iClientH]->m_iIsOnTown == DEF_PK))
        {
            memset(cData, 0, sizeof(cData));

            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_CREATENEWGUILD;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_MSGTYPE_REJECT;


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
        else
        {

            memset(m_pClientList[iClientH]->m_cGuildName, 0, sizeof(m_pClientList[iClientH]->m_cGuildName));
            strcpy(m_pClientList[iClientH]->m_cGuildName, cGuildName);

            memset(m_pClientList[iClientH]->m_cLocation, 0, sizeof(m_pClientList[iClientH]->m_cLocation));
            strcpy(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName);

            GetLocalTime(&SysTime);
            m_pClientList[iClientH]->m_iGuildGUID = (int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());
        }
    }
}

void CGame::RequestDisbandGuildHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char * cp, cGuildName[21], cTxt[120];

    if (m_bIsCrusadeMode == true) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memset(cGuildName, 0, sizeof(cGuildName));

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
    }
}

void CGame::RequestPurchaseItemHandler(int iClientH, char * pItemName, int iNum)
{
    CItem * pItem;
    char * cp, cItemName[21], cData[100];
    short * sp;
    uint32_t * dwp, dwGoldCount, dwItemCount;
    uint16_t * wp, wTempPrice;
    int   i, iRet, iEraseReq, iGoldWeight;
    int   iCost, iCost2, iDiscountRatio, iDiscountCost;
    double dTmp1, dTmp2, dTmp3;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    //if ( (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) &&
    //	 (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, m_pClientList[iClientH]->m_cLocation, 10) != 0) ) return;




    // if (m_pClientList[iClientH]->m_cSide != DEF_NETURAL && m_pClientList[iClientH]->m_bIsOnShop == false ) {
    if (m_pClientList[iClientH]->m_bIsOnShop == false)
    {
        return;
    }


    memset(cData, 0, sizeof(cData));
    memset(cItemName, 0, sizeof(cItemName));



    if (memcmp(pItemName, DEF_ITEMNAME_10ARROWS, 8) == 0)
    {
        strcpy(cItemName, DEF_ITEMNAME_ARROW);
        dwItemCount = 10;
    }
    else if (memcmp(pItemName, DEF_ITEMNAME_100ARROWS, 9) == 0)
    {
        strcpy(cItemName, DEF_ITEMNAME_ARROW);
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
        if (_bInitItemAttr(pItem, cItemName) == false)
        {


            delete pItem;
        }
        else
        {

            if (pItem->m_bIsForSale == false)
            {


                delete pItem;
                return;
            }

            pItem->m_dwCount = dwItemCount;


            pItem->m_sTouchEffectType = DEF_ITET_ID;
            pItem->m_sTouchEffectValue1 = iDice(1, 100000);
            pItem->m_sTouchEffectValue2 = iDice(1, 100000);

#ifdef DEF_LOGTIME
            pItem->m_sTouchEffectValue3 = timeGetTime();
#else 

            SYSTEMTIME SysTime;
            char cTemp[20];

            GetLocalTime(&SysTime); //
            memset(cTemp, 0, sizeof(cTemp));
            //			wsprintf(cTemp, "%d%02d%02d",  (short)SysTime.wMonth, (short)SysTime.wDay,(short) SysTime.wHour);
            wsprintf(cTemp, "%d%02d%", (short)SysTime.wMonth, (short)SysTime.wDay);
            pItem->m_sTouchEffectValue3 = atoi(cTemp);
#endif				
            //v2.19 2002-11-14 ¹°°¡ °¡°Ý °è»ê ºÎºÐ Àü¸éÀü ÀÌ±äÂÊÀº ÂÍ ½Î´Ù... -_-.
#ifdef DEF_V219  
            if (m_iCrusadeWinnerSide == m_pClientList[iClientH]->m_cSide)
            {
                iCost = (int)((float)(pItem->m_wPrice) * 0.9f + 0.5f);

                iCost = iCost * pItem->m_dwCount;
                iCost2 = pItem->m_wPrice * pItem->m_dwCount;
            }
            else
            {

                iCost2 = iCost = pItem->m_wPrice * pItem->m_dwCount;
            }
#else
            iCost = pItem->m_wPrice * pItem->m_dwCount;
#endif


            dwGoldCount = dwGetItemCount(iClientH, "Gold");



            iDiscountRatio = ((m_pClientList[iClientH]->m_iCharisma - 10) / 4);


            //	iDiscountRatio = (m_pClientList[iClientH]->m_iCharisma / 4) -1;
            //	if (iDiscountRatio == 0) iDiscountRatio = 1;

            dTmp1 = (double)(iDiscountRatio);
            dTmp2 = dTmp1 / 100.0f;
            dTmp1 = (double)iCost;
            dTmp3 = dTmp1 * dTmp2;
            iDiscountCost = (int)dTmp3;

#ifdef DEF_V219  
            //¹°°Ç°¡°ÝÀÇ Àý¹ÝÀÌ»óÀº Àý´ë ¾È½ÎÁø´Ù.(¹°°¡+Ä«¸®½º¸¶ Àû¿ëÀ» ÇÏ´õ¶óµµ...)
            if ((iCost - iDiscountCost) <= (iCost2 / 2))
            {
                iDiscountCost = iCost - (iCost2 / 2) + 1;
            }
#else
            if (iDiscountCost >= (iCost / 2)) iDiscountCost = (iCost / 2) - 1;
#endif

            if (dwGoldCount < (DWORD)(iCost - iDiscountCost))
            {


                delete pItem;

                dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
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

                        DeleteClient(iClientH, true, true);
                        return;
                }
                return;
            }

            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
            {

                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;


                dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_ITEMPURCHASED;

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

                wp = (uint16_t *)cp;
                *wp = (iCost - iDiscountCost);
                wTempPrice = (iCost - iDiscountCost);
                cp += 2;

                if (iEraseReq == 1)
                {
                    delete pItem;
                    pItem = 0;
                }


                iRet = m_pClientList[iClientH]->iSendMsg(cData, 48);


                iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - wTempPrice);

                iCalcTotalWeight(iClientH);

                //v1.4 ¸¶À»ÀÇ ÀÚ±Ý¿¡ ´õÇÑ´Ù. 
                m_stCityStatus[m_pClientList[iClientH]->m_cSide].iFunds += wTempPrice;

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
    } // for ??

    if (i <= 1) return;

#ifdef DEF_TAIWANLOG
    _bItemLog(DEF_ITEMLOG_BUY, iClientH, i - 1, pItem);
#endif
}

void CGame::RequestTeleportHandler(int iClientH, char * pData, char * cMapName, int dX, int dY)
{
    char * pBuffer, cTempMapName[21];
    uint32_t * dwp;
    uint16_t * wp;
    char * cp, cDestMapName[11], cDir, cMapIndex, cPrice = 0;
    short * sp, sX, sY;
    int * ip, i, iRet, iSize, iDestX, iDestY, iExH, iMapSide, iTmpMapSide;
    bool    bRet, bIsLockedMapNotify;
    SYSTEMTIME SysTime{};

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;

    if ((m_pClientList[iClientH]->m_cSide == DEF_ELVINE)
        && (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
        && ((pData[0] == '1') || (pData[0] == '3'))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) return;

    if ((m_pClientList[iClientH]->m_cSide == DEF_ARESDEN)
        && (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
        && ((pData[0] == '1') || (pData[0] == '3'))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) return;

    bIsLockedMapNotify = false;

    if (m_pClientList[iClientH]->m_bIsExchangeMode == true)
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;
        _ClearExchangeStatus(iExH);
        _ClearExchangeStatus(iClientH);
    }

    //	if ((memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) && (pData[0] == '1'))
    //		return;

    RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(13, iClientH, DEF_OWNERTYPE_PLAYER,
        m_pClientList[iClientH]->m_sX,
        m_pClientList[iClientH]->m_sY);

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);

    sX = m_pClientList[iClientH]->m_sX;
    sY = m_pClientList[iClientH]->m_sY;

    memset(cDestMapName, 0, sizeof(cDestMapName));
    bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSearchTeleportDest(sX, sY, cDestMapName, &iDestX, &iDestY, &cDir);

    // Crusade
    if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0))
    {

        iMapSide = iGetMapLocationSide(cDestMapName);

        if (iMapSide >= 3) iMapSide -= 2;

        if ((iMapSide != 0) && (m_pClientList[iClientH]->m_cSide == iMapSide))
        {

        }
        else
        {
            iDestX = -1;
            iDestY = -1;
            bIsLockedMapNotify = true;
            memset(cDestMapName, 0, sizeof(cDestMapName));
            strcpy(cDestMapName, m_pClientList[iClientH]->m_cLockedMapName);
        }
    }

    // todo: this should never be called because the server runs all maps
    if ((bRet == true) && (cMapName == 0))
    {
        for (i = 0; i < DEF_MAXMAPS; i++)
            if (m_pMapList[i] != 0)
            {
                if (memcmp(m_pMapList[i]->m_cName, cDestMapName, 10) == 0)
                {
                    m_pClientList[iClientH]->m_sX = iDestX;
                    m_pClientList[iClientH]->m_sY = iDestY;
                    m_pClientList[iClientH]->m_cDir = cDir;
                    m_pClientList[iClientH]->m_cMapIndex = i;
                    memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
                    memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[i]->m_cName, 10);
                    goto RTH_NEXTSTEP;
                }
            }

        m_pClientList[iClientH]->m_sX = iDestX;
        m_pClientList[iClientH]->m_sY = iDestY;
        m_pClientList[iClientH]->m_cDir = cDir;
        memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
        memcpy(m_pClientList[iClientH]->m_cMapName, cDestMapName, 10);

        // !!!!
        m_pClientList[iClientH]->m_bIsOnServerChange = true;
        m_pClientList[iClientH]->m_bIsOnWaitingProcess = true;
        return;
    }
    else
    {

        switch (pData[0])
        {
            case '0':
                // Forced Recall. 


            case '1':


                //if (memcmp(m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_cName, "resurr", 6) == 0) return;

                memset(cTempMapName, 0, sizeof(cTempMapName));
                if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
                {
                    strcpy(cTempMapName, "default");
                }
                else
                {


                    if (m_pClientList[iClientH]->m_iLevel > 80)
                    {
                        if (m_pClientList[iClientH]->m_cSide == DEF_ARESDEN)
                            strcpy(cTempMapName, "aresden");
                        else
                            strcpy(cTempMapName, "elvine");
                    }
                    else
                    {
                        if (m_pClientList[iClientH]->m_cSide == DEF_ARESDEN)
                            strcpy(cTempMapName, "arefarm");
                        else strcpy(cTempMapName, "elvfarm");
                    }
                }

                // Crusade
                if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0))
                {

                    bIsLockedMapNotify = true;
                    memset(cTempMapName, 0, sizeof(cTempMapName));
                    strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
                }

                for (i = 0; i < DEF_MAXMAPS; i++)
                    if (m_pMapList[i] != 0)
                    {
                        if (memcmp(m_pMapList[i]->m_cName, cTempMapName, 10) == 0)
                        {


                            GetMapInitialPoint(i, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cLocation);

                            m_pClientList[iClientH]->m_cMapIndex = i;
                            memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
                            memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[i]->m_cName, 10);
                            goto RTH_NEXTSTEP;
                        }
                    }


                m_pClientList[iClientH]->m_sX = -1;
                m_pClientList[iClientH]->m_sY = -1;

                memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
                memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);

                // !!!
                m_pClientList[iClientH]->m_bIsOnServerChange = true;
                m_pClientList[iClientH]->m_bIsOnWaitingProcess = true;
                return;

            case '2':

                // Crusade
                if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0) && (memcmp(cMapName, "resurr", 6) != 0))
                {

                    dX = -1;
                    dY = -1;
                    bIsLockedMapNotify = true;
                    memset(cTempMapName, 0, sizeof(cTempMapName));
                    strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
                }
                else
                {
                    memset(cTempMapName, 0, sizeof(cTempMapName));
                    strcpy(cTempMapName, cMapName);
                }

                cMapIndex = iGetMapIndex(cTempMapName);
                if (cMapIndex == -1)
                {


                    m_pClientList[iClientH]->m_sX = dX; //-1;
                    m_pClientList[iClientH]->m_sY = dY; //-1;

                    memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
                    memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);

                    // !!!
                    m_pClientList[iClientH]->m_bIsOnServerChange = true;
                    m_pClientList[iClientH]->m_bIsOnWaitingProcess = true;
                    return;
                }

                m_pClientList[iClientH]->m_sX = dX;
                m_pClientList[iClientH]->m_sY = dY;
                m_pClientList[iClientH]->m_cMapIndex = cMapIndex;

                memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
                memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[cMapIndex]->m_cName, 10);
                break;

            case '3':


                if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0) && (memcmp(cMapName, "resurr", 6) != 0))
                {

                    dX = -1;
                    dY = -1;
                    bIsLockedMapNotify = true;
                    memset(cTempMapName, 0, sizeof(cTempMapName));
                    strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
                }
                else
                {
                    memset(cTempMapName, 0, sizeof(cTempMapName));
                    strcpy(cTempMapName, cMapName);
                }

                cMapIndex = iGetMapIndex(cTempMapName);
                if (cMapIndex == -1)
                {


                    m_pClientList[iClientH]->m_sX = dX; //-1;
                    m_pClientList[iClientH]->m_sY = dY; //-1;

                    memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
                    memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);

                    // !!!
                    m_pClientList[iClientH]->m_bIsOnServerChange = true;
                    m_pClientList[iClientH]->m_bIsOnWaitingProcess = true;
                    return;
                }

                m_pClientList[iClientH]->m_sX = dX;
                m_pClientList[iClientH]->m_sY = dY;
                m_pClientList[iClientH]->m_cMapIndex = cMapIndex;

                memset(m_pClientList[iClientH]->m_cMapName, 0, sizeof(m_pClientList[iClientH]->m_cMapName));
                memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[cMapIndex]->m_cName, 10);
                break;
        }
    }

    RTH_NEXTSTEP:;



    m_pClientList[iClientH]->m_bIsPoisoned = false;

    SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);

    iSetSide(iClientH);



    //v2.19 2002-11-14 Àü¸éÀü ½Â¸®ÇÑÂÊÀ¸ °¡°ÝÀ» 10ÇÁ·Î ½Î°Ô ÇØÁØ´Ù.
    if (true == m_pClientList[iClientH]->m_bIsOnShop)
    {
        cPrice = 0;

        if (m_iCrusadeWinnerSide == m_pClientList[iClientH]->m_cSide)
            cPrice = -10;
    }


    // Crusade
    if (bIsLockedMapNotify == true) SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LOCKEDMAP, m_pClientList[iClientH]->m_iLockedMapTime, 0, 0, m_pClientList[iClientH]->m_cLockedMapName);

    pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
    memset(pBuffer, 0, DEF_MSGBUFFERSIZE + 1);

    m_pClientList[iClientH]->m_bInitComplete = true; //Change TP Bug/Hack Init Data


    dwp = (uint32_t *)(pBuffer + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_INITDATA;
    wp = (uint16_t *)(pBuffer + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);


    if (m_pClientList[iClientH]->m_bIsObserverMode == false)
        bGetEmptyPosition(&m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cMapIndex);
    else GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY);


    sp = (short *)cp;
    *sp = iClientH;		// Player ObjectID
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sX - 14 - 5;
    cp += 2;

    sp = (short *)cp;
    *sp = m_pClientList[iClientH]->m_sY - 12 - 5;
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
    // v1.4 ApprColor
    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iApprColor;
    cp += 4;

    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iStatus;
    cp += 4;//+2


    memcpy(cp, m_pClientList[iClientH]->m_cMapName, 10);
    cp += 10;


    memcpy(cp, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10);
    cp += 10;


    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true)
        *cp = 1;
    else *cp = m_cDayOrNight;
    cp++;


    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true)
        *cp = 0;
    else *cp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus;
    cp++;

    // v1.4 Contribution
    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iContribution;
    cp += 4;


    if (m_pClientList[iClientH]->m_bIsObserverMode == false)
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH,
            DEF_OWNERTYPE_PLAYER,
            m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY);
    }

    // v1.41
    *cp = (char)m_pClientList[iClientH]->m_bIsObserverMode;
    cp++;

    // v1.41 
    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iRating;
    cp += 4;

    // v1.44
    ip = (int *)cp;
    *ip = m_pClientList[iClientH]->m_iHP;
    cp += 4;


#ifdef DEF_V219
    * cp = cPrice;
    cp += 1;
#else 
    * cp = 0;
    cp += 1;
#endif


    iSize = iComposeInitMapData(m_pClientList[iClientH]->m_sX - 10, m_pClientList[iClientH]->m_sY - 7, iClientH, cp);
    cp += iSize;



    iRet = m_pClientList[iClientH]->iSendMsg(pBuffer, 46 + iSize + 4 + 4 + 1 + 4 + 4 + 1 + 2 + 3); // v2.183 // v1.41
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:

            DeleteClient(iClientH, true, true);
            if (pBuffer != 0) delete pBuffer;
            return;
    }

    if (pBuffer != 0) delete pBuffer;


    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);



    iSetSide(iClientH);



    m_pClientList[iClientH]->m_bIsWarLocation = false;
    m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 0;




    if ((DEF_ARESDEN == m_pClientList[iClientH]->m_cSide) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
        && (m_pClientList[iClientH]->m_iAdminUserLevel < 1))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;


        SetForceRecallTime(iClientH);
    }

    else if ((DEF_ELVINE == m_pClientList[iClientH]->m_cSide) &&
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;


        SetForceRecallTime(iClientH);

    }
    else if ((DEF_NETURAL == m_pClientList[iClientH]->m_cSide) &&
        (m_bIsCrusadeMode == true) && ((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvine") == 0)
            || (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresden") == 0))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
    }

    // v2.181 2002-10-24

    iMapSide = iGetMapLocationSide(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);

    if (iMapSide >= 3) iTmpMapSide = iMapSide - 2;
    else iTmpMapSide = iMapSide;

    m_pClientList[iClientH]->m_bIsInBuilding = false;


    if ((m_pClientList[iClientH]->m_cSide != iTmpMapSide) && (iMapSide != 0))
    {


        if ((iMapSide <= 2) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
            && (m_pClientList[iClientH]->m_cSide != DEF_NETURAL))
        {
            m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
            m_pClientList[iClientH]->m_bIsWarLocation = true;
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
            m_pClientList[iClientH]->m_bIsInBuilding = true;
        }
    }




#ifndef DEF_GUILDWARMODE
    else if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == true) &&
        (m_iFightzoneNoForceRecall == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {

        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
        m_pClientList[iClientH]->m_bIsWarLocation = true;


        GetLocalTime(&SysTime);
        m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2 * 20 * 60 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 2 * 20;
    }
#endif 
    else if (((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arejail", 7) == 0) ||
        (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvjail", 7) == 0))
        && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        m_pClientList[iClientH]->m_bIsWarLocation = true;
        m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();

        // v2.17 2002-7-15 
        if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 5;

        }
        else if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 20 * 5)
        {
            m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 20 * 5;  // 5ºÐ
        }
    }



    if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, 0, 0, 0);
        wsprintf(G_cTxt, "(!) Game Server Force Recall Time  %d (%d)min", m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall / 20);
        log->info(G_cTxt);
    }



    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SAFEATTACKMODE, 0, 0, 0, 0);
    // v1.3
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, 0, 0, 0);
    // V1.3
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMPOSLIST, 0, 0, 0, 0);
    // v1.4 
    _SendQuestContents(iClientH);
    _CheckQuestEnvironment(iClientH);

    // v1.432
    if (m_pClientList[iClientH]->m_iSpecialAbilityTime == 0)
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYENABLED, 0, 0, 0, 0);


    if (m_bIsCrusadeMode == true)
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

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, 0, 0, -1);
        }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, 0, 0);
    }
    else
    {

        if (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID)
        {
            m_pClientList[iClientH]->m_iCrusadeDuty = 0;
            m_pClientList[iClientH]->m_iConstructionPoint = 0;
        }
        else if ((m_pClientList[iClientH]->m_dwCrusadeGUID != 0) && (m_pClientList[iClientH]->m_dwCrusadeGUID != m_dwCrusadeGUID))
        {

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, 0, 0, 0, -1);
            m_pClientList[iClientH]->m_iWarContribution = 0;
            m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
        }
    }

    // v1.42

    if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 5) == 0)
    {
        wsprintf(G_cTxt, "Char(%s)-Enter(%s) Observer(%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_bIsObserverMode);
        log->info(G_cTxt);
    }

    // Crusade
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, -1, 0);

    // v2.15
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);



    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);
}

void CGame::RequestStudyMagicHandler(int iClientH, char * pName, bool bIsPurchase)
{
    char * cp, cMagicName[31], cData[100];
    uint32_t * dwp, dwGoldCount;
    uint16_t * wp;
    int * ip, iReqInt, iCost, iRet;
    bool bMagic = true;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;


    memset(cData, 0, sizeof(cData));

    memset(cMagicName, 0, sizeof(cMagicName));
    memcpy(cMagicName, pName, 30);

    iRet = _iGetMagicNumber(cMagicName, &iReqInt, &iCost);
    if (iRet == -1)
    {


    }
    else
    {
        if (bIsPurchase == true)
        {
            if (m_pMagicConfigList[iRet]->m_iGoldCost < 0) bMagic = false;
            dwGoldCount = dwGetItemCount(iClientH, "Gold");
            if ((DWORD)iCost > dwGoldCount)  bMagic = false;

            if (m_pClientList[iClientH]->m_bIsOnTower == false) bMagic = false;
        }


        if (m_pClientList[iClientH]->m_cMagicMastery[iRet] != 0) return;

        if ((iReqInt <= m_pClientList[iClientH]->m_iInt) && (bMagic == true))
        {


            if (bIsPurchase == true) SetItemCount(iClientH, "Gold", dwGoldCount - iCost);


            iCalcTotalWeight(iClientH);


            m_pClientList[iClientH]->m_cMagicMastery[iRet] = 1;


            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_MAGICSTUDYSUCCESS;

            cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);


            *cp = iRet;
            cp++;

            memcpy(cp, cMagicName, 30);
            cp += 30;


            iRet = m_pClientList[iClientH]->iSendMsg(cData, 37);

#ifdef DEF_TAIWANLOG 
            _bItemLog(DEF_ITEMLOG_MAGICLEARN, iClientH, cMagicName, 0);
#endif
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


            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
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

                    DeleteClient(iClientH, true, true);
                    return;
            }
        }
    }
}

void CGame::StateChangeHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char * cp, cStateChange1, cStateChange2, cStateChange3;
    char cStr, cVit, cDex, cInt, cMag, cChar;
    char cStateTxt[512];
    int iOldStr, iOldVit, iOldDex, iOldInt, iOldMag, iOldChar;
    int iTotalSetting = 0;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
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


    wsprintf(G_cTxt, "(*) Char(%s) Str(%d) Vit(%d) Dex(%d) Int(%d) Mag(%d) Chr(%d) ", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iStr, m_pClientList[iClientH]->m_iVit, m_pClientList[iClientH]->m_iDex, m_pClientList[iClientH]->m_iInt, m_pClientList[iClientH]->m_iMag, m_pClientList[iClientH]->m_iCharisma);
    log->info(G_cTxt);

    if (!bChangeState(cStateChange1, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }
    if (!bChangeState(cStateChange2, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }
    if (!bChangeState(cStateChange3, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }

    if (m_pClientList[iClientH]->m_iGuildRank == 0)
    {
        if (m_pClientList[iClientH]->m_iCharisma - cChar < 20)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
            return;
        }
    }

    if (iOldStr + iOldVit + iOldDex + iOldInt + iOldMag + iOldChar != ((DEF_PLAYERMAXLEVEL - 1) * 3 + 70))
        return;


    if (cStr < 0 || cVit < 0 || cDex < 0 || cInt < 0 || cMag < 0 || cChar < 0
        || cStr + cVit + cDex + cInt + cMag + cChar != 3)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }


    if ((m_pClientList[iClientH]->m_iStr - cStr > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iStr - cStr < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }

    if ((m_pClientList[iClientH]->m_iDex - cDex > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iDex - cDex < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }

    if ((m_pClientList[iClientH]->m_iInt - cInt > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iInt - cInt < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }

    if ((m_pClientList[iClientH]->m_iVit - cVit > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iVit - cVit < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }

    if ((m_pClientList[iClientH]->m_iMag - cMag > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iMag - cMag < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }

    if ((m_pClientList[iClientH]->m_iCharisma - cChar > DEF_CHARPOINTLIMIT)
        || (m_pClientList[iClientH]->m_iCharisma - cChar < 10))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
        return;
    }

    if (m_pClientList[iClientH]->m_iLU_Point < 0) m_pClientList[iClientH]->m_iLU_Point = 0;


    //m_pClientList[iClientH]->m_iLU_Point += 3;

    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft--;

    m_pClientList[iClientH]->m_iStr -= cStr;
    m_pClientList[iClientH]->m_iVit -= cVit;
    m_pClientList[iClientH]->m_iDex -= cDex;
    m_pClientList[iClientH]->m_iInt -= cInt;
    //2003-04-22Ã€Ã Â¸Â¶Â¹Ã½Ã€Â» Â»Ã¨ÃÂ¦ Â½ÃƒÃ…Â²Â´Ã™.... Ã€ÃŽÃ†Â®Â°Â¡ Â³Â»Â·ÃÂ°Â¡Â°Ã­ Â³ÂªÂ¼Â­ ÂµÂ¹Â¾Ã†Â°Â¡Â¾ÃŸ Ã‡Ã‘Â´Ã™...
    //if(cInt > 0)
    //bCheckMagicInt(iClientH);
    m_pClientList[iClientH]->m_iMag -= cMag;
    m_pClientList[iClientH]->m_iCharisma -= cChar;

    ZeroMemory(cStateTxt, sizeof(cStateTxt));

    wsprintf(cStateTxt, "Stat Change STR(%d->%d)VIT(%d->%d)DEX(%d->%d)INT(%d->%d)MAG(%d->%d)CHARISMA(%d->%d)",
        iOldStr, m_pClientList[iClientH]->m_iStr,
        iOldVit, m_pClientList[iClientH]->m_iVit,
        iOldDex, m_pClientList[iClientH]->m_iDex,
        iOldInt, m_pClientList[iClientH]->m_iInt,
        iOldMag, m_pClientList[iClientH]->m_iMag,
        iOldChar, m_pClientList[iClientH]->m_iCharisma
    );

    //_bCustomLog(DEF_ITEMLOG_CUSTOM,iClientH,NULL,cStateTxt);

    //2003-04-22Ã€Ã Â½ÂºÃ…Â³Ã€Â» Â³Â»Â·ÃÂ°Â¡Â°Ã” Ã‡Ã‘Â´Ã™... 
    //bCheckSkillState(iClientH);

    //Â¼ÂºÂ°Ã¸..!!!
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_SUCCESS, 0, 0, 0, 0);
}

void CGame::LevelUpSettingsHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char * cp, cStr, cVit, cDex, cInt, cMag, cChar;
    int iTotalSetting = 0;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_iLU_Point <= 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
        return;
    }

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    cStr = *cp;
    cp++;

    cVit = *cp;
    cp++;

    cDex = *cp;
    cp++;

    cInt = *cp;
    cp++;

    cMag = *cp;
    cp++;

    cChar = *cp;
    cp++;

    if ((cStr + cVit + cDex + cInt + cMag + cChar) > m_pClientList[iClientH]->m_iLU_Point)
    { // -3
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
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

    //(Â?Â?ÂºÂ? Ã?Â?Â?ÂºÂ?Âª + Â?Â?ÂºÂ?Â?Ã? Ã?Ã?Ã?ÃŽÃ?Â? > Â?Â?ÂºÂ?Â?Ã? Ã?Â?Â?ÂºÂ?Âª Ã?Â?Â?Ã?Ã?Â?)Â?Ã? ÂºÃ?Ã?Â?Â?Ã?Ã?ÃŒÂ?Ã?.. ÃƒÂ?Â?Â? ÂºÃ?Â?Â?.. Â?Â?ÂºÂ?Â?Ã? Ã?Ã?Ã?ÃŽÃ?Â?Â?Â? Ã?Â?Â?Ã?Ã?Â?Â?ÃŽ Â?Â?ÃƒÃŸÂ?Ã? ÃƒÂ?Â?Â? ÂºÃ?Â?Â?..
    if (iTotalSetting + m_pClientList[iClientH]->m_iLU_Point > ((m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70))
    {
        m_pClientList[iClientH]->m_iLU_Point = /*m_cLU_Str ÃƒÃŠÂ?Ã?Â?Âª*/3 + (m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70 - iTotalSetting;

        //iTotalSettingÂ?ÂªÃ?ÃŒ Ã?ÃŸÂ?Ã?ÂµÃ? Â?Ã?Â?Ã?Â?Ã?...
        if (m_pClientList[iClientH]->m_iLU_Point < 0)
            m_pClientList[iClientH]->m_iLU_Point = 0;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
        return;
    }

    //(Â?Â?ÂºÂ? Ã?Â?Â?ÂºÂ?Âª + Â?Â?ÂºÂ?Â?Ã? Â?ÃƒÃ?Â? Ã?Ã?Ã?ÃŽÃ?Â? Ã?D > Â?Â?ÂºÂ?Â?Ã? Ã?Â?Â?ÂºÂ?Âª Ã?Â?Â?Ã?Ã?Â?)Ã?ÃŒÂ?Ã? ÃƒÂ?Â?Â? ÂºÃ?Â?Â?..
    if (iTotalSetting + (cStr + cVit + cDex + cInt + cMag + cChar)
        > ((m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70))
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
        return;
    }

    m_pClientList[iClientH]->m_iLU_Point = m_pClientList[iClientH]->m_iLU_Point - (cStr + cVit + cDex + cInt + cMag + cChar);

    // Â?Ã?Â?Ã?Â?Â? Â?Ã?Ã?Â?Â?Ã? Â?ÂªÃ?Â? Ã?Ã?Â?Ã?Ã?Ã?Â?Ã?.
    m_pClientList[iClientH]->m_iStr += cStr;
    m_pClientList[iClientH]->m_iVit += cVit;
    m_pClientList[iClientH]->m_iDex += cDex;
    m_pClientList[iClientH]->m_iInt += cInt;
    m_pClientList[iClientH]->m_iMag += cMag;
    m_pClientList[iClientH]->m_iCharisma += cChar;

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SETTING_SUCCESS, 0, 0, 0, 0);
}

void CGame::FightzoneReserveHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char cData[100];
    int iFightzoneNum, * ip, iEnableReserveTime;
    uint32_t * dwp, dwGoldCount;
    uint16_t * wp, wResult;
    int     iRet, iResult = 1, iCannotReserveDay;
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    GetLocalTime(&SysTime);


    iEnableReserveTime = 2 * 20 * 60 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 5 * 20;

    dwGoldCount = dwGetItemCount(iClientH, "Gold");

    ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);

    iFightzoneNum = *ip;


    if ((iFightzoneNum < 1) || (iFightzoneNum > DEF_MAXFIGHTZONE)) return;


    //if ((iFightzoneNum >= 1) && (iFightzoneNum <= 4)) return;






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


#ifdef DEF_TAIWANLOG
        _bItemLog(DEF_ITEMLOG_RESERVEFIGZONE, iClientH, (char *)NULL, NULL);
#endif

        if (SysTime.wHour % 2)	m_pClientList[iClientH]->m_iReserveTime += 1;
        else					m_pClientList[iClientH]->m_iReserveTime += 2;
        wsprintf(G_cTxt, "(*) Reserve FIGHTZONETICKET : Char(%s) TICKENUMBER (%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iReserveTime);
        log->info(G_cTxt);
        log->info(G_cTxt);

        m_pClientList[iClientH]->m_iFightZoneTicketNumber = 50;
        iResult = 1;
    }

    memset(cData, 0, sizeof(cData));

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_FIGHTZONE_RESERVE;

    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
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

            DeleteClient(iClientH, true, true);
            return;
    }
}

void CGame::RequestCivilRightHandler(int iClientH, char * pData)
{
    char * cp, cData[100]{};
    uint32_t * dwp;
    uint16_t * wp, wResult;
    int  iRet;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;


    if (m_pClientList[iClientH]->m_cSide != DEF_NETURAL) wResult = 0;
    else wResult = 1;


    if (m_pClientList[iClientH]->m_iLevel < 5) wResult = 0;

    if (wResult == 1)
    {

        memset(m_pClientList[iClientH]->m_cLocation, 0, sizeof(m_pClientList[iClientH]->m_cLocation));
        strcpy(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName);

#ifdef DEF_TAIWANLOG
        _bItemLog(DEF_ITEMLOG_APPLY, iClientH, (char)0, 0);
#endif
    }



    if (memcmp(m_pClientList[iClientH]->m_cLocation, "aresden", 7) == 0)
        m_pClientList[iClientH]->m_cSide = DEF_ARESDEN;

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvine", 6) == 0)
        m_pClientList[iClientH]->m_cSide = DEF_ELVINE;

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_CIVILRIGHT;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
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

            DeleteClient(iClientH, true, true);
            return;
    }


    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);


    if (m_pClientList[iClientH]->m_iPartyID != 0) RequestDeletePartyHandler(iClientH);


    CheckSpecialEventThirdYear(iClientH);


    RequestHuntmode(iClientH);

}

void CGame::RequestRetrieveItemHandler(int iClientH, char * pData)
{
    char * cp, cBankItemIndex, cMsg[100];
    int i, j, iRet, iItemWeight;
    uint32_t * dwp;
    uint16_t * wp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    cBankItemIndex = *cp;


    if (m_pClientList[iClientH]->m_bIsOnWarehouse == false) return;


    if ((cBankItemIndex < 0) || (cBankItemIndex >= DEF_MAXBANKITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] == 0)
    {

        memset(cMsg, 0, sizeof(cMsg));

        dwp = (uint32_t *)(cMsg + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_RETRIEVEITEM;
        wp = (uint16_t *)(cMsg + DEF_INDEX2_MSGTYPE);
        *wp = DEF_MSGTYPE_REJECT;

        iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 8);
    }
    else
    {

        /*
        if ( (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
        (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) ) {
        //iItemWeight = m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_wWeight * m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount;
        iItemWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex], m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);
        }
        else iItemWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex], 1); //m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_wWeight;
        */
        // v1.432
        iItemWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex], m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);

        if ((iItemWeight + m_pClientList[iClientH]->m_iCurWeightLoad) > _iCalcMaxLoad(iClientH))
        {


            memset(cMsg, 0, sizeof(cMsg));


            dwp = (uint32_t *)(cMsg + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (uint16_t *)(cMsg + DEF_INDEX2_MSGTYPE);
            *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

            iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 6);
            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:

                    DeleteClient(iClientH, true, true);
                    break;
            }
            return;
        }

        //!!!
        if ((m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
            (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW))
        {

            for (i = 0; i < DEF_MAXITEMS; i++)
                if ((m_pClientList[iClientH]->m_pItemList[i] != 0) &&
                    (m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType) &&
                    (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cName, 20) == 0))
                {

                    // v1.41 !!! 
                    SetItemCount(iClientH, i, m_pClientList[iClientH]->m_pItemList[i]->m_dwCount + m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);


                    delete m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
                    m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = 0;


                    if (m_pClientList[iClientH]->m_pItemList[i] != 0)
                    {
                        if (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum == 650)
                            _bItemLog(DEF_ITEMLOG_RETRIEVE, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[i], true);
                        else
                            _bItemLog(DEF_ITEMLOG_RETRIEVE, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[i]);
                    }



                    for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++)
                    {
                        if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != 0) && (m_pClientList[iClientH]->m_pItemInBankList[j] == 0))
                        {
                            m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

                            m_pClientList[iClientH]->m_pItemInBankList[j + 1] = 0;
                        }
                    }


                    memset(cMsg, 0, sizeof(cMsg));

                    dwp = (uint32_t *)(cMsg + DEF_INDEX4_MSGID);
                    *dwp = MSGID_RESPONSE_RETRIEVEITEM;
                    wp = (uint16_t *)(cMsg + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_MSGTYPE_CONFIRM;

                    cp = (char *)(cMsg + DEF_INDEX2_MSGTYPE + 2);
                    *cp = cBankItemIndex;
                    cp++;
                    *cp = i;
                    cp++;


                    iCalcTotalWeight(iClientH);

                    m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);


                    iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 8);




                    m_pClientList[iClientH]->m_bIsBankModified = true;

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


            goto RRIH_NOQUANTITY;
        }
        else
        {
            RRIH_NOQUANTITY:;

            for (i = 0; i < DEF_MAXITEMS; i++)
                if (m_pClientList[iClientH]->m_pItemList[i] == 0)
                {


                    m_pClientList[iClientH]->m_pItemList[i] = m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
                    // v1.3 1-27 12:22
                    m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
                    m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

                    m_pClientList[iClientH]->m_bIsItemEquipped[i] = false;

                    m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = 0;


                    if (m_pClientList[iClientH]->m_pItemList[i] != 0)
                    {
                        if (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum == 650)
                            _bItemLog(DEF_ITEMLOG_RETRIEVE, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[i], true);
                        else
                            _bItemLog(DEF_ITEMLOG_RETRIEVE, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[i]);
                    }



                    for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++)
                    {
                        if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != 0) && (m_pClientList[iClientH]->m_pItemInBankList[j] == 0))
                        {
                            m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

                            m_pClientList[iClientH]->m_pItemInBankList[j + 1] = 0;
                        }
                    }


                    memset(cMsg, 0, sizeof(cMsg));

                    dwp = (uint32_t *)(cMsg + DEF_INDEX4_MSGID);
                    *dwp = MSGID_RESPONSE_RETRIEVEITEM;
                    wp = (uint16_t *)(cMsg + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_MSGTYPE_CONFIRM;

                    cp = (char *)(cMsg + DEF_INDEX2_MSGTYPE + 2);
                    *cp = cBankItemIndex;
                    cp++;
                    *cp = i;
                    cp++;


                    iCalcTotalWeight(iClientH);


                    m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);



                    iRet = m_pClientList[iClientH]->iSendMsg(cMsg, 8);


                    m_pClientList[iClientH]->m_bIsBankModified = true;

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

            memset(cMsg, 0, sizeof(cMsg));

            dwp = (uint32_t *)(cMsg + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_RETRIEVEITEM;
            wp = (uint16_t *)(cMsg + DEF_INDEX2_MSGTYPE);
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

            DeleteClient(iClientH, true, true);
            return;
    }
}

void CGame::RequestFullObjectData(int iClientH, char * pData)
{
    uint32_t * dwp;
    uint16_t * wp, wObjectID;
    char * cp, cData[100];
    short * sp, sX, sY;
    int	 iTemp, iTemp2;
    int * ip, iRet;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    wp = (uint16_t *)(pData + DEF_INDEX2_MSGTYPE);
    wObjectID = *wp;

    memset(cData, 0, sizeof(cData));
    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_EVENT_MOTION;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTSTOP;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    if (wObjectID < 10000)
    {


        if ((wObjectID == 0) || (wObjectID >= DEF_MAXCLIENTS)) return;
        if (m_pClientList[wObjectID] == 0) return;

        wp = (uint16_t *)cp;
        *wp = wObjectID;			// ObjectID
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
        //v1.4 ApprColor
        ip = (int *)cp;
        *ip = m_pClientList[wObjectID]->m_iApprColor;
        cp += 4;

        ip = (int *)cp;



        iTemp = m_pClientList[wObjectID]->m_iStatus;
        iTemp = 0x0FFFFFFF & iTemp;
        //sTemp2 = (short)iGetPlayerABSStatus(wObjectID); // 2002-11-14
        iTemp2 = (int)iGetPlayerABSStatus(wObjectID, iClientH);
        iTemp = (iTemp | (iTemp2 << 28));

        *ip = iTemp;
        cp += 4;

        if (m_pClientList[wObjectID]->m_bIsKilled == true) // v1.4
            *cp = 1;
        else *cp = 0;
        cp++;

        ip = (int *)cp;
        *ip = m_pClientList[wObjectID]->m_iVit * 3 + m_pClientList[wObjectID]->m_iLevel * 2 + m_pClientList[wObjectID]->m_iStr / 2;
        cp += 4;

        ip = (int *)cp;
        *ip = m_pClientList[wObjectID]->m_iHP;
        cp += 4;

        //iMaxPoint = m_iVit*3 + m_iLevel*2 + m_iStr/2;
        //Change HP Bar

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 43 + 8); // v1.4
    }
    else
    {


        if (((wObjectID - 10000) == 0) || ((wObjectID - 10000) >= DEF_MAXNPCS)) return;
        if (m_pNpcList[wObjectID - 10000] == 0) return;

        wp = (uint16_t *)cp;
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

        iTemp = m_pNpcList[wObjectID]->m_iStatus;
        iTemp = 0x0FFFFFFF & iTemp;

        iTemp2 = iGetNpcRelationship(wObjectID, iClientH);
        iTemp = (iTemp | (iTemp2 << 28));
        *ip = iTemp;
        cp += 4;

        if (m_pNpcList[wObjectID]->m_bIsKilled == true) // v1.4
            *cp = 1;
        else *cp = 0;
        cp++;

        ip = (int *)cp;
        *ip = m_pNpcList[wObjectID]->m_iMaxHP;
        cp += 4;

        ip = (int *)cp;
        *ip = m_pNpcList[wObjectID]->m_iHP;
        cp += 4;

        //Change HP Bar

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 25 + 2 + 8 + 8); // v1.4
    }

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

void CGame::CheckAndNotifyPlayerConnection(int iClientH, char * pMsg, uint32_t dwSize)
{
    char   seps[] = "= \t\n";
    char * token, * cp, cName[11], cBuff[256], cTemp[120];
    CStrTok * pStrTok;
    int i;
    uint16_t * wp;

    if (m_pClientList[iClientH] == 0) return;
    if (dwSize <= 0) return;

    memset(cTemp, 0, sizeof(cTemp));
    memset(cName, 0, sizeof(cName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == 0)
    {
        delete pStrTok;
        return;
    }

    if (strlen(token) > 10)
        memcpy(cName, token, 10);
    else memcpy(cName, token, strlen(token));


    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(cName, m_pClientList[i]->m_cCharName, 10) == 0))
        {




            if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
            {
                // v2.14 /fi ±â´É 
                cp = (char *)cTemp;
                memcpy(cp, m_pClientList[i]->m_cMapName, 10);
                cp += 10;

                wp = (uint16_t *)cp;
                *wp = (WORD)m_pClientList[i]->m_sX;
                cp += 2;

                wp = (uint16_t *)cp;
                *wp = (WORD)m_pClientList[i]->m_sY;
                cp += 2;
            }

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERONGAME, 0, 0, 0, m_pClientList[i]->m_cCharName, 0, 0, 0, 0, 0, 0, cTemp);

            delete pStrTok;
            return;
        }

    //SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
    delete pStrTok;
}

void CGame::ToggleWhisperPlayer(int iClientH, char * pMsg, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if (dwMsgSize <= 0) return;

    memset(cName, 0, sizeof(cName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == 0)
    {

        m_pClientList[iClientH]->m_iWhisperPlayerIndex = -1;
        memset(m_pClientList[iClientH]->m_cWhisperPlayerName, 0, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_WHISPERMODEOFF, 0, 0, 0, cName);
        m_pClientList[iClientH]->m_bIsCheckingWhisperPlayer = false;
        memset(m_pClientList[iClientH]->m_cWhisperPlayerName, 0, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
    }
    else
    {
        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));

        m_pClientList[iClientH]->m_iWhisperPlayerIndex = -1;
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {

                if (i == iClientH)
                {
                    delete pStrTok;
                    return;
                }
                //ÀÎµ¦½º¸¦ ÇÒ´ç 
                m_pClientList[iClientH]->m_iWhisperPlayerIndex = i;
                memset(m_pClientList[iClientH]->m_cWhisperPlayerName, 0, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
                strcpy(m_pClientList[iClientH]->m_cWhisperPlayerName, cName);
                break;
            }


        if (m_pClientList[iClientH]->m_iWhisperPlayerIndex == -1)
        {
            m_pClientList[iClientH]->m_bIsCheckingWhisperPlayer = true;

            memset(m_pClientList[iClientH]->m_cWhisperPlayerName, 0, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
            strcpy(m_pClientList[iClientH]->m_cWhisperPlayerName, cName);
        }
        else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_WHISPERMODEON, 0, 0, 0, m_pClientList[iClientH]->m_cWhisperPlayerName);
        delete pStrTok;
        return;
    }

    delete pStrTok;
}


void CGame::SetPlayerProfile(int iClientH, char * pMsg, uint32_t dwMsgSize)
{
    char cTemp[256];
    int i;


    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize - 7) <= 0) return;

    memset(cTemp, 0, sizeof(cTemp));
    memcpy(cTemp, (pMsg + 7), dwMsgSize - 7);


    for (i = 0; i < 256; i++)
        if (cTemp[i] == ' ') cTemp[i] = '_';


    cTemp[255] = 0;

    memset(m_pClientList[iClientH]->m_cProfile, 0, sizeof(m_pClientList[iClientH]->m_cProfile));
    strcpy(m_pClientList[iClientH]->m_cProfile, cTemp);
}

void CGame::GetPlayerProfile(int iClientH, char * pMsg, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256], cBuff2[500];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    memset(cName, 0, sizeof(cName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {

                memset(cBuff2, 0, sizeof(cBuff2));
                //v2.19 2002-12-18 profileÀÌ ¾Ê³ª¿À´Â ¹ö±× ¼öÁ¤ 
                wsprintf(cBuff2, "%s Profile:%s", cName, m_pClientList[i]->m_cProfile); // v2.04
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERPROFILE, 0, 0, 0, cBuff2);

                delete pStrTok;
                return;
            }

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
    }

    delete pStrTok;
    return;
}

void CGame::ShutUpPlayer(int iClientH, char * pMsg, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256];
    CStrTok * pStrTok;
    int i, iTime;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }


    memset(cName, 0, sizeof(cName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));


        token = pStrTok->pGet();
        if (token == 0)
            iTime = 0;
        else iTime = atoi(token);

        if (iTime < 0) iTime = 0;

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {
                m_pClientList[i]->m_iTimeLeft_ShutUp = iTime * 20;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERSHUTUP, iTime, 0, 0, cName);
                SendNotifyMsg(NULL, i, DEF_NOTIFY_PLAYERSHUTUP, iTime, 0, 0, cName);

                // Admin Log
                wsprintf(G_cTxt, "GM Order(%s): Shutup PC(%s) (%d)Min", m_pClientList[iClientH]->m_cCharName,
                    m_pClientList[i]->m_cCharName, iTime);

                delete pStrTok;
                return;
            }

        delete pStrTok;
        return;
    }

    delete pStrTok;
    return;
}

void CGame::SetPlayerReputation(int iClientH, char * pMsg, char cValue, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH]->m_iLevel < 15) return;

    if ((m_pClientList[iClientH]->m_iTimeLeft_Rating != 0) || (m_pClientList[iClientH]->m_iPKCount != 0))
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTRATING, m_pClientList[iClientH]->m_iTimeLeft_Rating, 0, 0, 0);
        return;
    }
    else if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
    { // 2002-11-15

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTRATING, 0, 0, 0, 0);
        return;
    }

    memset(cName, 0, sizeof(cName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pMsg, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {

                if (i != iClientH)
                {

                    if (cValue == 0)
                        m_pClientList[i]->m_iRating--;
                    else if (cValue == 1)
                        m_pClientList[i]->m_iRating++;

                    if (m_pClientList[i]->m_iRating > 10000)  m_pClientList[i]->m_iRating = 10000;
                    if (m_pClientList[i]->m_iRating < -10000) m_pClientList[i]->m_iRating = -10000;

                    m_pClientList[iClientH]->m_iTimeLeft_Rating = 20 * 60;

                    SendNotifyMsg(NULL, i, DEF_NOTIFY_RATINGPLAYER, cValue, 0, 0, cName);
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RATINGPLAYER, cValue, 0, 0, cName);

                    delete pStrTok;
                    return;
                }
            }

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
    }

    delete pStrTok;
    return;
}

void CGame::ReqCreatePortionHandler(int iClientH, char * pData)
{
    uint32_t * dwp;
    uint16_t * wp;
    char * cp, cI[6]{}, cPortionName[21], cData[120];
    int    iRet, i, j, iEraseReq, iSkillLimit, iSkillLevel, iResult, iDifficulty;
    short * sp, sItemIndex[6]{}, sTemp;
    short  sItemNumber[6]{}, sItemArray[12]{};
    bool   bDup, bFlag;
    CItem * pItem;

    if (m_pClientList[iClientH] == 0) return;
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
        if ((cI[i] >= 0) && (m_pClientList[iClientH]->m_pItemList[cI[i]] == 0)) return;
    }

    for (i = 0; i < 6; i++)
        if (cI[i] >= 0)
        {

            bDup = false;
            for (j = 0; j < 6; j++)
                if (sItemIndex[j] == cI[i])
                {

                    sItemNumber[j]++;
                    bDup = true;
                }
            if (bDup == false)
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
            if (m_pClientList[iClientH]->m_pItemList[sItemIndex[i]] == 0) return;

            if (m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_dwCount < sItemNumber[i]) return;
        }


    bFlag = true;
    while (bFlag == true)
    {
        bFlag = false;
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
                    bFlag = true;
                }
            }
    }

    //testcode
    /*
    char cTemp[120];
    char cTxt1[120];
    char cTxt2[120];
    memset(cTxt1, 0, sizeof(cTxt1));
    memset(cTxt2, 0, sizeof(cTxt2));
    for (i = 0; i < 6; i++) {
    memset(cTemp, 0, sizeof(cTemp));
    if (sItemIndex[i] != -1)
    wsprintf(cTemp, "(%d) ", m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_sIDnum);
    else strcpy(cTemp, "(*)");
    strcat(cTxt1, cTemp);

    memset(cTemp, 0, sizeof(cTemp));
    wsprintf(cTemp, "(%d) ", sItemNumber[i]);
    strcat(cTxt2, cTemp);
    }
    log->info(cTxt1);
    log->info(cTxt2);
    */


    j = 0;
    for (i = 0; i < 6; i++)
    {
        if (sItemIndex[i] != -1)
            sItemArray[j] = m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_sIDnum;
        else sItemArray[j] = sItemIndex[i];
        sItemArray[j + 1] = sItemNumber[i];
        j += 2;
    }

    //testcode
    /*
    char cTemp[120], cTxt1[120];
    memset(cTxt1, 0, sizeof(cTxt1));
    for (i = 0; i < 12; i++) {
    memset(cTemp, 0, sizeof(cTemp));
    wsprintf(cTemp, "%d", sItemArray[i]);
    strcat(cTxt1, cTemp);
    }
    log->info(cTxt1);
    */


    memset(cPortionName, 0, sizeof(cPortionName));

    for (i = 0; i < DEF_MAXPORTIONTYPES; i++)
        if (m_pPortionConfigList[i] != 0)
        {
            bFlag = false;
            for (j = 0; j < 12; j++)
                if (m_pPortionConfigList[i]->m_sArray[j] != sItemArray[j]) bFlag = true;

            if (bFlag == false)
            {

                memset(cPortionName, 0, sizeof(cPortionName));
                memcpy(cPortionName, m_pPortionConfigList[i]->m_cName, 20);
                iSkillLimit = m_pPortionConfigList[i]->m_iSkillLimit;
                iDifficulty = m_pPortionConfigList[i]->m_iDifficulty;
            }
        }


    if (strlen(cPortionName) == 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOMATCHINGPOTION, 0, 0, 0, 0);
        return;
    }


    iSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[12];
    if (iSkillLimit > iSkillLevel)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LOWPOTIONSKILL, 0, 0, 0, cPortionName);
        return;
    }


    iSkillLevel -= iDifficulty;
    if (iSkillLevel <= 0) iSkillLevel = 1;

#if defined(DEF_TESTSERVER)
    iSkillLevel = 100;
#else

    if (m_pClientList[iClientH]->m_cSkillMastery[12] == 100)
        iSkillLevel += 5;
#endif

    iResult = iDice(1, 100);
    if (iResult > iSkillLevel)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_POTIONFAIL, 0, 0, 0, cPortionName);
        return;
    }


    /*
    if ((m_pClientList[iClientH]->m_iLevel < 20) && (iSkillLevel > 80 )) {

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PORTIONFAIL, 0, 0, 0, cPortionName);
    return;
    }
    */



    CalculateSSN_SkillIndex(iClientH, 12, 1);


    if (strlen(cPortionName) != 0)
    {
        pItem = 0;
        pItem = new CItem;
        if (pItem == 0) return;


        for (i = 0; i < 6; i++)
            if (sItemIndex[i] != -1)
            {
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME)
                    // v1.41 !!!
                    SetItemCount(iClientH, sItemIndex[i], //     m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_cName,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex[i]]->m_dwCount - sItemNumber[i]);
                else ItemDepleteHandler(iClientH, sItemIndex[i], false);
            }

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_POTIONSUCCESS, 0, 0, 0, cPortionName);

        GetExp(iClientH, iDice(2, (iDifficulty / 3))); //m_pClientList[iClientH]->m_iExpStock += iDice(1, (iDifficulty/3));

        if ((_bInitItemAttr(pItem, cPortionName) == true))
        {


            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
            {
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
                    pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4


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
                        break;
                }
            }
        }
        else
        {
            delete pItem;
            pItem = 0;
        }
    }
}

void CGame::_SetItemPos(int iClientH, char * pData)
{
    char * cp, cItemIndex;
    short * sp, sX, sY;

    if (m_pClientList[iClientH] == 0) return;

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
    if (m_pClientList[iClientH]->m_pItemList[cItemIndex] != 0)
    {
        m_pClientList[iClientH]->m_ItemPosList[cItemIndex].x = sX;
        m_pClientList[iClientH]->m_ItemPosList[cItemIndex].y = sY;
    }
}

void CGame::BuildItemHandler(int iClientH, char * pData)
{
    char * cp, cName[21], cElementItemID[6];
    int    i, x, z, iMatch, iCount, iPlayerSkillLevel, iResult, iTotalValue, iResultValue, iTemp, iItemCount[DEF_MAXITEMS]{};
    CItem * pItem;
    bool   bFlag, bItemFlag[6]{};
    double dV1, dV2, dV3;
    uint32_t  dwTemp, dwTemp2;
    uint16_t   wTemp;


    if (m_pClientList[iClientH] == 0) return;
    m_pClientList[iClientH]->m_iSkillMsgRecvCount++;

    cp = (char *)(pData + 11);
    memset(cName, 0, sizeof(cName));
    memcpy(cName, cp, 20);
    cp += 20;

    //testcode
    //log->info(cName);

    memset(cElementItemID, 0, sizeof(cElementItemID));
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


    bFlag = true;
    while (bFlag == true)
    {
        bFlag = false;
        for (i = 0; i <= 4; i++)
            if ((cElementItemID[i] == -1) && (cElementItemID[i + 1] != -1))
            {
                cElementItemID[i] = cElementItemID[i + 1];
                cElementItemID[i + 1] = -1;
                bFlag = true;
            }
    }

    for (i = 0; i < 6; i++) bItemFlag[i] = false;

    //testcode
    //wsprintf(G_cTxt, "%d %d %d %d %d %d", cElementItemID[0], cElementItemID[1], cElementItemID[2],
    //	     cElementItemID[3], cElementItemID[4], cElementItemID[5]);
    //log->info(G_cTxt);

    iPlayerSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[13];
    iResult = iDice(1, 100);

    if (iResult > iPlayerSkillLevel)
    {


        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMFAIL, 0, 0, 0, 0);
        return;
    }


    for (i = 0; i < 6; i++)
        if (cElementItemID[i] != -1)
        {

            if ((cElementItemID[i] < 0) || (cElementItemID[i] > DEF_MAXITEMS)) return;
            if (m_pClientList[iClientH]->m_pItemList[cElementItemID[i]] == 0) return;
        }


    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
        if (m_pBuildItemList[i] != 0)
        {
            if (memcmp(m_pBuildItemList[i]->m_cName, cName, 20) == 0)
            {



                if (m_pBuildItemList[i]->m_iSkillLimit > m_pClientList[iClientH]->m_cSkillMastery[13]) return;

                for (x = 0; x < DEF_MAXITEMS; x++)
                    if (m_pClientList[iClientH]->m_pItemList[x] != 0)
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
                            if ((cElementItemID[z] != -1) && (bItemFlag[z] == false))
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
                                    bItemFlag[z] = true;

                                    goto BIH_LOOPBREAK;
                                }
                            }
                        BIH_LOOPBREAK:;
                    }
                }


                if (iMatch != 6)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMFAIL, 0, 0, 0, 0);
                    return;
                }


                dV2 = (double)m_pBuildItemList[i]->m_iMaxValue;
                if (iTotalValue <= 0)
                    dV3 = 1.0f;
                else dV3 = (double)iTotalValue;
                dV1 = (double)(dV3 / dV2) * 100.0f;


                iTotalValue = (int)dV1;


                pItem = new CItem;
                if (_bInitItemAttr(pItem, m_pBuildItemList[i]->m_cName) == false)
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
                    //Àç·á ¼øµµ¿¡ µû¸¥ ¼º´É ÀÔ·Â: SpecEffectValue1Àº ¼ö¸í, SpecEffectValue2´Â ¼º´É °¡ÁßÄ¡ 


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

                    //Custom-ItemÀº »ö»óÀÌ 2¹ø. 
                    pItem->m_cItemColor = 2;
                }

                //testcode
                wsprintf(G_cTxt, "Custom-Item(%s) Value(%d) Life(%d/%d)", pItem->m_cName, pItem->m_sItemSpecEffectValue2, pItem->m_wCurLifeSpan, pItem->m_wMaxLifeSpan);
                log->info(G_cTxt);


                bAddItem(iClientH, pItem, 0);
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMSUCCESS, pItem->m_sItemSpecEffectValue2, pItem->m_cItemType, 0, 0);

#ifdef DEF_TAIWANLOG

                _bItemLog(DEF_ITEMLOG_MAKE, iClientH, (int)-1, pItem);
#endif	


                for (x = 0; x < 6; x++)
                    if (cElementItemID[x] != -1)
                    {
                        if (m_pClientList[iClientH]->m_pItemList[cElementItemID[x]] == 0)
                        {
                            // ### BUG POINT!!!

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


                GetExp(iClientH, iDice(2, (m_pBuildItemList[i]->m_iSkillLimit / 4))); //m_pClientList[iClientH]->m_iExpStock += iDice(1, (m_pBuildItemList[i]->m_iSkillLimit/4));

                return;
            }
        }

    /*

    for (i = 0; i < DEF_MAXBUILDITEMS; i++)
    if (m_pBuildItemList[i] != 0) {
    if (memcmp(m_pBuildItemList[i]->m_cName, cName, 20) == 0) {



    if (m_pBuildItemList[i]->m_iSkillLimit > m_pClientList[iClientH]->m_cSkillMastery[13]) return;

    iMatch = 0;


    for (x = 0; x < 6; x++) m_pBuildItemList[i]->m_iIndex[x] = -1;
    for (x = 0; x < DEF_MAXITEMS; x++) bItemFlag[x] = false;


    iTotalValue = 0;
    for (x = 0; x < 6; x++)
    if (m_pBuildItemList[i]->m_iMaterialItemCount[x] == 0) iMatch++;
    else {
    bFlag = false;
    for (z = 0; z < DEF_MAXITEMS; z++)
    if ((m_pClientList[iClientH]->m_pItemList[z] != 0)) {
    if ((m_pClientList[iClientH]->m_pItemList[z]->m_sIDnum  == m_pBuildItemList[i]->m_iMaterialItemID[x]) &&
    (m_pClientList[iClientH]->m_pItemList[z]->m_dwCount >= m_pBuildItemList[i]->m_iMaterialItemCount[x]) &&
    (bItemFlag[z] == false)) {

    m_pBuildItemList[i]->m_iIndex[x] = z;
    bItemFlag[z] = true;
    iTotalValue += (m_pClientList[iClientH]->m_pItemList[z]->m_sItemSpecEffectValue1 * m_pBuildItemList[i]->m_iMaterialItemValue[x]);

    bFlag = true;
    goto BIH_LOOPBREAK;
    }
    }

    BIH_LOOPBREAK:;

    if (bFlag == true) iMatch++;
    }

    if (iMatch != 6) {

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMFAIL, 0, 0, 0, 0);
    return;
    }

    //testcode
    wsprintf(G_cTxt, "iTotalValue(%d) MaxValue(%d)", iTotalValue, m_pBuildItemList[i]->m_iMaxValue);
    log->info(G_cTxt);


    dV2 = (double)m_pBuildItemList[i]->m_iMaxValue;
    if (iTotalValue <= 0)
    dV3 = 1.0f;
    else dV3 = (double)iTotalValue;
    dV1 = (double)(dV3/dV2)*100.0f;


    iTotalValue = (int)dV1;

    //testcode
    wsprintf(G_cTxt, "iTotalValue(%d)", iTotalValue);
    log->info(G_cTxt);





    for (x = 0; x < 6; x++)
    if (m_pBuildItemList[i]->m_iIndex[x] != -1) {
    iIndex = m_pBuildItemList[i]->m_iIndex[x];
    iCount = m_pClientList[iClientH]->m_pItemList[iIndex]->m_dwCount - m_pBuildItemList[i]->m_iMaterialItemCount[x];
    if (iCount < 0) iCount = 0;
    SetItemCount(iClientH, iIndex, iCount);
    }


    pItem = new CItem;
    if (_bInitItemAttr(pItem, m_pBuildItemList[i]->m_cName) == false) {
    delete pItem;
    return;
    }

    if (pItem->m_cItemType == DEF_ITEMTYPE_MATERIAL) {

    pItem->m_sItemSpecEffectValue1 = iDice(1, iPlayerSkillLevel);

    if (pItem->m_sItemSpecEffectValue1 < (iPlayerSkillLevel/2))
    pItem->m_sItemSpecEffectValue1 = (iPlayerSkillLevel/2);
    }
    else {



    dwTemp = pItem->m_dwAttribute;
    dwTemp = dwTemp | 0x00000001;
    pItem->m_dwAttribute = dwTemp;

    iResultValue = (iTotalValue - m_pBuildItemList[i]->m_iAverageValue);
    //Àç·á ¼øµµ¿¡ µû¸¥ ¼º´É ÀÔ·Â: SpecEffectValue1Àº ¼ö¸í, SpecEffectValue2´Â ¼º´É °¡ÁßÄ¡

    //testcode
    wsprintf(G_cTxt, "iResultValue(%d) %d %d", iResultValue, iTotalValue, m_pBuildItemList[i]->m_iAverageValue);
    log->info(G_cTxt);


    if (iResultValue == 0)
    dV2 = 1.0f;
    else dV2 = (double)iResultValue;
    dV3 = (double)pItem->m_wMaxLifeSpan;
    dV1 = (dV2/100.0f)*dV3;

    iTemp  = (int)pItem->m_wMaxLifeSpan;
    iTemp += (int)dV1;

    if (iTemp <= 0)
    wTemp = 1;
    else wTemp = (WORD)iTemp;

    if (wTemp <= pItem->m_wMaxLifeSpan*2) {

    pItem->m_wMaxLifeSpan = wTemp;
    pItem->m_wCurLifeSpan = wTemp;
    pItem->m_sItemSpecEffectValue1 = (short)wTemp;
    }
    else pItem->m_sItemSpecEffectValue1 = (short)pItem->m_wMaxLifeSpan;


    if (iResultValue > 0) {
    dV2 = (double)iResultValue;
    dV3 = (double)(100 - m_pBuildItemList[i]->m_iAverageValue);
    dV1 = (dV2/dV3)*100.0f;
    pItem->m_sItemSpecEffectValue2 = (int)dV1;
    }
    else if (iResultValue < 0) {
    dV2 = (double)abs(iResultValue);
    dV3 = (double)(m_pBuildItemList[i]->m_iAverageValue);
    dV1 = (dV2/dV3)*100.0f;
    pItem->m_sItemSpecEffectValue2 = -1*abs(100 - (int)dV1);
    }
    else pItem->m_sItemSpecEffectValue2 = 0;
    }

    //testcode
    wsprintf(G_cTxt, "Custom-Item(%s) Value(%d)", pItem->m_cName, pItem->m_sItemSpecEffectValue2);
    log->info(G_cTxt);


    bAddItem(iClientH, pItem, 0);
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_BUILDITEMSUCCESS, 0, 0, 0, 0);
    }
    }

    */
}

void CGame::RequestNoticementHandler(int iClientH, char * pData)
{
    char * cp, cData[120];
    int * ip, iRet, iClientSize;
    uint32_t * dwp;
    uint16_t * wp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_dwNoticementDataSize < 10) return;

    ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
    iClientSize = *ip;

    if (iClientSize != m_dwNoticementDataSize)
    {

        cp = new char[m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2];
        memset(cp, 0, m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2);
        memcpy((cp + DEF_INDEX2_MSGTYPE + 2), m_pNoticementData, m_dwNoticementDataSize);

        dwp = (uint32_t *)(cp + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_NOTICEMENT;
        wp = (uint16_t *)(cp + DEF_INDEX2_MSGTYPE);
        *wp = DEF_MSGTYPE_REJECT;

        iRet = m_pClientList[iClientH]->iSendMsg(cp, m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2);

        delete cp;
    }
    else
    {
        memset(cData, 0, sizeof(cData));

        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_NOTICEMENT;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_MSGTYPE_CONFIRM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
    }

}

void CGame::RequestCheckAccountPasswordHandler(char * pData, uint32_t dwMsgSize)
{
    int * ip, i, iLevel;
    char * cp, cAccountName[11], cAccountPassword[11];

    cp = (char *)(pData + 6);

    memset(cAccountName, 0, sizeof(cAccountName));
    memset(cAccountPassword, 0, sizeof(cAccountPassword));

    memcpy(cAccountName, cp, 10);
    cp += 10;

    memcpy(cAccountPassword, cp, 10);
    cp += 10;

    ip = (int *)cp;
    iLevel = *ip;
    cp += 4;

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cAccountName, cAccountName) == 0))
        {

            if ((strcmp(m_pClientList[i]->m_cAccountPassword, cAccountPassword) != 0) || (m_pClientList[i]->m_iLevel != iLevel))
            {
                wsprintf(G_cTxt, "(TestLog) Error! Account(%s)-Level(%d) password(or level) mismatch! Disconnect.", cAccountName, iLevel);
                log->info(G_cTxt);

                DeleteClient(i, false, true);
                return;
            }
        }
}

int CGame::iRequestPanningMapDataRequest(int iClientH, char * pData)
{
    char * cp, cDir, cData[3000]{};
    uint32_t * dwp;
    uint16_t * wp;
    short * sp, dX, dY;
    int   iRet, iSize;

    if (m_pClientList[iClientH] == 0) return 0;
    if (m_pClientList[iClientH]->m_bIsObserverMode == false) return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

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

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_PANNING;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOVE_CONFIRM;

    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

    sp = (short *)cp;
    *sp = dX - 10;
    cp += 2;

    sp = (short *)cp;
    *sp = dY - 7;
    cp += 2;

    *cp = cDir;
    cp++;

    iSize = iComposeMoveMapData(dX - 10, dY - 7, iClientH, cDir, cp);

    iRet = m_pClientList[iClientH]->iSendMsg(cData, iSize + 12 + 1 + 4);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:

            DeleteClient(iClientH, true, true);
            return 0;
    }

    return 1;
}

void CGame::GetMagicAbilityHandler(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_cSkillMastery[4] != 0) return;


    m_pClientList[iClientH]->m_cSkillMastery[4] = 20;
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILL, 4, m_pClientList[iClientH]->m_cSkillMastery[4], 0, 0);

    bCheckTotalSkillMasteryPoints(iClientH, 4);
}

void CGame::RequestRestartHandler(int iClientH)
{
    char  cTmpMap[32];

    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_bIsKilled == true)
    {

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
                        m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10; // v2.04
                    }
                    else
                    {
                        memcpy(m_pClientList[iClientH]->m_cMapName, "resurr1", 7);
                        m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10;
                    }
                }
                // v2.16 2002-5-31
                if (strcmp(cTmpMap, "elvine") == 0)
                {
                    memcpy(m_pClientList[iClientH]->m_cMapName, "elvjail", 7);
                    strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvjail");
                    m_pClientList[iClientH]->m_iLockedMapTime = 60 * 3; // 3ºÐ 
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
                        m_pClientList[iClientH]->m_iDeadPenaltyTime = 60 * 10; // v2.04
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
                    m_pClientList[iClientH]->m_iLockedMapTime = 60 * 3; // 3ºÐ 

                }
                else if (m_pClientList[iClientH]->m_iLevel > 80)
                    memcpy(m_pClientList[iClientH]->m_cMapName, "resurr2", 7);
                else memcpy(m_pClientList[iClientH]->m_cMapName, "elvfarm", 7);
            }
        }


        m_pClientList[iClientH]->m_bIsKilled = false;
        m_pClientList[iClientH]->m_iHP = (3 * m_pClientList[iClientH]->m_iVit) + (2 * m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iStr / 2);
        m_pClientList[iClientH]->m_iHungerStatus = 100;

        memset(cTmpMap, 0, sizeof(cTmpMap));
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

    if (m_pClientList[iClientH] == 0) return;

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
        if (m_pClientList[iClientH]->m_pItemList[cIndex] == 0) return;


        ReqSellItemConfirmHandler(iClientH, cIndex, iAmount, 0);

        if (m_pClientList[iClientH] == 0) return;
    }
}

void CGame::CreateNewPartyHandler(int iClientH)
{
    bool bFlag;

    if (m_pClientList[iClientH] == 0) return;

    bFlag = m_pClientList[iClientH]->bCreateNewParty();
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_RESPONSE_CREATENEWPARTY, (int)bFlag, 0, 0, 0);
}

void CGame::JoinPartyHandler(int iClientH, int iV1, char * pMemberName)
{
    short sAppr2;
    int i;

    if (m_pClientList[iClientH] == 0) return;

    switch (iV1)
    {
        case 0:
            RequestDeletePartyHandler(iClientH);
            break;

        case 1:
            //testcode
            wsprintf(G_cTxt, "Join Party Req: %s(%d) ID(%d) Stat(%d) ReqJoinH(%d) ReqJoinName(%s)", m_pClientList[iClientH]->m_cCharName, iClientH,
                m_pClientList[iClientH]->m_iPartyID, m_pClientList[iClientH]->m_iPartyStatus, m_pClientList[iClientH]->m_iReqJoinPartyClientH,
                m_pClientList[iClientH]->m_cReqJoinPartyName);
            log->info(G_cTxt);

            if ((m_pClientList[iClientH]->m_iPartyID != 0) || (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL))
            {

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);

                /*			m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
                memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
                m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                */
                //testcode
                log->info("Join Party Reject (1)");
                return;
            }

            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cCharName, pMemberName) == 0))
                {

                    sAppr2 = (short)((m_pClientList[i]->m_sAppr2 & 0xF000) >> 12);
                    if (sAppr2 != 0)
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);
                        //testcode
                        log->info("Join Party Reject (2)");
                    }
                    else if (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide)
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);
                        //testcode
                        log->info("Join Party Reject (3)");
                    }
                    else if (m_pClientList[i]->m_iPartyStatus == DEF_PARTYSTATUS_PROCESSING)
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);
                        //testcode
                        log->info("Join Party Reject (4)");
                        //testcode
                        wsprintf(G_cTxt, "Party join reject(2) ClientH:%d ID:%d JoinName:%d", i, m_pClientList[i]->m_iPartyID, m_pClientList[i]->m_cReqJoinPartyName);
                        log->info(G_cTxt);

                        m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
                        memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
                        m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                    }
                    else
                    {

                        m_pClientList[i]->m_iReqJoinPartyClientH = iClientH;
                        memset(m_pClientList[i]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[i]->m_cReqJoinPartyName));
                        strcpy(m_pClientList[i]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName);
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_QUERY_JOINPARTY, 0, 0, 0, m_pClientList[iClientH]->m_cCharName);


                        m_pClientList[iClientH]->m_iReqJoinPartyClientH = i;
                        memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
                        strcpy(m_pClientList[iClientH]->m_cReqJoinPartyName, m_pClientList[i]->m_cCharName);

                        m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
                    }
                    return;
                }
            break;

        case 2:
            if (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM)
            {
            }
            break;
    }
}

void CGame::RequestSetGuildConstructLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName)
{
    int i;
    int iIndex;
    uint32_t dwTemp, dwTime;

    if (m_pClientList[iClientH] == 0) return;

    dwTime = timeGetTime();

    //testcode
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
                memset(m_pGuildTeleportLoc[i].m_cDestMapName2, 0, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName2));
                strcpy(m_pGuildTeleportLoc[i].m_cDestMapName2, pMapName);
                m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;
                return;
            }
        }


    dwTemp = 0;
    iIndex = -1;
    for (i = 0; i < DEF_MAXGUILDS; i++)
    {
        if (m_pGuildTeleportLoc[i].m_iV1 == 0)
        {

            m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
            m_pGuildTeleportLoc[i].m_sDestX2 = dX;
            m_pGuildTeleportLoc[i].m_sDestY2 = dY;
            memset(m_pGuildTeleportLoc[i].m_cDestMapName2, 0, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName2));
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

    //testcode
    log->info("(X) No more GuildConstructLoc Space! Replaced.");

    m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
    m_pGuildTeleportLoc[i].m_sDestX2 = dX;
    m_pGuildTeleportLoc[i].m_sDestY2 = dY;
    memset(m_pGuildTeleportLoc[i].m_cDestMapName2, 0, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName2));
    strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
    m_pGuildTeleportLoc[i].m_dwTime2 = dwTime;
}

void CGame::RequestSummonWarUnitHandler(int iClientH, int dX, int dY, char cType, char cNum, char cMode)
{
    char cName[6], cNpcName[21], cMapName[11], cNpcWayPoint[11], cOwnerType;
    int i, x;
    int iNamingValue, tX, tY, ix, iy;
    bool bRet;
    short sOwnerH;
    uint32_t dwTime = timeGetTime();

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    memset(cNpcWayPoint, 0, sizeof(cNpcWayPoint));
    memset(cNpcName, 0, sizeof(cNpcName));
    memset(cMapName, 0, sizeof(cMapName));


    if (cType < 0) return;
    if (cType >= DEF_MAXNPCTYPES) return;
    if (cNum > 10) return;


    if (m_pClientList[iClientH]->m_iConstructionPoint < m_iNpcConstructionPoint[cType]) return;

    if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0) && (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true)) return;


    cNum = 1;


    for (x = 1; x <= cNum; x++)
    {

        iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue == -1)
        {


        }
        else
        {

            memset(cName, 0, sizeof(cName));
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
            }

            //testcode
            wsprintf(G_cTxt, "(!) Request Summon War Unit (%d) (%s)", cType, cNpcName);
            log->info(G_cTxt);

            tX = (int)dX;
            tY = (int)dY;


            bRet = false;
            switch (cType)
            {
                case 36:
                case 37:
                case 38:
                case 39:

                    if (strcmp(m_pClientList[iClientH]->m_cConstructMapName, m_pClientList[iClientH]->m_cMapName) != 0) bRet = true;
                    if (abs(m_pClientList[iClientH]->m_sX - m_pClientList[iClientH]->m_iConstructLocX) > 10) bRet = true;
                    if (abs(m_pClientList[iClientH]->m_sY - m_pClientList[iClientH]->m_iConstructLocY) > 10) bRet = true;

                    if (bRet == true)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 2, 0, 0, 0);
                        return;
                    }


                    /////
                    for (i = 0; i < DEF_MAXGUILDS; i++)
                        if (m_pGuildTeleportLoc[i].m_iV1 == m_pClientList[iClientH]->m_iGuildGUID)
                        {
                            m_pGuildTeleportLoc[i].m_dwTime = dwTime;
                            if (m_pGuildTeleportLoc[i].m_iV2 >= DEF_MAXCONSTRUCTNUM)
                            {

                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 3, 0, 0, 0);
                                return;
                            }
                            else
                            {

                                m_pGuildTeleportLoc[i].m_iV2++;
                                goto RSWU_LOOPBREAK;
                            }
                        }


                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 3, 0, 0, 0);
                    return;
                    break;


                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 51:
                    for (i = 0; i < DEF_MAXGUILDS; i++)
                        if (m_pGuildTeleportLoc[i].m_iNumSummonNpc >= DEF_MAX_CRUSADESUMMONMOB)
                        {
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 4, 0, 0, 0);
                            return;
                        }
                        else
                        {
                            m_pGuildTeleportLoc[i].m_iNumSummonNpc++;
                            goto RSWU_LOOPBREAK;
                        }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 3, 0, 0, 0);
                    return;
                    break;
            }

            RSWU_LOOPBREAK:;


            bRet = false;
            switch (cType)
            {
                case 36:
                case 37:
                    for (ix = tX - 2; ix <= tX + 2; ix++)
                        for (iy = tY - 2; iy <= tY + 2; iy++)
                        {
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_NPC))
                            {
                                switch (m_pNpcList[sOwnerH]->m_sType)
                                {
                                    case 36:
                                    case 37:
                                        bRet = true;
                                        break;
                                }
                            }
                        }


                    if ((dY <= 32) || (dY >= 783)) bRet = true;
                    break;
            }

            if (bRet == true)
            {

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 1, 0, 0, 0);
                return;
            }


            if (cMode == 0)
            {
                bRet = bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_FOLLOW, &tX, &tY, cNpcWayPoint, 0, 0, -1, false, false, false, false, m_pClientList[iClientH]->m_iGuildGUID);
                bSetNpcFollowMode(cName, m_pClientList[iClientH]->m_cCharName, DEF_OWNERTYPE_PLAYER);
            }
            else bRet = bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_GUARD, &tX, &tY, cNpcWayPoint, 0, 0, -1, false, false, false, false, m_pClientList[iClientH]->m_iGuildGUID);

            if (bRet == false)
            {

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
            }
            else
            {

                m_pClientList[iClientH]->m_iConstructionPoint -= m_iNpcConstructionPoint[cType];
                if (m_pClientList[iClientH]->m_iConstructionPoint < 0) m_pClientList[iClientH]->m_iConstructionPoint = 0;

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 0, 0);

#ifdef DEF_TAIWANLOG
                _bItemLog(DEF_ITEMLOG_SUMMONMONSTER, iClientH, cNpcName, NULL);
#endif
            }
        }
    }
}

void CGame::SelectCrusadeDutyHandler(int iClientH, int iDuty)
{
    if (m_pClientList[iClientH] == 0) return;

    if ((m_pClientList[iClientH]->m_iGuildRank != 0) && (iDuty == 3)) return;

    //v2.19 2002-11-15  Àü¸éÀü½Ã Àü¿¡ Àü¸éÀü¿¡ ÁøÂÊÀº ±æµå¸¶½ºÅÍµéÀº ¼ÒÈ¯ Æ÷ÀÎÆ®¸¦ 3000À» ³Ö¾î ÁØ´Ù..
    //	if(m_pClientList[iClientH]->m_dwCrusadeGUID != m_dwCrusadeGUID)

#ifdef DEF_V219

    if ((3 - m_iWinnerSide) == m_pClientList[iClientH]->m_cSide)
        if (m_pClientList[iClientH]->m_iCrusadeDuty == 0)
        {
            if (3 == iDuty)
            {
                m_pClientList[iClientH]->m_iConstructionPoint = 3000;
            }
        }
#endif

    m_pClientList[iClientH]->m_iCrusadeDuty = iDuty;
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CRUSADE, (DWORD)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, 0, 0);

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
    bool bIsSlatePresent = false;
    CItem * pItem;
    int iSlateType, iEraseReq;
    uint32_t * dwp;
    uint16_t * wp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;

    for (i = 0; i < 4; i++)
    {
        cItemID[i] = 0;
        ctr[i] = 0;
    }

    cp = (char *)pData;
    cp += 11;

    // 14% chance of creating slates
    if (iDice(1, 100) < m_sSlateSuccessRate) bIsSlatePresent = true;

    try
    {
        // make sure slates really exist
        for (i = 0; i < 4; i++)
        {
            cItemID[i] = *cp;
            cp++;

            if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] == 0 || cItemID[i] > DEF_MAXITEMS)
            {
                bIsSlatePresent = false;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, 0, 0, 0, 0);
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
        bIsSlatePresent = false;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, 0, 0, 0, 0);
        wsprintf(G_cTxt, "TSearch Slate Hack: (%s) Player: (%s) - creating slates without correct item!", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
        log->info(G_cTxt);
        DeleteClient(iClientH, true, true);
        return;
    }

    // Are all 4 slates present ??
    if (ctr[0] != 1 || ctr[1] != 1 || ctr[2] != 1 || ctr[3] != 1)
    {
        bIsSlatePresent = false;
        return;
    }

    if (m_pClientList[iClientH]->m_iAdminUserLevel > 3) bIsSlatePresent = true;

    // if we failed, kill everything
    if (!bIsSlatePresent)
    {
        for (i = 0; i < 4; i++)
        {
            if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != 0)
            {
                ItemDepleteHandler(iClientH, cItemID[i], false);
            }
        }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, 0, 0, 0, 0);
        return;
    }

    // make the slates
    for (i = 0; i < 4; i++)
    {
        if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != 0)
        {
            ItemDepleteHandler(iClientH, cItemID[i], false);
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
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_CREATESUCCESS, iSlateType, 0, 0, 0);

    memset(cData, 0, sizeof(cData));

    // Create slates
    if (_bInitItemAttr(pItem, 867) == false)
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
        if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
        {
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

            *cp = (char)pItem->m_sItemSpecEffectValue2;
            cp++;

            dwp = (uint32_t *)cp;
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
                    DeleteClient(iClientH, true, true);
                    return;
            }
        }
        else
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);
            SendEventToNearClient_TypeB(MSGID_MAGICCONFIGURATIONCONTENTS, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame,
                pItem->m_cItemColor);
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
                    break;
            }
        }
    }
    return;
}

void CGame::ClientCommonHandler(int iClientH, char * pData, uint32_t size)
{
    uint16_t wCommand;
    short sX, sY;
    int iV1, iV2, iV3, iV4;
    char cDir;
    std::string sString;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;

    stream_read sr(pData, size);

    sr.read_uint32();
    wCommand = sr.read_uint16();
    sX = sr.read_uint16();
    sY = sr.read_uint16();
    cDir = sr.read_byte();

    switch (wCommand)
    {
        case DEF_COMMONTYPE_BUILDITEM:
        case DEF_COMMONTYPE_REQ_CREATEPOTION:
        case DEF_COMMONTYPE_CRAFTITEM:
        case DEF_COMMONTYPE_REQ_CREATESLATE:
            sString = sr.read_string(20);
            break;
        default:
            iV1 = sr.read_uint32();
            iV2 = sr.read_uint32();
            iV3 = sr.read_uint32();
            sString = sr.read_string(30);
            iV4 = sr.read_uint32();
            break;
    }

    // todo: fix this
    char * pString = (char *)sString.c_str();

    switch (wCommand)
    {
        case DEF_COMMONTYPE_REQUEST_HUNTMODE:
            RequestHuntmode(iClientH);
            break;

        case DEF_COMMONTYPE_REQGUILDNAME:
            RequestGuildNameHandler(iClientH, iV1, iV2);
            break;

        case DEF_COMMONTYPE_REQPLAYERNAME:
            RequestPlayerNameHandler(iClientH, iV1, iV2);
            break;

        case DEF_COMMONTYPE_UPGRADEITEM:
            RequestItemUpgradeHandler(iClientH, iV1);
            break;

        case DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY:
            RequestAcceptJoinPartyHandler(iClientH, iV1);
            break;

        case DEF_COMMONTYPE_SETGUILDTELEPORTLOC:
            RequestSetGuildTeleportLocHandler(iClientH, iV1, iV2, m_pClientList[iClientH]->m_iGuildGUID, pString);
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

            //case DEF_COMMONTYPE_REQUEST_CREATENEWPARTY:
            //	CreateNewPartyHandler(iClientH);
            //	break;

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
            SetExchangeItem(iClientH, iV1, iV2, iV3);
            break;

        case DEF_COMMONTYPE_REQ_GETHEROMANTLE:
            GetHeroMantleHandler(iClientH);
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

        case DEF_COMMONTYPE_REQ_CREATEPOTION:
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
            DropItemHandler(iClientH, iV1, iV2, pString, true);
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
            ReleaseItemHandler(iClientH, iV1, true);
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

    }
}

void CGame::JoinGuildApproveHandler(int iClientH, char * pName)
{
    int i;
    bool bIsExist = false;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;




    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {

            if (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide) return;


            memset(m_pClientList[i]->m_cGuildName, 0, sizeof(m_pClientList[i]->m_cGuildName));
            strcpy(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName);


            m_pClientList[i]->m_iGuildGUID = m_pClientList[iClientH]->m_iGuildGUID;


            memset(m_pClientList[i]->m_cLocation, 0, sizeof(m_pClientList[i]->m_cLocation));
            strcpy(m_pClientList[i]->m_cLocation, m_pClientList[iClientH]->m_cLocation);

            m_pClientList[i]->m_iGuildRank = DEF_GUILDSTARTRANK; //@@@  GuildRankÀÇ ½ÃÀÛÀº DEF_GUILDSTARTRANK


            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_JOINGUILDAPPROVE, 0, 0, 0, 0);

#ifdef DEF_TAIWANLOG
            _bItemLog(DEF_ITEMLOG_JOINGUILD, i, (char *)NULL, NULL);
#endif


            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_CLEARGUILDNAME, m_pClientList[i]->m_cMapIndex, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY, 0, 0, 0);



            SendGuildMsg(i, DEF_NOTIFY_NEWGUILDSMAN, 0, 0, 0);
            return;
        }
}

void CGame::JoinGuildRejectHandler(int iClientH, char * pName)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;




    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {


            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_JOINGUILDREJECT, 0, 0, 0, 0);
            return;
        }


}

void CGame::DismissGuildApproveHandler(int iClientH, char * pName)
{
    int i;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;




    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {
            SendGuildMsg(i, DEF_NOTIFY_DISMISSGUILDSMAN, 0, 0, 0);

            memset(m_pClientList[i]->m_cGuildName, 0, sizeof(m_pClientList[i]->m_cGuildName));
            strcpy(m_pClientList[i]->m_cGuildName, "NONE");
            m_pClientList[i]->m_iGuildRank = -1;
            m_pClientList[i]->m_iGuildGUID = -1;

            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, 0, 0, 0, 0);

            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_CLEARGUILDNAME, m_pClientList[i]->m_cMapIndex, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY, 0, 0, 0);
            return;
        }
}

void CGame::DismissGuildRejectHandler(int iClientH, char * pName)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;




    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0))
        {


            SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_DISMISSGUILDREJECT, 0, 0, 0, 0);
            return;
        }


}

void CGame::GuildNotifyHandler(char * pData, uint32_t dwMsgSize)
{

    char * cp, cCharName[11], cGuildName[21];

    memset(cCharName, 0, sizeof(cCharName));
    memset(cGuildName, 0, sizeof(cGuildName));

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    memcpy(cCharName, cp, 10);
    cp += 10;

    memcpy(cGuildName, cp, 20);
    cp += 20;


}

void CGame::ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char * pItemName)
{
    char cItemCategory;
    short sRemainLife;
    int   iPrice;
    double d1, d2, d3;
    bool   bNeutral;
    uint32_t  dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2, dwMul1, dwMul2;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;
    if (iNum <= 0) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

    iCalcTotalWeight(iClientH);

    // v1.42
    bNeutral = false;
    if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) bNeutral = true;



    switch (cSellToWhom)
    {
        case 15:
        case 24:
            cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;


            if ((cItemCategory >= 11) && (cItemCategory <= 50))
            {


                iPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) * iNum;
                sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;


                //v1.42 Áß¸³ÀÎ °æ¿ì ¹ÝÀÇ ¹Ý°ª.
                if (bNeutral == true) iPrice = iPrice / 2;
                if (iPrice <= 0)    iPrice = 1;
                if (iPrice > 1000000) iPrice = 1000000;

                if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH))
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
                }
                else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
            }

            else if ((cItemCategory >= 1) && (cItemCategory <= 10))
            {

                sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

                if (sRemainLife == 0)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 2, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
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

                    if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != 0)
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


                    if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != 0)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
                        dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;


                        //Ãß°¡ µ¶¼ºÀúÇ×(1), Ãß°¡ ¸íÁß°ª(2), Ãß°¡ ¹æ¾î°ª(3), HP È¸º¹·® Ãß°¡(4), SP È¸º¹·® Ãß°¡(5)
                        //MP È¸º¹·® Ãß°¡(6), Ãß°¡ ¸¶¹ýÀúÇ×(7), ¹°¸® ´ë¹ÌÁö Èí¼ö(8), ¸¶¹ý ´ë¹ÌÁö Èí¼ö(9)
                        //¿¬Å¸ ´ë¹ÌÁö Ãß°¡(10), ´õ ¸¹Àº °æÇèÄ¡(11), ´õ¸¹Àº Gold(12)
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

                    //v1.42 Áß¸³ÀÎ °æ¿ì ¹ÝÀÇ ¹Ý°ª.
                    if (bNeutral == true) iPrice = iPrice / 2;
                    if (iPrice <= 0)    iPrice = 1;
                    if (iPrice > 1000000) iPrice = 1000000;

                    if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH))
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
                    }
                    else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
                }
            }
            else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 1, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
            break;


        default:
            break;
    }
}

void CGame::ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char * pString)
{
    CItem * pItemGold;
    short sRemainLife;
    int   iPrice;
    double d1, d2, d3;
    char * cp, cItemName[21], cData[120]{}, cItemCategory;
    uint32_t * dwp, dwMul1, dwMul2, dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2;
    uint16_t * wp;
    int    iEraseReq, iRet;
    short * sp;
    bool   bNeutral;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;
    if (iNum <= 0) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

    if (m_pClientList[iClientH]->m_bIsOnShop == false) return;


    iCalcTotalWeight(iClientH);
    cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

    // v1.42
    bNeutral = false;
    if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) bNeutral = true;

    iPrice = 0;

    if ((cItemCategory >= 1) && (cItemCategory <= 10))
    {
        // ¹«±â·ù´Ù

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

            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != 0)
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


            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != 0)
            {
                dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
                dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;


                //Ãß°¡ µ¶¼ºÀúÇ×(1), Ãß°¡ ¸íÁß°ª(2), Ãß°¡ ¹æ¾î°ª(3), HP È¸º¹·® Ãß°¡(4), SP È¸º¹·® Ãß°¡(5)
                //MP È¸º¹·® Ãß°¡(6), Ãß°¡ ¸¶¹ýÀúÇ×(7), ¹°¸® ´ë¹ÌÁö Èí¼ö(8), ¸¶¹ý ´ë¹ÌÁö Èí¼ö(9)
                //¿¬Å¸ ´ë¹ÌÁö Ãß°¡(10), ´õ ¸¹Àº °æÇèÄ¡(11), ´õ¸¹Àº Gold(12)
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

            //v1.42 Áß¸³ÀÎ °æ¿ì ¹ÝÀÇ ¹Ý°ª.
            if (bNeutral == true) iPrice = iPrice / 2;
            if (iPrice <= 0) iPrice = 1;
            if (iPrice > 1000000) iPrice = 1000000;


            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, 0, 0, 0);

            _bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);


            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                (m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW))
            {

                // v1.41 !!!
                SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
            }

            else ItemDepleteHandler(iClientH, cItemID, false, false);

        }
    }
    else
        if ((cItemCategory >= 11) && (cItemCategory <= 50))
        {

            iPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
            iPrice = iPrice * iNum;

            //v1.42 Áß¸³ÀÎ °æ¿ì ¹ÝÀÇ ¹Ý°ª.
            if (bNeutral == true) iPrice = iPrice / 2;
            if (iPrice <= 0) iPrice = 1;
            if (iPrice > 1000000) iPrice = 1000000;


            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, 0, 0, 0);

            _bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);


            if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
                (m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW))
            {

                // v1.41 !!!
                SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
            }

            else ItemDepleteHandler(iClientH, cItemID, false, false);
        }


    if (iPrice <= 0) return;

    pItemGold = new CItem;
    memset(cItemName, 0, sizeof(cItemName));
    wsprintf(cItemName, "Gold");
    _bInitItemAttr(pItemGold, cItemName);

    pItemGold->m_dwCount = iPrice;

    if (_bAddClientItemList(iClientH, pItemGold, &iEraseReq) == true)
    {


        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);


        *cp = 1;
        cp++;

        memcpy(cp, pItemGold->m_cName, 20);
        cp += 20;

        dwp = (uint32_t *)cp;
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

        wp = (uint16_t *)cp;
        *wp = pItemGold->m_wCurLifeSpan;
        cp += 2;

        wp = (uint16_t *)cp;
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

        *cp = (char)pItemGold->m_sItemSpecEffectValue2; // v1.41 
        cp++;

        dwp = (uint32_t *)cp;
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

                DeleteClient(iClientH, true, true);
                break;
        }
    }
    else
    {


        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY, pItemGold);


        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItemGold->m_sSprite, pItemGold->m_sSpriteFrame, pItemGold->m_cItemColor); // v1.4 color


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

void CGame::ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char * pString)
{
    char cItemCategory;
    short sRemainLife, sPrice;
    double d1, d2, d3;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;

    cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;


    if ((cItemCategory >= 1) && (cItemCategory <= 10))
    {
        // ¹«±â·ù´Ù


        if (cRepairWhom != 24)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
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
    else if (((cItemCategory >= 43) && (cItemCategory <= 50)) || ((cItemCategory >= 11) && (cItemCategory <= 13)))
    {



        if (cRepairWhom != 15)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
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

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 1, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
    }
}

void CGame::ReqRepairItemCofirmHandler(int iClientH, char cItemID, char * pString)
{
    short    sRemainLife, sPrice;
    char * cp, cItemCategory, cData[120]{};
    double   d1, d2, d3;
    uint32_t * dwp, dwGoldCount;
    uint16_t * wp;
    int      iRet, iGoldWeight;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    if ((cItemID < 0) || (cItemID >= 50)) return;
    if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;


    if (m_pClientList[iClientH]->m_bIsOnShop != true) return;
    //testcode
    //log->info("Repair!");

    cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;


    if (((cItemCategory >= 1) && (cItemCategory <= 10)) || ((cItemCategory >= 43) && (cItemCategory <= 50)) ||
        ((cItemCategory >= 11) && (cItemCategory <= 13)))
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

            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_NOTIFY;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
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

                    DeleteClient(iClientH, true, true);
                    return;
            }
            return;
        }
        else
        {
            //µ·ÀÌ ÃæºÐÇÏ´Ù. °íÄ¥ ¼ö ÀÖ´Ù. 


            m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMREPAIRED, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan, 0, 0);

#ifdef TAIWANLOG
            if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName != 0)
                _bItemLog(DEF_ITEMLOG_REPAIR, iClientH, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, NULL);
#endif 

            iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - sPrice);


            iCalcTotalWeight(iClientH);

            //v1.4 ¸¶À»ÀÇ ÀÚ±Ý¿¡ ´õÇÑ´Ù. 
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

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_iAllocatedFish == 0) return;
    if (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] == 0) return;


    m_pClientList[iClientH]->m_bSkillUsingStatus[1] = false;

    iResult = iDice(1, 100);
    if (m_pClientList[iClientH]->m_iFishChance >= iResult)
    {



        GetExp(iClientH, iDice(m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_iDifficulty, 6)); //m_pClientList[iClientH]->m_iExpStock += iDice(m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_iDifficulty, 5);

        CalculateSSN_SkillIndex(iClientH, 1, m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_iDifficulty);


        pItem = m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_pItem;
        m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_pItem = 0;


        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY,
            pItem);


        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color


        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FISHSUCCESS, 0, 0, 0, 0);
        iFishH = m_pClientList[iClientH]->m_iAllocatedFish;
        m_pClientList[iClientH]->m_iAllocatedFish = 0;


        bDeleteFish(iFishH, 1);
        return;
    }


    m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FISHFAIL, 0, 0, 0, 0);

    m_pClientList[iClientH]->m_iAllocatedFish = 0;
}

void CGame::RequestCreatePartyHandler(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL)
    {
        return;
    }

    m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;

    //testcode
    wsprintf(G_cTxt, "Request Create Party: %d", iClientH);
    log->info(G_cTxt);
}

void CGame::RequestJoinPartyHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cName[12];
    CStrTok * pStrTok;
    int i;


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL) return;
    if ((dwMsgSize) <= 0) return;

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token != 0)
    {
        memset(cName, 0, sizeof(cName));
        strcpy(cName, token);
    }
    else return;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cCharName, cName) == 0))
        {
            if ((m_pClientList[i]->m_iPartyID == 0) || (m_pClientList[i]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM))
            {
                return;
            }

            return;
        }


    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
}

void CGame::RequestDismissPartyHandler(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;

    m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
}

void CGame::GetPartyInfoHandler(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;
}

void CGame::RequestDeletePartyHandler(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iPartyID != 0)
    {
        m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
    }
}

void CGame::RequestAcceptJoinPartyHandler(int iClientH, int iResult)
{
    int iH;

    if (m_pClientList[iClientH] == 0) return;

    switch (iResult)
    {
        case 0:
            iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
            if (m_pClientList[iH] == 0)
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

            SendNotifyMsg(NULL, iH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);
            //testcode
            wsprintf(G_cTxt, "Party join reject(3) ClientH:%d ID:%d", iH, m_pClientList[iH]->m_iPartyID);
            log->info(G_cTxt);

            m_pClientList[iH]->m_iPartyID = 0;
            m_pClientList[iH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
            m_pClientList[iH]->m_iReqJoinPartyClientH = 0;
            memset(m_pClientList[iH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iH]->m_cReqJoinPartyName));

            m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
            memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
            break;

        case 1:
            if ((m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM) && (m_pClientList[iClientH]->m_iPartyID != 0))
            {
                iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
                if (m_pClientList[iH] == 0)
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
            }
            else
            {
                iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
                if (m_pClientList[iH] == 0)
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

            if ((m_pClientList[iClientH]->m_iPartyID != 0) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM))
            {

                RequestDismissPartyHandler(iClientH);
            }
            else
            {

                iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;


                if ((m_pClientList[iH] != 0) && (m_pClientList[iH]->m_iReqJoinPartyClientH == iClientH) &&
                    (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) == 0))
                {

                    m_pClientList[iH]->m_iReqJoinPartyClientH = 0;
                    memset(m_pClientList[iH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iH]->m_cReqJoinPartyName));
                }

                m_pClientList[iClientH]->m_iPartyID = 0;
                m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
                memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
            }
            break;
    }
}

void CGame::RequestItemUpgradeHandler(int iClientH, int iItemIndex)
{
    int i, iValue, iItemX, iItemY, iSoM, iSoX, iSomH, iSoxH;
    uint32_t dwTemp, dwSWEType;
    double dV1, dV2, dV3;
    short sItemUpgrade = 2;


    if (m_pClientList[iClientH] == 0) return;
    if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return;

    iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
    if (iValue >= 15 || iValue < 0)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, 0, 0, 0);
        return;
    }

    if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 717)
    {
        if (iValue >= 7 || iValue < 0)
        {

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, 0, 0, 0);
            return;
        }


    }
    switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cCategory)
    {
        case 1:
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
            {
                case 20: // ???ºÄ????ö 
                case 610:
                case 611: // Á?????-???º
                case 612:
                case 613:
                case 614:
                case 616:
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                    return;

                case 703:
                case 709:
                case 727:
                case 736:
                case 737:
                case 718:
                    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
                        return;
                    }



                    sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;

                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                        return;
                    }

                    if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
                        return;
                    }

                    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);

                    if ((iValue == 0) && m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 703)
                    {
                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;


                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;



                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 736) == false)
                        {

                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                            return;
                        }


                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 2;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
                            m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;

                    }
                    else if ((iValue == 0) && ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 709) || (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 727)))
                    {

                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;


                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;




                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 737) == false)
                        {

                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                            return;
                        }



                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 2;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
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
                        iValue += 2;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                    }
                    break;
                case 717:  //Èæ±â»çÀÇ·¹ÀÌÇÇ¾î //v2.19 2002-12-7 ÁöÁ¸ ¾ÆÀÌÅÛ °ü·Ã Ãß°¡ ·¹ÀÌÇÇ¾î °æ¿ì 15±îÁö ¿Ã¸®¸é ³Ê¹« ½ê´Ù 7·Î ¼¼ÆÃ

                    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
                        return;
                    }



                    sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;

                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                        return;
                    }

                    if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
                        return;
                    }

                    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
                    iValue += 1;
                    if (iValue > 7) iValue = 7;
                    dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                    dwTemp = dwTemp & 0x0FFFFFFF;
                    m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                    _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);

                    break;
                default:



                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != 0)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                        if (dwSWEType == 9)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                            return;
                        }
                    }


                    iSoX = iSoM = 0;
                    for (i = 0; i < DEF_MAXITEMS; i++)
                        if (m_pClientList[iClientH]->m_pItemList[i] != 0)
                        {
                            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                            {
                                case 656: iSoX++; iSoxH = i; break;
                                case 657: iSoM++; iSomH = i; break;
                            }
                        }

                    if (iSoX > 0)
                    {
                        if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == false)
                        {

                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);

                            iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28; // v2.172
                            if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false);

                            ItemDepleteHandler(iClientH, iSoxH, false);
                            return;
                        }

                        if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0)
                        {

                            iValue++;
                            if (iValue > 10)
                                iValue = 10;
                            else
                            {

                                dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                                dwTemp = dwTemp & 0x0FFFFFFF;
                                m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                                ItemDepleteHandler(iClientH, iSoxH, false);
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

                                ItemDepleteHandler(iClientH, iSoxH, false);
                            }
                        }
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                    break;
            }
            break;

        case 3: // È? 
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
            break;

        case 5:



            if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != 0)
            {
                dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                if (dwSWEType == 8)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                    return;
                }
            }
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
            {
                case 620:
                case 623:
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                    return;
                default: break;
            }


            iSoX = iSoM = 0;
            for (i = 0; i < DEF_MAXITEMS; i++)
                if (m_pClientList[iClientH]->m_pItemList[i] != 0)
                {
                    switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                    {
                        case 656: iSoX++; iSoxH = i; break;
                        case 657: iSoM++; iSomH = i; break;
                    }
                }


            if (iSoM > 0)
            {

                if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, true) == false)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                    iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28; // v2.172
                    if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false);

                    ItemDepleteHandler(iClientH, iSomH, false);
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

                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0)
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

                    ItemDepleteHandler(iClientH, iSomH, false);
                }
            }
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, 0, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);
            break;

        case 15: //v2.19 2002-12-7 ?êÅ?º? ?ü?Ã ÁöÁ? ??????ÀÌµå?? ?ÈµÈ?Ù.
        case 6: // ?æ?î?? 
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
                case 713: // ???ë?º 
                case 724:
                case 725:
                case 726:
                case 728:
                case 729:
                case 730:
                case 731:
                case 716:
                case 719:
                case 775:
                case 776:


                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                    return;

                default:



                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != 0)
                    {
                        dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
                        if (dwSWEType == 8)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                            return;
                        }
                    }
                    iSoX = iSoM = 0;
                    for (i = 0; i < DEF_MAXITEMS; i++)
                        if (m_pClientList[iClientH]->m_pItemList[i] != 0)
                        {
                            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                            {
                                case 656: iSoX++; iSoxH = i; break;
                                case 657: iSoM++; iSomH = i; break;
                            }
                        }


                    if (iSoM > 0)
                    {

                        if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, true) == false)
                        {

                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                            iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28; // v2.172
                            if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false);

                            ItemDepleteHandler(iClientH, iSomH, false);
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

                            if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0)
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

                            ItemDepleteHandler(iClientH, iSomH, false);
                        }
                    }
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, 0, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);
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

                    if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
                        (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
                        return;
                    }

                    if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
                        return;
                    }


                    sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;

                    if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
                        return;
                    }

                    m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);

                    if (iValue == 0)
                    {
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
                    }

                    if ((iValue == 4) && ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 714) || (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 732)))
                    {
                        iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
                        iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;


                        delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;

                        m_pClientList[iClientH]->m_pItemList[iItemIndex] = new CItem;

                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
                        m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;




                        if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 738) == false)
                        {

                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                            return;
                        }


                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

                        iValue += 2;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIZONITEMCHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
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
                        iValue += 2;
                        if (iValue > 15) iValue = 15;
                        dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
                        dwTemp = dwTemp & 0x0FFFFFFF;
                        m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                        _bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
                        break;
                    }

                default:


                    iSoX = iSoM = 0;
                    for (i = 0; i < DEF_MAXITEMS; i++)
                        if (m_pClientList[iClientH]->m_pItemList[i] != 0)
                        {
                            switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum)
                            {
                                case 656: iSoX++; iSoxH = i; break;
                                case 657: iSoM++; iSomH = i; break;
                            }
                        }

                    if (iSoX > 0)
                    {

                        if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == false)
                        {

                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
                            iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28; // v2.172
                            if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false);

                            ItemDepleteHandler(iClientH, iSoxH, false);
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

                            ItemDepleteHandler(iClientH, iSoxH, false);
                        }
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);

                    break;
            }
            break;

            /*	case 13: // ·Îºê
            switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
            case 715:
            case 733:
            if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) return;
            break;

            default:
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
            break;
            }
            break;
            */
        default:

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
            break;
    }
}

void CGame::ScreenSettingsHandler(std::shared_ptr<CClient> player, char * pData, uint32_t dwMsgSize)
{
    if (!player) return;

    stream_read sr(pData, dwMsgSize);

    sr.read<uint32_t>();

    player->screenwidth_v = sr.read<uint16_t>();
    player->screenheight_v = sr.read<uint16_t>();
    player->screenwidth = sr.read<uint16_t>();
    player->screenheight = sr.read<uint16_t>();
    player->screen_size_x = player->screenwidth_v / 32;
    player->screen_size_y = player->screenheight_v / 32;

    log->info("ScreenSettingsHandler: screenwidth_v {} screenheight_v {} screenwidth {} screenheight {} screen_size_x {} screen_size_y {}", player->screenwidth_v, player->screenheight_v, player->screenwidth, player->screenheight, player->screen_size_x, player->screen_size_y);
}

void CGame::RequestTeleportListHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;

    char * cp, cData[512];
    int		iRet;
    uint32_t * dwp;
    uint16_t * wp;
    int		iMapSide = -1;
    int * listCount;
    char	cNpcName[21];
    int * ip;

    if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
        iMapSide = DEF_ARESDEN;
    else if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
        iMapSide = DEF_ELVINE;


    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memset(cNpcName, 0, sizeof(cNpcName));
    strncpy(cNpcName, cp, 20);
    cp += 20;

    memset(cData, 0, sizeof(cData));
    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_TELEPORT_LIST;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    cp = cData + 6;

    listCount = (int *)cp; *listCount = 0;
    cp += 4;

    int		index;
    for (index = 0; index < DEF_MAXTELEPORTLIST; index++)
    {
        if (m_pTeleportConfigList[index] == 0) continue;


        if (strncmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName,
            m_pTeleportConfigList[index]->m_cSourceMap, 10) != 0)
            continue;


        if (strncmp(m_pTeleportConfigList[index]->m_cNpcname, cNpcName, 20) != 0)
            continue;


        if (m_pTeleportConfigList[index]->m_iMinLvl > m_pClientList[iClientH]->m_iLevel ||
            m_pTeleportConfigList[index]->m_iMaxLvl < m_pClientList[iClientH]->m_iLevel)
            continue;


        if (m_pTeleportConfigList[index]->m_bHunter == false &&
            m_pClientList[iClientH]->m_bIsHunter == true)
            continue;


        if (m_pTeleportConfigList[index]->m_bNetural == false &&
            m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
            continue;


        if (m_pTeleportConfigList[index]->m_bCriminal == false &&
            m_pClientList[iClientH]->m_iPKCount > 0)
            continue;


        if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL &&
            (m_pTeleportConfigList[index]->m_iSide == DEF_BOTHSIDE ||
                m_pTeleportConfigList[index]->m_iSide == iMapSide))
        {
            // index[4], mapname[10], X[4], Y[4], Cost[4]
            ip = (int *)cp;
            *ip = index;
            cp += 4;

            memcpy(cp, m_pTeleportConfigList[index]->m_cTargetMap, 10);
            cp += 10;

            ip = (int *)cp;
            *ip = m_pTeleportConfigList[index]->m_iX;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pTeleportConfigList[index]->m_iY;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pTeleportConfigList[index]->m_iCost;
            cp += 4;
        }
        else if ((m_pTeleportConfigList[index]->m_iSide == DEF_BOTHSIDE) ||
            (m_pTeleportConfigList[index]->m_iSide == m_pClientList[iClientH]->m_cSide))
        {
            // index[4], mapname[10], X[4], Y[4], Cost[4]
            ip = (int *)cp;
            *ip = index;
            cp += 4;

            memcpy(cp, m_pTeleportConfigList[index]->m_cTargetMap, 10);
            cp += 10;

            ip = (int *)cp;
            *ip = m_pTeleportConfigList[index]->m_iX;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pTeleportConfigList[index]->m_iY;
            cp += 4;

            ip = (int *)cp;
            *ip = m_pTeleportConfigList[index]->m_iCost;
            cp += 4;
        }
        else continue;

        (*listCount)++;
    }


    if ((*listCount) == 0)
        *wp = DEF_MSGTYPE_REJECT;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 10 + (*listCount) * 26);

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

void CGame::RequestChargedTeleportHandler(int iClientH, char * pData, uint32_t dwMsgSize)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;

    char * cp, cData[64];
    int		iRet;
    uint32_t * dwp;
    uint16_t * wp;
    int		iMapSide = -1;
    int		index;
    uint16_t	wConfirm = DEF_MSGTYPE_CONFIRM;
    short	sError = 0;


    if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
        iMapSide = DEF_ARESDEN;
    else if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
        iMapSide = DEF_ELVINE;


    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    index = (int)(*cp);
    cp += 4;

    if (index < 0 || index >= DEF_MAXTELEPORTLIST)
        return;

    if (m_pTeleportConfigList[index] == 0)
        return;


    if (strncmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName,
        m_pTeleportConfigList[index]->m_cSourceMap, 10) != 0)
        return;


    if (m_pTeleportConfigList[index]->m_iMinLvl > m_pClientList[iClientH]->m_iLevel ||
        m_pTeleportConfigList[index]->m_iMaxLvl < m_pClientList[iClientH]->m_iLevel)
    {
        wConfirm = DEF_MSGTYPE_REJECT;
        sError = 1;
    }


    if (wConfirm == DEF_MSGTYPE_CONFIRM &&
        m_pTeleportConfigList[index]->m_bHunter == false &&
        m_pClientList[iClientH]->m_bIsHunter == true)
    {
        wConfirm = DEF_MSGTYPE_REJECT;
        sError = 2;
    }


    if (wConfirm == DEF_MSGTYPE_CONFIRM &&
        m_pTeleportConfigList[index]->m_bNetural == false &&
        m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
    {
        wConfirm = DEF_MSGTYPE_REJECT;
        sError = 3;
    }


    if (wConfirm == DEF_MSGTYPE_CONFIRM &&
        m_pTeleportConfigList[index]->m_bCriminal == false &&
        m_pClientList[iClientH]->m_iPKCount > 0)
    {
        wConfirm = DEF_MSGTYPE_REJECT;
        sError = 4;
    }


    if (wConfirm == DEF_MSGTYPE_CONFIRM &&
        m_pClientList[iClientH]->m_cSide == DEF_NETURAL &&
        (m_pTeleportConfigList[index]->m_iSide != DEF_BOTHSIDE && m_pTeleportConfigList[index]->m_iSide != iMapSide))
    {
        wConfirm = DEF_MSGTYPE_REJECT;
        sError = 5;
    }
    else if (
        wConfirm == DEF_MSGTYPE_CONFIRM &&
        m_pTeleportConfigList[index]->m_iSide != DEF_BOTHSIDE &&
        m_pTeleportConfigList[index]->m_iSide != m_pClientList[iClientH]->m_cSide)
    {
        wConfirm = DEF_MSGTYPE_REJECT;
        sError = 5;
    }


    if (wConfirm == DEF_MSGTYPE_CONFIRM)
    {

        uint32_t dwGoldCount = dwGetItemCount(iClientH, "Gold");

        if (dwGoldCount >= m_pTeleportConfigList[index]->m_iCost)
        {

            int iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - m_pTeleportConfigList[index]->m_iCost);


            iCalcTotalWeight(iClientH);


            m_stCityStatus[m_pClientList[iClientH]->m_cSide].iFunds += m_pTeleportConfigList[index]->m_iCost;

        }
        else
        {
            wConfirm = DEF_MSGTYPE_REJECT;
            sError = 6;
        }
    }

    // Invalid Charged-teleport
    if (wConfirm == DEF_MSGTYPE_REJECT)
    {
        memset(cData, 0, sizeof(cData));
        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_CHARGED_TELEPORT;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = wConfirm;

        cp = cData + DEF_INDEX2_MSGTYPE + 2;

        short * sp;
        sp = (short *)cp;
        *sp = sError;
        cp += 2;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6 + 2);

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


    RequestTeleportHandler(iClientH, "2   ",
        m_pTeleportConfigList[index]->m_cTargetMap,
        m_pTeleportConfigList[index]->m_iX,
        m_pTeleportConfigList[index]->m_iY);

}

void CGame::UseSkillHandler(int iClientH, int iV1, int iV2, int iV3)
{
    char  cOwnerType;
    short sAttackerWeapon, sOwnerH;
    int   iResult, iPlayerSkillLevel;
    uint32_t dwTime = timeGetTime();

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    if ((iV1 < 0) || (iV1 >= DEF_MAXSKILLTYPE)) return;
    if (m_pSkillConfigList[iV1] == 0) return;

    if (m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] == true) return;
    m_pClientList[iClientH]->m_dwLastActionTime = dwTime;


    /*
    if (iV1 != 19) {
    m_pClientList[iClientH]->m_iAbuseCount++;
    if ((m_pClientList[iClientH]->m_iAbuseCount % 30) == 0) {
    wsprintf(G_cTxt, "(!) ÇØÅ· ¿ëÀÇÀÚ(%s) Skill(%d) Tries(%d)",m_pClientList[iClientH]->m_cCharName,
    iV1, m_pClientList[iClientH]->m_iAbuseCount);
    log->info(G_cTxt);
    }
    }
    */


    iPlayerSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[iV1];
    iResult = iDice(1, 100);

    if (iResult > iPlayerSkillLevel)
    {


        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
        return;
    }


    switch (m_pSkillConfigList[iV1]->m_sType)
    {
        case DEF_SKILLEFFECTTYPE_PRETEND:
            switch (m_pSkillConfigList[iV1]->m_sValue1)
            {
                case 1:



                    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == true)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
                        return;
                    }

                    //¸¸¾à ¹Ù´Ú¿¡ ½ÃÃ¼°¡ ÀÖ´Ù¸é Á×ÀºÃ´ ÇÏ±â¸¦ ÇÒ ¼ö ¾ø´Ù. 
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
                    if (sOwnerH != 0)
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
                        return;
                    }


                    iResult = 0;
                    if (m_pClientList[iClientH]->m_iAdminUserLevel <= 0)
                    {
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY - 1);
                        iResult += sOwnerH;
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY + 1);
                        iResult += sOwnerH;
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX - 1, m_pClientList[iClientH]->m_sY);
                        iResult += sOwnerH;
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX + 1, m_pClientList[iClientH]->m_sY);
                        iResult += sOwnerH;

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX - 1, m_pClientList[iClientH]->m_sY - 1);
                        iResult += sOwnerH;
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX + 1, m_pClientList[iClientH]->m_sY - 1);
                        iResult += sOwnerH;
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX - 1, m_pClientList[iClientH]->m_sY + 1);
                        iResult += sOwnerH;
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX + 1, m_pClientList[iClientH]->m_sY + 1);
                        iResult += sOwnerH;
                    }

                    if (iResult != 0)
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
                        return;
                    }


                    if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
                    {
                        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

                        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
                        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                    }


                    CalculateSSN_SkillIndex(iClientH, iV1, 1);


                    sAttackerWeapon = 1;
                    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, 0, sAttackerWeapon, 0);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(14, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetDeadOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
                    break;
            }
            break;

    }

    m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] = true;
}

void CGame::SetDownSkillIndexHandler(int iClientH, int iSkillIndex)
{
    if (m_pClientList[iClientH] == 0) return;
    if ((iSkillIndex < 0) || (iSkillIndex >= DEF_MAXSKILLTYPE)) return;

    if (m_pClientList[iClientH]->m_cSkillMastery[iSkillIndex] > 0)
        m_pClientList[iClientH]->m_iDownSkillIndex = iSkillIndex;


    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, 0, 0, 0);
}

void CGame::GetHeroMantleHandler(int iClientH)
{
    int   i, iNum, iRet, iEraseReq, iItemID;
    char * cp, cData[256]{};
    CItem * pItem;
    uint32_t * dwp;
    short * sp;
    uint16_t * wp;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iEnemyKillCount < 300) return;
    if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) return;

    switch (m_pClientList[iClientH]->m_cSide)
    {
        case 1: iItemID = 400; break;
        case 2: iItemID = 401; break;
    }


    iNum = 1;
    for (i = 1; i <= iNum; i++)
    {

        pItem = new CItem;
        if (_bInitItemAttr(pItem, iItemID) == false)
        {

            delete pItem;
            pItem = 0;
        }
        else
        {

            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
            {

                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;


                if (m_pClientList[iClientH]->m_iEnemyKillCount >= 300)
                {
                    m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
                }

                //testcode ·Î±×ÆÄÀÏ¿¡ ±â·ÏÇÑ´Ù.
                wsprintf(G_cTxt, "(*) Get Mantle : Char(%s) Player-EK(%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iEnemyKillCount);
                log->info(G_cTxt);


                pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;


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

                _bItemLog(DEF_ITEMLOG_GET, iClientH, (int)-1, pItem);

                /*
                *cp = (char)(pItem->m_dwAttribute & 0x00000001);
                cp++;
                */

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


                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, 0, 0, 0);
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
    }
}

void CGame::RequestSetGuildTeleportLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName)
{
    char * cp, cData[120];
    int i;
    int * ip, iIndex;
    uint32_t dwTemp, dwTime;

    if (m_pClientList[iClientH] == 0) return;


    if (dY < 100) dY = 100;
    if (dY > 600) dY = 600;


    memset(cData, 0, sizeof(cData));
    cp = (char *)cData;
    *cp = GSM_SETGUILDTELEPORTLOC;
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
    //

    dwTime = timeGetTime();

    //testcode
    wsprintf(G_cTxt, "SetGuildTeleportLoc: %d %s %d %d", iGuildGUID, pMapName, dX, dY);
    log->info(G_cTxt);


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
                memset(m_pGuildTeleportLoc[i].m_cDestMapName, 0, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
                strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
                m_pGuildTeleportLoc[i].m_dwTime = dwTime;
                return;
            }
        }


    dwTemp = 0;
    iIndex = -1;
    for (i = 0; i < DEF_MAXGUILDS; i++)
    {
        if (m_pGuildTeleportLoc[i].m_iV1 == 0)
        {

            m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
            m_pGuildTeleportLoc[i].m_sDestX = dX;
            m_pGuildTeleportLoc[i].m_sDestY = dY;
            memset(m_pGuildTeleportLoc[i].m_cDestMapName, 0, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
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

    //testcode
    log->info("(X) No more GuildTeleportLoc Space! Replaced.");

    m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
    m_pGuildTeleportLoc[i].m_sDestX = dX;
    m_pGuildTeleportLoc[i].m_sDestY = dY;
    memset(m_pGuildTeleportLoc[i].m_cDestMapName, 0, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
    strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
    m_pGuildTeleportLoc[i].m_dwTime = dwTime;
}


void CGame::RequestGuildTeleportHandler(int iClientH)
{
    int i;
    char cMapName[11];

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iLockedMapTime != 0)
    {

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LOCKEDMAP, m_pClientList[iClientH]->m_iLockedMapTime, 0, 0, m_pClientList[iClientH]->m_cLockedMapName);
        return;
    }

    for (i = 0; i < DEF_MAXGUILDS; i++)
        if (m_pGuildTeleportLoc[i].m_iV1 == m_pClientList[iClientH]->m_iGuildGUID)
        {

            memset(cMapName, 0, sizeof(cMapName));
            strcpy(cMapName, m_pGuildTeleportLoc[i].m_cDestMapName);

            //testcode
            wsprintf(G_cTxt, "ReqGuildTeleport: %d %d %d %s", m_pClientList[iClientH]->m_iGuildGUID, m_pGuildTeleportLoc[i].m_sDestX, m_pGuildTeleportLoc[i].m_sDestY, cMapName);
            log->info(G_cTxt);


            RequestTeleportHandler(iClientH, "2   ", cMapName, m_pGuildTeleportLoc[i].m_sDestX, m_pGuildTeleportLoc[i].m_sDestY);
            return;
        }


    switch (m_pClientList[iClientH]->m_cSide)
    {
        case 1:
            break;
        case 2:
            break;
    }
}

void CGame::RequestHelpHandler(int iClientH)
{

    int i;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iGuildRank == -1) return;
    if (m_pClientList[iClientH]->m_iCrusadeDuty != 1) return;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iGuildRank == 0) &&
            (m_pClientList[i]->m_iCrusadeDuty == 3) && (m_pClientList[i]->m_iGuildGUID == m_pClientList[iClientH]->m_iGuildGUID))
        {

            SendNotifyMsg(NULL, i, DEF_NOTIFY_HELP, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_iHP, m_pClientList[iClientH]->m_cCharName);
            return;
        }


    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELPFAILED, 0, 0, 0, 0);
}

void CGame::RequestGuildNameHandler(int iClientH, int iObjectID, int iIndex)
{
    if (m_pClientList[iClientH] == 0) return;
    if ((iObjectID <= 0) || (iObjectID >= DEF_MAXCLIENTS)) return;

    if (m_pClientList[iObjectID] == 0)
    {


    }
    else
    {

        // SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REQGUILDNAMEANSWER, iObjectID, iIndex, m_pClientList[iObjectID]->m_iGuildRank, m_pClientList[iObjectID]->m_cGuildName);
        // SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REQGUILDNAMEANSWER, m_pClientList[iObjectID]->m_iGuildRank, iIndex, m_pClientList[iObjectID]->m_iGuildRank, m_pClientList[iObjectID]->m_cGuildName);
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REQGUILDNAMEANSWER, m_pClientList[iObjectID]->m_iGuildRank, iIndex, 0, m_pClientList[iObjectID]->m_cGuildName);
    }
}

void CGame::RequestPlayerNameHandler(int iClientH, int iObjectID, int iIndex)
{
    if (m_pClientList[iClientH] == 0) return;
    if ((iObjectID <= 0) || (iObjectID >= DEF_MAXCLIENTS)) return;

    if (m_pClientList[iObjectID] == 0)
    {


    }
    else
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REQPLAYERNAMEANSWER, iIndex, m_pClientList[iObjectID]->m_iAdminUserLevel, m_pClientList[iObjectID]->m_iPKCount, m_pClientList[iObjectID]->m_cCharName, m_pClientList[iObjectID]->m_bEnabled, 0, 0, 0, 0, 0, m_pClientList[iObjectID]->m_cProfile);
    }
}

void CGame::ToggleCombatModeHandler(int iClientH)
{
    short sAppr2;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;

    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;

    sAppr2 = (short)((m_pClientList[iClientH]->m_sAppr2 & 0xF000) >> 12);

    m_pClientList[iClientH]->m_bIsAttackModeChange = true; // v2.172


    if (sAppr2 == 0)
    {

        m_pClientList[iClientH]->m_sAppr2 = (0xF000 | m_pClientList[iClientH]->m_sAppr2);
    }
    else
    {

        m_pClientList[iClientH]->m_sAppr2 = (0x0FFF & m_pClientList[iClientH]->m_sAppr2);
    }


    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

}

void CGame::SpecialEventHandler()
{
    uint32_t dwTime;


    dwTime = timeGetTime();


    if ((dwTime - m_dwSpecialEventTime) < DEF_MOBEVENTTIME) return;
    m_dwSpecialEventTime = dwTime;
    m_bIsSpecialEventTime = true;

    switch (iDice(1, 350))
    {
        case 98: m_cSpecialEventType = 2; break;
        default: m_cSpecialEventType = 1; break;
    }
}


void CGame::ToggleSafeAttackModeHandler(int iClientH) //v1.1
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;


    if (m_pClientList[iClientH]->m_bIsSafeAttackMode == true)
        m_pClientList[iClientH]->m_bIsSafeAttackMode = false;
    else m_pClientList[iClientH]->m_bIsSafeAttackMode = true;

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SAFEATTACKMODE, 0, 0, 0, 0);
}

void CGame::QuestAcceptedHandler(int iClientH)
{
    int iIndex;

    if (m_pClientList[iClientH] == 0) return;

    if (m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_iAssignType == 1)
    {

        switch (m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_iType)
        {
            case 10:
                _ClearQuestStatus(iClientH);
                RequestTeleportHandler(iClientH, "2   ", m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_cTargetName,
                    m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_sX, m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_sY);
                return;
        }
    }


    m_pClientList[iClientH]->m_iQuest = m_pClientList[iClientH]->m_iAskedQuest;
    iIndex = m_pClientList[iClientH]->m_iQuest;
    m_pClientList[iClientH]->m_iQuestID = m_pQuestConfigList[iIndex]->m_iQuestID;
    m_pClientList[iClientH]->m_iCurQuestCount = 0;
    m_pClientList[iClientH]->m_bIsQuestCompleted = false;

    _CheckQuestEnvironment(iClientH);
    _SendQuestContents(iClientH);
}
