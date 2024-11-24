//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include "Map.h"

extern char G_cTxt[512];

int  _tmp_iMCProb[] = { 0, 300, 250, 200, 150, 100, 80, 70, 60, 50, 40 };
int  _tmp_iMLevelPenalty[] = { 0,   5,   5,   8,   8,  10, 14, 28, 32, 36, 40 };
void CGame::PlayerMagicHandler(int iClientH, int dX, int dY, short sType, bool bItemEffect, int iV1)
{
    short * sp, sX, sY, sOwnerH, sMagicCircle, rx, ry, sRemainItemSprite, sRemainItemSpriteFrame, sLevelMagic;
    char * cp, cData[120]{}, cDir, cOwnerType, cName[11], cNpcWaypoint[11], cName_Master[11], cNpcName[21], cRemainItemColor;
    double dV1, dV2, dV3, dV4;
    int    i, iErr, iRet, ix, iy, iResult, iDiceRes, iNamingValue, iFollowersNum, iEraseReq, iWhetherBonus;
    int    tX, tY, iManaCost, iMagicAttr, iItemID;
    CItem * pItem;
    uint32_t * dwp, dwTime = timeGetTime();
    uint16_t * wp, wWeaponType;

#ifdef DEF_GUILDWARMODE
    switch (sType)
    {
        case 31:
        case 40:
        case 41:
        case 46:
        case 54:
        case 55:
        case 73:
            return;
    }
#endif

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    //if (((dwTime - m_pClientList[iClientH]->m_dwRecentAttackTime) < 1000) && (bItemEffect == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
    //	wsprintf(G_cTxt, "3.51 Detection: (%s) Player: (%s) - Magic casting speed is too fast! Hack?", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
    //	PutHackLogFileList(G_cTxt);
    //	DeleteClient(iClientH, true, true);
    //	return;
    //}
    //m_pClientList[iClientH]->m_dwRecentAttackTime = dwTime;
    //m_pClientList[iClientH]->m_dwLastActionTime = dwTime;

    if ((dX < 0) || (dX >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeX) ||
        (dY < 0) || (dY >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY)) return;

    if (m_pClientList[iClientH]->m_cMapIndex < 0) return;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] == 0) return;

    if ((sType < 0) || (sType >= 100))     return;
    if (m_pMagicConfigList[sType] == 0) return;

    if ((bItemEffect == false) && (m_pClientList[iClientH]->m_cMagicMastery[sType] != 1)) return;

    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled == false) return;

    if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
    {
        wWeaponType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);
        if ((wWeaponType >= 35) && (wWeaponType < 39))
        {

        }
        else return;
    }

    if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] != -1) ||
        (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)) return;

    //if ((dwTime - m_pClientList[iClientH]->m_dwRecentAttackTime) < 100) return; 
    m_pClientList[iClientH]->m_dwRecentAttackTime = dwTime;
    m_pClientList[iClientH]->m_dwLastActionTime = dwTime;

    sX = m_pClientList[iClientH]->m_sX;
    sY = m_pClientList[iClientH]->m_sY;



    //         1      2     3     4     5	 6     7	 8	  9    10


    sMagicCircle = (sType / 10) + 1;
    if (m_pClientList[iClientH]->m_cSkillMastery[4] == 0)
        dV1 = 1.0f;
    else dV1 = (double)m_pClientList[iClientH]->m_cSkillMastery[4];


    if (bItemEffect == true) dV1 = (double)100.0f;

    dV2 = (double)(dV1 / 100.0f);
    dV3 = (double)_tmp_iMCProb[sMagicCircle];

    dV1 = dV2 * dV3;
    iResult = (int)dV1;


    if (m_pClientList[iClientH]->m_iInt > 50)
        iResult += (m_pClientList[iClientH]->m_iInt - 50) / 2;


    sLevelMagic = (m_pClientList[iClientH]->m_iLevel / 10);
    if (sMagicCircle != sLevelMagic)
    {
        if (sMagicCircle > sLevelMagic)
        {

            dV1 = (double)(m_pClientList[iClientH]->m_iLevel - sLevelMagic * 10);
            dV2 = (double)abs(sMagicCircle - sLevelMagic) * _tmp_iMLevelPenalty[sMagicCircle];
            dV3 = (double)abs(sMagicCircle - sLevelMagic) * 10;
            dV4 = (dV1 / dV3) * dV2;

            iResult -= abs(abs(sMagicCircle - sLevelMagic) * _tmp_iMLevelPenalty[sMagicCircle] - (int)dV4);
        }
        else
        {

            iResult += 5 * abs(sMagicCircle - sLevelMagic);
        }
    }


    switch (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus)
    {
        case 0: break;
        case 1: iResult = iResult - (iResult / 24); break;
        case 2:	iResult = iResult - (iResult / 12); break;
        case 3: iResult = iResult - (iResult / 5);  break;
    }



    if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == 10)
    {
        dV1 = (double)iResult;
        dV2 = (double)(m_pClientList[iClientH]->m_iSpecialWeaponEffectValue * 3);
        dV3 = dV1 + dV2;
        iResult = (int)dV3;
    }

    if (iResult <= 0) iResult = 1;

    iWhetherBonus = iGetWhetherMagicBonusEffect(sType, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus);

    iManaCost = m_pMagicConfigList[sType]->m_sValue1;
    if ((m_pClientList[iClientH]->m_bIsSafeAttackMode == true) &&
        (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == false))
    {

        iManaCost += (iManaCost / 2) - (iManaCost / 10);
    }

    if (m_pClientList[iClientH]->m_iManaSaveRatio > 0)
    {

        dV1 = (double)m_pClientList[iClientH]->m_iManaSaveRatio;
        dV2 = (double)(dV1 / 100.0f);
        dV3 = (double)iManaCost;
        dV1 = dV2 * dV3;
        dV2 = dV3 - dV1;
        iManaCost = (int)dV2;

        if (iManaCost <= 0) iManaCost = 1;
    }

    if (iResult < 100)
    {

        iDiceRes = iDice(1, 100);
        if (iResult < iDiceRes)
        {

            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, 0, -1, 0);
            return;
        }
    }

    if (((m_pClientList[iClientH]->m_iHungerStatus <= 10) || (m_pClientList[iClientH]->m_iSP <= 0)) && (iDice(1, 1000) <= 100))
    {

        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, 0, -1, 0);
        return;
    }


    if (m_pClientList[iClientH]->m_iMP < iManaCost)
    {

        return;
    }


    iResult = m_pClientList[iClientH]->m_cSkillMastery[4];

    if (m_pClientList[iClientH]->m_iMag > 50) iResult += (m_pClientList[iClientH]->m_iMag - 50);


    sLevelMagic = (m_pClientList[iClientH]->m_iLevel / 10);
    if (sMagicCircle != sLevelMagic)
    {
        if (sMagicCircle > sLevelMagic)
        {

            dV1 = (double)(m_pClientList[iClientH]->m_iLevel - sLevelMagic * 10);
            dV2 = (double)abs(sMagicCircle - sLevelMagic) * _tmp_iMLevelPenalty[sMagicCircle];
            dV3 = (double)abs(sMagicCircle - sLevelMagic) * 10;
            dV4 = (dV1 / dV3) * dV2;

            iResult -= abs(abs(sMagicCircle - sLevelMagic) * _tmp_iMLevelPenalty[sMagicCircle] - (int)dV4);
        }
        else
        {

            iResult += 5 * abs(sMagicCircle - sLevelMagic);
        }
    }

    iResult += m_pClientList[iClientH]->m_iAddAR;

    if (iResult <= 0) iResult = 1;

    if (sType >= 80) iResult += 10000;

    if (m_pMagicConfigList[sType]->m_cCategory == 1)
    {

        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetAttribute(sX, sY, 0x00000005) != 0) return;
    }

    iMagicAttr = m_pMagicConfigList[sType]->m_iAttribute;

    if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
    {
        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
    }


    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

    if ((m_bIsCrusadeMode == false) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
    {

        if (((m_pClientList[iClientH]->m_bIsHunter == true) ||
            (m_pClientList[sOwnerH]->m_bIsHunter == true)) && (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide))
            goto MAGIC_NOEFFECT;

        if ((m_pClientList[iClientH]->m_bIsHunter == true) && (m_pClientList[sOwnerH]->m_bIsHunter == false))
        {

            switch (m_pMagicConfigList[sType]->m_sType)
            {
                case DEF_MAGICTYPE_SPDOWN_AREA:
                case DEF_MAGICTYPE_SUMMON:
                case DEF_MAGICTYPE_PROTECT:
                case DEF_MAGICTYPE_HOLDOBJECT:
                case DEF_MAGICTYPE_INVISIBILITY:
                case DEF_MAGICTYPE_BERSERK:
                case DEF_MAGICTYPE_POISON:
                case DEF_MAGICTYPE_HPUP_SPOT:
                    goto MAGIC_NOEFFECT;
            }
        }
    }



    if (m_pMagicConfigList[sType]->m_dwDelayTime == 0)
    {

        switch (m_pMagicConfigList[sType]->m_sType)
        {
            case DEF_MAGICTYPE_POLYMORPH:

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (1)
                { // bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false) {
                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] != 0) goto MAGIC_NOEFFECT;
                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] = (char)m_pMagicConfigList[sType]->m_sValue4;

                            m_pClientList[sOwnerH]->m_sOriginalType = m_pClientList[sOwnerH]->m_sType;

                            m_pClientList[sOwnerH]->m_sType = 18;
                            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] != 0) goto MAGIC_NOEFFECT;
                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] = (char)m_pMagicConfigList[sType]->m_sValue4;

                            m_pNpcList[sOwnerH]->m_sOriginalType = m_pNpcList[sOwnerH]->m_sType;

                            m_pNpcList[sOwnerH]->m_sType = 18;
                            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                            break;
                    }


                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_POLYMORPH, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                        sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);


                    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POLYMORPH, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                }
                break;

            case DEF_MAGICTYPE_DAMAGE_SPOT:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                }
                break;

            case DEF_MAGICTYPE_DAMAGE_SPOT_SPDOWN:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                {
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                }

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    {
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                }
                break;


            case DEF_MAGICTYPE_DAMAGE_LINEAR:

                sX = m_pClientList[iClientH]->m_sX;
                sY = m_pClientList[iClientH]->m_sY;

                for (i = 2; i < 10; i++)
                {
                    iErr = 0;
                    m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
                }


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                        }
                    }

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr); // v1.41 false

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr); // v1.41 false
                }
                break;


            case DEF_MAGICTYPE_ICE_LINEAR:

                sX = m_pClientList[iClientH]->m_sX;
                sY = m_pClientList[iClientH]->m_sY;

                for (i = 2; i < 10; i++)
                {
                    iErr = 0;
                    m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                    }
                                }
                                break;
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                        }
                                    }
                                    break;
                            }
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                    }
                                }
                                break;
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                        }
                                    }
                                    break;
                            }
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                    }
                                }
                                break;
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                        }
                                    }
                                    break;
                            }
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                    }
                                }
                                break;
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                        }
                                    }
                                    break;
                            }
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                    }
                                }
                                break;
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                        }
                                    }
                                    break;
                            }
                        }
                    }

                    if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
                }


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                        }
                                    }
                                    break;
                            }
                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            {
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                                switch (cOwnerType)
                                {
                                    case DEF_OWNERTYPE_PLAYER:
                                        if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                        if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                        {
                                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                            {
                                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                                SetIceFlag(sOwnerH, cOwnerType, true);

                                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                    sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                            }
                                        }
                                        break;

                                    case DEF_OWNERTYPE_NPC:
                                        if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                        if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                        {
                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                            {
                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                                SetIceFlag(sOwnerH, cOwnerType, true);

                                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                    sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                            }
                                        }
                                        break;
                                }
                            }
                        }
                    }

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                {
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr); // v1.41 false

                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                            if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                            {
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                {
                                    m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                    SetIceFlag(sOwnerH, cOwnerType, true);

                                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                        sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                }
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                            if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                            {
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                {
                                    m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                    SetIceFlag(sOwnerH, cOwnerType, true);

                                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                        sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                }
                            }
                            break;
                    }
                }

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    {
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr); // v1.41 false

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                    }
                                }
                                break;
                        }
                    }
                }
                break;

            case DEF_MAGICTYPE_HPUP_SPOT:

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                Effect_HpUp_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                break;

            case DEF_MAGICTYPE_TREMOR:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                }


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                }


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT:


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT_SPDOWN:


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, false, iMagicAttr);
                            Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            {
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, false, iMagicAttr);
                                Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                            }
                        }
                    }
                break;

            case DEF_MAGICTYPE_SPDOWN_AREA:

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;

            case DEF_MAGICTYPE_SPUP_AREA:

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                Effect_SpUp_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);

                        Effect_SpUp_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;

            case DEF_MAGICTYPE_TELEPORT:

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:

                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (sOwnerH == iClientH))
                        {

                            RequestTeleportHandler(iClientH, "1   ");
                        }
                        break;
                }
                break;

            case DEF_MAGICTYPE_SUMMON:
                if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == true) return;

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);


                if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[iClientH]->m_cSide == m_pClientList[sOwnerH]->m_cSide))
                {

                    iFollowersNum = iGetFollowerNumber(sOwnerH, cOwnerType);


                    if (iFollowersNum >= (m_pClientList[iClientH]->m_cSkillMastery[4] / 20)) break;

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


                        memset(cNpcName, 0, sizeof(cNpcName));

                        switch (iV1)
                        {
                            case 0:
                                iResult = iDice(1, m_pClientList[iClientH]->m_cSkillMastery[4] / 10);


                                if (iResult < m_pClientList[iClientH]->m_cSkillMastery[4] / 20)
                                    iResult = m_pClientList[iClientH]->m_cSkillMastery[4] / 20;

                                switch (iResult)
                                {
                                    case 1: strcpy(cNpcName, "Slime"); break;
                                    case 2: strcpy(cNpcName, "Giant-Ant"); break;
                                    case 3: strcpy(cNpcName, "Amphis"); break;
                                    case 4: strcpy(cNpcName, "Orc"); break;
                                    case 5: strcpy(cNpcName, "Skeleton"); break;
                                    case 6:	strcpy(cNpcName, "Clay-Golem"); break;
                                    case 7:	strcpy(cNpcName, "Stone-Golem"); break;
                                    case 8: strcpy(cNpcName, "Orc-Mage"); break;
                                    case 9:	strcpy(cNpcName, "Hellbound"); break;
                                    case 10:strcpy(cNpcName, "Cyclops"); break;
                                }
                                break;

                            case 1:	strcpy(cNpcName, "Orc"); break;
                            case 2: strcpy(cNpcName, "Skeleton"); break;
                            case 3: strcpy(cNpcName, "Clay-Golem"); break;
                            case 4: strcpy(cNpcName, "Stone-Golem"); break;
                            case 5: strcpy(cNpcName, "Hellbound"); break;
                            case 6: strcpy(cNpcName, "Cyclops"); break;
                            case 7: strcpy(cNpcName, "Troll"); break;
                            case 8: strcpy(cNpcName, "Orge"); break;
                        }

                        if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM, &dX, &dY, cNpcWaypoint, 0, 0, m_pClientList[iClientH]->m_cSide, false, true) == false)
                        {

                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                        }
                        else
                        {
                            memset(cName_Master, 0, sizeof(cName_Master));
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    memcpy(cName_Master, m_pClientList[sOwnerH]->m_cCharName, 10);
                                    break;
                                case DEF_OWNERTYPE_NPC:
                                    memcpy(cName_Master, m_pNpcList[sOwnerH]->m_cName, 5);
                                    break;
                            }
                            bSetNpcFollowMode(cName, cName_Master, cOwnerType);
