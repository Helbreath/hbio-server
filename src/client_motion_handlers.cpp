//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

int CGame::iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, char cMoveType)
{
    char * cp, cData[3000]{};
    CTile * pTile;
    DWORD * dwp, dwTime;
    WORD * wp, wObjectID;
    short * sp, dX, dY, sTemp, sTemp2, sDOtype, pTopItem;
    int * ip, iSize, iDamage;
    std::size_t iRet;
    BOOL  bRet, bIsBlocked = FALSE;

    if (m_pClientList[iClientH] == NULL) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

    dwTime = timeGetTime();
    /*m_pClientList[iClientH]->m_dwLastActionTime = dwTime;
    if (cMoveType == 2) {
        if (m_pClientList[iClientH]->m_iRecentWalkTime > dwTime) {
            m_pClientList[iClientH]->m_iRecentWalkTime = dwTime;
            if (m_pClientList[iClientH]->m_sV1 < 1) {
                if (m_pClientList[iClientH]->m_iRecentWalkTime < dwTime) {
                    m_pClientList[iClientH]->m_sV1++;
                }
                else {
                    bIsBlocked = TRUE;
                    m_pClientList[iClientH]->m_sV1 = 0;
                }
            }
        m_pClientList[iClientH]->m_iRecentWalkTime = dwTime;
        }
        if (bIsBlocked == FALSE) m_pClientList[iClientH]->m_iMoveMsgRecvCount++;
        if (m_pClientList[iClientH]->m_iMoveMsgRecvCount >= 3) {
            if (m_pClientList[iClientH]->m_dwMoveLAT != 0) {
                if ((dwTime - m_pClientList[iClientH]->m_dwMoveLAT) < (590)) {
                    //wsprintf(G_cTxt, "3.51 Walk Speeder: (%s) Player: (%s) walk difference: %d. Speed Hack?", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, dwTime - m_pClientList[iClientH]->m_dwMoveLAT);
                    //log->info(G_cTxt);
                    bIsBlocked = TRUE;
                }
            }
            m_pClientList[iClientH]->m_dwMoveLAT = dwTime;
            m_pClientList[iClientH]->m_iMoveMsgRecvCount = 0;
        }
    }
    else if (cMoveType == 1) {
        if (m_pClientList[iClientH]->m_iRecentRunTime > dwTime) {
            m_pClientList[iClientH]->m_iRecentRunTime = dwTime;
            if (m_pClientList[iClientH]->m_sV1 < 1) {
                if (m_pClientList[iClientH]->m_iRecentRunTime < dwTime) {
                    m_pClientList[iClientH]->m_sV1++;
                }
                else {
                    bIsBlocked = TRUE;
                    m_pClientList[iClientH]->m_sV1 = 0;
                }
            }
        m_pClientList[iClientH]->m_iRecentRunTime = dwTime;
        }
        if (bIsBlocked == FALSE) m_pClientList[iClientH]->m_iRunMsgRecvCount++;
        if (m_pClientList[iClientH]->m_iRunMsgRecvCount >= 3) {
            if (m_pClientList[iClientH]->m_dwRunLAT != 0) {
                if ((dwTime - m_pClientList[iClientH]->m_dwRunLAT) < (290)) {
                    //wsprintf(G_cTxt, "3.51 Run Speeder: (%s) Player: (%s) run difference: %d. Speed Hack?", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, dwTime - m_pClientList[iClientH]->m_dwRunLAT);
                    //log->info(G_cTxt);
                    bIsBlocked = TRUE;
                }
            }
            m_pClientList[iClientH]->m_dwRunLAT	= dwTime;
            m_pClientList[iClientH]->m_iRunMsgRecvCount = 0;
        }
    }*/

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

    pTopItem = 0;
    bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(dX, dY, &sDOtype, &pTopItem);

    if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
        bRet = FALSE;

    if ((bRet == TRUE) && (bIsBlocked == FALSE))
    {
        if (m_pClientList[iClientH]->m_iQuest != NULL) _bCheckIsQuestCompleted(iClientH);

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(1, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);

        m_pClientList[iClientH]->m_sX = dX;
        m_pClientList[iClientH]->m_sY = dY;
        m_pClientList[iClientH]->m_cDir = cDir;

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner((short)iClientH,
            DEF_OWNERTYPE_PLAYER,
            dX, dY);

        if (sDOtype == DEF_DYNAMICOBJECT_SPIKE)
        {
            if ((m_pClientList[iClientH]->m_bIsNeutral == TRUE) && ((m_pClientList[iClientH]->m_sAppr2 & 0xF000) == 0))
            {

            }
            else
            {
                iDamage = iDice(2, 4);

                if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
                    m_pClientList[iClientH]->m_iHP -= iDamage;
            }
        }

        if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 0;

        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_MOTION;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
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

        if (cMoveType == 1)
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
                    DeleteClient(iClientH, TRUE, TRUE);
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
                DeleteClient(iClientH, TRUE, TRUE);
                return 0;
        }

        /*if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->3CA18h == TRUE) {

            .text:00406037                 mov     [ebp+var_C1C], 0
            .text:0040603E                 xor     edx, edx
            .text:00406040                 mov     [ebp+var_C1B], edx
            .text:00406046                 mov     [ebp+var_C17], edx
            .text:0040604C                 mov     [ebp+var_C13], dx

            bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->sub_4C0F20(dX, dY, cTemp, wV1, wV2);
            if (bRet == 1) {
                RequestTeleportHandler(iClientH, "2   ", cTemp, wV1, wV2);
            }
        }*/
    }
    else
    {
        m_pClientList[iClientH]->m_bIsMoveBlocked = TRUE;

        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_MOTION;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_OBJECTMOVE_REJECT;
        if (bIsBlocked == TRUE)
        {
            m_pClientList[iClientH]->m_dwAttackLAT = 1050;
        }
        m_pClientList[iClientH]->m_dwAttackLAT = 1010;

        wObjectID = (WORD)iClientH;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

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

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 42);
        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:
                DeleteClient(iClientH, TRUE, TRUE);
                return 0;
        }

        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
        return 0;
    }

    return 1;
}

