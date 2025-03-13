//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"
#include "map.h"

extern char G_cTxt[512];

void CGame::RequestAdminUserMode(int iClientH, char * pData)
{



}

void CGame::AdminOrder_CallGuard(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cTargetName[11], cBuff[256], cNpcName[21], cNpcWaypoint[11];
    CStrTok * pStrTok;
    int i, iNamingValue, tX, tY;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif
    memset(cName, 0, sizeof(cName));
    memset(cNpcWaypoint, 0, sizeof(cNpcWaypoint));
    memset(cTargetName, 0, sizeof(cTargetName));
    memset(cNpcName, 0, sizeof(cNpcName));
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


                if (memcmp(m_pClientList[i]->m_cMapName, "aresden", 7) == 0)
                    strcpy(cNpcName, "Guard-Aresden");
                else if (memcmp(m_pClientList[i]->m_cMapName, "elvine", 6) == 0)
                    strcpy(cNpcName, "Guard-Elvine");
                else strcpy(cNpcName, "Guard-Neutral");

                iNamingValue = m_pMapList[m_pClientList[i]->m_cMapIndex]->iGetEmptyNamingValue();
                if (iNamingValue == -1)
                {

                }
                else
                {

                    wsprintf(cName, "XX%d", iNamingValue);
                    cName[0] = '_';
                    cName[1] = m_pClientList[i]->m_cMapIndex + 65;

                    tX = (int)m_pClientList[i]->m_sX;
                    tY = (int)m_pClientList[i]->m_sY;
                    if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM,
                        &tX, &tY, cNpcWaypoint, 0, 0, -1, false, true) == false)
                    {

                        m_pMapList[m_pClientList[i]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                    }
                    else
                    {

                        wsprintf(G_cTxt, "GM Order(%s): Guard Attack PC(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cCharName);
                        bSetNpcAttackMode(cName, i, DEF_OWNERTYPE_PLAYER, true);
                    }
                }

                delete pStrTok;
                return;
            }


        SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cTargetName);
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_SummonDemon(int iClientH)
{
    char cName[21], cNpcName[21], cNpcWaypoint[11];
    int iNamingValue, tX, tY;

    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue == -1)
    {

    }
    else
    {

        memset(cNpcName, 0, sizeof(cNpcName));
        strcpy(cNpcName, "Demon");

        memset(cName, 0, sizeof(cName));
        wsprintf(cName, "XX%d", iNamingValue);
        cName[0] = '_';
        cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        memset(cNpcWaypoint, 0, sizeof(cNpcWaypoint));

        tX = (int)m_pClientList[iClientH]->m_sX;
        tY = (int)m_pClientList[iClientH]->m_sY;
        if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, (rand() % 9),
            DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, 0, 0, -1, false, false) == false)
        {

            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
        else
        {
            wsprintf(G_cTxt, "GM Order(%s): Summon Demon", m_pClientList[iClientH]->m_cCharName);
        }
    }

}
void CGame::AdminOrder_SummonMOB(int iClientH)
{
    char cName[21], cNpcName[21], cNpcWaypoint[11], cName_Master[11];
    int iNamingValue, tX{}, tY{};
    char cOwnerType;
    short sOwnerH;
    int dX, dY;

    if (m_pClientList[iClientH] == 0) return;

    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);


    if ((iClientH != 0) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
    {

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

            strcpy(cNpcName, "Demon");

            dX = m_pClientList[iClientH]->m_sX;
            dY = m_pClientList[iClientH]->m_sY;

            memset(cNpcWaypoint, 0, sizeof(cNpcWaypoint));

            if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM, &dX, &dY, cNpcWaypoint, 0, 0, m_pClientList[iClientH]->m_cSide, false, true) == false)
            {

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
            }
            else
            {
                memset(cName_Master, 0, sizeof(cName_Master));
                memcpy(cName_Master, m_pClientList[iClientH]->m_cCharName, 10);
                bSetNpcFollowMode(cName, cName_Master, DEF_OWNERTYPE_PLAYER);
            }
        }
    }
}

void CGame::AdminOrder_ReserveFightzone(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;
    int iNum;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (m_iFightzoneNoForceRecall == 0)
    {
        m_iFightzoneNoForceRecall = 1;
        SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -5, 0, 0, 0);
    }
    else
    {
        m_iFightzoneNoForceRecall = 0;
        SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -4, 0, 0, 0);
    }

    if (token == 0)
    {
        delete pStrTok;
        return;
    }

    iNum = atoi(token) - 1;

    if (m_iFightZoneReserve[iNum] != -1)
    {
        wsprintf(G_cTxt, "GM Order(%s): FightzoneNumber(%d) FightzoneReserved", m_pClientList[iClientH]->m_cCharName, iNum);

        m_iFightZoneReserve[iNum] = -1;

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, iNum + 1, 0, 0, 0);

    }
    else
    {
        wsprintf(G_cTxt, "GM Order(%s): FightzoneNumber(%d) Cancel FightzoneReserved", m_pClientList[iClientH]->m_cCharName, iNum);

        m_iFightZoneReserve[iNum] = 0;

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -3, 0, 0, 0);
    }
}