#ifdef DEF_TAIWANLOG
                            _bItemLog(DEF_ITEMLOG_SUMMONMONSTER, iClientH, cNpcName, NULL);
#endif
                        }
                    }
                }
                break;

            case DEF_MAGICTYPE_CREATE:



                if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetIsMoveAllowedTile(dX, dY) == false)
                    goto MAGIC_NOEFFECT;

                pItem = new CItem;

                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:


                        if (iDice(1, 2) == 1)
                            iItemID = 99;
                        else iItemID = 98;
                        break;
                }

                _bInitItemAttr(pItem, iItemID);


                pItem->m_sTouchEffectType = DEF_ITET_ID;
                pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                pItem->m_sTouchEffectValue2 = iDice(1, 100000);
                pItem->m_sTouchEffectValue3 = timeGetTime();


                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(dX, dY, pItem);


                _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);


                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                    dX, dY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
                break;

            case DEF_MAGICTYPE_PROTECT:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);


                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0) goto MAGIC_NOEFFECT;

                        if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) goto MAGIC_NOEFFECT;


                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                        switch (m_pMagicConfigList[sType]->m_sValue4)
                        {
                            case 1:
                                SetProtectionFromArrowFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, true);
                                break;
                            case 2:
                            case 5:
                                SetMagicProtectionFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, true);
                                break;
                            case 3:
                            case 4:
                                SetDefenseShieldFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, true);
                                break;
                        }
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0) goto MAGIC_NOEFFECT;

                        if (m_pNpcList[sOwnerH]->m_cActionLimit != 0) goto MAGIC_NOEFFECT;
                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = (char)m_pMagicConfigList[sType]->m_sValue4;

                        switch (m_pMagicConfigList[sType]->m_sValue4)
                        {
                            case 1:
                                SetProtectionFromArrowFlag(sOwnerH, DEF_OWNERTYPE_NPC, true);
                                break;
                            case 2:
                            case 5:
                                SetMagicProtectionFlag(sOwnerH, DEF_OWNERTYPE_NPC, true);
                                break;
                            case 3:
                            case 4:
                                SetDefenseShieldFlag(sOwnerH, DEF_OWNERTYPE_NPC, true);
                                break;
                        }
                        break;
                }


                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_PROTECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                    sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);


                if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_PROTECT, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                break;

            case DEF_MAGICTYPE_HOLDOBJECT:

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                {

                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto MAGIC_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_iAddPR >= 500) goto MAGIC_NOEFFECT;

                            if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) goto MAGIC_NOEFFECT;

                            if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            {

                                if (m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->iGetAttribute(sX, sY, 0x00000006) != 0) goto MAGIC_NOEFFECT;
                                if (m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->iGetAttribute(dX, dY, 0x00000006) != 0) goto MAGIC_NOEFFECT;
                            }


                            if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "middleland") != 0 &&
                                m_bIsCrusadeMode == false && m_pClientList[iClientH]->m_cSide == m_pClientList[sOwnerH]->m_cSide)
                                goto MAGIC_NOEFFECT;

                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicLevel >= 6) goto MAGIC_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto MAGIC_NOEFFECT;
                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;
                    }


                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                        sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);


                    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_HOLDOBJECT, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                }
                break;

            case DEF_MAGICTYPE_INVISIBILITY:
                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto MAGIC_NOEFFECT;

                                if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) goto MAGIC_NOEFFECT;

                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetInvisibilityFlag(sOwnerH, cOwnerType, true);

                                RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto MAGIC_NOEFFECT;

                                if (m_pNpcList[sOwnerH]->m_cActionLimit == 0)
                                {

                                    m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                    SetInvisibilityFlag(sOwnerH, cOwnerType, true);

                                    RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_INVISIBILITY);
                                }
                                break;
                        }


                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_INVISIBILITY, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                            sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);

                        if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_INVISIBILITY, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                        break;

                    case 2:

                        if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) goto MAGIC_NOEFFECT;


                        for (ix = dX - 8; ix <= dX + 8; ix++)
                            for (iy = dY - 8; iy <= dY + 8; iy++)
                            {
                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (sOwnerH != 0)
                                {
                                    switch (cOwnerType)
                                    {
                                        case DEF_OWNERTYPE_PLAYER:
                                            if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                            if (m_pClientList[sOwnerH]->m_bIsHunter == true) goto MAGIC_NOEFFECT;

                                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0)
                                            {
                                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                                                SetInvisibilityFlag(sOwnerH, cOwnerType, false);
                                                bRemoveFromDelayEventList(sOwnerH, cOwnerType, DEF_MAGICTYPE_INVISIBILITY);
                                            }
                                            break;

                                        case DEF_OWNERTYPE_NPC:
                                            if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0)
                                            {
                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                                                SetInvisibilityFlag(sOwnerH, cOwnerType, false);
                                                bRemoveFromDelayEventList(sOwnerH, cOwnerType, DEF_MAGICTYPE_INVISIBILITY);
                                            }
                                            break;
                                    }
                                }
                            }
                        break;
                }
                break;

            case DEF_MAGICTYPE_CREATE_DYNAMIC:



                if (m_bIsCrusadeMode == false)
                {
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresden") == 0) return;
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvine") == 0) return;
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "arefarm") == 0) return;
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvfarm") == 0) return;
                }

                switch (m_pMagicConfigList[sType]->m_sValue10)
                {
                    case DEF_DYNAMICOBJECT_PCLOUD_BEGIN:

                    case DEF_DYNAMICOBJECT_FIRE:
                    case DEF_DYNAMICOBJECT_SPIKE:

#ifdef DEF_TAIWANLOG 
                        short sTemp_X, sTemp_Y;

                        sTemp_X = m_pClientList[iClientH]->m_sX;
                        sTemp_Y = m_pClientList[iClientH]->m_sY;

                        m_pClientList[iClientH]->m_sX = dX;
                        m_pClientList[iClientH]->m_sY = dY;

                        _bItemLog(DEF_ITEMLOG_SPELLFIELD, iClientH, m_pMagicConfigList[sType]->m_cName, NULL);

                        m_pClientList[iClientH]->m_sX = sTemp_X;
                        m_pClientList[iClientH]->m_sY = sTemp_Y;
#endif

                        switch (m_pMagicConfigList[sType]->m_sValue11)
                        {
                            case 1:
                                // wall - type
                                cDir = m_Misc.cGetNextMoveDir(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, dX, dY);
                                switch (cDir)
                                {
                                    case 1:	rx = 1; ry = 0;   break;
                                    case 2: rx = 1; ry = 1;   break;
                                    case 3: rx = 0; ry = 1;   break;
                                    case 4: rx = -1; ry = 1;  break;
                                    case 5: rx = 1; ry = 0;   break;
                                    case 6: rx = -1; ry = -1; break;
                                    case 7: rx = 0; ry = -1;  break;
                                    case 8: rx = 1; ry = -1;  break;
                                }

                                iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, m_pMagicConfigList[sType]->m_sValue10, m_pClientList[iClientH]->m_cMapIndex,
                                    dX, dY, m_pMagicConfigList[sType]->m_dwLastTime * 1000);

                                bAnalyzeCriminalAction(iClientH, dX, dY);

                                for (i = 1; i <= m_pMagicConfigList[sType]->m_sValue12; i++)
                                {
                                    iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, m_pMagicConfigList[sType]->m_sValue10, m_pClientList[iClientH]->m_cMapIndex,
                                        dX + i * rx, dY + i * ry, m_pMagicConfigList[sType]->m_dwLastTime * 1000);
                                    bAnalyzeCriminalAction(iClientH, dX + i * rx, dY + i * ry);

                                    iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, m_pMagicConfigList[sType]->m_sValue10, m_pClientList[iClientH]->m_cMapIndex,
                                        dX - i * rx, dY - i * ry, m_pMagicConfigList[sType]->m_dwLastTime * 1000);
                                    bAnalyzeCriminalAction(iClientH, dX - i * rx, dY - i * ry);
                                }
                                break;

                            case 2:
                                // Field - Type
                                bool bFlag = false;
                                int cx, cy;
                                for (ix = dX - m_pMagicConfigList[sType]->m_sValue12; ix <= dX + m_pMagicConfigList[sType]->m_sValue12; ix++)
                                    for (iy = dY - m_pMagicConfigList[sType]->m_sValue12; iy <= dY + m_pMagicConfigList[sType]->m_sValue12; iy++)
                                    {
                                        iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, m_pMagicConfigList[sType]->m_sValue10, m_pClientList[iClientH]->m_cMapIndex,
                                            ix, iy, m_pMagicConfigList[sType]->m_dwLastTime * 1000, m_pMagicConfigList[sType]->m_sValue5);


                                        if (bAnalyzeCriminalAction(iClientH, ix, iy, true) == true)
                                        {
                                            bFlag = true;
                                            cx = ix;
                                            cy = iy;
                                        }
                                    }

                                if (bFlag == true) bAnalyzeCriminalAction(iClientH, cx, cy);
                                break;
                        }
                        break;

                    case DEF_DYNAMICOBJECT_ICESTORM:
#ifdef DEF_TAIWANLOG 


                        sTemp_X = m_pClientList[iClientH]->m_sX;
                        sTemp_Y = m_pClientList[iClientH]->m_sY;

                        m_pClientList[iClientH]->m_sX = dX;
                        m_pClientList[iClientH]->m_sY = dY;

                        _bItemLog(DEF_ITEMLOG_SPELLFIELD, iClientH, m_pMagicConfigList[sType]->m_cName, NULL);

                        m_pClientList[iClientH]->m_sX = sTemp_X;
                        m_pClientList[iClientH]->m_sY = sTemp_Y;

#endif

                        // Ice-Storm Dynamic Object 
                        iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, m_pMagicConfigList[sType]->m_sValue10, m_pClientList[iClientH]->m_cMapIndex,
                            dX, dY, m_pMagicConfigList[sType]->m_dwLastTime * 1000,
                            m_pClientList[iClientH]->m_cSkillMastery[4]);
                        break;

                    default:
                        break;
                }
                break;

            case DEF_MAGICTYPE_POSSESSION:



                if (_iCalcPlayerNum(m_pClientList[iClientH]->m_cMapIndex, dX, dY, 1) != 0) break;

                pItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(dX, dY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);
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
                            dX, dY, sRemainItemSprite, sRemainItemSpriteFrame, cRemainItemColor);


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
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(dX, dY, pItem);

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
                break;

            case DEF_MAGICTYPE_CONFUSE:

                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:
                    case 2: // Confusion, Mass Confusion 	
                        for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                            for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                            {

                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                                {

                                    if (m_pClientList[sOwnerH] == 0) goto PMH_SKIP_CONFUSE;
                                    if (m_pClientList[sOwnerH]->m_cSide == m_pClientList[iClientH]->m_cSide) goto PMH_SKIP_CONFUSE;
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                    {

                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] != 0) break;
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] = (char)m_pMagicConfigList[sType]->m_sValue4;


                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_CONFUSE, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);


                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_CONFUSE, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                                    }
                                }

                                PMH_SKIP_CONFUSE:;
                            }
                        break;

                    case 3: // Illusion, Mass-Illusion

                        if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto MAGIC_NOEFFECT;
                        for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                            for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                            {

                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                                {

                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if (m_pClientList[sOwnerH]->m_cSide == m_pClientList[iClientH]->m_cSide) goto MAGIC_NOEFFECT;

                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                    {

                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] != 0) break;
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] = (char)m_pMagicConfigList[sType]->m_sValue4;


                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_CONFUSE, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);


                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_CONFUSE, m_pMagicConfigList[sType]->m_sValue4, iClientH, 0);
                                    }
                                }
                            }
                        break;
                }
                break;

            case DEF_MAGICTYPE_POISON:

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL) goto MAGIC_NOEFFECT;


                if (m_pMagicConfigList[sType]->m_sValue4 == 1)
                {

                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                            if (m_pClientList[sOwnerH]->m_bIsHunter == true) goto MAGIC_NOEFFECT;

                            if (m_pClientList[sOwnerH]->m_cSide == DEF_NETURAL) goto MAGIC_NOEFFECT;


                            bAnalyzeCriminalAction(iClientH, dX, dY);

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            {

                                if (bCheckResistingPoisonSuccess(sOwnerH, cOwnerType) == false)
                                {

                                    m_pClientList[sOwnerH]->m_bIsPoisoned = true;
                                    m_pClientList[sOwnerH]->m_iPoisonLevel = m_pMagicConfigList[sType]->m_sValue5;
                                    m_pClientList[sOwnerH]->m_dwPoisonTime = dwTime;
                                    SetPoisonFlag(sOwnerH, cOwnerType, true);

                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pMagicConfigList[sType]->m_sValue5, 0, 0);
#ifdef DEF_TAIWANLOG
                                    _bItemLog(DEF_ITEMLOG_POISONED, sOwnerH, (char *)0, NULL);
#endif
                                }
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            {

                                if (bCheckResistingPoisonSuccess(sOwnerH, cOwnerType) == false)
                                {


                                }
                            }
                            break;
                    }
                }
                else if (m_pMagicConfigList[sType]->m_sValue4 == 0)
                {

                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                            if (m_pClientList[sOwnerH]->m_bIsPoisoned == true)
                            {

                                m_pClientList[sOwnerH]->m_bIsPoisoned = false;
                                SetPoisonFlag(sOwnerH, cOwnerType, false);

                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                            break;
                    }
                }
                break;

            case DEF_MAGICTYPE_BERSERK:
                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0) goto MAGIC_NOEFFECT;
                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetBerserkFlag(sOwnerH, cOwnerType, true);
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0) goto MAGIC_NOEFFECT;

                                if (m_pNpcList[sOwnerH]->m_cActionLimit != 0) goto MAGIC_NOEFFECT;

                                if (m_pClientList[iClientH]->m_cSide != m_pNpcList[sOwnerH]->m_cSide) goto MAGIC_NOEFFECT;

                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetBerserkFlag(sOwnerH, cOwnerType, true);
                                break;
                        }


                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_BERSERK, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                            sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);

                        if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_BERSERK, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                        break;
                }
                break;


            case DEF_MAGICTYPE_DAMAGE_AREA_ARMOR_BREAK:

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                            ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            {
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                                ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                            }
                        }
                    }
                break;

            case DEF_MAGICTYPE_ICE:

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        {

                            //Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) goto MAGIC_NOEFFECT;

                                    if (m_pClientList[sOwnerH]->m_bIsHunter == true) goto MAGIC_NOEFFECT;

                                    if (m_pClientList[sOwnerH]->m_cSide == DEF_NETURAL) goto MAGIC_NOEFFECT;


                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                        }
                                    }
                                    break;
                            }

                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            {

                                //Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                            }
                        }
                    }
                break;

            default:
                break;
        }
    }
    else
    {


    }

    MAGIC_NOEFFECT:;

    if (m_pClientList[iClientH] == 0) return;


    m_pClientList[iClientH]->m_iMP -= iManaCost;
    if (m_pClientList[iClientH]->m_iMP < 0)
        m_pClientList[iClientH]->m_iMP = 0;

    CalculateSSN_SkillIndex(iClientH, 4, 1);

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);


    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);

}


