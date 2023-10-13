//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

extern char G_cTxt[512];

void CGame::AdminOrder_CallGuard(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cTargetName[11], cBuff[256], cNpcName[30], cNpcWaypoint[11];
    CStrTok * pStrTok;
    int i, iNamingValue, tX, tY;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCallGaurd)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cNpcName, sizeof(cNpcName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        // tokenÃ€ÃŒ Â°Ã° ÃƒÂ¤Ã†ÃƒÃ€Â» ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ°Ã” Â¸Â¸ÂµÃ© Â»Ã§Â¿Ã«Ã€Ãš Ã€ÃŒÂ¸Â§ 
        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
            {
                // Â¸Ã±Ã‡Â¥ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. 	

                if (memcmp(m_pClientList[i]->m_cMapName, "aresden", 7) == 0)
                    strcpy(cNpcName, "Guard-Aresden");
                else if (memcmp(m_pClientList[i]->m_cMapName, "elvine", 6) == 0)
                    strcpy(cNpcName, "Guard-Elvine");
                else strcpy(cNpcName, "Guard-Neutral");  // <- Ã€ÃŒÂ°ÃÃ€Âº Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¹Ã‡Â·ÃŽ ÃÃŸÂ¸Â³ Â°Â¡ÂµÃ¥Â¸Â¦ Â»Ã½Â¼ÂºÂ½ÃƒÃ…Â²Â´Ã™.

                iNamingValue = m_pMapList[m_pClientList[i]->m_cMapIndex]->iGetEmptyNamingValue();
                if (iNamingValue == -1)
                {
                    // Â´ÃµÃ€ÃŒÂ»Ã³ Ã€ÃŒ Â¸ÃŠÂ¿Â¡ NPCÂ¸Â¦ Â¸Â¸ÂµÃ©Â¼Ã¶ Â¾Ã¸Â´Ã™. Ã€ÃŒÂ¸Â§Ã€Â» Ã‡Ã’Â´Ã§Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â±Ã¢ Â¶Â§Â¹Â®.
                }
                else
                {
                    // NPCÂ¸Â¦ Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™.
                    wsprintf(cName, "XX%d", iNamingValue);
                    cName[0] = '_';
                    cName[1] = m_pClientList[i]->m_cMapIndex + 65;

                    tX = (int)m_pClientList[i]->m_sX;
                    tY = (int)m_pClientList[i]->m_sY;
                    if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM,
                        &tX, &tY, cNpcWaypoint, NULL, NULL, -1, FALSE, TRUE) == FALSE)
                    {
                        // Â½Ã‡Ã†ÃÃ‡ÃŸÃ€Â¸Â¹Ã‡Â·ÃŽ Â¿Â¹Â¾Ã ÂµÃˆ NameValueÂ¸Â¦ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™.
                        m_pMapList[m_pClientList[i]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
                    }
                    else
                    {
                        // Â°Â¡ÂµÃ¥Â¸Â¦ Â»Ã½Â¼ÂºÂ½ÃƒÃ„Ã—Â´Ã™. Â°Ã¸Â°Ã Â¸Ã±Ã‡Â¥Â¸Â¦ Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™.
                        bSetNpcAttackMode(cName, i, DEF_OWNERTYPE_PLAYER, TRUE);
                    }
                }

                delete pStrTok;
                return;
            }

        // Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cTargetName);
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_Kill(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cTargetName[11], cBuff[256], cNpcName[30], cNpcWaypoint[11];
    CStrTok * pStrTok;
    int i;
    int sAttackerWeapon, sDamage, iExH;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGMKill)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cNpcName, sizeof(cNpcName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    //testcode
    //wsprintf(G_cTxt, "%s", token);
    //log->info(G_cTxt);
    //
    token = pStrTok->pGet();
    //testcode
    //wsprintf(G_cTxt, "%s", token);
    //log->info(G_cTxt);
    //
    if (token != NULL)
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, token);
    }
    else
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, "null");
    }

    token = pStrTok->pGet();
    if (token != NULL)
    {
        sDamage = atoi(token);
    }
    else
    {
        sDamage = 1;
    }

    //testcode
    //wsprintf(G_cTxt, "%s", token);
    //log->info(G_cTxt);
    //

    if (token == NULL) { token = "null"; }
    if (cName != NULL)
    {
        token = cName;
        // tokenÃ€ÃŒ Â°Ã° ÃƒÂ¤Ã†ÃƒÃ€Â» ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ°Ã” Â¸Â¸ÂµÃ© Â»Ã§Â¿Ã«Ã€Ãš Ã€ÃŒÂ¸Â§ 
        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
            {
                // Â¸Ã±Ã‡Â¥ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. 	

                m_pClientList[i]->m_iHP = 0;
                //if (iGetMaxHP(i) < m_pClientList[i]->m_iHP) m_pClientList[i]->m_iHP = iGetMaxHP(i);
                m_pClientList[i]->m_bIsKilled = TRUE;

                if (m_pClientList[i]->m_bIsExchangeMode == TRUE)
                {
                    iExH = m_pClientList[i]->m_iExchangeH;
                    _ClearExchangeStatus(iExH);
                    _ClearExchangeStatus(i);
                }

                RemoveFromTarget(i, DEF_OWNERTYPE_PLAYER);

                SendNotifyMsg(NULL, i, DEF_NOTIFY_KILLED, NULL, NULL, NULL, m_pClientList[iClientH]->m_cCharName);
                sAttackerWeapon = 1;
                SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, NULL);
                // ÃÂ¤Â»Ã³ Ã€Â§Ã„Â¡Â¿Â¡Â¼Â­ ÃÃ¶Â¿Ã®Â´Ã™.
                m_pMapList[m_pClientList[i]->m_cMapIndex]->ClearOwner(12, i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                // ÃÃ—Ã€Âº Ã€Â§Ã„Â¡ Ã‡Â¥Â½ÃƒÂ¸Â¦ Ã‡Ã‘Â´Ã™.
                m_pMapList[m_pClientList[i]->m_cMapIndex]->SetDeadOwner(i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);

                delete pStrTok;
                return;
            }

        // Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cTargetName);
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_Revive(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cName[11], cTargetName[11], cBuff[256], cNpcName[30], cNpcWaypoint[11];
    CStrTok * pStrTok;
    int i;
    int sAttackerWeapon, sDamage, sHP;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGMRevive)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cNpcName, sizeof(cNpcName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);

    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, token);
    }
    else
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, "null");
    }

    token = pStrTok->pGet();

    if (token != NULL)
    {
        sDamage = atoi(token);
    }
    else
    {
        sDamage = 1;
    }
    token = pStrTok->pGet();
    if (token != NULL)
    {
        sHP = atoi(token);
    }
    else
    {
        sHP = 1;
    }

    token = cName;
    //if (token == NULL) { token = "null"; }
    if (cName != NULL)
    {
        // tokenÃ€ÃŒ Â°Ã° ÃƒÂ¤Ã†ÃƒÃ€Â» ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ°Ã” Â¸Â¸ÂµÃ© Â»Ã§Â¿Ã«Ã€Ãš Ã€ÃŒÂ¸Â§ 
        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
            {
                // Â¸Ã±Ã‡Â¥ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. 	

                m_pClientList[i]->m_iHP = sHP;
                if (iGetMaxHP(i) < m_pClientList[i]->m_iHP) m_pClientList[i]->m_iHP = iGetMaxHP(i);
                m_pClientList[i]->m_bIsKilled = FALSE;
                m_pClientList[i]->m_iLastDamage = sDamage;
                //			SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGEMOVE, m_pClientList[i]->m_iLastDamage, NULL, NULL);
                SendNotifyMsg(NULL, i, DEF_NOTIFY_HP, NULL, NULL, NULL, NULL);
                sAttackerWeapon = 1;
                SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, sDamage, sAttackerWeapon, NULL);
                m_pMapList[m_pClientList[i]->m_cMapIndex]->ClearOwner(14, i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                m_pMapList[m_pClientList[i]->m_cMapIndex]->SetDeadOwner(i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);

                delete pStrTok;
                return;
            }

        // Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cTargetName);
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_SummonDemon(int iClientH)
{
    char cName[30], cNpcName[30], cNpcWaypoint[11];
    int iNamingValue, tX, tY;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == FALSE) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonDemon)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue == -1)
    {
        // Â´ÃµÃ€ÃŒÂ»Ã³ Ã€ÃŒ Â¸ÃŠÂ¿Â¡ NPCÂ¸Â¦ Â¸Â¸ÂµÃ©Â¼Ã¶ Â¾Ã¸Â´Ã™. Ã€ÃŒÂ¸Â§Ã€Â» Ã‡Ã’Â´Ã§Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â±Ã¢ Â¶Â§Â¹Â®.
    }
    else
    {
        // NPCÂ¸Â¦ Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™.
        ZeroMemory(cNpcName, sizeof(cNpcName));
        strcpy(cNpcName, "Demon");

        ZeroMemory(cName, sizeof(cName));
        wsprintf(cName, "XX%d", iNamingValue);
        cName[0] = '_';
        cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));

        tX = (int)m_pClientList[iClientH]->m_sX;
        tY = (int)m_pClientList[iClientH]->m_sY;
        if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, (rand() % 9),
            DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, NULL, NULL, -1, FALSE, FALSE) == FALSE)
        {
            // Â½Ã‡Ã†ÃÃ‡ÃŸÃ€Â¸Â¹Ã‡Â·ÃŽ Â¿Â¹Â¾Ã ÂµÃˆ NameValueÂ¸Â¦ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™.
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
        else
        {
            // Admin Log
            wsprintf(G_cTxt, "Admin Order(%s): Summon Demon", m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
        }
    }
}

