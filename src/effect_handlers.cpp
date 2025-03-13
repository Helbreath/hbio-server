//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "map.h"

void CGame::Effect_Damage_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3, bool bExp, int iAttr)
{
    int iPartyID, iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iMaxSuperAttack;
    char cAttackerSide, cDamageMoveDir;
    uint32_t dwTime;
    double dTmp1, dTmp2, dTmp3;
    short sAtkX, sAtkY, sTgtX, sTgtY, dX, dY;

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == 0) return;

    if (cAttackerType == DEF_OWNERTYPE_NPC)
        if (m_pNpcList[sAttackerH] == 0) return;

    dwTime = timeGetTime();


    iDamage = iDice(sV1, sV2) + sV3;
    if (iDamage <= 0) iDamage = 0;

    iPartyID = 0;


    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER:


            if (m_bIsCrusadeMode == false &&
                (m_pClientList[sAttackerH]->m_bIsHunter == true) &&
                (cTargetType == DEF_OWNERTYPE_PLAYER)) return;

            dTmp1 = (double)iDamage;
            if (m_pClientList[sAttackerH]->m_iMag <= 0)
                dTmp2 = 1.0f;
            else dTmp2 = (double)m_pClientList[sAttackerH]->m_iMag;

            dTmp2 = dTmp2 / 3.3f;
            dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
            iDamage = (int)(dTmp3 + 0.5f);

            iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;

            if (iDamage <= 0) iDamage = 0;


            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)
                iDamage += iDamage / 3;


            if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == true) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
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

            if (m_pClientList[sTargetH] == 0) return;
            if (m_pClientList[sTargetH]->m_bIsInitComplete == false) return;

            if (m_pClientList[sTargetH]->m_bIsKilled == true) return;


            if ((m_bIsCrusadeMode == false) &&
                (m_pClientList[sTargetH]->m_iPKCount == 0) &&
                (m_pClientList[sTargetH]->m_bIsHunter == true) &&
                (cAttackerType == DEF_OWNERTYPE_PLAYER)) return;

#ifdef DEF_ALLPLAYERPROTECT

            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {
                if (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NEVERNONPK)
                {
                    if (m_pClientList[sTargetH]->m_iPKCount == 0)  return;
                }
                else
                {
                    if ((m_bIsCrusadeMode == false) &&
                        (m_pClientList[sTargetH]->m_iPKCount == 0) &&
                        (m_pClientList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide) &&
                        (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return;
                }
            }

#endif  
#ifdef DEF_SAMESIDETOWNPROTECT

            if ((m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_cSide == cAttackerSide) && (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return;
#endif 		
#ifdef DEF_BEGINNERTOWNPROTECT
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return;
#endif

            if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;

            if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == false) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;


            if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;


            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {

                if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true)
                {
                    iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
                    if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
                    {

                    }
                    else
                    {

                        if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)
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

                    ItemDepleteHandler(sTargetH, iIndex, true);
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

            if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == true) &&
                (iDice(1, 10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage))
            {

                iDamage = m_pClientList[sTargetH]->m_iHP - 1;
            }


            if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                iDamage = iDamage / 2;


            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true))
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

                            SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
                        }
                    }


                    SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);

                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);


                    if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != true)
                    {
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                    }

                    if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {

                        // 1: Hold-Person 
                        // 2: Paralyze
                        SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], 0, 0);

                        m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }
            }

            sTgtX = m_pClientList[sTargetH]->m_sX;
            sTgtY = m_pClientList[sTargetH]->m_sY;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == 0) return;
            if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
            if ((m_bIsCrusadeMode == true) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;

            sTgtX = m_pNpcList[sTargetH]->m_sX;
            sTgtY = m_pNpcList[sTargetH]->m_sY;

            switch (m_pNpcList[sTargetH]->m_cActionLimit)
            {
                case 1:
                case 2:
                case 6:
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

                    if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false)
                    {

                        cDamageMoveDir = iDice(1, 8);
                        dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                        dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) return;
                    }


                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);

                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
                    m_pNpcList[sTargetH]->m_sX = dX;
                    m_pNpcList[sTargetH]->m_sY = dY;
                    m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;


                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);


                    dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                    dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                    if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false)
                    {

                        cDamageMoveDir = iDice(1, 8);
                        dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
                        dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

                        if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) return;
                    }


                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);

                    m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
                    m_pNpcList[sTargetH]->m_sX = dX;
                    m_pNpcList[sTargetH]->m_sY = dY;
                    m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;


                    SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);

                    if (bCheckEnergySphereDestination(sTargetH, sAttackerH, cAttackerType) == true)
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


                SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
                //

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

                        m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                    }

                    int iExp;

                    if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != true) &&
                        (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
                    {

                        if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage)
                        {
                            iExp = iDamage;
                            if ((m_bIsCrusadeMode == true) && (iExp > 10)) iExp = 10;

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

                            if (bExp == true)
                                GetExp(sAttackerH, iExp, true);
                            else GetExp(sAttackerH, (iExp / 2), true);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
                        }
                        else
                        {
                            // Crusade
                            iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            if ((m_bIsCrusadeMode == true) && (iExp > 10)) iExp = 10;

                            //v2.03 918 °æÇèÄ¡ Áõ°¡ 
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


                            if (bExp == true)
                                GetExp(sAttackerH, iExp, true); //m_pClientList[sAttackerH]->m_iExpStock += iExp;     //m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            else GetExp(sAttackerH, (iExp / 2), true); //m_pClientList[sAttackerH]->m_iExpStock += (iExp/2); //(m_pNpcList[sTargetH]->m_iNoDieRemainExp/2);
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
                        }
                    }
                }
            }
            break;
    }
}

