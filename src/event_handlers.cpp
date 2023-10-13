//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

void CGame::NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage)
{
    short  sAttackerWeapon;
    int * ip, i, iQuestIndex, iExp, iConstructionPoint, iWarContribution, iMapIndex;
    double dTmp1, dTmp2, dTmp3;
    char * cp, cData[120], cQuestRemain;

    if (m_pNpcList[iNpcH] == NULL) return;
    if (m_pNpcList[iNpcH]->m_bIsKilled == TRUE) return;

    m_pNpcList[iNpcH]->m_bIsKilled = TRUE;
    m_pNpcList[iNpcH]->m_iHP = 0;
    m_pNpcList[iNpcH]->m_iLastDamage = sDamage;

    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

    RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);

    ReleaseFollowMode(iNpcH, DEF_OWNERTYPE_NPC);

    m_pNpcList[iNpcH]->m_iTargetIndex = NULL;
    m_pNpcList[iNpcH]->m_cTargetType = NULL;
    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
    }
    else sAttackerWeapon = 1;
    SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, NULL);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(10, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
    m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_DEAD;

    m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
    m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();

    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) return;

    NpcDeadItemGenerator(iNpcH, sAttackerH, cAttackerType);

    if ((m_pNpcList[iNpcH]->m_bIsSummoned != TRUE) && (cAttackerType == DEF_OWNERTYPE_PLAYER) &&
        (m_pClientList[sAttackerH] != NULL))
    {
        iExp = (m_pNpcList[iNpcH]->m_iExp / 3);
        if (m_pNpcList[iNpcH]->m_iNoDieRemainExp > 0)
            iExp += m_pNpcList[iNpcH]->m_iNoDieRemainExp;

        if (m_pClientList[sAttackerH]->m_iAddExp != NULL)
        {
            dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
            dTmp2 = (double)iExp;
            dTmp3 = (dTmp1 / 100.0f) * dTmp2;
            iExp += (int)dTmp3;
        }

        if (m_pNpcList[iNpcH]->m_sType == 81)
        {
            for (i = 1; i < DEF_MAXCLIENTS; i++)
            {
                if (m_pClientList[i] != NULL)
                {
                    SendNotifyMsg(sAttackerH, i, DEF_NOTIFY_ABADDONKILLED, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                }
            }
        }

        if (m_bIsCrusadeMode == TRUE)
        {
            if (iExp > 10) iExp = iExp / 3;
        }

        GetExp(sAttackerH, iExp, TRUE);

        iQuestIndex = m_pClientList[sAttackerH]->m_iQuest;
        if (iQuestIndex != NULL)
        {
            if (m_pQuestConfigList[iQuestIndex] != NULL)
            {
                switch (m_pQuestConfigList[iQuestIndex]->m_iType)
                {
                    case DEF_QUESTTYPE_MONSTERHUNT:
                        if ((m_pClientList[sAttackerH]->m_bQuestMatchFlag_Loc == TRUE) &&
                            (m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_pNpcList[iNpcH]->m_sType))
                        {
                            m_pClientList[sAttackerH]->m_iCurQuestCount++;
                            cQuestRemain = (m_pQuestConfigList[m_pClientList[sAttackerH]->m_iQuest]->m_iMaxCount - m_pClientList[sAttackerH]->m_iCurQuestCount);
                            SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_QUESTCOUNTER, cQuestRemain, NULL, NULL, NULL);
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
        case 64: m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum(); break;

    }

    if (iConstructionPoint != NULL)
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

                    SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, NULL, NULL);
                }
                else
                {
                    m_pClientList[sAttackerH]->m_iWarContribution -= (iWarContribution * 2);
                    if (m_pClientList[sAttackerH]->m_iWarContribution < 0)
                        m_pClientList[sAttackerH]->m_iWarContribution = 0;

                    wsprintf(G_cTxt, "Friendly Npc Killed by player! WarContribution: -%d", iWarContribution);
                    log->info(G_cTxt);

                    SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, NULL, NULL);
                }
                break;

            case DEF_OWNERTYPE_NPC:
                if (m_pNpcList[sAttackerH]->m_iGuildGUID != NULL)
                {
                    if (m_pNpcList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide)
                    {
                        for (i = 1; i < DEF_MAXCLIENTS; i++)
                            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[sAttackerH]->m_iGuildGUID) &&
                                (m_pClientList[i]->m_iCrusadeDuty == 3))
                            {

                                m_pClientList[i]->m_iConstructionPoint += iConstructionPoint;
                                if (m_pClientList[i]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                                    m_pClientList[i]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                                wsprintf(G_cTxt, "Enemy Npc Killed by Npc! Construct point +%d", iConstructionPoint);
                                log->info(G_cTxt);
                                SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, NULL, NULL);
                                goto NKH_GOTOPOINT1;
                            }

                        ZeroMemory(cData, sizeof(cData));
                        cp = (char *)cData;
                        *cp = GSM_CONSTRUCTIONPOINT;
                        cp++;
                        ip = (int *)cp;
                        *ip = m_pNpcList[sAttackerH]->m_iGuildGUID;
                        cp += 4;
                        ip = (int *)cp;
                        *ip = iConstructionPoint;
                        cp += 4;
                        bStockMsgToGateServer(cData, 9);
                    }
                }
                break;
        }
    }

    NKH_GOTOPOINT1:;

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

    if ((m_bIsHeldenianMode == TRUE) && (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_bIsHeldenianMap == TRUE) && (m_cHeldenianModeType == 1))
    {
        iMapIndex = 0;
        iMapIndex = m_pNpcList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cMapIndex;
        if ((m_pNpcList[iNpcH]->m_sType == 87) || (m_pNpcList[iNpcH]->m_sType == 89))
        {
            if (m_pNpcList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cSide == 1)
            {
                m_iHeldenianAresdenLeftTower--;
                wsprintf(G_cTxt, "Aresden Tower Broken, Left TOWER %d", m_iHeldenianAresdenLeftTower);
            }
            else if (m_pNpcList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cSide == 2)
            {
                m_iHeldenianElvineLeftTower--;
                wsprintf(G_cTxt, "Elvine Tower Broken, Left TOWER %d", m_iHeldenianElvineLeftTower);
            }
            log->info(G_cTxt);
            UpdateHeldenianStatus();
        }
        if ((m_iHeldenianElvineLeftTower == 0) || (m_iHeldenianAresdenLeftTower == 0))
        {
            GlobalEndHeldenianMode();
        }
    }
}

void CGame::DropItemHandler(int iClientH, short sItemIndex, int iAmount, char * pItemName, BOOL bByPlayer)
{
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
    if ((iAmount != -1) && (iAmount < 0)) return;

    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
        (iAmount == -1))
        iAmount = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount;


    if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0) return;

    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
        (((int)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount - iAmount) > 0))
    {
        pItem = new CItem;
        if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == FALSE)
        {
            delete pItem;
            return;
        }
        else
        {
            if (iAmount <= 0)
            {
                delete pItem;
                return;
            }
            pItem->m_dwCount = (DWORD)iAmount;
        }

        if ((DWORD)iAmount > m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount)
        {
            delete pItem;
            return;
        }

        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount -= iAmount;

        SetItemCount(iClientH, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount);

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY, pItem);

        if (bByPlayer == TRUE)
            _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);
        else
            _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem, TRUE);

        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED, sItemIndex, iAmount, NULL, NULL);
    }
    else
    {

        ReleaseItemHandler(iClientH, sItemIndex, TRUE);

        if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == TRUE)
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);

        if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP) &&
            (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan == 0))
        {
            delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
            m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
        }
        else
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]);

            if (bByPlayer == TRUE)
                _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);
            else
                _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex], TRUE);

            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);
        }

        m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
        m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, NULL, NULL);

        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
    }

    iCalcTotalWeight(iClientH);
}

BOOL CGame::bEquipItemHandler(int iClientH, short sItemIndex, BOOL bNotify)
{
    char  cEquipPos, cHeroArmorType;
    short   sSpeed;
    short sTemp;
    int iTemp;

    if (m_pClientList[iClientH] == NULL) return FALSE;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return FALSE;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return FALSE;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return FALSE;

    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan == 0) return FALSE;

    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) == NULL) &&
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sLevelLimit > m_pClientList[iClientH]->m_iLevel)) return FALSE;


    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 0)
    {
        switch (m_pClientList[iClientH]->m_sType)
        {
            case 1:
            case 2:
            case 3:
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 1) return FALSE;
                break;
            case 4:
            case 5:
            case 6:
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 2) return FALSE;
                break;
        }
    }

    if (iGetItemWeight(m_pClientList[iClientH]->m_pItemList[sItemIndex], 1) > m_pClientList[iClientH]->m_iStr * 100) return FALSE;

    cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

    if ((cEquipPos == DEF_EQUIPPOS_BODY) || (cEquipPos == DEF_EQUIPPOS_LEGGINGS) ||
        (cEquipPos == DEF_EQUIPPOS_ARMS) || (cEquipPos == DEF_EQUIPPOS_HEAD))
    {
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4)
        {
            case 10: // Str
                if (m_pClientList[iClientH]->m_iStr < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
                    return FALSE;
                }
                break;
            case 11: // Dex
                if (m_pClientList[iClientH]->m_iDex < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
                    return FALSE;
                }
                break;
            case 12: // Vit
                if (m_pClientList[iClientH]->m_iVit < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
                    return FALSE;
                }
                break;
            case 13: // Int
                if (m_pClientList[iClientH]->m_iInt < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
                    return FALSE;
                }
                break;
            case 14: // Mag
                if (m_pClientList[iClientH]->m_iMag < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
                    return FALSE;
                }
                break;
            case 15: // Chr
                if (m_pClientList[iClientH]->m_iCharisma < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, NULL, NULL);
                    ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], TRUE);
                    return FALSE;
                }
                break;
        }
    }

    if (cEquipPos == DEF_EQUIPPOS_TWOHAND)
    {
        // Stormbringer
        if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 845)
        {
            if (m_pClientList[iClientH]->m_iInt < 65)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, sItemIndex, NULL, NULL);
                ReleaseItemHandler(iClientH, sItemIndex, TRUE);
                return FALSE;
            }
        }
    }

    if (cEquipPos == DEF_EQUIPPOS_RHAND)
    {
        // Resurrection wand(MS.10) or Resurrection wand(MS.20)
        if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 866))
        {
            if (m_pClientList[iClientH]->m_iInt > 99 && m_pClientList[iClientH]->m_iMag > 99 && m_pClientList[iClientH]->m_iSpecialAbilityTime < 1)
            {
                m_pClientList[iClientH]->m_cMagicMastery[94] = TRUE;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_SUCCESS, NULL, NULL, NULL, NULL);
            }
        }
    }

    if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY))
    {

        if ((m_pClientList[iClientH]->m_iSpecialAbilityType != 0))
        {
            if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos != m_pClientList[iClientH]->m_iSpecialAbilityEquipPos)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], NULL, NULL);
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], TRUE);
            }
        }
    }


    if (cEquipPos == DEF_EQUIPPOS_NONE) return FALSE;

    if (cEquipPos == DEF_EQUIPPOS_TWOHAND)
    {
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
        else
        {
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND], FALSE);
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND], FALSE);
        }
    }
    else
    {
        if ((cEquipPos == DEF_EQUIPPOS_LHAND) || (cEquipPos == DEF_EQUIPPOS_RHAND))
        {
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND], FALSE);
        }

        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
    }


    if (cEquipPos == DEF_EQUIPPOS_RELEASEALL)
    {
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
        {
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
        }
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD] != -1)
        {
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD], FALSE);
        }
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY] != -1)
        {
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY], FALSE);
        }
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS] != -1)
        {
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS], FALSE);
        }
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS] != -1)
        {
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS], FALSE);
        }
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS] != -1)
        {
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS], FALSE);
        }
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK] != -1)
        {
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK], FALSE);
        }
    }
    else
    {
        if (cEquipPos == DEF_EQUIPPOS_HEAD || cEquipPos == DEF_EQUIPPOS_BODY || cEquipPos == DEF_EQUIPPOS_ARMS ||
            cEquipPos == DEF_EQUIPPOS_LEGGINGS || cEquipPos == DEF_EQUIPPOS_PANTS || cEquipPos == DEF_EQUIPPOS_BACK)
        {
            if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RELEASEALL] != -1)
            {
                ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RELEASEALL], FALSE);
            }
        }
        if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
            ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], FALSE);
    }


    m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = sItemIndex;
    m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = TRUE;

    switch (cEquipPos)
    {

        case DEF_EQUIPPOS_HEAD:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFF0F;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4);
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFFF0;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor));
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_PANTS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xF0FF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8);
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFF0FF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 8);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_LEGGINGS:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0x0FFF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12);
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFF0F;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 4);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_BODY:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;

            if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue < 100)
            {
                sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12);
                m_pClientList[iClientH]->m_sAppr3 = sTemp;
            }
            else
            {
                sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue - 100) << 12);
                m_pClientList[iClientH]->m_sAppr3 = sTemp;
                sTemp = m_pClientList[iClientH]->m_sAppr4;
                sTemp = sTemp | 0x080;
                m_pClientList[iClientH]->m_sAppr4 = sTemp;
            }

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFF0FFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 20);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_ARMS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFFF0;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue));
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 12);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_LHAND:
            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xFFF0;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue));
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xF0FFFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 24);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_RHAND:
            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4);
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
            m_pClientList[iClientH]->m_iApprColor = iTemp;

            iTemp = m_pClientList[iClientH]->m_iStatus;
            iTemp = iTemp & 0xFFFFFFF0;
            sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);
            sSpeed -= (m_pClientList[iClientH]->m_iStr / 13);
            if (sSpeed < 0) sSpeed = 0;
            iTemp = iTemp | (int)sSpeed;
            m_pClientList[iClientH]->m_iStatus = iTemp;
            m_pClientList[iClientH]->m_iComboAttackCount = 0;
            break;

        case DEF_EQUIPPOS_TWOHAND:
            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4);
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFFFFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
            m_pClientList[iClientH]->m_iApprColor = iTemp;

            iTemp = m_pClientList[iClientH]->m_iStatus;
            iTemp = iTemp & 0xFFFFFFF0;
            sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);
            sSpeed -= (m_pClientList[iClientH]->m_iStr / 13);
            if (sSpeed < 0) sSpeed = 0;
            iTemp = iTemp | (int)sSpeed;
            m_pClientList[iClientH]->m_iStatus = iTemp;
            m_pClientList[iClientH]->m_iComboAttackCount = 0;
            break;

        case DEF_EQUIPPOS_BACK:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xF0FF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8);
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFF0FFFF;
            iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 16);
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_RELEASEALL:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;
            sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12);
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor;
            iTemp = iTemp & 0xFFF0FFFF;
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;
    }

    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY)
    {
        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect)
        {
            case 0: break;
            case 1:
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0004;
                break;

            case 2:
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x000C;
                break;

            case 3:
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0008;
                break;
        }
    }

    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)
    {
        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC;
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect)
        {
            case 0:
                break;
            case 50:
            case 51:
            case 52:
                m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0002;
                break;
            default:
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
                    m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0001;
                break;
        }
    }

    cHeroArmorType = _cCheckHeroItemEquipped(iClientH);
    if (cHeroArmorType != 0x0FFFFFFFF) m_pClientList[iClientH]->m_cHeroArmorBonus = cHeroArmorType;

    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
    CalcTotalItemEffect(iClientH, sItemIndex, bNotify);
    return TRUE;

}

void CGame::GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char * pItemName)
{
    int iRet, iEraseReq;
    short * sp, sOwnerH;
    char * cp, cOwnerType, cData[100]{}, cCharName[30];
    DWORD * dwp;
    WORD * wp;
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (iAmount <= 0) return;

    if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0)
    {
        log->info("GiveItemHandler - Not matching Item name");
        return;
    }

    ZeroMemory(cCharName, sizeof(cCharName));

    if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
        (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount > (DWORD)iAmount))
    {
        pItem = new CItem;
        if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == FALSE)
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

        if (wObjectID != NULL)
        {
            if (wObjectID < 10000)
            {
                if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS))
                {
                    if (m_pClientList[wObjectID] != NULL)
                    {
                        if ((WORD)sOwnerH != wObjectID) sOwnerH = NULL;
                    }
                }
            }
            else
            {
                if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS))
                {
                    if (m_pNpcList[wObjectID - 10000] != NULL)
                    {
                        if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = NULL;
                    }
                }
            }
        }

        if (sOwnerH == NULL)
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

            _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

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

                if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == TRUE)
                {
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

                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 53);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:
                            DeleteClient(sOwnerH, TRUE, TRUE);
                            break;
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED, sItemIndex, iAmount, NULL, cCharName);
                }
                else
                {
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                        m_pClientList[iClientH]->m_sY,
                        pItem);

                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);

                    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                    *dwp = MSGID_NOTIFY;
                    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 6);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:
                            DeleteClient(sOwnerH, TRUE, TRUE);
                            break;
                    }

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTGIVEITEM, sItemIndex, iAmount, NULL, cCharName);
                }

            }
            else
            {
                memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

                if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0)
                {
                    if (bSetItemToBankItem(iClientH, pItem) == FALSE)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, NULL, NULL, NULL, NULL);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

                        _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
                    }
                }
                else
                {
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, pItem);

                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
                }
            }
        }
    }
    else
    {
        ReleaseItemHandler(iClientH, sItemIndex, TRUE);

        if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)
            m_pClientList[iClientH]->m_cArrowIndex = -1;

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

        if (wObjectID != NULL)
        {
            if (wObjectID < 10000)
            {
                if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS))
                {
                    if (m_pClientList[wObjectID] != NULL)
                    {
                        if ((WORD)sOwnerH != wObjectID) sOwnerH = NULL;
                    }
                }
            }
            else
            {
                if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS))
                {
                    if (m_pNpcList[wObjectID - 10000] != NULL)
                    {
                        if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = NULL;
                    }
                }
            }
        }

        if (sOwnerH == NULL)
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]);
            _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

            SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, NULL, NULL);
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
                        (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) &&
                        (memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[sOwnerH]->m_cLocation, 10) == 0) &&
                        (m_pClientList[sOwnerH]->m_iGuildRank == 0))
                    {
                        SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION, NULL, NULL, NULL, NULL);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, NULL, cCharName);

                        _bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

                        goto REMOVE_ITEM_PROCEDURE;
                    }
                }

                if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89))
                {
                    if ((memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[sOwnerH]->m_cGuildName, 20) == 0) &&
                        (m_pClientList[iClientH]->m_iGuildRank != -1) &&
                        (m_pClientList[sOwnerH]->m_iGuildRank == 0))
                    {
                        SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION, NULL, NULL, NULL, NULL);
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, NULL, cCharName);

                        _bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

                        goto REMOVE_ITEM_PROCEDURE;
                    }
                }

                if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == TRUE)
                {
                    _bItemLog(DEF_ITEMLOG_GIVE, iClientH, sOwnerH, pItem);

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

                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 53);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:
                            DeleteClient(sOwnerH, TRUE, TRUE);
                            break;
                    }
                }
                else
                {
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                        m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]);
                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);

                    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                    *dwp = MSGID_NOTIFY;
                    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                    *wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

                    iRet = m_pClientList[sOwnerH]->iSendMsg(cData, 6);
                    switch (iRet)
                    {
                        case DEF_XSOCKEVENT_QUENEFULL:
                        case DEF_XSOCKEVENT_SOCKETERROR:
                        case DEF_XSOCKEVENT_CRITICALERROR:
                        case DEF_XSOCKEVENT_SOCKETCLOSED:
                            DeleteClient(sOwnerH, TRUE, TRUE);
                            break;
                    }

                    ZeroMemory(cCharName, sizeof(cCharName));
                }
            }
            else
            {
                memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

                if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0)
                {
                    if (bSetItemToBankItem(iClientH, sItemIndex) == FALSE)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, NULL, NULL, NULL, NULL);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                            m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                        _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);
                    }
                }
                else if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Kennedy", 7) == 0)
                {
                    if ((m_bIsCrusadeMode == FALSE) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89))
                    {

                        if ((m_pClientList[iClientH]->m_iGuildRank != 0) && (m_pClientList[iClientH]->m_iGuildRank != -1))
                        {
                            SendNotifyMsg(iClientH, iClientH, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, NULL, NULL, NULL, NULL);

                            ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
                            memcpy(m_pClientList[iClientH]->m_cGuildName, "NONE", 4);
                            m_pClientList[iClientH]->m_iGuildRank = -1;
                            m_pClientList[iClientH]->m_iGuildGUID = -1;

                            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);

                            m_pClientList[iClientH]->m_iExp -= 300;
                            if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_EXP, NULL, NULL, NULL, NULL);
                        }

                        delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
                    }
                    else
                    {
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                            m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                        _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);

                        ZeroMemory(cCharName, sizeof(cCharName));

                    }
                }
                else
                {
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
                        m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                    _bItemLog(DEF_ITEMLOG_DROP, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
                        m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);

                    ZeroMemory(cCharName, sizeof(cCharName));
                }
            }
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, iAmount, NULL, cCharName);
        }

        REMOVE_ITEM_PROCEDURE:;

        if (m_pClientList[iClientH] == NULL) return;

        m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;
        m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;

        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
    }

    iCalcTotalWeight(iClientH);
}

void CGame::ClientKilledHandler(int iClientH, int iAttackerH, char cAttackerType, short sDamage)
{
    char * cp, cAttackerName[30], cData[120];
    short sAttackerWeapon;
    int * ip, i, iExH;
    BOOL  bIsSAattacked = FALSE;


    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;

    if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "fight", 5) == 0)
    {
        m_pClientList[iClientH]->m_dwFightzoneDeadTime = timeGetTime();
        wsprintf(G_cTxt, "Fightzone Dead Time: %d", m_pClientList[iClientH]->m_dwFightzoneDeadTime);
        log->info(G_cTxt);
    }

    m_pClientList[iClientH]->m_bIsKilled = TRUE;
    m_pClientList[iClientH]->m_iHP = 0;

    if (m_pClientList[iClientH]->m_bIsExchangeMode == TRUE)
    {
        iExH = m_pClientList[iClientH]->m_iExchangeH;
        _ClearExchangeStatus(iExH);
        _ClearExchangeStatus(iClientH);
    }

    RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

    ZeroMemory(cAttackerName, sizeof(cAttackerName));
    switch (cAttackerType)
    {
        case DEF_OWNERTYPE_PLAYER_INDIRECT:
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[iAttackerH] != NULL)
                memcpy(cAttackerName, m_pClientList[iAttackerH]->m_cCharName, 10);
            break;
        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[iAttackerH] != NULL)
#ifdef DEF_LOCALNPCNAME 
                wsprintf(cAttackerName, "NPCNPCNPC@%d", m_pNpcList[iAttackerH]->m_sType);
#else 
                memcpy(cAttackerName, m_pNpcList[iAttackerH]->m_cNpcName, 20);
#endif
            break;
        default:
            break;
    }

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_KILLED, NULL, NULL, NULL, cAttackerName);
    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        sAttackerWeapon = ((m_pClientList[iAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
    }
    else sAttackerWeapon = 1;
    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, NULL);
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(12, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetDeadOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) return;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == TRUE)
    {
        if (m_pClientList[iClientH]->m_cSide == 1)
        {
            m_iHeldenianAresdenDead++;
        }
        else if (m_pClientList[iClientH]->m_cSide == 2)
        {
            m_iHeldenianElvineDead++;
        }
        UpdateHeldenianStatus();
    }

    if (cAttackerType == DEF_OWNERTYPE_PLAYER)
    {
        switch (m_pClientList[iAttackerH]->m_iSpecialAbilityType)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                bIsSAattacked = TRUE;
                break;
        }

        if (iAttackerH == iClientH) return;
        if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0)
        {
            if (m_pClientList[iClientH]->m_iPKCount == 0)
                ApplyPKpenalty(iAttackerH, iClientH);
            else
                PK_KillRewardHandler(iAttackerH, iClientH);
        }
        else
        {
            if (m_pClientList[iClientH]->m_iGuildRank == -1)
            {
                if (memcmp(m_pClientList[iAttackerH]->m_cLocation, "NONE", 4) == 0)
                {
                    if (m_pClientList[iClientH]->m_iPKCount == 0)
                        ApplyPKpenalty(iAttackerH, iClientH);
                }
                else
                {
                    if (memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iAttackerH]->m_cLocation, 10) == 0)
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
                if (memcmp(m_pClientList[iAttackerH]->m_cLocation, "NONE", 4) == 0)
                {
                    if (m_pClientList[iClientH]->m_iPKCount == 0)
                        ApplyPKpenalty(iAttackerH, iClientH);
                }
                else
                {
                    if (memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iAttackerH]->m_cLocation, 10) == 0)
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
            if (memcmp(m_pClientList[iAttackerH]->m_cLocation, "NONE", 4) == 0)
            {
            }
            else
            {
                if (memcmp(m_pClientList[iAttackerH]->m_cLocation, m_pClientList[iClientH]->m_cLocation, 10) == 0)
                {
                }
                else
                {
                    ApplyCombatKilledPenalty(iClientH, 2, bIsSAattacked);
                }
            }
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 1) && (m_pClientList[iClientH]->m_iPKCount <= 3))
        {
            ApplyCombatKilledPenalty(iClientH, 3, bIsSAattacked);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 4) && (m_pClientList[iClientH]->m_iPKCount <= 11))
        {
            ApplyCombatKilledPenalty(iClientH, 6, bIsSAattacked);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 12))
        {
            ApplyCombatKilledPenalty(iClientH, 12, bIsSAattacked);
        }
    }
    else if (cAttackerType == DEF_OWNERTYPE_NPC)
    {
        _bPKLog(DEF_PKLOG_BYNPC, iClientH, NULL, cAttackerName);

        if (m_pClientList[iClientH]->m_iPKCount == 0)
        {
            ApplyCombatKilledPenalty(iClientH, 1, bIsSAattacked);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 1) && (m_pClientList[iClientH]->m_iPKCount <= 3))
        {
            ApplyCombatKilledPenalty(iClientH, 3, bIsSAattacked);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 4) && (m_pClientList[iClientH]->m_iPKCount <= 11))
        {
            ApplyCombatKilledPenalty(iClientH, 6, bIsSAattacked);
        }
        else if ((m_pClientList[iClientH]->m_iPKCount >= 12))
        {
            ApplyCombatKilledPenalty(iClientH, 12, bIsSAattacked);
        }
        if (m_pNpcList[iAttackerH]->m_iGuildGUID != NULL)
        {

            if (m_pNpcList[iAttackerH]->m_cSide != m_pClientList[iClientH]->m_cSide)
            {
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[iAttackerH]->m_iGuildGUID) &&
                        (m_pClientList[i]->m_iCrusadeDuty == 3))
                    {
                        m_pClientList[i]->m_iConstructionPoint += (m_pClientList[iClientH]->m_iLevel / 2);

                        if (m_pClientList[i]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                            m_pClientList[i]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                        wsprintf(G_cTxt, "Enemy Player Killed by Npc! Construction +%d", (m_pClientList[iClientH]->m_iLevel / 2));
                        log->info(G_cTxt);
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, NULL, NULL);
                        return;
                    }

                ZeroMemory(cData, sizeof(cData));
                cp = (char *)cData;
                *cp = GSM_CONSTRUCTIONPOINT;
                cp++;
                ip = (int *)cp;
                *ip = m_pNpcList[iAttackerH]->m_iGuildGUID;
                cp += 4;
                ip = (int *)cp;
                *ip = (m_pClientList[iClientH]->m_iLevel / 2);
                cp += 4;
                bStockMsgToGateServer(cData, 9);
            }
        }
    }
    else if (cAttackerType == DEF_OWNERTYPE_PLAYER_INDIRECT)
    {
        _bPKLog(DEF_PKLOG_BYOTHER, iClientH, NULL, NULL);
    }

    char cKillMsg[80];
    ZeroMemory(cKillMsg, sizeof(cKillMsg));

    switch (iDice(1, 4))
    {
        case 1:
            wsprintf(cKillMsg, "%s whooped %s's ass!", cAttackerName, m_pClientList[iClientH]->m_cCharName);
            break;

        case 2:
            wsprintf(cKillMsg, "%s smashed %s's face into the ground!", cAttackerName, m_pClientList[iClientH]->m_cCharName);
            break;

        case 3:
            wsprintf(cKillMsg, "%s was sliced to pieces by %s!", m_pClientList[iClientH]->m_cCharName, cAttackerName);
            break;

        case 4:
            wsprintf(cKillMsg, "%s was gutted by %s!", m_pClientList[iClientH]->m_cCharName, cAttackerName);
            break;

        default:
            wsprintf(cKillMsg, "%s is now sleeping for good thanks to %s", m_pClientList[iClientH]->m_cCharName, cAttackerName);
            break;

    }
    wsprintf(G_cTxt, "%s killed %s", m_pClientList[iAttackerH]->m_cCharName, m_pClientList[iClientH]->m_cCharName);
    log->info(G_cTxt);

    for (int iS = 0; iS < DEF_MAXCLIENTS; iS++)
    {
        if ((m_pClientList[iS] != NULL))
        {
            EKAnnounce(iS, cKillMsg);
        }
    }
}

void CGame::ReleaseItemHandler(int iClientH, short sItemIndex, BOOL bNotice)
{
    char cEquipPos, cHeroArmorType;
    short  sTemp;
    int   iTemp;

    if (m_pClientList[iClientH] == NULL) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return;

    if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == FALSE) return;

    cHeroArmorType = _cCheckHeroItemEquipped(iClientH);
    if (cHeroArmorType != 0x0FFFFFFFF) m_pClientList[iClientH]->m_cHeroArmorBonus = 0;

    cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;
    if (cEquipPos == DEF_EQUIPPOS_RHAND)
    {
        if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL)
        {
            if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 866))
            {
                m_pClientList[iClientH]->m_cMagicMastery[94] = FALSE;
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_STATECHANGE_SUCCESS, NULL, NULL, NULL, NULL);
            }
        }
    }

    switch (cEquipPos)
    {
        case DEF_EQUIPPOS_RHAND:
            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;	// Â¹Â«Â±Ã¢ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0x0FFFFFFF; // Â¹Â«Â±Ã¢ Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;

            // V1.3 Â¹Â«Â±Ã¢Â¼Ã“ÂµÂµ Ã€Ã§Â°Ã¨Â»Ãª <- Â¸Ã‡Â¼Ã•Ã€ÃŒÂ´ÃÂ±Ã± 0
            iTemp = m_pClientList[iClientH]->m_iStatus;
            iTemp = iTemp & 0xFFFFFFF0;
            m_pClientList[iClientH]->m_iStatus = iTemp;
            break;

        case DEF_EQUIPPOS_LHAND:
            // Â¿ÃžÂ¼Ã•Â¿Â¡ Ã€Ã¥Ã‚Ã¸Ã‡ÃÂ´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›. Â¹Ã¦Ã†ÃÂ·Ã¹Ã€ÃŒÂ´Ã™. 
            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xFFF0;	// Â¹Â«Â±Ã¢ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xF0FFFFFF; // Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_TWOHAND:
            // Â¾Ã§Â¼Ã•Â¿Â¡ Ã€Ã¥Ã‚Ã¸Ã‡Ã‘Â´Ã™ÃÃ¶Â¸Â¸ Â»Ã§Â½Ã‡ Â¿Ã€Â¸Â¥Â¼Ã•Â¿Â¡Â¸Â¸ ÂµÃ©Â°Ã­ Ã€Ã–Â´Ã™. Â¿ÃžÂ¼Ã•Ã€Âº ÂºÃ±Â¾Ã®Ã€Ã–Â´Ã‚ Â»Ã³Ã…Ã‚.
            sTemp = m_pClientList[iClientH]->m_sAppr2;
            sTemp = sTemp & 0xF00F;	// Â¹Â«Â±Ã¢ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr2 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0x0FFFFFFF; // Â¹Â«Â±Ã¢ Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_BODY:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;	// Â°Ã‘Â¿ÃŠ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            // Â°Ã‘Â¿ÃŠ ÃˆÂ®Ã€Ã¥ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xFF7F;
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xFF0FFFFF; // Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_BACK:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0xF0FF;	// Â¸ÃÃ…Ã¤ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xFFF0FFFF; // Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_ARMS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFFF0;	// Â°Ã‘Â¿ÃŠ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xFFFF0FFF; // Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_PANTS:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xF0FF;	// Â°Ã‘Â¿ÃŠ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xFFFFF0FF; // Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_LEGGINGS:
            sTemp = m_pClientList[iClientH]->m_sAppr4;
            sTemp = sTemp & 0x0FFF;	// Â°Ã‘Â¿ÃŠ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr4 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xFFFFFF0F; // Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_HEAD:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0xFF0F;	// Ã…ÃµÂ±Â¸ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xFFFFFFF0; // Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;

        case DEF_EQUIPPOS_RELEASEALL:
            sTemp = m_pClientList[iClientH]->m_sAppr3;
            sTemp = sTemp & 0x0FFF;	// Â¸ÃÃ…Ã¤ Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_sAppr3 = sTemp;

            iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 Ã„ÃƒÂ·Â¯Â°Âª Â¼Â¼Ã†Ãƒ 
            iTemp = iTemp & 0xFFF0FFFF; // Â¸ÃÃ…Ã¤ Â»Ã¶ ÂºÃ±Ã†Â®Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®.
            m_pClientList[iClientH]->m_iApprColor = iTemp;
            break;
    }

    //v1.432 Ã†Â¯Â¼Ã¶ Â´Ã‰Â·Ã‚Ã€ÃŒ ÂºÃŽÂ¿Â©ÂµÃˆ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒÂ¶Ã³Â¸Ã© Ã‡ÃƒÂ·Â¡Â±Ã— Â¼Â³ÃÂ¤ 
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY)
    {
        // Â°Ã¸Â°Ã Ã†Â¯Â¼Ã¶ Â´Ã‰Â·Ã‚ ÂºÃ±Ã†Â® Ã…Â¬Â¸Â®Â¾Ã®
        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;
    }

    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)
    {
        // Â¹Ã¦Â¾Ã®Â±Â¸ Ã†Â¯Â¼Ã¶ Â´Ã‰Â·Ã‚ ÂºÃ±Ã†Â® Ã…Â¬Â¸Â®Â¾Ã®
        m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC;
    }

    m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;
    m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = -1;

    // Â¿ÃœÃ‡Ã¼Ã€ÃŒ Â¹Ã™Â²Ã¯Â°ÃÃ€Â» Â¾Ã‹Â¸Â°Â´Ã™.
    if (bNotice == TRUE)
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);

    // Ã€ÃŒ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Ã‡Ã˜ÃÂ¦Ã‡ÃÂ´Ã‚ Â°ÃÂ¿Â¡ ÂµÃ»Â¸Â£Â´Ã‚ Ã†Â¯Â¼ÂºÃ„Â¡Ã€Ã‡ ÂºÂ¯ÃˆÂ­Â¸Â¦ Â°Ã¨Â»ÃªÃ‡Ã‘Â´Ã™. 
    CalcTotalItemEffect(iClientH, sItemIndex, TRUE);
}