void CGame::NpcMagicHandler(int iNpcH, short dX, short dY, short sType)
{
    short  sOwnerH;
    char   cOwnerType;
    int i, iErr, ix, iy, sX, sY, tX, tY, iResult, iWhetherBonus, iMagicAttr;
    uint32_t  dwTime = timeGetTime();

    if (m_pNpcList[iNpcH] == 0) return;
    if ((dX < 0) || (dX >= m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_sSizeX) ||
        (dY < 0) || (dY >= m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_sSizeY)) return;

    if ((sType < 0) || (sType >= 100))     return;
    if (m_pMagicConfigList[sType] == 0) return;


    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_bIsAttackEnabled == false) return;


    iResult = m_pNpcList[iNpcH]->m_iMagicHitRatio;


    iWhetherBonus = iGetWhetherMagicBonusEffect(sType, m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cWhetherStatus);

    iMagicAttr = m_pMagicConfigList[sType]->m_iAttribute;

    if (m_pMagicConfigList[sType]->m_dwDelayTime == 0)
    {

        switch (m_pMagicConfigList[sType]->m_sType)
        {
            case DEF_MAGICTYPE_INVISIBILITY:
                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == 0) goto NMH_NOEFFECT;
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto NMH_NOEFFECT;
                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetInvisibilityFlag(sOwnerH, cOwnerType, true);

                                RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_PLAYER);
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == 0) goto NMH_NOEFFECT;
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto NMH_NOEFFECT;
                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetInvisibilityFlag(sOwnerH, cOwnerType, true);

                                RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_NPC);
                                break;
                        }


                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_INVISIBILITY, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                            sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);

                        if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_INVISIBILITY, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                        break;

                    case 2:

                        for (ix = dX - 8; ix <= dX + 8; ix++)
                            for (iy = dY - 8; iy <= dY + 8; iy++)
                            {
                                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (sOwnerH != 0)
                                {
                                    switch (cOwnerType)
                                    {
                                        case DEF_OWNERTYPE_PLAYER:
                                            if (m_pClientList[sOwnerH] == 0) goto NMH_NOEFFECT;
                                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0)
                                            {
                                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                                                SetInvisibilityFlag(sOwnerH, cOwnerType, false);
                                                bRemoveFromDelayEventList(sOwnerH, cOwnerType, DEF_MAGICTYPE_INVISIBILITY);
                                            }
                                            break;

                                        case DEF_OWNERTYPE_NPC:
                                            if (m_pNpcList[sOwnerH] == 0) goto NMH_NOEFFECT;
                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0)
                                            {
                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                                                SetInvisibilityFlag(sOwnerH, cOwnerType, false);
                                                bRemoveFromDelayEventList(sOwnerH, cOwnerType, DEF_MAGICTYPE_INVISIBILITY);
                                            }
                                            break;
                                    }
                                }
                            }
                        break;
                }
                break;

            case DEF_MAGICTYPE_HOLDOBJECT:

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                {

                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == 0) goto NMH_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto NMH_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_iAddPR >= 500) goto NMH_NOEFFECT;
                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == 0) goto NMH_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicLevel >= 6) goto NMH_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto NMH_NOEFFECT;
                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;
                    }


                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                        sOwnerH, cOwnerType, 0, 0, 0, m_pMagicConfigList[sType]->m_sValue4, 0, 0);


                    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_HOLDOBJECT, m_pMagicConfigList[sType]->m_sValue4, 0, 0);
                }
                break;


            case DEF_MAGICTYPE_DAMAGE_LINEAR:

                sX = m_pNpcList[iNpcH]->m_sX;
                sY = m_pNpcList[iNpcH]->m_sY;

                for (i = 2; i < 10; i++)
                {
                    iErr = 0;
                    m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {

                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                    }

                    if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
                }


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                        }
                    }

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, false, iMagicAttr);

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, false, iMagicAttr);
                }
                break;

            case DEF_MAGICTYPE_DAMAGE_SPOT:
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                }
                break;

            case DEF_MAGICTYPE_HPUP_SPOT:

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                Effect_HpUp_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA:
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {

                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                }


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT:


                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                                Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, false, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_SPDOWN_AREA:

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                    Effect_SpDown_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            Effect_SpDown_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;

            case DEF_MAGICTYPE_SPUP_AREA:

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                Effect_SpUp_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);

                        Effect_SpUp_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;


            case DEF_MAGICTYPE_ICE:

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false)
                            && (bCheckResistingIceSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                        {
                            //Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);

                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == 0) continue; // goto NMH_NOEFFECT;

                                    if (m_pClientList[sOwnerH]->m_bIsHunter == true) continue; // goto NMH_NOEFFECT;

                                    if (m_pClientList[sOwnerH]->m_cSide == DEF_NETURAL) continue; // goto NMH_NOEFFECT;


                                    if (m_pClientList[sOwnerH]->m_iHP > 0)
                                    {
                                        Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, true, iMagicAttr);
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, true);

                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == 0) continue; // goto NMH_NOEFFECT;

                                    /*	if (m_pNpcList[sOwnerH]->m_iHP > 0) {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] == 0) {
                                    m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] = 1;
                                    SetIceFlag(sOwnerH, cOwnerType, true);

                                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10*1000),
                                    sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
                                    }
                                    } */
                                    break;
                            }
                        }

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {

                            if ((bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false) && (bCheckResistingIceSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == false))
                            {

                                //Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, true, iMagicAttr);
                                Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, true, iMagicAttr);

                                if (m_pClientList[sOwnerH]->m_iHP > 0)
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                            }
                        }
                    }
                break;
        }
    }
    else
    {


    }

    NMH_NOEFFECT:;


    m_pNpcList[iNpcH]->m_iMana -= m_pMagicConfigList[sType]->m_sValue1;
    if (m_pNpcList[iNpcH]->m_iMana < 0)
        m_pNpcList[iNpcH]->m_iMana = 0;


    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pNpcList[iNpcH]->m_cMapIndex,
        m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, dX, dY, (sType + 100), m_pNpcList[iNpcH]->m_sType);

}

