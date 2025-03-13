//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "map.h"

extern char G_cTxt[512];


bool CGame::_bNpcBehavior_ManaCollector(int iNpcH)
{
    int dX, dY, iMaxMP, iTotal;
    short sOwnerH;
    char  cOwnerType;
    double dV1, dV2, dV3;
    bool bRet;

    if (m_pNpcList[iNpcH] == 0) return false;
    if (m_pNpcList[iNpcH]->m_sAppr2 != 0) return false;

    bRet = false;

    for (dX = m_pNpcList[iNpcH]->m_sX - 5; dX <= m_pNpcList[iNpcH]->m_sX + 5; dX++)
        for (dY = m_pNpcList[iNpcH]->m_sY - 5; dY <= m_pNpcList[iNpcH]->m_sY + 5; dY++)
        {
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

            if (sOwnerH != 0)
            {
                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:

                        if (m_pNpcList[iNpcH]->m_cSide == m_pClientList[sOwnerH]->m_cSide)
                        {
                            iMaxMP = (2 * m_pClientList[sOwnerH]->m_iMag) + (2 * m_pClientList[sOwnerH]->m_iLevel) + (m_pClientList[sOwnerH]->m_iInt / 2); // v1.4
                            if (m_pClientList[sOwnerH]->m_iMP < iMaxMP)
                            {
                                iTotal = iDice(1, (m_pClientList[sOwnerH]->m_iMag));
                                if (m_pClientList[sOwnerH]->m_iAddMP != 0)
                                {
                                    dV2 = (double)iTotal;
                                    dV3 = (double)m_pClientList[sOwnerH]->m_iAddMP;
                                    dV1 = (dV3 / 100.0f) * dV2;
                                    iTotal += (int)dV1;
                                }

                                m_pClientList[sOwnerH]->m_iMP += iTotal;

                                if (m_pClientList[sOwnerH]->m_iMP > iMaxMP)
                                    m_pClientList[sOwnerH]->m_iMP = iMaxMP;

                                SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_MP, 0, 0, 0, 0);
                            }
                        }
                        break;

                    case DEF_OWNERTYPE_NPC:

                        if (m_pNpcList[sOwnerH]->m_sType == 42)
                        {

                            if (m_pNpcList[sOwnerH]->m_iV1 > 0)
                            {

                                if (m_pNpcList[sOwnerH]->m_iV1 >= 3)
                                {
                                    m_iCollectedMana[m_pNpcList[iNpcH]->m_cSide] += 3;
                                    m_pNpcList[sOwnerH]->m_iV1 -= 3;
                                    bRet = true;
                                }
                                else
                                {
                                    m_iCollectedMana[m_pNpcList[iNpcH]->m_cSide] += m_pNpcList[sOwnerH]->m_iV1;
                                    m_pNpcList[sOwnerH]->m_iV1 = 0;
                                    bRet = true;
                                }
                            }
                            else bRet = false;
                            //testcode
                            //wsprintf(G_cTxt, "Side: %d  Mana: %d", m_pNpcList[iNpcH]->m_cSide, m_iCollectedMana[m_pNpcList[iNpcH]->m_cSide]);
                            //log->info(G_cTxt);
                        }
                        break;
                }
            }
        }

    return bRet;
}


bool CGame::_bNpcBehavior_Detector(int iNpcH)
{
    int dX, dY;
    short sOwnerH;
    char  cOwnerType, cSide;
    ;
    bool  bFlag = false;

    if (m_pNpcList[iNpcH] == 0) return false;
    if (m_pNpcList[iNpcH]->m_sAppr2 != 0) return false;


    for (dX = m_pNpcList[iNpcH]->m_sX - 10; dX <= m_pNpcList[iNpcH]->m_sX + 10; dX++)
        for (dY = m_pNpcList[iNpcH]->m_sY - 10; dY <= m_pNpcList[iNpcH]->m_sY + 10; dY++)
        {
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

            cSide = 0;
            if (sOwnerH != 0)
            {
                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        cSide = m_pClientList[sOwnerH]->m_cSide;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        cSide = m_pNpcList[sOwnerH]->m_cSide;
                        break;
                }
            }

            if ((cSide != 0) && (cSide != m_pNpcList[iNpcH]->m_cSide))
            {

                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0)
                        {
                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                            SetInvisibilityFlag(sOwnerH, cOwnerType, false);
                        }
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0)
                        {
                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
                            SetInvisibilityFlag(sOwnerH, cOwnerType, false);
                        }
                        break;
                }

                bFlag = true;
            }
        }

    return bFlag;
}


