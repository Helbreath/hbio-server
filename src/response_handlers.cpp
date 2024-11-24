//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include "Map.h"
#include "Tile.h"

extern char G_cTxt[512];

int CGame::iComposeInitMapData(short sX, short sY, int iClientH, char * pData)
{
    int * ip, iSize, iTileExists;
    CTile * pTileSrc, * pTile;
    unsigned char ucHeader;
    short * sp, * pTotal;
    int	 iTemp, iTemp2;
    uint16_t * wp;
    char * cp;

    if (m_pClientList[iClientH] == 0) return 0;

    pTotal = (short *)pData;
    cp = (char *)(pData + 2);

    CClient * client = m_pClientList[iClientH];

    iSize = 2;
    iTileExists = 0;
    pTileSrc = (CTile *)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile +
        (sX)+(sY)*m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

    for (int ix = client->m_sX - client->screen_size_x / 2 - 3; ix < client->m_sX - client->screen_size_x; ++ix)
        for (int iy = client->m_sY - client->screen_size_y / 2 - 3; iy < client->m_sY - client->screen_size_y; ++iy)
        {
            pTile = (CTile *)(pTileSrc + ix + iy * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

            if ((pTile->m_sOwner != 0) || (pTile->m_sDeadOwner != 0) ||
                (pTile->m_pItem[0] != 0) || (pTile->m_sDynamicObjectType != 0))
            {
                iTileExists++;

                sp = (short *)cp;
                *sp = (short)ix;
                cp += 2;
                sp = (short *)cp;
                *sp = (short)iy;
                cp += 2;
                iSize += 4;

                ucHeader = 0;
                if (pTile->m_sOwner != 0)
                {


                    if (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER)
                    {
                        if (m_pClientList[pTile->m_sOwner] != 0) ucHeader = ucHeader | 0x01;
                        else
                        {
                            // ###debugcode
                            wsprintf(G_cTxt, "Empty player handle: %d", pTile->m_sOwner);
                            //log->info(G_cTxt);
                            //
                            pTile->m_sOwner = 0;
                        }
                    }

                    if (pTile->m_cOwnerClass == DEF_OWNERTYPE_NPC)
                    {
                        if (m_pNpcList[pTile->m_sOwner] != 0) ucHeader = ucHeader | 0x01;
                        else pTile->m_sOwner = 0;
                    }
                }
                if (pTile->m_sDeadOwner != 0)
                {
                    if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_PLAYER)
                    {
                        if (m_pClientList[pTile->m_sDeadOwner] != 0) ucHeader = ucHeader | 0x02;
                        else pTile->m_sDeadOwner = 0;
                    }
                    if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_NPC)
                    {
                        if (m_pNpcList[pTile->m_sDeadOwner] != 0) ucHeader = ucHeader | 0x02;
                        else pTile->m_sDeadOwner = 0;
                    }
                }
                if (pTile->m_pItem[0] != 0)				ucHeader = ucHeader | 0x04;
                if (pTile->m_sDynamicObjectType != 0)    ucHeader = ucHeader | 0x08;
                //
                *cp = ucHeader;
                cp++;
                iSize++;

                if ((ucHeader & 0x01) != 0)
                {
                    switch (pTile->m_cOwnerClass)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            // Object ID number(Player) : 1~10000
                            sp = (short *)cp;
                            *sp = pTile->m_sOwner;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pClientList[pTile->m_sOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appearance1
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr1;
                            cp += 2;
                            iSize += 2;
                            // Appearance2
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Appearance3
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr3;
                            cp += 2;
                            iSize += 2;
                            // Appearance4
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr4;
                            cp += 2;
                            iSize += 2;
                            // v1.4 ApprColor
                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sOwner]->m_iApprColor;
                            cp += 4;
                            iSize += 4;

                            // Status
                            ip = (int *)cp;//Change short to int status


                            iTemp = m_pClientList[pTile->m_sOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            //sTemp2 = (short)iGetPlayerABSStatus(pTile->m_sOwner); // 2002-11-14
                            iTemp2 = (int)iGetPlayerABSStatus(pTile->m_sOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pClientList[pTile->m_sOwner]->m_cCharName, 10);
                            cp += 10;
                            iSize += 10;

                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sOwner]->m_iVit * 3 + m_pClientList[pTile->m_sOwner]->m_iLevel * 2 + m_pClientList[pTile->m_sOwner]->m_iStr / 2;
                            cp += 4;
                            iSize += 4;

                            ip = (int *)cp;
                            *ip = m_pNpcList[pTile->m_sOwner]->m_iHP;
                            cp += 4;
                            iSize += 4;

                            //Change HP Bar

                            break;

                        case DEF_OWNERTYPE_NPC:
                            // Object ID number(NPC) : 10000~
                            sp = (short *)cp;
                            *sp = pTile->m_sOwner + 10000;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pNpcList[pTile->m_sOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appr2
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Status
                            ip = (int *)cp;

                            iTemp = m_pNpcList[pTile->m_sOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            iTemp2 = iGetNpcRelationship(pTile->m_sOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pNpcList[pTile->m_sOwner]->m_cName, 5);
                            cp += 5;
                            iSize += 5;


                            ip = (int *)cp;
                            *ip = m_pNpcList[pTile->m_sOwner]->m_iMaxHP;
                            cp += 4;
                            iSize += 4;

                            ip = (int *)cp;
                            *ip = m_pNpcList[pTile->m_sOwner]->m_iHP;
                            cp += 4;
                            iSize += 4;

                            //Change HP Bar


                            break;
                    }
                }

                if ((ucHeader & 0x02) != 0)
                {

                    switch (pTile->m_cDeadOwnerClass)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            // Object ID number : 1~10000
                            sp = (short *)cp;
                            *sp = pTile->m_sDeadOwner;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pClientList[pTile->m_sDeadOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appearance1
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr1;
                            cp += 2;
                            iSize += 2;
                            // Appearance2
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Appearance3
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr3;
                            cp += 2;
                            iSize += 2;
                            // Appearance4
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr4;
                            cp += 2;
                            iSize += 2;
                            // v1.4 ApprColor
                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sDeadOwner]->m_iApprColor;
                            cp += 4;
                            iSize += 4;

                            // Status
                            ip = (int *)cp;


                            iTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            //sTemp2 = (short)iGetPlayerABSStatus(pTile->m_sDeadOwner); // 2002-11-14
                            iTemp2 = (int)iGetPlayerABSStatus(pTile->m_sDeadOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pClientList[pTile->m_sDeadOwner]->m_cCharName, 10);
                            cp += 10;
                            iSize += 10;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            // Object ID number : 10000	~
                            sp = (short *)cp;
                            *sp = pTile->m_sDeadOwner + 10000;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pNpcList[pTile->m_sDeadOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appr2
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Status
                            ip = (int *)cp;

                            iTemp = m_pNpcList[pTile->m_sDeadOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            iTemp2 = iGetNpcRelationship(pTile->m_sDeadOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pNpcList[pTile->m_sDeadOwner]->m_cName, 5);
                            cp += 5;
                            iSize += 5;
                            break;
                    }
                }

                if (pTile->m_pItem[0] != 0)
                {


                    sp = (short *)cp;
                    *sp = pTile->m_pItem[0]->m_sSprite;
                    cp += 2;
                    iSize += 2;

                    sp = (short *)cp;
                    *sp = pTile->m_pItem[0]->m_sSpriteFrame;
                    cp += 2;
                    iSize += 2;

                    *cp = pTile->m_pItem[0]->m_cItemColor;
                    cp++;
                    iSize++;
                }

                if (pTile->m_sDynamicObjectType != 0)
                {

                    wp = (uint16_t *)cp;
                    *wp = pTile->m_wDynamicObjectID;
                    cp += 2;
                    iSize += 2;

                    sp = (short *)cp;
                    *sp = pTile->m_sDynamicObjectType;
                    cp += 2;
                    iSize += 2;
                }
            }
        }

    *pTotal = iTileExists;

    // testcode
    //wsprintf(G_cTxt, "ComposeInitMapData: %d", iTileExists);
    //log->info(G_cTxt);

    return iSize;
}

int CGame::iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char * pData)
{
    int * ip, iSize, iTileExists;
    CTile * pTileSrc, * pTile;
    unsigned char ucHeader;
    short * sp, * pTotal;
    int	 iTemp, iTemp2;
    uint16_t * wp;
    char * cp;

    if (m_pClientList[iClientH] == 0) return 0;

    pTotal = (short *)pData;
    cp = (char *)(pData + 2);

    iSize = 2;
    iTileExists = 0;
    pTileSrc = (CTile *)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile +
        (sX)+(sY)*m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

    CClient * client = m_pClientList[iClientH];

    for (int x = client->m_sX - client->screen_size_x / 2 - 3; x < client->m_sX - client->screen_size_x; ++x)
    {
        for (int y = client->m_sY - client->screen_size_y / 2 - 3; y < client->m_sY - client->screen_size_y; ++y)
        {
            if (x < 0 || y < 0 || x >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeX || y >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY)
                continue;

            pTile = (CTile *)(pTileSrc + x + y * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);


            if ((pTile->m_sOwner != 0) || (pTile->m_sDeadOwner != 0) ||
                (pTile->m_pItem[0] != 0) || (pTile->m_sDynamicObjectType != 0))
            {
                iTileExists++;

                sp = (short *)cp;
                *sp = x;
                cp += 2;
                sp = (short *)cp;
                *sp = y;
                cp += 2;
                iSize += 4;

                ucHeader = 0;
                if (pTile->m_sOwner != 0)
                {

                    if (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER)
                    {
                        if (m_pClientList[pTile->m_sOwner] != 0) ucHeader = ucHeader | 0x01;
                        else pTile->m_sOwner = 0;
                    }
                    if (pTile->m_cOwnerClass == DEF_OWNERTYPE_NPC)
                    {
                        if (m_pNpcList[pTile->m_sOwner] != 0) ucHeader = ucHeader | 0x01;
                        else pTile->m_sOwner = 0;
                    }
                }
                if (pTile->m_sDeadOwner != 0)
                {
                    if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_PLAYER)
                    {
                        if (m_pClientList[pTile->m_sDeadOwner] != 0)	ucHeader = ucHeader | 0x02;
                        else pTile->m_sDeadOwner = 0;
                    }
                    if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_NPC)
                    {
                        if (m_pNpcList[pTile->m_sDeadOwner] != 0) ucHeader = ucHeader | 0x02;
                        else pTile->m_sDeadOwner = 0;
                    }
                }
                if (pTile->m_pItem[0] != 0)				ucHeader = ucHeader | 0x04;
                if (pTile->m_sDynamicObjectType != 0)    ucHeader = ucHeader | 0x08;
                //
                *cp = ucHeader;
                cp++;
                iSize++;

                if ((ucHeader & 0x01) != 0)
                {

                    switch (pTile->m_cOwnerClass)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            // Object ID number(Player) : 1~10000
                            sp = (short *)cp;
                            *sp = pTile->m_sOwner;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pClientList[pTile->m_sOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appearance1
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr1;
                            cp += 2;
                            iSize += 2;
                            // Appearance2
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Appearance3
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr3;
                            cp += 2;
                            iSize += 2;
                            // Appearance4
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr4;
                            cp += 2;
                            iSize += 2;
                            // v1.4 
                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sOwner]->m_iApprColor;
                            cp += 4;
                            iSize += 4;

                            // Status
                            ip = (int *)cp;

                            iTemp = m_pClientList[pTile->m_sOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            //sTemp2 = (short)iGetPlayerABSStatus(pTile->m_sOwner); // 2002-11-14
                            iTemp2 = (int)iGetPlayerABSStatus(pTile->m_sOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pClientList[pTile->m_sOwner]->m_cCharName, 10);
                            cp += 10;
                            iSize += 10;

                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sOwner]->m_iVit * 3 + m_pClientList[pTile->m_sOwner]->m_iLevel * 2 + m_pClientList[pTile->m_sOwner]->m_iStr / 2;
                            cp += 4;
                            iSize += 4;

                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sOwner]->m_iHP;
                            cp += 4;
                            iSize += 4;

                            //Change HP Bar

                            break;

                        case DEF_OWNERTYPE_NPC:
                            // Object ID number(NPC) : 10000	~
                            sp = (short *)cp;
                            *sp = pTile->m_sOwner + 10000;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pNpcList[pTile->m_sOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appearance2
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Status
                            ip = (int *)cp;

                            iTemp = m_pNpcList[pTile->m_sOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            iTemp2 = iGetNpcRelationship(pTile->m_sOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pNpcList[pTile->m_sOwner]->m_cName, 5);
                            cp += 5;
                            iSize += 5;

                            ip = (int *)cp;
                            *ip = m_pNpcList[pTile->m_sOwner]->m_iMaxHP;
                            cp += 4;
                            iSize += 4;

                            ip = (int *)cp;
                            *ip = m_pNpcList[pTile->m_sOwner]->m_iHP;
                            cp += 4;
                            iSize += 4;

                            //Change HP Bar
                            break;
                    }
                }

                if ((ucHeader & 0x02) != 0)
                {

                    switch (pTile->m_cDeadOwnerClass)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            // Object ID number(Player) : 1~10000
                            sp = (short *)cp;
                            *sp = pTile->m_sDeadOwner;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pClientList[pTile->m_sDeadOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appearance1
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr1;
                            cp += 2;
                            iSize += 2;
                            // Appearance2
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Appearance3
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr3;
                            cp += 2;
                            iSize += 2;
                            // Appearance4
                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr4;
                            cp += 2;
                            iSize += 2;
                            // v1.4 ApprColor
                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sDeadOwner]->m_iApprColor;
                            cp += 4;
                            iSize += 4;

                            // Status
                            ip = (int *)cp;

                            iTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            //sTemp2 = (short)iGetPlayerABSStatus(pTile->m_sDeadOwner); // 2002-11-14
                            iTemp2 = (int)iGetPlayerABSStatus(pTile->m_sDeadOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pClientList[pTile->m_sDeadOwner]->m_cCharName, 10);
                            cp += 10;
                            iSize += 10;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            // Object ID number(NPC) : 10000~
                            sp = (short *)cp;
                            *sp = pTile->m_sDeadOwner + 10000;
                            cp += 2;
                            iSize += 2;
                            // object type
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sType;
                            cp += 2;
                            iSize += 2;
                            // dir
                            *cp = m_pNpcList[pTile->m_sDeadOwner]->m_cDir;
                            cp++;
                            iSize++;
                            // Appearance2
                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;
                            // Status
                            ip = (int *)cp;

                            iTemp = m_pNpcList[pTile->m_sDeadOwner]->m_iStatus;
                            iTemp = 0x0FFFFFFF & iTemp;
                            iTemp2 = iGetNpcRelationship(pTile->m_sDeadOwner, iClientH);
                            iTemp = (iTemp | (iTemp2 << 28));
                            *ip = iTemp;
                            cp += 4;//+2
                            iSize += 4;//+2
                            // Name
                            memcpy(cp, m_pNpcList[pTile->m_sDeadOwner]->m_cName, 5);
                            cp += 5;
                            iSize += 5;
                            break;
                    }
                }

                if (pTile->m_pItem[0] != 0)
                {


                    sp = (short *)cp;
                    *sp = pTile->m_pItem[0]->m_sSprite;
                    cp += 2;
                    iSize += 2;

                    sp = (short *)cp;
                    *sp = pTile->m_pItem[0]->m_sSpriteFrame;
                    cp += 2;
                    iSize += 2;

                    *cp = pTile->m_pItem[0]->m_cItemColor;
                    cp++;
                    iSize++;
                }

                if (pTile->m_sDynamicObjectType != 0)
                {

                    wp = (uint16_t *)cp;
                    *wp = pTile->m_wDynamicObjectID;
                    cp += 2;
                    iSize += 2;

                    sp = (short *)cp;
                    *sp = pTile->m_sDynamicObjectType;
                    cp += 2;
                    iSize += 2;
                }
            }
        }
    }

    *pTotal = iTileExists;
    return iSize;
}