void CGame::ToggleCombatModeHandler(int iClientH)
{
    short sAppr2;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;
    // ÃÃ—Ã€ÂºÃƒÂ´Ã‡ÃÂ±Ã¢Â³Âª ÃÃ—Ã€Âº Â»Ã³Ã…Ã‚Â¿Â¡Â¼Â­Â´Ã‚ ÂºÂ¯ÃˆÂ¯Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
    if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == TRUE) return;

    sAppr2 = (short)((m_pClientList[iClientH]->m_sAppr2 & 0xF000) >> 12);

    m_pClientList[iClientH]->m_bIsAttackModeChange = TRUE; // v2.172


    if (sAppr2 == 0)
    {
        // ÂºÃ±Ã€Ã¼Ã…Ãµ Â¸Ã°ÂµÃ¥Â¿Â´Â´Ã™. Ã€Ã¼Ã…ÃµÂ¸Ã°ÂµÃ¥Â·ÃŽ Â¹Ã™Â²Ã›Â´Ã™.
        m_pClientList[iClientH]->m_sAppr2 = (0xF000 | m_pClientList[iClientH]->m_sAppr2);
    }
    else
    {
        // Ã€Ã¼Ã…Ãµ Â¸Ã°ÂµÃ¥Â¿Â´Â´Ã™. ÂºÃ±Ã€Ã¼Ã…ÃµÂ¸Ã°ÂµÃ¥Â·ÃŽ Â¹Ã™Â²Ã›Â´Ã™.
        m_pClientList[iClientH]->m_sAppr2 = (0x0FFF & m_pClientList[iClientH]->m_sAppr2);
    }

    // Ã„Â³Â¸Â¯Ã…ÃÃ€Ã‡ Â¿ÃœÃ‡Ã¼Ã€ÃŒ Â¹Ã™Â²Ã®Â¾ÃºÃ€Â¸Â¹Ã‡Â·ÃŽ Ã€ÃŒÂºÂ¥Ã†Â®Â¸Â¦ Ã€Ã¼Â´ÃžÃ‡Ã‘Â´Ã™.
    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);

}

