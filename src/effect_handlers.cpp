//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

void CGame::Effect_Damage_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3, BOOL bExp, int iAttr)
{
    int iPartyID, iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iExp, iMaxSuperAttack, iRepDamage;
    char cAttackerSide, cDamageMoveDir;
    DWORD dwTime;
    double dTmp1, dTmp2, dTmp3;
    short sAtkX, sAtkY, sTgtX, sTgtY, dX, dY, sItemIndex;

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == NULL) return;

    if (cAttackerType == DEF_OWNERTYPE_NPC)
        if (m_pNpcList[sAttackerH] == NULL) return;

    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] != 0) &&
        (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsHeldenianMap == 1) && (m_bHeldenianInitiated == TRUE)) return;

    dwTime = timeGetTime();
    iDamage = iDice(sV1, sV2) + sV3;
    if (iDamage <= 0) iDamage = 0;

    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if ((m_bAdminSecurity == TRUE) && (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0)) return;
            if (m_pClientList[sAttackerH]->m_cHeroArmorBonus == 2) iDamage += 4;
            if ((m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] == -1) || (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] == -1))
            {
                sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL))
                {
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 732 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 738)
                    {
                        iDamage *= (int)1.2;
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 863 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 864)
                    {
                        if (m_pClientList[sAttackerH]->m_iRating > 0)
                        {
                            iRepDamage = m_pClientList[sAttackerH]->m_iRating / 100;
                            if (iRepDamage < 5) iRepDamage = 5;
                            if (iRepDamage > 15) iRepDamage = 15;
                            iDamage += iRepDamage;
                        }
                        if (cTargetType == DEF_OWNERTYPE_PLAYER)
                        {
                            if (m_pClientList[sTargetH] != NULL)
                            {
                                if (m_pClientList[sTargetH]->m_iRating < 0)
                                {
                                    iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 10);
                                    if (iRepDamage > 10) iRepDamage = 10;
                                    iDamage += iRepDamage;
                                }
                            }
                        }
                    }
                }
                sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_NECK];
                if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL))
                {
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859)
                    { // NecklaceOfKloness  
                        if (cTargetType == DEF_OWNERTYPE_PLAYER)
                        {
                            if (m_pClientList[sTargetH] != NULL)
                            {
                                iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 20);
                                if (iRepDamage > 5) iRepDamage = 5;
                                iDamage += iRepDamage;
                            }
                        }
                    }
                }
            }

            if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == TRUE) && (cTargetType == DEF_OWNERTYPE_PLAYER)) return;

            dTmp1 = (double)iDamage;
            if (m_pClientList[sAttackerH]->m_iMag <= 0)
                dTmp2 = 1.0f;
            else dTmp2 = (double)m_pClientList[sAttackerH]->m_iMag;
            dTmp2 = dTmp2 / 3.3f;
            dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
            iDamage = (int)(dTmp3 + 0.5f);

            iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;
            if (iDamage <= 0) iDamage = 0;

            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                iDamage += iDamage / 3;

            if (bCheckHeldenianMap(sAttackerH, m_iBTFieldMapIndex, DEF_OWNERTYPE_PLAYER) == 1)
            {
                iDamage += iDamage / 3;
            }

            if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == TRUE) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
            {
                if (m_pClientList[sAttackerH]->m_iLevel <= 80)
                {
                    iDamage += (iDamage * 7) / 10;
                }
                else if (m_pClientList[sAttackerH]->m_iLevel <= 100)
                {
                    iDamage += iDamage / 2;
                }
                else
                    iDamage += iDamage / 3;
            }

            cAttackerSide = m_pClientList[sAttackerH]->m_cSide;
            sAtkX = m_pClientList[sAttackerH]->m_sX;
            sAtkY = m_pClientList[sAttackerH]->m_sY;
            iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
            break;

        case DEF_OWNERTYPE_NPC:
            cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
            sAtkX = m_pNpcList[sAttackerH]->m_sX;
            sAtkY = m_pNpcList[sAttackerH]->m_sY;
            break;
    }

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:

            if (m_pClientList[sTargetH] == NULL) return;
            if (m_pClientList[sTargetH]->m_bIsInitComplete == FALSE) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return;

            if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == FALSE) &&
                (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == TRUE)) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == TRUE) &&
                (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsOwnLocation == TRUE)) return;

            if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;
            if ((m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) && (m_pClientList[sTargetH]->m_iAdminUserLevel == 0)) return;
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == TRUE) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;
            if ((m_pClientList[sTargetH]->m_iPartyID != NULL) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;
            m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;

            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {
                if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == TRUE)
                {
                    iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
                    if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
                    {

                    }
                    else
                    {
                        if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                        {
                            if (m_pClientList[sAttackerH]->m_iGuildGUID != m_pClientList[sTargetH]->m_iGuildGUID)
                            {

                            }
                            else return;
                        }
                        else return;
                    }
                }

                if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY, 0x00000005) != 0) return;
            }

            ClearSkillUsingStatus(sTargetH);

            switch (iAttr)
            {
                case 1:
                    if (m_pClientList[sTargetH]->m_iAddAbsEarth != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsEarth;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 2:
                    if (m_pClientList[sTargetH]->m_iAddAbsAir != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsAir;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 3:
                    if (m_pClientList[sTargetH]->m_iAddAbsFire != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsFire;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 4:
                    if (m_pClientList[sTargetH]->m_iAddAbsWater != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsWater;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                default: break;
            }

            iIndex = m_pClientList[sTargetH]->m_iMagicDamageSaveItemIndex;
            if ((iIndex != -1) && (iIndex >= 0) && (iIndex < DEF_MAXITEMS))
            {

                switch (m_pClientList[sTargetH]->m_pItemList[iIndex]->m_sIDnum)
                {
                    case 335:
                        dTmp1 = (double)iDamage;
                        dTmp2 = dTmp1 * 0.2f;
                        dTmp3 = dTmp1 - dTmp2;
                        iDamage = (int)(dTmp3 + 0.5f);
                        break;

                    case 337:
                        dTmp1 = (double)iDamage;
                        dTmp2 = dTmp1 * 0.1f;
                        dTmp3 = dTmp1 - dTmp2;
                        iDamage = (int)(dTmp3 + 0.5f);
                        break;
                }
                if (iDamage <= 0) iDamage = 0;

                iRemainLife = m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan;
                if (iRemainLife <= iDamage)
                {
                    ItemDepleteHandler(sTargetH, iIndex, TRUE);
                }
                else
                {
                    m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan -= iDamage;
                }
            }

            if (m_pClientList[sTargetH]->m_iAddAbsMD != 0)
            {
                dTmp1 = (double)iDamage;
                dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsMD;
                dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                iDamage = iDamage - (int)dTmp3;
            }

            if (cTargetType == DEF_OWNERTYPE_PLAYER)
            {
                iDamage -= (iDice(1, m_pClientList[sTargetH]->m_iVit / 10) - 1);
                if (iDamage <= 0) iDamage = 0;
            }

            if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == TRUE) &&
                (iDice(1, 10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage))
            {
                iDamage = m_pClientList[sTargetH]->m_iHP - 1;
            }

            if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                iDamage = iDamage / 2;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == TRUE))
            {
                switch (m_pClientList[sTargetH]->m_iSpecialAbilityType)
                {
                    case 51:
                    case 52:
                        return;
                }
            }

            m_pClientList[sTargetH]->m_iHP -= iDamage;
            if (m_pClientList[sTargetH]->m_iHP <= 0)
            {
                ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iDamage);
            }
            else
            {
                if (iDamage > 0)
                {
                    if (m_pClientList[sTargetH]->m_iAddTransMana > 0)
                    {
                        dTmp1 = (double)m_pClientList[sTargetH]->m_iAddTransMana;
                        dTmp2 = (double)iDamage;
                        dTmp3 = (dTmp1 / 100.0f) * dTmp2 + 1.0f;

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
                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);

                    if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != TRUE)
                    {
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                    }

                    if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);
                        m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }
            }

            sTgtX = m_pClientList[sTargetH]->m_sX;
            sTgtY = m_pClientList[sTargetH]->m_sY;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == NULL) return;
            if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
            if ((m_bIsCrusadeMode == TRUE) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;

            sTgtX = m_pNpcList[sTargetH]->m_sX;
            sTgtY = m_pNpcList[sTargetH]->m_sY;

            switch (m_pNpcList[sTargetH]->m_cActionLimit)
            {
                case 1:
                case 2:
                    return;

                case 4:
                    if (sTgtX == sAtkX)
                    {
                        if (sTgtY == sAtkY) return;
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
                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, NULL) == FALSE) return;
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

                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, NULL) == FALSE) return;
                    }

                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);
                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
                    m_pNpcList[sTargetH]->m_sX = dX;
                    m_pNpcList[sTargetH]->m_sY = dY;
                    m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;

                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, NULL, NULL, NULL);

                    if (bCheckEnergySphereDestination(sTargetH, sAttackerH, cAttackerType) == TRUE)
                    {
                        DeleteNpc(sTargetH);
                    }
                    return;
            }

            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {
                switch (m_pNpcList[sTargetH]->m_sType)
                {
                    case 40:
                    case 41:
                        if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return;
                        break;
                }
            }

            switch (m_pNpcList[sTargetH]->m_sType)
            {
                case 67: // McGaffin
                case 68: // Perry
                case 69: // Devlin
                    return;
            }

            if (m_pNpcList[sTargetH]->m_iAbsDamage > 0)
            {
                dTmp1 = (double)iDamage;
                dTmp2 = (double)(m_pNpcList[sTargetH]->m_iAbsDamage) / 100.0f;
                dTmp3 = dTmp1 * dTmp2;
                dTmp2 = dTmp1 - dTmp3;
                iDamage = (int)dTmp2;
                if (iDamage < 0) iDamage = 1;
            }

            if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                iDamage = iDamage / 2;

            m_pNpcList[sTargetH]->m_iHP -= iDamage;
            if (m_pNpcList[sTargetH]->m_iHP < 0)
            {
                NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);
            }
            else
            {
                switch (cAttackerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
                            && (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) return;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide) return;
                        break;
                }

                SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);

                if ((iDice(1, 3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0))
                {
                    if ((cAttackerType == DEF_OWNERTYPE_NPC) &&
                        (m_pNpcList[sAttackerH]->m_sType == m_pNpcList[sTargetH]->m_sType) &&
                        (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide)) return;

                    m_pNpcList[sTargetH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                    m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;
                    m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
                    m_pNpcList[sTargetH]->m_cTargetType = cAttackerType;

                    m_pNpcList[sTargetH]->m_dwTime = dwTime;

                    if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                    }

                    if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != TRUE) &&
                        (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
                    {
                        if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage)
                        {
                            iExp = iDamage;
                            if ((m_bIsCrusadeMode == TRUE) && (iExp > 10)) iExp = 10;

                            if (m_pClientList[sAttackerH]->m_iAddExp > 0)
                            {
                                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                dTmp2 = (double)iExp;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                iExp += (int)dTmp3;
                            }

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

                            if (bExp == TRUE)
                                GetExp(sAttackerH, iExp, TRUE);
                            else GetExp(sAttackerH, (iExp / 2), TRUE);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
                        }
                        else
                        {
                            iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            if ((m_bIsCrusadeMode == TRUE) && (iExp > 10)) iExp = 10;

                            if (m_pClientList[sAttackerH]->m_iAddExp > 0)
                            {
                                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                dTmp2 = (double)iExp;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                iExp += (int)dTmp3;
                            }

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

                            if (bExp == TRUE)
                                GetExp(sAttackerH, iExp, TRUE);
                            else GetExp(sAttackerH, (iExp / 2), TRUE);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
                        }
                    }
                }
            }
            break;
    }
}