void CGame::_NpcBehavior_GrandMagicGenerator(int iNpcH)
{
    switch (m_pNpcList[iNpcH]->m_cSide)
    {
        case 1:
            if (m_iAresdenMana > DEF_GMGMANACONSUMEUNIT)
            {

                m_iAresdenMana = 0;
                m_pNpcList[iNpcH]->m_iManaStock++;

                if (m_pNpcList[iNpcH]->m_iManaStock > m_pNpcList[iNpcH]->m_iMaxMana)
                {
                    MeteorStrikeMsgHandler(1);

                    m_pNpcList[iNpcH]->m_iManaStock = 0;
                    m_iAresdenMana = 0;
                }
                //testcode
                wsprintf(G_cTxt, "(!) Aresden GMG %d/%d", m_pNpcList[iNpcH]->m_iManaStock, m_pNpcList[iNpcH]->m_iMaxMana);
                log->info(G_cTxt);
            }
            break;

        case 2:
            if (m_iElvineMana > DEF_GMGMANACONSUMEUNIT)
            {

                m_iElvineMana = 0;
                m_pNpcList[iNpcH]->m_iManaStock++;

                if (m_pNpcList[iNpcH]->m_iManaStock > m_pNpcList[iNpcH]->m_iMaxMana)
                {
                    MeteorStrikeMsgHandler(2);

                    m_pNpcList[iNpcH]->m_iManaStock = 0;
                    m_iElvineMana = 0;
                }
                //testcode
                wsprintf(G_cTxt, "(!) Elvine GMG %d/%d", m_pNpcList[iNpcH]->m_iManaStock, m_pNpcList[iNpcH]->m_iMaxMana);
                log->info(G_cTxt);
            }
            break;
    }
}

