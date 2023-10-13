//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

int CGame::_iTalkToNpcResult_Cityhall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange)
{
    int iQuest, iEraseReq, iExp;
    CItem * pItem;

    if (m_pClientList[iClientH] == NULL) return 0;

    if (m_pClientList[iClientH]->m_iQuest != NULL)
    {
        if (m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest] == NULL) return -4;
        else if (m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iFrom == 4)
        {
            if (m_pClientList[iClientH]->m_bIsQuestCompleted == TRUE)
            {
                if ((m_pClientList[iClientH]->m_iQuestRewardType > 0) &&
                    (m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType] != NULL))
                {
                    pItem = new CItem;
                    _bInitItemAttr(pItem, m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType]->m_cName);
                    pItem->m_dwCount = m_pClientList[iClientH]->m_iQuestRewardAmount;
                    if (_bCheckItemReceiveCondition(iClientH, pItem) == TRUE)
                    {
                        _bAddClientItemList(iClientH, pItem, &iEraseReq);
                        SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, NULL);
                        if (iEraseReq == 1) delete pItem;

                        m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, m_pClientList[iClientH]->m_iQuestRewardAmount,
                            m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType]->m_cName, m_pClientList[iClientH]->m_iContribution);

                        _ClearQuestStatus(iClientH);
                        return -5;
                    }
                    else
                    {
                        delete pItem;
                        SendItemNotifyMsg(iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);

                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 0, m_pClientList[iClientH]->m_iQuestRewardAmount,
                            m_pItemConfigList[m_pClientList[iClientH]->m_iQuestRewardType]->m_cName, m_pClientList[iClientH]->m_iContribution);

                        return -5;
                    }
                }
                else if (m_pClientList[iClientH]->m_iQuestRewardType == -1)
                {
                    m_pClientList[iClientH]->m_iExpStock += m_pClientList[iClientH]->m_iQuestRewardAmount;
                    m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, m_pClientList[iClientH]->m_iQuestRewardAmount,
                        "Â°Ã¦Ã‡Ã¨Ã„Â¡              ", m_pClientList[iClientH]->m_iContribution);

                    _ClearQuestStatus(iClientH);
                    return -5;
                }
                else if (m_pClientList[iClientH]->m_iQuestRewardType == -2)
                {
                    iExp = iDice(1, (10 * m_pClientList[iClientH]->m_iLevel));
                    iExp = iExp * m_pClientList[iClientH]->m_iQuestRewardAmount;

                    m_pClientList[iClientH]->m_iExpStock += iExp;
                    m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, iExp,
                        "Â°Ã¦Ã‡Ã¨Ã„Â¡              ", m_pClientList[iClientH]->m_iContribution);

                    _ClearQuestStatus(iClientH);
                    return -5;
                }
                else
                {
                    m_pClientList[iClientH]->m_iContribution += m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iContribution;

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTREWARD, 4, 1, 0,
                        "                     ", m_pClientList[iClientH]->m_iContribution);

                    _ClearQuestStatus(iClientH);
                    return -5;
                }
            }
            else return -1;
        }

        return -4;
    }

    if (memcmp(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10) == 0)
    {
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
    if (m_pClientList[iClientH] == NULL) return 0;

    if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0)
    {
        if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0)
        {
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (200), NULL, NULL, NULL, NULL);
            return 1000;
        }
        else
            if (memcmp(m_pClientList[iClientH]->m_cMapName, "elv", 3) == 0)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (201), NULL, NULL, NULL, NULL);
                return 1001;
            }
    }
    else
        if (memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0)
        {
            if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (202), NULL, NULL, NULL, NULL);
                return 1002;
            }
            else
                if (memcmp(m_pClientList[iClientH]->m_cMapName, "elv", 3) == 0)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (203), NULL, NULL, NULL, NULL);
                    return 1003;
                }
        }
        else
            if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0)
            {
                if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0)
                {
                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (204), NULL, NULL, NULL, NULL);
                    return 1004;
                }
                else
                    if (memcmp(m_pClientList[iClientH]->m_cMapName, "elvine", 6) == 0)
                    {
                        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (205), NULL, NULL, NULL, NULL);
                        return 1005;
                    }
                    else
                        if (memcmp(m_pClientList[iClientH]->m_cMapName, "default", 7) == 0)
                        {
                            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (206), NULL, NULL, NULL, NULL);
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