void CGame::Effect_Damage_Spot_Type2(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, BOOL bExp, int iAttr)
{
    int iPartyID, iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iExp, iMaxSuperAttack, iRepDamage;
    char cAttackerSide, cDamageMoveDir, cDamageMinimum;
    DWORD dwTime;
    double dTmp1, dTmp2, dTmp3;
    short sTgtX, sTgtY, sItemIndex;

    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] == NULL)) return;
    if ((cAttackerType == DEF_OWNERTYPE_NPC) && (m_pNpcList[sAttackerH] == NULL)) return;
    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] != 0) &&
        (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsHeldenianMap == 1) && (m_bHeldenianInitiated == TRUE)) return;

    dwTime = timeGetTime();
    sTgtX = 0;
    sTgtY = 0;
    iDamage = iDice(sV1, sV2) + sV3;
    if (iDamage <= 0) iDamage = 0;

    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if ((m_bAdminSecurity == TRUE) && (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0)) return;
            if (m_pClientList[sAttackerH]->m_cHeroArmorBonus == 2) iDamage += 4;
            if ((m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] == -1) || (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] == -1))
            {
                sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
                if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL))
                {
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 861 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 862)
                    {
                        iDamage *= (int)1.3;
                    }
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 863 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 864)
                    {
                        if (m_pClientList[sAttackerH]->m_iRating > 0)
                        {
                            iRepDamage = m_pClientList[sAttackerH]->m_iRating / 100;
                            if (iRepDamage < 5) iRepDamage = 5;
                            if (iRepDamage > 15) iRepDamage = 15;
                            iDamage += iRepDamage;
                        }
                        if (cTargetType == DEF_OWNERTYPE_PLAYER)
                        {
                            if (m_pClientList[sTargetH] != NULL)
                            {
                                if (m_pClientList[sTargetH]->m_iRating < 0)
                                {
                                    iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 10);
                                    if (iRepDamage > 10) iRepDamage = 10;
                                    iDamage += iRepDamage;
                                }
                            }
                        }
                    }
                }
                sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_NECK];
                if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != NULL))
                {
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859)
                    { // NecklaceOfKloness  
                        if (cTargetType == DEF_OWNERTYPE_PLAYER)
                        {
                            if (m_pClientList[sTargetH] != NULL)
                            {
                                iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 20);
                                if (iRepDamage > 5) iRepDamage = 5;
                                iDamage += iRepDamage;
                            }
                        }
                    }
                }
            }

            if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == TRUE) && (cTargetType == DEF_OWNERTYPE_PLAYER)) return;

            dTmp1 = (double)iDamage;
            if (m_pClientList[sAttackerH]->m_iMag <= 0)
                dTmp2 = 1.0f;
            else dTmp2 = (double)m_pClientList[sAttackerH]->m_iMag;
            dTmp2 = dTmp2 / 3.3f;
            dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
            iDamage = (int)(dTmp3 + 0.5f);
            if (iDamage <= 0) iDamage = 0;
            // order switched with above
            iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;

            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                iDamage += iDamage / 3;

            if (bCheckHeldenianMap(sAttackerH, m_iBTFieldMapIndex, DEF_OWNERTYPE_PLAYER) == 1)
            {
                iDamage += iDamage / 3;
            }

            if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == TRUE) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
            {
                if (m_pClientList[sAttackerH]->m_iLevel <= 80)
                {
                    iDamage += (iDamage * 7) / 10;
                }
                else if (m_pClientList[sAttackerH]->m_iLevel <= 100)
                {
                    iDamage += iDamage / 2;
                }
                else
                    iDamage += iDamage / 3;
            }

            cAttackerSide = m_pClientList[sAttackerH]->m_cSide;
            iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
            break;

        case DEF_OWNERTYPE_NPC:
            cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
            break;
    }

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:

            if (m_pClientList[sTargetH] == NULL) return;
            if (m_pClientList[sTargetH]->m_bIsInitComplete == FALSE) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return;
            if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;
            if (m_pClientList[sTargetH]->m_cMapIndex == -1) return;
            if ((m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) && (m_pClientList[sTargetH]->m_iAdminUserLevel == 0)) return;
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == TRUE) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;

            if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == FALSE) &&
                (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == TRUE)) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == TRUE) &&
                (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsOwnLocation == TRUE)) return;

            if ((m_pClientList[sTargetH]->m_iPartyID != NULL) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;
            m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;

            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {
                if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == TRUE)
                {
                    iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
                    if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
                    {

                    }
                    else
                    {
                        if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                        {
                            if (m_pClientList[sAttackerH]->m_iGuildGUID != m_pClientList[sTargetH]->m_iGuildGUID)
                            {

                            }
                            else return;
                        }
                        else return;
                    }
                }

                if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY, 0x00000005) != 0) return;
            }

            ClearSkillUsingStatus(sTargetH);

            switch (iAttr)
            {
                case 1:
                    if (m_pClientList[sTargetH]->m_iAddAbsEarth != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsEarth;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 2:
                    if (m_pClientList[sTargetH]->m_iAddAbsAir != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsAir;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 3:
                    if (m_pClientList[sTargetH]->m_iAddAbsFire != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsFire;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 4:
                    if (m_pClientList[sTargetH]->m_iAddAbsWater != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsWater;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                default: break;
            }

            iIndex = m_pClientList[sTargetH]->m_iMagicDamageSaveItemIndex;
            if ((iIndex != -1) && (iIndex >= 0) && (iIndex < DEF_MAXITEMS))
            {

                switch (m_pClientList[sTargetH]->m_pItemList[iIndex]->m_sIDnum)
                {
                    case 335:
                        dTmp1 = (double)iDamage;
                        dTmp2 = dTmp1 * 0.2f;
                        dTmp3 = dTmp1 - dTmp2;
                        iDamage = (int)(dTmp3 + 0.5f);
                        break;

                    case 337:
                        dTmp1 = (double)iDamage;
                        dTmp2 = dTmp1 * 0.1f;
                        dTmp3 = dTmp1 - dTmp2;
                        iDamage = (int)(dTmp3 + 0.5f);
                        break;
                }
                if (iDamage <= 0) iDamage = 0;

                iRemainLife = m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan;
                if (iRemainLife <= iDamage)
                {
                    ItemDepleteHandler(sTargetH, iIndex, TRUE);
                }
                else
                {
                    m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan -= iDamage;
                }
            }

            if (m_pClientList[sTargetH]->m_iAddAbsMD != 0)
            {
                dTmp1 = (double)iDamage;
                dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsMD;
                dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                iDamage = iDamage - (int)dTmp3;
            }

            if (cTargetType == DEF_OWNERTYPE_PLAYER)
            {
                iDamage -= (iDice(1, m_pClientList[sTargetH]->m_iVit / 10) - 1);
                if (iDamage <= 0) iDamage = 0;
            }

            if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                iDamage = iDamage / 2;

            if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 5) break;

            if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == TRUE) &&
                (iDice(1, 10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage))
            {
                iDamage = m_pClientList[sTargetH]->m_iHP - 1;
            }

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == TRUE))
            {
                switch (m_pClientList[sTargetH]->m_iSpecialAbilityType)
                {
                    case 51:
                    case 52:
                        return;
                }
            }

            m_pClientList[sTargetH]->m_iHP -= iDamage;
            if (m_pClientList[sTargetH]->m_iHP <= 0)
            {
                ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iDamage);
            }
            else
            {
                if (iDamage > 0)
                {
                    if (m_pClientList[sTargetH]->m_iAddTransMana > 0)
                    {
                        dTmp1 = (double)m_pClientList[sTargetH]->m_iAddTransMana;
                        dTmp2 = (double)iDamage;
                        dTmp3 = (dTmp1 / 100.0f) * dTmp2 + 1.0f;

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

                    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE))
                    {
                        cDamageMinimum = 80;
                    }
                    else
                    {
                        cDamageMinimum = 50;
                    }

                    if (iDamage >= cDamageMinimum)
                    {
                        sTgtX = m_pClientList[sTargetH]->m_sX;
                        sTgtY = m_pClientList[sTargetH]->m_sY;
                        if (sTgtX == sAtkX)
                        {
                            if (sTgtY == sAtkY) return;
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
                        m_pClientList[sTargetH]->m_iLastDamage = iDamage;
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iDamage, NULL, NULL);
                    }

                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);

                    if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != TRUE)
                    {
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                    }

                    if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);
                        m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }
            }
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == NULL) return;
            if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
            if ((m_bIsCrusadeMode == TRUE) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;

            switch (m_pNpcList[sTargetH]->m_cActionLimit)
            {
                case 1:
                case 2:
                case 4:
                case 6:
                    return;

                case 3:
                case 5:
                    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
                    {
                        switch (m_pNpcList[sTargetH]->m_sType)
                        {
                            case 40:
                            case 41:
                                if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return;
                                break;
                        }
                    }
            }

            if (m_pNpcList[sTargetH]->m_iAbsDamage > 0)
            {
                dTmp1 = (double)iDamage;
                dTmp2 = (double)(m_pNpcList[sTargetH]->m_iAbsDamage) / 100.0f;
                dTmp3 = dTmp1 * dTmp2;
                dTmp2 = dTmp1 - dTmp3;
                iDamage = (int)dTmp2;
                if (iDamage < 0) iDamage = 1;
            }

            if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                iDamage = iDamage / 2;

            m_pNpcList[sTargetH]->m_iHP -= iDamage;
            if (m_pNpcList[sTargetH]->m_iHP < 0)
            {
                NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);
            }
            else
            {
                switch (cAttackerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
                            && (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) return;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide) return;
                        break;
                }

                SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);
                if ((iDice(1, 3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0))
                {
                    if ((cAttackerType == DEF_OWNERTYPE_NPC) &&
                        (m_pNpcList[sAttackerH]->m_sType == m_pNpcList[sTargetH]->m_sType) &&
                        (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide)) return;

                    m_pNpcList[sTargetH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                    m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;
                    m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
                    m_pNpcList[sTargetH]->m_cTargetType = cAttackerType;
                    m_pNpcList[sTargetH]->m_dwTime = dwTime;

                    if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                    }

                    if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != TRUE) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
                    {
                        if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage)
                        {
                            iExp = iDamage;
                            if ((m_bIsCrusadeMode == TRUE) && (iExp > 10)) iExp = 10;

                            if (m_pClientList[sAttackerH]->m_iAddExp > 0)
                            {
                                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                dTmp2 = (double)iExp;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                iExp += (int)dTmp3;
                            }

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

                            if (bExp == TRUE)
                                GetExp(sAttackerH, iExp, TRUE);
                            else GetExp(sAttackerH, (iExp / 2), TRUE);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
                        }
                        else
                        {
                            iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            if ((m_bIsCrusadeMode == TRUE) && (iExp > 10)) iExp = 10;

                            if (m_pClientList[sAttackerH]->m_iAddExp > 0)
                            {
                                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                dTmp2 = (double)iExp;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                iExp += (int)dTmp3;
                            }

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

                            if (bExp == TRUE)
                                GetExp(sAttackerH, iExp, TRUE);
                            else GetExp(sAttackerH, (iExp / 2), TRUE);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
                        }
                    }
                }
            }
            break;
    }
}