int  _tmp_iMCProb[] = { 0, 300, 250, 200, 150, 100, 80, 70, 60, 50, 40 };
int  _tmp_iMLevelPenalty[] = { 0,   5,   5,   8,   8,  10, 14, 28, 32, 36, 40 };
void CGame::PlayerMagicHandler(int iClientH, int dX, int dY, short sType, BOOL bItemEffect, int iV1)
{
    short * sp, sX, sY, sOwnerH, sMagicCircle, rx, ry, sRemainItemSprite, sRemainItemSpriteFrame, sLevelMagic, sTemp;
    char * cp, cData[120]{}, cDir, cOwnerType, cName[11], cItemName[30], cNpcWaypoint[11], cName_Master[11], cNpcName[30], cRemainItemColor, cScanMessage[256];
    double dV1, dV2, dV3, dV4;
    int    i, iErr, iRet, ix, iy, iResult, iDiceRes, iNamingValue, iFollowersNum, iEraseReq, iWhetherBonus;
    int    tX, tY, iManaCost, iMagicAttr;
    CItem * pItem;
    DWORD * dwp, dwTime;
    WORD * wp, wWeaponType;
    short sEqStatus;
    int iMapSide = 0;

    dwTime = timeGetTime();
    m_pClientList[iClientH]->m_bMagicConfirm = TRUE;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if ((dX < 0) || (dX >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeX) ||
        (dY < 0) || (dY >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY)) return;

    if (((dwTime - m_pClientList[iClientH]->m_dwRecentAttackTime) < 1000) && (bItemEffect == 0))
    {
        try
        {
            wsprintf(G_cTxt, "3.51 Detection: (%s) Player: (%s) - Magic casting speed is too fast! Hack?", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {
        }
        return;
    }
    m_pClientList[iClientH]->m_dwRecentAttackTime = dwTime;
    m_pClientList[iClientH]->m_dwLastActionTime = dwTime;

    if (m_pClientList[iClientH]->m_cMapIndex < 0) return;
    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] == NULL) return;

    if ((sType < 0) || (sType >= 100))     return;
    if (m_pMagicConfigList[sType] == NULL) return;

    if ((bItemEffect == FALSE) && (m_pClientList[iClientH]->m_cMagicMastery[sType] != 1)) return;

    if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) return;
    //if ((var_874 == TRUE) && (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == TRUE) && (m_pMagicConfigList[sType]->m_sType != 8)) return;

    if (((m_pClientList[iClientH]->m_iStatus & 0x100000) != 0) && (bItemEffect != TRUE))
    {
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, NULL, -1, NULL);
        return;
    }

    if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
    {
        wWeaponType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);
        if ((wWeaponType >= 34) && (wWeaponType <= 39))
        {
        }
        else return;
    }

    if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] != -1) ||
        (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)) return;

    if ((m_pClientList[iClientH]->m_iSpellCount > 1) && (bItemEffect == FALSE))
    {
        try
        {
            wsprintf(G_cTxt, "TSearch Spell Hack: (%s) Player: (%s) - casting magic without precasting.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {
        }
        return;
    }

    if (m_pClientList[iClientH]->m_bInhibition == TRUE)
    {
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, 0, -1, NULL);
        return;
    }

    /*if (((m_pClientList[iClientH]->m_iUninteruptibleCheck - (iGetMaxHP(iClientH)/10)) > (m_pClientList[iClientH]->m_iHP)) && (m_pClientList[iClientH]->m_bMagicItem == FALSE)) {
        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL);
        return;
    }*/

    if (m_pMagicConfigList[sType]->m_sType == 32)
    { // Invisiblity
        sEqStatus = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
        if ((sEqStatus != -1) && (m_pClientList[iClientH]->m_pItemList[sEqStatus] != NULL))
        {
            if ((m_pClientList[iClientH]->m_pItemList[sEqStatus]->m_sIDnum == 865) || (m_pClientList[iClientH]->m_pItemList[sEqStatus]->m_sIDnum == 866))
            {
                bItemEffect = TRUE;
            }
        }
    }

    sX = m_pClientList[iClientH]->m_sX;
    sY = m_pClientList[iClientH]->m_sY;

    sMagicCircle = (sType / 10) + 1;
    if (m_pClientList[iClientH]->m_cSkillMastery[4] == 0)
        dV1 = 1.0f;
    else dV1 = (double)m_pClientList[iClientH]->m_cSkillMastery[4];

    if (bItemEffect == TRUE) dV1 = (double)100.0f;
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
    if ((m_pClientList[iClientH]->m_bIsSafeAttackMode == TRUE) &&
        (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == FALSE))
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

    wWeaponType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);
    if (wWeaponType == 34)
    {
        iManaCost += 20;
    }

    if (iResult < 100)
    {
        iDiceRes = iDice(1, 100);
        if (iResult < iDiceRes)
        {
            SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, 0, -1, NULL);
            return;
        }
    }

    if (((m_pClientList[iClientH]->m_iHungerStatus <= 10) || (m_pClientList[iClientH]->m_iSP <= 0)) && (iDice(1, 1000) <= 100))
    {
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, 0, -1, NULL);
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

    if (m_pMagicConfigList[sType]->m_sType == 28)
    {
        iResult += 10000;
    }

    if (m_pMagicConfigList[sType]->m_cCategory == 1)
    {
        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetAttribute(sX, sY, 0x00000005) != 0) return;
    }

    iMagicAttr = m_pMagicConfigList[sType]->m_iAttribute;
    if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
    {
        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
    }

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
    if ((m_bIsCrusadeMode == FALSE) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
    {
        if ((m_pClientList[iClientH]->m_bIsPlayerCivil != TRUE) && (m_pClientList[sOwnerH]->m_bIsPlayerCivil == TRUE))
        {
            if (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide) return;
        }
        else if ((m_pClientList[iClientH]->m_bIsPlayerCivil == TRUE) && (m_pClientList[sOwnerH]->m_bIsPlayerCivil == FALSE))
        {
            switch (m_pMagicConfigList[sType]->m_sType)
            {
                case 1:  // DEF_MAGICTYPE_DAMAGE_SPOT
                case 4:  // DEF_MAGICTYPE_SPDOWN_SPOT 4
                case 8:  // DEF_MAGICTYPE_TELEPORT 8
                case 10: // DEF_MAGICTYPE_CREATE 10
                case 11: // DEF_MAGICTYPE_PROTECT 11
                case 12: // DEF_MAGICTYPE_HOLDOBJECT 12
                case 16: // DEF_MAGICTYPE_CONFUSE
                case 17: // DEF_MAGICTYPE_POISON
                case 32: // DEF_MAGICTYPE_RESURRECTION
                    return;
            }
        }
    }

    if (m_pMagicConfigList[sType]->m_dwDelayTime == 0)
    {
        switch (m_pMagicConfigList[sType]->m_sType)
        {

            case DEF_MAGICTYPE_DAMAGE_SPOT:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) && (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                }
                break;

            case DEF_MAGICTYPE_HPUP_SPOT:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                Effect_HpUp_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) && (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                }

                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) && (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_SPDOWN_SPOT:
                break;

            case DEF_MAGICTYPE_SPDOWN_AREA:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;

            case DEF_MAGICTYPE_POLYMORPH:
                // Ã‚ÂºÃ‚Â¯Ã‚Â½Ãƒâ€¦ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½. 
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (1)
                { // bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE) {
                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] != 0) goto MAGIC_NOEFFECT;
                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            // Ã‚Â¿ÃƒÂ¸Ã‚Â·Ã‚Â¡ Ãƒâ€¦Ã‚Â¸Ãƒâ‚¬Ãƒâ€Ãƒâ‚¬Ã‚Â» Ãƒâ‚¬ÃƒÂºÃƒâ‚¬ÃƒÂ¥Ãƒâ€¡ÃƒËœ Ã‚Â³ÃƒÂµÃ‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                            m_pClientList[sOwnerH]->m_sOriginalType = m_pClientList[sOwnerH]->m_sType;
                            // Ã‚Â¹Ãƒâ„¢Ã‚Â²ÃƒÂ¯ Ã‚Â¿ÃƒÅ“Ãƒâ€¡ÃƒÂ¼Ãƒâ‚¬Ã‚Â» Ãƒâ€¦ÃƒÂ«Ã‚ÂºÃ‚Â¸Ãƒâ€¡ÃƒËœ ÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                            m_pClientList[sOwnerH]->m_sType = 18;
                            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] != 0) goto MAGIC_NOEFFECT;
                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_POLYMORPH] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            // Ã‚Â¿ÃƒÂ¸Ã‚Â·Ã‚Â¡ Ãƒâ€¦Ã‚Â¸Ãƒâ‚¬Ãƒâ€Ãƒâ‚¬Ã‚Â» Ãƒâ‚¬ÃƒÂºÃƒâ‚¬ÃƒÂ¥Ãƒâ€¡ÃƒËœ Ã‚Â³ÃƒÂµÃ‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                            m_pNpcList[sOwnerH]->m_sOriginalType = m_pNpcList[sOwnerH]->m_sType;
                            // Ã‚Â¹Ãƒâ„¢Ã‚Â²ÃƒÂ¯ Ã‚Â¿ÃƒÅ“Ãƒâ€¡ÃƒÂ¼Ãƒâ‚¬Ã‚Â» Ãƒâ€¦ÃƒÂ«Ã‚ÂºÃ‚Â¸Ãƒâ€¡ÃƒËœ ÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                            m_pNpcList[sOwnerH]->m_sType = 18;
                            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                            break;
                    }

                    // Ã‚ÂºÃ‚Â¯Ã‚Â½Ãƒâ€¦ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_POLYMORPH, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ã‚Â»ÃƒÂ½Ã‚Â°ÃƒÂ¥Ãƒâ‚¬Ã‚Â½Ãƒâ‚¬Ã‚Â» Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POLYMORPH, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                }
                break;

            case DEF_MAGICTYPE_CANCELLATION:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) && (m_pClientList[sOwnerH]->m_iHP > 0) && (m_pClientList[sOwnerH]->m_iAdminUserLevel == 0))
                {

                    // Removes Invisibility Flag 0x0010
                    SetInvisibilityFlag(sOwnerH, cOwnerType, FALSE);

                    // Removes Illusion Flag 0x01000000
                    SetIllusionFlag(sOwnerH, cOwnerType, FALSE);

                    // Removes Defense Shield Flag 0x02000000
                    // Removes Great Defense Shield Flag 0x02000000
                    SetDefenseShieldFlag(sOwnerH, cOwnerType, FALSE);

                    // Removes Absolute Magic Protection Flag 0x04000000	
                    // Removes Protection From Magic Flag 0x04000000
                    SetMagicProtectionFlag(sOwnerH, cOwnerType, FALSE);

                    // Removes Protection From Arrow Flag 0x08000000
                    SetProtectionFromArrowFlag(sOwnerH, cOwnerType, FALSE);

                    // Removes Illusion Movement Flag 0x00200000
                    SetIllusionMovementFlag(sOwnerH, cOwnerType, FALSE);

                    // Removes Berserk Flag 0x0020
                    SetBerserkFlag(sOwnerH, cOwnerType, FALSE);

                    //Removes ice-added 
                    SetIceFlag(sOwnerH, cOwnerType, FALSE);

                    //Remove paralyse

                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_ICE);
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INHIBITION);
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_INHIBITION, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_INVISIBILITY, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_BERSERK);
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_BERSERK, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_PROTECT);
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_PROTECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_CONFUSE);
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_CONFUSE, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    // Update Client
                    SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                }
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT_SPDOWN:
                // Á÷°ÝÀº Ã³¸®ÇÏÁö ¾Ê´Â´Ù.
                // ÁÖº¯ °ø°Ý È¿°ú 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // ÀÚ½Åµµ ÇÇÆøµÉ ¼ö ÀÖÀ¸´Ï ÁÖÀÇ.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);
                            Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);
                                Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                            }
                        }
                    }
                break;

            case DEF_MAGICTYPE_DAMAGE_LINEAR:
                // Ãƒâ‚¬ÃƒÂÃƒÂÃƒÂ·Ã‚Â¼Ã‚Â± Ã‚Â»ÃƒÂ³Ã‚Â¿Ã‚Â¡ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ã‚Â¸ÃƒÂ±Ãƒâ€¡Ã‚Â¥Ã‚Â¸Ã‚Â¦ Ã‚Â¸ÃƒÂ°Ã‚ÂµÃƒÅ½ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                sX = m_pClientList[iClientH]->m_sX;
                sY = m_pClientList[iClientH]->m_sY;

                for (i = 2; i < 10; i++)
                {
                    iErr = 0;
                    m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

                    // tx, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx-1, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx+1, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx, ty-1
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx, ty+1
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
                }

                // ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ã‚ÂµÃ‚Âµ Ãƒâ€¡Ãƒâ€¡Ãƒâ€ ÃƒÂ¸Ã‚ÂµÃƒâ€° Ã‚Â¼ÃƒÂ¶ Ãƒâ‚¬Ãƒâ€“Ãƒâ‚¬Ã‚Â¸Ã‚Â´ÃƒÂ ÃƒÂÃƒâ€“Ãƒâ‚¬Ãƒâ€¡.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }
                    }

                // dX, dY
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr); // v1.41 FALSE

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr); // v1.41 FALSE
                }
                break;

                // v2.16 2002-5-23 Ã‚Â°ÃƒÂ­Ã‚Â±Ã‚Â¤Ãƒâ€¡ÃƒÂ¶ 
            case DEF_MAGICTYPE_ICE_LINEAR:
                // Ãƒâ‚¬ÃƒÂÃƒÂÃƒÂ·Ã‚Â¼Ã‚Â± Ã‚Â»ÃƒÂ³Ã‚Â¿Ã‚Â¡ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ã‚Â¸ÃƒÂ±Ãƒâ€¡Ã‚Â¥Ã‚Â¸Ã‚Â¦ Ã‚Â¸ÃƒÂ°Ã‚ÂµÃƒÅ½ Ã‚Â¾ÃƒÂ³Ã‚Â¸Ã‚Â®Ã‚Â¸ÃƒÂ§ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                sX = m_pClientList[iClientH]->m_sX;
                sY = m_pClientList[iClientH]->m_sY;

                for (i = 2; i < 10; i++)
                {
                    iErr = 0;
                    m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

                    // tx, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (m_pClientList[sOwnerH]->m_iHP < 0) goto MAGIC_NOEFFECT;
                                // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    }
                                }
                                break;
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        }
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx-1, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    }
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        }
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx+1, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    }
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        }
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx, ty-1
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    }
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        }
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx, ty+1
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    }
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        }
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
                }

                // ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ã‚ÂµÃ‚Âµ Ãƒâ€¡Ãƒâ€¡Ãƒâ€ ÃƒÂ¸Ã‚ÂµÃƒâ€° Ã‚Â¼ÃƒÂ¶ Ãƒâ‚¬Ãƒâ€“Ãƒâ‚¬Ã‚Â¸Ã‚Â´ÃƒÂ ÃƒÂÃƒâ€“Ãƒâ‚¬Ãƒâ€¡.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        }
                                    }
                                    break;
                            }
                            //
                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                                // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                                switch (cOwnerType)
                                {
                                    case DEF_OWNERTYPE_PLAYER:
                                        if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                        // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                        if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                        {
                                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                            {
                                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                                SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                                // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                    sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                                // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                            }
                                        }
                                        break;

                                    case DEF_OWNERTYPE_NPC:
                                        if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                        if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                        {
                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                            {
                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                                SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                                // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                    sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            }
                                        }
                                        break;
                                }
                                //
                            }
                        }
                    }

                // dX, dY
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                {
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr); // v1.41 FALSE
                    // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                            if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                            {
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                {
                                    m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                    SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                    // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                        sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                }
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                            {
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                {
                                    m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                    SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                    // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                        sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                }
                            }
                            break;
                    }
                    //
                }

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr); // v1.41 FALSE
                        // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                    }
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                    }
                                }
                                break;
                        }
                        //
                    }
                }
                break;


            case DEF_MAGICTYPE_INHIBITION:
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INHIBITION] != 0) goto MAGIC_NOEFFECT;
                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 5) goto MAGIC_NOEFFECT;
                        if (m_pClientList[iClientH]->m_cSide == m_pClientList[sOwnerH]->m_cSide) goto MAGIC_NOEFFECT;
                        if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) goto MAGIC_NOEFFECT;
                        //if (m_pClientList[sOwnerH]->m_iAdminUserLevel != 0) goto MAGIC_NOEFFECT;
                        m_pClientList[sOwnerH]->m_bInhibition = TRUE;
                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_INHIBITION, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                            sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                        break;
                }
                break;


            case DEF_MAGICTYPE_TREMOR: // v1.4 ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â¿Ã‚Â¡ Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ® Ãƒâ‚¬ÃƒÅ“Ã‚Â·ÃƒÂ¹ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ„¢.
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                }

                // ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ã‚ÂµÃ‚Âµ Ãƒâ€¡Ãƒâ€¡Ãƒâ€ ÃƒÂ¸Ã‚ÂµÃƒâ€° Ã‚Â¼ÃƒÂ¶ Ãƒâ‚¬Ãƒâ€“Ãƒâ‚¬Ã‚Â¸Ã‚Â´ÃƒÂ ÃƒÂÃƒâ€“Ãƒâ‚¬Ãƒâ€¡.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT:
                // ÃƒÂÃƒÂ·Ã‚Â°ÃƒÂÃƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â³Ã‚Â¸Ã‚Â®Ãƒâ€¡ÃƒÂÃƒÂÃƒÂ¶ Ã‚Â¾ÃƒÅ Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                // ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ã‚ÂµÃ‚Âµ Ãƒâ€¡Ãƒâ€¡Ãƒâ€ ÃƒÂ¸Ã‚ÂµÃƒâ€° Ã‚Â¼ÃƒÂ¶ Ãƒâ‚¬Ãƒâ€“Ãƒâ‚¬Ã‚Â¸Ã‚Â´ÃƒÂ ÃƒÂÃƒâ€“Ãƒâ‚¬Ãƒâ€¡.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_SPUP_AREA:
                // SpÃ‚Â°Ã‚Â¡ Ã‚Â»ÃƒÂ³Ã‚Â½Ãƒâ€šÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                // Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÅ Ã‚Â¿ÃƒÂ¤Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ„¢. 
                Effect_SpUp_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                // ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ã‚ÂµÃ‚Âµ Ãƒâ€¡Ãƒâ€¡Ãƒâ€ ÃƒÂ¸Ã‚ÂµÃƒâ€° Ã‚Â¼ÃƒÂ¶ Ãƒâ‚¬Ãƒâ€“Ãƒâ‚¬Ã‚Â¸Ã‚Â´ÃƒÂ ÃƒÂÃƒâ€“Ãƒâ‚¬Ãƒâ€¡.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        // Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÅ Ã‚Â¿ÃƒÂ¤ Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ„¢.
                        Effect_SpUp_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;

                // v2.16 2002-5-23 Â°Ã­Â±Â¤Ã‡Ã¶ 
            case DEF_MAGICTYPE_DAMAGE_LINEAR_SPDOWN:
                // Ã€ÃÃÃ·Â¼Â± Â»Ã³Â¿Â¡ Ã€Ã–Â´Ã‚ Â¸Ã±Ã‡Â¥Â¸Â¦ Â¸Ã°ÂµÃŽ Â¾Ã³Â¸Â®Â¸Ã§ Â°Ã¸Â°ÃÃ‡Ã‘Â´Ã™.
                sX = m_pClientList[iClientH]->m_sX;
                sY = m_pClientList[iClientH]->m_sY;

                for (i = 2; i < 10; i++)
                {
                    iErr = 0;
                    m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

                    // tx, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;
                        }
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx-1, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {

                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx+1, ty
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx, ty-1
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    // tx, ty+1
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, sX, sY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;
                        }
                        //
                    }

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;
                            }
                            //
                        }
                    }

                    if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
                }

                // ÃÃ–ÂºÂ¯ Â°Ã¸Â°Ã ÃˆÂ¿Â°Ãº 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // Ã€ÃšÂ½Ã…ÂµÂµ Ã‡Ã‡Ã†Ã¸ÂµÃ‰ Â¼Ã¶ Ã€Ã–Ã€Â¸Â´Ã ÃÃ–Ã€Ã‡.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    {
                                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                        Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                        ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                    }
                                    break;
                            }
                            //
                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                                // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                                switch (cOwnerType)
                                {
                                    case DEF_OWNERTYPE_PLAYER:
                                        if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                        // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                        {
                                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                            Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                            ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                        }
                                        break;

                                    case DEF_OWNERTYPE_NPC:
                                        if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                        {
                                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                            Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                            ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                        }
                                        break;
                                }
                                //
                            }
                        }
                    }

                // dX, dY
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                {
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr); // v1.41 FALSE
                    // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                            }
                            break;
                    }
                    //
                }

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    {
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr); // v1.41 FALSE
                        // Â¾Ã³Â¾Ã®Â¼Â­ ÂµÂ¿Ã€Ã›Ã€ÃŒ Â´ÃŠÂ¾Ã®ÃÃ¶Â´Ã‚ ÃˆÂ¿Â°Ãº
                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                // Ã…Â¸Â°Ã™Ã€ÃŒ Â»Ã¬Â¾Ã†Ã€Ã–Â°Ã­ Â¾Ã³Ã€Â½ Ã€ÃºÃ‡Ã—Â¿Â¡ Â½Ã‡Ã†ÃÃ‡ÃŸÂ´Ã™Â¸Ã© Â¾Ã³Â¾Ã®ÂºÃ™Â´Ã‚Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                                }
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                {
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                    Effect_SpDown_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                                    ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);

                                }
                                break;
                        }
                        //
                    }
                }
                break;

            case DEF_MAGICTYPE_TELEPORT:
                // Ãƒâ€¦ÃƒÅ¡Ã‚Â·Ã‚Â¹Ãƒâ€ ÃƒÂ·Ãƒâ€ Ã‚Â® Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½. sValue 4Ã‚Â¿Ã‚Â¡ Ã‚ÂµÃƒÂ»Ã‚Â¶ÃƒÂ³Ã‚Â¼Ã‚Â­ Ãƒâ€¦ÃƒÅ¡Ã‚Â·Ã‚Â¹Ãƒâ€ ÃƒÂ·Ãƒâ€ Ã‚Â® Ã‚Â¸ÃƒÂ±Ãƒâ‚¬ÃƒÂ»ÃƒÂÃƒÂ¶Ã‚Â°Ã‚Â¡ Ã‚Â°ÃƒÂ¡ÃƒÂÃ‚Â¤Ã‚ÂµÃƒË†Ã‚Â´Ãƒâ„¢.
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:
                        // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ãƒâ‚¬ÃƒÅ’ Ã‚Â¼Ãƒâ€™Ã‚Â¼Ãƒâ€œÃ‚ÂµÃƒË† Ã‚Â¸Ã‚Â¶Ãƒâ‚¬Ã‚Â»Ã‚Â·ÃƒÅ½ Ãƒâ€¦ÃƒÅ¡Ã‚Â·Ã‚Â¹Ãƒâ€ ÃƒÂ·Ãƒâ€ Ã‚Â®. RecallÃƒâ‚¬ÃƒÅ’Ã‚Â´Ãƒâ„¢.
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (sOwnerH == iClientH))
                        {
                            // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ã‚Â¿ÃƒÅ“Ã‚Â¿Ã‚Â¡Ã‚Â´Ãƒâ€š RecallÃƒâ€¡Ãƒâ€™ Ã‚Â¼ÃƒÂ¶ Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ„¢.
                            RequestTeleportHandler(iClientH, "1   ");
                        }
                        break;
                }
                break;

            case DEF_MAGICTYPE_SUMMON:
                // Ã‚Â¼Ãƒâ€™ÃƒË†Ã‚Â¯Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ 

                // Ã‚Â»ÃƒÂ§Ãƒâ€¦ÃƒÂµÃƒâ‚¬ÃƒÂ¥ Ã‚Â³Ã‚Â»Ã‚Â¿Ã‚Â¡Ã‚Â¼Ã‚Â­Ã‚Â´Ãƒâ€š Ã‚Â¼Ãƒâ€™ÃƒË†Ã‚Â¯Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬ÃƒÅ’ Ã‚ÂºÃƒâ€™Ã‚Â°Ã‚Â¡Ã‚Â´Ãƒâ€°.
                if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == TRUE) return;

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                // ÃƒÂÃƒÂ¶ÃƒÂÃ‚Â¤Ã‚ÂµÃƒË† OwnerÃ‚Â°Ã‚Â¡ MasterÃ‚Â°Ã‚Â¡ Ã‚ÂµÃƒË†Ã‚Â´Ãƒâ„¢. 
                if ((sOwnerH != NULL) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
                {
                    // MasterÃ‚Â·ÃƒÅ½ ÃƒÂÃƒÂ¶ÃƒÂÃ‚Â¤Ã‚ÂµÃƒË† Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬Ã‚Â» Ã‚ÂµÃƒÂ»Ã‚Â¶ÃƒÂ³Ã‚Â´Ãƒâ„¢Ã‚Â´ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ã‚Â°Ã‚Â´ÃƒÆ’Ã‚Â¼ Ã‚Â¼ÃƒÂ¶Ã‚Â¸Ã‚Â¦ Ã‚Â°ÃƒÂ¨Ã‚Â»ÃƒÂªÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢. 
                    iFollowersNum = iGetFollowerNumber(sOwnerH, cOwnerType);

                    // Ã‚Â¼Ãƒâ€™ÃƒË†Ã‚Â¯Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬Ã‚Â» CastingÃƒâ€¡Ãƒâ€˜ Ãƒâ‚¬ÃƒÅ¡Ãƒâ‚¬Ãƒâ€¡ Magery/20 Ã‚Â¸Ã‚Â¸Ãƒâ€¦Ã‚Â­Ãƒâ‚¬Ãƒâ€¡ Ã‚Â¸ÃƒÂ³Ã‚Â½Ã‚ÂºÃƒâ€¦ÃƒÂÃ‚Â¸Ã‚Â¦ Ã‚Â¼Ãƒâ€™ÃƒË†Ã‚Â¯Ãƒâ€¡Ãƒâ€™ Ã‚Â¼ÃƒÂ¶ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ„¢.
                    if (iFollowersNum >= (m_pClientList[iClientH]->m_cSkillMastery[4] / 20)) break;

                    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
                    if (iNamingValue == -1)
                    {
                        // Ã‚Â´ÃƒÂµÃƒâ‚¬ÃƒÅ’Ã‚Â»ÃƒÂ³ Ãƒâ‚¬ÃƒÅ’ Ã‚Â¸ÃƒÅ Ã‚Â¿Ã‚Â¡ NPCÃ‚Â¸Ã‚Â¦ Ã‚Â¸Ã‚Â¸Ã‚ÂµÃƒÂ©Ã‚Â¼ÃƒÂ¶ Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ„¢. Ãƒâ‚¬ÃƒÅ’Ã‚Â¸Ã‚Â§Ãƒâ‚¬Ã‚Â» Ãƒâ€¡Ãƒâ€™Ã‚Â´ÃƒÂ§Ãƒâ€¡Ãƒâ€™ Ã‚Â¼ÃƒÂ¶ Ã‚Â¾ÃƒÂ¸Ã‚Â±ÃƒÂ¢ Ã‚Â¶Ã‚Â§Ã‚Â¹Ã‚Â®.
                    }
                    else
                    {
                        // NPCÃ‚Â¸Ã‚Â¦ Ã‚Â»ÃƒÂ½Ã‚Â¼Ã‚ÂºÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                        ZeroMemory(cName, sizeof(cName));
                        wsprintf(cName, "XX%d", iNamingValue);
                        cName[0] = '_';
                        cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

                        // MageryÃ‚Â¿Ã‚Â¡ Ã‚ÂµÃƒÂ»Ã‚Â¶ÃƒÂ³ Ã‚Â¼Ãƒâ€™ÃƒË†Ã‚Â¯Ã‚ÂµÃƒâ€¡Ã‚Â´Ãƒâ€š Ã‚Â¸ÃƒÂ³Ã‚Â½Ã‚ÂºÃƒâ€¦ÃƒÂÃƒâ‚¬Ãƒâ€¡ Ã‚ÂµÃƒÂ®Ã‚Â±ÃƒÅ¾Ãƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ¾Ã‚Â¶ÃƒÂ³ÃƒÂÃƒÂ¸Ã‚Â´Ãƒâ„¢.
                        ZeroMemory(cNpcName, sizeof(cNpcName));

                        switch (iV1)
                        {
                            case NULL: // Ãƒâ‚¬ÃƒÂÃ‚Â¹ÃƒÂÃƒâ‚¬ÃƒÂ»Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ 
                                iResult = iDice(1, m_pClientList[iClientH]->m_cSkillMastery[4] / 10);

                                // v1.42 ÃƒÆ’Ãƒâ€“Ãƒâ‚¬ÃƒÂº Ã‚Â¸ÃƒÂ· Ã‚Â·Ã‚Â¹Ã‚ÂºÃ‚Â§Ãƒâ‚¬Ã‚Â» Ãƒâ‚¬Ãƒâ€Ã‚Â·Ãƒâ€š 
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

                        if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM, &dX, &dY, cNpcWaypoint, NULL, NULL, m_pClientList[iClientH]->m_cSide, FALSE, TRUE) == FALSE)
                        {
                            // Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ãƒâ‚¬Ã‚Â¸Ã‚Â¹Ãƒâ€¡Ã‚Â·ÃƒÅ½ Ã‚Â¿Ã‚Â¹Ã‚Â¾ÃƒÂ Ã‚ÂµÃƒË† NameValueÃ‚Â¸Ã‚Â¦ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚Â½ÃƒÆ’Ãƒâ€¦Ã‚Â²Ã‚Â´Ãƒâ„¢.
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                        }
                        else
                        {
                            ZeroMemory(cName_Master, sizeof(cName_Master));
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
                        }
                    }
                }
                break;

            case DEF_MAGICTYPE_CREATE:
                // Ã‚Â¹Ã‚Â«Ã‚Â¾ÃƒÂ°Ã‚Â°Ã‚Â¡Ã‚Â¸Ã‚Â¦ Ã‚Â»ÃƒÂ½Ã‚Â¼Ã‚ÂºÃ‚Â½ÃƒÆ’Ãƒâ€¦Ã‚Â°Ã‚Â´Ãƒâ€š Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ 

                // Ãƒâ‚¬Ã‚Â§Ãƒâ€žÃ‚Â¡Ãƒâ€¡Ãƒâ€™ Ã‚Â¼ÃƒÂ¶ Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ€š Ã‚Â°ÃƒÂ·Ã‚Â¿Ã‚Â¡Ã‚Â´Ãƒâ€š Ã‚Â»ÃƒÂ½Ã‚Â±ÃƒÂ¢ÃƒÂÃƒÂ¶ Ã‚Â¾ÃƒÅ Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢. 
                if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetIsMoveAllowedTile(dX, dY) == FALSE)
                    goto MAGIC_NOEFFECT;

                pItem = new CItem;

                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:
                        // Food
                        if (iDice(1, 2) == 1)
                            wsprintf(cItemName, "Meat");
                        else wsprintf(cItemName, "Baguette");
                        break;
                }

                _bInitItemAttr(pItem, cItemName);

                // v2.15 Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬Ã‚Â¸Ã‚Â·ÃƒÅ½ Ã‚Â»ÃƒÂ½Ã‚Â±ÃƒÂ¤ Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃ‚Â¿Ã‚Â¡ Ã‚Â°ÃƒÂ­Ãƒâ‚¬Ã‚Â¯Ã‚Â¹ÃƒÂ¸ÃƒË†Ã‚Â£ Ãƒâ‚¬ÃƒÂºÃƒâ‚¬ÃƒÂ¥ 
                pItem->m_sTouchEffectType = DEF_ITET_ID;
                pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                pItem->m_sTouchEffectValue2 = iDice(1, 100000);
                pItem->m_sTouchEffectValue3 = (short)timeGetTime();

                // Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» Ã‚Â¼Ã‚Â­Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ‚¬Ã‚Â§Ãƒâ€žÃ‚Â¡Ã‚Â¿Ã‚Â¡ Ã‚Â¹ÃƒÂ¶Ã‚Â¸Ã‚Â°Ã‚Â´Ãƒâ„¢. 
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(dX, dY, pItem);

                // v1.41 ÃƒË†ÃƒÂ±Ã‚Â±ÃƒÂ Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» Ã‚Â¶Ã‚Â³Ã‚Â¾ÃƒÂ®Ã‚Â¶ÃƒÅ¸Ã‚Â¸Ã‚Â° Ã‚Â°ÃƒÂÃƒâ‚¬ÃƒÅ’Ã‚Â¶ÃƒÂ³Ã‚Â¸ÃƒÂ© Ã‚Â·ÃƒÅ½Ã‚Â±Ãƒâ€”Ã‚Â¸Ã‚Â¦ Ã‚Â³Ã‚Â²Ã‚Â±ÃƒÂ¤Ã‚Â´Ãƒâ„¢. 
                _bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);

                // Ã‚Â´Ãƒâ„¢Ã‚Â¸Ã‚Â¥ Ãƒâ€¦Ã‚Â¬Ã‚Â¶ÃƒÂ³Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ°Ãƒâ€ Ã‚Â®Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€ Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â¶Ã‚Â³Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¸ Ã‚Â°ÃƒÂÃƒâ‚¬Ã‚Â» Ã‚Â¾Ãƒâ€¹Ã‚Â¸Ã‚Â°Ã‚Â´Ãƒâ„¢. 
                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
                    dX, dY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color
                break;

            case DEF_MAGICTYPE_PROTECT:
                // Ã‚ÂºÃ‚Â¸ÃƒË†Ã‚Â£ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ 
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                // Ã‚ÂºÃ‚Â¸ÃƒË†Ã‚Â£ Ã‚Â»ÃƒÂ³Ãƒâ€¦Ãƒâ€šÃ‚Â¶ÃƒÂ³Ã‚Â´Ãƒâ€š Ã‚Â°ÃƒÂÃƒâ‚¬Ã‚Â» Ã‚Â¼Ã‚Â³ÃƒÂÃ‚Â¤Ãƒâ€¡ÃƒÂÃ‚Â±ÃƒÂ¢ Ãƒâ‚¬ÃƒÂ¼Ã‚Â¿Ã‚Â¡ Ãƒâ‚¬ÃƒÅ’Ã‚Â¹ÃƒÅ’ Ãƒâ€¡ÃƒËœÃ‚Â´ÃƒÂ§ Ã‚ÂºÃ‚Â¸ÃƒË†Ã‚Â£Ã‚Â°Ã‚Â¡ Ã‚Â°Ãƒâ€°Ã‚Â·ÃƒÂÃƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€šÃƒÂÃƒÂ¶ ÃƒË†Ã‚Â®Ãƒâ‚¬ÃƒÅ½Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ã‚Â°Ãƒâ€°Ã‚Â·ÃƒÂÃƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬Ã‚Âº Ã‚Â¹Ã‚Â«Ã‚Â½ÃƒÆ’Ã‚ÂµÃƒË†Ã‚Â´Ãƒâ„¢. 
                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0) goto MAGIC_NOEFFECT;
                        // v1.4334 ÃƒÂÃƒÅ¸Ã‚Â¸Ã‚Â³Ãƒâ‚¬Ã‚Âº Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ¦Ãƒâ‚¬Ã‚Â» Ã‚Â¸ÃƒÂ¸Ãƒâ€¡ÃƒÂÃ‚Â°Ãƒâ€ Ã‚Â¼ÃƒÂ¶ÃƒÂÃ‚Â¤
                        if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) goto MAGIC_NOEFFECT;

                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                        switch (m_pMagicConfigList[sType]->m_sValue4)
                        {
                            case 1:
                                SetProtectionFromArrowFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, TRUE);
                                break;
                            case 2:
                            case 5:
                                SetMagicProtectionFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, TRUE);
                                break;
                            case 3:
                            case 4:
                                SetDefenseShieldFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, TRUE);
                                break;
                        }
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0) goto MAGIC_NOEFFECT;
                        // ÃƒÂÃ‚Â¤Ãƒâ‚¬ÃƒÂ» NPCÃ‚ÂµÃƒÂ©Ãƒâ‚¬Ã‚Âº Ã‚ÂºÃ‚Â¸ÃƒË†Ã‚Â£ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ãƒâ‚¬ÃƒÂ»Ã‚Â¿ÃƒÂ« Ã‚Â¾ÃƒË†Ã‚ÂµÃƒÅ .				
                        if (m_pNpcList[sOwnerH]->m_cActionLimit != 0) goto MAGIC_NOEFFECT;
                        m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = (char)m_pMagicConfigList[sType]->m_sValue4;

                        switch (m_pMagicConfigList[sType]->m_sValue4)
                        {
                            case 1:
                                SetProtectionFromArrowFlag(sOwnerH, DEF_OWNERTYPE_NPC, TRUE);
                                break;
                            case 2:
                            case 5:
                                SetMagicProtectionFlag(sOwnerH, DEF_OWNERTYPE_NPC, TRUE);
                                break;
                            case 3:
                            case 4:
                                SetDefenseShieldFlag(sOwnerH, DEF_OWNERTYPE_NPC, TRUE);
                                break;
                        }
                        break;
                }

                // Ã‚ÂºÃ‚Â¸ÃƒË†Ã‚Â£ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_PROTECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                    sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ã‚Â»ÃƒÂ½Ã‚Â°ÃƒÂ¥Ãƒâ‚¬Ã‚Â½Ãƒâ‚¬Ã‚Â» Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_PROTECT, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                break;

            case DEF_MAGICTYPE_SCAN:
                ZeroMemory(cScanMessage, sizeof(cScanMessage));
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                {
                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            wsprintf(cScanMessage, " Player: %s HP:%d MP:%d.", m_pClientList[sOwnerH]->m_cCharName, m_pClientList[sOwnerH]->m_iHP, m_pClientList[sOwnerH]->m_iMP);
                            ShowClientMsg(iClientH, cScanMessage);
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            wsprintf(cScanMessage, " NPC: %s HP:%d MP:%d", m_pNpcList[sOwnerH]->m_cNpcName, m_pNpcList[sOwnerH]->m_iHP, m_pNpcList[sOwnerH]->m_iMana);
                            ShowClientMsg(iClientH, cScanMessage);
                            break;
                    }
                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, dX, dY, 10, 10);
                }
                break;

            case DEF_MAGICTYPE_HOLDOBJECT:
                // Ã‚Â¿Ãƒâ‚¬Ã‚ÂºÃƒÂªÃƒÂÃ‚Â§Ãƒâ€ Ã‚Â®Ãƒâ‚¬Ãƒâ€¡ Ã‚Â¿ÃƒÂ²ÃƒÂÃƒÂ·Ãƒâ‚¬Ãƒâ€œÃƒâ‚¬Ã‚Â» Ã‚ÂºÃƒâ‚¬Ã‚Â¼ÃƒÂ¢Ãƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢. 
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                {

                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto MAGIC_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_iAddPR >= 500) goto MAGIC_NOEFFECT;
                            // v1.4334 ÃƒÂÃƒÅ¸Ã‚Â¸Ã‚Â³Ãƒâ‚¬Ã‚Âº Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€ Ãƒâ€ ÃƒÂÃ‚Â·Ã‚Â²Ãƒâ‚¬Ã‚Â» Ã‚Â¸ÃƒÂ¸Ãƒâ€¡ÃƒÂÃ‚Â°Ãƒâ€ Ã‚Â¼ÃƒÂ¶ÃƒÂÃ‚Â¤
                            if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) goto MAGIC_NOEFFECT;
                            // 2002-09-10 #2 Ã‚Â¾ÃƒË†Ãƒâ‚¬ÃƒÂ¼ÃƒÂÃƒÂ¶Ã‚Â´ÃƒÂ«(No-Attack-Area) Ãƒâ€ ÃƒÂÃ‚Â·Ã‚Â² Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ã‚Â¾ÃƒË†Ã‚ÂµÃƒâ€¡Ã‚Â°Ãƒâ€ Ãƒâ€¡Ãƒâ€
                            if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            {

                                if (m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->iGetAttribute(sX, sY, 0x00000006) != 0) goto MAGIC_NOEFFECT;
                                if (m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->iGetAttribute(dX, dY, 0x00000006) != 0) goto MAGIC_NOEFFECT;
                            }

                            // 2002-09-10 #3 Ãƒâ‚¬ÃƒÂ¼Ã‚Â¸ÃƒÂ©Ãƒâ‚¬ÃƒÂ¼Ã‚Â½ÃƒÆ’Ã‚Â¿ÃƒÂ Ã‚Â¹ÃƒÅ’Ã‚ÂµÃƒÂ©Ã‚Â·Ã‚Â£Ã‚ÂµÃƒÂ¥Ã‚Â¿Ã‚Â¡Ã‚Â¼Ã‚Â­Ã‚Â¸Ã‚Â¦ ÃƒÂÃ‚Â¦Ã‚Â¿ÃƒÅ“Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ã‚Â°Ã‚Â°Ãƒâ‚¬Ã‚Âº Ãƒâ€ ÃƒÂ­Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€Ã‚Â´Ãƒâ€š Ãƒâ€ ÃƒÂÃ‚Â·Ã‚Â² Ã‚Â¾ÃƒË†Ã‚ÂµÃƒÅ 
                            if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "middleland") != 0 &&
                                m_bIsCrusadeMode == FALSE &&
                                m_pClientList[iClientH]->m_cSide == m_pClientList[sOwnerH]->m_cSide)
                                goto MAGIC_NOEFFECT;

                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicLevel >= 6) goto MAGIC_NOEFFECT; // v1.4 Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ã‚Â·Ã‚Â¹Ã‚ÂºÃ‚Â§ 6Ãƒâ‚¬ÃƒÅ’Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ½ Ã‚Â¸Ã‚Â®Ãƒâ€žÃ‚Â¡Ã‚Â±ÃƒÅ¾ Ãƒâ‚¬ÃƒÅ’Ã‚Â»ÃƒÂ³Ãƒâ‚¬Ãƒâ€¡ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ã‚Â¸ÃƒÂ³Ã‚Â½Ã‚ÂºÃƒâ€¦ÃƒÂÃ‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€Ã‚Â´Ãƒâ€š Ã‚Â¸Ã‚Â¶Ã‚ÂºÃƒÂ± Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ãƒâ€¦ÃƒÂ«Ãƒâ€¡ÃƒÂÃƒÂÃƒÂ¶ Ã‚Â¾ÃƒÅ Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto MAGIC_NOEFFECT;
                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;
                    }

                    // Ã‚ÂºÃ‚Â¸ÃƒË†Ã‚Â£ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ã‚Â»ÃƒÂ½Ã‚Â°ÃƒÂ¥Ãƒâ‚¬Ã‚Â½Ãƒâ‚¬Ã‚Â» Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_HOLDOBJECT, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
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
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if ((sOwnerH != iClientH) && ((memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0) || (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0)) && ((memcmp(m_pClientList[sOwnerH]->m_cLocation, "elvhunter", 9) != 0) || (memcmp(m_pClientList[sOwnerH]->m_cLocation, "arehunter", 9) != 0))) goto MAGIC_NOEFFECT;
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto MAGIC_NOEFFECT;
                                if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) goto MAGIC_NOEFFECT;

                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetInvisibilityFlag(sOwnerH, cOwnerType, TRUE);
                                RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto MAGIC_NOEFFECT;

                                if (m_pNpcList[sOwnerH]->m_cActionLimit == 0)
                                {
                                    // Ãƒâ‚¬ÃƒÅ’Ã‚ÂµÃ‚Â¿Ãƒâ€¡ÃƒÂÃƒÂÃƒÂ¶ Ã‚Â¾ÃƒÅ Ã‚Â´Ãƒâ€š NPCÃ‚Â´Ãƒâ€š Ãƒâ€¦ÃƒÂµÃ‚Â¸ÃƒÂ­ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬Ã‚Â» Ã‚Â°Ãƒâ€° Ã‚Â¼ÃƒÂ¶ Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ„¢.
                                    m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                    SetInvisibilityFlag(sOwnerH, cOwnerType, TRUE);
                                    // Ãƒâ‚¬ÃƒÅ’ NPCÃ‚Â¸Ã‚Â¦ ÃƒÆ’ÃƒÅ¸Ãƒâ‚¬ÃƒÂ»Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´ÃƒÂ¸ Ã‚Â¸ÃƒÂ³Ã‚Â½Ã‚ÂºÃƒâ€¦ÃƒÂÃ‚Â¸Ã‚Â¦ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚Â½ÃƒÆ’Ãƒâ€¦Ã‚Â²Ã‚Â´Ãƒâ„¢.
                                    RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_INVISIBILITY);
                                }
                                break;
                        }

                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_INVISIBILITY, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                            sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                        if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_INVISIBILITY, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                        break;

                    case 2:
                        // v1.4334 ÃƒÂÃƒÅ¸Ã‚Â¸Ã‚Â³Ãƒâ‚¬Ã‚Âº Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€ ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ãƒâ‚¬Ã‚Â» Ã‚Â¸ÃƒÂ¸Ãƒâ€¡ÃƒÂÃ‚Â°Ãƒâ€ Ã‚Â¼ÃƒÂ¶ÃƒÂÃ‚Â¤
                        if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) goto MAGIC_NOEFFECT;
                        if ((memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0) || (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0)) goto MAGIC_NOEFFECT;

                        // dX, dY Ã‚Â¹ÃƒÂÃ‚Â°ÃƒÂ¦ 8 ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯Ãƒâ‚¬Ãƒâ€¡ Invisibility Ã‚Â»ÃƒÂ³Ãƒâ€¦Ãƒâ€šÃƒâ‚¬ÃƒÅ½ ObjectÃ‚Â°Ã‚Â¡ Ãƒâ‚¬Ãƒâ€“Ãƒâ‚¬Ã‚Â¸Ã‚Â¸ÃƒÂ© Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦ Ã‚Â½ÃƒÆ’Ãƒâ€¦Ã‚Â²Ã‚Â´Ãƒâ„¢.
                        for (ix = dX - 8; ix <= dX + 8; ix++)
                            for (iy = dY - 8; iy <= dY + 8; iy++)
                            {
                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (sOwnerH != NULL)
                                {
                                    switch (cOwnerType)
                                    {
                                        case DEF_OWNERTYPE_PLAYER:
                                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != NULL)
                                            {
                                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
                                                SetInvisibilityFlag(sOwnerH, cOwnerType, FALSE);
                                                bRemoveFromDelayEventList(sOwnerH, cOwnerType, DEF_MAGICTYPE_INVISIBILITY);
                                            }
                                            break;

                                        case DEF_OWNERTYPE_NPC:
                                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != NULL)
                                            {
                                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
                                                SetInvisibilityFlag(sOwnerH, cOwnerType, FALSE);
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
                // Dynamic ObjectÃ‚Â¸Ã‚Â¦ Ã‚Â»ÃƒÂ½Ã‚Â¼Ã‚ÂºÃƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ ÃƒË†Ã‚Â¿Ã‚Â·Ãƒâ€šÃƒâ‚¬ÃƒÅ’ ÃƒÂÃƒÂ¶Ã‚Â¼Ãƒâ€œÃ‚ÂµÃƒâ€¡Ã‚Â´Ãƒâ€š Ãƒâ€¦Ã‚Â¸Ãƒâ‚¬Ãƒâ€.

                // v2.1 Ã‚Â¸Ã‚Â¶Ãƒâ‚¬Ã‚Â» Ã‚Â³Ã‚Â»Ã‚Â¿Ã‚Â¡Ã‚Â¼Ã‚Â­Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÅ Ã‚ÂµÃƒÂ¥ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ã‚Â±ÃƒÂÃƒÂÃƒÂ¶(Ãƒâ‚¬ÃƒÂ¼Ã‚Â¸ÃƒÂ©Ãƒâ‚¬ÃƒÂ¼Ã‚Â½ÃƒÆ’ ÃƒÂÃ‚Â¦Ã‚Â¿ÃƒÅ“)
                if (m_bIsCrusadeMode == FALSE)
                {
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresden") == 0) return;
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvine") == 0) return;
                    // v2.14
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "arefarm") == 0) return;
                    if (strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvfarm") == 0) return;
                }

                switch (m_pMagicConfigList[sType]->m_sValue10)
                {
                    case DEF_DYNAMICOBJECT_PCLOUD_BEGIN: // Ã‚ÂµÃ‚Â¶Ã‚Â±Ã‚Â¸Ã‚Â¸Ã‚Â§

                    case DEF_DYNAMICOBJECT_FIRE:   // Fire Ãƒâ‚¬ÃƒÅ’Ã‚Â´Ãƒâ„¢.
                    case DEF_DYNAMICOBJECT_SPIKE:  // Spike

#ifdef DEF_TAIWANLOG 
                        short sTemp_X, sTemp_Y;
                        // v2.15 Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬Ã‚Â» Ã‚Â»ÃƒÂ§Ã‚Â¿ÃƒÂ«Ãƒâ€¡Ãƒâ€˜ Ãƒâ‚¬Ã‚Â§Ãƒâ€žÃ‚Â¡Ã‚Â¸Ã‚Â¦ Ãƒâ€¡Ã‚Â¥Ã‚Â½ÃƒÆ’Ãƒâ€¡ÃƒÂÃ‚Â±ÃƒÂ¢ Ãƒâ‚¬Ã‚Â§Ãƒâ€¡Ãƒâ€˜ Ã‚ÂºÃ‚Â¯Ã‚Â¼ÃƒÂ¶
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
                                BOOL bFlag = FALSE;
                                int cx, cy;
                                for (ix = dX - m_pMagicConfigList[sType]->m_sValue12; ix <= dX + m_pMagicConfigList[sType]->m_sValue12; ix++)
                                    for (iy = dY - m_pMagicConfigList[sType]->m_sValue12; iy <= dY + m_pMagicConfigList[sType]->m_sValue12; iy++)
                                    {
                                        iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, m_pMagicConfigList[sType]->m_sValue10, m_pClientList[iClientH]->m_cMapIndex,
                                            ix, iy, m_pMagicConfigList[sType]->m_dwLastTime * 1000, m_pMagicConfigList[sType]->m_sValue5);

                                        // Ã‚Â¸Ã‚Â¸Ã‚Â¾ÃƒÂ  Ã‚Â¸Ã‚Â¶Ãƒâ‚¬Ã‚Â»Ã‚Â¿Ã‚Â¡Ã‚Â¼Ã‚Â­ Ãƒâ€¡ÃƒÅ Ã‚ÂµÃƒÂ¥Ã‚Â¸Ã‚Â¦ Ã‚Â±ÃƒÂ± Ã‚Â°ÃƒÂ·Ã‚Â¿Ã‚Â¡ Ã‚Â¹Ã‚Â«Ã‚Â°ÃƒÂ­Ãƒâ€¡Ãƒâ€˜ Ãƒâ‚¬ÃƒÅ¡Ã‚Â°Ã‚Â¡ Ãƒâ‚¬Ãƒâ€“Ã‚Â¾ÃƒÂºÃ‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂÃƒâ‚¬ÃƒÅ¡Ã‚Â´Ãƒâ€š Ã‚Â°Ã‚Â¡Ã‚ÂµÃƒÂ¥Ãƒâ‚¬Ãƒâ€¡ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂÃƒâ‚¬Ã‚Â» Ã‚Â¹ÃƒÅ¾Ã‚Â°Ãƒâ€ Ã‚ÂµÃƒË†Ã‚Â´Ãƒâ„¢. 
                                        if (bAnalyzeCriminalAction(iClientH, ix, iy, TRUE) == TRUE)
                                        {
                                            bFlag = TRUE;
                                            cx = ix;
                                            cy = iy;
                                        }
                                    }
                                // Ãƒâ€¡ÃƒÅ Ã‚ÂµÃƒÂ¥Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â°ÃƒÂ¦Ã‚ÂºÃƒÂ±Ã‚Â¸Ã‚Â¦ 1Ã‚Â¸ÃƒÂ­Ã‚Â¸Ã‚Â¸ Ã‚Â¼Ãƒâ€™ÃƒË†Ã‚Â¯Ãƒâ€¡ÃƒÂÃ‚Â±ÃƒÂ¢ Ãƒâ‚¬Ã‚Â§Ãƒâ€¡Ãƒâ€.
                                if (bFlag == TRUE) bAnalyzeCriminalAction(iClientH, cx, cy);
                                break;
                        }
                        //
                        break;

                    case DEF_DYNAMICOBJECT_ICESTORM:
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
                // Ã‚Â¿ÃƒÂ¸Ã‚Â°Ãƒâ€¦Ã‚Â¸Ã‚Â®Ã‚Â¿Ã‚Â¡ Ã‚Â¶Ã‚Â³Ã‚Â¾ÃƒÂ®ÃƒÂÃ‚Â® Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ã‚Â¹Ã‚Â°Ã‚Â°Ãƒâ€¡Ãƒâ‚¬Ã‚Â» ÃƒÂÃƒÂ½Ã‚Â¾ÃƒÂ®Ã‚Â¿Ãƒâ‚¬Ã‚Â´Ãƒâ€š Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬ÃƒÅ’Ã‚Â´Ãƒâ„¢. 
                // v2.12 ÃƒÂÃƒÅ¸Ã‚Â¸Ã‚Â³Ãƒâ‚¬Ã‚Âº Ãƒâ€ ÃƒÂ·ÃƒÂÃ‚Â¦Ã‚Â¼Ãƒâ€¡ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ã‚Â»ÃƒÂ§Ã‚Â¿ÃƒÂ« Ã‚ÂºÃƒâ€™Ã‚Â°Ã‚Â¡ 
                if (m_pClientList[iClientH]->m_cSide == NULL) goto MAGIC_NOEFFECT;

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (sOwnerH != NULL) break; // v1.41 Ãƒâ€ ÃƒÂ·ÃƒÂÃ‚Â¦Ã‚Â¼Ãƒâ€¡ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ãƒâ‚¬Ã‚Âº Ã‚Â»ÃƒÂ§Ã‚Â¶ÃƒÂ·Ãƒâ‚¬ÃƒÅ’ Ã‚Â¼Ã‚Â­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ‚¬Ã‚Â§Ã‚Â¿Ã‚Â¡Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â·Ãƒâ€šÃƒâ‚¬ÃƒÅ’ Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ„¢. 

                pItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(dX, dY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);
                if (pItem != NULL)
                {
                    // Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â°Ã‚Â¡ Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» ÃƒË†Ã‚Â¹Ã‚ÂµÃƒÂ¦Ãƒâ€¡ÃƒÂÃ‚Â¿Ã‚Â´Ã‚Â´Ãƒâ„¢. 
                    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
                    {
                        // Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» ÃƒË†Ã‚Â¹Ã‚ÂµÃƒÂ¦Ãƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢.

                        // v1.411 Ã‚Â·ÃƒÅ½Ã‚Â±Ãƒâ€” Ã‚Â³Ã‚Â²Ã‚Â±ÃƒÂ¤Ã‚Â´Ãƒâ„¢.
                        _bItemLog(DEF_ITEMLOG_GET, iClientH, (int)-1, pItem);

                        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                        *dwp = MSGID_NOTIFY;
                        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                        *wp = DEF_NOTIFY_ITEMOBTAINED;

                        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

                        // 1Ã‚Â°Ã‚Â³ ÃƒË†Ã‚Â¹Ã‚ÂµÃƒÂ¦Ãƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢. <- Ã‚Â¿Ã‚Â©Ã‚Â±ÃƒÂ¢Ã‚Â¼Ã‚Â­ 1Ã‚Â°Ã‚Â³Ã‚Â¶ÃƒÂµ Ãƒâ€žÃ‚Â«Ã‚Â¿ÃƒÂ®Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚Â¸Ã‚Â»Ãƒâ€¡ÃƒÂÃ‚Â´Ãƒâ€š Ã‚Â°ÃƒÂÃƒâ‚¬ÃƒÅ’ Ã‚Â¾Ãƒâ€ Ã‚Â´ÃƒÂÃ‚Â´Ãƒâ„¢
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

                        *cp = (char)0; // Ã‚Â¾ÃƒÂ²Ãƒâ‚¬Ã‚Âº Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬ÃƒÅ’Ã‚Â¹Ãƒâ€¡Ã‚Â·ÃƒÅ½ Ãƒâ‚¬ÃƒÂ¥Ãƒâ€šÃƒÂ¸Ã‚ÂµÃƒâ€¡ÃƒÂÃƒÂ¶ Ã‚Â¾ÃƒÅ Ã‚Â¾Ãƒâ€™Ã‚Â´Ãƒâ„¢.
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

                        *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
                        cp++;

                        dwp = (DWORD *)cp;
                        *dwp = pItem->m_dwAttribute;
                        cp += 4;
                        /*
                        *cp = (char)(pItem->m_dwAttribute & 0x00000001); // Custom-ItemÃƒâ‚¬ÃƒÅ½ÃƒÂÃƒÂ¶Ãƒâ‚¬Ãƒâ€¡ Ã‚Â¿Ã‚Â©Ã‚ÂºÃƒÅ½
                        cp++;
                        */

                        if (iEraseReq == 1) delete pItem;

                        // Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» ÃƒÂÃƒÂÃ‚Â°ÃƒÂ­Ã‚Â³Ã‚Â­ ÃƒË†Ãƒâ€ž Ã‚Â³Ã‚Â²Ãƒâ‚¬Ã‚Âº Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» Ã‚Â´Ãƒâ„¢Ã‚Â¸Ã‚Â¥ Ãƒâ€¦Ã‚Â¬Ã‚Â¶ÃƒÂ³Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ°Ãƒâ€ Ã‚Â®Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€ Ã‚Â¾Ãƒâ€¹Ã‚Â¸Ã‚Â°Ã‚Â´Ãƒâ„¢. 
                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, m_pClientList[iClientH]->m_cMapIndex,
                            dX, dY, sRemainItemSprite, sRemainItemSpriteFrame, cRemainItemColor); // v1.4

                        // Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€º ÃƒÂÃ‚Â¤Ã‚ÂºÃ‚Â¸ Ãƒâ‚¬ÃƒÂ¼Ã‚Â¼Ãƒâ€º 
                        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

                        switch (iRet)
                        {
                            case DEF_XSOCKEVENT_QUENEFULL:
                            case DEF_XSOCKEVENT_SOCKETERROR:
                            case DEF_XSOCKEVENT_CRITICALERROR:
                            case DEF_XSOCKEVENT_SOCKETCLOSED:
                                // Ã‚Â¸ÃƒÅ¾Ã‚Â½ÃƒÆ’ÃƒÂÃƒÂ¶Ã‚Â¸Ã‚Â¦ Ã‚ÂºÃ‚Â¸Ã‚Â³Ã‚Â¾Ã‚Â¶Ã‚Â§ Ã‚Â¿Ã‚Â¡Ã‚Â·Ã‚Â¯Ã‚Â°Ã‚Â¡ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© ÃƒÂÃ‚Â¦Ã‚Â°Ãƒâ€¦Ãƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                DeleteClient(iClientH, TRUE, TRUE);
                                return;
                        }
                    }
                    else
                    {
                        // Ã‚Â°ÃƒÂ¸Ã‚Â°Ã‚Â£Ãƒâ‚¬ÃƒÅ’ Ã‚ÂºÃƒÅ½ÃƒÂÃ‚Â·Ãƒâ€¡ÃƒÂÃ‚Â°Ãƒâ€¦Ã‚Â³Ã‚Âª Ãƒâ€¡Ãƒâ€˜Ã‚Â°ÃƒÂ¨ÃƒÂÃƒÅ¸Ã‚Â·Ã‚Â®Ãƒâ‚¬Ã‚Â» ÃƒÆ’ÃƒÅ Ã‚Â°ÃƒÂºÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢. Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» Ã‚Â¾ÃƒÂ²Ãƒâ‚¬Ã‚Â» Ã‚Â¼ÃƒÂ¶ Ã‚Â¾ÃƒÂ¸Ã‚Â´Ãƒâ„¢.

                        // Ã‚Â°Ã‚Â¡ÃƒÂÃ‚Â®Ã‚Â¿Ãƒâ€Ã‚Â´ÃƒÂ¸ Ã‚Â¾Ãƒâ€ Ãƒâ‚¬ÃƒÅ’Ãƒâ€¦Ãƒâ€ºÃƒâ‚¬Ã‚Â» Ã‚Â¿ÃƒÂ¸Ã‚Â»ÃƒÂ³ÃƒË†Ã‚Â¸Ã‚ÂºÃ‚Â¹Ã‚Â½ÃƒÆ’Ãƒâ€¦Ã‚Â²Ã‚Â´Ãƒâ„¢. 
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(dX, dY, pItem);

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
                                // Ã‚Â¸ÃƒÅ¾Ã‚Â½ÃƒÆ’ÃƒÂÃƒÂ¶Ã‚Â¸Ã‚Â¦ Ã‚ÂºÃ‚Â¸Ã‚Â³Ã‚Â¾Ã‚Â¶Ã‚Â§ Ã‚Â¿Ã‚Â¡Ã‚Â·Ã‚Â¯Ã‚Â°Ã‚Â¡ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© ÃƒÂÃ‚Â¦Ã‚Â°Ãƒâ€¦Ãƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                DeleteClient(iClientH, TRUE, TRUE);
                                return;
                        }
                    }
                }
                //
                break;

            case DEF_MAGICTYPE_CONFUSE:
                // if the caster side is the same as the targets side, no effect occurs
                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1: // confuse LanguageÃƒÂ¬Ã‚ÂÃ‚Â´ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤. 
                    case 2: // Confusion, Mass Confusion 	
                        for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                            for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                            {
                                // ÃƒÂ¬Ã…Â¾Ã‚ÂÃƒÂ¬Ã¢â‚¬Â¹Ã‚Â ÃƒÂ«Ã‚ÂÃ¢â‚¬Å¾ ÃƒÂ­Ã¢â‚¬ÂÃ‚Â¼ÃƒÂ­Ã‚ÂÃ‚Â­ÃƒÂ«Ã‚ÂÃ‚Â  ÃƒÂ¬Ã‹â€ Ã‹Å“ ÃƒÂ¬Ã…Â¾Ã‹â€ ÃƒÂ¬Ã…â€œÃ‚Â¼ÃƒÂ«Ã¢â‚¬Â¹Ã‹â€  ÃƒÂ¬Ã‚Â£Ã‚Â¼ÃƒÂ¬Ã‚ÂÃ‹Å“.
                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                                {
                                    // ÃƒÂ­Ã¢â‚¬Â¢Ã‚Â´ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¹ ÃƒÂ¬Ã…â€œÃ¢â‚¬Å¾ÃƒÂ¬Ã‚Â¹Ã‹Å“ÃƒÂ¬Ã¢â‚¬â€Ã‚Â ÃƒÂ¬Ã‚ÂºÃ‚ÂÃƒÂ«Ã‚Â¦Ã‚Â­ÃƒÂ­Ã¢â‚¬Å¾Ã‚Â°ÃƒÂªÃ‚Â°Ã¢â€šÂ¬ ÃƒÂ¬Ã…Â¾Ã‹â€ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE) && (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide))
                                    {
                                        // ÃƒÂ¬Ã†â€™Ã‚ÂÃƒÂ«Ã…â€™Ã¢â€šÂ¬ÃƒÂ«Ã‚Â°Ã‚Â©ÃƒÂ¬Ã‚ÂÃ‚Â´ ÃƒÂ«Ã‚Â§Ã‹â€ ÃƒÂ«Ã‚Â²Ã¢â‚¬Â¢ ÃƒÂ¬Ã‚Â Ã¢â€šÂ¬ÃƒÂ­Ã¢â‚¬Â¢Ã‚Â­ÃƒÂ¬Ã¢â‚¬â€Ã‚Â ÃƒÂ¬Ã¢â‚¬Â¹Ã‚Â¤ÃƒÂ­Ã…â€™Ã‚Â¨ÃƒÂ­Ã¢â‚¬â€œÃ‹â€ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] != 0) break; //ÃƒÂ¬Ã‚ÂÃ‚Â´ÃƒÂ«Ã‚Â¯Ã‚Â¸ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤ÃƒÂ«Ã‚Â¥Ã‚Â¸ ConfuseÃƒÂ­Ã…Â¡Ã‚Â¨ÃƒÂªÃ‚Â³Ã‚Â¼ÃƒÂªÃ‚Â°Ã¢â€šÂ¬ ÃƒÂ¬Ã…Â¾Ã‹â€ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤ÃƒÂ«Ã‚Â©Ã‚Â´ ÃƒÂ«Ã‚Â¬Ã‚Â´ÃƒÂ¬Ã¢â‚¬Â¹Ã…â€œÃƒÂ«Ã‚ÂÃ…â€œÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] = (char)m_pMagicConfigList[sType]->m_sValue4;

                                        // ÃƒÂ­Ã…Â¡Ã‚Â¨ÃƒÂªÃ‚Â³Ã‚Â¼ÃƒÂªÃ‚Â°Ã¢â€šÂ¬ ÃƒÂ­Ã¢â‚¬Â¢Ã‚Â´ÃƒÂ¬Ã‚Â Ã…â€œÃƒÂ«Ã‚ÂÃ‚Â  ÃƒÂ«Ã¢â‚¬Â¢Ã…â€™ ÃƒÂ«Ã‚Â°Ã…â€œÃƒÂ¬Ã†â€™Ã‚ÂÃƒÂ­Ã¢â‚¬Â¢Ã‚Â  ÃƒÂ«Ã¢â‚¬ÂÃ…â€œÃƒÂ«Ã‚Â Ã‹â€ ÃƒÂ¬Ã‚ÂÃ‚Â´ ÃƒÂ¬Ã‚ÂÃ‚Â´ÃƒÂ«Ã‚Â²Ã‚Â¤ÃƒÂ­Ã…Â Ã‚Â¸ÃƒÂ«Ã‚Â¥Ã‚Â¼ ÃƒÂ«Ã¢â‚¬Å“Ã‚Â±ÃƒÂ«Ã‚Â¡Ã‚ÂÃƒÂ­Ã¢â‚¬Â¢Ã…â€œÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_CONFUSE, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                                        // ÃƒÂ«Ã‚Â§Ã‹â€ ÃƒÂ«Ã‚Â²Ã¢â‚¬Â¢ÃƒÂ¬Ã¢â‚¬â€Ã‚Â ÃƒÂªÃ‚Â±Ã‚Â¸ÃƒÂ«Ã‚Â Ã‚Â¸ÃƒÂ¬Ã‚ÂÃ…â€™ÃƒÂ¬Ã‚ÂÃ¢â‚¬Å¾ ÃƒÂ¬Ã¢â‚¬Â¢Ã…â€™ÃƒÂ«Ã‚Â¦Ã‚Â°ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_CONFUSE, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                                    }
                                }
                            }
                        break;

                    case 3: // Ilusion, Mass-Ilusion
                        for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                            for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                            {
                                // ÃƒÂ¬Ã…Â¾Ã‚ÂÃƒÂ¬Ã¢â‚¬Â¹Ã‚Â ÃƒÂ«Ã‚ÂÃ¢â‚¬Å¾ ÃƒÂ­Ã¢â‚¬ÂÃ‚Â¼ÃƒÂ­Ã‚ÂÃ‚Â­ÃƒÂ«Ã‚ÂÃ‚Â  ÃƒÂ¬Ã‹â€ Ã‹Å“ ÃƒÂ¬Ã…Â¾Ã‹â€ ÃƒÂ¬Ã…â€œÃ‚Â¼ÃƒÂ«Ã¢â‚¬Â¹Ã‹â€  ÃƒÂ¬Ã‚Â£Ã‚Â¼ÃƒÂ¬Ã‚ÂÃ‹Å“.
                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                                {
                                    // ÃƒÂ­Ã¢â‚¬Â¢Ã‚Â´ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¹ ÃƒÂ¬Ã…â€œÃ¢â‚¬Å¾ÃƒÂ¬Ã‚Â¹Ã‹Å“ÃƒÂ¬Ã¢â‚¬â€Ã‚Â ÃƒÂ¬Ã‚ÂºÃ‚ÂÃƒÂ«Ã‚Â¦Ã‚Â­ÃƒÂ­Ã¢â‚¬Å¾Ã‚Â°ÃƒÂªÃ‚Â°Ã¢â€šÂ¬ ÃƒÂ¬Ã…Â¾Ã‹â€ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE) && (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide))
                                    {
                                        // ÃƒÂ¬Ã†â€™Ã‚ÂÃƒÂ«Ã…â€™Ã¢â€šÂ¬ÃƒÂ«Ã‚Â°Ã‚Â©ÃƒÂ¬Ã‚ÂÃ‚Â´ ÃƒÂ«Ã‚Â§Ã‹â€ ÃƒÂ«Ã‚Â²Ã¢â‚¬Â¢ ÃƒÂ¬Ã‚Â Ã¢â€šÂ¬ÃƒÂ­Ã¢â‚¬Â¢Ã‚Â­ÃƒÂ¬Ã¢â‚¬â€Ã‚Â ÃƒÂ¬Ã¢â‚¬Â¹Ã‚Â¤ÃƒÂ­Ã…â€™Ã‚Â¨ÃƒÂ­Ã¢â‚¬â€œÃ‹â€ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] != 0) break; //ÃƒÂ¬Ã‚ÂÃ‚Â´ÃƒÂ«Ã‚Â¯Ã‚Â¸ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤ÃƒÂ«Ã‚Â¥Ã‚Â¸ ConfuseÃƒÂ­Ã…Â¡Ã‚Â¨ÃƒÂªÃ‚Â³Ã‚Â¼ÃƒÂªÃ‚Â°Ã¢â€šÂ¬ ÃƒÂ¬Ã…Â¾Ã‹â€ ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤ÃƒÂ«Ã‚Â©Ã‚Â´ ÃƒÂ«Ã‚Â¬Ã‚Â´ÃƒÂ¬Ã¢â‚¬Â¹Ã…â€œÃƒÂ«Ã‚ÂÃ…â€œÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] = (char)m_pMagicConfigList[sType]->m_sValue4;

                                        switch (m_pMagicConfigList[sType]->m_sValue4)
                                        {
                                            case 3:
                                                SetIllusionFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, TRUE);
                                                break;
                                        }

                                        // ÃƒÂ­Ã…Â¡Ã‚Â¨ÃƒÂªÃ‚Â³Ã‚Â¼ÃƒÂªÃ‚Â°Ã¢â€šÂ¬ ÃƒÂ­Ã¢â‚¬Â¢Ã‚Â´ÃƒÂ¬Ã‚Â Ã…â€œÃƒÂ«Ã‚ÂÃ‚Â  ÃƒÂ«Ã¢â‚¬Â¢Ã…â€™ ÃƒÂ«Ã‚Â°Ã…â€œÃƒÂ¬Ã†â€™Ã‚ÂÃƒÂ­Ã¢â‚¬Â¢Ã‚Â  ÃƒÂ«Ã¢â‚¬ÂÃ…â€œÃƒÂ«Ã‚Â Ã‹â€ ÃƒÂ¬Ã‚ÂÃ‚Â´ ÃƒÂ¬Ã‚ÂÃ‚Â´ÃƒÂ«Ã‚Â²Ã‚Â¤ÃƒÂ­Ã…Â Ã‚Â¸ÃƒÂ«Ã‚Â¥Ã‚Â¼ ÃƒÂ«Ã¢â‚¬Å“Ã‚Â±ÃƒÂ«Ã‚Â¡Ã‚ÂÃƒÂ­Ã¢â‚¬Â¢Ã…â€œÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_CONFUSE, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                                        // ÃƒÂ«Ã‚Â§Ã‹â€ ÃƒÂ«Ã‚Â²Ã¢â‚¬Â¢ÃƒÂ¬Ã¢â‚¬â€Ã‚Â ÃƒÂªÃ‚Â±Ã‚Â¸ÃƒÂ«Ã‚Â Ã‚Â¸ÃƒÂ¬Ã‚ÂÃ…â€™ÃƒÂ¬Ã‚ÂÃ¢â‚¬Å¾ ÃƒÂ¬Ã¢â‚¬Â¢Ã…â€™ÃƒÂ«Ã‚Â¦Ã‚Â°ÃƒÂ«Ã¢â‚¬Â¹Ã‚Â¤.
                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_CONFUSE, m_pMagicConfigList[sType]->m_sValue4, iClientH, NULL);
                                    }
                                }
                            }
                        break;

                    case 4: // Ilusion Movement
                        if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) break;
                        for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                            for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                            {
                                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                                {
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE) && (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] != 0) break;
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                        switch (m_pMagicConfigList[sType]->m_sValue4)
                                        {
                                            case 4:
                                                SetIllusionMovementFlag(sOwnerH, DEF_OWNERTYPE_PLAYER, TRUE);
                                                break;
                                        }

                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_CONFUSE, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_CONFUSE, m_pMagicConfigList[sType]->m_sValue4, iClientH, NULL);
                                    }
                                }
                            }
                }
                break;


            case DEF_MAGICTYPE_POISON:
                // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶ Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½. Ã‚Â¸Ãƒâ€¢Ãƒâ‚¬ÃƒÂº Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ãƒâ‚¬Ã‚Â» Ã‚Â±Ã‚Â¼Ã‚Â¸Ã‚Â®Ã‚Â°ÃƒÂ­ Ã‚Â´Ãƒâ„¢Ãƒâ‚¬Ã‚Â½Ãƒâ‚¬Ã‚Â¸Ã‚Â·ÃƒÅ½ Ã‚ÂµÃ‚Â¶Ã‚Â¼Ã‚ÂºÃƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ãƒâ‚¬Ã‚Â» Ãƒâ€¡Ãƒâ€˜Ã‚Â¹ÃƒÂ¸ Ã‚Â´ÃƒÂµ Ã‚Â±Ã‚Â¼Ã‚Â¸Ã‚Â°Ã‚Â´Ãƒâ„¢. 
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                if (m_pMagicConfigList[sType]->m_sValue4 == 1)
                {
                    // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ãƒâ‚¬Ã‚Â» Ã‚Â°Ãƒâ€¦Ã‚Â´Ãƒâ€š Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ 
                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            // v1.4334 ÃƒÂÃƒÅ¸Ã‚Â¸Ã‚Â³Ãƒâ‚¬Ã‚Âº Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€ ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ãƒâ‚¬Ã‚Â» Ã‚Â¸ÃƒÂ¸Ãƒâ€¡ÃƒÂÃ‚Â°Ãƒâ€ Ã‚Â¼ÃƒÂ¶ÃƒÂÃ‚Â¤
                            if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) goto MAGIC_NOEFFECT;

                            // Ã‚Â¹ÃƒÂ¼ÃƒÂÃƒâ€¹Ãƒâ€¡ÃƒÂ Ãƒâ‚¬Ã‚Â§Ã‚Â¶ÃƒÂ³Ã‚Â¸ÃƒÂ© 
                            bAnalyzeCriminalAction(iClientH, dX, dY);

                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                // Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢. Ã‚ÂµÃ‚Â¶Ã‚Â¼Ã‚ÂºÃƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ãƒâ‚¬Ã‚Â» Ã‚Â°ÃƒÂ¨Ã‚Â»ÃƒÂªÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                if (bCheckResistingPoisonSuccess(sOwnerH, cOwnerType) == FALSE)
                                {
                                    // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ã‚ÂµÃƒâ€¡Ã‚Â¾ÃƒÂºÃ‚Â´Ãƒâ„¢.
                                    m_pClientList[sOwnerH]->m_bIsPoisoned = TRUE;
                                    m_pClientList[sOwnerH]->m_iPoisonLevel = m_pMagicConfigList[sType]->m_sValue5;
                                    m_pClientList[sOwnerH]->m_dwPoisonTime = dwTime;
                                    SetPoisonFlag(sOwnerH, cOwnerType, TRUE);
                                    // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ã‚ÂµÃƒâ€¡Ã‚Â¾ÃƒÂºÃƒâ‚¬Ã‚Â½Ãƒâ‚¬Ã‚Â» Ã‚Â¾Ãƒâ€¹Ã‚Â¸Ã‚Â°Ã‚Â´Ãƒâ„¢. 
                                    SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pMagicConfigList[sType]->m_sValue5, NULL, NULL);