void CGame::AdminOrder_SummonDeath(int iClientH)
{
    char cName[30], cNpcName[30], cNpcWaypoint[11];
    int iNamingValue, tX, tY;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == FALSE) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonDeath)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue == -1)
    {
        // Â´ÃµÃ€ÃŒÂ»Ã³ Ã€ÃŒ Â¸ÃŠÂ¿Â¡ NPCÂ¸Â¦ Â¸Â¸ÂµÃ©Â¼Ã¶ Â¾Ã¸Â´Ã™. Ã€ÃŒÂ¸Â§Ã€Â» Ã‡Ã’Â´Ã§Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â±Ã¢ Â¶Â§Â¹Â®.
    }
    else
    {
        // NPCÂ¸Â¦ Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™.
        ZeroMemory(cNpcName, sizeof(cNpcName));
        strcpy(cNpcName, "Wyvern");

        ZeroMemory(cName, sizeof(cName));
        wsprintf(cName, "XX%d", iNamingValue);
        cName[0] = '_';
        cName[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));

        tX = (int)m_pClientList[iClientH]->m_sX;
        tY = (int)m_pClientList[iClientH]->m_sY;
        if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, (rand() % 9),
            DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, NULL, NULL, -1, FALSE, FALSE) == FALSE)
        {
            // Â½Ã‡Ã†ÃÃ‡ÃŸÃ€Â¸Â¹Ã‡Â·ÃŽ Â¿Â¹Â¾Ã ÂµÃˆ NameValueÂ¸Â¦ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™.
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
        else
        {
            // Admin Log
            wsprintf(G_cTxt, "Admin Order(%s): Summon Death", m_pClientList[iClientH]->m_cCharName);
            log->info(G_cTxt);
        }
    }

}

void CGame::AdminOrder_ReserveFightzone(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;
    int iNum;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;


    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelReserveFightzone)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (m_iFightzoneNoForceRecall == 0)
    {
        m_iFightzoneNoForceRecall = 1;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -5, NULL, NULL, NULL);
    }
    else
    {
        m_iFightzoneNoForceRecall = 0;
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -4, NULL, NULL, NULL);
    }

    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    iNum = atoi(token) - 1;

    if (m_iFightZoneReserve[iNum] != -1)
    {
        // Admin Log
        wsprintf(G_cTxt, "Admin Order(%s):  %d FightzoneReserved", m_pClientList[iClientH]->m_cCharName, iNum);
        log->info(G_cTxt);

        m_iFightZoneReserve[iNum] = -1;        // Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â¿Â¹Â¾Ã Ã‡Ã‘ Â»Ã§Ã…ÃµÃ€Ã¥Ã€Âº -1 Â°ÂªÃ€ÃŒ ÂµÃ©Â¾Ã®Â°Â£Â´Ã™.
        // Ã€ÃŒÃÂ¦ÂºÃŽÃ…Ã Â°Ã¨Â¼Ã“ Â´Ã™Â¸Â¥ Ã€Â¯Ã€ÃºÂ´Ã‚ Â¿Â¹Â¾Ã Ã€ÃŒ ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ´Ã™.
// Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â»Ã§Ã…ÃµÃ€Ã¥ Â¿Â¹Â¾Ã Â¿Â¡ Â¼ÂºÂ°Ã¸Ã‡ÃÂ´Ã‚ Â°Ã¦Â¿Ã¬ 
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, iNum + 1, NULL, NULL, NULL);

    }
    else
    {	// Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â¿Â¹Â¾Ã Ã‡Ã‘ Â»Ã§Ã…ÃµÃ€Ã¥Ã€Â» Â´Ã™Â½Ãƒ Â¿Â¹Â¾Ã Ã‡ÃÂ¸Ã© Â¿Â¹Â¾Ã Ã€ÃŒ ÃƒÃ«Â¼Ã’ÂµÃˆÂ´Ã™.
        wsprintf(G_cTxt, "Admin Order(%s):  %d Cancel FightzoneReserved", m_pClientList[iClientH]->m_cCharName, iNum);
        log->info(G_cTxt);

        m_iFightZoneReserve[iNum] = 0;
        // Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â»Ã§Ã…ÃµÃ€Ã¥ Â¿Â¹Â¾Ã Ã€Â» ÃƒÃ«Â¼Ã’Ã‡ÃÂ´Ã‚ Â°Ã¦Â¿Ã¬       -3 Â°ÂªÃ€Â» Â³Ã‘Â°ÃœÃÃ˜Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -3, NULL, NULL, NULL);

    }

    return;
}


void CGame::AdminOrder_CloseConn(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cTargetName[11], cBuff[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGMCloseconn)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
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
                if (m_pClientList[i]->m_bIsInitComplete == TRUE)
                {
                    DeleteClient(i, TRUE, TRUE, TRUE, TRUE);
                }

                // Admin Log
                wsprintf(G_cTxt, "Admin Order(%s): Close Conn", m_pClientList[iClientH]->m_cCharName);
                log->info(G_cTxt);

                delete pStrTok;
                return;
            }
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, NULL, NULL, NULL, cTargetName);
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_CheckIP(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cIP[30], cInfoString[500];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCheckIP)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        // tokenÃ€ÃŒ IP addressÃ€ÃŒÂ´Ã™. 
        ZeroMemory(cIP, sizeof(cIP));
        strcpy(cIP, token);

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cIPaddress, cIP, strlen(cIP)) == 0))
            {
                // Â¿Ã¤Â±Â¸Ã‡Ã‘ ÃÃ–Â¼Ã’Â¿Ã Ã€ÃÃ„Â¡Ã‡ÃÂ´Ã‚ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â® Â¹ÃŸÂ°ÃŸ. 
                ZeroMemory(cInfoString, sizeof(cInfoString));
                wsprintf(cInfoString, "Name(%s/%s) Loc(%s: %d %d) Level(%d:%d) Init(%d) IP(%s)",
                    m_pClientList[i]->m_cAccountName, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cMapName,
                    m_pClientList[i]->m_sX, m_pClientList[i]->m_sY,
                    m_pClientList[i]->m_iLevel, NULL/*m_pClientList[i]->m_cAccountStatus*/,
                    m_pClientList[i]->m_bIsInitComplete, m_pClientList[i]->m_cIPaddress);

                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_IPACCOUNTINFO, NULL, NULL, NULL, cInfoString);
            }
    }


    delete pStrTok;
}

