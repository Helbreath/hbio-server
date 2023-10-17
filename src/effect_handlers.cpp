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
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859) // NecklaceOfKloness 
                    { 
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
                    if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859) // NecklaceOfKloness  
                    {
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

    iDamage = iDice(sV1, sV2) + sV3;
    if (iDamage <= 0) iDamage = 0;

    iPartyID = 0;

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

            iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;

            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                iDamage += iDamage / 3;

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
            if (m_pClientList[sTargetH] == NULL) return;
            if (m_pClientList[sTargetH]->m_bIsInitComplete == FALSE) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == TRUE) return;
            if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;
            if (m_pClientList[sTargetH]->m_cMapIndex == -1) return;
            if ((m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) && (m_pClientList[sTargetH]->m_iAdminUserLevel == 0)) return;
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == TRUE) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;

            if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == TRUE)) return;
            if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == TRUE)) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == TRUE) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == TRUE)) return;

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
                        // 
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
                        iMoveDamage = 80;
                    else iMoveDamage = 50;

                    if (iDamage >= iMoveDamage)
                    {
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

                        m_pClientList[sTargetH]->m_iLastDamage = iDamage;
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iDamage, NULL, NULL);
                    }
                    else
                    {
                        EDSD_SKIPDAMAGEMOVE:;
                        SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);
                    }

                    if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != TRUE)
                    {
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                    }

                    if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        // 1: Hold-Person 
                        // 2: Paralyze
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

                    int iExp;

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
                                GetExp(sAttackerH, iExp);
                            else GetExp(sAttackerH, (iExp / 2));
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
                                GetExp(sAttackerH, iExp);
                            else GetExp(sAttackerH, (iExp / 2));
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
                if (m_pClientList[sTargetH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[sTargetH]->m_iSP -= iSP;
                    if (m_pClientList[sTargetH]->m_iSP < 0) m_pClientList[sTargetH]->m_iSP = 0;
                    SendNotifyMsg(NULL, sTargetH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                }
            }
            break;

        case DEF_OWNERTYPE_NPC:
            break;
    }
}

void CGame::Effect_SpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)
{
    int iSP, iMaxSP;
    DWORD dwTime = timeGetTime();

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == NULL) return;

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
            break;
    }
}