#ifdef DEF_TAIWANLOG
                                    _bItemLog(DEF_ITEMLOG_POISONED, sOwnerH, (char *)NULL, NULL);
#endif
                                }
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_iHP > 0) goto MAGIC_NOEFFECT;
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                // Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢. Ã‚ÂµÃ‚Â¶Ã‚Â¼Ã‚ÂºÃƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ãƒâ‚¬Ã‚Â» Ã‚Â°ÃƒÂ¨Ã‚Â»ÃƒÂªÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                if (bCheckResistingPoisonSuccess(sOwnerH, cOwnerType) == FALSE)
                                {
                                    // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ã‚ÂµÃƒâ€¡Ã‚Â¾ÃƒÂºÃ‚Â´Ãƒâ„¢.

                                }
                            }
                            break;
                    }
                }
                else if (m_pMagicConfigList[sType]->m_sValue4 == 0)
                {
                    // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ãƒâ‚¬Ã‚Â» Ãƒâ€¡Ã‚ÂªÃ‚Â´Ãƒâ€š Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ 
                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;

                            if (m_pClientList[sOwnerH]->m_bIsPoisoned == TRUE)
                            {
                                // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ã‚ÂµÃƒË† Ã‚Â»ÃƒÂ³Ãƒâ€¦Ãƒâ€šÃ‚Â¿Ã‚Â´Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ãƒâ‚¬Ã‚Â» Ãƒâ€¡Ã‚Â¬Ã‚Â´Ãƒâ„¢.
                                m_pClientList[sOwnerH]->m_bIsPoisoned = FALSE;
                                SetPoisonFlag(sOwnerH, cOwnerType, FALSE);
                                // ÃƒÂÃƒÅ¸Ã‚ÂµÃ‚Â¶Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡Ã‚Â®Ã‚Â·ÃƒË†Ãƒâ‚¬Ã‚Â½Ãƒâ‚¬Ã‚Â» Ã‚Â¾Ãƒâ€¹Ã‚Â¸Ã‚Â°Ã‚Â´Ãƒâ„¢. 
                                SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, NULL, NULL, NULL);
                            }
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                            break;
                    }
                }
                break;

            case DEF_MAGICTYPE_BERSERK:
                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:
                        // Ã‚Â¹ÃƒÂ¶Ã‚Â¼Ã‚Â­Ãƒâ€žÃ‚Â¿ Ã‚Â¸ÃƒÂ°Ã‚ÂµÃƒÂ¥Ã‚Â·ÃƒÅ½ Ãƒâ‚¬ÃƒÂ¼ÃƒË†Ã‚Â¯Ã‚ÂµÃƒË†Ã‚Â´Ãƒâ„¢.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0) goto MAGIC_NOEFFECT;
                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetBerserkFlag(sOwnerH, cOwnerType, TRUE);
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0) goto MAGIC_NOEFFECT;
                                // ÃƒÂÃ‚Â¤Ãƒâ‚¬ÃƒÂ» NPCÃ‚ÂµÃƒÂ©Ãƒâ‚¬Ã‚Âº Ã‚Â±Ã‚Â¤Ã‚ÂºÃƒÂ Ã‚Â¾ÃƒË†Ã‚ÂµÃƒÅ 				
                                if (m_pNpcList[sOwnerH]->m_cActionLimit != 0) goto MAGIC_NOEFFECT;
                                // 2002-09-11 #3 Ãƒâ‚¬ÃƒÂ» Ã‚Â¸ÃƒÂ³Ã‚Â½Ã‚ÂºÃƒâ€¦ÃƒÂÃ‚Â´Ãƒâ€š Ã‚Â±Ã‚Â¤Ã‚ÂºÃƒÂÃ‚ÂµÃƒâ€¡ÃƒÂÃƒÂ¶ Ã‚Â¾ÃƒÅ Ãƒâ‚¬Ã‚Â½
                                if (m_pClientList[iClientH]->m_cSide != m_pNpcList[sOwnerH]->m_cSide) goto MAGIC_NOEFFECT;

                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetBerserkFlag(sOwnerH, cOwnerType, TRUE);
                                break;
                        }

                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_BERSERK, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                            sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                        if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_BERSERK, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                        break;
                }
                break;

                // v2.16 2002-5-23 Ã‚Â°ÃƒÂ­Ã‚Â±Ã‚Â¤Ãƒâ€¡ÃƒÂ¶ Ã‚Â¼ÃƒÂ¶ÃƒÂÃ‚Â¤
            case DEF_MAGICTYPE_DAMAGE_AREA_ARMOR_BREAK:
                // ÃƒÂÃƒâ€“Ã‚ÂºÃ‚Â¯ Ã‚Â°ÃƒÂ¸Ã‚Â°ÃƒÂ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // Ãƒâ‚¬ÃƒÅ¡Ã‚Â½Ãƒâ€¦Ã‚ÂµÃ‚Âµ Ãƒâ€¡Ãƒâ€¡Ãƒâ€ ÃƒÂ¸Ã‚ÂµÃƒâ€° Ã‚Â¼ÃƒÂ¶ Ãƒâ‚¬Ãƒâ€“Ãƒâ‚¬Ã‚Â¸Ã‚Â´ÃƒÂ ÃƒÂÃƒâ€“Ãƒâ‚¬Ãƒâ€¡.
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            // Ã‚Â¹ÃƒÂ¦Ã‚Â¾ÃƒÂ®Ã‚Â±Ã‚Â¸Ãƒâ‚¬Ãƒâ€¡ Ã‚Â¼ÃƒÂ¶Ã‚Â¸ÃƒÂ­Ãƒâ‚¬Ã‚Â» ÃƒÂÃƒâ„¢Ãƒâ‚¬ÃƒÅ½Ã‚Â´Ãƒâ„¢.
                            ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                                // Ã‚Â¹ÃƒÂ¦Ã‚Â¾ÃƒÂ®Ã‚Â±Ã‚Â¸Ãƒâ‚¬Ãƒâ€¡ Ã‚Â¼ÃƒÂ¶Ã‚Â¸ÃƒÂ­Ãƒâ‚¬Ã‚Â» ÃƒÂÃƒâ„¢Ãƒâ‚¬ÃƒÅ½Ã‚Â´Ãƒâ„¢.
                                ArmorLifeDecrement(iClientH, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue10);
                            }
                        }
                    }
                break;

                // Resurrection Magic. 
            case DEF_MAGICTYPE_RESURRECTION:
                // 10 Mins once
                if (m_pClientList[iClientH]->m_iSpecialAbilityTime != 0) goto MAGIC_NOEFFECT;
                m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC / 2;
                // Get the ID of the dead Player/NPC on coords dX, dY. 
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                switch (cOwnerType)
                {
                    // For Player. 
                    case DEF_OWNERTYPE_PLAYER:
                        // The Player has to exist. 
                        if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                        // Resurrection is not for alive Players. 
                        if (m_pClientList[sOwnerH]->m_bIsKilled == FALSE) goto MAGIC_NOEFFECT;
                        // Set Deadflag to Alive. 
                        m_pClientList[sOwnerH]->m_bIsKilled = FALSE;
                        // Player's HP becomes half of the Max HP. 
                        m_pClientList[sOwnerH]->m_iHP = ((m_pClientList[sOwnerH]->m_iLevel * 2) + (m_pClientList[sOwnerH]->m_iVit * 3) + (m_pClientList[sOwnerH]->m_iStr / 2)) / 2;
                        // Send new HP to Player. 
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        // Make Player stand up. (Currently, by a fake damage). 
                        m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->ClearDeadOwner(dX, dY);
                        m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->SetOwner(sOwnerH, DEF_OWNERTYPE_PLAYER, dX, dY);
                        SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, NULL, NULL, NULL);
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                        break;
                        // Resurrection is not for NPC's. 
                    case DEF_OWNERTYPE_NPC:
                        goto MAGIC_NOEFFECT;
                        break;
                }
                break;

            case DEF_MAGICTYPE_ICE:
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        {
                            // Ã‚Â´ÃƒÂ«Ã‚Â¹ÃƒÅ’ÃƒÂÃƒÂ¶Ã‚Â¿ÃƒÂ Ãƒâ€¡Ãƒâ€Ã‚Â²Ã‚Â²
                            //Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                            Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                            // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº
                            switch (cOwnerType)
                            {
                                case DEF_OWNERTYPE_PLAYER:
                                    if (m_pClientList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    // Ãƒâ€¦Ã‚Â¸Ã‚Â°Ãƒâ„¢Ãƒâ‚¬ÃƒÅ’ Ã‚Â»ÃƒÂ¬Ã‚Â¾Ãƒâ€ Ãƒâ‚¬Ãƒâ€“Ã‚Â°ÃƒÂ­ Ã‚Â¾ÃƒÂ³Ãƒâ‚¬Ã‚Â½ Ãƒâ‚¬ÃƒÂºÃƒâ€¡Ãƒâ€”Ã‚Â¿Ã‚Â¡ Ã‚Â½Ãƒâ€¡Ãƒâ€ ÃƒÂÃƒâ€¡ÃƒÅ¸Ã‚Â´Ãƒâ„¢Ã‚Â¸ÃƒÂ© Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚ÂºÃƒâ„¢Ã‚Â´Ãƒâ€šÃ‚Â´Ãƒâ„¢.
                                    if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                            // Ã‚Â´ÃƒÂ«Ã‚Â»ÃƒÂ³Ãƒâ‚¬ÃƒÅ’ Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ãƒâ‚¬ÃƒÅ½ Ã‚Â°ÃƒÂ¦Ã‚Â¿ÃƒÂ¬ Ã‚Â¾Ãƒâ€¹Ã‚Â·ÃƒÂÃƒÂÃƒËœÃ‚Â´Ãƒâ„¢.
                                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
                                        }
                                    }
                                    break;

                                case DEF_OWNERTYPE_NPC:
                                    if (m_pNpcList[sOwnerH] == NULL) goto MAGIC_NOEFFECT;
                                    if ((m_pNpcList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                    {
                                        if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                        {
                                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                            SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                            // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                            bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                                sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);
                                        }
                                    }
                                    break;
                            }

                        }

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃƒÂÃƒâ€”Ãƒâ‚¬Ã‚Âº ÃƒÆ’Ã‚Â´Ãƒâ€¡ÃƒÂÃ‚Â°ÃƒÂ­ Ãƒâ‚¬Ãƒâ€“Ã‚Â´Ãƒâ€š Ãƒâ€¡ÃƒÆ’Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ®Ã‚Â´Ãƒâ„¢.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            {

                                //Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                                // Ã‚Â¾ÃƒÂ³Ã‚Â¾ÃƒÂ®Ã‚Â¼Ã‚Â­ Ã‚ÂµÃ‚Â¿Ãƒâ‚¬Ãƒâ€ºÃƒâ‚¬ÃƒÅ’ Ã‚Â´ÃƒÅ Ã‚Â¾ÃƒÂ®ÃƒÂÃƒÂ¶Ã‚Â´Ãƒâ€š ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂº. ÃƒÂÃƒâ€”ÃƒÂÃƒÂ¶ Ã‚Â¾ÃƒÅ Ã‚Â¾Ãƒâ€™Ãƒâ‚¬Ã‚Â¸Ã‚Â¸ÃƒÂ© Ãƒâ‚¬ÃƒÂ»Ã‚Â¿ÃƒÂ« 
                                if ((m_pClientList[sOwnerH]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE))
                                {
                                    if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
                                    {
                                        m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
                                        SetIceFlag(sOwnerH, cOwnerType, TRUE);
                                        // ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â°Ã‚Â¡ Ãƒâ€¡ÃƒËœÃƒÂÃ‚Â¦Ã‚ÂµÃƒâ€° Ã‚Â¶Ã‚Â§ Ã‚Â¹ÃƒÅ¸Ã‚Â»ÃƒÂ½Ãƒâ€¡Ãƒâ€™ Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’ Ãƒâ‚¬ÃƒÅ’Ã‚ÂºÃ‚Â¥Ãƒâ€ Ã‚Â®Ã‚Â¸Ã‚Â¦ Ã‚ÂµÃƒÂ®Ã‚Â·ÃƒÂÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
                                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
                                            sOwnerH, cOwnerType, NULL, NULL, NULL, 1, NULL, NULL);

                                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, NULL, NULL);
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
        // Casting ÃƒË†Ãƒâ€ž Ã‚ÂµÃƒÂ´Ã‚Â·Ã‚Â¹Ãƒâ‚¬ÃƒÅ’Ã‚Â°Ã‚Â¡ Ã‚Â°Ãƒâ€°Ã‚Â¸Ã‚Â®Ã‚Â´Ãƒâ€š Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½
        // Resurrection wand(MS.10) or Resurrection wand(MS.20)

        if (m_pMagicConfigList[sType]->m_sType == DEF_MAGICTYPE_RESURRECTION)
        {
            //Check if player has resurrection wand
            if (m_pClientList[iClientH] != NULL && m_pClientList[iClientH]->m_iSpecialAbilityTime == 0 &&
                m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == FALSE)
            {
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (m_pClientList[sOwnerH] != NULL)
                {
                    // GM's can ressurect ne1, and players must be on same side to ressurect

                    if ((m_pClientList[iClientH]->m_iAdminUserLevel < 1) &&
                        (m_pClientList[sOwnerH]->m_cSide != m_pClientList[iClientH]->m_cSide))
                    {
                        return;
                    }
                    if (cOwnerType == DEF_OWNERTYPE_PLAYER && m_pClientList[sOwnerH] != NULL &&
                        m_pClientList[sOwnerH]->m_iHP <= 0)
                    {
                        m_pClientList[sOwnerH]->m_bIsBeingResurrected = TRUE;
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_RESURRECTPLAYER, NULL, NULL, NULL, NULL);
                        if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
                        {
                            m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = TRUE;
                            m_pClientList[iClientH]->m_dwSpecialAbilityStartTime = dwTime;
                            m_pClientList[iClientH]->m_iSpecialAbilityLastSec = 0;
                            m_pClientList[iClientH]->m_iSpecialAbilityTime = m_pMagicConfigList[sType]->m_dwDelayTime;

                            sTemp = m_pClientList[iClientH]->m_sAppr4;
                            sTemp = 0xFF0F & sTemp;
                            sTemp = sTemp | 0x40;
                            m_pClientList[iClientH]->m_sAppr4 = sTemp;
                        }
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 1, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityLastSec, NULL);
                        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                    }
                }
            }
        }
    }

    MAGIC_NOEFFECT:;

    if (m_pClientList[iClientH] == NULL) return;

    //Mana Slate
    if ((m_pClientList[iClientH]->m_iStatus & 0x800000) != 0)
    {
        iManaCost = 0;
    }

    // ManaÃ‚Â¸Ã‚Â¦ Ã‚Â°Ã‚Â¨Ã‚Â¼Ãƒâ€™Ã‚Â½ÃƒÆ’Ãƒâ€¦Ã‚Â°Ã‚Â°ÃƒÂ­ Ãƒâ€¦ÃƒÂ«Ã‚ÂºÃ‚Â¸Ãƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢.
    m_pClientList[iClientH]->m_iMP -= iManaCost; // sValue1Ãƒâ‚¬ÃƒÅ’ Mana Cost
    if (m_pClientList[iClientH]->m_iMP < 0)
        m_pClientList[iClientH]->m_iMP = 0;

    CalculateSSN_SkillIndex(iClientH, 4, 1);

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MP, NULL, NULL, NULL, NULL);

    // Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½ ÃƒË†Ã‚Â¿Ã‚Â°ÃƒÂºÃ‚Â¸Ã‚Â¦ Ã‚Â´Ãƒâ„¢Ã‚Â¸Ã‚Â¥ Ãƒâ€¦Ã‚Â¬Ã‚Â¶ÃƒÂ³Ãƒâ‚¬ÃƒÅ’Ã‚Â¾ÃƒÂ°Ãƒâ€ Ã‚Â®Ã‚Â¿Ã‚Â¡Ã‚Â°Ãƒâ€ Ãƒâ‚¬ÃƒÂ¼Ã‚Â¼Ãƒâ€ºÃƒâ€¡Ãƒâ€˜Ã‚Â´Ãƒâ„¢. Ã‚Â¸Ã‚Â¶Ã‚Â¹ÃƒÂ½Ã‚Â¹ÃƒÂ¸ÃƒË†Ã‚Â£ + 100Ãƒâ‚¬ÃƒÅ’ Ã‚Â¿Ã‚Â¡Ãƒâ€ ÃƒÂ¥Ãƒâ€ Ã‚Â® Ã‚Â¹ÃƒÂ¸ÃƒË†Ã‚Â£ 
    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);

}

void CGame::NpcMagicHandler(int iNpcH, short dX, short dY, short sType)
{
    short  sOwnerH;
    char   cOwnerType;
    int i, iErr, ix, iy, sX, sY, tX, tY, iResult, iWhetherBonus, iMagicAttr;
    DWORD  dwTime = timeGetTime();

    if (m_pNpcList[iNpcH] == NULL) return;
    if ((dX < 0) || (dX >= m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_sSizeX) ||
        (dY < 0) || (dY >= m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_sSizeY)) return;

    if ((sType < 0) || (sType >= 100))     return;
    if (m_pMagicConfigList[sType] == NULL) return;

    // °ø°ÝÀ§Ä¡°¡ °ø°Ý ºÒ°¡´É ¸ÊÀÌ¶ó¸é °ø°Ý ºÒ°¡´É 
    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_bIsAttackEnabled == FALSE) return;

    // ¸ó½ºÅÍ º° ¸¶¹ý ¸íÁß·ü ÀÔ·Â 
    iResult = m_pNpcList[iNpcH]->m_iMagicHitRatio;

    // ³¯¾¾¿¡ ÀÇÇÑ ¸¶¹ý °ø°Ý·Â Á¶Á¤  
    iWhetherBonus = iGetWhetherMagicBonusEffect(sType, m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cWhetherStatus);

    // v1.41 ¸¶¹ý ¼Ó¼º 
    iMagicAttr = m_pMagicConfigList[sType]->m_iAttribute;

    if (m_pMagicConfigList[sType]->m_dwDelayTime == 0)
    {
        // Áï½Ã È¿°ú¸¦ º¸´Â ¸¶¹ý 
        switch (m_pMagicConfigList[sType]->m_sType)
        {
            case DEF_MAGICTYPE_INVISIBILITY:
                switch (m_pMagicConfigList[sType]->m_sValue4)
                {
                    case 1:
                        // º¸ÀÌÁö ¾Ê´Â »óÅÂ·Î ¸¸µç´Ù. 
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

                        switch (cOwnerType)
                        {
                            case DEF_OWNERTYPE_PLAYER:
                                if (m_pClientList[sOwnerH] == NULL) goto NMH_NOEFFECT;
                                if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto NMH_NOEFFECT;
                                m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetInvisibilityFlag(sOwnerH, cOwnerType, TRUE);
                                // ÀÌ Ä³¸¯ÅÍ¸¦ ÃßÀûÇÏ°í ÀÖ´ø ¸ó½ºÅÍ¸¦ ÇØÁ¦½ÃÅ²´Ù.
                                RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_PLAYER);
                                break;

                            case DEF_OWNERTYPE_NPC:
                                if (m_pNpcList[sOwnerH] == NULL) goto NMH_NOEFFECT;
                                if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != 0) goto NMH_NOEFFECT;
                                m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = (char)m_pMagicConfigList[sType]->m_sValue4;
                                SetInvisibilityFlag(sOwnerH, cOwnerType, TRUE);
                                // ÀÌ NPC¸¦ ÃßÀûÇÏ°í ÀÖ´ø ¸ó½ºÅÍ¸¦ ÇØÁ¦½ÃÅ²´Ù.
                                RemoveFromTarget(sOwnerH, DEF_OWNERTYPE_NPC);
                                break;
                        }

                        // È¿°ú°¡ ÇØÁ¦µÉ ¶§ ¹ß»ýÇÒ µô·¹ÀÌ ÀÌº¥Æ®¸¦ µî·ÏÇÑ´Ù.
                        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_INVISIBILITY, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                            sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                        if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                            SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_INVISIBILITY, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                        break;

                    case 2:
                        // dX, dY ¹Ý°æ 8 ÁÖº¯ÀÇ Invisibility »óÅÂÀÎ Object°¡ ÀÖÀ¸¸é ÇØÁ¦ ½ÃÅ²´Ù.
                        for (ix = dX - 8; ix <= dX + 8; ix++)
                            for (iy = dY - 8; iy <= dY + 8; iy++)
                            {
                                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                                if (sOwnerH != NULL)
                                {
                                    switch (cOwnerType)
                                    {
                                        case DEF_OWNERTYPE_PLAYER:
                                            if (m_pClientList[sOwnerH] == NULL) goto NMH_NOEFFECT;
                                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != NULL)
                                            {
                                                if (m_pClientList[sOwnerH]->m_sType != 66)
                                                {
                                                    m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
                                                    SetInvisibilityFlag(sOwnerH, cOwnerType, FALSE);
                                                    bRemoveFromDelayEventList(sOwnerH, cOwnerType, DEF_MAGICTYPE_INVISIBILITY);
                                                }
                                            }
                                            break;

                                        case DEF_OWNERTYPE_NPC:
                                            if (m_pNpcList[sOwnerH] == NULL) goto NMH_NOEFFECT;
                                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] != NULL)
                                            {
                                                if (m_pClientList[sOwnerH]->m_sType != 66)
                                                {
                                                    m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
                                                    SetInvisibilityFlag(sOwnerH, cOwnerType, FALSE);
                                                    bRemoveFromDelayEventList(sOwnerH, cOwnerType, DEF_MAGICTYPE_INVISIBILITY);
                                                }
                                            }
                                            break;
                                    }
                                }
                            }
                        break;
                }
                break;

            case DEF_MAGICTYPE_HOLDOBJECT:
                // ¿ÀºêÁ§Æ®ÀÇ ¿òÁ÷ÀÓÀ» ºÀ¼âÇÑ´Ù. 
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                {

                    switch (cOwnerType)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            if (m_pClientList[sOwnerH] == NULL) goto NMH_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto NMH_NOEFFECT;
                            if (m_pClientList[sOwnerH]->m_iAddPR >= 500) goto NMH_NOEFFECT;
                            m_pClientList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            if (m_pNpcList[sOwnerH] == NULL) goto NMH_NOEFFECT;
                            if (m_pNpcList[sOwnerH]->m_cMagicLevel >= 6) goto NMH_NOEFFECT; // v1.4 ¸¶¹ý ·¹º§ 6ÀÌ»óÀÎ ¸®Ä¡±Þ ÀÌ»óÀÇ ¸¶¹ý ¸ó½ºÅÍ¿¡°Ô´Â ¸¶ºñ ¸¶¹ý ÅëÇÏÁö ¾Ê´Â´Ù.
                            if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) goto NMH_NOEFFECT;
                            m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = (char)m_pMagicConfigList[sType]->m_sValue4;
                            break;
                    }

                    // ºÀ¼â È¿°ú°¡ ÇØÁ¦µÉ ¶§ ¹ß»ýÇÒ µô·¹ÀÌ ÀÌº¥Æ®¸¦ µî·ÏÇÑ´Ù.
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + (m_pMagicConfigList[sType]->m_dwLastTime * 1000),
                        sOwnerH, cOwnerType, NULL, NULL, NULL, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);

                    // È¿°ú°¡ »ý°åÀ½À» ¾Ë·ÁÁØ´Ù.
                    if (cOwnerType == DEF_OWNERTYPE_PLAYER)
                        SendNotifyMsg(NULL, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_HOLDOBJECT, m_pMagicConfigList[sType]->m_sValue4, NULL, NULL);
                }
                break;


            case DEF_MAGICTYPE_DAMAGE_LINEAR:
                // ÀÏÁ÷¼± »ó¿¡ ÀÖ´Â ¸ñÇ¥¸¦ ¸ðµÎ °ø°ÝÇÑ´Ù.
                sX = m_pNpcList[iNpcH]->m_sX;
                sY = m_pNpcList[iNpcH]->m_sY;

                for (i = 2; i < 10; i++)
                {
                    iErr = 0;
                    m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

                    // tx, ty
                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx-1, ty
                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx+1, ty
                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx, ty-1
                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // tx, ty+1
                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
                }

                // ÁÖº¯ °ø°Ý È¿°ú 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // ÀÚ½Åµµ ÇÇÆøµÉ ¼ö ÀÖÀ¸´Ï ÁÖÀÇ.
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                            if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }
                    }

                // dX, dY
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);
                }
                break;

            case DEF_MAGICTYPE_DAMAGE_SPOT:
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                }
                break;

            case DEF_MAGICTYPE_HPUP_SPOT:
                // ÀÌ ¸¶¹ýÀº ¸íÁß·ü°ú´Â »ó°üÀÌ ¾ø´Ù.
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                Effect_HpUp_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA:
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);

                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
                if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                    (m_pClientList[sOwnerH]->m_iHP > 0))
                {
                    // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                    if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, TRUE, iMagicAttr);
                }

                // ÁÖº¯ °ø°Ý È¿°ú 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // ÀÚ½Åµµ ÇÇÆøµÉ ¼ö ÀÖÀ¸´Ï ÁÖÀÇ.
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                            if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT:
                // Á÷°ÝÀº °è»êÇÏÁö ¾Ê´Â´Ù.			
                // ÁÖº¯ °ø°Ý È¿°ú 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // ÀÚ½Åµµ ÇÇÆøµÉ ¼ö ÀÖÀ¸´Ï ÁÖÀÇ.
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // Á×Àº Ã´ÇÏ°í ÀÖ´Â ÇÃ·¹ÀÌ¾î´Ù.
                            if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot_DamageMove(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }
                    }
                break;

            case DEF_MAGICTYPE_SPDOWN_AREA:
                // Sp°¡ ÁÙ¾îµç´Ù.
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                    Effect_SpDown_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                // ÁÖº¯ °ø°Ý È¿°ú 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // ÀÚ½Åµµ ÇÇÆøµÉ ¼ö ÀÖÀ¸´Ï ÁÖÀÇ.
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        if (bCheckResistingMagicSuccess(m_pNpcList[iNpcH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_SpDown_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;

            case DEF_MAGICTYPE_SPUP_AREA:
                // Sp°¡ »ó½ÂÇÑ´Ù.
                m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
                // ¸¶¹ý ÀúÇ×ÀÌ ÇÊ¿ä¾ø´Ù. 
                Effect_SpUp_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6);
                // ÁÖº¯ °ø°Ý È¿°ú 
                for (iy = dY - m_pMagicConfigList[sType]->m_sValue3; iy <= dY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                    for (ix = dX - m_pMagicConfigList[sType]->m_sValue2; ix <= dX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                    {
                        // ÀÚ½Åµµ ÇÇÆøµÉ ¼ö ÀÖÀ¸´Ï ÁÖÀÇ.
                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                        // ¸¶¹ýÀúÇ×ÀÌ ÇÊ¿ä ¾ø´Ù.
                        Effect_SpUp_Spot(iNpcH, DEF_OWNERTYPE_NPC, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9);
                    }
                break;

        }
    }
    else
    {
        // Casting ÈÄ µô·¹ÀÌ°¡ °É¸®´Â ¸¶¹ý

    }

    NMH_NOEFFECT:;

    // Mana¸¦ °¨¼Ò½ÃÅ²´Ù.
    m_pNpcList[iNpcH]->m_iMana -= m_pMagicConfigList[sType]->m_sValue1; // sValue1ÀÌ Mana Cost
    if (m_pNpcList[iNpcH]->m_iMana < 0)
        m_pNpcList[iNpcH]->m_iMana = 0;

    // ¸¶¹ý È¿°ú¸¦ ´Ù¸¥ Å¬¶óÀÌ¾ðÆ®¿¡°Ô Àü¼ÛÇÑ´Ù. ¸¶¹ý¹øÈ£ + 100ÀÌ ¿¡ÆåÆ® ¹øÈ£ 
    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pNpcList[iNpcH]->m_cMapIndex,
        m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, dX, dY, (sType + 100), m_pNpcList[iNpcH]->m_sType);

}