void CGame::ResponseCreateNewGuildHandler(char * pData, uint32_t dwMsgSize)
{
    int i{};
    uint16_t * wp, wResult{};
    uint32_t * dwp;
    char * cp, cCharName[11]{}, cData[100]{}, cTxt[120]{};
    int iRet{};

    memset(cCharName, 0, sizeof(cCharName));
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memcpy(cCharName, cp, 10);
    cp += 10;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0) &&
            (m_pClientList[i]->m_iLevel >= 20) && (m_pClientList[i]->m_iCharisma >= 20))
        {

            wp = (uint16_t *)(pData + DEF_INDEX2_MSGTYPE);
            switch (*wp)
            {
                case DEF_LOGRESMSGTYPE_CONFIRM:
                    wResult = DEF_MSGTYPE_CONFIRM;
                    m_pClientList[i]->m_iGuildRank = 0;
                    wsprintf(cTxt, "(!) New guild(%s) creation success! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
                    log->info(cTxt);
                    break;

                case DEF_LOGRESMSGTYPE_REJECT:
                    wResult = DEF_MSGTYPE_REJECT;
                    memset(m_pClientList[i]->m_cGuildName, 0, sizeof(m_pClientList[i]->m_cGuildName));
                    memcpy(m_pClientList[i]->m_cGuildName, "NONE", 4);
                    m_pClientList[i]->m_iGuildRank = -1;
                    m_pClientList[i]->m_iGuildGUID = -1;
                    wsprintf(cTxt, "(!) New guild(%s) creation Fail! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
                    log->info(cTxt);
                    break;
            }

            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_CREATENEWGUILD;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = wResult;

            iRet = m_pClientList[i]->iSendMsg(cData, 6);
            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:
                    DeleteClient(i, true, true);
                    return;
            }

            return;
        }

    wsprintf(cTxt, "(!)Non-existing player data received from Log server(2): CharName(%s)", cCharName);
    log->info(cTxt);
}