void CGame::AdminOrder_Polymorph(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelPolymorph)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        if (memcmp(token, "off", 3) == 0)
            m_pClientList[iClientH]->m_sType = m_pClientList[iClientH]->m_sOriginalType;

        else if (memcmp(token, "Slime", 5) == 0)
            m_pClientList[iClientH]->m_sType = 10;

        else if (memcmp(token, "Skeleton", 8) == 0)
            m_pClientList[iClientH]->m_sType = 11;

        else if (memcmp(token, "Stone-Golem", 11) == 0)
            m_pClientList[iClientH]->m_sType = 12;

        else if (memcmp(token, "Cyclops", 7) == 0)
            m_pClientList[iClientH]->m_sType = 13;

        else if (memcmp(token, "Orc", 3) == 0)
            m_pClientList[iClientH]->m_sType = 14;

        else if (memcmp(token, "ShopKeeper", 10) == 0)
            m_pClientList[iClientH]->m_sType = 15;

        else if (memcmp(token, "Giant-Ant", 9) == 0)
            m_pClientList[iClientH]->m_sType = 16;

        else if (memcmp(token, "Scorpion", 8) == 0)
            m_pClientList[iClientH]->m_sType = 17;

        else if (memcmp(token, "Zombie", 6) == 0)
            m_pClientList[iClientH]->m_sType = 18;

        else if (memcmp(token, "Gandlf", 6) == 0)
            m_pClientList[iClientH]->m_sType = 19;

        else if (memcmp(token, "Howard", 6) == 0)
            m_pClientList[iClientH]->m_sType = 20;

        else if (memcmp(token, "Gaurd", 5) == 0)
            m_pClientList[iClientH]->m_sType = 21;

        else if (memcmp(token, "Amphis", 6) == 0)
            m_pClientList[iClientH]->m_sType = 22;

        else if (memcmp(token, "Clay-Golem", 10) == 0)
            m_pClientList[iClientH]->m_sType = 23;

        else if (memcmp(token, "Tom", 3) == 0)
            m_pClientList[iClientH]->m_sType = 24;

        else if (memcmp(token, "William", 7) == 0)
            m_pClientList[iClientH]->m_sType = 25;

        else if (memcmp(token, "Kennedy", 7) == 0)
            m_pClientList[iClientH]->m_sType = 26;

        else if (memcmp(token, "Hellbound", 9) == 0)
            m_pClientList[iClientH]->m_sType = 27;

        else if (memcmp(token, "Troll", 5) == 0)
            m_pClientList[iClientH]->m_sType = 28;

        else if (memcmp(token, "Orge", 4) == 0)
            m_pClientList[iClientH]->m_sType = 29;

        else if (memcmp(token, "Liche", 5) == 0)
            m_pClientList[iClientH]->m_sType = 30;

        else if (memcmp(token, "Demon", 5) == 0)
            m_pClientList[iClientH]->m_sType = 31;

        else if (memcmp(token, "Unicorn", 7) == 0)
            m_pClientList[iClientH]->m_sType = 32;

        else if (memcmp(token, "WereWolf", 8) == 0)
            m_pClientList[iClientH]->m_sType = 33;

        else if (memcmp(token, "Dummy", 5) == 0)
            m_pClientList[iClientH]->m_sType = 34;

        else if (memcmp(token, "Energy-Sphere", 13) == 0)
            m_pClientList[iClientH]->m_sType = 35;

        else if (memcmp(token, "AGT", 3) == 0)
            m_pClientList[iClientH]->m_sType = 36;

        else if (memcmp(token, "CGT", 3) == 0)
            m_pClientList[iClientH]->m_sType = 37;

        else if (memcmp(token, "MS", 2) == 0)
            m_pClientList[iClientH]->m_sType = 38;

        else if (memcmp(token, "DT", 2) == 0)
            m_pClientList[iClientH]->m_sType = 39;

        else if (memcmp(token, "ESG", 3) == 0)
            m_pClientList[iClientH]->m_sType = 40;

        else if (memcmp(token, "GMG", 3) == 0)
            m_pClientList[iClientH]->m_sType = 41;

        else if (memcmp(token, "ManaStone", 9) == 0)
            m_pClientList[iClientH]->m_sType = 42;

        else if (memcmp(token, "LWB", 3) == 0)
            m_pClientList[iClientH]->m_sType = 43;

        else if (memcmp(token, "GHK", 3) == 0)
            m_pClientList[iClientH]->m_sType = 44;

        else if (memcmp(token, "GHC", 6) == 0)
            m_pClientList[iClientH]->m_sType = 45;

        else if (memcmp(token, "TK", 2) == 0)
            m_pClientList[iClientH]->m_sType = 46;

        else if (memcmp(token, "BG", 2) == 0)
            m_pClientList[iClientH]->m_sType = 47;

        else if (memcmp(token, "Stalker", 7) == 0)
            m_pClientList[iClientH]->m_sType = 48;

        else if (memcmp(token, "Hellclaw", 8) == 0)
            m_pClientList[iClientH]->m_sType = 49;

        else if (memcmp(token, "Tigerworm", 8) == 0)
            m_pClientList[iClientH]->m_sType = 50;

        else if (memcmp(token, "CP", 2) == 0)
            m_pClientList[iClientH]->m_sType = 51;

        else if (memcmp(token, "Gagoyle", 7) == 0)
            m_pClientList[iClientH]->m_sType = 52;

        else if (memcmp(token, "Beholder", 8) == 0)
            m_pClientList[iClientH]->m_sType = 53;

        else if (memcmp(token, "Dark-Elf", 8) == 0)
            m_pClientList[iClientH]->m_sType = 54;

        else if (memcmp(token, "Rabbit", 6) == 0)
            m_pClientList[iClientH]->m_sType = 55;

        else if (memcmp(token, "Cat", 3) == 0)
            m_pClientList[iClientH]->m_sType = 56;

        else if (memcmp(token, "Giant-Frog", 10) == 0)
            m_pClientList[iClientH]->m_sType = 57;

        else if (memcmp(token, "Mountain-Giant", 14) == 0)
            m_pClientList[iClientH]->m_sType = 58;

        else if (memcmp(token, "Ettin", 5) == 0)
            m_pClientList[iClientH]->m_sType = 59;

        else if (memcmp(token, "Cannibal-Plant", 13) == 0)
            m_pClientList[iClientH]->m_sType = 60;

        else if (memcmp(token, "Rudolph", 7) == 0)
            m_pClientList[iClientH]->m_sType = 61;

        else if (memcmp(token, "DireBoar", 8) == 0)
            m_pClientList[iClientH]->m_sType = 62;

        else if (memcmp(token, "Frost", 5) == 0)
            m_pClientList[iClientH]->m_sType = 63;

        else if (memcmp(token, "Crops", 5) == 0)
            m_pClientList[iClientH]->m_sType = 64;

        else if (memcmp(token, "Ice-Golem", 9) == 0)
            m_pClientList[iClientH]->m_sType = 65;

        //else if (memcmp(token, "Wyvern", 6) == 0)
        //	m_pClientList[iClientH]->m_sType = 66;

        else if (memcmp(token, "McGaffin", 8) == 0)
            m_pClientList[iClientH]->m_sType = 67;

        else if (memcmp(token, "Perry", 5) == 0)
            m_pClientList[iClientH]->m_sType = 68;

        else if (memcmp(token, "Devlin", 6) == 0)
            m_pClientList[iClientH]->m_sType = 69;

        else if (memcmp(token, "Barlog", 6) == 0)
            m_pClientList[iClientH]->m_sType = 70;

        else if (memcmp(token, "Centaurus", 9) == 0)
            m_pClientList[iClientH]->m_sType = 71;

        else if (memcmp(token, "Claw-Turtle", 11) == 0)
            m_pClientList[iClientH]->m_sType = 72;

        //else if (memcmp(token, "Fire-Wyvern", 11) == 0)
        //	m_pClientList[iClientH]->m_sType = 73;

        else if (memcmp(token, "Giant-Crayfish", 14) == 0)
            m_pClientList[iClientH]->m_sType = 74;

        else if (memcmp(token, "Giant-Lizard", 12) == 0)
            m_pClientList[iClientH]->m_sType = 75;

        else if (memcmp(token, "Giant-Plant", 11) == 0)
            m_pClientList[iClientH]->m_sType = 76;

        else if (memcmp(token, "MasterMage-Orc", 14) == 0)
            m_pClientList[iClientH]->m_sType = 77;

        else if (memcmp(token, "Minotaurs", 9) == 0)
            m_pClientList[iClientH]->m_sType = 78;

        else if (memcmp(token, "Nizie", 5) == 0)
            m_pClientList[iClientH]->m_sType = 79;

        else if (memcmp(token, "Tentocle", 8) == 0)
            m_pClientList[iClientH]->m_sType = 80;

        //else if (memcmp(token, "Abaddon", 7) == 0)
        //	m_pClientList[iClientH]->m_sType = 81;

        else if (memcmp(token, "Sor", 3) == 0)
            m_pClientList[iClientH]->m_sType = 82;

        else if (memcmp(token, "ATK", 3) == 0)
            m_pClientList[iClientH]->m_sType = 83;

        else if (memcmp(token, "Elf", 3) == 0)
            m_pClientList[iClientH]->m_sType = 84;

        else if (memcmp(token, "DSK", 3) == 0)
            m_pClientList[iClientH]->m_sType = 85;

        else if (memcmp(token, "HBT", 3) == 0)
            m_pClientList[iClientH]->m_sType = 86;

        else if (memcmp(token, "CT", 2) == 0)
            m_pClientList[iClientH]->m_sType = 87;

        else if (memcmp(token, "Bar", 3) == 0)
            m_pClientList[iClientH]->m_sType = 88;

        else if (memcmp(token, "AGC", 3) == 0)
            m_pClientList[iClientH]->m_sType = 89;

        else if (memcmp(token, "Gail", 4) == 0)
            m_pClientList[iClientH]->m_sType = 90;

        else if (memcmp(token, "gate", 4) == 0)
            m_pClientList[iClientH]->m_sType = 91;
    }
    SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
    delete pStrTok;
}

void CGame::AdminOrder_SetInvi(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetInvis)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        // Ã€ÃŒ Â°ÂªÃ€ÃŒ Â¹Â®Ã€Ãš '1'Ã€ÃŒÂ¸Ã© Ã…ÃµÂ¸Ã­Ã€Â¸Â·ÃŽ Â¼Â¼Ã†Â®. '0'Ã€ÃŒÂ¸Ã© Ã‡Ã˜ÃÂ¦ 
        if (token[0] == '1') SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        else
            if (token[0] == '0') SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
    }

    delete pStrTok;
}

void CGame::AdminOrder_SetZerk(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetZerk)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        // Ã€ÃŒ Â°ÂªÃ€ÃŒ Â¹Â®Ã€Ãš '1'Ã€ÃŒÂ¸Ã© Ã…ÃµÂ¸Ã­Ã€Â¸Â·ÃŽ Â¼Â¼Ã†Â®. '0'Ã€ÃŒÂ¸Ã© Ã‡Ã˜ÃÂ¦ 


        if (token[0] == '1')
        {
            SetBerserkFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
            m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = (char)m_pMagicConfigList[32]->m_sValue4;
        }
        else
            if (token[0] == '0') SetBerserkFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
    }

    delete pStrTok;
}

void CGame::AdminOrder_SetFreeze(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetIce)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        // Ã€ÃŒ Â°ÂªÃ€ÃŒ Â¹Â®Ã€Ãš '1'Ã€ÃŒÂ¸Ã© Ã…ÃµÂ¸Ã­Ã€Â¸Â·ÃŽ Â¼Â¼Ã†Â®. '0'Ã€ÃŒÂ¸Ã© Ã‡Ã˜ÃÂ¦ 
        if (token[0] == '1') SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        else
            if (token[0] == '0') SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
    }

    delete pStrTok;
}