void CGame::AdminOrder_CloseConn(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cTargetName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

#ifdef DEF_TIAWAN
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3)
    {
#else
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 1)
    {
#endif

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
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

                if (m_pClientList[i]->m_bIsInitComplete == true)
                {


                    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_pClientList[i]->m_iAdminUserLevel)
                    {
                        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
                        delete pStrTok;
                        return;
                    }

                    wsprintf(G_cTxt, "GM Order(%s): Close Connection PC(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cCharName);

                    DeleteClient(i, true, true, true, true);
                }

                delete pStrTok;
                return;
            }

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cTargetName);
    }

    delete pStrTok;
    return;
    }



void CGame::AdminOrder_CreateFish(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;
    CItem * pItem;
    int tX, tY, iType;
    int iItemID = 0;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    tX = tY = iType = 0;

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token != 0)
    {
        tX = atoi(token);
    }

    token = pStrTok->pGet();
    if (token != 0)
    {
        tY = atoi(token);
    }

    token = pStrTok->pGet();
    if (token != 0)
    {
        iType = atoi(token);
    }

    if ((tX != 0) && (tY != 0) && (iType != 0))
    {

        pItem = new CItem;
        if (pItem == 0)
        {
            delete pStrTok;
            return;
        }

        iItemID = 100;

        if (_bInitItemAttr(pItem, iItemID) == true)
        {
            iCreateFish(m_pClientList[iClientH]->m_cMapIndex, tX, tY, iType, pItem, 1, 60000 * 20);

            delete pItem;
            pItem = 0;
        }
        else
        {
            delete pItem;
            pItem = 0;
        }
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_Teleport(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cMapName[256];
    CStrTok * pStrTok;
    int dX, dY;
    bool   bFlag;

    dX = dY = -1;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


    if (m_pClientList[iClientH]->m_bIsObserverMode == true) return;


    if (m_pClientList[iClientH]->m_iAdminUserLevel < 1)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token != 0)
    {
        memset(cMapName, 0, sizeof(cMapName));
        strcpy(cMapName, token);
    }

    token = pStrTok->pGet();
    if (token != 0)
    {
        dX = atoi(token);
    }

    token = pStrTok->pGet();
    if (token != 0)
    {
        dY = atoi(token);
    }



    bFlag = false;
    if (strcmp("arefarm", cMapName) == 0) bFlag = true;
    if (strcmp("elvfarm", cMapName) == 0) bFlag = true;
    if (strcmp("aresden", cMapName) == 0) bFlag = true;
    if (strcmp("aresdend1", cMapName) == 0) bFlag = true;
    if (strcmp("cath_1", cMapName) == 0) bFlag = true;
    if (strcmp("wrhus_1", cMapName) == 0) bFlag = true;
    if (strcmp("bsmith_1", cMapName) == 0) bFlag = true;
    if (strcmp("gshop_1", cMapName) == 0) bFlag = true;
    if (strcmp("wrhus_1f", cMapName) == 0) bFlag = true;
    if (strcmp("bsmith_1f", cMapName) == 0) bFlag = true;
    if (strcmp("gshop_1f", cMapName) == 0) bFlag = true;
    if (strcmp("gldhall_1", cMapName) == 0) bFlag = true;
    if (strcmp("cityhall_1", cMapName) == 0) bFlag = true;
    if (strcmp("wzdtwr_1", cMapName) == 0) bFlag = true;
    if (strcmp("huntzone2", cMapName) == 0) bFlag = true;
    if (strcmp("huntzone4", cMapName) == 0) bFlag = true;
    if (strcmp("areuni", cMapName) == 0) bFlag = true;
    if (strcmp("resurr1", cMapName) == 0) bFlag = true;
    if (strcmp("resurr2", cMapName) == 0) bFlag = true;

    if (strcmp("elvine", cMapName) == 0)      bFlag = true;
    if (strcmp("elvined1", cMapName) == 0)    bFlag = true;
    if (strcmp("cath_2", cMapName) == 0)      bFlag = true;
    if (strcmp("wrhus_2", cMapName) == 0)     bFlag = true;
    if (strcmp("gshop_2", cMapName) == 0)     bFlag = true;
    if (strcmp("bsmith_2", cMapName) == 0) bFlag = true;
    if (strcmp("wrhus_2f", cMapName) == 0)     bFlag = true;
    if (strcmp("gshop_2f", cMapName) == 0)     bFlag = true;
    if (strcmp("bsmith_2f", cMapName) == 0) bFlag = true;
    if (strcmp("gldhall_2", cMapName) == 0)   bFlag = true;
    if (strcmp("cityhall_2", cMapName) == 0) bFlag = true;
    if (strcmp("wzdtwr_2", cMapName) == 0) bFlag = true;
    if (strcmp("huntzone1", cMapName) == 0) bFlag = true;
    if (strcmp("huntzone3", cMapName) == 0) bFlag = true;
    if (strcmp("elvuni", cMapName) == 0) bFlag = true;

    if (strcmp("dglv2", cMapName) == 0) bFlag = true;
    if (strcmp("dglv3", cMapName) == 0) bFlag = true;
    if (strcmp("dglv4", cMapName) == 0) bFlag = true;

    if (strcmp("bisle", cMapName) == 0)   bFlag = true;
    if (strcmp("default", cMapName) == 0) bFlag = true;

    if (strcmp("arejail", cMapName) == 0) bFlag = true;
    if (strcmp("elvjail", cMapName) == 0) bFlag = true;

    if (strcmp("fightzone1", cMapName) == 0) bFlag = true;
    if (strcmp("fightzone2", cMapName) == 0) bFlag = true;
    if (strcmp("fightzone3", cMapName) == 0) bFlag = true;
    if (strcmp("fightzone4", cMapName) == 0) bFlag = true;

    if (strcmp("fightzone5", cMapName) == 0) bFlag = true;
    if (strcmp("fightzone6", cMapName) == 0) bFlag = true;
    if (strcmp("fightzone7", cMapName) == 0) bFlag = true;
    if (strcmp("fightzone8", cMapName) == 0) bFlag = true;
    if (strcmp("fightzone9", cMapName) == 0) bFlag = true;

    if (strcmp("middleland", cMapName) == 0) bFlag = true;

    if (strcmp("middled1n", cMapName) == 0) bFlag = true;
    if (strcmp("middled1x", cMapName) == 0) bFlag = true;

    if (strcmp("arebrk11", cMapName) == 0) bFlag = true;
    if (strcmp("arebrk12", cMapName) == 0) bFlag = true;
    if (strcmp("arebrk21", cMapName) == 0) bFlag = true;
    if (strcmp("arebrk22", cMapName) == 0) bFlag = true;

    if (strcmp("elvbrk11", cMapName) == 0) bFlag = true;
    if (strcmp("elvbrk12", cMapName) == 0) bFlag = true;
    if (strcmp("elvbrk21", cMapName) == 0) bFlag = true;
    if (strcmp("elvbrk22", cMapName) == 0) bFlag = true;

    if (strcmp("arewrhus", cMapName) == 0) bFlag = true;
    if (strcmp("elvwrhus", cMapName) == 0) bFlag = true;

    if (strcmp("toh1", cMapName) == 0) bFlag = true;
    if (strcmp("toh2", cMapName) == 0) bFlag = true;
    if (strcmp("toh3", cMapName) == 0) bFlag = true;

    if (strcmp("2ndmiddle", cMapName) == 0) bFlag = true;
    if (strcmp("icebound", cMapName) == 0) bFlag = true;

    if (strcmp("procella", cMapName) == 0) bFlag = true;
    if (strcmp("inferniaA", cMapName) == 0) bFlag = true;
    if (strcmp("inferniaB", cMapName) == 0) bFlag = true;
    if (strcmp("maze", cMapName) == 0) bFlag = true;
    if (strcmp("HRampart", cMapName) == 0)     bFlag = true;
    if (strcmp("GodH", cMapName) == 0)   bFlag = true;
    if (strcmp("druncncity", cMapName) == 0) bFlag = true;
    if (strcmp("CmdHall_1", cMapName) == 0) bFlag = true;
    if (strcmp("CmdHall_2", cMapName) == 0) bFlag = true;
    if (strcmp("BtField", cMapName) == 0) bFlag = true;
    if (strcmp("abaddon", cMapName) == 0) bFlag = true;


    if (bFlag == false) { delete pStrTok; return; }

    if ((dX == -1) && (dY == -1))
        wsprintf(G_cTxt, "GM Order(%s): teleport MapName(%s)", m_pClientList[iClientH]->m_cCharName, cMapName);
    else wsprintf(G_cTxt, "GM Order(%s): teleport MapName(%s)(%d %d)", m_pClientList[iClientH]->m_cCharName, cMapName, dX, dY);

    RequestTeleportHandler(iClientH, "2   ", cMapName, dX, dY);

    delete pStrTok;
    return;
}