int CGame::iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType, char cDir, WORD wTargetObjectID, BOOL bResponse, BOOL bIsDash)
{
    char cData[100]{};
    DWORD * dwp, dwTime;
    WORD * wp;
    int     iRet, iExp, tdX = 0, tdY = 0, i;
    short   sOwner, sAbsX, sAbsY;
    char    cOwnerType;
    BOOL    bNearAttack = FALSE, var_AC = FALSE;
    short sItemIndex;
    int tX, tY, iErr, iStX, iStY;

    if (m_pClientList[iClientH] == NULL) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return 0;

    dwTime = timeGetTime();
    m_pClientList[iClientH]->m_dwLastActionTime = dwTime;
    m_pClientList[iClientH]->m_iAttackMsgRecvCount++;
    if (m_pClientList[iClientH]->m_iAttackMsgRecvCount >= 7)
    {
        if (m_pClientList[iClientH]->m_dwAttackLAT != 0)
        {
            if ((dwTime - m_pClientList[iClientH]->m_dwAttackLAT) < (3500))
            {
                DeleteClient(iClientH, TRUE, TRUE, TRUE);
                return 0;
            }
        }
        m_pClientList[iClientH]->m_dwAttackLAT = dwTime;
        m_pClientList[iClientH]->m_iAttackMsgRecvCount = 0;
    }

    if ((wTargetObjectID != NULL) && (wType != 2))
    {
        if (wTargetObjectID < DEF_MAXCLIENTS)
        {
            if (m_pClientList[wTargetObjectID] != NULL)
            {
                tdX = m_pClientList[wTargetObjectID]->m_sX;
                tdY = m_pClientList[wTargetObjectID]->m_sY;
            }
        }
        else if ((wTargetObjectID > 10000) && (wTargetObjectID < (10000 + DEF_MAXNPCS)))
        {
            if (m_pNpcList[wTargetObjectID - 10000] != NULL)
            {
                tdX = m_pNpcList[wTargetObjectID - 10000]->m_sX;
                tdY = m_pNpcList[wTargetObjectID - 10000]->m_sY;
            }
        }

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, dX, dY);
        if (sOwner == (wTargetObjectID - 10000))
        {
            tdX = m_pNpcList[sOwner]->m_sX;
            dX = tdX;
            tdY = m_pNpcList[sOwner]->m_sY;
            dY = tdY;
            bNearAttack = FALSE;
            var_AC = TRUE;
        }
        if (var_AC != TRUE)
        {
            if ((tdX == dX) && (tdY == dY))
            {
                bNearAttack = FALSE;
            }
            else if ((abs(tdX - dX) <= 1) && (abs(tdY - dY) <= 1))
            {
                dX = tdX;
                dY = tdY;
                bNearAttack = TRUE;
            }
        }
    }

    if ((dX < 0) || (dX >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeX) ||
        (dY < 0) || (dY >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY)) return 0;

    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

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

    sAbsX = abs(sX - dX);
    sAbsY = abs(sY - dY);
    if ((wType != 2) && (wType < 20))
    {
        if (var_AC == FALSE)
        {
            sItemIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
            if (sItemIndex != -1)
            {
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return 0;
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 845)
                {
                    if ((sAbsX > 4) || (sAbsY > 4)) wType = 0;
                }
                else
                {
                    if ((sAbsX > 1) || (sAbsY > 1)) wType = 0;
                }
            }
            else
            {
                if ((sAbsX > 1) || (sAbsY > 1)) wType = 0;
            }
        }
        else
        {
            cDir = m_Misc.cGetNextMoveDir(sX, sY, dX, dY);
            if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bCheckFlySpaceAvailable(sX, sY, cDir, sOwner)) != FALSE)
                wType = 0;
        }
    }

    ClearSkillUsingStatus(iClientH);
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    m_pClientList[iClientH]->m_cDir = cDir;

    iExp = 0;
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, dX, dY);

    if (sOwner != NULL)
    {
        if ((wType != 0) && ((dwTime - m_pClientList[iClientH]->m_dwRecentAttackTime) > 100))
        {
            if ((m_pClientList[iClientH]->m_pIsProcessingAllowed == FALSE) && (m_pClientList[iClientH]->m_bIsInsideWarehouse == FALSE)
                && (m_pClientList[iClientH]->m_bIsInsideWizardTower == FALSE) && (m_pClientList[iClientH]->m_bIsInsideOwnTown == FALSE))
            {
                sItemIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
                if (sItemIndex != -1 && m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL)
                {
                    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 874)
                    {
                        for (i = 2; i < 10; i++)
                        {
                            iErr = 0;
                            m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, tX, tY);
                            //iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, tX, tY, wType, bNearAttack, bIsDash, TRUE);
                            if ((abs(tdX - dX) <= 1) && (abs(tdY - dY) <= 1))
                            {
                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, dX, dY);
                                //iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, FALSE);
                            }
                        }
                    }
                    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 873)
                    {
                        if ((m_pClientList[iClientH]->m_sAppr2 & 0xF000) != 0)
                        {
                            if (m_bHeldenianInitiated != 1)
                            {
                                iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, DEF_DYNAMICOBJECT_FIRE3, m_pClientList[iClientH]->m_cMapIndex, dX, dY, (iDice(1, 7) + 3) * 1000, 8);
                            }
                            iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, FALSE);
                        }
                    }
                    else
                    {
                        iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, FALSE);
                    }
                }
                else
                {
                    iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, FALSE);
                }
            }
            else
            {
                iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, FALSE);
            }
            if (m_pClientList[iClientH] == NULL) return 0;
            m_pClientList[iClientH]->m_dwRecentAttackTime = dwTime;
        }
    }
    else _CheckMiningAction(iClientH, dX, dY);

    if (iExp != 0)
    {
        GetExp(iClientH, iExp, TRUE);
    }

    if (bResponse == TRUE)
    {
        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_RESPONSE_MOTION;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_OBJECTMOTION_ATTACK_CONFIRM;

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:
                DeleteClient(iClientH, TRUE, TRUE);
                return 0;
        }
    }

    return 1;
}