void CGame::AdminOrder_GetNpcStatus(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cNpcName[256];
    CStrTok * pStrTok;
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGetNpcStatus)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);

    token = pStrTok->pGet();
    token = pStrTok->pGet();
    if (token != NULL)
    {
        ZeroMemory(cNpcName, sizeof(cNpcName));
        strcpy(cNpcName, token);
    }

    for (i = 0; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != NULL)
        {
            if (memcmp(m_pNpcList[i]->m_cName, cNpcName, 5) == 0)
            {
                SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINIFO, 1, i, NULL, NULL);
            }
        }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_CreateFish(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cItemName[30];
    CStrTok * pStrTok;
    CItem * pItem;
    int tX, tY, iType;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCreateFish)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    tX = tY = iType = 0;

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token != NULL)
    {
        tX = atoi(token);
    }

    token = pStrTok->pGet();
    if (token != NULL)
    {
        tY = atoi(token);
    }

    token = pStrTok->pGet();
    if (token != NULL)
    {
        iType = atoi(token);
    }

    if ((tX != 0) && (tY != 0) && (iType != 0))
    {
        // Ã€ÃŒ ÃÃ‚Ã‡Â¥Â¿Â¡ Â¹Â°Â°Ã­Â±Ã¢Â¸Â¦ Â¸Â¸ÂµÃ§Â´Ã™.
        pItem = new CItem;
        if (pItem == NULL)
        {
            delete pStrTok;
            return;
        }
        ZeroMemory(cItemName, sizeof(cItemName));
        strcpy(cItemName, "Â¹Â°Â°Ã­Â±Ã¢");
        if (_bInitItemAttr(pItem, cItemName) == TRUE)
        {
            iCreateFish(m_pClientList[iClientH]->m_cMapIndex, tX, tY, iType, pItem, 1, 60000 * 20);
        }
        else delete pItem;
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_Teleport(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cMapName[256];
    CStrTok * pStrTok;
    int dX, dY;
    BOOL   bFlag;

    dX = dY = -1;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelTeleport)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token != NULL)
    {
        ZeroMemory(cMapName, sizeof(cMapName));
        strcpy(cMapName, token);
    }

    token = pStrTok->pGet();
    if (token != NULL)
    {
        dX = atoi(token);
    }

    token = pStrTok->pGet();
    if (token != NULL)
    {
        dY = atoi(token);
    }

    bFlag = FALSE;
    if (strcmp("2ndmiddle", cMapName) == 0) bFlag = TRUE;
    if (strcmp("abaddon", cMapName) == 0) bFlag = TRUE;
    if (strcmp("arebrk11", cMapName) == 0) bFlag = TRUE;
    if (strcmp("arebrk12", cMapName) == 0) bFlag = TRUE;
    if (strcmp("arebrk21", cMapName) == 0) bFlag = TRUE;
    if (strcmp("arebrk22", cMapName) == 0) bFlag = TRUE;
    if (strcmp("arefarm", cMapName) == 0) bFlag = TRUE;
    if (strcmp("arejail", cMapName) == 0) bFlag = TRUE;
    if (strcmp("aremidl", cMapName) == 0) bFlag = TRUE;
    if (strcmp("aremidr", cMapName) == 0) bFlag = TRUE;
    if (strcmp("aresden", cMapName) == 0) bFlag = TRUE;
    if (strcmp("aresdend1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("areuni", cMapName) == 0) bFlag = TRUE;
    if (strcmp("arewrhus", cMapName) == 0) bFlag = TRUE;
    if (strcmp("bisle", cMapName) == 0)   bFlag = TRUE;
    if (strcmp("bsmith_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("bsmith_1f", cMapName) == 0) bFlag = TRUE;
    if (strcmp("bsmith_2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("bsmith_2f", cMapName) == 0) bFlag = TRUE;
    if (strcmp("BtField", cMapName) == 0) bFlag = TRUE;
    if (strcmp("cath_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("cath_2", cMapName) == 0)      bFlag = TRUE;
    if (strcmp("cityhall_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("cityhall_2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("CmdHall_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("CmdHall_2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("default", cMapName) == 0) bFlag = TRUE;
    if (strcmp("dglv2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("dglv3", cMapName) == 0) bFlag = TRUE;
    if (strcmp("dglv4", cMapName) == 0) bFlag = TRUE;
    if (strcmp("druncncity", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvbrk11", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvbrk12", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvbrk21", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvbrk22", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvfarm", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvine", cMapName) == 0)  bFlag = TRUE;
    if (strcmp("elvined1", cMapName) == 0)    bFlag = TRUE;
    if (strcmp("elvjail", cMapName) == 0)    bFlag = TRUE;
    if (strcmp("elvmidl", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvmidr", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvuni", cMapName) == 0) bFlag = TRUE;
    if (strcmp("elvwrhus", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone3", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone4", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone5", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone6", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone7", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone8", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone9", cMapName) == 0) bFlag = TRUE;
    if (strcmp("fightzone10", cMapName) == 0) bFlag = TRUE;
    if (strcmp("gldhall_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("gldhall_2", cMapName) == 0)   bFlag = TRUE;
    if (strcmp("GodH", cMapName) == 0)   bFlag = TRUE;
    if (strcmp("gshop_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("gshop_1f", cMapName) == 0) bFlag = TRUE;
    if (strcmp("gshop_2", cMapName) == 0)     bFlag = TRUE;
    if (strcmp("gshop_2f", cMapName) == 0)     bFlag = TRUE;
    if (strcmp("HRampart", cMapName) == 0)     bFlag = TRUE;
    if (strcmp("huntzone1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("huntzone2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("huntzone3", cMapName) == 0) bFlag = TRUE;
    if (strcmp("huntzone4", cMapName) == 0) bFlag = TRUE;
    if (strcmp("icebound", cMapName) == 0) bFlag = TRUE;
    if (strcmp("inferniaA", cMapName) == 0) bFlag = TRUE;
    if (strcmp("inferniaB", cMapName) == 0) bFlag = TRUE;
    if (strcmp("maze", cMapName) == 0) bFlag = TRUE;
    if (strcmp("middled1n", cMapName) == 0) bFlag = TRUE;
    if (strcmp("middled1x", cMapName) == 0) bFlag = TRUE;
    if (strcmp("middleland", cMapName) == 0) bFlag = TRUE;
    if (strcmp("penalty", cMapName) == 0) bFlag = TRUE;
    if (strcmp("procella", cMapName) == 0) bFlag = TRUE;
    if (strcmp("resurr1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("resurr2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("toh1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("toh2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("toh3", cMapName) == 0) bFlag = TRUE;
    if (strcmp("wrhus_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("wrhus_1f", cMapName) == 0) bFlag = TRUE;
    if (strcmp("wrhus_2", cMapName) == 0)     bFlag = TRUE;
    if (strcmp("wrhus_2f", cMapName) == 0)     bFlag = TRUE;
    if (strcmp("wzdtwr_1", cMapName) == 0) bFlag = TRUE;
    if (strcmp("wzdtwr_2", cMapName) == 0) bFlag = TRUE;
    if (strcmp("Test", cMapName) == 0) bFlag = TRUE;
    if (strcmp("GMMap", cMapName) == 0) bFlag = TRUE;
    if (strcmp("dv", cMapName) == 0) bFlag = TRUE;
    if (strcmp("HBX", cMapName) == 0) bFlag = TRUE;

    if (bFlag == TRUE)
        RequestTeleportHandler(iClientH, "2   ", cMapName, dX, dY);

    delete pStrTok;
    return;
}

void CGame::AdminOrder_Time(int iClientH, char * pData, DWORD dwMsgSize)
{
    //Time command By ACiDx Last edit on Sept,06,04
    BOOL m_bManualTime;
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;
    int i;

    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelTime)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        if (token[0] == '2')
        {
            m_cDayOrNight = 2;
            wsprintf(G_cTxt, "(%s) Admin(%s) Order: Forces night mode", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);
            m_bManualTime = TRUE;
        }
        else if (token[0] == '1')
        {
            m_cDayOrNight = 1;
            wsprintf(G_cTxt, "(%s) Admin(%s) Order: Forces day mode", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            m_bManualTime = TRUE;
            //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);
        }
        else if (token[0] == '0')
        {
            wsprintf(G_cTxt, "(%s) Admin(%s) Order: Disables force time", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
            m_bManualTime = FALSE;
            //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);
        }
        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE))
            {
                if ((m_pClientList[i]->m_cMapIndex >= 0) &&
                    (m_pMapList[m_pClientList[i]->m_cMapIndex] != NULL) &&
                    (m_pMapList[m_pClientList[i]->m_cMapIndex]->m_bIsFixedDayMode == FALSE))
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_TIMECHANGE, m_cDayOrNight, NULL, NULL, NULL);
            }
    }

    delete pStrTok;
}

void CGame::AdminOrder_CheckRep(int iClientH, char * pData, DWORD dwMsgSize)
{
    CStrTok * pStrTok;
    short m_iRating;
    char * token, cName[11], cTargetName[11], cRepMessage[256], cTemp[256], seps[] = "= \t\n", cBuff[256];
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    ZeroMemory(cTemp, sizeof(cTemp));
    ZeroMemory(cRepMessage, sizeof(cRepMessage));
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 1)
    {
        wsprintf(cRepMessage, " You have %d reputation points.", m_pClientList[iClientH]->m_iRating);
        ShowClientMsg(iClientH, cRepMessage);
    }
    else
    {
        if ((dwMsgSize) <= 0) return;
        ZeroMemory(cTargetName, sizeof(cTargetName));
        ZeroMemory(cBuff, sizeof(cBuff));
        memcpy(cBuff, pData, dwMsgSize);

        pStrTok = new CStrTok(cBuff, seps);
        token = pStrTok->pGet();

        token = pStrTok->pGet();

        if (token != NULL)
        {
            ZeroMemory(cName, sizeof(cName));
            strcpy(cName, token);
        }
        else
        {
            ZeroMemory(cName, sizeof(cName));
            strcpy(cName, "null");
        }

        token = pStrTok->pGet();

        if (token != NULL)
        {
            m_iRating = atoi(token);
        }

        if (token == NULL)
        {
            token = "null";
        }

        if (cName != NULL)
        {
            token = cName;

            if (strlen(token) > 10)
                memcpy(cTargetName, token, 10);
            else memcpy(cTargetName, token, strlen(token));

            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
                {
                    wsprintf(cRepMessage, " %s has %d reputation points.", m_pClientList[i]->m_cCharName, m_pClientList[i]->m_iRating);
                    ShowClientMsg(iClientH, cRepMessage);
                }
        }
    }
}

void CGame::AdminOrder_Pushplayer(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cMapName[256], cName[11], cTargetName[11];
    CStrTok * pStrTok;
    int dX, dY;
    int i;
    BOOL   bFlag;
    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cBuff, sizeof(cBuff));

    dX = dY = -1;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelPushPlayer)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();


    if (token != NULL)
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, token);
    }
    else
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, "null");
    }
    token = pStrTok->pGet();
    if (token != NULL)
    {
        ZeroMemory(cMapName, sizeof(cMapName));
        strcpy(cMapName, token);
    }

    token = pStrTok->pGet();
    if (token != NULL)
    {
        dX = atoi(token);
    }

    token = pStrTok->pGet();
    if (token != NULL)
    {
        dY = atoi(token);
    }

    if (token == NULL) { token = "null"; }
    if (cName != NULL)
    {
        token = cName;

        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
            {
                bFlag = FALSE;
                //Asks For Flag on maps
                if (strcmp("2ndmiddle", cMapName) == 0) bFlag = TRUE;
                if (strcmp("abaddon", cMapName) == 0) bFlag = TRUE;
                if (strcmp("arebrk11", cMapName) == 0) bFlag = TRUE;
                if (strcmp("arebrk12", cMapName) == 0) bFlag = TRUE;
                if (strcmp("arebrk21", cMapName) == 0) bFlag = TRUE;
                if (strcmp("arebrk22", cMapName) == 0) bFlag = TRUE;
                if (strcmp("arefarm", cMapName) == 0) bFlag = TRUE;
                if (strcmp("arejail", cMapName) == 0) bFlag = TRUE;
                if (strcmp("aremidl", cMapName) == 0) bFlag = TRUE;
                if (strcmp("aremidr", cMapName) == 0) bFlag = TRUE;
                if (strcmp("aresden", cMapName) == 0) bFlag = TRUE;
                if (strcmp("aresdend1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("areuni", cMapName) == 0) bFlag = TRUE;
                if (strcmp("arewrhus", cMapName) == 0) bFlag = TRUE;
                if (strcmp("bisle", cMapName) == 0)   bFlag = TRUE;
                if (strcmp("bsmith_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("bsmith_1f", cMapName) == 0) bFlag = TRUE;
                if (strcmp("bsmith_2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("bsmith_2f", cMapName) == 0) bFlag = TRUE;
                if (strcmp("BtField", cMapName) == 0) bFlag = TRUE;
                if (strcmp("cath_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("cath_2", cMapName) == 0)      bFlag = TRUE;
                if (strcmp("cityhall_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("cityhall_2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("CmdHall_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("CmdHall_2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("default", cMapName) == 0) bFlag = TRUE;
                if (strcmp("dglv2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("dglv3", cMapName) == 0) bFlag = TRUE;
                if (strcmp("dglv4", cMapName) == 0) bFlag = TRUE;
                if (strcmp("druncncity", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvbrk11", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvbrk12", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvbrk21", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvbrk22", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvfarm", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvine", cMapName) == 0)  bFlag = TRUE;
                if (strcmp("elvined1", cMapName) == 0)    bFlag = TRUE;
                if (strcmp("elvjail", cMapName) == 0)    bFlag = TRUE;
                if (strcmp("elvmidl", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvmidr", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvuni", cMapName) == 0) bFlag = TRUE;
                if (strcmp("elvwrhus", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone3", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone4", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone5", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone6", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone7", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone8", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone9", cMapName) == 0) bFlag = TRUE;
                if (strcmp("fightzone10", cMapName) == 0) bFlag = TRUE;
                if (strcmp("gldhall_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("gldhall_2", cMapName) == 0)   bFlag = TRUE;
                if (strcmp("GodH", cMapName) == 0)   bFlag = TRUE;
                if (strcmp("gshop_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("gshop_1f", cMapName) == 0) bFlag = TRUE;
                if (strcmp("gshop_2", cMapName) == 0)     bFlag = TRUE;
                if (strcmp("gshop_2f", cMapName) == 0)     bFlag = TRUE;
                if (strcmp("HRampart", cMapName) == 0)     bFlag = TRUE;
                if (strcmp("huntzone1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("huntzone2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("huntzone3", cMapName) == 0) bFlag = TRUE;
                if (strcmp("huntzone4", cMapName) == 0) bFlag = TRUE;
                if (strcmp("icebound", cMapName) == 0) bFlag = TRUE;
                if (strcmp("inferniaA", cMapName) == 0) bFlag = TRUE;
                if (strcmp("inferniaB", cMapName) == 0) bFlag = TRUE;
                if (strcmp("maze", cMapName) == 0) bFlag = TRUE;
                if (strcmp("middled1n", cMapName) == 0) bFlag = TRUE;
                if (strcmp("middled1x", cMapName) == 0) bFlag = TRUE;
                if (strcmp("middleland", cMapName) == 0) bFlag = TRUE;
                if (strcmp("penalty", cMapName) == 0) bFlag = TRUE;
                if (strcmp("procella", cMapName) == 0) bFlag = TRUE;
                if (strcmp("resurr1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("resurr2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("toh1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("toh2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("toh3", cMapName) == 0) bFlag = TRUE;
                if (strcmp("wrhus_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("wrhus_1f", cMapName) == 0) bFlag = TRUE;
                if (strcmp("wrhus_2", cMapName) == 0)     bFlag = TRUE;
                if (strcmp("wrhus_2f", cMapName) == 0)     bFlag = TRUE;
                if (strcmp("wzdtwr_1", cMapName) == 0) bFlag = TRUE;
                if (strcmp("wzdtwr_2", cMapName) == 0) bFlag = TRUE;
                if (strcmp("Test", cMapName) == 0) bFlag = TRUE;
                if (strcmp("GMMap", cMapName) == 0) bFlag = TRUE;

                //Defines I as Max clients
                if (bFlag == TRUE)
                    //Reqeust the Push/Teleport
                    RequestTeleportHandler(i, "2   ", cMapName, dX, dY);
                wsprintf(G_cTxt, "(%s) GM(%s) sends (%s) Player(%s) to [%s](%d,%d)", m_pClientList[iClientH]->m_cIPaddress,
                    m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cIPaddress, m_pClientList[i]->m_cCharName, cMapName, dX, dY);
                //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);
                delete pStrTok;
                return;
            }
    }
}
// SUMMONGUILD Deleted
/*
void CGame::AdminOrder_SummonGuild(int iClientH, char *pData, DWORD dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, * cp, cBuff[256], cMapName[11], char cGuildName[20];
 WORD   *wp;
 int    pX, pY, i;
 CStrTok * pStrTok;
 DWORD  dwGoldCount;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonGuild) {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
        // dwGoldCount = player gold
        dwGoldCount = dwGetItemCount(iClientH, "Gold");
        // if summonguildcost is greater than player gold return
        if (m_iSummonGuildCost > dwGoldCount) {
            return;
        }
        // if summonguildcost is less than player gold
        else {
            // reduce gold by summonguildcost
            SetItemCount(iClientH, "Gold", dwGoldCount - m_iSummonGuildCost);
        }
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new  CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == NULL) {
        delete pStrTok;
        return;
    }

    pX = m_pClientList[iClientH]->m_sX;
    pY = m_pClientList[iClientH]->m_sY;
    //memcpy(cGuildName, m_pClientList[iClientH]->m_cGuildName, 20);
    memcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 11);

    if (strlen(token) > 20)
            memcpy(cGuildName, token, 20);
        else memcpy(cGuildName, token, strlen(token));

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cGuildName, token) == 0)) {
            RequestTeleportHandler(i, "2   ", cMapName, pX, pY);
    }//m_pClientList[i]->m_cCharName

    wsprintf(G_cTxt,"GM Order(%s): PC(%s) Summoned to (%s)", m_pClientList[iClientH]->m_cGuildName, cGuildName, cMapName);
    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);

    ZeroMemory(cBuff, sizeof(cBuff));
    cp = (char *)cBuff;
    *cp = GSM_REQUEST_SUMMONGUILD;
    cp++;

    memcpy(cp, cGuildName, 20);
    cp += 20;

    memcpy(cp, cMapName, 10);
    cp += 10;

    wp = (WORD *)cp;
    *wp = m_pClientList[iClientH]->m_sX;
    cp += 2;

    wp = (WORD *)cp;
    *wp = m_pClientList[iClientH]->m_sY;
    cp += 2;

    bStockMsgToGateServer(cBuff, 25);

    delete pStrTok;
}
*/

void CGame::AdminOrder_SummonGuild(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * cBuff[256], cLocation[11];
    int    pX, pY, i;
    // CStrTok * pStrTok;
    char buff[100];
    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    //if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
     // Admin user level+a+t T-T¦T¬+ŠT-T¦ +a+t T-+?T++U+aTC TC+×TC+c+š+n T-+T T¬+¬T++Í.
    //  SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
    // return;
    //}

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pX = m_pClientList[iClientH]->m_sX;
    pY = m_pClientList[iClientH]->m_sY;
    memcpy(cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 11);
    wsprintf(buff, "Summon-Guild: %s (%s) %s %d %d", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cGuildName, cLocation, pX, pY);
    log->info(buff);
    for (i = 0; i < DEF_MAXCLIENTS; i++)

        if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName) == 0) && (i != iClientH))
        {
            RequestTeleportHandler(i, "2   ", cLocation, pX, pY);
        }//m_pClientList[i]->m_cCharName
       // delete pStrTok;
}

void CGame::AdminOrder_GetFightzoneTicket(int iClientH)
{
    int iReserveTime, iFightzoneTN, iFightzoneN;
    char cTemp[30];
    SYSTEMTIME SysTime;

    if (m_pClientList[iClientH] == NULL) return;
    if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "fightzone", 9) == 0)
    {

        iReserveTime = m_pClientList[iClientH]->m_iReserveTime;
        GetLocalTime(&SysTime);
        m_pClientList[iClientH]->m_iReserveTime = SysTime.wMonth * 10000 + SysTime.wDay * 100 + (SysTime.wHour + 3);

        ZeroMemory(cTemp, sizeof(cTemp));
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

        ZeroMemory(cTemp, sizeof(cTemp));
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

void CGame::AdminOrder_SetAttackMode(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;
    int    i;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetAttackMode)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();


    if (token != NULL)
    {
        if (token[0] == '1')
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled = TRUE;
            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE) && (m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex))
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_GLOBALATTACKMODE, (int)m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled, NULL, NULL, NULL);
        }
        else if (token[0] == '0')
        {
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled = FALSE;

            for (i = 1; i < DEF_MAXCLIENTS; i++)
                if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE) && (m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex))
                    SendNotifyMsg(NULL, i, DEF_NOTIFY_GLOBALATTACKMODE, (int)m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled, NULL, NULL, NULL);
        }
    }

    delete pStrTok;
}

void CGame::AdminOrder_UnsummonAll(int iClientH)
{
    int i;

    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelUnsummonAll)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    for (i = 1; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != NULL)
        {
            if ((m_pNpcList[i]->m_bIsSummoned == TRUE) && (m_pNpcList[i]->m_bIsKilled == FALSE))
                NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
        }
}

void CGame::AdminOrder_UnsummonDemon(int iClientH)
{
    int i;

    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelUnsummonDemon)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    for (i = 1; i < DEF_MAXNPCS; i++)
        if (m_pNpcList[i] != NULL)
        {
            if ((m_pNpcList[i]->m_sType == 31) && (m_pNpcList[i]->m_bIsKilled == FALSE))
                NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
        }
}

void CGame::AdminOrder_Summon(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cName_Master[10], cName_Slave[10], cNpcName[256], cWaypoint[11], cSA;
    int    pX, pY, j, iNum, iNamingValue;
    CStrTok * pStrTok;
    BOOL   bMaster;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == FALSE) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummon)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    ZeroMemory(cNpcName, sizeof(cNpcName));
    strcpy(cNpcName, token);

    token = pStrTok->pGet();

    if (token != NULL)
        iNum = atoi(token);
    else iNum = 1;

    if (iNum <= 0)  iNum = 1;
    if (iNum >= 50)  iNum = 50;

    cSA = 0;
    pX = m_pClientList[iClientH]->m_sX;
    pY = m_pClientList[iClientH]->m_sY;

    // Â¸ÃžÂ½ÃƒÃÃ¶ ÃƒÃ¢Â·Ã‚ 
    wsprintf(G_cTxt, "(!) Admin Order: Summon(%s)-(%d)", cNpcName, iNum);
    log->info(G_cTxt);

    iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
    if (iNamingValue != -1)
    {
        // Master MobÃ€Â» Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™.
        ZeroMemory(cName_Master, sizeof(cName_Master));
        wsprintf(cName_Master, "XX%d", iNamingValue);
        cName_Master[0] = '_';
        cName_Master[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

        if ((bMaster = bCreateNewNpc(cNpcName, cName_Master, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, FALSE, TRUE)) == FALSE)
        {
            // Â½Ã‡Ã†ÃÃ‡ÃŸÃ€Â¸Â¹Ã‡Â·ÃŽ Â¿Â¹Â¾Ã ÂµÃˆ NameValueÂ¸Â¦ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™.
            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
        }
    }

    for (j = 0; j < (iNum - 1); j++)
    {
        iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
        if (iNamingValue != -1)
        {
            // Slave MobÂµÃ©Ã€Â» Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™.
            ZeroMemory(cName_Slave, sizeof(cName_Slave));
            wsprintf(cName_Slave, "XX%d", iNamingValue);
            cName_Slave[0] = '_';
            cName_Slave[1] = m_pClientList[iClientH]->m_cMapIndex + 65;

            // v1.411 Â¸Ã³Â½ÂºÃ…ÃÃ€Ã‡ Ã†Â¯Â¼Ã¶ Ã†Â¯Â¼ÂºÃ„Â¡Â¸Â¦ Ã€Ã”Â·Ã‚Ã‡Ã‘Â´Ã™. 

            if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, FALSE) == FALSE)
            {
                // Â½Ã‡Ã†ÃÃ‡ÃŸÃ€Â¸Â¹Ã‡Â·ÃŽ Â¿Â¹Â¾Ã ÂµÃˆ NameValueÂ¸Â¦ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™.
                m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
            }
            else
            {
                // SlaveÂ¸Ã°ÂµÃ¥Â·ÃŽ Ã€Ã¼ÃˆÂ¯.
                bSetNpcFollowMode(cName_Slave, cName_Master, DEF_OWNERTYPE_NPC);
            }
        }
    }

    delete pStrTok;
}

void CGame::AdminOrder_SummonAll(int iClientH, char * pData, DWORD dwMsgSize)
{

    char   seps[] = "= \t\n";
    char * token, * cp, cBuff[256], cLocation[11], cMapName[11];
    WORD * wp;
    int    pX, pY, i;
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == FALSE) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonAll)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    pX = m_pClientList[iClientH]->m_sX;
    pY = m_pClientList[iClientH]->m_sY;


    memcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 11);

    if (strlen(token) > 10)
        memcpy(cLocation, token, 10);
    else memcpy(cLocation, token, strlen(token));

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cLocation, token) == 0))
        {
            RequestTeleportHandler(i, "2   ", cMapName, pX, pY);
        }

    wsprintf(G_cTxt, "GM Order(%s): PC(%s) Summoned to (%s)", m_pClientList[iClientH]->m_cLocation,
        cLocation, cMapName);
    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);

    ZeroMemory(cBuff, sizeof(cBuff));
    cp = (char *)cBuff;
    *cp = GSM_REQUEST_SUMMONALL;
    cp++;

    memcpy(cp, cLocation, 10);
    cp += 10;

    memcpy(cp, cMapName, 10);
    cp += 10;

    wp = (WORD *)cp;
    *wp = m_pClientList[iClientH]->m_sX;
    cp += 2;

    wp = (WORD *)cp;
    *wp = m_pClientList[iClientH]->m_sY;
    cp += 2;

    bStockMsgToGateServer(cBuff, 25);

    delete pStrTok;
}

void CGame::AdminOrder_SummonPlayer(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, * cp, cBuff[256], cMapName[11], cName[11];
    WORD * wp;
    int    pX, pY, i;
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonPlayer)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    ZeroMemory(cName, sizeof(cName));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    pX = m_pClientList[iClientH]->m_sX;
    pY = m_pClientList[iClientH]->m_sY;
    memcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 11);

    if (strlen(token) > 10)
        memcpy(cName, token, 10);
    else memcpy(cName, token, strlen(token));

    for (i = 0; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0))
        {
            if (i == iClientH)
            {
                delete pStrTok;
                return;
            }
            RequestTeleportHandler(i, "2   ", cMapName, pX, pY);
            delete pStrTok;
            return;
        }//m_pClientList[i]->m_cCharName

    // find char on other hg's
    wsprintf(G_cTxt, "GM Order(%s): PC(%s) Summoned to (%s)", m_pClientList[iClientH]->m_cCharName,
        cName, cMapName);
    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, G_cTxt);

    ZeroMemory(cBuff, sizeof(cBuff));
    cp = (char *)cBuff;
    *cp = GSM_REQUEST_SUMMONPLAYER;
    cp++;

    memcpy(cp, cName, 10);
    cp += 10;

    memcpy(cp, cMapName, 10);
    cp += 10;

    wp = (WORD *)cp;
    *wp = m_pClientList[iClientH]->m_sX;
    cp += 2;

    wp = (WORD *)cp;
    *wp = m_pClientList[iClientH]->m_sY;
    cp += 2;

    bStockMsgToGateServer(cBuff, 25);

    delete pStrTok;
}

void CGame::AdminOrder_DisconnectAll(int iClientH, char * pData, DWORD dwMsgSize)
{

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == FALSE) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelDisconnectAll)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    _iForcePlayerDisconect(DEF_MAXCLIENTS);
}

void CGame::AdminOrder_SetObserverMode(int iClientH)
{

    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_bIsObserverMode == TRUE)
    {
        // Â»Ãµ Ã€Â§Ã„Â¡Â¿Â¡ Ã‡Â¥Â½ÃƒÃ‡Ã‘Â´Ã™. 
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, NULL, NULL, NULL);
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_OBSERVERMODE, 0, NULL, NULL, NULL);
        m_pClientList[iClientH]->m_bIsObserverMode = FALSE;
    }
    else
    {
        // Ã‡Ã¶Ã€Ã§ Ã€Â§Ã„Â¡Â¿Â¡Â¼Â­ ÃÃ¶Â¿Ã®Â´Ã™. Â¾Ã¸Â¾Ã®ÃÃ¸ Ã‡Ã Ã€Â§Â¸Â¦ Â¾Ã‹Â·ÃÃÃ Â¾ÃŸ Ã‡Ã‘Â´Ã™. Â¾Ã†ÃÃ· Â¹ÃŒÂ±Â¸Ã‡Ã¶
        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(1, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
        SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, NULL, NULL, NULL);
        //iRequestPanningMapDataRequest(iClientH, pData)
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_OBSERVERMODE, 1, NULL, NULL, NULL);
        m_pClientList[iClientH]->m_bIsObserverMode = TRUE;
    }
}

void CGame::AdminOrder_EnableAdminCommand(int iClientH, char * pData, DWORD dwMsgSize)
{
    char * token, cBuff[256];
    std::size_t len;
    char   seps[] = "= \t\n";
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);
    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {
        len = strlen(token);
        if (len > 10) len = 10;
        if (memcmp(token, m_cSecurityNumber, len) == 0)
        {
            m_pClientList[iClientH]->m_bIsAdminCommandEnabled = TRUE;
        }
        else
        {
            try
            {
                wsprintf(G_cTxt, "(%s) Player(%s) attempts to access /enableadmincommand with %s", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, token);
                log->info(G_cTxt);
            }
            catch (...)
            {
            }
            return;
        }
    }
}

void CGame::AdminOrder_CreateItem(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * cp, * token, cBuff[256], cItemName[256], cData[256], cTemp[256], cAttribute[256], cValue[256];
    SYSTEMTIME SysTime;
    CStrTok * pStrTok;
    CItem * pItem;
    short * sp;
    int    iRet, iTemp, iEraseReq, iValue;
    DWORD * dwp;
    WORD * wp, wTemp;
    double dV1, dV2, dV3;

    // if the client doesnt exist than no effect.
    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == FALSE) return;

    // if the command is just "/createitem " than no effect.
    if ((dwMsgSize) <= 0) return;

    // if client's admin level is less than 4 no effect.
    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCreateItem)
    {
        // if the admin user level is not 0 and less than 4 send message 
        if (m_pClientList[iClientH]->m_iAdminUserLevel != 0)
            SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    } // close if (m_pClientList[iClientH]->m_iAdminUserLevel < 4) {

    // IP address not able to create item 
    // DISABLED: if (memcmp(m_pClientList[iClientH]->m_cIPaddress, "192.168.1.", 12) != 0) return;

    // check if the client has typed "/enableadmincreateitem"
    //if (m_pClientList[iClientH]->m_bIsAdminCreateItemEnabled == FALSE) return;

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);
    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    // if the token is something it is an item name
    if (token != NULL)
    {
        ZeroMemory(cItemName, sizeof(cItemName));
        strcpy(cItemName, token);
    } // close if (token != NULL) {

    // if the 2nd token is true it is the item attribute
    ZeroMemory(cAttribute, sizeof(cAttribute));
    token = pStrTok->pGet();
    if (token != NULL)
    {
        ZeroMemory(cAttribute, sizeof(cAttribute));
        strcpy(cAttribute, token);
    } // close if (token != NULL) {

    // if the 3rd token is true it is the item value
    ZeroMemory(cValue, sizeof(cValue));
    iValue = 0;
    token = pStrTok->pGet();
    if (token != NULL)
    {
        ZeroMemory(cValue, sizeof(cValue));
        strcpy(cValue, token);
        iValue = atoi(cValue);
    } // close if (token != NULL) {

    pItem = new CItem;
    // if the given itemname doesnt exist delete item
    if (_bInitItemAttr(pItem, cItemName) == FALSE)
    {
        delete pItem;
        return;
    } //close if (_bInitItemAttr(pItem, cItemName) == FALSE) {

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
                } // close if (wTemp <= pItem->m_wMaxLifeSpan*2) {
                else pItem->m_sItemSpecEffectValue1 = (short)pItem->m_wMaxLifeSpan;
            } // close if ((iValue >= 1) && (iValue <= 200)) {
            else pItem->m_dwAttribute = NULL;
        } // close if (pItem->m_dwAttribute == 1) {
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
                } // close switch ((pItem->m_dwAttribute & 0xF00000) >> 20) {
            } // close if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK) {
            else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE)
            {
                switch ((pItem->m_dwAttribute & 0xF00000) >> 20)
                {
                    case 10: pItem->m_cItemColor = 5; break;
                } // close switch ((pItem->m_dwAttribute & 0xF00000) >> 20) {
            } // close else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE) {
        } // close else {
    } // close if (strlen(cAttribute) != 0) {

    // if the item id is
    switch (pItem->m_sIDnum)
    {
        case 511: // ArenaTicket
        case 513: // ArenaTicket(2)
        case 515: // ArenaTicket(3)
        case 517: // ArenaTicket(4)
        case 530: // ArenaTicket(5)
        case 531: // ArenaTicket(6)
        case 532: // ArenaTicket(7)
        case 533: // ArenaTicket(8)
        case 534: // ArenaTicket(9)
            GetLocalTime(&SysTime);
            pItem->m_sTouchEffectType = DEF_ITET_DATE;
            // v1.4311-3 º¯°æ ¿î¿µÀÚ°¡ ¹ß±ÞÇÑ ÀÔÀå±ÇÀº ±×³¯Àº Ç×»ó ÀÔÀå °¡´É ..
            pItem->m_sTouchEffectValue1 = (short)SysTime.wMonth;
            pItem->m_sTouchEffectValue2 = (short)SysTime.wDay;
            pItem->m_sTouchEffectValue3 = 24;
            break;

        default:
            GetLocalTime(&SysTime);
            pItem->m_sTouchEffectType = DEF_ITET_ID;
            pItem->m_sTouchEffectValue1 = iDice(1, 100000);
            pItem->m_sTouchEffectValue2 = iDice(1, 100000);
            // ¸¶Áö¸· ¼ýÀÚ´Â ¾ÆÀÌÅÛ »ý¼º ¿ù, ÀÏ	
            ZeroMemory(cTemp, sizeof(cTemp));
            wsprintf(cTemp, "%d%2d", (short)SysTime.wMonth, (short)SysTime.wDay);
            pItem->m_sTouchEffectValue3 = atoi(cTemp);
            break;
    }

    ZeroMemory(cData, sizeof(cData));
    if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == TRUE)
    {
        // ¾ÆÀÌÅÛÀ» È¹µæÇß´Ù.
        dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
        *dwp = MSGID_NOTIFY;
        wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
        *wp = DEF_NOTIFY_ITEMOBTAINED;

        cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

        // 1°³ È¹µæÇß´Ù. Amount°¡ ¾Æ´Ï´Ù!
        *cp = 1;
        cp++;

        memcpy(cp, pItem->m_cName, 20);
        cp += 20;

        dwp = (DWORD *)cp;
        *dwp = pItem->m_dwCount;	// ¼ö·®À» ÀÔ·Â 
        cp += 4;

        *cp = pItem->m_cItemType;
        cp++;

        *cp = pItem->m_cEquipPos;
        cp++;

        *cp = (char)0; // ¾òÀº ¾ÆÀÌÅÛÀÌ¹Ç·Î ÀåÂøµÇÁö ¾Ê¾Ò´Ù.
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

        // v2.15 ·Î±× °ü·Ã ¼öÁ¤
        if (iEraseReq == 1)
        {
            delete pItem;
            pItem = NULL;
        }

        // ¾ÆÀÌÅÛ Á¤º¸ Àü¼Û 
        iRet = m_pClientList[iClientH]->iSendMsg(cData, 53);


        // v2.14 Admin Log
        wsprintf(G_cTxt, "(%s) GM Order(%s): Create ItemName(%s)", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, cItemName);
        //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, NULL, G_cTxt);

        return;
    }
    else
    {
        // ¾ÆÀÌÅÛÀ» ¼ÒÁöÇÒ ¼ö ¾ø´Â »óÈ²ÀÌ´Ù.		
        delete pItem;
        return;
    }
}