void CGame::Effect_Damage_Spot_DamageMove(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, BOOL bExp, int iAttr)
{
    int iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iMaxSuperAttack;
    DWORD dwTime, wWeaponType;
    char cAttackerSide, cDamageMoveDir;
    double dTmp1, dTmp2, dTmp3;
    int iPartyID, iMoveDamage;
    short sTgtX, sTgtY;

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == NULL) return;

    if (cAttackerType == DEF_OWNERTYPE_NPC)
        if (m_pNpcList[sAttackerH] == NULL) return;

    dwTime = timeGetTime();
    sTgtX = 0;
    sTgtY = 0;

    // ÇØ´ç Å¸ÄÏ¿¡°Ô ´ë¹ÌÁö¸¦ ¸ÔÀÎ´Ù. 
    iDamage = iDice(sV1, sV2) + sV3;
    if (iDamage <= 0) iDamage = 0;

    iPartyID = 0;

    // °ø°ÝÀÚ°¡ ÇÃ·¹ÀÌ¾î¶ó¸é Mag¿¡ µû¸¥ º¸³Ê½º ´ë¹ÌÁö¸¦ °¡»ê 
    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if ((m_bAdminSecurity == TRUE) && (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0)) return;
            dTmp1 = (double)iDamage;
            if (m_pClientList[sAttackerH]->m_iMag <= 0)
                dTmp2 = 1.0f;
            else dTmp2 = (double)m_pClientList[sAttackerH]->m_iMag;

            dTmp2 = dTmp2 / 3.3f;
            dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
            iDamage = (int)(dTmp3 + 0.5f);
            if (iDamage <= 0) iDamage = 0;

            // v1.432 2001 4 7 13 7
            iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;

            // v1.44 »çÅõÀåÀÌ¸é ´ë¹ÌÁö 1.33¹è 
            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                iDamage += iDamage / 3;

            // Crusade : Àü¸éÀü ¸ðµåÀÏ¶§ ´ëÀÎ °ø°Ý·Â 1.33¹è 
            if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == TRUE) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
            {
                // v2.15 Àú·¦ÀÇ °æ¿ì´Â µ¥¹ÌÁö°¡ Áõ°¡ÇÑ´Ù. 1.7 ¹è 
                if (m_pClientList[sAttackerH]->m_iLevel <= 80)
                {
                    iDamage += (iDamage * 7) / 10;
                } // v2.15 Àú·¦ÀÇ °æ¿ì´Â µ¥¹ÌÁö°¡ Áõ°¡ÇÑ´Ù. 1.5 ¹è 
                else if (m_pClientList[sAttackerH]->m_iLevel <= 100)
                {
                    iDamage += iDamage / 2;
                }
                else iDamage += iDamage / 3;
            }

            if (m_pClientList[sAttackerH]->m_cHeroArmorBonus == 2)
            {
                iDamage += 4;
            }

            wWeaponType = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
            if (wWeaponType == 34)
            {
                iDamage += iDamage / 3;
            }

            if (bCheckHeldenianMap(sAttackerH, m_iBTFieldMapIndex, DEF_OWNERTYPE_PLAYER) == 1)
            {
                iDamage += iDamage / 3;
            }

            cAttackerSide = m_pClientList[sAttackerH]->m_cSide;

            iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
            break;

        case DEF_OWNERTYPE_NPC:
            cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
            break;
    }

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            // °ø°Ý ´ë»óÀÌ Á¸ÀçÇÏÁö ¾ÊÀ¸¸é ¸®ÅÏ 
            if (m_pClientList[sTargetH] == NULL) return;
            if (m_pClientList[sTargetH]->m_bIsInitComplete == FALSE) return;
            // ÀÌ¹Ì Á×¾î ÀÖ´Ù¸é Ã³¸® ¾ÈÇÔ.
            if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return;
            // ·¢À¸·Î ÀÎÇØ º¸È£¸¦ ¹Þ¾Æ¾ß ÇÑ´Ù¸é 
            if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;
            // °ø°ÝÀÚ°¡ À§Ä¡ÇÑ ¸ÊÀÌ °ø°Ý ºÒ°¡´É ¸ÊÀÌ¶ó¸é 
            // v2.03 ¼­¹ö ´Ù¿îµÇ¾î¼­ °íÄ§ 
            if (m_pClientList[sTargetH]->m_cMapIndex == -1) return;
            if ((m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) && (m_pClientList[sTargetH]->m_iAdminUserLevel == 0)) return;
            // v1.41 °ø°ÝÀÚ°¡ Áß¸³ÀÌ¸é ´ëÀÎ °ø°ÝÀÌ ºÒ°¡´ÉÇÏ´Ù. 
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == TRUE) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;

            // v2.172 °°ÀºÆí³¢¸®´Â ¸¶À»¿¡¼­ °ø°Ý´çÇÏÁö ¾Ê´Â´Ù. ¹üÁËÀÚ¿Í NPC´Â ¿¹¿Ü Áß¸³µµ °ø°Ý ¾ÊµÇ°Ô 
            if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == TRUE)) return;
            if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == TRUE)) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == TRUE) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == TRUE)) return;

            // 01-12-17 °°Àº ÆÄÆ¼¿øÀÌ¸é °ø°Ý ºÒ°¡ 
            if ((m_pClientList[sTargetH]->m_iPartyID != NULL) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;
            m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;

            // ¸¸¾à °ø°ÝÀÚ°¡ ÇÃ·¹ÀÌ¾îÀÌ°í °ø°ÝÀÚ°¡ ¾ÈÀü °ø°Ý ¸ðµå¶ó¸é °ø°ÝÀÇ ÀÇ¹Ì°¡ ¾ø´Ù. 
            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {

                if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == TRUE)
                {
                    iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
                    if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
                    {
                        // ¾Æ¹« È¿°ú ¾øÀ½. ¸¶³ª ¼Òºñ·®ÀÌ ÁÙ¾úÀ¸¹Ç·Î 
                    }
                    else
                    {
                        // °°Àº ÆíÀÌÁö¸¸ ¸¸¾à »çÅõÀå ³»¿¡¼­ ÆíÀÌ °°´Ù¸é °ø°Ý Ã³¸®¸¦ ÇØ¾ßÇÑ´Ù.
                        if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                        {
                            if (m_pClientList[sAttackerH]->m_iGuildGUID != m_pClientList[sTargetH]->m_iGuildGUID)
                            {
                                // »çÅõÀå ³»¿¡¼­ ´Ù¸¥ ±æµå´Ù. °ø°ÝÀÌ °¡´ÉÇÏ´Ù. 
                            }
                            else return;
                        }
                        else return;
                    }
                }

                // ¸¶¹ý º¸È£ È¤Àº ¾ÈÀü ¿µ¿ªÀÌ¶ó¸é °ø°Ý ¼º°ø ¸øÇÔ 
                if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY, 0x00000005) != 0) return;
            }

            // ½ºÅ³ÀÇ »ç¿ë»óÅÂ¸¦ ¸ðµÎ ¹«È¿È­ ÇÑ´Ù.
            ClearSkillUsingStatus(sTargetH);

            // v1.432 ¼Ó¼ºº° ´ë¹ÌÁö °¨¼Ò 
            switch (iAttr)
            {
                case 1:
                    if (m_pClientList[sTargetH]->m_iAddAbsEarth != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsEarth;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 2:
                    if (m_pClientList[sTargetH]->m_iAddAbsAir != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsAir;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 3:
                    if (m_pClientList[sTargetH]->m_iAddAbsFire != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsFire;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                case 4:
                    if (m_pClientList[sTargetH]->m_iAddAbsWater != 0)
                    {
                        dTmp1 = (double)iDamage;
                        dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsWater;
                        dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                        iDamage = iDamage - (int)(dTmp3);
                        if (iDamage < 0) iDamage = 0;
                    }
                    break;

                default: break;
            }

            iIndex = m_pClientList[sTargetH]->m_iMagicDamageSaveItemIndex;
            if ((iIndex != -1) && (iIndex >= 0) && (iIndex < DEF_MAXITEMS))
            {
                // ¸¶¹ý ´ë¹ÌÁö Àý°¨ È¿°ú¸¦ °¡Áø ¾ÆÀÌÅÛÀÌ´Ù.

                // ¾ÆÀÌÅÛÀÇ Á¾·ù¿¡ µû¶ó ´ë¹ÌÁö¸¦ ÁÙÀÎ´Ù. 
                switch (m_pClientList[sTargetH]->m_pItemList[iIndex]->m_sIDnum)
                {
                    case 335: // ¿¡¸Ó¶öµå ¹ÝÁö 20%
                        dTmp1 = (double)iDamage;
                        dTmp2 = dTmp1 * 0.2f;
                        dTmp3 = dTmp1 - dTmp2;
                        iDamage = (int)(dTmp3 + 0.5f);
                        break;

                    case 337: // ·çºñ ¹ÝÁö 10%
                        dTmp1 = (double)iDamage;
                        dTmp2 = dTmp1 * 0.1f;
                        dTmp3 = dTmp1 - dTmp2;
                        iDamage = (int)(dTmp3 + 0.5f);
                        break;
                }
                if (iDamage <= 0) iDamage = 0;

                iRemainLife = m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan;
                if (iRemainLife <= iDamage)
                {
                    // ¸¶¹ý µ¥¹ÌÁö Àý°¨ ¾ÆÀÌÅÛÀÌ ºÎ¼­Áø´Ù. 
                    ItemDepleteHandler(sTargetH, iIndex, TRUE);
                }
                else
                {
                    // ¸¶¹ý µ¥¹ÌÁö Àý°¨ ¾ÆÀÌÅÛÀÇ ¼ö¸í¸¸ ÁÙÀÎ´Ù. 
                    m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan -= iDamage;
                }
            }

            // v1.42 Ãß°¡µÈ °íÁ¤ ¸¶¹ý ´ë¹ÌÁö Àý°¨
            if (m_pClientList[sTargetH]->m_iAddAbsMD != 0)
            {
                dTmp1 = (double)iDamage;
                dTmp2 = (double)m_pClientList[sTargetH]->m_iAddAbsMD;
                dTmp3 = (dTmp2 / 100.0f) * dTmp1;
                iDamage = iDamage - (int)dTmp3;
            }

            // v1.4 VIT¿¡ µû¸¥ ´ë¹ÌÁö °¨¼Ò 
            if (cTargetType == DEF_OWNERTYPE_PLAYER)
            {
                iDamage -= (iDice(1, m_pClientList[sTargetH]->m_iVit / 10) - 1);
                if (iDamage <= 0) iDamage = 0;
            }

            // ¸¸¾à ¸¶¹ý º¸È£ÁßÀÌ¶ó¸é ´ë¹ÌÁö´Â 1/2 
            if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                iDamage = iDamage / 2;

            if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == TRUE) &&
                (iDice(1, 10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage))
            {
                // Çà¿îÈ¿°ú·Î Á×À½À» ¸ð¸éÇÑ´Ù.
                iDamage = m_pClientList[sTargetH]->m_iHP - 1;
            }

            // v1.432 Æ¯¼ö ´É·Â Áß ¸ðµç ´ë¹ÌÁö¸¦ ¸·´Â È¿°ú°¡ È°¼ºÈ­ µÈ °æ¿ì ´ë¹ÌÁö¸¦ ÀÔÁö ¾Ê´Â´Ù.
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == TRUE))
            {
                switch (m_pClientList[sTargetH]->m_iSpecialAbilityType)
                {
                    case 51:
                    case 52:
                        // 
                        return;
                }
            }

            m_pClientList[sTargetH]->m_iHP -= iDamage;
            if (m_pClientList[sTargetH]->m_iHP <= 0)
            {
                // ÇÃ·¹ÀÌ¾î°¡ »ç¸ÁÇß´Ù.
                ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iDamage);
            }
            else
            {
                if (iDamage > 0)
                {
                    // v2.04 Å¸°ÝÀ» ¹Þ¾Ò´Âµ¥ ¸¶³ª º¯È¯ Æ¯¼ºÄ¡°¡ ÀÖ¾ú´Ù¸é 
                    if (m_pClientList[sTargetH]->m_iAddTransMana > 0)
                    {
                        dTmp1 = (double)m_pClientList[sTargetH]->m_iAddTransMana;
                        dTmp2 = (double)iDamage;
                        dTmp3 = (dTmp1 / 100.0f) * dTmp2 + 1.0f;

                        // ÃÖ´ë ¸¶³ªÄ¡ 
                        iTemp = (2 * m_pClientList[sTargetH]->m_iMag) + (2 * m_pClientList[sTargetH]->m_iLevel) + (m_pClientList[sTargetH]->m_iInt / 2);
                        m_pClientList[sTargetH]->m_iMP += (int)dTmp3;
                        if (m_pClientList[sTargetH]->m_iMP > iTemp) m_pClientList[sTargetH]->m_iMP = iTemp;
                    }

                    // v2.04 Å¸°ÝÀ» ¹Þ¾Ò´Âµ¥ ÇÊ»ì ÃæÀüÀÇ Æ¯¼ºÄ¡°¡ ÀÖ¾ú´Ù¸é 
                    if (m_pClientList[sTargetH]->m_iAddChargeCritical > 0)
                    {
                        // È®·ü °è»ê¿¡ µû¶ó¼­ ÇÊ»ì±â°¡ ÃæÀüµÈ´Ù.
                        if (iDice(1, 100) < (m_pClientList[sTargetH]->m_iAddChargeCritical))
                        {
                            iMaxSuperAttack = (m_pClientList[sTargetH]->m_iLevel / 10);
                            if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;
                            // v1.12 ¼­¹ö¿Í Å¬¶óÀÌ¾ðÆ® °£¿¡ Ä«¿îÆ®°¡ ÀÏÄ¡ÇÏÁö ¾Ê´Â °æ¿ì°¡ ÀÖÀ» ¼ö ÀÖÀ¸¹Ç·Î °¡°¨¿¡ »ó°ü¾øÀÌ º¸³½´Ù.
                            SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, NULL, NULL, NULL, NULL);
                        }
                    }

                    // v1.44 »çÅõÀåÀÌ¸é ´ë¹ÌÁö 80ÀÌ»óÀÏ¶§ ¹Ð¸°´Ù.
                    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE))
                        iMoveDamage = 80;
                    else iMoveDamage = 50;

                    if (iDamage >= iMoveDamage)
                    {
                        // ´ë¹ÌÁö°¡ 50ÀÌ»óÀÌ¸é Æ¨±ä´Ù.
                ///		char cDamageMoveDir;
                        sTgtX = m_pClientList[sTargetH]->m_sX;
                        sTgtY = m_pClientList[sTargetH]->m_sY;

                        if (sTgtX == sAtkX)
                        {
                            if (sTgtY == sAtkY)     goto EDSD_SKIPDAMAGEMOVE;
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

                        // v1.44 ¹Ð·Á³¯¶§ ´ë¹ÌÁö¸¦ ÀÔ·ÂÇÑ´Ù.
                        m_pClientList[sTargetH]->m_iLastDamage = iDamage;
                        // ¹ÞÀº ´ë¹ÌÁö¸¦ Åëº¸ÇÑ´Ù. <- HP¸¦ ±×´ë·Î ¾Ë¸°´Ù.
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        // Æ¨°Ü ³ª°¡¶ó´Â ¸Þ½ÃÁö ÀÔ·Â 	
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iDamage, NULL, NULL);
                    }
                    else
                    {
                        EDSD_SKIPDAMAGEMOVE:;
                        // ¹ÞÀº ´ë¹ÌÁö¸¦ Åëº¸ÇÑ´Ù. <- HP¸¦ ±×´ë·Î ¾Ë¸°´Ù.
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        // Ãæ°ÝÀ» ¹Þ¾Ò´Ù¸é Ãæ°Ýµ¿ÀÛ Àü¼Û 
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);
                    }

                    // v1.4 Á×ÀºÃ´ÇÏ°í ÀÖ´Â °æ¿ì´Â Owner À§Ä¡¸¦ ¿Å±ä´Ù. 
                    if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != TRUE)
                    {
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                    }

                    if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        // Hold-Person µÇ¾î ÀÖ¾ú´ø »óÅÂ¶ó¸é Ç®¸°´Ù. ¸¶¹ý°ø°Ý ÆÐ·²¶óÀÌÁî µÈ °Íµµ Ç®¸°´Ù.
                        // 1: Hold-Person 
                        // 2: Paralize
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);

                        m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }
            }
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == NULL) return;
            if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
            if ((m_bIsCrusadeMode == TRUE) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;

            switch (m_pNpcList[sTargetH]->m_cActionLimit)
            {
                case 1:
                case 2:
                case 4:
                    return;
            }

            // ÀüÀï¿ë ±¸Á¶¹°Áß ±×·£µå ¸ÅÁ÷ Á¦³×·¹ÀÌÅÍ, ¿¡³ÊÁö ½Çµå Á¦³×·¹ÀÌÅÍ´Â ¾Æ±ºÀÌ³ª Áß¸³ÀÇ °ø°Ý¿¡ ´ë¹ÌÁö¸¦ ÀÔÁö ¾Ê´Â´Ù.
            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {
                switch (m_pNpcList[sTargetH]->m_sType)
                {
                    case 40:
                    case 41:
                        if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return;
                        break;
                }
            }

            switch (m_pNpcList[sTargetH]->m_sType)
            {
                case 67: // McGaffin
                case 68: // Perry
                case 69: // Devlin
                    iDamage = 0;
                    break;
            }

            // ¸ó½ºÅÍ°¡ ¸¶¹ý ´ë¹ÌÁö Èí¼ö·üÀÌ ÀÖ´Ù¸é(AbsDamage°¡ 0º¸´Ù Å©´Ù) ¿ø·¡ ¸¶¹ý ´ë¹ÌÁö¸¦ °¨¼Ò½ÃÅ²´Ù.
            if (m_pNpcList[sTargetH]->m_iAbsDamage > 0)
            {
                dTmp1 = (double)iDamage;
                dTmp2 = (double)(m_pNpcList[sTargetH]->m_iAbsDamage) / 100.0f;
                dTmp3 = dTmp1 * dTmp2;
                dTmp2 = dTmp1 - dTmp3;
                iDamage = (int)dTmp2;
                if (iDamage < 0) iDamage = 1;
            }

            // ¸¸¾à ¸¶¹ý º¸È£ÁßÀÌ¶ó¸é ´ë¹ÌÁö´Â 1/2 
            if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                iDamage = iDamage / 2;

            m_pNpcList[sTargetH]->m_iHP -= iDamage;
            if (m_pNpcList[sTargetH]->m_iHP < 0)
            {
                // NPC°¡ »ç¸ÁÇß´Ù.
                NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);
            }
            else
            {
                // °ø°Ý´çÇßÁö¸¸ »ì¾ÆÀÖ´Ù. ¹Ý°ÝÇÑ´Ù.

                // ÆíÀÌ °°À¸¸é ¹Ý°ÝÇÏÁö ¾Ê´Â´Ù.
                switch (cAttackerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        // v2.15 °æºñ´Â °°Àº Æíµµ ¹Ý°ÝÇÑ´Ù.			
                        if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
                            && (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) return;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide) return;
                        break;
                }

                // Ãæ°ÝÀ» ¹Þ¾Ò´Ù¸é Ãæ°Ýµ¿ÀÛ Àü¼Û
                SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);

                if ((iDice(1, 3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0))
                {

                    // µ¿Á·ÀÌ°í ÆíÀÌ °°À¸¸é ¹Ý°ÝÇÏÁö ¾Ê´Â´Ù.
                    if ((cAttackerType == DEF_OWNERTYPE_NPC) &&
                        (m_pNpcList[sAttackerH]->m_sType == m_pNpcList[sTargetH]->m_sType) &&
                        (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide)) return;

                    // ActionLimit°¡ 1ÀÌ¸é ¹Ý°ÝÀ» ÇÏÁö ¾Ê´Â´Ù. ¿ÀÁ÷ ¿òÁ÷ÀÏ¼ö¸¸ ÀÖÀ¸´Ï.
                    m_pNpcList[sTargetH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                    m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;
                    m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
                    m_pNpcList[sTargetH]->m_cTargetType = cAttackerType;

                    // ¿©±â¼­ Ç¥È¿ µ¿ÀÛ°°Àº°ÍÀ» À§ÇÑ ¸Þ½ÃÁö ¹ß¼Û.

                    // Damage¸¦ ÀÔÀº Ãæ°ÝÀ¸·Î ÀÎÇÑ Áö¿¬È¿°ú.
                    m_pNpcList[sTargetH]->m_dwTime = dwTime;

                    if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        // Hold µÇ¾î ÀÖ¾ú´ø »óÅÂ¶ó¸é Ç®¸°´Ù. 	
                        m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                    }

                    //Crusade
                    int iExp;

                    // NPC¿¡ ´ëÇÑ °ø°ÝÀÌ ¼º°øÇßÀ¸¹Ç·Î °ø°ÝÀÚ°¡ ÇÃ·¹ÀÌ¾î¶ó¸é ÀÔÈù ´ë¹ÌÁö ¸¸Å­ÀÇ °æÇèÄ¡¸¦ °ø°ÝÀÚ¿¡°Ô ÁØ´Ù. 
                    if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != TRUE) &&
                        (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != NULL))
                    {
                        // ExpStockÀ» ¿Ã¸°´Ù. ´Ü ¼ÒÈ¯¸÷ÀÎ °æ¿ì °æÇèÄ¡¸¦ ¿Ã¸®Áö ¾Ê´Â´Ù.
                        if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage)
                        {
                            // Crusade
                            iExp = iDamage;
                            if ((m_bIsCrusadeMode == TRUE) && (iExp > 10)) iExp = 10;

                            //v2.03 918 °æÇèÄ¡ Áõ°¡ 
                            if (m_pClientList[sAttackerH]->m_iAddExp > 0)
                            {
                                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                dTmp2 = (double)iExp;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                iExp += (int)dTmp3;
                            }

                            // v2.17 2002-8-6 °ø°ÝÀÚÀÇ ·¹º§ÀÌ 100 ÀÌ»óÀÌ¸é Åä³¢³ª °í¾çÀÌ¸¦ ÀâÀ»¶§ °æÇèÄ¡°¡ ¿Ã¶ó°¡Áö ¾Ê´Â´Ù.
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

                            if (bExp == TRUE)
                                GetExp(sAttackerH, iExp); //m_pClientList[sAttackerH]->m_iExpStock += iExp;     //iDamage;
                            else GetExp(sAttackerH, (iExp / 2)); //m_pClientList[sAttackerH]->m_iExpStock += (iExp/2); //(iDamage/2);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
                        }
                        else
                        {
                            // Crusade
                            iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            if ((m_bIsCrusadeMode == TRUE) && (iExp > 10)) iExp = 10;

                            //v2.03 918 °æÇèÄ¡ Áõ°¡ 
                            if (m_pClientList[sAttackerH]->m_iAddExp > 0)
                            {
                                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
                                dTmp2 = (double)iExp;
                                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                                iExp += (int)dTmp3;
                            }

                            // v2.17 2002-8-6 °ø°ÝÀÚÀÇ ·¹º§ÀÌ 100 ÀÌ»óÀÌ¸é Åä³¢³ª °í¾çÀÌ¸¦ ÀâÀ»¶§ °æÇèÄ¡°¡ ¿Ã¶ó°¡Áö ¾Ê´Â´Ù.
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


                            if (bExp == TRUE)
                                GetExp(sAttackerH, iExp); //m_pClientList[sAttackerH]->m_iExpStock += iExp;     //m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            else GetExp(sAttackerH, (iExp / 2)); //m_pClientList[sAttackerH]->m_iExpStock += (iExp/2); //(m_pNpcList[sTargetH]->m_iNoDieRemainExp/2);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
                        }
                    }
                }
            }
            break;
    }
}