void CGame::NpcBehavior_Attack(int iNpcH)
{
    int   iMagicType;
    short sX, sY, dX, dY;
    char  cDir;
    uint32_t dwTime = timeGetTime();

    if (m_pNpcList[iNpcH] == 0) return;
    if (m_pNpcList[iNpcH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) return;
    if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;

    switch (m_pNpcList[iNpcH]->m_cActionLimit)
    {
        case 1:
        case 2:
        case 3:
        case 4:
            return;

        case 5:
            if (m_pNpcList[iNpcH]->m_iBuildCount > 0) return;
    }


    int iStX, iStY;
    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex] != 0)
    {
        iStX = m_pNpcList[iNpcH]->m_sX / 20;
        iStY = m_pNpcList[iNpcH]->m_sY / 20;
        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iMonsterActivity++;
    }


    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount == 0)
        m_pNpcList[iNpcH]->m_iAttackCount = 0;


    m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;
    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 20)
    {

        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;

        if ((m_pNpcList[iNpcH]->m_bIsPermAttackMode == false))
            m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;


        return;
    }


    sX = m_pNpcList[iNpcH]->m_sX;
    sY = m_pNpcList[iNpcH]->m_sY;
    dX = 0;
    dY = 0;

    switch (m_pNpcList[iNpcH]->m_cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0)
            {

                m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                return;
            }
            dX = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
            dY = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0)
            {

                m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                return;
            }
            dX = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
            dY = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
            break;
    }


    if ((iGetDangerValue(iNpcH, dX, dY) > m_pNpcList[iNpcH]->m_cBravery) &&
        (m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) &&
        (m_pNpcList[iNpcH]->m_cActionLimit != 5))
    {

        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
        m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
        return;
    }


    if ((m_pNpcList[iNpcH]->m_iHP <= 2) && (iDice(1, m_pNpcList[iNpcH]->m_cBravery) <= 3) &&
        (m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) &&
        (m_pNpcList[iNpcH]->m_cActionLimit != 5))
    {

        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
        m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
        return;
    }


    if ((abs(sX - dX) <= 1) && (abs(sY - dY) <= 1))
    {


        cDir = m_Misc.cGetNextMoveDir(sX, sY, dX, dY);
        if (cDir == 0) return;

        m_pNpcList[iNpcH]->m_cDir = cDir;

        if (m_pNpcList[iNpcH]->m_cActionLimit == 5)
        {

            switch (m_pNpcList[iNpcH]->m_sType)
            {
                case 36:
                    SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 2);
                    iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2);
                    break;

                case 37: // Cannon Guard Tower: 
                    SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 1);
                    m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
                    NpcMagicHandler(iNpcH, dX, dY, 61);
                    break;
            }
        }

        else
        {

            if (m_pNpcList[iNpcH]->m_cMagicLevel == 11)
            {// v2.20 2002-12-20 Ice-GolemÀÇ °ø°Ý ÇüÅÂ
                SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 1);
                m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
                NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 75);
            }
            else
            {
                SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 1);
                iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 1);
            }
        }

        m_pNpcList[iNpcH]->m_iAttackCount++;


        if ((m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) && (m_pNpcList[iNpcH]->m_cActionLimit == 0))
        {
            switch (m_pNpcList[iNpcH]->m_iAttackStrategy)
            {
                case DEF_ATTACKAI_EXCHANGEATTACK:

                    m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                    m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
                    break;

                case DEF_ATTACKAI_TWOBYONEATTACK:

                    if (m_pNpcList[iNpcH]->m_iAttackCount >= 2)
                    {
                        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                        m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
                    }
                    break;
            }
        }
    }
    else
    {

        cDir = m_Misc.cGetNextMoveDir(sX, sY, dX, dY);
        if (cDir == 0) return;
        m_pNpcList[iNpcH]->m_cDir = cDir;


        if ((m_pNpcList[iNpcH]->m_cMagicLevel > 0) && (iDice(1, 2) == 1) &&
            (abs(sX - dX) <= 9) && (abs(sY - dY) <= 7))
        {
            iMagicType = -1;
            switch (m_pNpcList[iNpcH]->m_cMagicLevel)
            {
                case 1:

                    if (m_pMagicConfigList[0]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 0;
                    break;
                case 2:

                    if (m_pMagicConfigList[10]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 10;
                    else if (m_pMagicConfigList[0]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 0;
                    break;
                case 3:

                    if (m_pMagicConfigList[20]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 20;
                    else if (m_pMagicConfigList[10]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 10;
                    break;
                case 4:

                    if (m_pMagicConfigList[30]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 30;
                    else if (m_pMagicConfigList[37]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 37;
                    else if (m_pMagicConfigList[20]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 20;
                    else if (m_pMagicConfigList[10]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 10;
                    break;
                case 5:

                    if (m_pMagicConfigList[43]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 43;
                    else if (m_pMagicConfigList[30]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 30;
                    else if (m_pMagicConfigList[37]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 37;
                    else if (m_pMagicConfigList[20]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 20;
                    else if (m_pMagicConfigList[10]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 10;
                    break;
                case 6:

                    if (m_pMagicConfigList[51]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 51;
                    else if (m_pMagicConfigList[43]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 43;
                    else if (m_pMagicConfigList[30]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 30;
                    else if (m_pMagicConfigList[37]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 37;
                    else if (m_pMagicConfigList[20]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 20;
                    else if (m_pMagicConfigList[10]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 10;
                    break;
                case 7:

                    if ((m_pMagicConfigList[70]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana) && (iDice(1, 5) == 3))
                        iMagicType = 70;
                    else if (m_pMagicConfigList[61]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 61;
                    else if (m_pMagicConfigList[60]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 60;
                    else if (m_pMagicConfigList[51]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 51;
                    else if (m_pMagicConfigList[43]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 43;
                    break;
                case 8:

                    if ((m_pMagicConfigList[35]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana) && (iDice(1, 3) == 2))
                        iMagicType = 35;
                    else if (m_pMagicConfigList[60]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 60;
                    else if (m_pMagicConfigList[51]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 51;
                    else if (m_pMagicConfigList[43]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                        iMagicType = 43;
                    break;

                case 9:
                    if ((m_pMagicConfigList[74]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana) && (iDice(1, 3) == 2))
                        iMagicType = 74;
                    break;
                case 10:
                    if ((m_pMagicConfigList[57]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana) && (iDice(1, 3) == 2))
                        iMagicType = 57;
                    break;
                case 11:
                    goto NBA_CHASE;
                    break;
            }

            if (iMagicType != -1)
            {


                if (m_pNpcList[iNpcH]->m_iAILevel >= 2)
                {
                    switch (m_pNpcList[iNpcH]->m_cTargetType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                            {

                                if ((abs(sX - dX) > m_pNpcList[iNpcH]->m_iAttackRange) || (abs(sY - dY) > m_pNpcList[iNpcH]->m_iAttackRange))
                                {
                                    m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                                    m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                                    return;
                                }
                                else goto NBA_CHASE;
                            }

                            if ((iMagicType == 35) && (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)) goto NBA_CHASE;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
                            {

                                if ((abs(sX - dX) > m_pNpcList[iNpcH]->m_iAttackRange) || (abs(sY - dY) > m_pNpcList[iNpcH]->m_iAttackRange))
                                {
                                    m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                                    m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
                                    return;
                                }
                                else goto NBA_CHASE;
                            }

                            if ((iMagicType == 35) && (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)) goto NBA_CHASE;
                            break;
                    }
                }

                SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 1);
                NpcMagicHandler(iNpcH, dX, dY, iMagicType);
                m_pNpcList[iNpcH]->m_dwTime = dwTime + 2000;
                return;
            }
        }


        if ((m_pNpcList[iNpcH]->m_cMagicLevel < 0) && (iDice(1, 2) == 1) &&
            (abs(sX - dX) <= 9) && (abs(sY - dY) <= 7))
        {
            iMagicType = -1;
            if (m_pMagicConfigList[43]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                iMagicType = 43;
            else if (m_pMagicConfigList[37]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                iMagicType = 37;
            else if (m_pMagicConfigList[0]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
                iMagicType = 0;

            if (iMagicType != -1)
            {
                SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 1);
                NpcMagicHandler(iNpcH, dX, dY, iMagicType);
                m_pNpcList[iNpcH]->m_dwTime = dwTime + 2000;
                return;
            }
        }


        if ((m_pNpcList[iNpcH]->m_iAttackRange > 1) &&
            (abs(sX - dX) <= m_pNpcList[iNpcH]->m_iAttackRange) && (abs(sY - dY) <= m_pNpcList[iNpcH]->m_iAttackRange))
        {

            cDir = m_Misc.cGetNextMoveDir(sX, sY, dX, dY);
            if (cDir == 0) return;

            m_pNpcList[iNpcH]->m_cDir = cDir;

            if (m_pNpcList[iNpcH]->m_cActionLimit == 5)
            {

                switch (m_pNpcList[iNpcH]->m_sType)
                {
                    case 36: // Crossbow Guard Tower
                        SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 2);
                        iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2);
                        break;

                    case 37:
                        SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 1);
                        m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
                        NpcMagicHandler(iNpcH, dX, dY, 61);
                        break;
                }
            }
            else
            {
                switch (m_pNpcList[iNpcH]->m_sType)
                {
                    case 51:
                        SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 1);
                        m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
                        NpcMagicHandler(iNpcH, dX, dY, 61);
                        break;

                    case 54:
                        SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 2);
                        iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2);
                        break;
                    case 63: //v2.20 2002-12-20 frost : ³Ãµ¿ °ø°Ý 
                    case 53: //Beholder: ³Ãµ¿ °ø°ÝÀ» ÇÑ´Ù.
                        switch (m_pNpcList[iNpcH]->m_cTargetType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0) goto NBA_BREAK1;

                                if ((m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_iHP > 0) &&
                                    (bCheckResistingIceSuccess(m_pNpcList[iNpcH]->m_cDir, m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, m_pNpcList[iNpcH]->m_iMagicHitRatio) == false))
                                {
                                    if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (5 * 1000),
                                            m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);

                                        SendNotifyMsg(0, m_pNpcList[iNpcH]->m_iTargetIndex, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0) goto NBA_BREAK1;
                                if ((m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_iHP > 0) &&
                                    (bCheckResistingIceSuccess(m_pNpcList[iNpcH]->m_cDir, m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_iMagicHitRatio) == false))
                                {
                                    if (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_NPC, true);

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (5 * 1000),
                                            m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_NPC, 0, 0, 0, 1, 0, 0);
                                    }
                                }
                                break;
                        }
                        NBA_BREAK1:;
                        SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 20);
                        iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 20);
                        break;

                    default:
                        SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 20);

                        iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 20);
                        break;
                }
            }

            m_pNpcList[iNpcH]->m_iAttackCount++;


            if ((m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) && (m_pNpcList[iNpcH]->m_cActionLimit == 0))
            {
                switch (m_pNpcList[iNpcH]->m_iAttackStrategy)
                {
                    case DEF_ATTACKAI_EXCHANGEATTACK:

                        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                        m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
                        break;

                    case DEF_ATTACKAI_TWOBYONEATTACK:

                        if (m_pNpcList[iNpcH]->m_iAttackCount >= 2)
                        {
                            m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                            m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
                        }
                        break;
                }
            }
            return;
        }


        NBA_CHASE:;

        if (m_pNpcList[iNpcH]->m_cActionLimit != 0) return;


        m_pNpcList[iNpcH]->m_iAttackCount = 0;

        {

            cDir = cGetNextMoveDir(sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);
            if (cDir == 0)
            {
                return;
            }
            dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
            dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];

            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(9, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
            m_pNpcList[iNpcH]->m_sX = dX;
            m_pNpcList[iNpcH]->m_sY = dY;
            m_pNpcList[iNpcH]->m_cDir = cDir;
            SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
        }
    }
}

void CGame::NpcBehavior_Dead(int iNpcH)
{
    uint32_t dwTime;

    if (m_pNpcList[iNpcH] == 0) return;

    dwTime = timeGetTime();

    m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;
    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 5)
    {

        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
    }
    // 
    if ((dwTime - m_pNpcList[iNpcH]->m_dwDeadTime) > m_pNpcList[iNpcH]->m_dwRegenTime)
        DeleteNpc(iNpcH);
}

void CGame::NpcBehavior_Flee(int iNpcH)
{
    char cDir;
    short sX, sY, dX, dY;
    short sTarget;
    char  cTargetType;

    if (m_pNpcList[iNpcH] == 0) return;
    if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;


    m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;


    switch (m_pNpcList[iNpcH]->m_iAttackStrategy)
    {
        case DEF_ATTACKAI_EXCHANGEATTACK:
        case DEF_ATTACKAI_TWOBYONEATTACK:
            if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount >= 2)
            {

                m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                return;
            }
            break;

        default:
            if (iDice(1, 2) == 1) NpcRequestAssistance(iNpcH);
            break;
    }

    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 10)
    {

        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
        m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
        m_pNpcList[iNpcH]->m_tmp_iError = 0;
        if (m_pNpcList[iNpcH]->m_iHP <= 3)
        {
            m_pNpcList[iNpcH]->m_iHP += iDice(1, m_pNpcList[iNpcH]->m_iHitDice);
            if (m_pNpcList[iNpcH]->m_iHP <= 0) m_pNpcList[iNpcH]->m_iHP = 1;
        }
        return;
    }


    TargetSearch(iNpcH, &sTarget, &cTargetType);
    if (sTarget != 0)
    {
        m_pNpcList[iNpcH]->m_iTargetIndex = sTarget;
        m_pNpcList[iNpcH]->m_cTargetType = cTargetType;
    }


    sX = m_pNpcList[iNpcH]->m_sX;
    sY = m_pNpcList[iNpcH]->m_sY;
    dX = 0;
    dY = 0;
    switch (m_pNpcList[iNpcH]->m_cTargetType)
    {
        case DEF_OWNERTYPE_PLAYER:
            dX = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
            dY = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
            break;
        case DEF_OWNERTYPE_NPC:
            dX = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
            dY = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
            break;
    }
    dX = sX - (dX - sX);
    dY = sY - (dY - sY);

    cDir = cGetNextMoveDir(sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);
    if (cDir == 0)
    {

    }
    else
    {
        dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
        dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];

        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(11, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
        m_pNpcList[iNpcH]->m_sX = dX;
        m_pNpcList[iNpcH]->m_sY = dY;
        m_pNpcList[iNpcH]->m_cDir = cDir;
        SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
    }
}

void CGame::NpcBehavior_Stop(int iNpcH)
{
    char  cTargetType;
    short sTarget = 0;
    bool  bFlag;



    if (m_pNpcList[iNpcH] == 0) return;


    m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;

    switch (m_pNpcList[iNpcH]->m_cActionLimit)
    {
        case 5:

            switch (m_pNpcList[iNpcH]->m_sType)
            {
                case 38: // Mana Collector
                    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount >= 3)
                    {
                        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                        bFlag = _bNpcBehavior_ManaCollector(iNpcH);

                        if (bFlag == true)
                        {

                            SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 1);
                        }
                    }
                    break;

                case 39: // Detector
                    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount >= 3)
                    {
                        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                        bFlag = _bNpcBehavior_Detector(iNpcH);

                        if (bFlag == true)
                        {

                            SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 1);
                        }
                    }
                    break;

                case 40: // Energy Shield Generator
                    break;

                case 41: // Grand Magic Generator
                    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount >= 3)
                    {
                        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                        _NpcBehavior_GrandMagicGenerator(iNpcH);
                    }
                    break;

                case 42:
                    m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                    m_pNpcList[iNpcH]->m_iV1 += 5;
                    if (m_pNpcList[iNpcH]->m_iV1 >= 5) m_pNpcList[iNpcH]->m_iV1 = 5;
                    break;

                default:
                    TargetSearch(iNpcH, &sTarget, &cTargetType);
                    break;
            }
            break;
    }

    if ((sTarget != 0))
    {


        m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
        m_pNpcList[iNpcH]->m_iTargetIndex = sTarget;
        m_pNpcList[iNpcH]->m_cTargetType = cTargetType;

        return;
    }
}

void CGame::NpcBehavior_Move(int iNpcH)
{
    char  cDir;
    short sX, sY, dX, dY, absX, absY;
    short sTarget, sDistance;
    char  cTargetType;

    if (m_pNpcList[iNpcH] == 0) return;
    if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;

    if ((m_pNpcList[iNpcH]->m_bIsSummoned == true) &&
        (m_pNpcList[iNpcH]->m_iSummonControlMode == 1)) return;
    if (m_pNpcList[iNpcH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) return;


    switch (m_pNpcList[iNpcH]->m_cActionLimit)
    {
        case 2:
        case 3:
        case 5:
            m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_STOP;
            m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
            return;
    }


    int iStX, iStY;
    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex] != 0)
    {
        iStX = m_pNpcList[iNpcH]->m_sX / 20;
        iStY = m_pNpcList[iNpcH]->m_sY / 20;
        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iMonsterActivity++;
    }


    m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;
    if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 5)
    {

        m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;


        absX = abs(m_pNpcList[iNpcH]->m_vX - m_pNpcList[iNpcH]->m_sX);
        absY = abs(m_pNpcList[iNpcH]->m_vY - m_pNpcList[iNpcH]->m_sY);

        if ((absX <= 2) && (absY <= 2))
        {

            CalcNextWayPointDestination(iNpcH);
        }

        m_pNpcList[iNpcH]->m_vX = m_pNpcList[iNpcH]->m_sX;
        m_pNpcList[iNpcH]->m_vY = m_pNpcList[iNpcH]->m_sY;
    }


    TargetSearch(iNpcH, &sTarget, &cTargetType);
    if (sTarget != 0)
    {

        if (m_pNpcList[iNpcH]->m_dwActionTime < 1000)
        {

            if (iDice(1, 3) == 3)
            {
                m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
                m_pNpcList[iNpcH]->m_iTargetIndex = sTarget;
                m_pNpcList[iNpcH]->m_cTargetType = cTargetType;

                return;
            }
        }
        else
        {

            m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
            m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
            m_pNpcList[iNpcH]->m_iTargetIndex = sTarget;
            m_pNpcList[iNpcH]->m_cTargetType = cTargetType;

            return;
        }
    }


    if ((m_pNpcList[iNpcH]->m_bIsMaster == true) && (iDice(1, 3) == 2)) return;

    if (m_pNpcList[iNpcH]->m_cMoveType == DEF_MOVETYPE_FOLLOW)
    {

        sX = m_pNpcList[iNpcH]->m_sX;
        sY = m_pNpcList[iNpcH]->m_sY;
        switch (m_pNpcList[iNpcH]->m_cFollowOwnerType)
        {
            case DEF_OWNERTYPE_PLAYER:

                if (m_pClientList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex] == 0)
                {
                    m_pNpcList[iNpcH]->m_cMoveType = DEF_MOVETYPE_RANDOM;
                    return;
                }

                dX = m_pClientList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sX;
                dY = m_pClientList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sY;
                break;
            case DEF_OWNERTYPE_NPC:

                if (m_pNpcList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex] == 0)
                {
                    m_pNpcList[iNpcH]->m_cMoveType = DEF_MOVETYPE_RANDOM;
                    m_pNpcList[iNpcH]->m_iFollowOwnerIndex = 0;
                    //bSerchMaster(iNpcH);
                    return;
                }

                dX = m_pNpcList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sX;
                dY = m_pNpcList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sY;
                break;
        }

        if (abs(sX - dX) >= abs(sY - dY))
            sDistance = abs(sX - dX);
        else sDistance = abs(sY - dY);

        if (sDistance >= 3)
        {

            cDir = cGetNextMoveDir(sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);
            if (cDir == 0)
            {

            }
            else
            {
                dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
                dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(3, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
                m_pNpcList[iNpcH]->m_sX = dX;
                m_pNpcList[iNpcH]->m_sY = dY;
                m_pNpcList[iNpcH]->m_cDir = cDir;
                SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
            }
        }
    }
    else
    {

        cDir = cGetNextMoveDir(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
            m_pNpcList[iNpcH]->m_dX, m_pNpcList[iNpcH]->m_dY,
            m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);

        if (cDir == 0)
        {

            if (iDice(1, 10) == 3) CalcNextWayPointDestination(iNpcH);
        }
        else
        {
            dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
            dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];

            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(4, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
            m_pNpcList[iNpcH]->m_sX = dX;
            m_pNpcList[iNpcH]->m_sY = dY;
            m_pNpcList[iNpcH]->m_cDir = cDir;
            SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
        }
    }
}