void CGame::GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, uint16_t wObjectID, char * pItemName)
{
    int iRet, iEraseReq;
    short * sp, sOwnerH;
    char * cp, cOwnerType, cData[100]{}, cCharName[21];
    uint32_t * dwp;
    uint16_t * wp;
    CItem * pItem;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (iAmount <= 0) return;


    if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0)
    {
        log->info("GiveItemHandler - Not matching Item name");
        return;
    }

    memset(cCharName, 0, sizeof(cCharName));

    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount > (DWORD)iAmount))
    {
        pItem = new CItem();
        if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == false)
        {

            delete pItem;
            return;
        }
        else
        {
            pItem->m_dwCount = iAmount;
        }

        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount -= iAmount;

        SetItemCount(iClientH, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount);

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);


        if (wObjectID != 0)
        {
            if (wObjectID < 10000)
            {

                if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS))
                {
                    if (m_pClientList[wObjectID] != 0)
                    {
                        if ((WORD)sOwnerH != wObjectID) sOwnerH = 0;
                    }
                }
            }
            else
            {
                if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS))
                {
                    if (m_pNpcList[wObjectID - 10000] != 0)
                    {
                        if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = 0;
                    }
                }
            }
        }

        if (sOwnerH == 0)
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

            _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);


            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
        }
        else
        {
            if (cOwnerType == DEF_OWNERTYPE_PLAYER)
            {
                memcpy(cCharName, m_pClientList[sOwnerH]->m_cCharName, 10);

                if (sOwnerH == iClientH)
                {
                    delete pItem;
                    return;
                }

                if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == true)
                {
                    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                    *dwp = MSGID_NOTIFY;
                    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_NOTIFY_ITEMOBTAINED;

                    cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

                    _bItemLog(DEF_ITEMLOG_GIVE, iClientH, sOwnerH, pItem);


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


                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 53);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:

                            DeleteClient(sOwnerH, true, true);
                            break;
                    }


                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED, sItemIndex, iAmount, 0, cCharName);
                }
                else
                {
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                        m_pClientList[iClientH]->m_sY,
                        pItem);

                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);


                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);


                    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                    *dwp = MSGID_NOTIFY;
                    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 6);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:

                            DeleteClient(sOwnerH, true, true);
                            break;
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTGIVEITEM, sItemIndex, iAmount, 0, cCharName);
                }

            }
            else
            {

                memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

                if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0)
                {

                    if (bSetItemToBankItem(iClientH, pItem) == false)
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, 0, 0, 0, 0);


                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

                        _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);


                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
                    }
                }
                else
                {

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);

                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
                }
            }
        }
    }
    else
    {



        ReleaseItemHandler(iClientH, sItemIndex, true);


        if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)
            m_pClientList[iClientH]->m_cArrowIndex = -1;

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);


        if (wObjectID != 0)
        {
            if (wObjectID < 10000)
            {

                if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS))
                {
                    if (m_pClientList[wObjectID] != 0)
                    {
                        if ((WORD)sOwnerH != wObjectID) sOwnerH = 0;
                    }
                }
            }
            else
            {
                if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS))
                {
                    if (m_pNpcList[wObjectID - 10000] != 0)
                    {
                        if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = 0;
                    }
                }
            }
        }

        if (sOwnerH == 0)
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]);

            _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, 0, 0);
        }
        else
        {
            if (cOwnerType == DEF_OWNERTYPE_PLAYER)
            {
                memcpy(cCharName, m_pClientList[sOwnerH]->m_cCharName, 10);
                pItem = m_pClientList[iClientH]->m_pItemList[sItemIndex];

                if (pItem->m_sIDnum == 88)
                {
                    if ((m_pClientList[iClientH]->m_iGuildRank == -1) &&
                        (m_pClientList[iClientH]->m_cSide != DEF_NETURAL) &&
                        (m_pClientList[iClientH]->m_cSide == m_pClientList[sOwnerH]->m_cSide) &&
                        (m_pClientList[sOwnerH]->m_iGuildRank == 0))
                    {
                        SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION, 0, 0, 0, 0);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, 0, cCharName);

                        _bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

                        goto REMOVE_ITEM_PROCEDURE;
                    }
                }

                if ((m_bIsCrusadeMode == false) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89))
                {
                    if ((memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[sOwnerH]->m_cGuildName, 20) == 0) &&
                        (m_pClientList[iClientH]->m_iGuildRank != -1) &&
                        (m_pClientList[sOwnerH]->m_iGuildRank == 0))
                    {
                        SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION, 0, 0, 0, 0);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, 0, cCharName);

                        _bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

                        goto REMOVE_ITEM_PROCEDURE;
                    }
                }

                if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == true)
                {
                    _bItemLog(DEF_ITEMLOG_GIVE, iClientH, sOwnerH, pItem);

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


                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 53);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:

                            DeleteClient(sOwnerH, true, true);
                            break;
                    }
                }
                else
                {
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                        m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);


                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);


                    dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
                    *dwp = MSGID_NOTIFY;
                    wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 6);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:

                            DeleteClient(sOwnerH, true, true);
                            break;
                    }


                    memset(cCharName, 0, sizeof(cCharName));
                }
            }
            else
            {
                memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

                if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0)
                {

                    if (bSetItemToBankItem(iClientH, sItemIndex) == false)
                    {

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, 0, 0, 0, 0);


                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                            m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]);


                        _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);


                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);
                    }
                }
                else if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Kennedy", 7) == 0)
                {



                    if ((m_bIsCrusadeMode == false) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89))
                    {
                        if ((m_pClientList[iClientH]->m_iGuildRank != 0) && (m_pClientList[iClientH]->m_iGuildRank != -1))
                        {

                            SendNotifyMsg(iClientH, iClientH, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, 0, 0, 0, 0);

#ifdef DEF_TAIWANLOG
                            _bItemLog(DEF_ITEMLOG_BANGUILD, iClientH, (char *)NULL, NULL);
#endif

                            memset(m_pClientList[iClientH]->m_cGuildName, 0, sizeof(m_pClientList[iClientH]->m_cGuildName));
                            memcpy(m_pClientList[iClientH]->m_cGuildName, "NONE", 4);
                            m_pClientList[iClientH]->m_iGuildRank = -1;
                            m_pClientList[iClientH]->m_iGuildGUID = -1;


                            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);


                            m_pClientList[iClientH]->m_iExp -= 300;
                            if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
                        }


                        delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
                    }
                    else
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                            m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]);


                        _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);


                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);


                        memset(cCharName, 0, sizeof(cCharName));

                    }
                }
                else
                {


                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                        m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]);


                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);


                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);


                    memset(cCharName, 0, sizeof(cCharName));
                }
            }


            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, iAmount, 0, cCharName);
        }

        REMOVE_ITEM_PROCEDURE:;


        if (m_pClientList[iClientH] == 0) return;


        m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
        m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;


        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
    }


    iCalcTotalWeight(iClientH);
}

void CGame::ClientKilledHandler(int iClientH, int iAttackerH, char cAttackerType, short sDamage)
{
    char * cp, cAttackerName[21], cData[120];
    short sAttackerWeapon;
    int * ip, i, iExH;
    bool  bIsSAattacked = false;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;



    if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "fight", 5) == 0)
    {
        m_pClientList[iClientH]->m_dwFightzoneDeadTime = timeGetTime();
        wsprintf(G_cTxt, "Fightzone Dead Time: %d", m_pClientList[iClientH]->m_dwFightzoneDeadTime);
        log->info(G_cTxt);
    }

    m_pClientList[iClientH]->m_bIsKilled = true;

    m_pClientList[iClientH]->m_iHP = 0;


    if (m_pClientList[iClientH]->m_bIsExchangeMode == true)
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;
        _ClearExchangeStatus(iExH);
        _ClearExchangeStatus(iClientH);
    }


    RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

    memset(cAttackerName, 0, sizeof(cAttackerName));
    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER_INDIRECT:
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[iAttackerH] != 0)
                memcpy(cAttackerName, m_pClientList[iAttackerH]->m_cCharName, 10);
            break;
        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[iAttackerH] != 0)
#ifdef DEF_LOCALNPCNAME
                wsprintf(cAttackerName, "NPCNPCNPC@%d", m_pNpcList[iAttackerH]->m_sType);
#else 
                memcpy(cAttackerName, m_pNpcList[iAttackerH]->m_cNpcName, 20);
#endif
            break;
        default:
            break;
    }

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_KILLED, 0, 0, 0, cAttackerName);

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        sAttackerWeapon = ((m_pClientList[iAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
    }
    else sAttackerWeapon = 1;
    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, 0);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(12, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetDeadOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);



    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) return;

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        switch (m_pClientList[iAttackerH]->m_iSpecialAbilityType)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                bIsSAattacked = true;
                break;
        }

        if (iAttackerH == iClientH) return;

        if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
        {

            if (m_pClientList[iClientH]->m_iPKCount == 0)
            {


                ApplyPKpenalty(iAttackerH, iClientH);
            }
            else
            {

                PK_KillRewardHandler(iAttackerH, iClientH);
            }
        }
        else
        {

            if (m_pClientList[iClientH]->m_iGuildRank == -1)
            {


                if (m_pClientList[iAttackerH]->m_cSide == DEF_NETURAL)
                {

                    if (m_pClientList[iClientH]->m_iPKCount == 0)
                    {

                        ApplyPKpenalty(iAttackerH, iClientH);
                    }
                    else
                    {


                    }
                }
                else
                {

                    if (m_pClientList[iClientH]->m_cSide == m_pClientList[iAttackerH]->m_cSide)
                    {

                        if (m_pClientList[iClientH]->m_iPKCount == 0)
                        {

                            ApplyPKpenalty(iAttackerH, iClientH);
                        }
                        else
                        {

                            PK_KillRewardHandler(iAttackerH, iClientH);
                        }
                    }
                    else
                    {

                        EnemyKillRewardHandler(iAttackerH, iClientH);
                    }
                }
            }
            else
            {


                if (m_pClientList[iAttackerH]->m_cSide == DEF_NETURAL)
                {

                    if (m_pClientList[iClientH]->m_iPKCount == 0)
                    {

                        ApplyPKpenalty(iAttackerH, iClientH);
                    }
                    else
                    {


                    }
                }
                else
                {

                    if (m_pClientList[iClientH]->m_cSide == m_pClientList[iAttackerH]->m_cSide)
                    {

                        if (m_pClientList[iClientH]->m_iPKCount == 0)
                        {

                            ApplyPKpenalty(iAttackerH, iClientH);
                        }
                        else
                        {

                            PK_KillRewardHandler(iAttackerH, iClientH);
                        }
                    }
                    else
                    {

                        EnemyKillRewardHandler(iAttackerH, iClientH);
                    }
                }
            }
        }


        if (m_pClientList[iClientH]->m_iPKCount == 0)
        {
            // Innocent
            if (m_pClientList[iAttackerH]->m_cSide == DEF_NETURAL)
            {
                //??ÇàÀÚ???Ô ?ø?Ý?Þ?Æ Á??ú?Ù. 
                //PK?çÇßÀ??Ç?Î ?æÇèÄ??? ÁÙÁö ?Ê?Â?Ù.
                //m_pClientList[iClientH]->m_iExp -= iDice(1, 100);
                //if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;
                //SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
            }
            else
            {
                if (m_pClientList[iClientH]->m_cSide == m_pClientList[iAttackerH]->m_cSide)
                {
                    //°°Àº ¸¶À» ½Ã¹Î È¤Àº ±æµå¿ø¿¡°Ô Á×¾ú´Ù. 
                    //PK´çÇßÀ¸¹Ç·Î °æÇèÁö°¡ ÁÙÁö ¾Ê´Â´Ù. 
                    //m_pClientList[iClientH]->m_iExp -= iDice(1, 100);
                    //if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;
                    //SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
                }
                else
                {

                    ApplyCombatKilledPenalty(iClientH, 2, bIsSAattacked);
                }
            }
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 1) && (m_pClientList[iClientH]->m_iPKCount <= 3))
        {
            // Criminal 
            ApplyCombatKilledPenalty(iClientH, 3, bIsSAattacked, true);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 4) && (m_pClientList[iClientH]->m_iPKCount <= 11))
        {
            // Murderer 
            ApplyCombatKilledPenalty(iClientH, 6, bIsSAattacked, true);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 12))
        {
            // Slaughterer 
            ApplyCombatKilledPenalty(iClientH, 12, bIsSAattacked, true);
        }
    }
    else if (cAttackerType == DEF_OWNERTYPE_NPC)
    {

        _bPKLog(DEF_PKLOG_BYNPC, (int)-1, iClientH, cAttackerName);


        if (m_pClientList[iClientH]->m_iPKCount == 0)
        {
            // Innocent
            ApplyCombatKilledPenalty(iClientH, 1, bIsSAattacked, true);  //v2.19 2002-12-14 ¸ó½ºÅÍ¿¡°Ô Á×¾úÀ»¶§¾ÆÀÌÅÛ(¼ÒÈ¯¸÷ Á¦¿Ü)
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 1) && (m_pClientList[iClientH]->m_iPKCount <= 3))
        {
            // Criminal 
            ApplyCombatKilledPenalty(iClientH, 3, bIsSAattacked, true);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 4) && (m_pClientList[iClientH]->m_iPKCount <= 11))
        {
            // Murderer 
            ApplyCombatKilledPenalty(iClientH, 6, bIsSAattacked, true);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 12))
        {
            // Slaughterer 
            ApplyCombatKilledPenalty(iClientH, 12, bIsSAattacked, true);
        }

        if (m_pNpcList[iAttackerH]->m_iGuildGUID != 0)
        {

            if (m_pNpcList[iAttackerH]->m_cSide != m_pClientList[iClientH]->m_cSide)
            {


                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[iAttackerH]->m_iGuildGUID) &&
                        (m_pClientList[i]->m_iCrusadeDuty == 3))
                    {
                        m_pClientList[i]->m_iConstructionPoint += (m_pClientList[iClientH]->m_iLevel / 2);

                        if (m_pClientList[i]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                            m_pClientList[i]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                        //testcode
                        wsprintf(G_cTxt, "Enemy Player Killed by Npc! Construction +%d", (m_pClientList[iClientH]->m_iLevel / 2));
                        log->info(G_cTxt);

                        SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, 0, 0);
                        return;
                    }
            }
        }
    }
    else if (cAttackerType == DEF_OWNERTYPE_PLAYER_INDIRECT)
    {
        _bPKLog(DEF_PKLOG_BYOTHER, (int)-1, iClientH, NULL);

        // m_pClientList[iClientH]->m_iExp -= iDice(1, 50);
        // if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;

        // SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
    }
}

