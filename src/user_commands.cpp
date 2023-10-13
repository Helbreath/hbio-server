//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

void CGame::UserCommand_BanGuildsman(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cTargetName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iGuildRank != 0)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOGUILDMASTERLEVEL, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
            {

                if (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[i]->m_cGuildName, 20) != 0)
                {

                    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_CANNOTBANGUILDMAN, NULL, NULL, NULL, NULL);
                    delete pStrTok;
                    return;
                }
                //bSendMsgToLS(MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN, i);

                SendGuildMsg(i, DEF_NOTIFY_DISMISSGUILDSMAN, NULL, NULL, NULL);

                ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
                strcpy(m_pClientList[i]->m_cGuildName, "NONE");
                m_pClientList[i]->m_iGuildRank = -1;
                m_pClientList[i]->m_iGuildGUID = -1;

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_SUCCESSBANGUILDMAN, NULL, NULL, NULL, NULL);

                SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_BANGUILD, NULL, NULL, NULL, NULL);

                SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);

                delete pStrTok;
                return;
            }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cTargetName);
    }

    delete pStrTok;
    return;
}

void CGame::UserCommand_DissmissGuild(int iClientH, char * pData, DWORD dwMsgSize)
{

}