void CGame::PK_KillRewardHandler(short sAttackerH, short sVictumH)
{
    if (m_pClientList[sAttackerH] == NULL) return;
    if (m_pClientList[sVictumH] == NULL)   return;

    _bPKLog(DEF_PKLOG_BYPLAYER, sAttackerH, sVictumH, NULL);

    if (m_pClientList[sAttackerH]->m_iPKCount != 0)
    {
        // PKÂ¸Â¦ Ã€Ã¢Ã€Âº Â»Ã§Â¶Ã·Ã€ÃŒ PKÂ¸Ã© Â¾Ã†Â¹Â«Â·Â± ÂµÃ¦Ã€ÃŒ Â¾Ã¸Â´Ã™.

    }
    else
    {
        // Ã†Ã·Â»Ã³Â±ÃÂ¸Â¸ Â´Â©Ã€Ã». Â°Ã¦Ã‡Ã¨Ã„Â¡Â´Ã‚ Â¿Ã€Â¸Â£ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™. 
        m_pClientList[sAttackerH]->m_iRewardGold += iGetExpLevel(m_pClientList[sVictumH]->m_iExp) * 3;


        if (m_pClientList[sAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
            m_pClientList[sAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
        if (m_pClientList[sAttackerH]->m_iRewardGold < 0)
            m_pClientList[sAttackerH]->m_iRewardGold = 0;

        SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_PKCAPTURED, m_pClientList[sVictumH]->m_iPKCount, m_pClientList[sVictumH]->m_iLevel, NULL, m_pClientList[sVictumH]->m_cCharName);
    }
}

void CGame::EnemyKillRewardHandler(int iAttackerH, int iClientH)
{
    // enemy-kill-mode = 1 | 0
    // if m_bEnemyKillMode is true than death match mode

    // DEATHMATCH MODE:
    // Aresden kills Elvine in Aresden and gets EK
    // Elvine kills Aresden in Aresden and gets an EK 
    // Elvine kills Aresden in Elvine and gets an EK
    // Aresden kills Elvine in Elvine and gets an EK

    // CLASSIC MODE:
    // Aresden kills Elvine in Aresden and gets EK
    // Elvine kills Aresden in Aresden and doesnt get an EK 
    // Elvine kills Aresden in Elvine and gets an EK
    // Aresden kills Elvine in Elvine and doesnt get an EK

    int iRewardExp, iEK_Level;

    // if the attacker doesnt exist no result
    if (m_pClientList[iAttackerH] == NULL) return;
    // if the target doesnt exist no result
    if (m_pClientList[iClientH] == NULL)   return;

    // Log the EK (killed by enemy)
    _bPKLog(DEF_PKLOG_BYENERMY, iAttackerH, iClientH, NULL);

    // set EK level to 30
    iEK_Level = 30;
    // if attacker's level is greater than 80, set ek level to 80
    if (m_pClientList[iAttackerH]->m_iLevel >= 80) iEK_Level = 80;
    // check if attacker level is less than or equal to max level
    if (m_pClientList[iAttackerH]->m_iLevel >= m_iPlayerMaxLevel)
    {
        // if the
        if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
        {
            // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80Ã€ÃŒÂ»Ã³Ã€ÃŒÂ°Ã­
            if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0)
                && (m_bEnemyKillMode == FALSE))
            {
                // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
            }

            if (m_bEnemyKillMode == TRUE)
            {
                // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
            }
        }
        // Ã†Ã·Â»Ã³Â±Ã Â´Â©Ã€Ã» 
        m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
        if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
            m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
        if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
            m_pClientList[iAttackerH]->m_iRewardGold = 0;

        // Ã€Ã»Ã€Â» Ã€Ã¢Â¾Ã’Â´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶ ÂºÂ¸Â³Â¿ 
        SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_ENEMYKILLREWARD, iClientH, NULL, NULL, NULL);
        return;
    }

    if (m_pClientList[iAttackerH]->m_iPKCount != 0)
    {
        // Ã€Ã»Ã€Â» Ã€Ã¢Ã€Âº Â»Ã§Â¶Ã·Ã€ÃŒ PKÂ¸Ã© Â¾Ã†Â¹Â«Â·Â± ÂµÃ¦Ã€ÃŒ Â¾Ã¸Â´Ã™.
    }
    else
    {
        // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â½ÃƒÂ¹ÃŽ, Â±Ã¦ÂµÃ¥Â¿Ã¸ Â¿Â©ÂºÃŽÂ¿Â¡ ÂµÃ»Â¶Ã³ Ã†Ã·Â»Ã³Ã€ÃŒ Â´ÃžÂ¶Ã³ÃÃ¸Â´Ã™. 
        if (m_pClientList[iClientH]->m_iGuildRank == -1)
        {
            // Â±Ã¦ÂµÃ¥Â¿Ã¸Ã€ÃŒ Â¾Ã†Â´ÃÂ¹Ã‡Â·ÃŽ Â½ÃƒÂ¹ÃŽ. (Â¿Â©Ã‡Ã Ã€ÃšÃ€Ã‡ Â°Ã¦Â¿Ã¬Â¿Â¡Â´Ã‚ Ã€ÃŒ Ã‡Ã”Â¼Ã¶Â°Â¡ ÃˆÂ£ÃƒÃ¢ÂµÃ‡ÃÃ¶ Â¾ÃŠÃ€Â¸Â¹Ã‡Â·ÃŽ)
            // v2.15 Ã€Ã»Ã€Â» ÃÃ—Â¿Â´Ã€Â»Â¶Â§ Ã†Ã²Â±Ã•Â°Ã¦Ã‡Ã¨Ã„Â¡Â¸Â¦ Â¾Ã²Â±Ã¢Ã€Â§Ã‡Ã˜ 
            iRewardExp = (iDice(3, (3 * iGetExpLevel(m_pClientList[iClientH]->m_iExp))) + iGetExpLevel(m_pClientList[iClientH]->m_iExp)) / 3;

            if (m_bIsCrusadeMode == TRUE)
            {
                // Ã…Â©Â·Ã§Â¼Â¼Ã€ÃŒÂµÃ¥ Â¸Ã°ÂµÃ¥Â¶Ã³Â¸Ã© Â°Ã¦Ã‡Ã¨Ã„Â¡Ã€Ã‡ (1/3)*3Ã€Â» Â¸Ã•Ã€Ãº ÃÃ–Â°Ã­ Â³ÂªÂ¸Ã“ÃÃ¶Â´Ã‚ 6Â¹Ã¨Â·ÃŽ Ã€Ã¼Ã€Ã¯ Â°Ã¸Ã‡Ã¥ÂµÂµÂ¿Â¡ Â´Â©Ã€Ã» 
                m_pClientList[iAttackerH]->m_iExp += (iRewardExp / 3) * 4;
                m_pClientList[iAttackerH]->m_iWarContribution += (iRewardExp - (iRewardExp / 3)) * 12;

                if (m_pClientList[iAttackerH]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                    m_pClientList[iAttackerH]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                m_pClientList[iAttackerH]->m_iConstructionPoint += m_pClientList[iClientH]->m_iLevel / 2;

                if (m_pClientList[iAttackerH]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                    m_pClientList[iAttackerH]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                //testcode
                wsprintf(G_cTxt, "Enemy Player Killed by Player! Construction: +%d WarContribution +%d", m_pClientList[iClientH]->m_iLevel / 2, (iRewardExp - (iRewardExp / 3)) * 6);
                log->info(G_cTxt);

                // Â¾Ã‹Â·ÃÃÃ˜Â´Ã™.
                SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iAttackerH]->m_iConstructionPoint, m_pClientList[iAttackerH]->m_iWarContribution, NULL, NULL);

                // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80 Ã€ÃŒÂ»Ã³Ã€ÃŽ Â°Ã¦Â¿Ã¬ Enemy Kill countÂ¸Â¦ Â¿ÃƒÂ¸Â°Â´Ã™.
                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {
                    // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80Ã€ÃŒÂ»Ã³Ã€ÃŒÂ°Ã­
                    if (memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0)
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }
                    if (m_bEnemyKillMode == TRUE)
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }
                }
                // Ã†Ã·Â»Ã³Â±Ã Â´Â©Ã€Ã» 
                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
            else
            {
                // Ã€ÃÂ¹Ã Â¸Ã°ÂµÃ¥.
                m_pClientList[iAttackerH]->m_iExp += iRewardExp;
                // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80 Ã€ÃŒÂ»Ã³Ã€ÃŽ Â°Ã¦Â¿Ã¬ Enemy Kill countÂ¸Â¦ Â¿ÃƒÂ¸Â°Â´Ã™.
                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {
                    // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80Ã€ÃŒÂ»Ã³Ã€ÃŒÂ°Ã­
                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0)
                        && (m_bEnemyKillMode == FALSE))
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }

                    if (m_bEnemyKillMode == TRUE)
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }
                }
                // Ã†Ã·Â»Ã³Â±Ã Â´Â©Ã€Ã» 
                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
        }
        else
        {
            // v2.15 Ã€Ã»Ã€Â» ÃÃ—Â¿Â´Ã€Â»Â¶Â§ Ã†Ã²Â±Ã•Â°Ã¦Ã‡Ã¨Ã„Â¡Â¸Â¦ Â¾Ã²Â±Ã¢Ã€Â§Ã‡Ã˜ 
            iRewardExp = (iDice(3, (3 * iGetExpLevel(m_pClientList[iClientH]->m_iExp))) + iGetExpLevel(m_pClientList[iClientH]->m_iExp)) / 3;

            // Â±Ã¦ÂµÃ¥Â¿Ã¸Ã€ÃŒÂ´Ã™.
            if (m_bIsCrusadeMode == TRUE)
            {
                // Ã…Â©Â·Ã§Â¼Â¼Ã€ÃŒÂµÃ¥ Â¸Ã°ÂµÃ¥.
                // Ã…Â©Â·Ã§Â¼Â¼Ã€ÃŒÂµÃ¥ Â¸Ã°ÂµÃ¥Â¶Ã³Â¸Ã© Â°Ã¦Ã‡Ã¨Ã„Â¡Ã€Ã‡ 1/3Ã€Â» Â¸Ã•Ã€Ãº ÃÃ–Â°Ã­ Â³ÂªÂ¸Ã“ÃÃ¶Â´Ã‚ 2Â¹Ã¨Â·ÃŽ Ã€Ã¼Ã€Ã¯ Â°Ã¸Ã‡Ã¥ÂµÂµÂ¿Â¡ Â´Â©Ã€Ã» 
                m_pClientList[iAttackerH]->m_iExp += (iRewardExp / 3) * 4;
                m_pClientList[iAttackerH]->m_iWarContribution += (iRewardExp - (iRewardExp / 3)) * 12;

                if (m_pClientList[iAttackerH]->m_iWarContribution > DEF_MAXWARCONTRIBUTION)
                    m_pClientList[iAttackerH]->m_iWarContribution = DEF_MAXWARCONTRIBUTION;

                m_pClientList[iAttackerH]->m_iConstructionPoint += m_pClientList[iClientH]->m_iLevel / 2;

                if (m_pClientList[iAttackerH]->m_iConstructionPoint > DEF_MAXCONSTRUCTIONPOINT)
                    m_pClientList[iAttackerH]->m_iConstructionPoint = DEF_MAXCONSTRUCTIONPOINT;

                //testcode
                wsprintf(G_cTxt, "Enemy Player Killed by Player! Construction: +%d WarContribution +%d", m_pClientList[iClientH]->m_iLevel / 2, (iRewardExp - (iRewardExp / 3)) * 6);
                log->info(G_cTxt);

                // Â¾Ã‹Â·ÃÃÃ˜Â´Ã™.
                SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iAttackerH]->m_iConstructionPoint, m_pClientList[iAttackerH]->m_iWarContribution, NULL, NULL);

                // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80 Ã€ÃŒÂ»Ã³Ã€ÃŽ Â°Ã¦Â¿Ã¬ Enemy Kill countÂ¸Â¦ Â¿ÃƒÂ¸Â°Â´Ã™.
                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {
                    // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80Ã€ÃŒÂ»Ã³Ã€ÃŒÂ°Ã­
                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0)
                        && (m_bEnemyKillMode == FALSE))
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }

                    if (m_bEnemyKillMode == TRUE)
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }
                }
                // Ã†Ã·Â»Ã³Â±Ã Â´Â©Ã€Ã» 
                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
            else
            {
                // Ã€ÃÂ¹Ã Â¸Ã°ÂµÃ¥.
                m_pClientList[iAttackerH]->m_iExp += iRewardExp;
                // Enemy Kill countÂ¸Â¦ Â¿ÃƒÂ¸Â°Â´Ã™.
                if (iGetExpLevel(m_pClientList[iClientH]->m_iExp) >= iEK_Level)
                {
                    // ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80Ã€ÃŒÂ»Ã³Ã€ÃŒÂ°Ã­
                    if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0)
                        && (m_bEnemyKillMode == FALSE))
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }

                    if (m_bEnemyKillMode == TRUE)
                    {
                        // ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
                        m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
                    }
                }
                // Ã†Ã·Â»Ã³Â±Ã Â´Â©Ã€Ã» 
                m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));
                if (m_pClientList[iAttackerH]->m_iRewardGold > DEF_MAXREWARDGOLD)
                    m_pClientList[iAttackerH]->m_iRewardGold = DEF_MAXREWARDGOLD;
                if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
                    m_pClientList[iAttackerH]->m_iRewardGold = 0;
            }
        }

        // Ã€Ã»Ã€Â» Ã€Ã¢Â¾Ã’Â´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶ ÂºÂ¸Â³Â¿ 
        SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_ENEMYKILLREWARD, iClientH, NULL, NULL, NULL);

        if (bCheckLimitedUser(iAttackerH) == FALSE)
        {
            // ÃƒÂ¼Ã‡Ã¨Ã†Ã‡ Â»Ã§Â¿Ã«Ã€Ãš ÃÂ¦Ã‡Ã‘Â¿Â¡ Ã‡Ã˜Â´Ã§ÂµÃ‡ÃÃ¶ Â¾ÃŠÃ€Â¸Â¸Ã© Â°Ã¦Ã‡Ã¨Ã„Â¡Â°Â¡ Â¿ÃƒÂ¶ÃºÂ´Ã™Â´Ã‚ Ã…Ã«ÂºÂ¸Â¸Â¦ Ã‡Ã‘Â´Ã™.
            SendNotifyMsg(NULL, iAttackerH, DEF_NOTIFY_EXP, NULL, NULL, NULL, NULL);
        }
        // Â·Â¹ÂºÂ§Ã€ÃŒ Â¿ÃƒÂ¶ÃºÂ´Ã‚ÃÃ¶Â¸Â¦ Â°Ã‹Â»Ã§Ã‡Ã‘Â´Ã™.
        bCheckLevelUp(iAttackerH);

        //v1.4 Â±Â³Ã€Ã¼Â¿Â¡Â¼Â­ Ã€ÃŒÂ±Ã¤ Ã„Â«Â¿Ã®Ã†Â®Â¸Â¦ Â¿ÃƒÂ¸Â°Â´Ã™.
        m_stCityStatus[m_pClientList[iAttackerH]->m_cSide].iWins++;
    }
}

void CGame::GetRewardMoneyHandler(int iClientH)
{
    int iRet, iEraseReq, iWeightLeft, iRewardGoldLeft;
    DWORD * dwp;
    WORD * wp;
    char * cp, cData[100], cItemName[30];
    CItem * pItem;
    short * sp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;


    // Â³Â²Ã€Âº ÃÃŸÂ·Â®Ã€Â» Â°Ã¨Â»ÃªÃ‡Ã‘Â´Ã™. 
    iWeightLeft = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);

    if (iWeightLeft <= 0) return;
    // ÃÃŸÂ·Â®Ã€Â» Â¹ÃÃ€Â¸Â·ÃŽ Â³ÂªÂ´Â«Â´Ã™. <- ÃƒÂ£Ã€Âº ÂµÂ·Ã€Â¸Â·ÃŽ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â»Ã¬ Â°Ã¸Â°Â£Ã€Âº Â¸Â¶Â·ÃƒÃ‡Ã˜ ÂµÃ–Â¾ÃŸ Ã‡ÃÂ¹Ã‡Â·ÃŽ.
    iWeightLeft = iWeightLeft / 2;
    if (iWeightLeft <= 0) return;

    pItem = new CItem;
    ZeroMemory(cItemName, sizeof(cItemName));
    wsprintf(cItemName, "Gold");
    _bInitItemAttr(pItem, cItemName);
    //pItem->m_dwCount = m_pClientList[iClientH]->m_iRewardGold;

    // (iWeightLeft / pItem->m_wWeight)Â°Â¡ ÃƒÃ–Â´Ã« Â¹ÃžÃ€Â» Â¼Ã¶ Ã€Ã–Â´Ã‚ GoldÂ°Â¹Â¼Ã¶. Â°Â®Â°Ã­Ã€Ã–Â´Ã‚ Ã†Ã·Â»Ã³Â±ÃÂ°Ãº ÂºÃ±Â±Â³Ã‡Ã‘Â´Ã™. 
    if ((iWeightLeft / iGetItemWeight(pItem, 1)) >= m_pClientList[iClientH]->m_iRewardGold)
    {
        // Ã†Ã·Â»Ã³Â±ÃÃ€Â» Â¸Ã°ÂµÃŽ Â¹ÃžÃ€Â» Â¼Ã¶ Ã€Ã–Â´Ã™. 
        pItem->m_dwCount = m_pClientList[iClientH]->m_iRewardGold;
        iRewardGoldLeft = 0;
    }
    else
    {
        // (iWeightLeft / pItem->m_wWeight)Â¸Â¸ Â¹ÃžÂ´Ã‚Â´Ã™.
        pItem->m_dwCount = (iWeightLeft / iGetItemWeight(pItem, 1));
        iRewardGoldLeft = m_pClientList[iClientH]->m_iRewardGold - (iWeightLeft / iGetItemWeight(pItem, 1));
    }

    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
    {
        // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» ÃˆÂ¹ÂµÃ¦Ã‡ÃŸÂ´Ã™.

        // Â³Â²Ã€Âº Ã†Ã·Â»Ã³Â±Ã Â³Â»Â¿Âª Â°Ã¨Â»Ãª.
        m_pClientList[iClientH]->m_iRewardGold = iRewardGoldLeft;

        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        // 1Â°Â³ ÃˆÂ¹ÂµÃ¦Ã‡ÃŸÂ´Ã™. <- Â¿Â©Â±Ã¢Â¼Â­ 1Â°Â³Â¶Ãµ Ã„Â«Â¿Ã®Ã†Â®Â¸Â¦ Â¸Â»Ã‡ÃÂ´Ã‚ Â°ÃÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™
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

        *cp = (char)0; // Â¾Ã²Ã€Âº Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒÂ¹Ã‡Â·ÃŽ Ã€Ã¥Ã‚Ã¸ÂµÃ‡ÃÃ¶ Â¾ÃŠÂ¾Ã’Â´Ã™.
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

        *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
        cp++;

        dwp = (DWORD *)cp;
        *dwp = pItem->m_dwAttribute;
        cp += 4;
        /*
        *cp = (char)(pItem->m_dwAttribute & 0x00000001); // Custom-ItemÃ€ÃŽÃÃ¶Ã€Ã‡ Â¿Â©ÂºÃŽ
        cp++;
        */

        if (iEraseReq == 1) delete pItem;

        // Â¾Ã†Ã€ÃŒÃ…Ã› ÃÂ¤ÂºÂ¸ Ã€Ã¼Â¼Ã› 
        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
                // Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
                DeleteClient(iClientH, TRUE, TRUE);
                return;
        }

        // Â±Ã— Â´Ã™Ã€Â½ Â³Â²Ã€Âº Ã†Ã·Â»Ã³Â±ÃÃ€Â» Â¾Ã‹Â·ÃÃÃ˜Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REWARDGOLD, NULL, NULL, NULL, NULL);
    }
    else
    {
        // Â¹ÃžÃ€Â» Â¼Ã¶ Â¾Ã¸Â´Ã‚ Â°Ã¦Â¿Ã¬Â´Ã‚ Â¾Ã†Â¹Â«Â·Â± ÃƒÂ³Â¸Â®Â¸Â¦ Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™. 

    }
}

void CGame::ItemDepleteHandler(int iClientH, short sItemIndex, BOOL bIsUseItemResult)
{
    // Â¼Ã’Â¸Ã°Â¼Âº Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ Ã„Â«Â¿Ã®Ã†Â®Â°Â¡ 0Ã€ÃŒÂ¸Ã© Â»Ã¨ÃÂ¦Ã‡Ã‘Â´Ã™. ÃˆÂ¤Ã€Âº ÃÃŸÂ°Ã­ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Ã†ÃˆÂ¾Ã†Â¹Ã¶Â¸Â° Â°ÃÂ¿Ã¬  

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;

    // v1.411 Â·ÃŽÂ±Ã—Â¸Â¦ Â³Â²Â±Ã¤Â´Ã™. 
    _bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, NULL, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

    // Â¸Ã•Ã€Ãº Ã€Ã¥Ã‚Ã¸ÂµÃ‡Â¾Ã® Ã€Ã–Â´Ã™Â¸Ã© Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™. Ã€ÃŒÂ·Â± Â°Ã¦Â¿Ã¬Â´Ã‚ Â°Ã…Ã€Ã‡ Â¾Ã¸Ã€Â» ÂµÃ­ 
    ReleaseItemHandler(iClientH, sItemIndex, TRUE);

    // Ã€ÃŒÃÂ¦ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¸Â®Â½ÂºÃ†Â®Â¿Â¡Â¼Â­ Â»Ã¨ÃÂ¦Ã‡Ã’Â°ÃÃ€Â» Ã…Ã«ÂºÂ¸Ã‡Ã‘Â´Ã™.
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMDEPLETED_ERASEITEM, sItemIndex, (int)bIsUseItemResult, NULL, NULL);

    // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¸ÃžÂ¸Ã°Â¸Â®Â¿Â¡Â¼Â­ Â»Ã¨ÃÂ¦ 
    delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
    m_pClientList[iClientH]->m_pItemList[sItemIndex] = NULL;

    m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = FALSE;

    // !!! BUG POINT
    // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ Ã€ÃŽÂµÂ¦Â½ÂºÂ°Â¡ Â¹Ã™Â²Ã¯ Â»Ã³ÃˆÂ²Ã€ÃŒÂ´Ã™. ArrowIndexÂ¿Ã Â°Â°Ã€Âº Â°ÃÃ€Âº Â´Ã™Â½Ãƒ Ã‡Ã’Â´Ã§Ã‡Ã˜Â¾ÃŸ Ã‡Ã‘Â´Ã™. 
    m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

    // Â¼Ã’ÃÃ¶Ã‡Â° ÃƒÃ‘ ÃÃŸÂ·Â® Ã€Ã§ Â°Ã¨Â»Ãª 
    iCalcTotalWeight(iClientH);
}

void CGame::UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID)
{
    int iTemp, iMax, iV1, iV2, iV3, iSEV1, iEffectResult = 0;
    DWORD dwTime;
    short sTemp, sTmpType, sTmpAppr1;
    char cSlateType[20];

    dwTime = timeGetTime();
    ZeroMemory(cSlateType, sizeof(cSlateType));

    //testcode
    //wsprintf(G_cTxt, "%d", sDestItemID);
    //log->info(G_cTxt);

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;

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

        // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ ÃˆÂ¿Â°ÃºÂ¿Â¡ Â¸Ã‚Â´Ã‚ ÃƒÂ³Â¸Â®Â¸Â¦ Ã‡Ã‘Â´Ã™. 
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
        {
            case DEF_ITEMEFFECTTYPE_WARM: // v2.172 2002-7-5 ÇØµ¿ ½Ã¾à. 

                // ³Ãµ¿ »óÅÂÀÎ °æ¿ì ÇØµ¿ µÇ¾ú´Ù´Â ¸Þ¼¼Áö¸¦ º¸³»ÁØ´Ù. 
                if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 1)
                {
                    //	SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);

                    bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_ICE);

                    // È¿°ú°¡ ÇØÁ¦µÉ ¶§ ¹ß»ýÇÒ µô·¹ÀÌ ÀÌº¥Æ®¸¦ µî·ÏÇÑ´Ù.
                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (1 * 1000),
                        iClientH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 1, NULL, NULL);


                    //				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_ICE, NULL, NULL, NULL);
                }

                m_pClientList[iClientH]->m_dwWarmEffectTime = dwTime;
                break;

            case DEF_ITEMEFFECTTYPE_LOTTERY:
                // ÂºÂ¹Â±Ã‡ Â¾Ã†Ã€ÃŒÃ…Ã› EV1(ÃˆÂ®Â·Ã¼: ÃƒÃ–Ã€Ãº 100) EV2(Â»Ã³Ã‡Â° ÃÂ¾Â·Ã¹) EV3(Â»Ã³Ã‡Â° Â¼Ã¶Â·Â®)
                iTemp = iDice(1, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1);
                if (iTemp == iDice(1, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1))
                {
                    // Â´Ã§ÃƒÂ·!

                }
                else
                {
                    // Â²ÃŽ!

                }
                break;

            case DEF_ITEMEFFECTTYPE_SLATES:
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != NULL)
                {
                    // Full Ancient Slate ??
                    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 867)
                    {
                        // Slates dont work on Heldenian Map
                        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2)
                        {
                            case 2: // Bezerk slate
                                m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = TRUE;
                                SetBerserkFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_BERSERK, dwTime + (1000 * 600),
                                    iClientH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 1, NULL, NULL);
                                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_BERSERK, 1, NULL, NULL);
                                strcpy(cSlateType, "Berserk");
                                break;

                            case 1: // Invincible slate
                                if (strlen(cSlateType) == 0)
                                {
                                    strcpy(cSlateType, "Invincible");
                                }
                            case 3: // Mana slate
                                if (strlen(cSlateType) == 0)
                                {
                                    strcpy(cSlateType, "Mana");
                                }
                            case 4: // Exp slate
                                if (strlen(cSlateType) == 0)
                                {
                                    strcpy(cSlateType, "Exp");
                                }
                                SetSlateFlag(iClientH, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2, TRUE);
                                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_ANCIENT_TABLET, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2,
                                    dwTime + (1000 * 600), iClientH, DEF_OWNERTYPE_PLAYER, NULL, NULL, NULL, 1, NULL, NULL);
                                switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2)
                                {
                                    case 1:
                                        iEffectResult = 4;
                                        break;
                                    case 3:
                                        iEffectResult = 5;
                                        break;
                                    case 4:
                                        iEffectResult = 6;
                                        break;
                                }
                        }
                        if (strlen(cSlateType) > 0)
                            _bItemLog(DEF_ITEMLOG_USE, iClientH, strlen(cSlateType), m_pClientList[iClientH]->m_pItemList[sItemIndex]);
                    }
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
            case DEF_ITEMEFFECTTYPE_CRITKOMM:
                CritInc(iClientH);
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

                if (m_pClientList[iClientH]->m_bIsPoisoned == TRUE)
                {
                    // ÃÃŸÂµÂ¶ÂµÃˆ Â»Ã³Ã…Ã‚Â¿Â´Â´Ã™Â¸Ã© ÃÃŸÂµÂ¶Ã€Â» Ã‡Â¬Â´Ã™.
                    m_pClientList[iClientH]->m_bIsPoisoned = FALSE;
                    // ÃÃŸÂµÂ¶Ã€ÃŒ Ã‡Â®Â·ÃˆÃ€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™. 
                    SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE); // removes poison aura when using a revitalizing potion
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, NULL, NULL, NULL);
                }
                break;

            case DEF_ITEMEFFECTTYPE_HPSTOCK:
                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;

                m_pClientList[iClientH]->m_iHPstock += iDice(iV1, iV2) + iV3;
                if (m_pClientList[iClientH]->m_iHPstock < 0)   m_pClientList[iClientH]->m_iHPstock = 0;
                if (m_pClientList[iClientH]->m_iHPstock > 500) m_pClientList[iClientH]->m_iHPstock = 500;

                // Â¹Ã¨Â°Ã­Ã‡Ã„Ã€Â» Ã‡Ã˜Â°Ã¡Ã‡Ã‘Â´Ã™. 
                m_pClientList[iClientH]->m_iHungerStatus += iDice(iV1, iV2) + iV3;
                if (m_pClientList[iClientH]->m_iHungerStatus > 100) m_pClientList[iClientH]->m_iHungerStatus = 100;
                if (m_pClientList[iClientH]->m_iHungerStatus < 0)   m_pClientList[iClientH]->m_iHungerStatus = 0;
                break;

            case DEF_ITEMEFFECTTYPE_STUDYSKILL:
                // Â±Ã¢Â¼ÃºÃ€Â» Â¹Ã¨Â¿Ã®Â´Ã™.	
                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
                iSEV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
                // iV1Ã€Âº Â¹Ã¨Â¿Ã¯ Skill Â¹Ã¸ÃˆÂ£. iV2Â´Ã‚ Â±Ã¢Â¼Ãº Â¼Ã¶ÃÃ˜, iSEV1Ã€Âº Â»Ã§Â¿Ã«Ã€Ãš ÃÂ¤Ã€Ã‡ Â±Ã¢Â¼Ãº Â¼Ã¶ÃÃ˜(Â¿Ã¬Â¼Â±Â¼Ã¸Ã€Â§) 
                if (iSEV1 == 0)
                {
                    // Â»Ã§Â¿Ã«Ã€Ãš ÃÂ¤Ã€Ã‡ Â±Ã¢Â¼ÃºÂ¼Ã¶ÃÃ˜Ã€ÃŒ 0Ã€ÃŒÂ¶Ã³Â¸Ã© Ã‡Â¥ÃÃ˜ Â±Ã¢Â¼ÃºÂ¼Ã¶ÃÃ˜Â¿Â¡ ÂµÃ»Â¶Ã³ Â±Ã¢Â¼ÃºÃ€Â» Â¹Ã¨Â¿Ã¬Â°Ã” ÂµÃˆÂ´Ã™. 
                    TrainSkillResponse(TRUE, iClientH, iV1, iV2);
                }
                else
                {
                    TrainSkillResponse(TRUE, iClientH, iV1, iSEV1);
                }
                break;

            case DEF_ITEMEFFECTTYPE_STUDYMAGIC:
                // iV1Ã€Âº Â¹Ã¨Â¿Ã¯ Â¸Â¶Â¹Ã½ Â¹Ã¸ÃˆÂ£.
                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                if (m_pMagicConfigList[iV1] != NULL)
                    RequestStudyMagicHandler(iClientH, m_pMagicConfigList[iV1]->m_cName, FALSE);
                break;

                /*case DEF_ITEMEFFECTTYPE_LOTTERY:
                    iLottery = iDice(1, m_pClientList[iClientH]->m_pItemList[sItemIndex]->
                    break;*/

            case DEF_ITEMEFFECTTYPE_MAGIC:
                // Ã…ÃµÂ¸Ã­ Â¸Ã°ÂµÃ¥Â¿Â´Â´Ã™Â¸Ã© Â¸Â¶Â¹Ã½ ÃˆÂ¿Â°Ãº Â¾Ã†Ã€ÃŒÃ…Ã› Â»Ã§Â¿Ã«Â½ÃƒÂ¿Â¡ Ã‡Ã˜ÃÂ¦ÂµÃˆÂ´Ã™.
                if ((m_pClientList[iClientH]->m_iStatus & 0x10) != 0)
                {
                    if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
                    {
                        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);

                        bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
                        m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = NULL;
                    }
                }

                switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1)
                {
                    case 1:
                        // Recall Â¸Â¶Â¹Ã½ ÃˆÂ¿Â°ÃºÂ°Â¡ Ã€Ã–Â´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›. 
                        // testcode
                        RequestTeleportHandler(iClientH, "1   ");
                        break;

                    case 2:
                        // Ã…ÃµÂ¸Ã­ Â¸Â¶Â¹Ã½ÃˆÂ¿Â°ÃºÂ°Â¡ Ã€Ã–Â´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›. 
                        PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 32, TRUE);
                        break;

                    case 3:
                        // Ã…ÃµÂ¸Ã­ Â¸Â¶Â¹Ã½ ÃƒÂ£Â±Ã¢. Â»Ã§Ã…ÃµÃ€Ã¥ Â³Â»ÂºÃŽÂ¸Ã© Â¼Ã’Â¿Ã«Â¾Ã¸Â´Ã™. 
                        if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == FALSE)
                            PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 34, TRUE);
                        break;

                    case 4:
                        // fixed location teleportation: Ã€Ã”Ã€Ã¥Â±Ã‡ ÂµÃ®ÂµÃ®
                        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2)
                        {
                            case 1:
                                // ÂºÃ­Â¸Â®ÂµÃ¹ Â¾Ã†Ã€ÃÂ·ÃŽ Â°Â£Â´Ã™ 
                                if (memcmp(m_pClientList[iClientH]->m_cMapName, "bisle", 5) != 0)
                                {
                                    //v1.42
                                    ItemDepleteHandler(iClientH, sItemIndex, TRUE);
                                    RequestTeleportHandler(iClientH, "2   ", "bisle", -1, -1);
                                }
                                break;
                            case 2: //lotery
                                ItemDepleteHandler(iClientH, sItemIndex, TRUE);
                                LoteryHandler(iClientH);
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
                                // Â°Ã¡Ã…ÃµÃ€Ã¥Ã€Â¸Â·ÃŽ Â°Â£Â´Ã™. 
                                SYSTEMTIME SysTime;

                                GetLocalTime(&SysTime);
                                // v1.4311-3 ÂºÂ¯Â°Ã¦ Ã€Ã”Ã€Ã¥Â±Ã‡ ÃƒÂ¼Ã…Â© Â´Ãž/Â³Â¯Ã‚Â¥/Â½ÃƒÂ°Â£Ã€Â¸Â·ÃŽ ÃƒÂ¼Ã…Â©Ã‡Ã‘Â´Ã™. 
                                // Ã€Ã”Ã€Ã¥ Â°Â¡Â´Ã‰Ã‡Ã‘ Â½ÃƒÂ°Â£ÂºÂ¸Â´Ã™ Ã€Ã›Â°Ã…Â³Âª Â°Â°Ã€Â¸Â¸Ã© Ã€Ã”Ã€Ã¥Â±Ã‡Ã€ÃŒ Â»Ã§Â¶Ã³ÃÃ¸Â´Ã™.
                                if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue1 != SysTime.wMonth) ||
                                    (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue2 != SysTime.wDay) ||
                                    (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue3 <= SysTime.wHour))
                                {
                                    // Â³Â¯Ã‚Â¥Â°Â¡ ÃÂ¤ÃˆÂ®Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã™. Â¾Ã†Â¹Â«Â·Â± ÃˆÂ¿Â°ÃºÂ°Â¡ Â¾Ã¸Â°Ã­ Ã€Ã”Ã€Ã¥Â±Ã‡Ã€Âº Â»Ã§Â¶Ã³ÃÃ¸Â´Ã™.
                                }
                                else
                                {
                                    char cDestMapName[11];
                                    ZeroMemory(cDestMapName, sizeof(cDestMapName));
                                    wsprintf(cDestMapName, "fightzone%d", m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 - 10);
                                    if (memcmp(m_pClientList[iClientH]->m_cMapName, cDestMapName, 10) != 0)
                                    {
                                        //v1.42
                                        ItemDepleteHandler(iClientH, sItemIndex, TRUE);
                                        RequestTeleportHandler(iClientH, "2   ", cDestMapName, -1, -1);
                                    }
                                }
                                break;
                        }
                        break;

                    case 5:
                        // Â¼Ã’ÃˆÂ¯ÃˆÂ¿Â°ÃºÂ°Â¡ Ã€Ã–Â´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã› 
                        PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, TRUE,
                            m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
                        break;
                }
                break;

            case DEF_ITEMEFFECTTYPE_FIRMSTAMINAR:
                m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar > 20 * 30) m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar = 20 * 30; // ÃƒÃ–Â´Ã« 30ÂºÃÂ°Â£ 
                break;

            case DEF_ITEMEFFECTTYPE_CHANGEATTR:
                switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1)
                {
                    case 1:
                        // Â¸Ã“Â¸Â® Â»Ã¶Ã€Â» Â¹Ã™Â²Ã›Â´Ã™. 
                        m_pClientList[iClientH]->m_cHairColor++;
                        if (m_pClientList[iClientH]->m_cHairColor > 15) m_pClientList[iClientH]->m_cHairColor = 0;

                        sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
                        m_pClientList[iClientH]->m_sAppr1 = sTemp;
                        break;

                    case 2:
                        // Â¸Ã“Â¸Â® Â½ÂºÃ…Â¸Ã€ÃÃ€Â» Â¹Ã™Â²Ã›Â´Ã™.
                        m_pClientList[iClientH]->m_cHairStyle++;
                        if (m_pClientList[iClientH]->m_cHairStyle > 7) m_pClientList[iClientH]->m_cHairStyle = 0;

                        sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
                        m_pClientList[iClientH]->m_sAppr1 = sTemp;
                        break;

                    case 3:
                        // Ã‡Ã‡ÂºÃŽÂ»Ã¶Ã€Â» Â¹Ã™Â²Ã›Â´Ã™.
                        // Â³Â»Â¿Ã«Ã€Â» Â¹Ã™Ã…ÃÃ€Â¸Â·ÃŽ AppearanceÂ¸Â¦ Â°Ã¨Â»Ãª, Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™.
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
                        // Â¼ÂºÃ€Ã¼ÃˆÂ¯ - Â¸Â¸Â¾Ã  Â¿ÃŠÃ€Â» Ã€Ã”Â°Ã­ Ã€Ã–Â´Ã™Â¸Ã© Â½Ã‡Ã†ÃÃ‡Ã‘Â´Ã™. 
                        sTemp = m_pClientList[iClientH]->m_sAppr3 & 0xFF0F;
                        if (sTemp == 0)
                        {
                            // sTempÂ°Â¡ 0Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© Â°Ã‘Â¿ÃŠ, Â¼Ã“Â¿ÃŠ, Â¹Ã™ÃÃ¶ÃÃŸ Ã‡Ã‘Â°Â¡ÃÃ¶Â¸Â¦ Ã€Ã”Â°Ã­ Ã€Ã–Â´Ã‚ Â°ÃÃ€ÃŒÂ´Ã™. Â¼ÂºÃ€Ã¼ÃˆÂ¯Ã€Â» Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™. 
                            if (m_pClientList[iClientH]->m_cSex == 1)
                                m_pClientList[iClientH]->m_cSex = 2;
                            else m_pClientList[iClientH]->m_cSex = 1;

                            // Â³Â»Â¿Ã«Ã€Â» Â¹Ã™Ã…ÃÃ€Â¸Â·ÃŽ AppearanceÂ¸Â¦ Â°Ã¨Â»Ãª, Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™.
                            if (m_pClientList[iClientH]->m_cSex == 1)
                            {
                                // Â³Â²Ã€ÃšÃ€ÃŒÂ´Ã™. 
                                sTmpType = 1;
                            }
                            else if (m_pClientList[iClientH]->m_cSex == 2)
                            {
                                // Â¿Â©Ã€ÃšÃ€ÃŒÂ´Ã™.
                                sTmpType = 4;
                            }

                            switch (m_pClientList[iClientH]->m_cSkin)
                            {
                                case 1:
                                    // Â¹Ã©Ã€ÃŽÃ€ÃŒÂ¸Ã© Â±Ã—Â´Ã«Â·ÃŽ.
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
                }

                SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
                break;
        }
        // *** Request Teleport HandlerÂ°Â¡ Ã€Ã›ÂµÂ¿ÂµÃ‡Â¸Ã© Ã€ÃŒÂ¹ÃŒ ÂµÂ¥Ã€ÃŒÃ…Ã Ã€ÃºÃ€Ã¥Ã€ÃŒ Â¿Ã¤ÃƒÂ»ÂµÃˆ Â»Ã³Ã…Ã‚Ã€ÃŒÂ¹Ã‡Â·ÃŽ Ã€ÃŒÃˆÃ„Â¿Â¡ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¾Ã¸Â¾Ã–ÂºÃÂ¾ÃŸ Â¼Ã’Â¿Ã«Ã€ÃŒ Â¾Ã¸Â´Ã™. 
        // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¸Ã•Ã€Ãº Â¾Ã¸Â¾Ã˜Â´Ã™.
        ItemDepleteHandler(iClientH, sItemIndex, TRUE);

        switch (iEffectResult)
        {
            case 1:
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                break;
            case 2:
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_MP, NULL, NULL, NULL, NULL);
                break;
            case 3:
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SP, NULL, NULL, NULL, NULL);
                break;
            case 4: // Invincible
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_INVINCIBLE, NULL, NULL, NULL, NULL);
                break;
            case 5: // Mana
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_MANA, NULL, NULL, NULL, NULL);
                break;
            case 6: // EXP
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SLATE_EXP, NULL, NULL, NULL, NULL);
                break;
            default:
                break;
        }
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST)
    {
        // Â»Ã§Â¿Ã«Ã‡ÃÂ¸Ã©Â¼Â­ Â¸Ã±Ã‡Â¥ÃÃ¶ÃÂ¡Ã€Â» ÃÃ¶ÃÂ¤Ã‡ÃÂ´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›.
        // dX, dYÃ€Ã‡ ÃÃ‚Ã‡Â¥Â°Â¡ Ã€Â¯ÃˆÂ¿ Â¹Ã¼Ã€Â§ Â³Â»Â¿Â¡ Ã€Ã–Â´Ã‚ÃÃ¶ ÃˆÂ®Ã€ÃŽÃ‡Ã˜Â¾ÃŸ Ã‡Ã‘Â´Ã™.
        if (_bDepleteDestTypeItemUseEffect(iClientH, dX, dY, sItemIndex, sDestItemID) == TRUE)
            ItemDepleteHandler(iClientH, sItemIndex, TRUE);
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)
    {
        // ÃˆÂ­Â»Ã¬Ã€Â» Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™. 
        m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_PERM)
    {
        // Â¿ÂµÂ±Â¸ÃˆÃ· Â¾Âµ Â¼Ã¶ Ã€Ã–Â´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›. ÃÃ¯ Â¾Â²Â°Ã­Â³ÂªÂµÂµ Â¾Ã¸Â¾Ã®ÃÃ¶ÃÃ¶ Â¾ÃŠÂ´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›. (ex: ÃÃ¶ÂµÂµ) 
        switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType)
        {
            case DEF_ITEMEFFECTTYPE_SHOWLOCATION:
                iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
                switch (iV1)
                {
                    case 1:
                        // Ã‡Ã¶Ã€Ã§ Ã€ÃšÂ½Ã…Ã€Ã‡ Ã€Â§Ã„Â¡Â¸Â¦ ÂºÂ¸Â¿Â©ÃÃ˜Â´Ã™. 
                        if (strcmp(m_pClientList[iClientH]->m_cMapName, "aresden") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 1, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvine") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 2, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "middleland") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 3, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "default") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 4, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone2") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 5, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone1") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 6, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone4") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 7, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone3") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 8, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "arefarm") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 9, NULL, NULL);
                        else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvfarm") == 0)
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 10, NULL, NULL);
                        else SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 0, NULL, NULL);
                        break;
                }
                break;
        }
    }
    else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_SKILL)
    {
        // Â±Ã¢Â¼ÃºÂ°Ãº Â°Ã¼Â·ÃƒÂµÃˆ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã‘Â´Ã™. Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ Â¼Ã¶Â¸Ã­Ã€Â» Â³Â·ÃƒÃŸÂ°Ã­ ÂµÃ´Â·Â¹Ã€ÃŒ Ã€ÃŒÂºÂ¥Ã†Â®Â¿Â¡ ÂµÃ®Â·ÃÃ‡Ã‘Â´Ã™. 

        if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) ||
            (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) ||
            (m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] == TRUE))
        {
            // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ Â¼Ã¶Â¸Ã­Ã€ÃŒ Â´Ã™ Ã‡ÃŸÂ°Ã…Â³Âª Â¾Ã¸Â°Ã…Â³Âª Â°Ã¼Â·Ãƒ Â½ÂºÃ…Â³Ã€Â» Â»Ã§Â¿Ã«ÃÃŸÃ€ÃŒÂ¶Ã³Â¸Ã© Â¹Â«Â½Ãƒ 
            return;
        }
        else
        {
            if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan != 0)
            {
                // ÃƒÃ–Â´Ã« Â¼Ã¶Â¸Ã­Ã€ÃŒ 0Ã€ÃŒÂ¸Ã© Â»Ã§Â¿Ã«Ã‡Ã˜ÂµÂµ Â¼Ã¶Â¸Ã­Ã€ÃŒ ÃÃ™ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
                m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan--;
                if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0)
                {
                    // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ Â¼Ã¶Â¸Ã­Ã€ÃŒ Â´Ã™ ÂµÃ‡Â¾ÃºÂ´Ã™.
                    // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Â¸ÃÂ°Â¡ÃÂ³Â´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶ <- Ã€ÃŒÂ°Ã‰ Â¹ÃžÃ€Â¸Â¸Ã© Ã€Ã¥Ã‚Ã¸ÃˆÂ­Â¸Ã©Â¿Â¡Â¼Â­ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ„Ã‘Â¾ÃŸ Ã‡Ã‘Â´Ã™.
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ITEMLIFESPANEND, DEF_EQUIPPOS_NONE, sItemIndex, NULL, NULL);
                }
                else
                {
                    // Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã« Â½ÃƒÂ°Â£ IDÂ°ÂªÃ€Â» Â±Â¸Ã‡Ã‘Â´Ã™. v1.12
                    int iSkillUsingTimeID = (int)timeGetTime();

                    bRegisterDelayEvent(DEF_DELAYEVENTTYPE_USEITEM_SKILL, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill,
                        dwTime + m_pSkillConfigList[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill]->m_sValue2 * 1000,
                        iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_cMapIndex, dX, dY,
                        m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill], iSkillUsingTimeID, NULL);

                    // Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã«ÃÃŸ 
                    m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = TRUE;
                    m_pClientList[iClientH]->m_iSkillUsingTimeID[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = iSkillUsingTimeID; //v1.12
                }
            }
        }
    }
}

