//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "tile.h"
#include "map.h"

int CGame::iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, bool bIsRun)
{
    char * cp, cData[3000]{};
    CTile * pTile;
    uint32_t * dwp, dwTime;
    uint16_t * wp, wObjectID;
    short * sp, dX, dY, sDOtype;
    int	 iTemp, iTemp2;
    int * ip, iRet, iSize, iDamage;
    bool  bRet;

    if (m_pClientList[iClientH] == 0) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;


    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

    dwTime = timeGetTime();
    m_pClientList[iClientH]->m_dwLastActionTime = dwTime;

// #ifndef NO_MSGSPEEDCHECK
//     if (bIsRun == false)
//     {
//         m_pClientList[iClientH]->m_iMoveMsgRecvCount++;
// 
//         if (m_pClientList[iClientH]->m_iMoveMsgRecvCount >= 7)
//         {
//             if (m_pClientList[iClientH]->m_dwMoveLAT != 0)
//             {
// 
//                 if ((dwTime - m_pClientList[iClientH]->m_dwMoveLAT) < (72 * 8 * 7 - 3000))
//                 {
//                     DeleteClient(iClientH, true, true);
//                     return 0;
//                 }
//             }
//             m_pClientList[iClientH]->m_dwMoveLAT = dwTime;
//             m_pClientList[iClientH]->m_iMoveMsgRecvCount = 0;
//         }
//     }
//     else
//     {
//         m_pClientList[iClientH]->m_iRunMsgRecvCount++;
// 
//         if (m_pClientList[iClientH]->m_iRunMsgRecvCount >= 7)
//         {
//             if (m_pClientList[iClientH]->m_dwRunLAT != 0)
//             {
// 
//                 if ((dwTime - m_pClientList[iClientH]->m_dwRunLAT) < (43 * 8 * 7 - 1500))
//                 {
//                     DeleteClient(iClientH, true, true);
//                     return 0;
//                 }
//             }
//             m_pClientList[iClientH]->m_dwRunLAT = dwTime;
//             m_pClientList[iClientH]->m_iRunMsgRecvCount = 0;
//         }
//     }
// #endif


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


    ClearSkillUsingStatus(iClientH);

    dX = m_pClientList[iClientH]->m_sX;
    dY = m_pClientList[iClientH]->m_sY;

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

    CItem * pTopItem = 0;

    bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(dX, dY, &sDOtype, pTopItem);


    if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
        bRet = false;

    if (bRet == true)
    {


        if (m_pClientList[iClientH]->m_iQuest != 0) _bCheckIsQuestCompleted(iClientH);



        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(1, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);

        m_pClientList[iClientH]->m_sX = dX;
        m_pClientList[iClientH]->m_sY = dY;
        m_pClientList[iClientH]->m_cDir = cDir;

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner((short)iClientH, DEF_OWNERTYPE_PLAYER, dX, dY);

        if (sDOtype == DEF_DYNAMICOBJECT_SPIKE)
        {
            if ((m_pClientList[iClientH]->m_bIsNeutral == true) && ((m_pClientList[iClientH]->m_sAppr2 & 0xF000) == 0))
            {

            }
            else
            {
                iDamage = iDice(2, 4);

                if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
                    m_pClientList[iClientH]->m_iHP -= iDamage;
            }
        }

        /*
        // v2.172
        short sRemainItemSprite, sRemainItemSpriteFrame;
        char cRemainItemColor;

        switch (pTopItem->m_sIDnum) {
        case 540:
        if (m_pClientList[iClientH]->m_cSide == 2) {

        if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS] != -1) &&
        (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS]] != 0) &&
        (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS]]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_ANTIMINE)) {

        }
        else {


        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, (61+100), m_pClientList[iClientH]->m_sType);
        }


        pTopItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);

        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, m_pClientList[iClientH]->m_cMapIndex,
        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
        sRemainItemSprite, sRemainItemSpriteFrame, cRemainItemColor);
        delete pTopItem;
        }
        break;
        case 541:
        if (m_pClientList[iClientH]->m_cSide == 1) {

        if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS] != -1) &&
        (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS]] != 0) &&
        (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS]]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_ANTIMINE)) {

        }
        else {


        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, (61+100), m_pClientList[iClientH]->m_sType);
        }


        pTopItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);

        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, m_pClientList[iClientH]->m_cMapIndex,
        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
        sRemainItemSprite, sRemainItemSpriteFrame, cRemainItemColor);
        delete pTopItem;
        }
        break;
        }
        //
        */

        if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 0;

        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_MOTION;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_OBJECTMOVE_CONFIRM;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        sp = (short *)cp;
        *sp = (short)(dX - 10);
        cp += 2;

        sp = (short *)cp;
        *sp = (short)(dY - 7);
        cp += 2;

        *cp = cDir;
        cp++;

        if (bIsRun == true)
        {
            if (m_pClientList[iClientH]->m_iSP > 0)
            {
                *cp = 0;
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[iClientH]->m_iSP--;
                    *cp = 1;
                }
            }
            else
            {
                *cp = 0;
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[iClientH]->m_iSP--;
                    *cp = 1;
                }
                if (m_pClientList[iClientH]->m_iSP < -10)
                {


                    m_pClientList[iClientH]->m_iSP = 0;
                    DeleteClient(iClientH, true, true);
                    return 0;
                }
            }
        }
        else *cp = 0;
        cp++;

        pTile = (CTile *)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile + dX + dY * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);
        *cp = (char)pTile->m_iOccupyStatus;
        cp++;

        ip = (int *)cp;
        *ip = m_pClientList[iClientH]->m_iHP;
        cp += 4;

        iSize = iComposeMoveMapData((short)(dX - 10), (short)(dY - 7), iClientH, cDir, cp);

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
    }
    else
    {
        m_pClientList[iClientH]->m_bIsMoveBlocked = true;

        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_MOTION;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_OBJECTMOVE_REJECT;

        wObjectID = (WORD)iClientH;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        wp = (uint16_t *)cp;
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



        iTemp = m_pClientList[wObjectID]->m_iStatus;
        iTemp = 0x0FFFFFFF & iTemp;

        iTemp2 = (short)iGetPlayerABSStatus(wObjectID, iClientH);
        iTemp = (iTemp | (iTemp2 << 28));
        *ip = iTemp;
        cp += 4;

        /*
        if (m_pClientList[iClientH]->m_iPKCount != 0) {

        sTemp = sTemp | ((2) << 12);
        }
        else if (m_pClientList[wObjectID]->m_iPKCount != 0) {

        sTemp = sTemp | ((2) << 12);
        }
        else {
        if (m_pClientList[iClientH]->m_cSide != m_pClientList[wObjectID]->m_cSide) {
        if ( (m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[wObjectID]->m_cSide != 0) ) {

        sTemp = sTemp | ((2) << 12);
        }
        else {
        sTemp = sTemp | ((0) << 12);
        }
        }
        else {

        if ( (memcmp(m_pClientList[wObjectID]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName, 20) == 0) &&
        (memcmp(m_pClientList[wObjectID]->m_cGuildName, "NONE", 4) != 0) ) {

        if (m_pClientList[wObjectID]->m_iGuildRank == 0)
        sTemp = sTemp | ((5) << 12);
        else sTemp = sTemp | ((3) << 12);
        }
        else
        if ( (memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[wObjectID]->m_cLocation, 10) == 0) &&
        (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0) &&
        (memcmp(m_pClientList[wObjectID]->m_cGuildName, "NONE", 4) != 0) &&
        (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[wObjectID]->m_cGuildName, 20) != 0) ) {

        sTemp = sTemp | ((4) << 12);
        }
        else sTemp = sTemp | ((1) << 12);
        }
        }

        *sp = sTemp;
        cp += 2;
        */

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 40 + 2);

        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:

                DeleteClient(iClientH, true, true);
                return 0;
        }
        return 0;
    }

    return 1;
}