int CGame::iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir)
{
    DWORD * dwp;
    WORD * wp;
    char * cp;
    short * sp, sRemainItemSprite, sRemainItemSpriteFrame;
    char  cRemainItemColor, cData[100]{};
    int   iRet, iEraseReq;
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

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

    ClearSkillUsingStatus(iClientH);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    pItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(sX, sY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);
    if (pItem != NULL)
    {
        if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
        {

            _bItemLog(DEF_ITEMLOG_GET, iClientH, NULL, pItem);

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
                    DeleteClient(iClientH, TRUE, TRUE);
                    return 0;
            }
        }
        else
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(sX, sY, pItem);

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
                    return 0;
            }
        }
    }

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOTION_CONFIRM;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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

int CGame::iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir)
{
    char cData[100]{};
    DWORD * dwp;
    WORD * wp;
    int     iRet;
    short   sOwnerH;
    char    cOwnerType;

    if (m_pClientList[iClientH] == NULL) return 0;
    if ((cDir <= 0) || (cDir > 8))       return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == TRUE)
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, sX, sY);
        if (sOwnerH != NULL)
        {
            DeleteClient(iClientH, TRUE, TRUE);
            return 0;
        }
    }

    ClearSkillUsingStatus(iClientH);

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

    m_pClientList[iClientH]->m_cDir = cDir;

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOTION_CONFIRM;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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

int CGame::iClientMotion_Magic_Handler(int iClientH, short sX, short sY, char cDir)
{
    char  cData[100]{};
    DWORD * dwp;
    WORD * wp;
    int     iRet;

    if (m_pClientList[iClientH] == NULL) return 0;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return 0;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return 0;

    if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

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

    ClearSkillUsingStatus(iClientH);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

    if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
    {
        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
    }

    m_pClientList[iClientH]->m_cDir = cDir;

    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_MOTION;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_OBJECTMOTION_CONFIRM;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
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