void CGame::ReleaseItemHandler(int iClientH, short sItemIndex, bool bNotice)
{
    char cEquipPos;
    short sTemp;
    int   iTemp;

    if (m_pClientList[iClientH] == 0) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return;


    if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == false) return;

    cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

    switch (cEquipPos)
    {
        case DEF_EQUIPPOS_RHAND:

            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0x0FFFFFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;


            iTemp = m_pClientList[iClientH]->m_iStatus;
            iTemp = iTemp & 0xFFFFFFF0;
            m_pClientList[iClientH]->m_iStatus = iTemp;
            break;

        case DEF_EQUIPPOS_LHAND:

            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xFFF0;
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xF0FFFFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_TWOHAND:

            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0x0FFFFFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_BODY:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;
            m_pClientList[iClientH]->m_sAppr3 = sTemp;


            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xFF7F;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFF0FFFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_BACK:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xF0FF;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFF0FFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_ARMS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFFF0;
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFF0FFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_PANTS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xF0FF;
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFF0FF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_LEGGINGS:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0x0FFF;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFF0F;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_HEAD:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFF0F;
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFFF0;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;
        case DEF_EQUIPPOS_FULLBODY: //v2.19 2002-12-7 »êÅ¸º¹ °ü·Ã 	Çìµå ¹Ùµð ¾Ï ½Å¹ß ÆÒÃ÷ ¸ÁÅä	
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;
            //		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12);
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFF0FFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;
    }

    //v1.432 Æ¯¼ö ´É·ÂÀÌ ºÎ¿©µÈ ¾ÆÀÌÅÛÀÌ¶ó¸é ÇÃ·¡±× ¼³Á¤ 
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY)
    {

        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;
    }

    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)
    {

        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC;
    }

    m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;
    m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = -1;


    if (bNotice == true)
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);


    CalcTotalItemEffect(iClientH, sItemIndex, true);
}

void CGame::PK_KillRewardHandler(short sAttackerH, short sVictumH)
{
    if (m_pClientList[sAttackerH] == 0) return;
    if (m_pClientList[sVictumH] == 0)   return;

    _bPKLog(DEF_PKLOG_BYPLAYER, sAttackerH, sVictumH, NULL);

    if (m_pClientList[sAttackerH]->m_iPKCount != 0)
    {


    }
    else
    {

        m_pClientList[sAttackerH]->m_iRewardGold += iGetExpLevel(m_pClientList[sVictumH]->m_iExp) * 3;


        if (m_pClientList[sAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
            m_pClientList[sAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
        if (m_pClientList[sAttackerH]->m_iRewardGold < 0)
            m_pClientList[sAttackerH]->m_iRewardGold = 0;

        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_PKCAPTURED, m_pClientList[sVictumH]->m_iPKCount, m_pClientList[sVictumH]->m_iLevel, 0, m_pClientList[sVictumH]->m_cCharName);
    }
}

void CGame::EnemyKillRewardHandler(int iAttackerH, int iClientH)
{
    int iRewardExp, iEK_Level;

    if (m_pClientList[iAttackerH] == 0) return;
    if (m_pClientList[iClientH] == 0)   return;

    _bPKLog(DEF_PKLOG_BYENERMY, iAttackerH, iClientH, NULL);

    iEK_Level = 30;
    if (m_pClientList[iAttackerH]->m_iLevel >= 80) iEK_Level = 80;

    /* v2.181 2002-10-24 ÁöÁ¸ÀÎ °æ¿ì °æÇèÄ¡ Àû°Ô ¸Ô´Â ¹ö±× ¼öÁ¤
    if (m_pClientList[iAttackerH]->m_iLevel >= m_iPlayerMaxLevel) {

    if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level) {

    if (memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0) {

    m_pClientList[iAttackerH]->m_iEnemyKillCount++;
    }
    }

    m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
    if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
    if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
    m_pClientList[iAttackerH]->m_iRewardGold = 0;


    SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_ENEMYKILLREWARD, iClientH, 0, 0, 0);
    return;
    } */

    if (m_pClientList[iAttackerH]->m_iPKCount != 0)
    {

    }
    else
    {

        if (m_pClientList[iClientH]->m_iGuildRank == -1)
        {


            iRewardExp = (iDice(3, (3 * iGetExpLevel(m_pClientList[iClientH]->m_iExp))) + iGetExpLevel(m_pClientList[iClientH]->m_iExp)) / 3;

            if (m_bIsCrusadeMode == true)
            {

                m_pClientList[iAttackerH]->m_iExp += iRewardExp;
                m_pClientList[iAttackerH]->m_iWarContribution += (iRewardExp) * 5;

                if (m_pClientList[iAttackerH]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                    m_pClientList[iAttackerH]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                m_pClientList[iAttackerH]->m_iConstructionPoint += m_pClientList[iClientH]->m_iLevel / 2;

                if (m_pClientList[iAttackerH]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                    m_pClientList[iAttackerH]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                //testcode
                wsprintf(G_cTxt, "Enemy Player Killed by Player! Construction: +%d WarContribution +%d", m_pClientList[iClientH]->m_iLevel / 2, (iRewardExp - (iRewardExp / 3)) * 6);
                log->info(G_cTxt);


                SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iAttackerH]->m_iConstructionPoint, m_pClientList[iAttackerH]->m_iWarContribution, 0, 0);


                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {


                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0))
                    {

                        m_pClientList[iAttackerH]->m_iEnemyKillCount++;
                    }
                }

                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
            else
            {

                m_pClientList[iAttackerH]->m_iExp += iRewardExp;

                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {


                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0))
                    {

                        m_pClientList[iAttackerH]->m_iEnemyKillCount++;
                    }
                }

                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
        }
        else
        {

            iRewardExp = (iDice(3, (3 * iGetExpLevel(m_pClientList[iClientH]->m_iExp))) + iGetExpLevel(m_pClientList[iClientH]->m_iExp)) / 3;


            if (m_bIsCrusadeMode == true)
            {


                m_pClientList[iAttackerH]->m_iExp += iRewardExp;
                m_pClientList[iAttackerH]->m_iWarContribution += (iRewardExp) * 5;

                if (m_pClientList[iAttackerH]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                    m_pClientList[iAttackerH]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                m_pClientList[iAttackerH]->m_iConstructionPoint += m_pClientList[iClientH]->m_iLevel / 2;

                if (m_pClientList[iAttackerH]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                    m_pClientList[iAttackerH]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                //testcode
                wsprintf(G_cTxt, "Enemy Player Killed by Player! Construction: +%d WarContribution +%d", m_pClientList[iClientH]->m_iLevel / 2, (iRewardExp - (iRewardExp / 3)) * 6);
                log->info(G_cTxt);


                SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iAttackerH]->m_iConstructionPoint, m_pClientList[iAttackerH]->m_iWarContribution, 0, 0);


                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {


                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0))
                    {

                        m_pClientList[iAttackerH]->m_iEnemyKillCount++;
                    }
                }

                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
            else
            {

                m_pClientList[iAttackerH]->m_iExp += iRewardExp;

                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {


                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0))
                    {


                        m_pClientList[iAttackerH]->m_iEnemyKillCount++;
                    }
                }

                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
        }


        SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_ENEMYKILLREWARD, iClientH, 0, 0, 0);

        if (bCheckLimitedUser(iAttackerH) == false)
        {

            SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
        }

        bCheckLevelUp(iAttackerH);

        //v1.4 ±³Àü¿¡¼­ ÀÌ±ä Ä«¿îÆ®¸¦ ¿Ã¸°´Ù.
        m_stCityStatus[m_pClientList[iAttackerH]->m_cSide].iWins++;
    }
}

void CGame::ItemDepleteHandler(int iClientH, short sItemIndex, bool bIsUseItemResult, bool bIsLog)
{


    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;


    if ((bIsLog == true) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_CONSUME)
        && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EAT)
        && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_USE_DEPLETE)
        && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_USE_DEPLETE_DEST)
        && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_MATERIAL)
        && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum != 380)
        && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum != 381)
        && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum != 382)
        )
        _bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

    ReleaseItemHandler(iClientH, sItemIndex, true);


    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMDEPLETED_ERASEITEM, sItemIndex, (int)bIsUseItemResult, 0, 0);


    delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
    m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;

    m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;

    m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);


    iCalcTotalWeight(iClientH);
}