void CGame::AdminOrder_CheckIP(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cIP[21], cInfoString[500];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 5)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

        memset(cIP, 0, sizeof(cIP));
        strcpy(cIP, token);

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cIPaddress, cIP, strlen(cIP)) == 0))
            {

                memset(cInfoString, 0, sizeof(cInfoString));
                wsprintf(cInfoString, "Name(%s/%s) Loc(%s: %d %d) Level(%d:%d) Init(%d) IP(%s)",
                    m_pClientList[i]->m_cAccountName, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cMapName,
                    m_pClientList[i]->m_sX, m_pClientList[i]->m_sY,
                    m_pClientList[i]->m_iLevel, 0/*m_pClientList[i]->m_cAccountStatus*/,
                    m_pClientList[i]->m_bIsInitComplete, m_pClientList[i]->m_cIPaddress);

                SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, cInfoString);
            }
    }


    delete pStrTok;
}

void CGame::AdminOrder_Polymorph(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;



    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

    }

    delete pStrTok;
}

void CGame::AdminOrder_SetInvi(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


    if (m_pClientList[iClientH]->m_iAdminUserLevel < 1)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

        if (token[0] == '1') SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, true);
        else
            if (token[0] == '0') SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
    }

    delete pStrTok;
}

void CGame::AdminOrder_GetNpcStatus(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cNpcName[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);

    token = pStrTok->pGet();
    token = pStrTok->pGet();
    if (token != 0)
    {
        memset(cNpcName, 0, sizeof(cNpcName));
        strcpy(cNpcName, token);
    }

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0)
        {
            if (memcmp(m_pNpcList[i]->m_cName, cNpcName, 5) == 0)
            {
                SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINIFO, 1, i, 0, 0);
            }
        }

    delete pStrTok;
    return;
}