void CGame::UseSkillHandler(int iClientH, int iV1, int iV2, int iV3)
{
    char  cOwnerType;
    short sAttackerWeapon, sOwnerH;
    int   iResult, iPlayerSkillLevel;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;

    if ((iV1 < 0) || (iV1 >= DEF_MAXSKILLTYPE)) return;
    if (m_pSkillConfigList[iV1] == NULL) return;
    // Ã€ÃŒÂ¹ÃŒ Â±Ã¢Â¼ÃºÃ€Â» Â»Ã§Â¿Ã«ÃÃŸÃ€ÃŒÂ¶Ã³ÂµÂµ Â¸Â®Ã…Ã.
    if (m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] == TRUE) return;

    // v1.3 !!1Ã‡Ã˜Ã…Â· Â°Ã‰Â·Â¯Â³Â»Â±Ã¢Â¿Ã«! 
    /*
    if (iV1 != 19) {
        m_pClientList[iClientH]->m_iAbuseCount++;
        if ((m_pClientList[iClientH]->m_iAbuseCount % 30) == 0) {
            wsprintf(G_cTxt, "(!) Ã‡Ã˜Ã…Â· Â¿Ã«Ã€Ã‡Ã€Ãš(%s) Skill(%d) Tries(%d)",m_pClientList[iClientH]->m_cCharName,
                                                                       iV1, m_pClientList[iClientH]->m_iAbuseCount);
            log->info(G_cTxt);
        }
    }
    */

    // Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Ã€Ã‡ Â±Ã¢Â¼ÃºÂ¼Ã¶ÃÃ˜Â¿Â¡ ÂµÃ»Â¶Ã³ Â¼ÂºÂ°Ã¸Â¿Â©ÂºÃŽÂ¸Â¦ Â°Ã¨Â»ÃªÃ‡Ã‘Â´Ã™. 
    iPlayerSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[iV1];
    iResult = iDice(1, 100);

    if (iResult > iPlayerSkillLevel)
    {
        // Â½Ã‡Ã†ÃÂ´Ã™. 
        // Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã«Ã€ÃŒ ÃÃŸÃÃ¶ ÂµÃ‡Â¾ÃºÃ€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, NULL, NULL, NULL, NULL);
        return;
    }

    // iV1Ã€ÃŒ Â±Ã¢Â¼Ãº Â¹Ã¸ÃˆÂ£ 
    switch (m_pSkillConfigList[iV1]->m_sType)
    {
        case DEF_SKILLEFFECTTYPE_PRETEND:
            switch (m_pSkillConfigList[iV1]->m_sValue1)
            {
                case 1:
                    // ÃÃ—Ã€ÂºÃƒÂ´Ã‡ÃÂ±Ã¢ Â±Ã¢Â¼ÃºÃ€ÃŒÂ´Ã™.	

                    // v1.44 Â»Ã§Ã…ÃµÃ€Ã¥Ã€ÃŒÂ¸Ã© ÃÃ—Ã€ÂºÃƒÂ´Ã‡ÃÂ±Ã¢ Â¸Ã¸Ã‡Ã‘Â´Ã™.
                    if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == TRUE)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, NULL, NULL, NULL, NULL);
                        return;
                    }

                    //Â¸Â¸Â¾Ã  Â¹Ã™Â´ÃšÂ¿Â¡ Â½ÃƒÃƒÂ¼Â°Â¡ Ã€Ã–Â´Ã™Â¸Ã© ÃÃ—Ã€ÂºÃƒÂ´ Ã‡ÃÂ±Ã¢Â¸Â¦ Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™. 
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
                    if (sOwnerH != NULL)
                    {
                        // ÃÃ—Ã€ÂºÃƒÂ´ Ã‡ÃÂ±Ã¢Â¸Â¦ Ã‡Ã’ Ã€ÃšÂ¸Â®Â¿Â¡ Â½ÃƒÃƒÂ¼Â°Â¡ Ã€Ã–Â¾Ã® Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã«Ã€ÃŒ ÃÃŸÃÃ¶ ÂµÃ‡Â¾ÃºÃ€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™.
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, NULL, NULL, NULL, NULL);
                        return;
                    }

                    // ÃÃ—Ã€ÂºÃƒÂ´ Ã‡ÃÂ°Ã­Ã€Ãš Ã‡ÃÂ´Ã‚ ÃÃ–ÂºÂ¯Â¿Â¡ Â¹Â°ÃƒÂ¼Â°Â¡ Ã€Ã–Â¾Ã®ÂµÂµ Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™. 
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
                        // ÃÃ—Ã€ÂºÃƒÂ´ Ã‡ÃÂ±Ã¢Â¸Â¦ Ã‡Ã’ Ã€ÃšÂ¸Â®Â¿Â¡ Â½ÃƒÃƒÂ¼Â°Â¡ Ã€Ã–Â¾Ã® Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã«Ã€ÃŒ ÃÃŸÃÃ¶ ÂµÃ‡Â¾ÃºÃ€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™.
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SKILLUSINGEND, NULL, NULL, NULL, NULL);
                        return;
                    }

                    // Â½ÂºÃ…Â³ Ã„Â«Â¿Ã®Ã†Â® Â¿ÃƒÂ¸Â°Â´Ã™. <-- Â³Â»ÂºÃŽÂ¿Â¡Â¼Â­ Ã„Â«Â¿Ã®Ã†ÃƒÃ€Â» Ã‡Ã˜Â¾ÃŸ Â¾Ã»Â¶Ã—Ã‡Ã‘ Â½ÂºÃ…Â³Ã€ÃŒ Â¿Ã€Â¸Â£Â´Ã‚ Â°Ã¦Â¿Ã¬Â°Â¡ Â¾Ã¸Â´Ã™. 
                    CalculateSSN_SkillIndex(iClientH, iV1, 1);

                    // Â´Ã™Â¸Â¥ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®Â¿Â¡Â°Ã” ÃÃ—Â´Ã‚ ÂµÂ¿Ã€Ã› Ã€Ã¼Â¼Ã›.
                    sAttackerWeapon = 1;
                    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, 0, sAttackerWeapon, NULL);
                    // ÃÂ¤Â»Ã³ Ã€Â§Ã„Â¡Â¿Â¡Â¼Â­ ÃÃ¶Â¿Ã®Â´Ã™.
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(14, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
                    // ÃÃ—Ã€Âº Ã€Â§Ã„Â¡ Ã‡Â¥Â½ÃƒÂ¸Â¦ Ã‡Ã‘Â´Ã™.
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetDeadOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
                    break;
            }
            break;

    }

    m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] = TRUE;
}

void CGame::NoticeHandler()
{
    char  cTemp, cBuffer[1000], cKey;
    DWORD dwSize, dwTime = timeGetTime();
    int i, iMsgIndex, iTemp;

    // Â°Ã¸ÃÃ¶Â»Ã§Ã‡Ã—Ã€ÃŒ 1Â°Â³ Ã€ÃŒÃ‡ÃÂ¶Ã³Â¸Ã© ÂºÂ¸Â³Â¾ Ã‡ÃŠÂ¿Ã¤Â°Â¡ Â¾Ã¸Â´Ã™.
    if (m_iTotalNoticeMsg <= 1) return;

    if ((dwTime - m_dwNoticeTime) > DEF_NOTICETIME)
    {
        // Â°Ã¸ÃÃ¶Â»Ã§Ã‡Ã—Ã€Â» Ã€Ã¼Â¼Ã›Ã‡Ã’ Â½ÃƒÂ°Â£Ã€ÃŒ ÂµÃ‡Â¾ÃºÂ´Ã™. 
        m_dwNoticeTime = dwTime;
        do
        {
            iMsgIndex = iDice(1, m_iTotalNoticeMsg) - 1;
        } while (iMsgIndex == m_iPrevSendNoticeMsg);

        m_iPrevSendNoticeMsg = iMsgIndex;

        ZeroMemory(cBuffer, sizeof(cBuffer));
        //         if (m_pNoticeMsgList[iMsgIndex] != NULL)
        //         {
        //             m_pNoticeMsgList[iMsgIndex]->Get(&cTemp, cBuffer, &dwSize, &iTemp, &cKey);
        //         }

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if (m_pClientList[i] != NULL)
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, cBuffer);
            }
    }
}

void CGame::SpecialEventHandler()
{
    DWORD dwTime;

    // Ã†Â¯ÂºÂ°Ã‡Ã‘ Ã€ÃŒÂºÂ¥Ã†Â®Â¸Â¦ Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™. 
    dwTime = timeGetTime();

    if ((dwTime - m_dwSpecialEventTime) < DEF_SPECIALEVENTTIME) return; // DEF_SPECIALEVENTTIME
    m_dwSpecialEventTime = dwTime;
    m_bIsSpecialEventTime = TRUE;

    switch (iDice(1, 180))
    {
        case 98: m_cSpecialEventType = 2; break; // ÂµÂ¥Â¸Ã³ ÃˆÂ¤Ã€Âº Ã€Â¯Â´ÃÃ„ÃœÃ€ÃŒ Â³ÂªÂ¿Ãƒ Â°Â¡Â´Ã‰Â¼ÂºÃ€Âº 30ÂºÃÂ¿Â¡ 1Â¹Ã¸ 1/30
        default: m_cSpecialEventType = 1; break;
    }
}

void CGame::ToggleSafeAttackModeHandler(int iClientH) //v1.1
{
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == FALSE) return;
    if (m_pClientList[iClientH]->m_bIsKilled == TRUE) return;

    // Â¾ÃˆÃ€Ã¼ Â°Ã¸Â°Ã Â¸Ã°ÂµÃ¥Â¸Â¦ Ã…Ã¤Â±Ã›Ã‡Ã‘Â´Ã™. 
    if (m_pClientList[iClientH]->m_bIsSafeAttackMode == TRUE)
        m_pClientList[iClientH]->m_bIsSafeAttackMode = FALSE;
    else m_pClientList[iClientH]->m_bIsSafeAttackMode = TRUE;

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SAFEATTACKMODE, NULL, NULL, NULL, NULL);
}

void CGame::NpcTalkHandler(int iClientH, int iWho)
{
    char cRewardName[30], cTargetName[30];
    int iResMode, iQuestNum, iQuestType, iRewardType, iRewardAmount, iContribution, iX, iY, iRange, iTargetType, iTargetCount;

    iQuestNum = 0;
    ZeroMemory(cTargetName, sizeof(cTargetName));
    if (m_pClientList[iClientH] == NULL) return;
    switch (iWho)
    {
        case 1: break;
        case 2:	break;
        case 3:	break;
        case 4:
            iQuestNum = _iTalkToNpcResult_Cityhall(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
            break;
        case 5: break;
        case 6:	break;
        case 32: break;
        case 21:
            iQuestNum = _iTalkToNpcResult_Guard(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
            if (iQuestNum >= 1000) return;
            break;
    }

    ZeroMemory(cRewardName, sizeof(cRewardName));
    if (iQuestNum > 0)
    {
        if (iRewardType > 1)
        {
            strcpy(cRewardName, m_pItemConfigList[iRewardType]->m_cName);
        }
        else
        {
            switch (iRewardType)
            {
                case -10: strcpy(cRewardName, "¦µ¦F-í"); break;
            }
        }

        m_pClientList[iClientH]->m_iAskedQuest = iQuestNum;
        m_pClientList[iClientH]->m_iQuestRewardType = iRewardType;
        m_pClientList[iClientH]->m_iQuestRewardAmount = iRewardAmount;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, iQuestType, iResMode, iRewardAmount, cRewardName, iContribution,
            iTargetType, iTargetCount, iX, iY, iRange, cTargetName);
    }
    else
    {
        switch (iQuestNum)
        {
            case  0: SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (iWho + 130), NULL, NULL, NULL, NULL); break;
            case -1:
            case -2:
            case -3:
            case -4: SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, abs(iQuestNum) + 100, NULL, NULL, NULL, NULL); break;
            case -5: break;
        }
    }
}

void CGame::SetDownSkillIndexHandler(int iClientH, int iSkillIndex)
{
    if (m_pClientList[iClientH] == NULL) return;
    if ((iSkillIndex < 0) || (iSkillIndex >= DEF_MAXSKILLTYPE)) return;

    if (m_pClientList[iClientH]->m_cSkillMastery[iSkillIndex] > 0)
        m_pClientList[iClientH]->m_iDownSkillIndex = iSkillIndex;

    // Â´Ã™Â¿Ã® Â½ÂºÃ…Â³Ã€Â» Â¼Â³ÃÂ¤Ã‡ÃŸÃ€Â¸Â´Ã Ã€Ã€Â´Ã¤Ã€Â» ÂºÂ¸Â³Â½Â´Ã™. 
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, NULL, NULL, NULL);
}

void CGame::GetOccupyFlagHandler(int iClientH)
{
    int   i, iNum, iRet, iEraseReq, iEKNum;
    char * cp, cData[256]{}, cItemName[30];
    CItem * pItem;
    DWORD * dwp;
    short * sp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iEnemyKillCount < 3) return;
    if (m_pClientList[iClientH]->m_cSide == 0) return;

    ZeroMemory(cItemName, sizeof(cItemName));
    switch (m_pClientList[iClientH]->m_cSide)
    {
        case 1: strcpy(cItemName, "Â¾Ã†Â·Â¹Â½ÂºÂµÂ§Â±ÃªÂ¹ÃŸ"); break;
        case 2: strcpy(cItemName, "Â¿Â¤Â¹Ã™Ã€ÃŽÂ±ÃªÂ¹ÃŸ");   break;
    }

    // ReqPurchaseItemHandlerÂ¿Â¡Â¼Â­ Â°Â¡ÃÂ®Â¿Ã‚ Â·Ã§Ã†Â¾Ã€Â» Â°Ã­ÃƒÃ†Ã€Â½.
    iNum = 1;
    for (i = 1; i <= iNum; i++)
    {

        pItem = new CItem;
        if (_bInitItemAttr(pItem, cItemName) == FALSE)
        {
            // Â±Â¸Ã€Ã”Ã‡ÃÂ°Ã­Ã€Ãš Ã‡ÃÂ´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Â¾Ã†Ã€ÃŒÃ…Ã› Â¸Â®Â½ÂºÃ†Â®Â»Ã³Â¿Â¡ Â¾Ã¸Â´Ã™. Â±Â¸Ã€Ã”Ã€ÃŒ ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ´Ã™.
            delete pItem;
        }
        else
        {

            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
            {
                // Â¿Â¡Â·Â¯ Â¹Ã¦ÃÃ¶Â¿Ã« Ã„ÃšÂµÃ¥
                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

                // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¹ÃžÃ€Â» Â¼Ã¶ Ã€Ã–Â´Ã™Â´Ã‚ Â°ÃÃ€ÃŒ ÃˆÂ®ÃÂ¤ ÂµÃ‡Â¾ÃºÃ€Â¸Â¹Ã‡Â·ÃŽ EK Â°ÂªÃ€Â» Â»Â©ÂµÂµ ÂµÃˆÂ´Ã™.
                if (m_pClientList[iClientH]->m_iEnemyKillCount > 12)
                {
                    iEKNum = 12;
                    m_pClientList[iClientH]->m_iEnemyKillCount -= 12;
                }
                else
                {
                    iEKNum = m_pClientList[iClientH]->m_iEnemyKillCount;
                    m_pClientList[iClientH]->m_iEnemyKillCount = 0;
                }

                // EKNumÃ€Â» Ã€Ã”Â·Ã‚Ã‡Ã‘Â´Ã™.
                pItem->m_sItemSpecEffectValue1 = iEKNum;

                //testcode Â·ÃŽÂ±Ã—Ã†Ã„Ã€ÃÂ¿Â¡ Â±Ã¢Â·ÃÃ‡Ã‘Â´Ã™.
                wsprintf(G_cTxt, "(*) Get Flag : Char(%s) Flag-EK(%d) Player-EK(%d)", m_pClientList[iClientH]->m_cCharName, iEKNum, m_pClientList[iClientH]->m_iEnemyKillCount);
                log->info(G_cTxt);

                // Â¾Ã†Ã€ÃŒÃ…Ã› Â¾Ã²Â¾ÃºÂ´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ Ã€Ã¼Â¼Ã›Ã‡Ã‘Â´Ã™.
                dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
                *dwp = MSGID_NOTIFY;
                wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
                *wp = DEF_NOTIFY_ITEMOBTAINED;

                cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
                // 1Â°Â³ ÃˆÂ¹ÂµÃ¦Ã‡ÃŸÂ´Ã™.
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

                *cp = (char)0; // Â¾Ã²Ã€Âº Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒÂ¹Ã‡Â·ÃŽ Ã€Ã¥Ã‚Ã¸ÂµÃ‡ÃÃ¶ Â¾ÃŠÂ¾Ã’Â´Ã™.
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

                *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
                cp++;

                dwp = (DWORD *)cp;
                *dwp = pItem->m_dwAttribute;
                cp += 4;
                /*
                *cp = (char)(pItem->m_dwAttribute & 0x00000001); // Custom-ItemÃ€ÃŽÃÃ¶Ã€Ã‡ Â¿Â©ÂºÃŽ
                cp++;
                */

                if (iEraseReq == 1) delete pItem;

                // Â¾Ã†Ã€ÃŒÃ…Ã› ÃÂ¤ÂºÂ¸ Ã€Ã¼Â¼Ã› 
                iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

                // Â¼Ã’ÃÃ¶Ã‡Â° ÃƒÃ‘ ÃÃŸÂ·Â® Ã€Ã§ Â°Ã¨Â»Ãª 
                iCalcTotalWeight(iClientH);

                switch (iRet)
                {
                    case DEF_XSOCKEVENT_QUENEFULL:
                    case DEF_XSOCKEVENT_SOCKETERROR:
                    case DEF_XSOCKEVENT_CRITICALERROR:
                    case DEF_XSOCKEVENT_SOCKETCLOSED:
                        // Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
                        DeleteClient(iClientH, TRUE, TRUE);
                        return;
                }

                // ÂºÂ¯Â°Ã¦ÂµÃˆ Â¿Â¡Â³ÃŠÂ¹ÃŒ Ã…Â³Ã€Â» Â¾Ã‹Â·ÃÃÃ˜Â´Ã™.
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, NULL, NULL, NULL);
            }
            else
            {
                // Â°Ã¸Â°Â£Ã€ÃŒ ÂºÃŽÃÂ·Ã‡Ã˜ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¾Ã²Ã€Â» Â¼Ã¶ Â¾Ã¸Â´Ã™.
                delete pItem;

                // Â¼Ã’ÃÃ¶Ã‡Â° ÃƒÃ‘ ÃÃŸÂ·Â® Ã€Ã§ Â°Ã¨Â»Ãª 
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
                        // Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
                        DeleteClient(iClientH, TRUE, TRUE);
                        return;
                }
            }
        }
    }
}

void CGame::GetFightzoneTicketHandler(int iClientH)
{
    int   iRet, iEraseReq, iMonth, iDay, iHour;
    char * cp, cData[256], cItemName[30];
    CItem * pItem;
    DWORD * dwp;
    short * sp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_iFightZoneTicketNumber <= 0)
    {
        // Ã€Ã”Ã€Ã¥Â±Ã‡Ã€Â» Â´Ã™ Â»Ã§Â¿Ã«Ã‡ÃŸÃ€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™.
        // Â»Ã§Ã…ÃµÃ€Ã¥ Â¹Ã¸ÃˆÂ£Â°Â¡ Ã€Â½Â¼Ã¶Â¸Ã© Â¿Â¹Â¾Ã Ã€Âº Ã‡ÃŸÂ´Ã‚ÂµÂ¥ Ã€Ã”Ã€Ã¥Â±Ã‡Ã€Â» Â´Ã™ Â¹ÃžÃ€ÂºÂ°Ã¦Â¿Ã¬ ..
        m_pClientList[iClientH]->m_iFightzoneNumber *= -1;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -1, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cItemName, sizeof(cItemName));

    if (m_pClientList[iClientH]->m_iFightzoneNumber == 1)
        strcpy(cItemName, "ArenaTicket");
    else  wsprintf(cItemName, "ArenaTicket(%d)", m_pClientList[iClientH]->m_iFightzoneNumber);

    pItem = new CItem;
    if (_bInitItemAttr(pItem, cItemName) == FALSE)
    {
        delete pItem;
        return;
    }

    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
    {
        // Â¿Â¡Â·Â¯ Â¹Ã¦ÃÃ¶Â¿Ã« Ã„ÃšÂµÃ¥
        if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

        // Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¹ÃžÃ€Â» Â¼Ã¶ Ã€Ã–Â´Ã™Â´Ã‚ Â°ÃÃ€ÃŒ ÃˆÂ®ÃÂ¤ ÂµÃ‡Â¾ÃºÃ€Â¸Â¹Ã‡Â·ÃŽ Ã€Ã”Ã€Ã¥Â±Ã‡Â°Â³Â¼Ã¶Â¸Â¦ Â»Â©ÂµÂµ ÂµÃˆÂ´Ã™. 
        m_pClientList[iClientH]->m_iFightZoneTicketNumber = m_pClientList[iClientH]->m_iFightZoneTicketNumber - 1;

        pItem->m_sTouchEffectType = DEF_ITET_DATE;

        iMonth = m_pClientList[iClientH]->m_iReserveTime / 10000;
        iDay = (m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000) / 100;
        iHour = m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000 - iDay * 100;

        pItem->m_sTouchEffectValue1 = iMonth;
        pItem->m_sTouchEffectValue2 = iDay;
        pItem->m_sTouchEffectValue3 = iHour;


        wsprintf(G_cTxt, "(*) Get FIGHTZONETICKET : Char(%s) TICKENUMBER (%d)(%d)(%d)", m_pClientList[iClientH]->m_cCharName, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3);
        log->info(G_cTxt);
        log->info(G_cTxt);

        ZeroMemory(cData, sizeof(cData));

        // Â¾Ã†Ã€ÃŒÃ…Ã› Â¾Ã²Â¾ÃºÂ´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ Ã€Ã¼Â¼Ã›Ã‡Ã‘Â´Ã™.
        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
        // 1Â°Â³ ÃˆÂ¹ÂµÃ¦Ã‡ÃŸÂ´Ã™.
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

        *cp = (char)0; // Â¾Ã²Ã€Âº Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒÂ¹Ã‡Â·ÃŽ Ã€Ã¥Ã‚Ã¸ÂµÃ‡ÃÃ¶ Â¾ÃŠÂ¾Ã’Â´Ã™.
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

        *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
        cp++;

        dwp = (DWORD *)cp;
        *dwp = pItem->m_dwAttribute;
        cp += 4;

        if (iEraseReq == 1) delete pItem;

        // Â¾Ã†Ã€ÃŒÃ…Ã› ÃÂ¤ÂºÂ¸ Ã€Ã¼Â¼Ã› 
        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

        // Â¼Ã’ÃÃ¶Ã‡Â° ÃƒÃ‘ ÃÃŸÂ·Â® Ã€Ã§ Â°Ã¨Â»Ãª 
        iCalcTotalWeight(iClientH);

        switch (iRet)
        {
            case DEF_XSOCKEVENT_QUENEFULL:
            case DEF_XSOCKEVENT_SOCKETERROR:
            case DEF_XSOCKEVENT_CRITICALERROR:
            case DEF_XSOCKEVENT_SOCKETCLOSED:
                // Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
                DeleteClient(iClientH, TRUE, TRUE);
                return;
        }
    }
    else
    {
        // Â°Ã¸Â°Â£Ã€ÃŒ ÂºÃŽÃÂ·Ã‡Ã˜ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¾Ã²Ã€Â» Â¼Ã¶ Â¾Ã¸Â´Ã™.
        delete pItem;

        // Â¼Ã’ÃÃ¶Ã‡Â° ÃƒÃ‘ ÃÃŸÂ·Â® Ã€Ã§ Â°Ã¨Â»Ãª 
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
                // Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
                DeleteClient(iClientH, TRUE, TRUE);
                return;
        }
    }
}

