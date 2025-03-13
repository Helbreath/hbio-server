//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"

void CGame::UserCommand_BanGuildsman(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cTargetName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iGuildRank != 0)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOGUILDMASTERLEVEL, 0, 0, 0, 0);
        return;
    }

    memset(cTargetName, 0, sizeof(cTargetName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
            {


                if (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[i]->m_cGuildName, 20) != 0)
                {


                    SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTBANGUILDMAN, 0, 0, 0, 0);
                    delete pStrTok;
                    return;
                }

                SendGuildMsg(i, DEF_NOTIFY_DISMISSGUILDSMAN, 0, 0, 0);


                memset(m_pClientList[i]->m_cGuildName, 0, sizeof(m_pClientList[i]->m_cGuildName));
                strcpy(m_pClientList[i]->m_cGuildName, "NONE");
                m_pClientList[i]->m_iGuildRank = -1;
                m_pClientList[i]->m_iGuildGUID = -1;


                SendNotifyMsg(0, iClientH, DEF_NOTIFY_SUCCESSBANGUILDMAN, 0, 0, 0, 0);


                SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_BANGUILD, 0, 0, 0, 0);


                SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
                SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_CLEARGUILDNAME, m_pClientList[i]->m_cMapIndex, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY, 0, 0, 0);

                delete pStrTok;
                return;
            }

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cTargetName);
    }

    delete pStrTok;
    return;
}



void CGame::UserCommand_DissmissGuild(int iClientH, char * pData, uint32_t dwMsgSize)
{

}