void CGame::AdminOrder_SetAttackMode(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;
    int    i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    //v1.42 ±æµåÀü¿ë ÀÓ½Ã ÄÚµå 
    if (memcmp(m_pClientList[iClientH]->m_cMapName, "fight", 4) != 0)
    {
        return;
    }

#ifdef DEF_TAIWAN
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {
#else

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {
#endif

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    //testcode
    //wsprintf(G_cTxt, "%s", token);
    //log->info(G_cTxt);

    if (token != 0)
    {

        if (token[0] == '1')
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled = true;

            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) && (m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex))
                    SendNotifyMsg(0, i, DEF_NOTIFY_GLOBALATTACKMODE, (int)m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled, 0, 0, 0);
        }
        else if (token[0] == '0')
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled = false;


            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) && (m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex))
                    SendNotifyMsg(0, i, DEF_NOTIFY_GLOBALATTACKMODE, (int)m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled, 0, 0, 0);
        }
    }

    delete pStrTok;
}

void CGame::AdminOrder_SetForceRecallTime(int iClientH, char * pData, uint32_t dwMsgSize)
{
    CStrTok * pStrTok;
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    int    iTime;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {

        if (token == 0)
            iTime = 0;
        else iTime = atoi(token);

        if (iTime < 0) iTime = 0;

        m_sForceRecallTime = iTime;

        wsprintf(G_cTxt, "(!) Game Server Force Recall Time (%d)min", m_sForceRecallTime);
        log->info(G_cTxt);
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_UnsummonAll(int iClientH)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif


    for (i = 1; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0)
        {
            if (m_pNpcList[i]->m_bIsKilled == false)
            {
                m_pNpcList[i]->m_bIsSummoned = true;
                NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
            }
        }
}

void CGame::AdminOrder_UnsummonBoss(int iClientH)
{
    int i;

    if (m_pClientList[iClientH] == 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

    for (i = 1; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != 0)
        {
            if (((m_pNpcList[i]->m_sType == 31) || (m_pNpcList[i]->m_sType == 49) || (m_pNpcList[i]->m_sType == 45) || (m_pNpcList[i]->m_sType == 47) || (m_pNpcList[i]->m_sType == 50) || (m_pNpcList[i]->m_sType == 52)) &&
                (m_pNpcList[i]->m_bIsKilled == false))
            {

                m_pNpcList[i]->m_bIsSummoned = true;
                NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
            }
        }
}

void CGame::AdminOrder_Summon(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cName_Master[10], cName_Slave[10], cNpcName[256], cWaypoint[11], cSA;
    int    pX, pY, j, iNum, iNamingValue;
    CStrTok * pStrTok;
    bool   bMaster;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

//     if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
//     {
// 
//         SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
//         return;
//     }
// 
// 
// #ifndef DEF_TESTSERVER	
//     if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
// #endif

    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == 0)
    {
        delete pStrTok;
        return;
    }

    memset(cNpcName, 0, sizeof(cNpcName));
    strcpy(cNpcName, token);

    token = pStrTok->pGet();

    if (token != 0)
        iNum = atoi(token);
    else iNum = 1;

    if (iNum <= 0)  iNum = 1;
    if (iNum >= 50)  iNum = 50;

    cSA = 0;
    pX = m_pClientList[iClientH]->m_sX;
    pY = m_pClientList[iClientH]->m_sY;

    wsprintf(G_cTxt, "(!) Admin Order: Summon(%s)-(%d)", cNpcName, iNum);
    log->info(G_cTxt);

    wsprintf(G_cTxt, "GM Order(%s): Summon NPC(%s)-Count(%d)", m_pClientList[iClientH]->m_cCharName, cNpcName, iNum);

    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue != -1)
    {

        memset(cName_Master, 0, sizeof(cName_Master));
        wsprintf(cName_Master, "XX%d", iNamingValue);
        cName_Master[0] = '_';
        cName_Master[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        if ((bMaster = bCreateNewNpc(cNpcName, cName_Master, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, false, true)) == false)
        {

            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
    }

    for (j = 0; j < (iNum - 1); j++)
    {
        iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue != -1)
        {

            memset(cName_Slave, 0, sizeof(cName_Slave));
            wsprintf(cName_Slave, "XX%d", iNamingValue);
            cName_Slave[0] = '_';
            cName_Slave[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

            if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, false) == false)
            {

                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
            }
            else
            {

                bSetNpcFollowMode(cName_Slave, cName_Master, DEF_OWNERTYPE_NPC);
            }
        }
    }

    delete pStrTok;
}

