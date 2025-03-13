//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "map.h"

void CGame::NpcTalkHandler(int iClientH, int iWho)
{
    char cRewardName[21], cTargetName[21];
    int iResMode, iQuestNum, iQuestType, iRewardType, iRewardAmount, iContribution, iX, iY, iRange, iTargetType, iTargetCount;

    iQuestNum = 0;
    memset(cTargetName, 0, sizeof(cTargetName));
    if (m_pClientList[iClientH] == 0) return;
    switch (iWho)
    {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            iQuestNum = _iTalkToNpcResult_Cityhall(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
            break;
        case 5:
            break;
        case 6:
            break;

        case 32:
            break;
        case 67:
        case 68:
        case 69:
            break;

        case 21:
            iQuestNum = _iTalkToNpcResult_Guard(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
            if (iQuestNum >= 1000) return;
            break;
    }

    memset(cRewardName, 0, sizeof(cRewardName));
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
                case -10: strcpy(cRewardName, "?æÇèÄ?"); break;
            }
        }


        m_pClientList[iClientH]->m_iAskedQuest = iQuestNum;
        m_pClientList[iClientH]->m_iQuestRewardType = iRewardType;
        m_pClientList[iClientH]->m_iQuestRewardAmount = iRewardAmount;

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, iQuestType, iResMode, iRewardAmount, cRewardName, iContribution,
            iTargetType, iTargetCount, iX, iY, iRange, cTargetName);
    }
    else
    {
        switch (iQuestNum)
        {
            case  0: SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (iWho + 130), 0, 0, 0, 0); break;
            case -1:
            case -2:
            case -3:
            case -4: SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, abs(iQuestNum) + 100, 0, 0, 0, 0); break;
            case -5: break;
        }
    }
}


int CGame::_iTalkToNpcResult_Cityhall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    int iQuest, iEraseReq, iExp;
    CItem * pItem;

    if (m_pClientList[iClientH] == 0) return 0;

    if (m_pClientList[iClientH]->m_iQuest != 0)
    {
        if (m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest] == 0) return -4;
        else if (m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iFrom == 4)
        {

            if (m_pClientList[iClientH]->m_bIsQuestCompleted == true)
            {


                if ((m_pClientList[iClientH]->m_iQuestRewardType > 0) &&
                    (m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType] != 0))
                {

                    pItem = new CItem;
                    _bInitItemAttr(pItem, m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType]->m_cName);
                    pItem->m_dwCount = m_pClientList[iClientH]->m_iQuestRewardAmount;
                    if (_bCheckItemReceiveCondition(iClientH, pItem) == true)
                    {

                        _bAddClientItemList(iClientH, pItem, &iEraseReq);
                        SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);
                        if (iEraseReq == 1)
                        {
                            delete pItem;
                            pItem = 0;
                        }


                        m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;


                        SendNotifyMsg(0, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, m_pClientList[iClientH]->m_iQuestRewardAmount,
                            m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType]->m_cName, m_pClientList[iClientH]->m_iContribution);

                        _ClearQuestStatus(iClientH);
                        return -5;
                    }
                    else
                    {
                        delete pItem;
                        pItem = 0;


                        SendItemNotifyMsg(iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, 0, 0);

                        SendNotifyMsg(0, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 0, m_pClientList[iClientH]->m_iQuestRewardAmount,
                            m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType]->m_cName, m_pClientList[iClientH]->m_iContribution);

                        return -5;
                    }
                }
                else if (m_pClientList[iClientH]->m_iQuestRewardType == -1)
                {

                    GetExp(iClientH, m_pClientList[iClientH]->m_iQuestRewardAmount); //m_pClientList[iClientH]->m_iExpStock += m_pClientList[iClientH]->m_iQuestRewardAmount;

                    m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;

                    SendNotifyMsg(0, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, m_pClientList[iClientH]->m_iQuestRewardAmount,
                        "°æÇèÄ¡              ", m_pClientList[iClientH]->m_iContribution);


                    _ClearQuestStatus(iClientH);
                    return -5;
                }
                else if (m_pClientList[iClientH]->m_iQuestRewardType == -2)
                {

                    //***
                    iExp = iDice(1, (10 * m_pClientList[iClientH]->m_iLevel));
                    //***
                    iExp = iExp * m_pClientList[iClientH]->m_iQuestRewardAmount;

                    GetExp(iClientH, iExp); //m_pClientList[iClientH]->m_iExpStock += iExp;

                    m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;

                    SendNotifyMsg(0, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, iExp,
                        "°æÇèÄ¡              ", m_pClientList[iClientH]->m_iContribution);


                    _ClearQuestStatus(iClientH);
                    return -5;
                }
                else
                {


                    m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;

                    SendNotifyMsg(0, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, 0,
                        "                     ", m_pClientList[iClientH]->m_iContribution);


                    _ClearQuestStatus(iClientH);
                    return -5;
                }
            }
            else return -1;
        }

        return -4;
    }


    if (m_pClientList[iClientH]->m_iIsOnTown == DEF_NONPK)
    { // 2002-11-15 ¼öÁ¤


        if (m_pClientList[iClientH]->m_iPKCount > 0) return -3;

        iQuest = __iSearchForQuest(iClientH, 4, pQuestType, pMode, pRewardType, pRewardAmount, pContribution, pTargetName, pTargetType, pTargetCount, pX, pY, pRange);
        if (iQuest <= 0) return -4;


        return iQuest;
    }
    else return -2;

    return -4;
}


