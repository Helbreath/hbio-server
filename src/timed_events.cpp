//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"

extern char G_cTxt[512];

bool CGame::bRegisterDelayEvent(int iDelayType, int iEffectType, uint32_t dwLastTime, int iTargetH, char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3)
{
    int i;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] == 0)
        {

            m_pDelayEventList[i] = new CDelayEvent;

            m_pDelayEventList[i]->m_iDelayType = iDelayType;
            m_pDelayEventList[i]->m_iEffectType = iEffectType;

            m_pDelayEventList[i]->m_cMapIndex = cMapIndex;
            m_pDelayEventList[i]->m_dX = dX;
            m_pDelayEventList[i]->m_dY = dY;

            m_pDelayEventList[i]->m_iTargetH = iTargetH;
            m_pDelayEventList[i]->m_cTargetType = cTargetType;
            m_pDelayEventList[i]->m_iV1 = iV1;
            m_pDelayEventList[i]->m_iV2 = iV2;
            m_pDelayEventList[i]->m_iV3 = iV3;

            m_pDelayEventList[i]->m_dwTriggerTime = dwLastTime;

            return true;
        }

    return false;
}

void CGame::DelayEventProcessor()
{
    int i, iSkillNum, iResult;
    uint32_t dwTime = timeGetTime();



    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if ((m_pDelayEventList[i] != 0) && (m_pDelayEventList[i]->m_dwTriggerTime < dwTime))
        {


            switch (m_pDelayEventList[i]->m_iDelayType)
            {
                case DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT:
                    CalcMeteorStrikeEffectHandler(m_pDelayEventList[i]->m_cMapIndex);
                    break;

                case DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE:
                    DoMeteorStrikeDamageHandler(m_pDelayEventList[i]->m_cMapIndex);
                    break;

                case DEF_DELAYEVENTTYPE_METEORSTRIKE:
                    MeteorStrikeHandler(m_pDelayEventList[i]->m_cMapIndex);
                    break;

                case DEF_DELAYEVENTTYPE_USEITEM_SKILL:

                    switch (m_pDelayEventList[i]->m_cTargetType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            iSkillNum = m_pDelayEventList[i]->m_iEffectType;

                            if (m_pClientList[m_pDelayEventList[i]->m_iTargetH] == 0) break;

                            if (m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[iSkillNum] == false) break;

                            if (m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[iSkillNum] != m_pDelayEventList[i]->m_iV2) break;


                            m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[iSkillNum] = false;
                            m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[iSkillNum] = 0;


                            iResult = iCalculateUseSkillItemEffect(m_pDelayEventList[i]->m_iTargetH, m_pDelayEventList[i]->m_cTargetType,
                                m_pDelayEventList[i]->m_iV1, iSkillNum, m_pDelayEventList[i]->m_cMapIndex, m_pDelayEventList[i]->m_dX, m_pDelayEventList[i]->m_dY);


                            SendNotifyMsg(0, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_SKILLUSINGEND, iResult, 0, 0, 0);
                            break;
                    }
                    break;

                case DEF_DELAYEVENTTYPE_DAMAGEOBJECT:
                    break;

                case DEF_DELAYEVENTTYPE_MAGICRELEASE:

                    switch (m_pDelayEventList[i]->m_cTargetType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[m_pDelayEventList[i]->m_iTargetH] == 0) break;

                            SendNotifyMsg(0, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_MAGICEFFECTOFF,
                                m_pDelayEventList[i]->m_iEffectType, m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_cMagicEffectStatus[m_pDelayEventList[i]->m_iEffectType], 0, 0);

                            m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_cMagicEffectStatus[m_pDelayEventList[i]->m_iEffectType] = 0;


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INVISIBILITY)
                                SetInvisibilityFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_BERSERK)
                                SetBerserkFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_POLYMORPH)
                            {
                                m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sType = m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sOriginalType;
                                SendEventToNearClient_TypeA(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                            }


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_ICE)
                                SetIceFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);



                            // Inbitition casting 
                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INHIBITION)
                                m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bInhibition = false;


                            // Confusion
                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_CONFUSE)
                                switch (m_pDelayEventList[i]->m_iV1)
                                {
                                    case 3: SetIllusionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false); break;
                                    case 4: SetIllusionMovementFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false); break;
                                }

                            // Protection Magic
                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_PROTECT)
                            {
                                switch (m_pDelayEventList[i]->m_iV1)
                                {
                                    case 1:
                                        SetProtectionFromArrowFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);
                                        break;
                                    case 2:
                                    case 5:
                                        SetMagicProtectionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);
                                        break;
                                    case 3:
                                    case 4:
                                        SetDefenseShieldFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);
                                        break;
                                }
                            }

                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[m_pDelayEventList[i]->m_iTargetH] == 0) break;

                            m_pNpcList[m_pDelayEventList[i]->m_iTargetH]->m_cMagicEffectStatus[m_pDelayEventList[i]->m_iEffectType] = 0;


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INVISIBILITY)
                                SetInvisibilityFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_BERSERK)
                                SetBerserkFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_POLYMORPH)
                            {
                                m_pNpcList[m_pDelayEventList[i]->m_iTargetH]->m_sType = m_pNpcList[m_pDelayEventList[i]->m_iTargetH]->m_sOriginalType;
                                SendEventToNearClient_TypeA(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                            }


                            if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_ICE)
                                SetIceFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);
                            break;
                    }
                    break;
            }

            delete m_pDelayEventList[i];
            m_pDelayEventList[i] = 0;
        }
}