void CGame::GetHeroMantleHandler(int iClientH, int iItemID, char * pString)
{
    int   i, iNum, iRet, iEraseReq;
    char * cp, cData[256]{}, cItemName[30];
    CItem * pItem;
    DWORD * dwp;
    short * sp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iEnemyKillCount < 100) return;
    if (m_pClientList[iClientH]->m_cSide == 0) return;
    if (_iGetItemSpaceLeft(iClientH) == 0)
    {
        SendItemNotifyMsg(iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
        return;
    }

    //Prevents a crash if item dosent exist
    if (m_pItemConfigList[iItemID] == NULL)  return;

    switch (iItemID)
    {
        // Hero Cape
        case 400: //Aresden HeroCape
        case 401: //Elvine HeroCape
            if (m_pClientList[iClientH]->m_iEnemyKillCount < 300) return;
            m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
            break;

            // Hero Helm
        case 403: //Aresden HeroHelm(M)
        case 404: //Aresden HeroHelm(W)
        case 405: //Elvine HeroHelm(M)
        case 406: //Elvine HeroHelm(W)
            if (m_pClientList[iClientH]->m_iEnemyKillCount < 150) return;
            m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
            if (m_pClientList[iClientH]->m_iContribution < 20) return;
            m_pClientList[iClientH]->m_iContribution -= 20;
            break;

            // Hero Cap
        case 407: //Aresden HeroCap(M)
        case 408: //Aresden HeroCap(W)
        case 409: //Elvine HeroHelm(M)
        case 410: //Elvine HeroHelm(W)
            if (m_pClientList[iClientH]->m_iEnemyKillCount < 100) return;
            m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
            if (m_pClientList[iClientH]->m_iContribution < 20) return;
            m_pClientList[iClientH]->m_iContribution -= 20;
            break;

            // Hero Armor
        case 411: //Aresden HeroArmor(M)
        case 412: //Aresden HeroArmor(W)
        case 413: //Elvine HeroArmor(M)
        case 414: //Elvine HeroArmor(W)
            if (m_pClientList[iClientH]->m_iEnemyKillCount < 300) return;
            m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
            if (m_pClientList[iClientH]->m_iContribution < 30) return;
            m_pClientList[iClientH]->m_iContribution -= 30;
            break;

            // Hero Robe
        case 415: //Aresden HeroRobe(M)
        case 416: //Aresden HeroRobe(W)
        case 417: //Elvine HeroRobe(M)
        case 418: //Elvine HeroRobe(W)
            if (m_pClientList[iClientH]->m_iEnemyKillCount < 200) return;
            m_pClientList[iClientH]->m_iEnemyKillCount -= 200;
            if (m_pClientList[iClientH]->m_iContribution < 20) return;
            m_pClientList[iClientH]->m_iContribution -= 20;
            break;

            // Hero Hauberk
        case 419: //Aresden HeroHauberk(M)
        case 420: //Aresden HeroHauberk(W)
        case 421: //Elvine HeroHauberk(M)
        case 422: //Elvine HeroHauberk(W)
            if (m_pClientList[iClientH]->m_iEnemyKillCount < 100) return;
            m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
            if (m_pClientList[iClientH]->m_iContribution < 10) return;
            m_pClientList[iClientH]->m_iContribution -= 10;
            break;

            // Hero Leggings
        case 423: //Aresden HeroLeggings(M)
        case 424: //Aresden HeroLeggings(W)
        case 425: //Elvine HeroLeggings(M)
        case 426: //Elvine HeroLeggings(W)
            if (m_pClientList[iClientH]->m_iEnemyKillCount < 150) return;
            m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
            if (m_pClientList[iClientH]->m_iContribution < 15) return;
            m_pClientList[iClientH]->m_iContribution -= 15;
            break;

        default:
            return;
            break;
    }

    ZeroMemory(cItemName, sizeof(cItemName));
    memcpy(cItemName, m_pItemConfigList[iItemID]->m_cName, 20);
    // ReqPurchaseItemHandler
    iNum = 1;
    for (i = 1; i <= iNum; i++)
    {
        pItem = new CItem;
        if (_bInitItemAttr(pItem, cItemName) == FALSE)
        {
            delete pItem;
        }
        else
        {

            if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
            {
                if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

                wsprintf(G_cTxt, "(*) Get HeroItem : Char(%s) Player-EK(%d) Player-Contr(%d) Hero Obtained(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iEnemyKillCount, m_pClientList[iClientH]->m_iContribution, cItemName);
                log->info(G_cTxt);

                pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
                pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
                pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
                pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

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

                *cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
                cp++;

                dwp = (DWORD *)cp;
                *dwp = pItem->m_dwAttribute;
                cp += 4;
                /*
                *cp = (char)(pItem->m_dwAttribute & 0x00000001); // Custom-Item??? ??
                cp++;
                */

                if (iEraseReq == 1) delete pItem;

                iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

                iCalcTotalWeight(iClientH);

                switch (iRet)
                {
                    case DEF_XSOCKEVENT_QUENEFULL:
                    case DEF_XSOCKEVENT_SOCKETERROR:
                    case DEF_XSOCKEVENT_CRITICALERROR:
                    case DEF_XSOCKEVENT_SOCKETCLOSED:
                        DeleteClient(iClientH, TRUE, TRUE);
                        return;
                }

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, NULL, NULL, NULL);
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

void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char * pItemName)
{
    short sOwnerH;
    char  cOwnerType;

    if (m_pClientList[iClientH] == NULL) return;
    if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
    if ((m_bAdminSecurity == TRUE) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == NULL) return;
    if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount < iAmount) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;
    if (m_pClientList[iClientH]->m_bIsExchangeMode == TRUE) return;
    if (wObjectID >= DEF_MAXCLIENTS) return;

    // ¾ÆÀÌÅÛÀ» ´©±¸¿Í ±³È¯ÇÏ°Ú´Ù´Â ¸Þ½ÃÁö°¡ µµÂøÇß´Ù. »ó´ë¹æ¿¡°Ô ¾Ë¸®°í ¾çÂÊ ¸ðµÎ ±³È¯Ã¢À» ¿­µµ·Ï ÇÑ´Ù. 
    //ÀÌÁ¦ dX, dY¿¡ ÀÖ´Â ¿ÀºêÁ§Æ®¿¡°Ô ¼Òºñ¼º ¾ÆÀÌÅÛÀ» °Ç³×ÁØ´Ù. 
    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);


    if ((sOwnerH != NULL) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
    {

        if ((m_bAdminSecurity == TRUE) && (m_pClientList[sOwnerH]->m_iAdminUserLevel > 0))
        {
            return;
        }

        // v1.4 ÁÖ°íÀÚ ÇÑ °´Ã¼¿Í ¸Â´ÂÁö ÆÇ´ÜÇÑ´Ù.
        if (wObjectID != NULL)
        {
            if (wObjectID < 10000)
            {
                // ÇÃ·¹ÀÌ¾î 
                if (m_pClientList[wObjectID] != NULL)
                {
                    if ((WORD)sOwnerH != wObjectID) sOwnerH = NULL;
                }
            }
            else sOwnerH = NULL;
        }

        if ((sOwnerH == NULL) || (m_pClientList[sOwnerH] == NULL))
        {
            //Â¾Ã†Ã€ÃŒÃ…Ã› Â±Â³ÃˆÂ¯Ã€Â» Â¿Ã¤ÃƒÂ»Ã‡Ã‘ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â¿Â¡Â°Ã” Ã‡Ã˜Â´Ã§ Ã€Â§Ã„Â¡Â¿Â¡ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â¾Ã¸Ã€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™. 
            _ClearExchangeStatus(iClientH);
        }
        else
        {
            if ((m_pClientList[sOwnerH]->m_bIsExchangeMode == TRUE) || (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) ||
                (m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->m_bIsFightZone == TRUE))
            {
                // Â»Ã³Â´Ã«Â¹Ã¦Ã€ÃŒ Ã€ÃŒÂ¹ÃŒ Â±Â³ÃˆÂ¯ ÃÃŸÃ€ÃŒÂ°Ã…Â³Âª Ã€Ã¼Ã…ÃµÂ¸Ã°ÂµÃ¥, ÃˆÂ¤Ã€Âº Â»Ã§Ã…ÃµÃ€Ã¥Ã€ÃŒÂ´Ã™. Â±Â³ÃˆÂ¯Â¸Ã°ÂµÃ¥Â·ÃŽ ÂµÃ©Â¾Ã®Â°Â¥ Â¼Ã¶ Â¾Ã¸Â´Ã™. 
                _ClearExchangeStatus(iClientH);
            }
            else
            {
                // Â±Â³ÃˆÂ¯Â¸Ã°ÂµÃ¥Â°Â¡ Â½ÃƒÃ€Ã›ÂµÃ‡Â¾ÃºÂ´Ã™. Ã€ÃŽÂµÂ¦Â½Âº, Ã€ÃŒÂ¸Â§ Ã€ÃºÃ€Ã¥  
                m_pClientList[iClientH]->m_bIsExchangeMode = TRUE;
                m_pClientList[iClientH]->m_iExchangeH = sOwnerH;
                ZeroMemory(m_pClientList[iClientH]->m_cExchangeName, sizeof(m_pClientList[iClientH]->m_cExchangeName));
                strcpy(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[sOwnerH]->m_cCharName);


                //Clear items in the list
                m_pClientList[iClientH]->iExchangeCount = 0;
                m_pClientList[sOwnerH]->iExchangeCount = 0;
                for (int i = 0; i < 4; i++)
                {
                    //Clear the trader
                    ZeroMemory(m_pClientList[iClientH]->m_cExchangeItemName[i], sizeof(m_pClientList[iClientH]->m_cExchangeItemName[i]));
                    m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
                    m_pClientList[iClientH]->m_iExchangeItemAmount[i] = 0;
                    //Clear the guy we're trading with
                    ZeroMemory(m_pClientList[sOwnerH]->m_cExchangeItemName[i], sizeof(m_pClientList[sOwnerH]->m_cExchangeItemName[i]));
                    m_pClientList[sOwnerH]->m_cExchangeItemIndex[i] = -1;
                    m_pClientList[sOwnerH]->m_iExchangeItemAmount[i] = 0;
                }

                // Â±Â³ÃˆÂ¯Ã‡ÃÂ°Ã­Ã€Ãš Ã‡ÃÂ´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã› Ã€ÃŽÂµÂ¦Â½Âº, Â¼Ã¶Â·Â® Ã€ÃºÃ€Ã¥ 
                m_pClientList[iClientH]->m_cExchangeItemIndex[m_pClientList[iClientH]->iExchangeCount] = (char)sItemIndex;
                m_pClientList[iClientH]->m_iExchangeItemAmount[m_pClientList[iClientH]->iExchangeCount] = iAmount;

                //ZeroMemory(m_pClientList[iClientH]->m_cExchangeItemName, sizeof(m_pClientList[iClientH]->m_cExchangeItemName));
                memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->iExchangeCount], m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, 20);

                m_pClientList[sOwnerH]->m_bIsExchangeMode = TRUE;
                m_pClientList[sOwnerH]->m_iExchangeH = iClientH;
                ZeroMemory(m_pClientList[sOwnerH]->m_cExchangeName, sizeof(m_pClientList[sOwnerH]->m_cExchangeName));
                strcpy(m_pClientList[sOwnerH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName);

                m_pClientList[iClientH]->iExchangeCount++;
                SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex + 1000, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);

                SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
                    m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);
            }
        }
    }
    else
    {
        // NPCÂ¿ÃÂ´Ã‚ Â¹Â°Â°Ã‡Ã€Â» Â±Â³ÃˆÂ¯Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        _ClearExchangeStatus(iClientH);

    }
}

void CGame::QuestAcceptedHandler(int iClientH)
{
    int iIndex;

    if (m_pClientList[iClientH] == NULL) return;

    // Does the quest exist ??
    if (m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest] == NULL) return;

    if (m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_iAssignType == 1)
    {
        // Ã€Ã¼Â¸Ã©Ã€Ã¼Â¿Ã« Ã„Ã¹Â½ÂºÃ†Â®Ã€ÃŒÂ´Ã™. 
        switch (m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_iType)
        {
            case 10: // Ã…ÃšÂ·Â¹Ã†Ã·Ã†Â®Â¿Ã« 1ÃˆÂ¸Â¼Âº Ã„Ã¹Â½ÂºÃ†Â®. Ã…ÃšÂ·Â¹Ã†Ã·Ã†Â®Ã‡ÃÂ°Ã­ Â³ÂªÂ¸Ã© Â³Â¡Ã€ÃŒÂ´Ã™. Â´ÃµÃ€ÃŒÂ»Ã³Ã€Ã‡ Ã„Ã¹Â½ÂºÃ†Â® Â¿Â©ÂºÃŽÂ°Â¡ Â¾Ã¸Ã€Â½.
                _ClearQuestStatus(iClientH);
                RequestTeleportHandler(iClientH, "2   ", m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_cTargetName,
                    m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_sX, m_pQuestConfigList[m_pClientList[iClientH]->m_iAskedQuest]->m_sY);
                return;
        }
    }

    // Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Ã„Ã¹Â½ÂºÃ†Â®Â¸Â¦ Â¼Ã¶Â¶Ã´Ã‡ÃŸÂ´Ã™. Ã„Ã¹Â½ÂºÃ†Â® Â¹Ã¸ÃˆÂ£, IDÂ°Âª Ã‡Ã’Â´Ã§.
    m_pClientList[iClientH]->m_iQuest = m_pClientList[iClientH]->m_iAskedQuest;
    iIndex = m_pClientList[iClientH]->m_iQuest;
    m_pClientList[iClientH]->m_iQuestID = m_pQuestConfigList[iIndex]->m_iQuestID;
    m_pClientList[iClientH]->m_iCurQuestCount = 0;
    m_pClientList[iClientH]->m_bIsQuestCompleted = FALSE;

    _CheckQuestEnvironment(iClientH);
    _SendQuestContents(iClientH);
}

void CGame::_TamingHandler(int iClientH, int iSkillNum, char cMapIndex, int dX, int dY)
{
    int iSkillLevel, iRange, iTamingLevel, iResult, iX, iY;
    short sOwnerH;
    char  cOwnerType;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pMapList[cMapIndex] == NULL) return;

    iSkillLevel = (int)m_pClientList[iClientH]->m_cSkillMastery[iSkillNum];
    iRange = iSkillLevel / 12; // ÃƒÃ–Â´Ã« Â¹Ã¼Ã€Â§Â´Ã‚ 8

    for (iX = dX - iRange; iX <= dX + iRange; iX++)
        for (iY = dY - iRange; iY <= dY + iRange; iY++)
        {
            sOwnerH = NULL;
            if ((iX > 0) && (iY > 0) && (iX < m_pMapList[cMapIndex]->m_sSizeX) && (iY < m_pMapList[cMapIndex]->m_sSizeY))
                m_pMapList[cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, iX, iY);

            if (sOwnerH != NULL)
            {
                switch (cOwnerType)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        if (m_pClientList[sOwnerH] == NULL) break;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        if (m_pNpcList[sOwnerH] == NULL) break;
                        // Â¸Ã³Â½ÂºÃ…ÃÃ€Ã‡ ÃÂ¾Â·Ã¹Â¿Â¡ ÂµÃ»Â¶Ã³ Â±Ã¦ÂµÃ©Â¿Â©ÃÃº Â°ÃÃ€ÃŽÃÃ¶Â¸Â¦ Â°Ã¡ÃÂ¤Ã‡Ã‘Â´Ã™.
                        iTamingLevel = 10;
                        switch (m_pNpcList[sOwnerH]->m_sType)
                        {
                            case 10:							// Â½Â½Â¶Ã³Ã€Ã“  
                            case 16: iTamingLevel = 1; break;	// Â°Â³Â¹ÃŒ
                            case 22: iTamingLevel = 2; break;	// Â¹Ã¬
                            case 17:							// Ã€Ã¼Â°Â¥
                            case 14: iTamingLevel = 3; break;	// Â¿Ã€Ã…Â©
                            case 18: iTamingLevel = 4; break;   // ÃÂ»ÂºÃ±
                            case 11: iTamingLevel = 5; break;	// Â½ÂºÃ„ÃŒÂ·Â¹Ã…Ã¦
                            case 23:
                            case 12: iTamingLevel = 6; break;	// Â°Ã±Â·Â½
                            case 28: iTamingLevel = 7; break;	// Ã†Â®Â·Ã‘
                            case 13:							// Â½ÃŽÃ€ÃŒÃ…Â¬Â·Ã“Â½Âº
                            case 27: iTamingLevel = 8; break;	// Ã‡Ã¯Ã‡ÃÂ¿Ã®ÂµÃ¥
                            case 29: iTamingLevel = 9; break;	// Â¿Ã€Â¿Ã¬Â°Ã…
                            case 33: iTamingLevel = 9; break;	// Â¿Ã¾Â¾Ã®Â¿Ã¯Ã‡Ã
                            case 30: iTamingLevel = 9; break;  // Â¸Â®Ã„Â¡
                            case 31:
                            case 32: iTamingLevel = 10; break;  // ÂµÂ¥Â¸Ã³, Ã€Â¯Â´ÃÃ„Ãœ
                        }

                        iResult = (iSkillLevel / 10);

                        // Â½ÂºÃ…Â³Ã€Ã‡ ÂµÃ®Â±ÃžÂ¿Â¡ ÂµÃ»Â¶Ã³ Â±Ã¦ÂµÃ©Ã€Ã Â¼Ã¶ Ã€Ã–Â´Ã‚ Â°Â¹Â¼Ã¶Â°Â¡ Â´ÃžÂ¶Ã³ÃÃ¸Â´Ã™.
                        if (iResult < iTamingLevel) break;

                        break;
                }
            }
        }
}

void CGame::ActivateSpecialAbilityHandler(int iClientH)
{
    DWORD dwTime = timeGetTime();
    short sTemp;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iSpecialAbilityTime != 0) return;
    if (m_pClientList[iClientH]->m_iSpecialAbilityType == 0) return;
    if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == TRUE) return;

    m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = TRUE;
    m_pClientList[iClientH]->m_dwSpecialAbilityStartTime = dwTime;

    m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;

    sTemp = m_pClientList[iClientH]->m_sAppr4;
    sTemp = sTemp & 0xFF0F;
    switch (m_pClientList[iClientH]->m_iSpecialAbilityType)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            sTemp = sTemp | 0x0010;
            break;
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
            sTemp = sTemp | 0x0020;
            break;
    }
    m_pClientList[iClientH]->m_sAppr4 = sTemp;

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 1, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityLastSec, NULL);
    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
}

void CGame::CancelQuestHandler(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;

    // Ã„Ã¹Â½ÂºÃ†Â® Â»Ã³Ã…Ã‚Â¸Â¦ Ã…Â¬Â¸Â®Â¾Ã®Ã‡Ã‘Â´Ã™.
    _ClearQuestStatus(iClientH);
    // Ã„Ã¹Â½ÂºÃ†Â®Â°Â¡ ÃƒÃ«Â¼Ã’ÂµÃ‡Â¾ÃºÃ€Â½Ã€Â» Â¾Ã‹Â·ÃÃÃ˜Â´Ã™.
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTABORTED, NULL, NULL, NULL, NULL);
}

void CGame::MapStatusHandler(int iClientH, int iMode, char * pMapName)
{
    int i;

    if (m_pClientList[iClientH] == NULL) return;

    switch (iMode)
    {
        case 1:
            if (m_pClientList[iClientH]->m_iCrusadeDuty == NULL) return;

            for (i = 0; i < DEF_MAXGUILDS; i++)
                if ((m_pGuildTeleportLoc[i].m_iV1 != NULL) && (m_pGuildTeleportLoc[i].m_iV1 == m_pClientList[iClientH]->m_iGuildGUID))
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_TCLOC, m_pGuildTeleportLoc[i].m_sDestX, m_pGuildTeleportLoc[i].m_sDestY,
                        NULL, m_pGuildTeleportLoc[i].m_cDestMapName, m_pGuildTeleportLoc[i].m_sDestX2, m_pGuildTeleportLoc[i].m_sDestY2,
                        NULL, NULL, NULL, NULL, m_pGuildTeleportLoc[i].m_cDestMapName2);
                    // °Ç¼³ À§Ä¡¸¦ ÀúÀåÇØ ³õ´Â´Ù.
                    ZeroMemory(m_pClientList[iClientH]->m_cConstructMapName, sizeof(m_pClientList[iClientH]->m_cConstructMapName));
                    memcpy(m_pClientList[iClientH]->m_cConstructMapName, m_pGuildTeleportLoc[i].m_cDestMapName2, 10);
                    m_pClientList[iClientH]->m_iConstructLocX = m_pGuildTeleportLoc[i].m_sDestX2;
                    m_pClientList[iClientH]->m_iConstructLocY = m_pGuildTeleportLoc[i].m_sDestY2;
                    return;
                }

            // ±æµå ¼Ò¼ÓÀÌ ¾Æ´Ñ ±ºÀÎÀÇ ¿äÃ»ÀÌ´Ù.
            break;

        case 3:
            //if (m_pClientList[iClientH]->m_iCrusadeDuty != 3) return;
            // µ¥ÀÌÅÍ Ä³½Ã ±¸Á¶Ã¼ ÃÊ±âÈ­ 
            for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
            {
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = NULL;
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = NULL;
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = NULL;
                m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = NULL;
            }
            m_pClientList[iClientH]->m_iCSIsendPoint = NULL;
            ZeroMemory(m_pClientList[iClientH]->m_cSendingMapName, sizeof(m_pClientList[iClientH]->m_cSendingMapName));

            if (strcmp(pMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName) == 0)
            {
                // ÇöÀç »ç¿ëÀÚ°¡ À§Ä¡ÇÑ ¸ÊÀÇ Á¤º¸¸¦ ¿ä±¸Çß´Ù.
                // ¸Ê µ¥ÀÌÅÍ¸¦ ¿äÃ»ÇÒ ´ç½ÃÀÇ ³»¿ëÀ» ±×´ë·Î Ä«ÇÇÇÑ´Ù.
                for (i = 0; i < m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalCrusadeStructures; i++)
                {
                    // v2.15 »ç¿ëÀÚ°¡ Commander °¡ ¾Æ´Ï¾îµµ °Ç¹°ÀÇ À§Ä¡¸¦ º¸¿©ÁØ´Ù.
                    if (m_pClientList[iClientH]->m_iCrusadeDuty == 3)
                    {
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cType;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cSide;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sX;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sY;
                    }
                    else if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cType == 42)
                    {
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cType;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].cSide;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sX;
                        m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stCrusadeStructureInfo[i].sY;
                    }
                }
                memcpy(m_pClientList[iClientH]->m_cSendingMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 10);
            }
            else
            {
                // ´Ù¸¥ ¸ÊÀÇ »óÈ²À» ¿äÃ»Çß´Ù. ¹Ìµé·£µåÀÇ °æ¿ì´Â °¡´É 
                if (strcmp(pMapName, "middleland") == 0)
                {
                    for (i = 0; i < m_iTotalMiddleCrusadeStructures; i++)
                    {
                        // v2.15 »ç¿ëÀÚ°¡ Commander °¡ ¾Æ´Ï¾îµµ °Ç¹°ÀÇ À§Ä¡¸¦ º¸¿©ÁØ´Ù.
                        if (m_pClientList[iClientH]->m_iCrusadeDuty == 3)
                        {
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_stMiddleCrusadeStructureInfo[i].cType;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_stMiddleCrusadeStructureInfo[i].cSide;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_stMiddleCrusadeStructureInfo[i].sX;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_stMiddleCrusadeStructureInfo[i].sY;
                        }
                        else if (m_stMiddleCrusadeStructureInfo[i].cType == 42)
                        {
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cType = m_stMiddleCrusadeStructureInfo[i].cType;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].cSide = m_stMiddleCrusadeStructureInfo[i].cSide;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sX = m_stMiddleCrusadeStructureInfo[i].sX;
                            m_pClientList[iClientH]->m_stCrusadeStructureInfo[i].sY = m_stMiddleCrusadeStructureInfo[i].sY;
                        }
                    }
                    strcpy(m_pClientList[iClientH]->m_cSendingMapName, "middleland");
                }
                else
                {
                    // ¾ÆÁ÷ Áö¿øµÇÁö ¾Ê´Â ±â´ÉÀÌ´Ù.
                }
            }

            _SendMapStatus(iClientH);
            break;
    }
}

void CGame::RequestHelpHandler(int iClientH)
{
    // Ã‡Ã¯Ã‡Ã Â¿Ã¤ÃƒÂ»Ã€ÃŒÂ´Ã™. Â±Ã¦ÂµÃ¥ Â¸Â¶Â½ÂºÃ…ÃÂ¿Â¡Â°Ã” ÃÃ‚Ã‡Â¥Â¸Â¦ Ã€Ã¼Â´ÃžÃ‡Ã˜ ÃÃ˜Â´Ã™.
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iGuildRank == -1) return;
    if (m_pClientList[iClientH]->m_iCrusadeDuty != 1) return;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iGuildRank == 0) &&
            (m_pClientList[i]->m_iCrusadeDuty == 3) && (m_pClientList[i]->m_iGuildGUID == m_pClientList[iClientH]->m_iGuildGUID))
        {
            // Â°Â°Ã€Âº Â±Ã¦ÂµÃ¥ Â¸Â¶Â½ÂºÃ…ÃÃ€ÃŒÂ°Ã­ ÃÃ¶ÃˆÃ–Â°Ã¼ Â¿ÂªÃˆÂ°Ã€Â» Â¸ÃƒÃ€Âº Â»Ã³Ã…Ã‚Ã€ÃŒÂ´Ã™.
            SendNotifyMsg(NULL, i, DEF_NOTIFY_HELP, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_iHP, m_pClientList[iClientH]->m_cCharName);
            return;
        }

    // Ã‡Ã¶Ã€Ã§ Â¸ÃŠÂ¿Â¡ ÃÃ¶ÃˆÃ–Â°Ã¼Ã€ÃŒ Â¾Ã¸Ã€Â½Ã€Â» Ã…Ã«ÂºÂ¸Ã‡Ã˜ ÃÃ˜Â´Ã™.
    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_HELPFAILED, NULL, NULL, NULL, NULL);
}

void CGame::ServerStockMsgHandler(char * pData)
{
    char * cp, * cp2, cTemp[120], cLocation[10], cGuildName[20], cName[11], cTemp2[120], cTemp3[120], cMapName[11], cBuffer[256]; short * sp;
    WORD * wp, wServerID, wClientH, wV1, wV2, wV3, wV4, wV5;
    DWORD * dwp;
    BOOL bFlag = FALSE;
    int * ip, i, iTotal, iV1, iV2, iV3, iRet;
    short sX, sY;

    iTotal = 0;
    cp = (char *)(pData + 6);
    while (bFlag == FALSE)
    {
        iTotal++;
        switch (*cp)
        {

            // SUMMONGUILD Deleted
            case GSM_REQUEST_SUMMONGUILD:
                cp++;
                ZeroMemory(cGuildName, sizeof(cGuildName));
                memcpy(cGuildName, cp, 20);
                cp += 20;

                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;

                cp2 = (char *)cTemp + 10;
                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                wp = (WORD *)cp;
                wV2 = *wp;
                cp += 2;

                for (i = 0; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cGuildName, cGuildName) == 0))
                    {
                        RequestTeleportHandler(i, "2   ", cTemp, wV1, wV2);
                    }
                break;

                // v2.14 Â¼ÂºÃˆÃ„Â´Ã ÃƒÃŸÂ°Â¡ Ã€Â¯Ã€Ãº Â¼Ã’ÃˆÂ¯ 
            case GSM_REQUEST_SUMMONPLAYER:
                cp++;
                ZeroMemory(cName, sizeof(cName));
                memcpy(cName, cp, 10);
                cp += 10;

                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;

                cp2 = (char *)cTemp + 10;

                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                wp = (WORD *)cp;
                wV2 = *wp;
                cp += 2;

                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cCharName, cName) == 0))
                    {
                        //wsprintf(G_cTxt, "%s %d %d", cTemp, wV1, wV2);
                        //log->info(G_cTxt);					
                        RequestTeleportHandler(i, "2   ", cTemp, wV1, wV2);
                        break;
                    }
                break;

            case GSM_REQUEST_SUMMONALL:
                cp++;
                ZeroMemory(cLocation, sizeof(cLocation));
                memcpy(cLocation, cp, 10);
                cp += 10;

                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;

                cp2 = (char *)cTemp + 10;

                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                wp = (WORD *)cp;
                wV2 = *wp;
                cp += 2;

                for (i = 0; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cLocation, cLocation) == 0))
                    {
                        RequestTeleportHandler(i, "2   ", cTemp, wV1, wV2);
                    }
                break;

            case GSM_CHATMSG:
                cp++;
                ZeroMemory(cTemp, sizeof(cTemp));
                ZeroMemory(cName, sizeof(cName));
                iV1 = *cp;
                cp++;
                ip = (int *)cp;
                iV2 = *ip;
                cp += 4;
                memcpy(cName, cp, 10);
                cp += 10;;
                sp = (short *)cp;
                wV1 = (WORD)*sp;
                cp += 2;
                ChatMsgHandlerGSM(iV1, iV2, cName, cp, wV1);
                cp += wV1;
                break;

            case GSM_CONSTRUCTIONPOINT:
                cp++;
                ip = (int *)cp;
                iV1 = *ip;
                cp += 4;
                ip = (int *)cp;
                iV2 = *ip;
                cp += 4;
                GSM_ConstructionPoint(iV1, iV2);
                break;

            case GSM_SETGUILDTELEPORTLOC:
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
                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;
                GSM_SetGuildTeleportLoc(iV1, iV2, iV3, cTemp);
                break;

            case GSM_SETGUILDCONSTRUCTLOC:
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
                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;
                GSM_SetGuildConstructLoc(iV1, iV2, iV3, cTemp);
                break;

            case GSM_REQUEST_SETFORCERECALLTIME:
                cp++;
                wp = (WORD *)cp;
                m_sForceRecallTime = *wp;
                cp += 2;

                wsprintf(G_cTxt, "(!) Game Server Force Recall Time (%d)min", m_sForceRecallTime);
                log->info(G_cTxt);
                break;

            case GSM_MIDDLEMAPSTATUS:
                cp++;
                // Â±Â¸ÃÂ¶ÃƒÂ¼ Ã…Â¬Â¸Â®Â¾Ã®
                for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
                {
                    m_stMiddleCrusadeStructureInfo[i].cType = NULL;
                    m_stMiddleCrusadeStructureInfo[i].cSide = NULL;
                    m_stMiddleCrusadeStructureInfo[i].sX = NULL;
                    m_stMiddleCrusadeStructureInfo[i].sY = NULL;
                }
                sp = (short *)cp;
                m_iTotalMiddleCrusadeStructures = *sp;
                cp += 2;
                // ÃÂ¤ÂºÂ¸ Ã€ÃÂ¾Ã®ÂµÃ©Ã€ÃŽÂ´Ã™.
                for (i = 0; i < m_iTotalMiddleCrusadeStructures; i++)
                {
                    m_stMiddleCrusadeStructureInfo[i].cType = *cp;
                    cp++;
                    m_stMiddleCrusadeStructureInfo[i].cSide = *cp;
                    cp++;
                    sp = (short *)cp;
                    m_stMiddleCrusadeStructureInfo[i].sX = *sp;
                    cp += 2;
                    sp = (short *)cp;
                    m_stMiddleCrusadeStructureInfo[i].sY = *sp;
                    cp += 2;
                }
                break;

            case GSM_BEGINCRUSADE:
                cp++;
                dwp = (DWORD *)cp;
                cp += 4;
                LocalStartCrusadeMode(*dwp);
                break;

            case GSM_BEGINAPOCALYPSE:
                cp++;
                dwp = (DWORD *)cp;
                cp += 4;
                LocalStartApocalypse(*dwp);
                break;

            case GSM_STARTHELDENIAN:
                cp++;
                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;
                wp = (WORD *)cp;
                wV2 = *wp;
                cp += 2;
                dwp = (DWORD *)cp;
                cp += 4;
                LocalStartHeldenianMode(wV1, wV2, *dwp);
                break;

            case GSM_ENDHELDENIAN:
                cp++;
                LocalEndHeldenianMode();
                break;

                /*case GSM_BEGINHELDENIEN:
                cp++;
                dwp = (DWORD *)cp;
                cp += 4;
                LocalStartHeldenian(*dwp);
                break;

            case GSM_ENDHeldenian:
                cp++;
                LocalEndHeldenian(*dwp)//sub_4ABCD0(*dwp);
                break;*/

            case GSM_UPDATECONFIGS:
                cp++;
                LocalUpdateConfigs(*cp);
                cp += 16;
                break;

            case GSM_ENDAPOCALYPSE:
                cp++;
                LocalEndApocalypse();
                break;

            case GSM_ENDCRUSADE:
                cp++;
                LocalEndCrusadeMode(*cp);
                cp += 16;
                break;

            case GSM_COLLECTEDMANA:
                cp++;
                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                wp = (WORD *)cp;
                wV2 = *wp;
                cp += 2;
                CollectedManaHandler(wV1, wV2);
                break;

            case GSM_GRANDMAGICLAUNCH:
                cp++;
                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                wp = (WORD *)cp;
                wV2 = *wp;
                cp += 2;

                switch (wV1)
                {
                    case 1: // Â¸ÃžÃ…Ã—Â¿Ã€ Â½ÂºÃ†Â®Â¶Ã³Ã€ÃŒÃ…Â© 
                        MeteorStrikeMsgHandler((char)wV2);
                        break;
                }
                break;

            case  GSM_GRANDMAGICRESULT:
                cp++;
                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;
                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;
                wp = (WORD *)cp;
                wV2 = *wp;
                cp += 2;
                wp = (WORD *)cp;
                wV3 = *wp;
                cp += 2;
                wp = (WORD *)cp;
                wV4 = *wp;
                cp += 2;

                wp = (WORD *)cp;
                wV5 = *wp;

                GrandMagicResultHandler(cTemp, wV1, wV2, wV3, wV4, wV5, cp);
                break;

                // v2.15 2002-5-21
            case GSM_REQUEST_SHUTUPPLAYER:
                cp++;
                wp = (WORD *)cp;
                wServerID = *wp;
                cp += 2;
                wp = (WORD *)cp;
                wClientH = *wp;
                cp += 2;

                ZeroMemory(cName, sizeof(cName));
                memcpy(cName, cp, 10);
                cp += 10;

                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;

                GSM_RequestShutupPlayer(cTemp, wServerID, wClientH, wV1, cName);
                break;

            case GSM_RESPONSE_SHUTUPPLAYER:
                cp++;
                wp = (WORD *)cp;
                wServerID = *wp;
                cp += 2;
                wp = (WORD *)cp;
                wClientH = *wp;
                cp += 2;

                ZeroMemory(cName, sizeof(cName));
                memcpy(cName, cp, 10);
                cp += 10;

                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;


                if (wServerID == m_wServerID_GSS)
                {
                    if ((m_pClientList[wClientH] != NULL) && (strcmp(m_pClientList[wClientH]->m_cCharName, cName) == 0) && (m_pClientList[wClientH]->m_iAdminUserLevel > 0))
                    {
                        SendNotifyMsg(NULL, wClientH, DEF_NOTIFY_PLAYERSHUTUP, wV1, NULL, NULL, cTemp);

                    }
                }
                break;

            case GSM_WHISFERMSG:
                ZeroMemory(cName, sizeof(cName));
                ZeroMemory(cBuffer, sizeof(cBuffer));
                cp++;
                memcpy(cName, cp, 10);
                cp += 10;
                wp = (WORD *)cp;
                wV1 = *wp;
                cp += 2;

                memcpy(cBuffer, cp, wV1);
                cp += wV1;

                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cCharName, cName) == 0))
                    {
                        iRet = m_pClientList[i]->iSendMsg(cBuffer, wV1);
                        if (m_pClientList[i]->m_iAdminUserLevel > 0)
                        {
                            char cTxt[200], cTmpName[12];
                            ZeroMemory(cTxt, sizeof(cTxt));
                            ZeroMemory(cTmpName, sizeof(cTmpName));

                            memcpy(cTmpName, cBuffer + 10, 10);
                            wsprintf(cTxt, "PC Chat(%s):\"%s\"\tto GM(%s)", cTmpName, cBuffer + 21, m_pClientList[i]->m_cCharName);
                            //bSendMsgToLS(MSGID_GAMEMASTERLOG, i, NULL, cTxt);
                        }
                        break;
                    }
                break;

            case GSM_REQUEST_FINDCHARACTER:
                cp++;

                wp = (WORD *)cp;
                wServerID = *wp;
                cp += 2;

                wp = (WORD *)cp;
                wClientH = *wp;
                cp += 2;

                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;

                ZeroMemory(cTemp2, sizeof(cTemp2));
                memcpy(cTemp2, cp, 10);
                cp += 10;

                GSM_RequestFindCharacter(wServerID, wClientH, cTemp, cTemp2);
                break;

            case GSM_RESPONSE_FINDCHARACTER:
                sX = -1;
                sY = -1;
                cp++;
                wp = (WORD *)cp;
                wServerID = *wp;
                cp += 2;
                wp = (WORD *)cp;
                wClientH = *wp;
                cp += 2;
                ZeroMemory(cTemp, sizeof(cTemp));
                memcpy(cTemp, cp, 10);
                cp += 10;

                ZeroMemory(cTemp2, sizeof(cTemp2));
                memcpy(cTemp2, cp, 10);
                cp += 10;

                ZeroMemory(cTemp3, sizeof(cTemp3));
                memcpy(cTemp3, cp, 14);

                if ((wServerID == m_wServerID_GSS) && m_pClientList[wClientH] != NULL)
                {
                    if (m_pClientList[wClientH]->m_bIsAdminOrderGoto == TRUE)
                    {
                        m_pClientList[wClientH]->m_bIsAdminOrderGoto = FALSE;
                        ZeroMemory(cMapName, sizeof(cMapName));
                        memcpy(cMapName, cp, 10);
                        cp += 10;

                        wp = (WORD *)cp;
                        sX = *wp;
                        cp += 2;

                        wp = (WORD *)cp;
                        sY = *wp;
                        cp += 2;

                        if (m_pClientList[wClientH]->m_iAdminUserLevel > 0)
                        {
                            if (sX == -1 && sY == -1)
                            {
                                wsprintf(G_cTxt, "GM Order(%s): GoTo MapName(%s)", m_pClientList[wClientH]->m_cCharName, cMapName);
                            }
                            else
                            {
                                wsprintf(G_cTxt, "GM Order(%s): GoTo MapName(%s)(%d %d)", m_pClientList[wClientH]->m_cCharName,
                                    cMapName, sX, sY);
                            }
                            //bSendMsgToLS(MSGID_GAMEMASTERLOG, wClientH, FALSE, G_cTxt);
                            RequestTeleportHandler(wClientH, "2   ", cMapName, sX, sY);
                        }
                    }
                    else
                    {
                        if ((m_pClientList[wClientH]->m_bIsCheckingWhisperPlayer == TRUE) && (strcmp(m_pClientList[wClientH]->m_cWhisperPlayerName, cTemp) == 0))
                        {
                            m_pClientList[wClientH]->m_iWhisperPlayerIndex = 10000;
                            SendNotifyMsg(NULL, wClientH, DEF_NOTIFY_WHISPERMODEON, NULL, NULL, NULL, m_pClientList[wClientH]->m_cWhisperPlayerName);
                        }
                        else
                        {
                            if (m_pClientList[wClientH]->m_iAdminUserLevel == 0)
                            {
                                ZeroMemory(cTemp3, sizeof(cTemp3));
                            }
                            if (strcmp(m_pClientList[wClientH]->m_cCharName, cTemp2) == 0)
                            {
                                SendNotifyMsg(NULL, wClientH, DEF_NOTIFY_PLAYERONGAME, NULL, NULL, NULL, cTemp,
                                    NULL, NULL, NULL, NULL, NULL, NULL, cTemp3);
                            }
                        }
                    }
                }
                break;

            default:
                bFlag = TRUE;
                break;
        }
    }

    //testcode
    //wsprintf(G_cTxt, "(!) Total %d GSM Messages.", iTotal-1);
    //log->info(G_cTxt);
}