void CGame::AdminOrder_SummonPlayer(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

    memset(cName, 0, sizeof(cName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0)
    {
        if (strlen(token) > 10)
            memcpy(cName, token, 10);
        else memcpy(cName, token, strlen(token));


        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
            {


                if (i == iClientH)
                {
                    delete pStrTok;
                    return;
                }

#ifdef DEF_TAIWANLOG
                short sX = 0, sY = 0;
                char cMapName[22];

                ZeroMemory(cMapName, sizeof(cMapName));

                sX = m_pClientList[i]->m_sX;
                sY = m_pClientList[i]->m_sY;
                strcpy(cMapName, m_pClientList[i]->m_cMapName);

                m_pClientList[i]->m_sX = m_pClientList[iClientH]->m_sX;
                m_pClientList[i]->m_sY = m_pClientList[iClientH]->m_sY;
                strcpy(m_pClientList[i]->m_cMapName, m_pClientList[iClientH]->m_cMapName);

                _bItemLog(DEF_ITEMLOG_SUMMONPLAYER, i, m_pClientList[iClientH]->m_cCharName, 0);

                m_pClientList[i]->m_sY = sY;
                m_pClientList[i]->m_sX = sX;

                strcpy(m_pClientList[i]->m_cMapName, cMapName);

#endif 

                RequestTeleportHandler(i, "2   ", m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
                delete pStrTok;
                return;
            }


        wsprintf(G_cTxt, "GM Order(%s): PC(%s) Summoned to (%s)", m_pClientList[iClientH]->m_cCharName, cName, m_pClientList[iClientH]->m_cMapName);
        delete pStrTok;
        return;
    }

    delete pStrTok;
}

void CGame::AdminOrder_DisconnectAll(int iClientH, char * pData, uint32_t dwMsgSize)
{

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


#ifndef DEF_TESTSERVER	
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;
#endif

    if (m_pClientList[iClientH]->m_iAdminUserLevel < 4)
    {

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    _iForcePlayerDisconect(DEF_MAXCLIENTS);
}

void CGame::AdminOrder_SetObserverMode(int iClientH)
{
    if (m_pClientList[iClientH] == 0) return;
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) return;

    if (m_pClientList[iClientH]->m_bIsObserverMode == true)
    {

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_OBSERVERMODE, 0, 0, 0, 0);
        m_pClientList[iClientH]->m_bIsObserverMode = false;
    }
    else
    {

        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(1, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);

        SendNotifyMsg(0, iClientH, DEF_NOTIFY_OBSERVERMODE, 1, 0, 0, 0);
        m_pClientList[iClientH]->m_bIsObserverMode = true;
    }
}

void CGame::AdminOrder_EnableAdminCreateItem(int iClientH, char * pData, uint32_t dwMsgSize)
{
    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

#ifdef DEF_TAIWAN
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 4)
    {
#else
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 4)
    {
#endif

        return;
    }

    m_pClientList[iClientH]->m_bIsAdminCreateItemEnabled = true;
}