void CGame::AdminOrder_SummonStorm(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cOwnerType;
    int    i;
    CStrTok * pStrTok;
    short  sOwnerH, sType, sX, sY;
    int    dX1, dY1, iWhetherBonus, iResult = 0, tX, tY, iErr, ix, iy;
    int    iStormCount, iMagicAttr;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;
    if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == FALSE) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelStorm)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    //No Storm Specified
    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    sType = atoi(token);

    //Only 100 Magic Spells
    if (sType <= 0)	sType = 1;
    if (sType > 100) sType = 100;

    //No Count Specified
    token = pStrTok->pGet();
    if (token == NULL)
    {
        delete pStrTok;
        return;
    }
    iStormCount = atoi(token);

    if (iStormCount < 0)
        iStormCount = 0;
    if (iStormCount > 30)
        iStormCount = 30;//Original : 30

    try
    {
        //Whether Bouns
        iWhetherBonus = iGetWhetherMagicBonusEffect(sType, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus);

        //Magic Attribute
        iMagicAttr = m_pMagicConfigList[sType]->m_iAttribute;

        //Calculate Area Coords
        //Gives Right
        dX1 = m_pClientList[iClientH]->m_sX;
        dX1 -= 10;
        //Gives Top
        dY1 = m_pClientList[iClientH]->m_sY;
        dY1 -= 7;

        while (iStormCount != 0)
        {
            int randX, randY;
            randX = (rand() % 20) + dX1;
            randY = (rand() % 13) + dY1;

            switch (m_pMagicConfigList[sType]->m_sType)
            {
                case DEF_MAGICTYPE_DAMAGE_SPOT:
                    //Calculate Damage
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, randX, randY);
                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, randX, randY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);
                    }
                    //Show effects
                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, randX, randY, (sType + 100), m_pClientList[iClientH]->m_sType);
                    break;

                case DEF_MAGICTYPE_DAMAGE_LINEAR:
                    // Ã€ÃÃÃ·Â¼Â± Â»Ã³Â¿Â¡ Ã€Ã–Â´Ã‚ Â¸Ã±Ã‡Â¥Â¸Â¦ Â¸Ã°ÂµÃŽ Â°Ã¸Â°ÃÃ‡Ã‘Â´Ã™.
                    sX = m_pClientList[iClientH]->m_sX;
                    sY = m_pClientList[iClientH]->m_sY;

                    for (i = 2; i < 10; i++)
                    {
                        iErr = 0;
                        m_Misc.GetPoint2(sX, sY, randX, randY, &tX, &tY, &iErr, i);

                        // tx, ty
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }

                        // tx-1, ty
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX - 1, tY);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }

                        // tx+1, ty
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX + 1, tY);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }

                        // tx, ty-1
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY - 1);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }

                        // tx, ty+1
                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                        m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX, tY + 1);
                        if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                            (m_pClientList[sOwnerH]->m_iHP > 0))
                        {
                            // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                        }

                        if ((abs(tX - randX) <= 1) && (abs(tY - randY) <= 1)) break;
                    }

                    // ÃÃ–ÂºÂ¯ Â°Ã¸Â°Ã ÃˆÂ¿Â°Ãº 
                    for (iy = randY - m_pMagicConfigList[sType]->m_sValue3; iy <= randY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                        for (ix = randX - m_pMagicConfigList[sType]->m_sValue2; ix <= randX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                        {
                            // Ã€ÃšÂ½Ã…ÂµÂµ Ã‡Ã‡Ã†Ã¸ÂµÃ‰ Â¼Ã¶ Ã€Ã–Ã€Â¸Â´Ã ÃÃ–Ã€Ã‡.
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                                (m_pClientList[sOwnerH]->m_iHP > 0))
                            {
                                // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            }
                        }

                    // dX, dY
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, randX, randY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr); // v1.41 FALSE

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, randX, randY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr); // v1.41 FALSE
                    }
                    //Show effects
                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, randX, randY, (sType + 100), m_pClientList[iClientH]->m_sType);
                    break;

                case DEF_MAGICTYPE_DAMAGE_AREA:
                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, randX, randY);
                    if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                        Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);

                    m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, randX, randY);
                    if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                        (m_pClientList[sOwnerH]->m_iHP > 0))
                    {
                        // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                        if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                            Effect_Damage_Spot(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, m_pMagicConfigList[sType]->m_sValue4, m_pMagicConfigList[sType]->m_sValue5, m_pMagicConfigList[sType]->m_sValue6 + iWhetherBonus, FALSE, iMagicAttr);
                    }

                    // ÃÃ–ÂºÂ¯ Â°Ã¸Â°Ã ÃˆÂ¿Â°Ãº 
                    for (iy = randY - m_pMagicConfigList[sType]->m_sValue3; iy <= randY + m_pMagicConfigList[sType]->m_sValue3; iy++)
                        for (ix = randX - m_pMagicConfigList[sType]->m_sValue2; ix <= randX + m_pMagicConfigList[sType]->m_sValue2; ix++)
                        {
                            // Ã€ÃšÂ½Ã…ÂµÂµ Ã‡Ã‡Ã†Ã¸ÂµÃ‰ Â¼Ã¶ Ã€Ã–Ã€Â¸Â´Ã ÃÃ–Ã€Ã‡.
                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, randX, randY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);

                            m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
                            if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != NULL) &&
                                (m_pClientList[sOwnerH]->m_iHP > 0))
                            {
                                // ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
                                if (bCheckResistingMagicSuccess(m_pClientList[iClientH]->m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
                                    Effect_Damage_Spot_DamageMove(iClientH, DEF_OWNERTYPE_PLAYER, sOwnerH, cOwnerType, randX, randY, m_pMagicConfigList[sType]->m_sValue7, m_pMagicConfigList[sType]->m_sValue8, m_pMagicConfigList[sType]->m_sValue9 + iWhetherBonus, FALSE, iMagicAttr);
                            }
                        }
                    //Show effects
                    SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex,
                        m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, randX, randY, (sType + 100), m_pClientList[iClientH]->m_sType);
                    break;
                default:
                    goto Skip_Storm;
            }
            Sleep(rand() % 10);
            iStormCount--;
        }
    }
    catch (...) {}
    Skip_Storm:
    delete pStrTok;
}