int CGame::iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType, char cDir, uint16_t wTargetObjectID, bool bResponse, bool bIsDash)
{
    char cData[100]{};
    uint32_t * dwp, dwTime;
    uint16_t * wp;
    int     iRet, iExp, tdX, tdY;
    short   sOwner, sAbsX, sAbsY;
    char    cOwnerType;
    bool    bNearAttack = false;


    if (m_pClientList[iClientH] == 0) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;

    dwTime = timeGetTime();
    m_pClientList[iClientH]->m_dwLastActionTime = dwTime;

#ifndef NO_MSGSPEEDCHECK	
    m_pClientList[iClientH]->m_iAttackMsgRecvCount++;
    if (m_pClientList[iClientH]->m_iAttackMsgRecvCount >= 7)
    {
        if (m_pClientList[iClientH]->m_dwAttackLAT != 0)
        {

            if ((dwTime - m_pClientList[iClientH]->m_dwAttackLAT) < (80 * 8 * 7 - 3000))
            {

                DeleteClient(iClientH, true, true);
                return 0;
            }
        }
        m_pClientList[iClientH]->m_dwAttackLAT = dwTime;
        m_pClientList[iClientH]->m_iAttackMsgRecvCount = 0;
    }
#endif

    if ((wTargetObjectID != 0) && (wType != 2))
    {

        tdX = 0;
        tdY = 0;

        if (wTargetObjectID < DEF_MAXCLIENTS)
        {
            if (m_pClientList[wTargetObjectID] != 0)
            {
                tdX = m_pClientList[wTargetObjectID]->m_sX;
                tdY = m_pClientList[wTargetObjectID]->m_sY;
            }
        }
        else if ((wTargetObjectID > 10000) && (wTargetObjectID < (10000 + DEF_MAXNPCS)))
        {
            if (m_pNpcList[wTargetObjectID - 10000] != 0)
            {
                tdX = m_pNpcList[wTargetObjectID - 10000]->m_sX;
                tdY = m_pNpcList[wTargetObjectID - 10000]->m_sY;
            }
        }


        if ((tdX == dX) && (tdY == dY))
        {

            bNearAttack = false;
        }
        else if ((abs(tdX - dX) <= 1) && (abs(tdY - dY) <= 1))
        {
            dX = tdX;
            dY = tdY;
            bNearAttack = true;
        }
    }

    if ((dX < 0) || (dX >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeX) ||
        (dY < 0) || (dY >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY)) return 0;


    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;


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


    sAbsX = abs(sX - dX);
    sAbsY = abs(sY - dY);
    if ((wType != 2) && (wType < 20))
    {
        if ((sAbsX > 1) || (sAbsY > 1)) wType = 0;
    }


    ClearSkillUsingStatus(iClientH);


    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    m_pClientList[iClientH]->m_cDir = cDir;

    iExp = 0;

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, dX, dY);

    if (sOwner != 0)
    {

        if ((wType != 0) && ((dwTime - m_pClientList[iClientH]->m_dwRecentAttackTime) > 100))
        {
            iExp = iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash);

            if (m_pClientList[iClientH] == 0) return 0;
            m_pClientList[iClientH]->m_dwRecentAttackTime = dwTime;
        }
    }
    else _CheckMiningAction(iClientH, dX, dY);

    if (iExp != 0)
    {
        GetExp(iClientH, iExp, true);
    }


    if (bResponse == true)
    {
        dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_MOTION;
        wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_OBJECTMOTION_ATTACK_CONFIRM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:

                DeleteClient(iClientH, true, true);
                return 0;
        }
    }

    return 1;
}