void CGame::Effect_Damage_Spot_DamageMove(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, bool bExp, int iAttr)
{
    int iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iMaxSuperAttack;
    uint32_t dwTime;
    char cAttackerSide, cDamageMoveDir;
    double dTmp1, dTmp2, dTmp3;
    int iPartyID, iMoveDamage;
    short sTgtX, sTgtY;

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == 0) return;

    if (cAttackerType == DEF_OWNERTYPE_NPC)
        if (m_pNpcList[sAttackerH] == 0) return;

    dwTime = timeGetTime();
    sTgtX = 0;
    sTgtY = 0;


    iDamage = iDice(sV1, sV2) + sV3;
    if (iDamage <= 0) iDamage = 0;

    iPartyID = 0;


    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER:


            if ((m_bIsCrusadeMode == false) &&
                (m_pClientList[sAttackerH]->m_bIsHunter == true) &&
                cTargetType == DEF_OWNERTYPE_PLAYER) return;

            dTmp1 = (double)iDamage;
            if (m_pClientList[sAttackerH]->m_iMag <= 0)
                dTmp2 = 1.0f;
            else dTmp2 = (double)m_pClientList[sAttackerH]->m_iMag;

            dTmp2 = dTmp2 / 3.3f;
            dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
            iDamage = (int)(dTmp3 + 0.5f);
            if (iDamage <= 0) iDamage = 0;

            iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;


            if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)
                iDamage += iDamage / 3;


            if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == true) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
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

            if (m_pClientList[sTargetH] == 0) return;
            if (m_pClientList[sTargetH]->m_bIsInitComplete == false) return;

            if (m_pClientList[sTargetH]->m_bIsKilled == true) return;

            if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;


            if (m_pClientList[sTargetH]->m_cMapIndex == -1) return;
            if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == false) return;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;


            if ((m_bIsCrusadeMode == false) &&
                (m_pClientList[sTargetH]->m_iPKCount == 0) &&
                (m_pClientList[sTargetH]->m_bIsHunter == true) &&
                (cAttackerType == DEF_OWNERTYPE_PLAYER)) return;