void CGame::AdminOrder_Weather(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256];
    CStrTok * pStrTok;
    char cPrevMode;
    int i, j, sType, sTime;
    DWORD dwTime;

    dwTime = timeGetTime();

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelWeather)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    sType = atoi(token);

    token = pStrTok->pGet();

    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    sTime = atoi(token);

    delete pStrTok;

    for (i = 0; i < DEF_MAXMAPS; i++)
    {
        if ((m_pMapList[i] != NULL) && (m_pMapList[i]->m_bIsFixedDayMode == FALSE))
        {
            cPrevMode = m_pMapList[i]->m_cWhetherStatus;
            if (m_pMapList[i]->m_cWhetherStatus != NULL)
            {
                if ((dwTime - m_pMapList[i]->m_dwWhetherStartTime) > m_pMapList[i]->m_dwWhetherLastTime)
                    m_pMapList[i]->m_cWhetherStatus = NULL;
            }
            else
            {
                sTime *= 1000;
                m_pMapList[i]->m_cWhetherStatus = sType;
                m_pMapList[i]->m_dwWhetherStartTime = dwTime;
                m_pMapList[i]->m_dwWhetherLastTime = sTime;
            }
            if (cPrevMode != m_pMapList[i]->m_cWhetherStatus)
            {
                for (j = 1; j < DEF_MAXCLIENTS; j++)
                    if ((m_pClientList[j] != NULL) && (m_pClientList[j]->m_bIsInitComplete == TRUE) && (m_pClientList[j]->m_cMapIndex == i))
                        SendNotifyMsg(NULL, j, DEF_NOTIFY_WHETHERCHANGE, m_pMapList[i]->m_cWhetherStatus, NULL, NULL, NULL);
            }
        }
    }
}