void CGame::Effect_HpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)
{
    int iHP, iMaxHP;
    DWORD dwTime = timeGetTime();

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == NULL) return;

    iHP = iDice(sV1, sV2) + sV3;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == NULL) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return;
            iMaxHP = (3 * m_pClientList[sTargetH]->m_iVit) + (2 * m_pClientList[sTargetH]->m_iLevel) + (m_pClientList[sTargetH]->m_iStr / 2);
            if (m_pClientList[sTargetH]->m_iSideEffect_MaxHPdown != 0)
                iMaxHP = iMaxHP - (iMaxHP / m_pClientList[sTargetH]->m_iSideEffect_MaxHPdown);
            if (m_pClientList[sTargetH]->m_iHP < iMaxHP)
            {
                m_pClientList[sTargetH]->m_iHP += iHP;
                if (m_pClientList[sTargetH]->m_iHP > iMaxHP) m_pClientList[sTargetH]->m_iHP = iMaxHP;
                if (m_pClientList[sTargetH]->m_iHP <= 0)     m_pClientList[sTargetH]->m_iHP = 1;
                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
            }
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == NULL) return;
            if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
            if (m_pNpcList[sTargetH]->m_bIsKilled == TRUE) return;
            iMaxHP = m_pNpcList[sTargetH]->m_iHitDice * 4;
            if (m_pNpcList[sTargetH]->m_iHP < iMaxHP)
            {
                m_pNpcList[sTargetH]->m_iHP += iHP;
                if (m_pNpcList[sTargetH]->m_iHP > iMaxHP) m_pNpcList[sTargetH]->m_iHP = iMaxHP;
                if (m_pNpcList[sTargetH]->m_iHP <= 0)     m_pNpcList[sTargetH]->m_iHP = 1;
            }
            break;
    }
}