#ifdef DEF_ALLPLAYERPROTECT

            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {
                if (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NEVERNONPK)
                {
                    if (m_pClientList[sTargetH]->m_iPKCount == 0)  return;
                }
                else
                {
                    if ((m_bIsCrusadeMode == false) &&
                        (m_pClientList[sTargetH]->m_iPKCount == 0) &&
                        (m_pClientList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide) &&
                        (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return;
                }
            }

#endif 
#ifdef DEF_SAMESIDETOWNPROTECT

            if ((m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_cSide == cAttackerSide) && (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return;
#endif 		
#ifdef DEF_BEGINNERTOWNPROTECT
            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_iIsOnTown == DEF_NONPK)) return;
#endif


            if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;


            if (cAttackerType == DEF_OWNERTYPE_PLAYER)
            {

                if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true)
                {
                    iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
                    if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
                    {

                    }
                    else
                    {

                        if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)
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

                    ItemDepleteHandler(sTargetH, iIndex, true);
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

            if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == true) &&
                (iDice(1, 10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage))
            {

                iDamage = m_pClientList[sTargetH]->m_iHP - 1;
            }


            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true))
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

                            SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
                        }
                    }


                    if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true))
                        iMoveDamage = 80;
                    else iMoveDamage = 50;

                    if (iDamage >= iMoveDamage)
                    {

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


                        m_pClientList[sTargetH]->m_iLastDamage = iDamage;

                        SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);

                        SendNotifyMsg(0, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iDamage, 0, 0);
                    }
                    else
                    {
                        EDSD_SKIPDAMAGEMOVE:;

                        SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);

                        SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
                    }


                    if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != true)
                    {
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                        m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
                    }

                    if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {

                        // 1: Hold-Person 
                        // 2: Paralyze
                        SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], 0, 0);

                        m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }
            }
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == 0) return;
            if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
            if ((m_bIsCrusadeMode == true) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;

            switch (m_pNpcList[sTargetH]->m_cActionLimit)
            {
                case 1:
                case 2:
                case 4:
                case 6:
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


                SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);

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

                        m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
                        bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
                    }

                    int iExp;

                    if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != true) &&
                        (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
                    {

                        if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage)
                        {
                            iExp = iDamage;
                            if ((m_bIsCrusadeMode == true) && (iExp > 10)) iExp = 10;

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

                            if (bExp == true)
                                GetExp(sAttackerH, iExp);
                            else GetExp(sAttackerH, (iExp / 2));
                            m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
                        }
                        else
                        {
                            iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;
                            if ((m_bIsCrusadeMode == true) && (iExp > 10)) iExp = 10;

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


                            if (bExp == true)
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
    uint32_t dwTime = timeGetTime();

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == 0) return;

    iHP = iDice(sV1, sV2) + sV3;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == 0) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == true) return;

            iMaxHP = (3 * m_pClientList[sTargetH]->m_iVit) + (2 * m_pClientList[sTargetH]->m_iLevel) + (m_pClientList[sTargetH]->m_iStr / 2);


            if (m_pClientList[sTargetH]->m_iSideEffect_MaxHPdown != 0)
                iMaxHP = iMaxHP - (iMaxHP / m_pClientList[sTargetH]->m_iSideEffect_MaxHPdown);

            if (m_pClientList[sTargetH]->m_iHP < iMaxHP)
            {
                m_pClientList[sTargetH]->m_iHP += iHP;

                if (m_pClientList[sTargetH]->m_iHP > iMaxHP) m_pClientList[sTargetH]->m_iHP = iMaxHP;
                if (m_pClientList[sTargetH]->m_iHP <= 0)     m_pClientList[sTargetH]->m_iHP = 1;

                SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);
            }
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sTargetH] == 0) return;
            if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
            if (m_pNpcList[sTargetH]->m_bIsKilled == true) return;

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
    int iSP, iMaxSP, iSideCondition;
    uint32_t dwTime = timeGetTime();

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == 0) return;


    iSP = iDice(sV1, sV2) + sV3;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == 0) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == true) return;


            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0))
            {
                if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (m_pClientList[sAttackerH]->m_iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;

                if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true)
                {
                    iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
                    if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6))
                    {

                    }
                    else
                    {

                        if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)
                        {
                            if (m_pClientList[sAttackerH]->m_iGuildGUID != m_pClientList[sTargetH]->m_iGuildGUID)
                            {

                            }
                            else return;
                        }
                        else return;
                    }
                }
            }

            iMaxSP = (2 * m_pClientList[sTargetH]->m_iStr) + (2 * m_pClientList[sTargetH]->m_iLevel);
            if (m_pClientList[sTargetH]->m_iSP > 0)
            {

                //v1.42 
                if (m_pClientList[sTargetH]->m_iTimeLeft_FirmStaminar == 0)
                {
                    m_pClientList[sTargetH]->m_iSP -= iSP;
                    if (m_pClientList[sTargetH]->m_iSP < 0) m_pClientList[sTargetH]->m_iSP = 0;
                    SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SP, 0, 0, 0, 0);
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
    uint32_t dwTime = timeGetTime();

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
        if (m_pClientList[sAttackerH] == 0) return;


    iSP = iDice(sV1, sV2) + sV3;

    switch (cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sTargetH] == 0) return;
            if (m_pClientList[sTargetH]->m_bIsKilled == true) return;

            iMaxSP = (2 * m_pClientList[sTargetH]->m_iStr) + (2 * m_pClientList[sTargetH]->m_iLevel);
            if (m_pClientList[sTargetH]->m_iSP < iMaxSP)
            {
                m_pClientList[sTargetH]->m_iSP += iSP;

                if (m_pClientList[sTargetH]->m_iSP > iMaxSP)
                    m_pClientList[sTargetH]->m_iSP = iMaxSP;

                SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SP, 0, 0, 0, 0);
            }
            break;

        case DEF_OWNERTYPE_NPC:

            break;
    }
}