int CGame::iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir)
{
    uint32_t * dwp;
    uint16_t * wp;
    char * cp;
    short * sp, sRemainItemSprite, sRemainItemSpriteFrame;
    char  cRemainItemColor, cData[100]{};
    int   iRet, iEraseReq;
    CItem * pItem;

    if (m_pClientList[iClientH] == 0) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;


    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;


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

    ClearSkillUsingStatus(iClientH);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    pItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(sX, sY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);
    if (pItem != 0)
    {
        if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
        {
            _bItemLog(DEF_ITEMLOG_GET, iClientH, (int)-1, pItem);

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

            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                sRemainItemSprite, sRemainItemSpriteFrame, cRemainItemColor);

            iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:

                    DeleteClient(iClientH, true, true);
                    return 0;
            }
        }
        else
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(sX, sY, pItem);

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
                    return 0;
            }
        }
    }

    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOTION_CONFIRM;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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

int CGame::iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir)
{
    char cData[100]{};
    uint32_t * dwp;
    uint16_t * wp;
    int     iRet;
    short   sOwnerH;
    char    cOwnerType;

    if (m_pClientList[iClientH] == 0) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;


    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;


    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true)
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, sX, sY);
        if (sOwnerH != 0)
        {
            DeleteClient(iClientH, true, true);
            return 0;
        }
    }


    ClearSkillUsingStatus(iClientH);


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


    m_pClientList[iClientH]->m_cDir = cDir;


    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);



    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOTION_CONFIRM;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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

int CGame::iClientMotion_Magic_Handler(int iClientH, short sX, short sY, char cDir)
{
    char cData[100]{};
    uint32_t * dwp;
    uint16_t * wp;
    int     iRet;

    if (m_pClientList[iClientH] == 0) return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;


    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;


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


    ClearSkillUsingStatus(iClientH);


    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);


    if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
    {
        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
    }

    m_pClientList[iClientH]->m_cDir = cDir;


    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOTION_CONFIRM;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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