void CGame::UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID)
{
    int iTemp, iMax, iV1, iV2, iV3, iSEV1, iEffectResult = 0;
    uint32_t dwTime;
    short sTemp, sTmpType, sTmpAppr1;

    dwTime = timeGetTime();

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;

    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_PERM) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_EAT) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_SKILL) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST))
    {
    }
    else return;

    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_EAT))
    {


        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
        {
            case DEF_ITEMEFFECTTYPE_WARM:


                if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 1)
                {
                    //	SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

                    bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_ICE);


                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (1 * 1000),
                        iClientH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);


                    //				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_ICE, 0, 0, 0);
                }

                m_pClientList[iClientH]->m_dwWarmEffectTime = dwTime;
                break;

            case DEF_ITEMEFFECTTYPE_LOTTERY:

                iTemp = iDice(1, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1);
                if (iTemp == iDice(1, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1))
                {
                }
                else
                {
                }
                break;

            case DEF_ITEMEFFECTTYPE_HP:
                iMax = iGetMaxHP(iClientH);
                if (m_pClientList[iClientH]->m_iHP < iMax)
                {

                    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0)
                    {
                        iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                        iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                        iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
                    }
                    else
                    {
                        iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
                        iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
                        iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
                    }

                    m_pClientList[iClientH]->m_iHP += (iDice(iV1, iV2) + iV3);
                    if (m_pClientList[iClientH]->m_iHP > iMax) m_pClientList[iClientH]->m_iHP = iMax;
                    if (m_pClientList[iClientH]->m_iHP <= 0)   m_pClientList[iClientH]->m_iHP = 1;

                    iEffectResult = 1;
                }
                break;

            case DEF_ITEMEFFECTTYPE_MP:
                iMax = (2 * m_pClientList[iClientH]->m_iMag) + (2 * m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iInt / 2);

                if (m_pClientList[iClientH]->m_iMP < iMax)
                {

                    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0)
                    {
                        iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                        iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                        iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
                    }
                    else
                    {
                        iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
                        iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
                        iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
                    }

                    m_pClientList[iClientH]->m_iMP += (iDice(iV1, iV2) + iV3);
                    if (m_pClientList[iClientH]->m_iMP > iMax)
                        m_pClientList[iClientH]->m_iMP = iMax;

                    iEffectResult = 2;
                }
                break;

            case DEF_ITEMEFFECTTYPE_SP:
                iMax = (2 * m_pClientList[iClientH]->m_iStr) + (2 * m_pClientList[iClientH]->m_iLevel);

                if (m_pClientList[iClientH]->m_iSP < iMax)
                {

                    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0)
                    {
                        iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                        iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                        iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
                    }
                    else
                    {
                        iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
                        iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
                        iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
                    }

                    m_pClientList[iClientH]->m_iSP += (iDice(iV1, iV2) + iV3);
                    if (m_pClientList[iClientH]->m_iSP > iMax)
                        m_pClientList[iClientH]->m_iSP = iMax;

                    iEffectResult = 3;
                }

                if (m_pClientList[iClientH]->m_bIsPoisoned == true)
                {

                    m_pClientList[iClientH]->m_bIsPoisoned = false;
                    SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);
                }
                break;

            case DEF_ITEMEFFECTTYPE_HPSTOCK:
                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;

                m_pClientList[iClientH]->m_iHPstock += iDice(iV1, iV2) + iV3;
                if (m_pClientList[iClientH]->m_iHPstock < 0)   m_pClientList[iClientH]->m_iHPstock = 0;
                if (m_pClientList[iClientH]->m_iHPstock > 500) m_pClientList[iClientH]->m_iHPstock = 500;


                m_pClientList[iClientH]->m_iHungerStatus += iDice(iV1, iV2) + iV3;
                if (m_pClientList[iClientH]->m_iHungerStatus > 100) m_pClientList[iClientH]->m_iHungerStatus = 100;
                if (m_pClientList[iClientH]->m_iHungerStatus < 0)   m_pClientList[iClientH]->m_iHungerStatus = 0;
                break;

            case DEF_ITEMEFFECTTYPE_STUDYSKILL:

                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                iSEV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;

                if (iSEV1 == 0)
                {

                    TrainSkillResponse(true, iClientH, iV1, iV2);
                }
                else
                {
                    TrainSkillResponse(true, iClientH, iV1, iSEV1);
                }
                break;

            case DEF_ITEMEFFECTTYPE_STUDYMAGIC:

                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                if (m_pMagicConfigList[iV1] != 0)
                    RequestStudyMagicHandler(iClientH, m_pMagicConfigList[iV1]->m_cName, false);
                break;

            case DEF_ITEMEFFECTTYPE_MAGIC:

                if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
                {
                    SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

                    bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
                    m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                }

                switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1)
                {
                    case 1:
                        RequestTeleportHandler(iClientH, "1   ");
                        break;

                    case 2:

                        PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 32, true);
                        break;

                    case 3:

                        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == false)
                            PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 34, true);
                        break;

                    case 4:

                        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2)
                        {
                            case 1:

                                if (memcmp(m_pClientList[iClientH]->m_cMapName, "bisle", 5) != 0)
                                {
                                    //v1.42
                                    ItemDepleteHandler(iClientH, sItemIndex, true);

                                    RequestTeleportHandler(iClientH, "3   ", "bisle", -1, -1);

                                }
                                break;

                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                            case 17:
                            case 18:
                            case 19:

                                SYSTEMTIME SysTime;

                                GetLocalTime(&SysTime);


                                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue1 != SysTime.wMonth) |
                                    (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue2 != SysTime.wDay) ||
                                    (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue3 <= SysTime.wHour))
                                {

                                }
                                else
                                {
                                    char cDestMapName[11];
                                    memset(cDestMapName, 0, sizeof(cDestMapName));
                                    wsprintf(cDestMapName, "fightzone%d", m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 - 10);
                                    if (memcmp(m_pClientList[iClientH]->m_cMapName, cDestMapName, 10) != 0)
                                    {
                                        //v1.42
                                        ItemDepleteHandler(iClientH, sItemIndex, true);

                                        RequestTeleportHandler(iClientH, "3   ", cDestMapName, -1, -1);
                                    }
                                }
                                break;
                        }
                        break;

                    case 5:

                        PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, true,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
                        break;
                }
                break;

            case DEF_ITEMEFFECTTYPE_FIRMSTAMINAR:
                m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar > 20 * 30) m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar = 20 * 30;
                break;

            case DEF_ITEMEFFECTTYPE_CHANGEATTR:
                switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1)
                {
                    case 1:

                        m_pClientList[iClientH]->m_cHairColor++;
                        if (m_pClientList[iClientH]->m_cHairColor > 15) m_pClientList[iClientH]->m_cHairColor = 0;

                        sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
                        m_pClientList[iClientH]->m_sAppr1 = sTemp;
                        break;

                    case 2:

                        m_pClientList[iClientH]->m_cHairStyle++;
                        if (m_pClientList[iClientH]->m_cHairStyle > 7) m_pClientList[iClientH]->m_cHairStyle = 0;

                        sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
                        m_pClientList[iClientH]->m_sAppr1 = sTemp;
                        break;

                    case 3:


                        m_pClientList[iClientH]->m_cSkin++;
                        if (m_pClientList[iClientH]->m_cSkin > 3)
                            m_pClientList[iClientH]->m_cSkin = 1;

                        if (m_pClientList[iClientH]->m_cSex == 1)      sTemp = 1;
                        else if (m_pClientList[iClientH]->m_cSex == 2) sTemp = 4;

                        switch (m_pClientList[iClientH]->m_cSkin)
                        {
                            case 2:	sTemp += 1; break;
                            case 3:	sTemp += 2; break;
                        }
                        m_pClientList[iClientH]->m_sType = sTemp;
                        break;

                    case 4:

                        sTemp = m_pClientList[iClientH]->m_sAppr3 & 0xFF0F;
                        if (sTemp == 0)
                        {

                            if (m_pClientList[iClientH]->m_cSex == 1)
                                m_pClientList[iClientH]->m_cSex = 2;
                            else m_pClientList[iClientH]->m_cSex = 1;


                            if (m_pClientList[iClientH]->m_cSex == 1)
                            {

                                sTmpType = 1;
                            }
                            else if (m_pClientList[iClientH]->m_cSex == 2)
                            {

                                sTmpType = 4;
                            }

                            switch (m_pClientList[iClientH]->m_cSkin)
                            {
                                case 1:

                                    break;
                                case 2:
                                    sTmpType += 1;
                                    break;
                                case 3:
                                    sTmpType += 2;
                                    break;
                            }

                            sTmpAppr1 = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
                            m_pClientList[iClientH]->m_sType = sTmpType;
                            m_pClientList[iClientH]->m_sAppr1 = sTmpAppr1;
                            //
                        }
                        break;

                    case 5:

                        m_pClientList[iClientH]->m_cUnderwear++;
                        if (m_pClientList[iClientH]->m_cUnderwear > 7) m_pClientList[iClientH]->m_cUnderwear = 0;

                        sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
                        m_pClientList[iClientH]->m_sAppr1 = sTemp;
                        break;
                }

                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                break;
        }


        ItemDepleteHandler(iClientH, sItemIndex, true);

        switch (iEffectResult)
        {
            case 1:
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                break;
            case 2:
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                break;
            case 3:
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                break;
            default:
                break;
        }
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST)
    {


        if (_bDepleteDestTypeItemUseEffect(iClientH, dX, dY, sItemIndex, sDestItemID) == true)
            ItemDepleteHandler(iClientH, sItemIndex, true);
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)
    {

        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_PERM)
    {

        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
        {
            case DEF_ITEMEFFECTTYPE_SHOWLOCATION:
                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                switch (iV1)
                {
                    case 1:

                        if (strcmp(m_pClientList[iClientH]->m_cMapName, "aresden") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 1, 0, 0);
                        else
                            if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvine") == 0)
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 2, 0, 0);
                            else
                                if (strcmp(m_pClientList[iClientH]->m_cMapName, "middleland") == 0)
                                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 3, 0, 0);
                                else
                                    if (strcmp(m_pClientList[iClientH]->m_cMapName, "default") == 0)
                                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 4, 0, 0);
                                    else
                                        if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone2") == 0)
                                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 5, 0, 0);
                                        else
                                            if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone1") == 0)
                                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 6, 0, 0);
                                            else
                                                if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone4") == 0)
                                                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 7, 0, 0);
                                                else
                                                    if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone3") == 0)
                                                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 8, 0, 0);
                                                    else
                                                        if (strcmp(m_pClientList[iClientH]->m_cMapName, "arefarm") == 0)
                                                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 9, 0, 0);
                                                        else
                                                            if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvfarm") == 0)
                                                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 10, 0, 0);
                                                            else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 0, 0, 0);
                        break;
                        break;
                }
                break;
        }
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_SKILL)
    {


        if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) ||
            (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) ||
            (m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] == true))
        {

            return;
        }
        else
        {
            if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan != 0)
            {

                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan--;
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0)
                {


                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMLIFESPANEND, DEF_EQUIPPOS_NONE, sItemIndex, 0, 0);
                }
                else
                {

                    int iSkillUsingTimeID = (int)timeGetTime();

                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_USEITEM_SKILL, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill,
                        dwTime + m_pSkillConfigList[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill]->m_sValue2 * 1000,
                        iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_cMapIndex, dX, dY,
                        m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill], iSkillUsingTimeID, 0);

                    // ±â¼ú »ç¿ëÁß 
                    m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = true;
                    m_pClientList[iClientH]->m_iSkillUsingTimeID[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = iSkillUsingTimeID; //v1.12
                }
            }
        }
    }
}