void CGame::DoMeteorStrikeDamageHandler(int iMapIndex)
{
    int i, iDamage;

    // Ã‡Ã˜Â´Ã§ Â¸ÃŠÃ€Ã‡ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®ÂµÃ©Ã€Ã‡ HPÂ¸Â¦ Ã€ÃÂ°Ã½ Â»Ã¨Â°Â¨
    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_cSide != 0) && (m_pClientList[i]->m_cMapIndex == iMapIndex))
        {
            if (m_pClientList[i]->m_iLevel < 80)
                iDamage = m_pClientList[i]->m_iLevel + iDice(1, 10);
            else iDamage = m_pClientList[i]->m_iLevel * 2 + iDice(1, 10);
            iDamage = iDice(1, m_pClientList[i]->m_iLevel) + m_pClientList[i]->m_iLevel;
            // ÃƒÃ–Â´Ã« 255Ã€ÃŒÂ»Ã³Ã€Ã‡ Â´Ã«Â¹ÃŒÃÃ¶Â´Ã‚ Â¹ÃžÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
            if (iDamage > 255) iDamage = 255;

            if (m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
            { //magic cut in half
                iDamage = (iDamage / 2) - 2;
            }

            if (m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 5)
            {
                iDamage = 0;
            }

            if (m_pClientList[i]->m_iAdminUserLevel > 0)
            { // Admins get no damage
                iDamage = 0;
            }

            m_pClientList[i]->m_iHP -= iDamage;
            if (m_pClientList[i]->m_iHP <= 0)
            {
                // Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
                ClientKilledHandler(i, NULL, NULL, iDamage);
                m_stMeteorStrikeResult.iCasualties++;
            }
            else
            {
                if (iDamage > 0)
                {
                    // Â¹ÃžÃ€Âº Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Ã…Ã«ÂºÂ¸Ã‡Ã‘Â´Ã™. <- HPÂ¸Â¦ Â±Ã—Â´Ã«Â·ÃŽ Â¾Ã‹Â¸Â°Â´Ã™.
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                    // ÃƒÃ¦Â°ÃÃ€Â» Â¹ÃžÂ¾Ã’Â´Ã™Â¸Ã© ÃƒÃ¦Â°ÃÂµÂ¿Ã€Ã› Ã€Ã¼Â¼Ã› 
                    SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, NULL, NULL);

                    // v1.4 ÃÃ—Ã€ÂºÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Â°Ã¦Â¿Ã¬Â´Ã‚ Owner Ã€Â§Ã„Â¡Â¸Â¦ Â¿Ã…Â±Ã¤Â´Ã™. 
                    if (m_pClientList[i]->m_bSkillUsingStatus[19] != TRUE)
                    {
                        m_pMapList[m_pClientList[i]->m_cMapIndex]->ClearOwner(0, i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                        m_pMapList[m_pClientList[i]->m_cMapIndex]->SetOwner(i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                    }

                    if (m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)
                    {
                        // Hold-Person ÂµÃ‡Â¾Ã® Ã€Ã–Â¾ÃºÂ´Ã¸ Â»Ã³Ã…Ã‚Â¶Ã³Â¸Ã© Ã‡Â®Â¸Â°Â´Ã™. Â¸Â¶Â¹Ã½Â°Ã¸Â°Ã Ã†ÃÂ·Â²Â¶Ã³Ã€ÃŒÃÃ® ÂµÃˆ Â°ÃÂµÂµ Ã‡Â®Â¸Â°Â´Ã™.
                        // 1: Hold-Person 
                        // 2: Paralize
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT], NULL, NULL);

                        m_pClientList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = NULL;
                        bRemoveFromDelayEventList(i, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
                    }
                }
            }
        }
}

void CGame::RequestGuildNameHandler(int iClientH, int iObjectID, int iIndex)
{
    if (m_pClientList[iClientH] == NULL) return;
    if ((iObjectID <= 0) || (iObjectID >= DEF_MAXCLIENTS)) return;

    if (m_pClientList[iObjectID] == NULL)
    {
        // Â¿Ã¤ÃƒÂ» Â¹ÃžÃ€Âº ObjectÂ°Â¡ Â¾Ã¸Â´Ã™.

    }
    else
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_REQGUILDNAMEANSWER, m_pClientList[iObjectID]->m_iGuildRank, iIndex, NULL, m_pClientList[iObjectID]->m_cGuildName);
    }
}

void CGame::RequestGuildTeleportHandler(int iClientH)
{
    int i;
    char cMapName[11];

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iLockedMapTime != NULL)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_LOCKEDMAP, m_pClientList[iClientH]->m_iLockedMapTime, NULL, NULL, m_pClientList[iClientH]->m_cLockedMapName);
        return;
    }

    // if a guild teleport is set when its not a crusade, log the hacker
    if (!m_bIsCrusadeMode)
    {
        try
        {
            wsprintf(G_cTxt, "Accessing crusade teleport: (%s) Player: (%s) - setting teleport location when crusade is disabled.",
                m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {
        }
        return;
    }

    // if a player is using guild teleport and he is not in a guild, log the hacker
    if (m_pClientList[iClientH]->m_iCrusadeDuty == 0)
    {
        try
        {
            wsprintf(G_cTxt, "Accessing crusade teleport: (%s) Player: (%s) - teleporting when not in a guild",
                m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {
        }
        return;
    }

    if ((m_pClientList[iClientH]->m_cMapIndex == m_iMiddlelandMapIndex) &&
        m_iMiddlelandMapIndex != -1)
        return;

    for (i = 0; i < DEF_MAXGUILDS; i++)
        if (m_pGuildTeleportLoc[i].m_iV1 == m_pClientList[iClientH]->m_iGuildGUID)
        {
            // ±æµå¸¦ Ã£¾Ò´Ù.
            ZeroMemory(cMapName, sizeof(cMapName));
            strcpy(cMapName, m_pGuildTeleportLoc[i].m_cDestMapName);

            //testcode
            wsprintf(G_cTxt, "ReqGuildTeleport: %d %d %d %s", m_pClientList[iClientH]->m_iGuildGUID, m_pGuildTeleportLoc[i].m_sDestX, m_pGuildTeleportLoc[i].m_sDestY, cMapName);
            log->info(G_cTxt);

            // !!! RequestTeleportHandler³»¿¡¼­ m_cMapNameÀ» ¾²±â ¶§¹®¿¡ ±×´ë·Î ÆÄ¶ó¹ÌÅÍ·Î ³Ñ°ÜÁÖ¸é ¿Àµ¿ÀÛ
            RequestTeleportHandler(iClientH, "2   ", cMapName, m_pGuildTeleportLoc[i].m_sDestX, m_pGuildTeleportLoc[i].m_sDestY);
            return;
        }

    // ±æµå¿¡ ÇÒ´çµÈ ÅÚ·¹Æ÷Æ® ÁÂÇ¥°¡ ¾ø´Ù. ±æµå ¼Ò¼ÓÀÌ ¾Æ´Ñ °æ¿ì
    switch (m_pClientList[iClientH]->m_cSide)
    {
        case 1: // ¾Æ·¹½ºµ§ ¼Ò¼Ó 
            break;
        case 2: // ¿¤¹ÙÀÎ ¼Ò¼Ó
            break;
    }
}

void CGame::RequestSetGuildTeleportLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName)
{
    char * cp, cData[120];
    int i;
    int * ip, iIndex;
    DWORD dwTemp, dwTime;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsOnServerChange == TRUE) return;

    // if a player is teleporting and its not a crusade, log the hacker
    if (!m_bIsCrusadeMode)
    {
        try
        {
            wsprintf(G_cTxt, "Accessing Crusade Set Teleport:(%s) Player: (%s) - setting point when not a crusade.",
                m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {

        }
        return;
    }

    // if a player is teleporting and its not a crusade, log the hacker
    if (m_pClientList[iClientH]->m_iCrusadeDuty != 3)
    {
        try
        {
            wsprintf(G_cTxt, "Accessing Crusade Set Teleport: (%s) Player: (%s) - setting point when not a guildmaster.",
                m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {

        }
        return;
    }

    // v2.04 ÁÂÇ¥ º¸Á¤ 
    if (dY < 100) dY = 100;
    if (dY > 600) dY = 600;

    // °ÔÀÌÆ® ¼­¹ö ¸Þ½ÃÁö ÀÛ¼º 
    ZeroMemory(cData, sizeof(cData));
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

    // ¸ÕÀú °°Àº GUID¸¦ °¡Áø ±æµå ÅÚ·¹Æ÷Æ® ÁÂÇ¥°¡ ¼³Á¤µÇ¾î ÀÖ´ÂÁö Ã£´Â´Ù.
    for (i = 0; i < DEF_MAXGUILDS; i++)
        if (m_pGuildTeleportLoc[i].m_iV1 == iGuildGUID)
        {
            // ÀÌ¹Ì ±æµå°¡ ÀÖ´Ù.
            if ((m_pGuildTeleportLoc[i].m_sDestX == dX) && (m_pGuildTeleportLoc[i].m_sDestY == dY) && (strcmp(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName) == 0))
            {
                // ³»¿ëÀÌ ÀÏÄ¡ÇÑ´Ù. Å¸ÀÓ ÀÔ·ÂÈÄ ½ºÅµ.
                m_pGuildTeleportLoc[i].m_dwTime = dwTime;
                return;
            }
            else
            {
                // ³»¿ë °»½ÅÈÄ 
                m_pGuildTeleportLoc[i].m_sDestX = dX;
                m_pGuildTeleportLoc[i].m_sDestY = dY;
                ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
                strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
                m_pGuildTeleportLoc[i].m_dwTime = dwTime;

                //°ÔÀÌÆ® ¼­¹ö¸¦ ÅëÇØ ´Ù¸¥ ¼­¹ö·Î Á¤º¸ Àü¼Û
                bStockMsgToGateServer(cData, 23);
                return;
            }
        }

    // ¼³Á¤µÈ ³»¿ëÀÌ ¾ø´Ù. »õ·Î ¼³Á¤ÇÑ´Ù.
    dwTemp = 0;
    iIndex = -1;
    for (i = 0; i < DEF_MAXGUILDS; i++)
    {
        if (m_pGuildTeleportLoc[i].m_iV1 == NULL)
        {

            m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
            m_pGuildTeleportLoc[i].m_sDestX = dX;
            m_pGuildTeleportLoc[i].m_sDestY = dY;
            ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
            strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
            m_pGuildTeleportLoc[i].m_dwTime = dwTime;

            //°ÔÀÌÆ® ¼­¹ö¸¦ ÅëÇØ ´Ù¸¥ ¼­¹ö·Î Á¤º¸ Àü¼Û  
            bStockMsgToGateServer(cData, 23);
            return;
        }
        else
        {
            // °¡Àå ¿À·§µ¿¾È ÂüÁ¶µÇÁö ¾ÊÀº ¸®½ºÆ®ÀÇ ÀÎµ¦½º¸¦ Ã£´Â´Ù.
            if (dwTemp < (dwTime - m_pGuildTeleportLoc[i].m_dwTime))
            {
                dwTemp = (dwTime - m_pGuildTeleportLoc[i].m_dwTime);
                iIndex = i;
            }
        }
    }

    // Ä³½Ã °ø°£ÀÌ ´Ù Ã¡´Ù. ÀÌ·± °æ¿ì ¸®½ºÆ® Áß °¡Àå ¿À·§µ¿¾È »ç¿ëÇÏÁö ¾ÊÀº ¸®½ºÆ®(iIndex)¸¦ »èÁ¦ÇÏ°í ¾÷µ¥ÀÌÆ® ÇÑ´Ù.
    if (iIndex == -1) return;

    //testcode
    log->info("(X) No more GuildTeleportLoc Space! Replaced.");

    m_pGuildTeleportLoc[i].m_iV1 = iGuildGUID;
    m_pGuildTeleportLoc[i].m_sDestX = dX;
    m_pGuildTeleportLoc[i].m_sDestY = dY;
    ZeroMemory(m_pGuildTeleportLoc[i].m_cDestMapName, sizeof(m_pGuildTeleportLoc[i].m_cDestMapName));
    strcpy(m_pGuildTeleportLoc[i].m_cDestMapName, pMapName);
    m_pGuildTeleportLoc[i].m_dwTime = dwTime;

    //°ÔÀÌÆ® ¼­¹ö¸¦ ÅëÇØ ´Ù¸¥ ¼­¹ö·Î Á¤º¸ Àü¼Û
    bStockMsgToGateServer(cData, 23);
}

void CGame::MeteorStrikeHandler(int iMapIndex)
{
    int i, ix, iy, dX, dY, iIndex, iTargetIndex, iTotalESG, iEffect;
    int iTargetArray[DEF_MAXSTRIKEPOINTS];
    short sOwnerH;
    char  cOwnerType;
    DWORD dwTime = timeGetTime();

    log->info("(!) Beginning Meteor Strike Procedure...");

    if (iMapIndex == -1)
    {
        log->info("(X) MeteorStrikeHandler Error! MapIndex -1!");
        return;
    }

    if (m_pMapList[iMapIndex] == NULL)
    {
        log->info("(X) MeteorStrikeHandler Error! NULL Map!");
        return;
    }

    if (m_pMapList[iMapIndex]->m_iTotalStrikePoints == 0)
    {
        log->info("(X) MeteorStrikeHandler Error! No Strike Points!");
        return;
    }

    for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++) iTargetArray[i] = -1;

    iIndex = 0;
    for (i = 1; i <= m_pMapList[iMapIndex]->m_iTotalStrikePoints; i++)
    {
        if (m_pMapList[iMapIndex]->m_stStrikePoint[i].iHP > 0)
        {
            iTargetArray[iIndex] = i; // Â½ÂºÃ†Â®Â¶Ã³Ã€ÃŒÃ…Â© Ã†Ã·Ã€ÃŽÃ†Â®Ã€Ã‡ Ã€ÃŽÂµÂ¦Â½ÂºÂ¸Â¦ Â³Ã–Â´Ã‚Â´Ã™.
            iIndex++;
        }
    }

    //testcode
    wsprintf(G_cTxt, "(!) Map(%s) has %d available strike points", m_pMapList[iMapIndex]->m_cName, iIndex);
    log->info(G_cTxt);

    // Â°Ã¡Â°Ãº ÂºÂ¸Â°Ã­Â¿Ã« Â½ÂºÃ†Â®Â·Â°ÃƒÃ§ Ã…Â¬Â¸Â®Â¾Ã®
    m_stMeteorStrikeResult.iCasualties = 0;
    m_stMeteorStrikeResult.iCrashedStructureNum = 0;
    m_stMeteorStrikeResult.iStructureDamageAmount = 0;

    if (iIndex == 0)
    {
        // Ã†Ã¸Â°ÃÃ‡Ã’ Â°Ã‡Â¹Â°Ã€ÃŒ Â¾Ã¸Â´Ã™. Â¸Ã°ÂµÃŽ Ã†Ã„Â±Â«ÂµÃ‡Â¾ÃºÂ´Ã™.
        log->info("(!) No strike points!");
        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT, NULL, dwTime + 6000, NULL, NULL, iMapIndex, NULL, NULL, NULL, NULL, NULL);
    }
    else
    {

        // Ã‡Ã˜Â´Ã§ Â¸ÃŠÃ€Ã‡ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®ÂµÃ©Â¿Â¡Â°Ã” Â¸ÃžÃ…Ã—Â¿Ã€ Â°Ã¸Â°Ã ÃˆÂ¿Â°ÃºÂ¸Â¦ ÂºÂ¸Â¿Â©ÃÃ–Â¶Ã³Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶ Ã€Ã¼Â¼Ã›
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE) && (m_pClientList[i]->m_cMapIndex == iMapIndex))
            {
                SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKEHIT, NULL, NULL, NULL, NULL);
            }

        for (i = 0; i < iIndex; i++)
        {
            // Â¸Ã°ÂµÃ§ Ã†Ã¸Â°Ã Â¸Ã±Ã‡Â¥Â¿Â¡ Â´Ã«Ã‡Ã˜ ÂµÂ¿Ã€ÃÃ‡Ã‘ Â°Ã¸Â°ÃÃ€Â» Â°Â¡Ã‡Ã‘Â´Ã™.
            iTargetIndex = iTargetArray[i];

            if (iTargetIndex == -1)
            {
                log->info("(X) Strike Point MapIndex: -1!");
                goto MSH_SKIP_STRIKE;
            }

            dX = m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].dX;
            dY = m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].dY;

            // dX, dY Ã€Â§Ã„Â¡ ÃÃ–Ã€Â§Â¿Â¡ ÃƒÃ–Â¼Ã’ 2Â°Â³ Ã€ÃŒÂ»Ã³Ã€Ã‡ Energy Shield GeneratorÂ°Â¡ ÃÂ¸Ã€Ã§Ã‡ÃÂ¸Ã© Â½ÂºÃ†Â®Â¶Ã³Ã€ÃŒÃ…Â© Ã†Ã·Ã€ÃŽÃ†Â®Â´Ã‚ Â¹Â«Â»Ã§Ã‡ÃÂ´Ã™. Â±Ã—Â·Â¯Â³Âª 1Â°Â³ Ã€ÃŒÃ‡ÃÃ€ÃŒÂ¸Ã© Â°Ã¸Â°ÃÂ¹ÃžÂ¾Ã’Ã€Â»Â¶Â§ HPÂ°Â¡ ÃÃ™Â¾Ã®ÂµÃ§Â´Ã™.
            // NPC ÃÃ–ÂºÂ¯Â¿Â¡ Ã€Ã–Â´Ã‚ Ã€Ã» Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ Ã…Â½ÃÃ¶Ã‡Ã˜ Â°Ã¦ÂºÂ¸Â¸Â¦ Â¿Ã¯Â¸Â°Â´Ã™.
            iTotalESG = 0;
            for (ix = dX - 10; ix <= dX + 10; ix++)
                for (iy = dY - 10; iy <= dY + 10; iy++)
                {
                    m_pMapList[iMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                    if ((cOwnerType == DEF_OWNERTYPE_NPC) && (m_pNpcList[sOwnerH] != NULL) && (m_pNpcList[sOwnerH]->m_sType == 40))
                    {
                        iTotalESG++;
                    }
                }

            // testcode
            wsprintf(G_cTxt, "(!) Meteor Strike Target(%d, %d) ESG(%d)", dX, dY, iTotalESG);
            log->info(G_cTxt);

            if (iTotalESG < 2)
            {

                m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iHP -= (2 - iTotalESG);
                if (m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iHP <= 0)
                {
                    // Â°Ã‡Â¹Â°Ã€ÃŒ Ã†Ã„Â±Â«ÂµÃ‡Â¾ÃºÂ´Ã™.
                    m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iHP = 0;
                    // Â°Ã‡Â¹Â° Â±Ã¢Â´Ã‰ Â»Ã³Â½Ã‡
                    m_pMapList[m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iMapIndex]->m_bIsDisabled = TRUE;
                    m_stMeteorStrikeResult.iCrashedStructureNum++;
                }
                else
                {
                    // Â°Ã‡Â¹Â°Ã€ÃŒ Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Â¾Ã²Â¾ÃºÂ´Ã™.
                    m_stMeteorStrikeResult.iStructureDamageAmount += (2 - iTotalESG);
                    // Â°Ã‡Â¹Â° Â´Ã«Â¹ÃŒÃÃ¶ Ã€Ã”Ã€Âº ÃˆÃ„ ÂºÃ’Ã…Â¸Â´Ã‚ Ã€ÃŒÂ¹ÃŒÃÃ¶ Â»Ã°Ã€Ã” 
                    iEffect = iDice(1, 5) - 1;
                    iAddDynamicObjectList(NULL, DEF_OWNERTYPE_PLAYER_INDIRECT, DEF_DYNAMICOBJECT_FIRE2, iMapIndex,
                        m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iEffectX[iEffect] + (iDice(1, 3) - 2),
                        m_pMapList[iMapIndex]->m_stStrikePoint[iTargetIndex].iEffectY[iEffect] + (iDice(1, 3) - 2), 60 * 1000 * 50);
                }
            }
            MSH_SKIP_STRIKE:;
        }

        // 1ÃƒÃŠ ÃˆÃ„ Â¸ÃžÃ…Ã—Â¿Ã€ Â½ÂºÃ†Â®Â¶Ã³Ã€ÃŒÃ…Â© Â°Ã¸Â°Ã Â´Ã«Â¹ÃŒÃÃ¶ ÃˆÂ¿Â°Ãº
        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE, NULL, dwTime + 1000, NULL, NULL, iMapIndex, NULL, NULL, NULL, NULL, NULL);
        // 4ÃƒÃŠ ÃˆÃ„ Â¸ÃžÃ…Ã—Â¿Ã€ Â½ÂºÃ†Â®Â¶Ã³Ã€ÃŒÃ…Â© Â°Ã¸Â°Ã Â´Ã«Â¹ÃŒÃÃ¶ ÃˆÂ¿Â°Ãº
        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE, NULL, dwTime + 4000, NULL, NULL, iMapIndex, NULL, NULL, NULL, NULL, NULL);
        // 6ÃƒÃŠ ÃˆÃ„ Â°Ã¸Â°Ã Â°Ã¡Â°Ãº Ã†Ã‡Â´Ãœ.
        bRegisterDelayEvent(DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT, NULL, dwTime + 6000, NULL, NULL, iMapIndex, NULL, NULL, NULL, NULL, NULL);
    }
}

void CGame::CollectedManaHandler(WORD wAresdenMana, WORD wElvineMana)
{
    if (m_iAresdenMapIndex != -1)
    {
        m_iAresdenMana += wAresdenMana;
        //testcode
        if (wAresdenMana > 0)
        {
            wsprintf(G_cTxt, "Aresden Mana: %d Total:%d", wAresdenMana, m_iAresdenMana);
            log->info(G_cTxt);
        }
    }

    if (m_iElvineMapIndex != -1)
    {
        m_iElvineMana += wElvineMana;
        //testcode
        if (wElvineMana > 0)
        {
            wsprintf(G_cTxt, "Elvine Mana: %d Total:%d", wElvineMana, m_iElvineMana);
            log->info(G_cTxt);
        }
    }
}

void CGame::CalcMeteorStrikeEffectHandler(int iMapIndex)
{
    int i, iActiveStructure, iStructureHP[DEF_MAXSTRIKEPOINTS];
    char * cp, * cp2, cData[120], cWinnerSide, cTempData[120];
    WORD * wp;

    if (m_bIsCrusadeMode == FALSE) return;

    // »ç¿ëÀÚ¿¡°Ô ³²Àº °Ç¹°ÀÇ HP¸¦ º¸¿©ÁÖ±â À§ÇÑ º¯¼ö ÃÊ±âÈ­ 
    for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++)
        iStructureHP[i] = 0;

    // Æø°ÝÀÇ È¿°ú¸¦ °è»êÇÑ´Ù. ¸ðµç °Ç¹°ÀÌ ÆÄ±«µÇ¾ú´Ù¸é Àü¸éÀü ¸ðµå Á¾·á. ±×·¸Áö ¾ÊÀ¸¸é °á°ú¸¦ ¸ðµç ¼­¹ö¿¡ ¸®Æ÷Æ®.
    iActiveStructure = 0;
    for (i = 1; i <= m_pMapList[iMapIndex]->m_iTotalStrikePoints; i++)
    {
        if (m_pMapList[iMapIndex]->m_stStrikePoint[i].iHP > 0)
        {
            iActiveStructure++;
            iStructureHP[i] = m_pMapList[iMapIndex]->m_stStrikePoint[i].iHP;
        }
    }

    //testcode
    wsprintf(G_cTxt, "ActiveStructure:%d  MapIndex:%d AresdenMap:%d ElvineMap:%d", iActiveStructure, iMapIndex, m_iAresdenMapIndex, m_iElvineMapIndex);
    log->info(G_cTxt);

    if (iActiveStructure == 0)
    {
        // ÀÛµ¿ÁßÀÎ °Ç¹°ÀÌ ¾ø´Ù. Å©·ç¼¼ÀÌµå ¸ðµå Á¾·á: ´Ù¸¥ ¼­¹ö¿¡ ¾Ë·ÁÁØ´Ù.
        // Å©·ç¼¼ÀÌµå ¸ðµå Á¾·áµÊÀ» ÇöÀç ¼­¹öÀÇ Å¬¶óÀÌ¾ðÆ®µé¿¡°Ô ¹Ù·Î Àü´Þ.
        if (iMapIndex == m_iAresdenMapIndex)
        {
            cWinnerSide = 2;
            LocalEndCrusadeMode(2); // ÀÌ±ä ÂÊÀº ¿¤¹ÙÀÎ
        }
        else if (iMapIndex == m_iElvineMapIndex)
        {
            cWinnerSide = 1;
            LocalEndCrusadeMode(1); // ÀÌ±ä ÂÊÀº ¾Æ·¹½ºµ§
        }
        else
        {
            cWinnerSide = 0;
            LocalEndCrusadeMode(0);
        }

        // ´Ù¸¥ ¼­¹ö¿¡ Å©·ç¼¼ÀÌµå Á¾·á¸¦ ¾Ë¸².
        ZeroMemory(cData, sizeof(cData));
        cp = (char *)(cData);
        *cp = GSM_ENDCRUSADE;
        cp++;

        *cp = cWinnerSide;
        cp++;

        wp = (WORD *)cp;
        *wp = m_stMeteorStrikeResult.iCrashedStructureNum;
        cp += 2;

        wp = (WORD *)cp;
        *wp = m_stMeteorStrikeResult.iStructureDamageAmount;
        cp += 2;

        wp = (WORD *)cp;
        *wp = m_stMeteorStrikeResult.iCasualties;
        cp += 2;

        memcpy(cp, m_pMapList[iMapIndex]->m_cName, 10);
        cp += 10;

        bStockMsgToGateServer(cData, 18);

    }
    else
    {
        // ÇÇÇØ »óÈ²À» ¸ðµç ¼­¹ö·Î ¸®Æ÷Æ®.
        ZeroMemory(cData, sizeof(cData));
        cp = (char *)(cData);
        *cp = GSM_GRANDMAGICRESULT;
        cp++;

        memcpy(cp, m_pMapList[iMapIndex]->m_cName, 10);
        cp += 10;

        wp = (WORD *)cp;
        *wp = m_stMeteorStrikeResult.iCrashedStructureNum;
        cp += 2;

        wp = (WORD *)cp;
        *wp = m_stMeteorStrikeResult.iStructureDamageAmount;
        cp += 2;

        wp = (WORD *)cp;
        *wp = m_stMeteorStrikeResult.iCasualties;
        cp += 2;

        wp = (WORD *)cp;
        *wp = (WORD)iActiveStructure;
        cp += 2;

        //v2.15 Ãß°¡  ÅäÅ» °Ç¹°ÀÇ °¹¼ö 
        ZeroMemory(cTempData, sizeof(cTempData));
        cp2 = (char *)(cTempData);

        wp = (WORD *)cp2;
        *wp = (WORD)m_pMapList[iMapIndex]->m_iTotalStrikePoints;
        cp2 += 2;

        for (i = 1; i <= m_pMapList[iMapIndex]->m_iTotalStrikePoints; i++)
        {
            wp = (WORD *)cp2;
            *wp = (WORD)iStructureHP[i];
            cp2 += 2;
        }

        memcpy(cp, cTempData, 2 * (m_pMapList[iMapIndex]->m_iTotalStrikePoints + 1));

        // v2.15 
        bStockMsgToGateServer(cData, 18 + (m_pMapList[iMapIndex]->m_iTotalStrikePoints + 1) * 2);

        // ÇöÀç ¼­¹ö¿¡´Â ½ºÅå ¸Þ½ÃÁö°¡ ¿ÀÁö ¾ÊÀ¸¹Ç·Î Á÷Á¢ Àü´ÞÇÑ´Ù.
        // v2.15 °Ç¹°ÀÇ °¹¼ö¿Í HP¸¦ º¸³½´Ù.
        GrandMagicResultHandler(m_pMapList[iMapIndex]->m_cName, m_stMeteorStrikeResult.iCrashedStructureNum, m_stMeteorStrikeResult.iStructureDamageAmount, m_stMeteorStrikeResult.iCasualties, iActiveStructure, m_pMapList[iMapIndex]->m_iTotalStrikePoints, cTempData);
    }

    // °á°ú º¸°í¿ë ½ºÆ®·°Ãç Å¬¸®¾î
    m_stMeteorStrikeResult.iCasualties = 0;
    m_stMeteorStrikeResult.iCrashedStructureNum = 0;
    m_stMeteorStrikeResult.iStructureDamageAmount = 0;
}

void CGame::GrandMagicResultHandler(char * cMapName, int iCrashedStructureNum, int iStructureDamageAmount, int iCasualities, int iActiveStructure, int iTotalStrikePoints, char * cData)
{
    int i;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != NULL)
        {
            SendNotifyMsg(NULL, i, DEF_NOTIFY_GRANDMAGICRESULT, iCrashedStructureNum, iStructureDamageAmount, iCasualities, cMapName, iActiveStructure, NULL, NULL, NULL, NULL, iTotalStrikePoints, cData);
        }
}

void CGame::MeteorStrikeMsgHandler(char cAttackerSide)
{
    int i;
    DWORD dwTime = timeGetTime();

    switch (cAttackerSide)
    {
        case 1:
            if (m_iElvineMapIndex != -1)
            {
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
                    {
                        if (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "elvine") == 0)
                        {
                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 1, NULL, NULL, NULL);
                        }
                        else
                        {
                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 2, NULL, NULL, NULL);
                        }
                    }
                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_METEORSTRIKE, NULL, dwTime + 5000, NULL, NULL, m_iElvineMapIndex, NULL, NULL, NULL, NULL, NULL);
            }
            else
            {
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if (m_pClientList[i] != NULL)
                    {
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 2, NULL, NULL, NULL);
                    }
            }
            break;

        case 2:
            if (m_iAresdenMapIndex != -1)
            {
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
                    {
                        if (strcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cLocationName, "aresden") == 0)
                        {
                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 3, NULL, NULL, NULL);
                        }
                        else
                        {
                            SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 4, NULL, NULL, NULL);
                        }
                    }
                bRegisterDelayEvent(DEF_DELAYEVENTTYPE_METEORSTRIKE, NULL, dwTime + 1000 * 5, NULL, NULL, m_iAresdenMapIndex, NULL, NULL, NULL, NULL, NULL);
            }
            else
            {
                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if (m_pClientList[i] != NULL)
                    {
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_METEORSTRIKECOMING, 4, NULL, NULL, NULL);
                    }
            }
            break;
    }
}

void CGame::LoteryHandler(int iClientH)
{
    CItem * pItem;
    int     iItemID;
    if (m_pClientList[iClientH] == NULL) return;
    switch (iDice(1, 22))
    {
        case 1:iItemID = 656; break; // XelimaStone
        case 2:iItemID = 657; break; // MerienStone
        case 3:iItemID = 650; break; // ZemstoneOfSacrifice
        case 4:iItemID = 652; break; // RedBall
        case 5:iItemID = 654; break; // BlueBall
        case 6:iItemID = 881; break; // ArmorDye(Indigo)
        case 7:iItemID = 882; break; // ArmorDye(CrimsonRed)
        case 8:iItemID = 883; break; // ArmorDye(Gold)
        case 9:iItemID = 884; break; // ArmorDye(Aqua)
        case 10:iItemID = 885; break; // ArmorDye(Pink)
        case 11:iItemID = 886; break; // ArmorDye(Violet)
        case 12:iItemID = 887; break; // ArmorDye(Blue) 
        case 13:iItemID = 888; break; // ArmorDye(Khaki) 
        case 14:iItemID = 889; break; // ArmorDye(Yellow) 
        case 15:iItemID = 890; break; // ArmorDye(Red) 
        case 16:iItemID = 971; break; // ArmorDye(Green)
        case 17:iItemID = 972; break; // ArmorDye(Black) 
        case 18:iItemID = 973; break; // ArmorDye(Knight) 
        case 19:iItemID = 970; break; // CritCandy
        case 20:iItemID = 651; break; // GreenBall
        case 21:iItemID = 653; break; // YellowBall
        case 22:iItemID = 655; break; // PearlBall
    }

    //chance
    if (iDice(1, 120) <= 3) iItemID = 650;//ZemstoneOfSacrifice
    //chance

    pItem = new CItem;
    if (_bInitItemAttr(pItem, iItemID) == FALSE)
    {
        delete pItem;
    }
    else
    {
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
            m_pClientList[iClientH]->m_sY, pItem);
        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
            m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
    }


}