void CGame::AdminOrder_EnableAdminCommand(int iClientH, char * pData, uint32_t dwMsgSize)
{
    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;


    if (m_pClientList[iClientH]->m_iAdminUserLevel < 1)
    {
        return;
    }

    m_pClientList[iClientH]->m_bIsAdminCommandEnabled = true;
}

void CGame::AdminOrder_CreateItem(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * cp, * token, cBuff[256], cItemName[256], cData[256], cTemp[256], cAttribute[256], cValue[256];
    SYSTEMTIME SysTime;
    CStrTok * pStrTok;
    CItem * pItem;
    short * sp;
    int    iRet, iTemp, iEraseReq, iValue;
    uint32_t * dwp;
    uint16_t * wp, wTemp;
    double dV1, dV2, dV3;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

#ifdef	DEF_TAIWAN
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 3)
    {
#else
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 4)
    {
#endif


        if (m_pClientList[iClientH]->m_iAdminUserLevel != 0)
            SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
        return;
    }

    //	if (memcmp(m_pClientList[iClientH]->m_cIPaddress, "203.234.215.", 12) != 0) return;


    if (m_pClientList[iClientH]->m_bIsAdminCreateItemEnabled == false) return;


    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token != 0)
    {
        memset(cItemName, 0, sizeof(cItemName));
        strcpy(cItemName, token);
    }


    memset(cAttribute, 0, sizeof(cAttribute));
    token = pStrTok->pGet();
    if (token != 0)
    {
        memset(cAttribute, 0, sizeof(cAttribute));
        strcpy(cAttribute, token);
    }


    memset(cValue, 0, sizeof(cValue));
    iValue = 0;
    token = pStrTok->pGet();
    if (token != 0)
    {
        memset(cValue, 0, sizeof(cValue));
        strcpy(cValue, token);
        iValue = atoi(cValue);
    }


    pItem = new CItem;

    if (_bInitItemAttr(pItem, cItemName) == false)
    {
        delete pItem;
        return;
    }


    if (strlen(cAttribute) != 0)
    {
        pItem->m_dwAttribute = atoi(cAttribute);
        if (pItem->m_dwAttribute == 1)
        {
            if ((iValue >= 1) && (iValue <= 200))
            {

                pItem->m_cItemColor = 2;
                pItem->m_sItemSpecEffectValue2 = iValue - 100;


                dV2 = (double)pItem->m_sItemSpecEffectValue2;
                dV3 = (double)pItem->m_wMaxLifeSpan;
                dV1 = (dV2 / 100.0f) * dV3;

                iTemp = (int)pItem->m_wMaxLifeSpan;
                iTemp += (int)dV1;

                if (iTemp <= 0)
                    wTemp = 1;
                else wTemp = (WORD)iTemp;

                if (wTemp <= pItem->m_wMaxLifeSpan * 2)
                {

                    pItem->m_wMaxLifeSpan = wTemp;
                    pItem->m_sItemSpecEffectValue1 = (short)wTemp;
                    pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
                }
                else pItem->m_sItemSpecEffectValue1 = (short)pItem->m_wMaxLifeSpan;

            }
            else pItem->m_dwAttribute = 0;
        }
        else
        {

            if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK)
            {
                switch ((pItem->m_dwAttribute & 0xF00000) >> 20)
                {
                    case 6:	pItem->m_cItemColor = 2; break;
                    case 8: pItem->m_cItemColor = 3; break;
                    case 1:	pItem->m_cItemColor = 5; break;
                    case 5:	pItem->m_cItemColor = 1; break;
                    case 3:	pItem->m_cItemColor = 7; break;
                    case 2:	pItem->m_cItemColor = 4; break;
                    case 7:	pItem->m_cItemColor = 6; break;
                    case 9:	pItem->m_cItemColor = 8; break;
                }
            }
            else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE)
            {
                switch ((pItem->m_dwAttribute & 0xF00000) >> 20)
                {
                    case 10: pItem->m_cItemColor = 5; break;
                }
            }
        }
    }


    switch (pItem->m_sIDnum)
    {
        case 511:
        case 513:
        case 515:
        case 517:
        case 530:
        case 531:
        case 532:
        case 533:
        case 534:
            GetLocalTime(&SysTime);
            pItem->m_sTouchEffectType = DEF_ITET_DATE;

            pItem->m_sTouchEffectValue1 = (short)SysTime.wMonth;
            pItem->m_sTouchEffectValue2 = (short)SysTime.wDay;
            pItem->m_sTouchEffectValue3 = 24;
            break;

        default:
            GetLocalTime(&SysTime);
            pItem->m_sTouchEffectType = DEF_ITET_ID;
            pItem->m_sTouchEffectValue1 = iDice(1, 100000);
            pItem->m_sTouchEffectValue2 = iDice(1, 100000);

            memset(cTemp, 0, sizeof(cTemp));
            wsprintf(cTemp, "%d%2d", (short)SysTime.wMonth, (short)SysTime.wDay);
            pItem->m_sTouchEffectValue3 = atoi(cTemp);
            break;
    }

    memset(cData, 0, sizeof(cData));
    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
    {

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

#ifdef DEF_TAIWANLOG 
        _bItemLog(DEF_ITEMLOG_CREATE, iClientH, (int)-1, pItem);
#endif

        if (iEraseReq == 1)
        {
            delete pItem;
            pItem = 0;
        }

        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);

        wsprintf(G_cTxt, "GM Order(%s): Create ItemName(%s)", m_pClientList[iClientH]->m_cCharName, cItemName);

        return;
    }
    else
    {

        delete pItem;
        return;
    }
}