void CGame::NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage)
{
    short  sAttackerWeapon;
    int * ip, i, iQuestIndex, iExp, iConstructionPoint, iWarContribution;
    double dTmp1, dTmp2, dTmp3;
    char * cp, cData[120];

    if (m_pNpcList[iNpcH] == 0) return;
    if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;

    m_pNpcList[iNpcH]->m_bIsKilled = true;
    m_pNpcList[iNpcH]->m_iHP = 0;
    m_pNpcList[iNpcH]->m_iLastDamage = sDamage;


    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;


    RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);


    ReleaseFollowMode(iNpcH, DEF_OWNERTYPE_NPC);


    m_pNpcList[iNpcH]->m_iTargetIndex = 0;
    m_pNpcList[iNpcH]->m_cTargetType = 0;

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
    }
    else sAttackerWeapon = 1;

    SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, 0);

    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(10, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

    m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_DEAD;


    m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;

    m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();


    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) return;


    NpcDeadItemGenerator(iNpcH, sAttackerH, cAttackerType);






    if ((m_pNpcList[iNpcH]->m_bIsSummoned != true) && (cAttackerType == DEF_OWNERTYPE_PLAYER) &&
        (m_pClientList[sAttackerH] != 0))
    {


        iExp = (m_pNpcList[iNpcH]->m_iExp / 3);
        if (m_pNpcList[iNpcH]->m_iNoDieRemainExp > 0)
            iExp += m_pNpcList[iNpcH]->m_iNoDieRemainExp;

        //v1.42 ?æÇèÄ? Áõ?? 
        if (m_pClientList[sAttackerH]->m_iAddExp != 0)
        {
            dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
            dTmp2 = (double)iExp;
            dTmp3 = (dTmp1 / 100.0f) * dTmp2;
            iExp += (int)dTmp3;
        }



        if (m_bIsCrusadeMode == true)
        {
            if ((strcmp(m_pClientList[sAttackerH]->m_cMapName, "default") != 0)
                || (strcmp(m_pClientList[sAttackerH]->m_cMapName, "arefarm") != 0)
                || (strcmp(m_pClientList[sAttackerH]->m_cMapName, "elvfarm") != 0))
                if (iExp > 10) iExp = iExp / 4;
        }

        //m_pClientList[sAttackerH]->m_iExpStock += iExp;

        if (m_pClientList[sAttackerH]->m_iLevel > 100)
        {
            switch (m_pNpcList[iNpcH]->m_sType)
            {
                case 55:
                case 56:
                    iExp = 0;
                    break;
                default: break;
            }
        }

        GetExp(sAttackerH, iExp);


        iQuestIndex = m_pClientList[sAttackerH]->m_iQuest;
        if (iQuestIndex != 0)
        {
            if (m_pQuestConfigList[iQuestIndex] != 0)
            {
                switch (m_pQuestConfigList[iQuestIndex]->m_iType)
                {
                    case DEF_QUESTTYPE_MONSTERHUNT:
                        if ((m_pClientList[sAttackerH]->m_bQuestMatchFlag_Loc == true) &&
                            (m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_pNpcList[iNpcH]->m_sType))
                        {

                            m_pClientList[sAttackerH]->m_iCurQuestCount++;
                            _bCheckIsQuestCompleted(sAttackerH);
                        }
                        break;
                }
            }
        }
    }


    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        switch (m_pNpcList[iNpcH]->m_sType)
        {
            case 32:

                m_pClientList[sAttackerH]->m_iRating -= 5;
                if (m_pClientList[sAttackerH]->m_iRating < -10000) m_pClientList[sAttackerH]->m_iRating = 0;
                if (m_pClientList[sAttackerH]->m_iRating > 10000) m_pClientList[sAttackerH]->m_iRating = 0;
                break;

            case 33:

                break;
        }
    }

    iConstructionPoint = 0;
    switch (m_pNpcList[iNpcH]->m_sType)
    {

        case 1:  iConstructionPoint = 50; iWarContribution = 100; break;
        case 2:  iConstructionPoint = 50; iWarContribution = 100; break;
        case 3:  iConstructionPoint = 50; iWarContribution = 100; break;
        case 4:  iConstructionPoint = 50; iWarContribution = 100; break;
        case 5:  iConstructionPoint = 50; iWarContribution = 100; break;
        case 6:  iConstructionPoint = 50; iWarContribution = 100; break;
        case 36: iConstructionPoint = 700; iWarContribution = 4000; break;
        case 37: iConstructionPoint = 700; iWarContribution = 4000; break;
        case 38: iConstructionPoint = 500; iWarContribution = 2000; break;
        case 39: iConstructionPoint = 500; iWarContribution = 2000; break;
        case 40: iConstructionPoint = 1500; iWarContribution = 5000; break;
        case 41: iConstructionPoint = 5000; iWarContribution = 10000; break;
        case 43: iConstructionPoint = 500; iWarContribution = 1000; break;
        case 44: iConstructionPoint = 1000; iWarContribution = 2000; break;
        case 45: iConstructionPoint = 1500; iWarContribution = 3000; break;
        case 46: iConstructionPoint = 1000; iWarContribution = 2000; break;
        case 47: iConstructionPoint = 1500; iWarContribution = 3000; break;
        case 51: iConstructionPoint = 800; iWarContribution = 1500; break;

        case 64:
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum();
            break;
    }

    if (iConstructionPoint != 0)
    {
        switch (cAttackerType)
        {
            case DEF_OWNERTYPE_PLAYER:
                if (m_pClientList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide)
                {

                    m_pClientList[sAttackerH]->m_iConstructionPoint += iConstructionPoint;

                    if (m_pClientList[sAttackerH]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                        m_pClientList[sAttackerH]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                    m_pClientList[sAttackerH]->m_iWarContribution += iWarContribution;
                    if (m_pClientList[sAttackerH]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                        m_pClientList[sAttackerH]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                    wsprintf(G_cTxt, "Enemy Npc Killed by player! Construction: +%d WarContribution: +%d", iConstructionPoint, iWarContribution);
                    log->info(G_cTxt);

                    SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, 0, 0);
                }
                else
                {

                    /*
                    m_pClientList[sAttackerH]->m_iWarContribution   -= (iWarContribution*2);
                    if (m_pClientList[sAttackerH]->m_iWarContribution < 0)
                    m_pClientList[sAttackerH]->m_iWarContribution = 0;

                    //testcode
                    wsprintf(G_cTxt, "Friendly Npc Killed by player! WarContribution: -%d", iWarContribution);
                    log->info(G_cTxt);
                    */


                    m_pClientList[sAttackerH]->m_iWarContribution = 0;

                    wsprintf(G_cTxt, "WarContribution: Friendly Npc Killed by player(%s)! ", m_pClientList[sAttackerH]->m_cAccountName);
                    log->info(G_cTxt);

                    SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, 0, 0);
                }
                break;

            case DEF_OWNERTYPE_NPC:
                if (m_pNpcList[sAttackerH]->m_iGuildGUID != 0)
                {
                    if (m_pNpcList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide)
                    {


                        for (i = 1; i < DEF_MAXCLIENTS; i++)
                            if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[sAttackerH]->m_iGuildGUID) &&
                                (m_pClientList[i]->m_iCrusadeDuty == 3))
                            {

                                m_pClientList[i]->m_iConstructionPoint += iConstructionPoint;
                                if (m_pClientList[i]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                                    m_pClientList[i]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                                //testcode
                                wsprintf(G_cTxt, "Enemy Npc Killed by Npc! Construct point +%d", iConstructionPoint);
                                log->info(G_cTxt);

                                SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, 0, 0);
                                goto NKH_GOTOPOINT1;
                            }
                    }
                }
                break;
        }
    }

    NKH_GOTOPOINT1:;

    //if ((m_pNpcList[iNpcH]->m_bIsSummoned != true) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && 
    //	(m_pClientList[sAttackerH] != 0))
    //{
    //	m_pClientList[sAttackerH]->m_iEnemyKillCount++;
    //	if (m_pClientList[sAttackerH]->m_iEnemyKillCount >  10000) m_pClientList[sAttackerH]->m_iEnemyKillCount = 10000;
    //}

    if (m_pNpcList[iNpcH]->m_cSpecialAbility == 7)
    {
        m_pNpcList[iNpcH]->m_iMana = 100;
        m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
        NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 30);
    }
    else if (m_pNpcList[iNpcH]->m_cSpecialAbility == 8)
    {
        m_pNpcList[iNpcH]->m_iMana = 100;
        m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
        NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 61);
    }
}