void CGame::AdminOrder_SetStatus(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, * token2, cBuff[256];
    int    iPass;
    CStrTok * pStrTok;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetStatus)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token2 = pStrTok->pGet();

    iPass = (int)token;
    if (token != NULL)
    {
        // Ã€ÃŒ Â°ÂªÃ€ÃŒ Â¹Â®Ã€Ãš '1'Ã€ÃŒÂ¸Ã© Ã…ÃµÂ¸Ã­Ã€Â¸Â·ÃŽ Â¼Â¼Ã†Â®. '0'Ã€ÃŒÂ¸Ã© Ã‡Ã˜ÃÂ¦ 

        if (token[0] == '0')
        {
            SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
            SetIllusionFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
            SetDefenseShieldFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
            SetMagicProtectionFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
            SetProtectionFromArrowFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
            SetIllusionMovementFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
            SetInhibitionCastingFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
            SetHeroFlag(iClientH, DEF_OWNERTYPE_PLAYER, FALSE);
        }
        if (token[0] == '1')
        {
            SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
        if (token[0] == '2')
        {
            SetIllusionFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
        if (token[0] == '3')
        {
            SetDefenseShieldFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
        if (token[0] == '4')
        {
            SetMagicProtectionFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
        if (token[0] == '5')
        {
            SetProtectionFromArrowFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
        if (token[0] == '6')
        {
            SetIllusionMovementFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
        if (token[0] == '7')
        {
            SetInhibitionCastingFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
        if (token[0] == '8')
        {
            SetHeroFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
        }
    }
    delete pStrTok;
}

void CGame::AdminOrder_GoTo(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char * token, cBuff[256], cPlayerName[11], cMapName[32];
    CStrTok * pStrTok;
    char * cp;
    WORD * wp;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGoto)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cPlayerName, sizeof(cPlayerName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();
    if (token == NULL)
    {
        delete pStrTok;
        return;
    }

    if (strlen(token) > 10)
    {
        memcpy(cPlayerName, token, 10);
    }
    else
    {
        memcpy(cPlayerName, token, strlen(token));
    }

    for (int i = 1; i < DEF_MAXCLIENTS; i++)
    {
        if (m_pClientList[i] != NULL)
        {
            if (memcmp(cPlayerName, m_pClientList[i]->m_cCharName, 10) == 0)
            {
                if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
                {
                    if (m_pClientList[i]->m_sX == -1 && m_pClientList[i]->m_sX == -1)
                    {
                        wsprintf(cBuff, "GM Order(%s): GoTo MapName(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cMapName);
                    }
                    else
                    {
                        wsprintf(cBuff, "GM Order(%s): GoTo MapName(%s)(%d %d)", m_pClientList[iClientH]->m_cCharName,
                            m_pClientList[i]->m_cMapName, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                    }
                    //bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, FALSE, cBuff);
                    ZeroMemory(cMapName, sizeof(cMapName));
                    strcpy(cMapName, m_pClientList[i]->m_cMapName);
                    RequestTeleportHandler(iClientH, "2   ", cMapName, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
                }
                delete pStrTok;
                return;
            }
        }
    }
    m_pClientList[iClientH]->m_bIsAdminOrderGoto = TRUE;

    ZeroMemory(cBuff, sizeof(cBuff));

    cp = (char *)cBuff;
    *cp = GSM_REQUEST_FINDCHARACTER;
    cp++;

    wp = (WORD *)cp;
    *wp = m_wServerID_GSS;
    cp += 2;

    wp = (WORD *)cp;
    *wp = iClientH;
    cp += 2;

    memcpy(cp, cPlayerName, 10);
    cp += 10;

    memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
    cp += 10;

    bStockMsgToGateServer(cBuff, 25);

    delete pStrTok;
}

void CGame::AdminOrder_MonsterCount(int iClientH, char * pData, DWORD dwMsgSize)
{
    int iMonsterCount;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelMonsterCount)
    {
        // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }
    iMonsterCount = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAliveObject;
    SendNotifyMsg(0, iClientH, DEF_NOTIFY_MONSTERCOUNT, iMonsterCount, NULL, NULL, NULL);
}

void CGame::AdminOrder_SetForceRecallTime(int iClientH, char * pData, DWORD dwMsgSize)
{
    CStrTok * pStrTok;
    char   seps[] = "= \t\n";
    char * token, cBuff[256], * cp;
    WORD * wp;
    int    iTime;

    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize) <= 0) return;


    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetRecallTime)
    {
        // Admin user levelÀÌ ³·¾Æ¼­ ÀÌ ±â´ÉÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL)
    {

        if (token == NULL)
            iTime = 0;
        else iTime = atoi(token);

        if (iTime < 0) iTime = 0;

        m_sForceRecallTime = iTime;

        ZeroMemory(cBuff, sizeof(cBuff));
        cp = (char *)cBuff;
        *cp = GSM_REQUEST_SETFORCERECALLTIME;
        cp++;

        wp = (WORD *)cp;
        *wp = iTime;
        cp += 2;

        bStockMsgToGateServer(cBuff, 3);

        wsprintf(G_cTxt, "(!) Game Server Force Recall Time (%d)min", m_sForceRecallTime);
        log->info(G_cTxt);
    }

    delete pStrTok;
    return;
}

void CGame::AdminOrder_UnsummonBoss(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelUnsummonBoss)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    for (int i = 1; i < DEF_MAXNPCS; i++)
    {
        if (m_pNpcList[i] != NULL && m_pNpcList[i]->m_bIsSummoned == FALSE)
        {
            switch (m_pNpcList[i]->m_sType)
            {
                case 31: //Demon
                case 49: //Hellclaw
                case 45: //GHKABS
                case 47: //BG
                case 50: //Tigerworm
                case 52: //Gagoyle
                case 81: //Abaddon
                case 66: //Wyvern
                case 73: //Fire-Wyvern
                    m_pNpcList[i]->m_bIsUnsummoned = TRUE;
                    NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
                    break;
            }
        }
    }
}

void CGame::AdminOrder_ClearNpc(int iClientH)
{
    if (m_pClientList[iClientH] == NULL) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelClearNpc)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    for (int i = 1; i < DEF_MAXNPCS; i++)
    {
        if (m_pNpcList[i] != NULL && m_pNpcList[i]->m_bIsSummoned == FALSE)
        {
            switch (m_pNpcList[i]->m_sType)
            {
                case 15:
                case 19:
                case 20:
                case 24:
                case 25:
                case 26:
                case 67:
                case 68:
                case 69:
                case 90:
                case 91:
                    break;

                default:
                    m_pNpcList[i]->m_bIsUnsummoned = TRUE;
                    NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
                    break;
            }
        }
    }
}

void CGame::AdminOrder_CheckStats(int iClientH, char * pData, DWORD dwMsgSize)
{
    char cStatMessage[256];
    char   seps[] = "= \t\n";
    char   cBuff[256];
    CStrTok * pStrTok;
    char * token, cName[11], cTargetName[11];
    int i;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_iAdminUserLevel < 1)
    {
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }
    if ((dwMsgSize) <= 0) return;
    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();

    token = pStrTok->pGet();

    if (token != NULL)
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, token);
    }
    else
    {
        ZeroMemory(cName, sizeof(cName));
        strcpy(cName, "null");
    }

    token = pStrTok->pGet();

    if (token == NULL)
    {
        token = "null";
    }

    if (cName != NULL)
    {
        token = cName;

        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

        for (i = 1; i < DEF_MAXCLIENTS; i++)
            if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0))
            {
                // m_iStr, m_iInt, m_iVit, m_iDex, m_iMag, m_iCharisma
                ZeroMemory(cStatMessage, sizeof(cStatMessage));
                wsprintf(cStatMessage, "Str:%d Dex:%d Vit:%d Int:%d Mag:%d Chr:%d", m_pClientList[i]->m_iStr, m_pClientList[i]->m_iDex, m_pClientList[i]->m_iVit, m_pClientList[i]->m_iInt, m_pClientList[i]->m_iMag, m_pClientList[i]->m_iCharisma);
                ShowClientMsg(iClientH, cStatMessage);
            }
    }
}

