//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

extern char G_cTxt[512];

void CGame::PartyOperationResultHandler(char * pData)
{
    char * cp, cResult, cName[12];
    uint16_t * wp;
    int i, iClientH, iPartyID, iTotal;

    cp = (char *)(pData + 4);
    wp = (uint16_t *)cp;
    cp += 2;


    switch (*wp)
    {
        case 1:
            cResult = *cp;
            cp++;
            wp = (uint16_t *)cp;
            iClientH = (int)*wp;
            cp += 2;
            memset(cName, 0, sizeof(cName));
            memcpy(cName, cp, 10);
            cp += 10;
            wp = (uint16_t *)cp;
            iPartyID = (int)*wp;
            cp += 2;
            PartyOperationResult_Create(iClientH, cName, cResult, iPartyID);
            //testcode
            wsprintf(G_cTxt, "party Operation Result: Create(ClientH:%d PartyID:%d)", iClientH, iPartyID);
            log->info(G_cTxt);
            break;

        case 2:
            wp = (uint16_t *)cp;
            iPartyID = *wp;
            cp += 2;
            PartyOperationResult_Delete(iPartyID);

            //testcode
            wsprintf(G_cTxt, "party Operation Result: Delete(PartyID:%d)", iPartyID);
            log->info(G_cTxt);
            break;

        case 3:
            wp = (uint16_t *)cp;
            iClientH = *wp;
            cp += 2;
            memset(cName, 0, sizeof(cName));
            memcpy(cName, cp, 10);
            cp += 10;

            if ((iClientH < 0) && (iClientH > DEF_MAXCLIENTS)) return;
            if (m_pClientList[iClientH] == 0) return;
            if (strcmp(m_pClientList[iClientH]->m_cCharName, cName) != 0) return;


            for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
                if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == iClientH)
                {
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = 0;
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers--;
                    //testcode
                    wsprintf(G_cTxt, "PartyID:%d member:%d Out(Clear) Total:%d", m_pClientList[iClientH]->m_iPartyID, iClientH, m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers);
                    log->info(G_cTxt);
                    goto PORH_LOOPBREAK1;
                }
            PORH_LOOPBREAK1:;

            for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++)
                if ((m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0) && (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] != 0))
                {
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1];
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] = 0;
                }

            m_pClientList[iClientH]->m_iPartyID = 0;
            m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;

            //testcode
            wsprintf(G_cTxt, "Party Status 0: %s", m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);

            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 8, 0, 0, 0);
            break;

        case 4:
            cResult = *cp;
            cp++;
            wp = (uint16_t *)cp;
            iClientH = (int)*wp;
            cp += 2;
            memset(cName, 0, sizeof(cName));
            memcpy(cName, cp, 10);
            cp += 10;
            wp = (uint16_t *)cp;
            iPartyID = (int)*wp;
            cp += 2;
            PartyOperationResult_Join(iClientH, cName, cResult, iPartyID);
            //testcode
            wsprintf(G_cTxt, "party Operation Result: Join(ClientH:%d PartyID:%d)", iClientH, iPartyID);
            log->info(G_cTxt);
            break;

        case 5:
            wp = (uint16_t *)cp;
            iClientH = (int)*wp;
            cp += 2;
            memset(cName, 0, sizeof(cName));
            memcpy(cName, cp, 10);
            cp += 10;
            wp = (uint16_t *)cp;
            iTotal = (int)*wp;
            cp += 2;
            PartyOperationResult_Info(iClientH, cName, iTotal, cp);
            //testcode
            wsprintf(G_cTxt, "party Operation Result: Info(ClientH:%d Total:%d)", iClientH, iTotal);
            log->info(G_cTxt);
            break;

        case 6:
            cResult = *cp;
            cp++;
            wp = (uint16_t *)cp;
            iClientH = (int)*wp;
            cp += 2;
            memset(cName, 0, sizeof(cName));
            memcpy(cName, cp, 10);
            cp += 10;
            wp = (uint16_t *)cp;
            iPartyID = (int)*wp;
            cp += 2;
            PartyOperationResult_Dismiss(iClientH, cName, cResult, iPartyID);
            //testcode
            wsprintf(G_cTxt, "party Operation Result: Dismiss(ClientH:%d PartyID:%d)", iClientH, iPartyID);
            log->info(G_cTxt);
            break;
    }
}

void CGame::PartyOperationResult_Create(int iClientH, char * pName, int iResult, int iPartyID)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

    switch (iResult)
    {
        case 0:
            if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
            if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

            m_pClientList[iClientH]->m_iPartyID = 0;
            m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
            m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 1, 0, 0, 0);
            break;

        case 1:
            if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
            if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

            m_pClientList[iClientH]->m_iPartyID = iPartyID;
            m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_CONFIRM;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 1, 1, 0, 0);


            for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
                if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0)
                {
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = iClientH;
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers++;
                    //testcode
                    wsprintf(G_cTxt, "PartyID:%d member:%d New Total:%d", m_pClientList[iClientH]->m_iPartyID, iClientH, m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers);
                    log->info(G_cTxt);
                    goto PORC_LOOPBREAK1;
                }
            PORC_LOOPBREAK1:;


            if ((m_pClientList[iClientH]->m_iReqJoinPartyClientH != 0) && (strlen(m_pClientList[iClientH]->m_cReqJoinPartyName) != 0))
            {
                m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
                memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
            }
            break;
    }
}