void CGame::Effect_SpDown_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)
{
    int iSP, iMaxSP;
    DWORD dwTime = timeGetTime();

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == NULL) return;

    // Ã‡Ã˜Â´Ã§ Ã…Â¸Ã„ÃÃ€Ã‡ SpÂ¸Â¦ Â³Â»Â¸Â°Â´Ã™.
    iSP = iDice(sV1, sV2) + sV3;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == NULL) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return;

            // Is the user having an invincibility slate
            if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return;

            iMaxSP = (2 * m_pClientList[sTargetH]->m_iStr) + (2 * m_pClientList[sTargetH]->m_iLevel);
            if (m_pClientList[sTargetH]->m_iSP > 0)
            {

                //v1.42 
                if (m_pClientList[sTargetH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[sTargetH]->m_iSP -= iSP;
                    if (m_pClientList[sTargetH]->m_iSP < 0) m_pClientList[sTargetH]->m_iSP = 0;
                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                }
            }
            break;

        case DEF_OWNERTYPE_NPC:
            // NPCÂ´Ã‚ Â½ÂºÃ…Ã‚Â¹ÃŒÂ³ÃŠ Â°Â³Â³Ã¤Ã€ÃŒ Â¾Ã¸Â´Ã™.
            break;
    }
}

void CGame::Effect_SpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)
{
    int iSP, iMaxSP;
    DWORD dwTime = timeGetTime();

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == NULL) return;

    // Ã‡Ã˜Â´Ã§ Ã…Â¸Ã„ÃÃ€Ã‡ SpÂ¸Â¦ Â¿ÃƒÂ¸Â°Â´Ã™.
    iSP = iDice(sV1, sV2) + sV3;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == NULL) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return;

            iMaxSP = (2 * m_pClientList[sTargetH]->m_iStr) + (2 * m_pClientList[sTargetH]->m_iLevel);
            if (m_pClientList[sTargetH]->m_iSP < iMaxSP)
            {
                m_pClientList[sTargetH]->m_iSP += iSP;

                if (m_pClientList[sTargetH]->m_iSP > iMaxSP)
                    m_pClientList[sTargetH]->m_iSP = iMaxSP;

                SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
            }
            break;

        case DEF_OWNERTYPE_NPC:
            // NPCÂ´Ã‚ Â½ÂºÃ…Ã‚Â¹ÃŒÂ³ÃŠ Â°Â³Â³Ã¤Ã€ÃŒ Â¾Ã¸Â´Ã™.
            break;
    }
}