/*void CGame::AdminOrder_CleanMap(int iClientH, char * pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    CStrTok * pStrTok;
    char   * token, cMapName[11], cBuff[256];
    BOOL bFlag = FALSE;	//Used to check if we are on the map we wanna clear
    int i;
    CItem *pItem;
    short sRemainItemSprite, sRemainItemSpriteFrame, dX, dY;
    char cRemainItemColor, len;


    if (m_pClientList[iClientH] == NULL) return;
    if ((dwMsgSize)	<= 0) return;

    if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCleanMap) {
        // Admin user levelÀÌ ³·¾Æ¼­ ÀÌ ±â´ÉÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, NULL, NULL, NULL, NULL);
        return;
    }

    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != NULL) {
        ZeroMemory(cMapName, sizeof(cMapName));
        strcpy(cMapName, token);

        for (i = 0; i < DEF_MAXMAPS; i++)	//Enum all maps
            if (m_pMapList[i] != NULL) {	//Is allocated map
                if (memcmp(m_pMapList[i]->m_cName, cMapName, 10) == 0) {	//is map same name
                    bFlag = TRUE; //Set flag
                    //Get X and Y coords
                    int m_x = m_pMapList[i]->m_sSizeX;
                    int m_y = m_pMapList[i]->m_sSizeY;
                    for(int j = 1; j < m_x; j++)
                        for(int k = 1; k < m_y; k++){
                            do {	//Delete all items on current tile
                                pItem = m_pMapList[i]->pGetItem(j, k, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor); // v1.4
                                if (pItem != NULL) {
                                    delete pItem;	//Delete item;
                                }
                            } while(pItem != NULL);
                        }
                    break;	//Break outo f loop
                }
            }

        if (!bFlag) {	//Notify GM he has to be on the map he clears
        }
        else{	//Notify GM that all items have been cleared
            for(int i = 1; i < DEF_MAXCLIENTS; i++){
                if (m_pClientList[i] != NULL) {
                len = strlen(cMapName);
                if(len > 10) len = 10;
                if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, cMapName, len) != 0) return;
                dX = m_pClientList[i]->m_sX;
                dY = m_pClientList[i]->m_sY;
                ZeroMemory(cMapName,sizeof(cMapName));
                strcpy(cMapName, m_pClientList[i]->m_cMapName);
                RequestTeleportHandler(i,"2   ", cMapName, dX, dY);
                }
            }
        }
    }

    delete pStrTok;
    return;
}*/