void CGame::PartyOperationResult_Join(int iClientH, char * pName, int iResult, int iPartyID)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;

    switch (iResult)
    {
        case 0:
            if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
            if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

            m_pClientList[iClientH]->m_iPartyID = 0;
            m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 4, 0, 0, pName);

            m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
            memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
            break;

        case 1:
            if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
            if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

            m_pClientList[iClientH]->m_iPartyID = iPartyID;
            m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_CONFIRM;
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 4, 1, 0, pName);

            m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
            memset(m_pClientList[iClientH]->m_cReqJoinPartyName, 0, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));


            for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
                if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0)
                {
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = iClientH;
                    m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers++;
                    //testcode
                    wsprintf(G_cTxt, "PartyID:%d member:%d In(Join) Total:%d", m_pClientList[iClientH]->m_iPartyID, iClientH, m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers);
                    log->info(G_cTxt);
                    goto PORC_LOOPBREAK1;
                }
            PORC_LOOPBREAK1:;


            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((i != iClientH) && (m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == iPartyID))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_PARTY, 4, 1, 0, pName);
                }
            break;
    }
}

void CGame::PartyOperationResult_Dismiss(int iClientH, char * pName, int iResult, int iPartyID)
{
    int i;


    switch (iResult)
    {
        case 0:
            break;

        case 1:
            if (iClientH == 0)
            {

                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cCharName, pName) == 0))
                    {
                        iClientH = i;
                        goto PORD_LOOPBREAK;
                    }
                PORD_LOOPBREAK:;


                for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
                    if (m_stPartyInfo[iPartyID].iIndex[i] == iClientH)
                    {
                        m_stPartyInfo[iPartyID].iIndex[i] = 0;
                        m_stPartyInfo[iPartyID].iTotalMembers--;
                        //testcode
                        wsprintf(G_cTxt, "PartyID:%d member:%d Out Total:%d", iPartyID, iClientH, m_stPartyInfo[iPartyID].iTotalMembers);
                        log->info(G_cTxt);
                        goto PORC_LOOPBREAK1;
                    }
                PORC_LOOPBREAK1:;

                for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++)
                    if ((m_stPartyInfo[iPartyID].iIndex[i] == 0) && (m_stPartyInfo[iPartyID].iIndex[i + 1] != 0))
                    {
                        m_stPartyInfo[iPartyID].iIndex[i] = m_stPartyInfo[iPartyID].iIndex[i + 1];
                        m_stPartyInfo[iPartyID].iIndex[i + 1] = 0;
                    }

                if (m_pClientList[iClientH] != 0)
                {
                    m_pClientList[iClientH]->m_iPartyID = 0;
                    m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                    m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
                }


                for (i = 1; i < DEF_MAXCLIENTS; i++)
                    if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == iPartyID))
                    {
                        SendNotifyMsg(NULL, i, DEF_NOTIFY_PARTY, 6, 1, 0, pName);
                    }
                return;
            }

            if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING)) return;
            if ((m_pClientList[iClientH] != 0) && (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0)) return;


            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == iPartyID))
                {
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_PARTY, 6, 1, 0, pName);
                }


            for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
                if (m_stPartyInfo[iPartyID].iIndex[i] == iClientH)
                {
                    m_stPartyInfo[iPartyID].iIndex[i] = 0;
                    m_stPartyInfo[iPartyID].iTotalMembers--;
                    //testcode
                    wsprintf(G_cTxt, "PartyID:%d member:%d Out Total:%d", iPartyID, iClientH, m_stPartyInfo[iPartyID].iTotalMembers);
                    log->info(G_cTxt);
                    goto PORC_LOOPBREAK2;
                }
            PORC_LOOPBREAK2:;

            for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++)
                if ((m_stPartyInfo[iPartyID].iIndex[i] == 0) && (m_stPartyInfo[iPartyID].iIndex[i + 1] != 0))
                {
                    m_stPartyInfo[iPartyID].iIndex[i] = m_stPartyInfo[iPartyID].iIndex[i + 1];
                    m_stPartyInfo[iPartyID].iIndex[i + 1] = 0;
                }

            if (m_pClientList[iClientH] != 0)
            {
                m_pClientList[iClientH]->m_iPartyID = 0;
                m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
                m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
            }
            break;
    }
}

void CGame::PartyOperationResult_Delete(int iPartyID)
{
    int i;

    for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
    {
        m_stPartyInfo[iPartyID].iIndex[i] = 0;
        m_stPartyInfo[iPartyID].iTotalMembers = 0;
    }


    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID == iPartyID))
        {
            SendNotifyMsg(NULL, i, DEF_NOTIFY_PARTY, 2, 0, 0, 0);
            m_pClientList[i]->m_iPartyID = 0;
            m_pClientList[i]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
            m_pClientList[i]->m_iReqJoinPartyClientH = 0;
            //testcode
            wsprintf(G_cTxt, "Notify delete party: %d", i);
            log->info(G_cTxt);
        }
}




void CGame::PartyOperationResult_Info(int iClientH, char * pName, int iTotal, char * pNameList)
{
    if (m_pClientList[iClientH] == 0) return;
    if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;
    if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;

    SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PARTY, 5, 1, iTotal, pNameList);
}