void CGame::AdminOrder_GetFightzoneTicket(int iClientH)
{
    int iReserveTime, iFightzoneTN, iFightzoneN;
    char cTemp[21];
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == 0) return;
    if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "fightzone", 9) == 0)
    {

        iReserveTime = m_pClientList[iClientH]->m_iReserveTime;
        GetLocalTime(&SysTime);
        m_pClientList[iClientH]->m_iReserveTime = SysTime.wMonth * 10000 + SysTime.wDay * 100 + (SysTime.wHour + 3);

        memset(cTemp, 0, sizeof(cTemp));
        strcpy(cTemp, (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName + 9));
        iFightzoneN = m_pClientList[iClientH]->m_iFightzoneNumber;
        iFightzoneTN = m_pClientList[iClientH]->m_iFightZoneTicketNumber;
        m_pClientList[iClientH]->m_iFightZoneTicketNumber = 10;
        m_pClientList[iClientH]->m_iFightzoneNumber = atoi(cTemp);

        GetFightzoneTicketHandler(iClientH);
        GetFightzoneTicketHandler(iClientH);
        GetFightzoneTicketHandler(iClientH);

        m_pClientList[iClientH]->m_iFightzoneNumber = iFightzoneN;
        m_pClientList[iClientH]->m_iFightZoneTicketNumber = iFightzoneTN;
        m_pClientList[iClientH]->m_iReserveTime = iReserveTime;
    }
    else
    {
        iReserveTime = m_pClientList[iClientH]->m_iReserveTime;
        GetLocalTime(&SysTime);
        m_pClientList[iClientH]->m_iReserveTime = SysTime.wMonth * 10000 + SysTime.wDay * 100 + (SysTime.wHour + 2);

        memset(cTemp, 0, sizeof(cTemp));
        strcpy(cTemp, (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName + 9));
        iFightzoneN = m_pClientList[iClientH]->m_iFightzoneNumber;
        iFightzoneTN = m_pClientList[iClientH]->m_iFightZoneTicketNumber;
        m_pClientList[iClientH]->m_iFightZoneTicketNumber = 10;
        m_pClientList[iClientH]->m_iFightzoneNumber = 1;

        GetFightzoneTicketHandler(iClientH);
        GetFightzoneTicketHandler(iClientH);
        GetFightzoneTicketHandler(iClientH);

        m_pClientList[iClientH]->m_iFightzoneNumber = iFightzoneN;
        m_pClientList[iClientH]->m_iFightZoneTicketNumber = iFightzoneTN;
        m_pClientList[iClientH]->m_iReserveTime = iReserveTime;
    }
}