void CGame::ResponseDisbandGuildHandler(char * pData, uint32_t dwMsgSize)
{
    int i{};
    uint16_t * wp, wResult{};
    uint32_t * dwp;
    char * cp, cCharName[11]{}, cData[100]{}, cTxt[120]{};
    int iRet{};

    memset(cCharName, 0, sizeof(cCharName));
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memcpy(cCharName, cp, 10);
    cp += 10;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0))
        {

            wp = (uint16_t *)(pData + DEF_INDEX2_MSGTYPE);
            switch (*wp)
            {
                case DEF_LOGRESMSGTYPE_CONFIRM:
                    wResult = DEF_MSGTYPE_CONFIRM;
                    wsprintf(cTxt, "(!) Disband guild(%s) success! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
                    log->info(cTxt);

                    SendGuildMsg(i, DEF_NOTIFY_GUILDDISBANDED, 0, 0, 0);

                    memset(m_pClientList[i]->m_cGuildName, 0, sizeof(m_pClientList[i]->m_cGuildName));
                    memcpy(m_pClientList[i]->m_cGuildName, "NONE", 4);
                    m_pClientList[i]->m_iGuildRank = -1;
                    m_pClientList[i]->m_iGuildGUID = -1;
                    break;

                case DEF_LOGRESMSGTYPE_REJECT:
                    wResult = DEF_MSGTYPE_REJECT;
                    wsprintf(cTxt, "(!) Disband guild(%s) Fail! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
                    log->info(cTxt);
                    break;
            }

            dwp = (uint32_t *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_DISBANDGUILD;
            wp = (uint16_t *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = wResult;

            iRet = m_pClientList[i]->iSendMsg(cData, 6);
            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:
                    DeleteClient(i, true, true);
                    return;
            }
            return;
        }

    wsprintf(cTxt, "(!)Non-existing player data received from Log server(2): CharName(%s)", cCharName);
    log->info(cTxt);
}