int CGame::_iTalkToNpcResult_Guard(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{

    if (m_pClientList[iClientH] == 0) return 0;


    if (m_pClientList[iClientH]->m_cSide == DEF_ARESDEN)
    {

        if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0)
        {

            SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (200), 0, 0, 0, 0);
            return 1000;
        }
        else
            if (memcmp(m_pClientList[iClientH]->m_cMapName, "elvine", 6) == 0)
            {

                SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (201), 0, 0, 0, 0);
                return 1001;
            }
    }
    else
        if (m_pClientList[iClientH]->m_cSide == DEF_ELVINE)
        {
            if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0)
            {

                SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (202), 0, 0, 0, 0);
                return 1002;
            }
            else
                if (memcmp(m_pClientList[iClientH]->m_cMapName, "elvine", 6) == 0)
                {

                    SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (203), 0, 0, 0, 0);
                    return 1003;
                }
        }
        else
            if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
            {
                if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0)
                {

                    SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (204), 0, 0, 0, 0);
                    return 1004;
                }
                else
                    if (memcmp(m_pClientList[iClientH]->m_cMapName, "elvine", 6) == 0)
                    {

                        SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (205), 0, 0, 0, 0);
                        return 1005;
                    }
                    else
                        if (memcmp(m_pClientList[iClientH]->m_cMapName, "default", 7) == 0)
                        {

                            SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (206), 0, 0, 0, 0);
                            return 1006;
                        }
            }

    return 0;
}


int CGame::_iTalkToNpcResult_GuildHall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    return -4;
}

int CGame::_iTalkToNpcResult_GShop(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    return -4;
}

int CGame::_iTalkToNpcResult_BSmith(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    return -4;
}

int CGame::_iTalkToNpcResult_WHouse(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    return -4;
}

int CGame::_iTalkToNpcResult_WTower(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    return -4;
}

void CGame::NpcRequestAssistance(int iNpcH)
{
    int ix, iy, sX, sY;
    short sOwnerH;
    char  cOwnerType;


    if (m_pNpcList[iNpcH] == 0) return;

    sX = m_pNpcList[iNpcH]->m_sX;
    sY = m_pNpcList[iNpcH]->m_sY;

    for (ix = sX - 8; ix <= sX + 8; ix++)
        for (iy = sY - 8; iy <= sY + 8; iy++)
        {
            m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
            if ((sOwnerH != 0) && (m_pNpcList[sOwnerH] != 0) && (cOwnerType == DEF_OWNERTYPE_NPC) &&
                (iNpcH != sOwnerH) && (m_pNpcList[sOwnerH]->m_cSide == m_pNpcList[iNpcH]->m_cSide) &&
                (m_pNpcList[sOwnerH]->m_bIsPermAttackMode == false) && (m_pNpcList[sOwnerH]->m_cBehavior == DEF_BEHAVIOR_MOVE))
            {


                m_pNpcList[sOwnerH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
                m_pNpcList[sOwnerH]->m_sBehaviorTurnCount = 0;
                m_pNpcList[sOwnerH]->m_iTargetIndex = m_pNpcList[iNpcH]->m_iTargetIndex;
                m_pNpcList[sOwnerH]->m_cTargetType = m_pNpcList[iNpcH]->m_cTargetType;

                return;
            }
        }
}