void CGame::AdminOrder_Add(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cSendmsg[50], cBuff[256];
    CStrTok * pStrTok;
    int i, o;
    int * ip, iRet;
    uint32_t * dwp;
    uint16_t * wp;
    char * cp;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    memset(cSendmsg, 0, sizeof(cSendmsg));
    memset(cName, 0, sizeof(cName));
    memset(cBuff, 0, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);


    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == 0)
    {
        delete pStrTok;
        return;
    }
    strcpy(cName, token);

    for (int j = 0; j < 50; j++)
    {
        if (!memcmp(m_pClientList[iClientH]->m_stFriendsList[j].m_cCharName, cName, 10))
        {
            wsprintf(cSendmsg, "Player %s is already on your friends list.", cName);
            SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cSendmsg);

            delete pStrTok;
            return;
        }
    }
    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
        {

            for (o = 0; o < 50; o++)
            {
                if (m_pClientList[iClientH]->m_stFriendsList[o].m_bUsed == false)
                {
                    memcpy(m_pClientList[iClientH]->m_stFriendsList[o].m_cCharName, m_pClientList[i]->m_cCharName, 10);
                    memcpy(m_pClientList[iClientH]->m_stFriendsList[o].m_cMapName, m_pClientList[i]->m_cMapName, 10);
                    m_pClientList[iClientH]->m_stFriendsList[o].m_bUsed = true;
                    //(cSendmsg, "Player %s added to your friends list.", m_pClientList[iClientH]->m_stFriendsList[o].m_cCharName);
                    //SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cSendmsg);

                    dwp = (uint32_t *)(cBuff + DEF_INDEX4_MSGID);
                    *dwp = MSGID_RESPONSE_FRIENDSLIST;
                    wp = (uint16_t *)(cBuff + DEF_INDEX2_MSGTYPE);
                    *wp = 1; // 5 = friend go offline / 4 = friend come online / = 3 = full update / 2 = delete friend / 1 = add friend

                    cp = (char *)(cBuff + DEF_INDEX2_MSGTYPE + 2);

                    memcpy(cp, m_pClientList[iClientH]->m_stFriendsList[o].m_cCharName, 10);
                    cp += 10;

                    memcpy(cp, m_pClientList[iClientH]->m_stFriendsList[o].m_cMapName, 10);
                    cp += 10;

                    m_pClientList[iClientH]->m_stFriendsList[o].m_cSide = m_pClientList[i]->m_cSide;
                    *cp = m_pClientList[i]->m_cSide;
                    cp++;

                    m_pClientList[iClientH]->m_stFriendsList[o].m_iLevel = m_pClientList[i]->m_iLevel;
                    ip = (int *)cp;
                    *ip = m_pClientList[i]->m_iLevel;
                    cp += 4;

                    ip = (int *)cp;
                    *ip = m_pClientList[i]->m_iPKCount;
                    cp += 4;

                    iRet = m_pClientList[iClientH]->iSendMsg(cBuff, 35);


                    delete pStrTok;
                    return;
                }
            }
        }


    SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);

    delete pStrTok;
    return;
}
void CGame::AdminOrder_View(int iClientH, char * pData, uint32_t dwMsgSize)
{
    char   cName[11], cSendmsg[90], cCharListBuffer[1000];
    int i;
    bool bTest = false;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    memset(cSendmsg, 0, sizeof(cSendmsg));
    memset(cName, 0, sizeof(cName));
    memset(cCharListBuffer, 0, sizeof(cCharListBuffer));

    for (i = 0; i < 50; i++)
        if (m_pClientList[iClientH]->m_stFriendsList[i].m_bUsed == true)
        {
//             dwp = (uint32_t *)(cCharListBuffer + DEF_INDEX4_MSGID);
//             *dwp = MSGID_FRIENDSLIST;
//             wp = (uint16_t *)(cCharListBuffer + DEF_INDEX2_MSGTYPE);
//             *wp = 3; //Send to gate 1 = online 2 = offline 3 = get info 4 = set info
// 
//             cp = (char *)(cCharListBuffer + DEF_INDEX2_MSGTYPE + 2);
// 
//             memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
//             cp += 10;
// 
//             memcpy(cp, m_pClientList[iClientH]->m_stFriendsList[i].m_cCharName, 10);
//             cp += 10;
// 
//             SendMsgToGateServer(MSGID_FRIENDSLIST, 26, cCharListBuffer);
        }

    //for (i = 0; i < 50; i++) 
    //	if (m_pClientList[iClientH]->m_stFriendsList[i].m_bUsed == true) {
    //		for (o = 0; o < DEF_MAXCLIENTS; o++)
    //		{
    //			if (m_pClientList[o] != 0)
    //				if (memcmp(m_pClientList[iClientH]->m_stFriendsList[i].m_cCharName, m_pClientList[o]->m_cCharName, 10) == 0)
    //				{
    //					wsprintf(cSendmsg, "Player %s is on friends list online as id %d %d", m_pClientList[iClientH]->m_stFriendsList[i].m_cCharName, o, m_pClientList[iClientH]->m_stFriendsList[i].m_iID);
    //					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cSendmsg);
    //					bTest = true;
    //					break;
    //				}
    //		}
    //		if (bTest == false)
    //		{
    //			bTest = false;
    //			wsprintf(cSendmsg, "Player %s is on friends list but offline", m_pClientList[iClientH]->m_stFriendsList[i].m_cCharName);
    //			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cSendmsg);
    //		}
    //	}
    SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "check done");


    //SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);

    return;
}