bool CGame::bRemoveFromDelayEventList(int iH, char cType, int iEffectType)
{
    int i;

    for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
        if (m_pDelayEventList[i] != 0)
        {

            if (iEffectType == 0)
            {

                if ((m_pDelayEventList[i]->m_iTargetH == iH) && (m_pDelayEventList[i]->m_cTargetType == cType))
                {
                    delete m_pDelayEventList[i];
                    m_pDelayEventList[i] = 0;
                }
            }
            else
            {

                if ((m_pDelayEventList[i]->m_iTargetH == iH) && (m_pDelayEventList[i]->m_cTargetType == cType) &&
                    (m_pDelayEventList[i]->m_iEffectType == iEffectType))
                {
                    delete m_pDelayEventList[i];
                    m_pDelayEventList[i] = 0;
                }
            }
        }

    return true;
}

void CGame::DelayEventProcess()
{

}

void CGame::TimeHitPointsUp(int iClientH)
{
    int iMaxHP, iTemp, iTotal;
    double dV1, dV2, dV3;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;

    //iMaxHP = (3*m_pClientList[iClientH]->m_iVit) + (2*m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iStr/2); // V1.4
    iMaxHP = iGetMaxHP(iClientH);

    if (m_pClientList[iClientH]->m_iHP < iMaxHP)
    {


        iTemp = iDice(1, (m_pClientList[iClientH]->m_iVit));
        if (iTemp < (m_pClientList[iClientH]->m_iVit / 2)) iTemp = (m_pClientList[iClientH]->m_iVit / 2);


        if (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown != 0)
            iTemp = iTemp - (iTemp / m_pClientList[iClientH]->m_iSideEffect_MaxHPdown);

        iTotal = iTemp + m_pClientList[iClientH]->m_iHPstock;

        // v2.20 2002-12-28 3ÁÖ³â ±â³ä¹ÝÁö ¹ö±× ¼öÁ¤ 
        iTotal += m_pClientList[iClientH]->m_iHPStatic_stock;

        if (m_pClientList[iClientH]->m_iAddHP != 0)
        {
            dV2 = (double)iTotal;
            dV3 = (double)m_pClientList[iClientH]->m_iAddHP;
            dV1 = (dV3 / 100.0f) * dV2;
            iTotal += (int)dV1;
        }

        m_pClientList[iClientH]->m_iHP += iTotal;
        if (m_pClientList[iClientH]->m_iHP > iMaxHP) m_pClientList[iClientH]->m_iHP = iMaxHP;
        if (m_pClientList[iClientH]->m_iHP <= 0)     m_pClientList[iClientH]->m_iHP = 0;

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
    }

    m_pClientList[iClientH]->m_iHPstock = 0;
}

void CGame::TimeManaPointsUp(int iClientH)
{
    int iMaxMP, iTotal;
    double dV1, dV2, dV3;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;

    iMaxMP = (2 * m_pClientList[iClientH]->m_iMag) + (2 * m_pClientList[iClientH]->m_iLevel) + (m_pClientList[iClientH]->m_iInt / 2); // v1.4
    if (m_pClientList[iClientH]->m_iMP < iMaxMP)
    {

        iTotal = iDice(1, (m_pClientList[iClientH]->m_iMag));
        if (m_pClientList[iClientH]->m_iAddMP != 0)
        {
            dV2 = (double)iTotal;
            dV3 = (double)m_pClientList[iClientH]->m_iAddMP;
            dV1 = (dV3 / 100.0f) * dV2;
            iTotal += (int)dV1;
        }

        m_pClientList[iClientH]->m_iMP += iTotal;

        if (m_pClientList[iClientH]->m_iMP > iMaxMP)
            m_pClientList[iClientH]->m_iMP = iMaxMP;

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
    }
}


void CGame::TimeStaminarPointsUp(int iClientH)
{
    int iMaxSP, iTotal;
    double dV1, dV2, dV3;

    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_bIsKilled == true) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
    if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;

    iMaxSP = (2 * m_pClientList[iClientH]->m_iStr) + (2 * m_pClientList[iClientH]->m_iLevel);
    if (m_pClientList[iClientH]->m_iSP < iMaxSP)
    {

        iTotal = iDice(1, (m_pClientList[iClientH]->m_iVit / 3));
        if (m_pClientList[iClientH]->m_iAddSP != 0)
        {
            dV2 = (double)iTotal;
            dV3 = (double)m_pClientList[iClientH]->m_iAddSP;
            dV1 = (dV3 / 100.0f) * dV2;
            iTotal += (int)dV1;
        }


        if (m_pClientList[iClientH]->m_iLevel <= 20)
        {
            iTotal += 15;
        }
        else if (m_pClientList[iClientH]->m_iLevel <= 40)
        {
            iTotal += 10;
        }
        else if (m_pClientList[iClientH]->m_iLevel <= 60)
        {
            iTotal += 5;
        }

        m_pClientList[iClientH]->m_iSP += iTotal;
        if (m_pClientList[iClientH]->m_iSP > iMaxSP)
            m_pClientList[iClientH]->m_iSP = iMaxSP;

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
    }
}
