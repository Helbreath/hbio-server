//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

extern char G_cTxt[512];

int CGame::iComposeInitMapData(short sX, short sY, int iClientH, char * pData)
{
    int * ip, ix, iy, iSize, iTileExists;
    CTile * pTileSrc, * pTile;
    unsigned char ucHeader;
    short * sp, * pTotal;
    int     sTemp, sTemp2;
    WORD * wp;
    char * cp;

    if (m_pClientList[iClientH] == NULL) return 0;

    pTotal = (short *)pData;
    cp = (char *)(pData + 2);

    iSize = 2;
    iTileExists = 0;
    pTileSrc = (CTile *)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile +
        (sX)+(sY)*m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

    for (iy = 0; iy < 16; iy++)
        for (ix = 0; ix < 21; ix++)
        {

            if (((sX + ix) == 100) && ((sY + iy) == 100))
                sX = sX;

            pTile = (CTile *)(pTileSrc + ix + iy * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

            if ((m_pClientList[pTile->m_sOwner] != NULL) && (pTile->m_sOwner != iClientH))
                if ((m_pClientList[pTile->m_sOwner]->m_cSide != 0) &&
                    (m_pClientList[pTile->m_sOwner]->m_cSide != m_pClientList[iClientH]->m_cSide) &&
                    ((m_pClientList[pTile->m_sOwner]->m_iStatus & 0x00000010) != 0))
                {
                    continue;
                }

            if ((pTile->m_sOwner != NULL) || (pTile->m_sDeadOwner != NULL) ||
                (pTile->m_pItem[0] != NULL) || (pTile->m_sDynamicObjectType != NULL))
            {
                iTileExists++;
                sp = (short *)cp;
                *sp = ix;
                cp += 2;
                sp = (short *)cp;
                *sp = iy;
                cp += 2;
                iSize += 4;

                ucHeader = 0;
                if (pTile->m_sOwner != NULL)
                {
                    if (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER)
                    {
                        if (m_pClientList[pTile->m_sOwner] != NULL) ucHeader = ucHeader | 0x01;
                        else
                        {
                            wsprintf(G_cTxt, "Empty player handle: %d", pTile->m_sOwner);
                            log->info(G_cTxt);
                            pTile->m_sOwner = NULL;
                        }
                    }

                    if (pTile->m_cOwnerClass == DEF_OWNERTYPE_NPC)
                    {
                        if (m_pNpcList[pTile->m_sOwner] != NULL) ucHeader = ucHeader | 0x01;
                        else pTile->m_sOwner = NULL;
                    }
                }
                if (pTile->m_sDeadOwner != NULL)
                {
                    if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_PLAYER)
                    {
                        if (m_pClientList[pTile->m_sDeadOwner] != NULL) ucHeader = ucHeader | 0x02;
                        else pTile->m_sDeadOwner = NULL;
                    }
                    if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_NPC)
                    {
                        if (m_pNpcList[pTile->m_sDeadOwner] != NULL) ucHeader = ucHeader | 0x02;
                        else pTile->m_sDeadOwner = NULL;
                    }
                }
                if (pTile->m_pItem[0] != NULL)				ucHeader = ucHeader | 0x04;
                if (pTile->m_sDynamicObjectType != NULL)    ucHeader = ucHeader | 0x08;

                *cp = ucHeader;
                cp++;
                iSize++;

                if ((ucHeader & 0x01) != 0)
                {
                    switch (pTile->m_cOwnerClass)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            sp = (short *)cp;
                            *sp = pTile->m_sOwner;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sType;
                            cp += 2;
                            iSize += 2;

                            *cp = m_pClientList[pTile->m_sOwner]->m_cDir;
                            cp++;
                            iSize++;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr1;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr3;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sOwner]->m_sAppr4;
                            cp += 2;
                            iSize += 2;

                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sOwner]->m_iApprColor;
                            cp += 4;
                            iSize += 4;

                            ip = (int *)cp;

                            sTemp = m_pClientList[pTile->m_sOwner]->m_iStatus;
                            sTemp = 0x0FFFFFFF & sTemp;
                            sTemp2 = iGetPlayerABSStatus(pTile->m_sOwner, iClientH);
                            sTemp = (sTemp | (sTemp2 << 28));
                            *ip = sTemp;
                            cp += 4;
                            iSize += 4;

                            memcpy(cp, m_pClientList[pTile->m_sOwner]->m_cCharName, 10);
                            cp += 10;
                            iSize += 10;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            sp = (short *)cp;
                            *sp = pTile->m_sOwner + 10000;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sOwner]->m_sType;
                            cp += 2;
                            iSize += 2;

                            *cp = m_pNpcList[pTile->m_sOwner]->m_cDir;
                            cp++;
                            iSize++;

                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;

                            ip = (int *)cp;

                            sTemp = m_pNpcList[pTile->m_sOwner]->m_iStatus;
                            sTemp = 0x0FFFFFFF & sTemp;
                            sTemp2 = iGetNpcRelationship(pTile->m_sOwner, iClientH);
                            sTemp = (sTemp | (sTemp2 << 28));
                            *ip = sTemp;
                            cp += 4;
                            iSize += 4;

                            memcpy(cp, m_pNpcList[pTile->m_sOwner]->m_cName, 5);
                            cp += 5;
                            iSize += 5;
                            break;
                    }
                }

                if ((ucHeader & 0x02) != 0)
                {
                    switch (pTile->m_cDeadOwnerClass)
                    {
                        case DEF_OWNERTYPE_PLAYER:
                            sp = (short *)cp;
                            *sp = pTile->m_sDeadOwner;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sType;
                            cp += 2;
                            iSize += 2;

                            *cp = m_pClientList[pTile->m_sDeadOwner]->m_cDir;
                            cp++;
                            iSize++;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr1;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr3;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr4;
                            cp += 2;
                            iSize += 2;

                            ip = (int *)cp;
                            *ip = m_pClientList[pTile->m_sDeadOwner]->m_iApprColor;
                            cp += 4;
                            iSize += 4;

                            ip = (int *)cp;

                            sTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus;
                            sTemp = 0x0FFFFFFF & sTemp;
                            sTemp2 = iGetPlayerABSStatus(pTile->m_sDeadOwner, iClientH);
                            sTemp = (sTemp | (sTemp2 << 28));
                            *ip = sTemp;
                            cp += 4;
                            iSize += 4;

                            memcpy(cp, m_pClientList[pTile->m_sDeadOwner]->m_cCharName, 10);
                            cp += 10;
                            iSize += 10;
                            break;

                        case DEF_OWNERTYPE_NPC:
                            sp = (short *)cp;
                            *sp = pTile->m_sDeadOwner + 10000;
                            cp += 2;
                            iSize += 2;

                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sType;
                            cp += 2;
                            iSize += 2;

                            *cp = m_pNpcList[pTile->m_sDeadOwner]->m_cDir;
                            cp++;
                            iSize++;

                            sp = (short *)cp;
                            *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sAppr2;
                            cp += 2;
                            iSize += 2;

                            ip = (int *)cp;

                            sTemp = m_pNpcList[pTile->m_sDeadOwner]->m_iStatus;
                            sTemp = 0x0FFFFFFF & sTemp;
                            sTemp2 = iGetNpcRelationship(pTile->m_sDeadOwner, iClientH);
                            sTemp = (sTemp | (sTemp2 << 28));
                            *ip = sTemp;
                            cp += 4;
                            iSize += 4;

                            memcpy(cp, m_pNpcList[pTile->m_sDeadOwner]->m_cName, 5);
                            cp += 5;
                            iSize += 5;
                            break;
                    }
                }

                if (pTile->m_pItem[0] != NULL)
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

                if (pTile->m_sDynamicObjectType != NULL)
                {
                    wp = (WORD *)cp;
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
    return iSize;
}

int CGame::iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char * pData)
{
    int * ip, ix, iy, iSize, iTileExists, iIndex;
    CTile * pTileSrc, * pTile;
    unsigned char ucHeader;
    short * sp, * pTotal;
    int iTemp, iTemp2;
    WORD * wp;
    char * cp;

    if (m_pClientList[iClientH] == NULL) return 0;

    pTotal = (short *)pData;
    cp = (char *)(pData + 2);

    iSize = 2;
    iTileExists = 0;

    pTileSrc = (CTile *)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile +
        (sX)+(sY)*m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

    iIndex = 0;

    while (1)
    {
        ix = _tmp_iMoveLocX[cDir][iIndex];
        iy = _tmp_iMoveLocY[cDir][iIndex];
        if ((ix == -1) || (iy == -1)) break;

        iIndex++;

        pTile = (CTile *)(pTileSrc + ix + iy * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

        if ((m_pClientList[pTile->m_sOwner] != NULL) && (pTile->m_sOwner != iClientH))
            if ((m_pClientList[pTile->m_sOwner]->m_cSide != 0) &&
                (m_pClientList[pTile->m_sOwner]->m_cSide != m_pClientList[iClientH]->m_cSide) &&
                ((m_pClientList[pTile->m_sOwner]->m_iStatus & 0x00000010) != 0))
            {
                continue;
            }

        if ((pTile->m_sOwner != NULL) || (pTile->m_sDeadOwner != NULL) ||
            (pTile->m_pItem[0] != NULL) || (pTile->m_sDynamicObjectType != NULL))
        {

            iTileExists++;

            sp = (short *)cp;
            *sp = ix;
            cp += 2;
            sp = (short *)cp;
            *sp = iy;
            cp += 2;
            iSize += 4;

            ucHeader = 0;

            if (pTile->m_sOwner != NULL)
            {
                if (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER)
                {
                    if (m_pClientList[pTile->m_sOwner] != NULL) ucHeader = ucHeader | 0x01;
                    else pTile->m_sOwner = NULL;
                }
                if (pTile->m_cOwnerClass == DEF_OWNERTYPE_NPC)
                {
                    if (m_pNpcList[pTile->m_sOwner] != NULL) ucHeader = ucHeader | 0x01;
                    else pTile->m_sOwner = NULL;
                }
            }
            if (pTile->m_sDeadOwner != NULL)
            {
                if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_PLAYER)
                {
                    if (m_pClientList[pTile->m_sDeadOwner] != NULL)	ucHeader = ucHeader | 0x02;
                    else pTile->m_sDeadOwner = NULL;
                }
                if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_NPC)
                {
                    if (m_pNpcList[pTile->m_sDeadOwner] != NULL) ucHeader = ucHeader | 0x02;
                    else pTile->m_sDeadOwner = NULL;
                }
            }

            if (pTile->m_pItem[0] != NULL)				ucHeader = ucHeader | 0x04;
            if (pTile->m_sDynamicObjectType != NULL)    ucHeader = ucHeader | 0x08;

            *cp = ucHeader;
            cp++;
            iSize++;

            if ((ucHeader & 0x01) != 0)
            {
                switch (pTile->m_cOwnerClass)
                {
                    case DEF_OWNERTYPE_PLAYER:
                        sp = (short *)cp;
                        *sp = pTile->m_sOwner;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sOwner]->m_sType;
                        cp += 2;
                        iSize += 2;

                        *cp = m_pClientList[pTile->m_sOwner]->m_cDir;
                        cp++;
                        iSize++;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sOwner]->m_sAppr1;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sOwner]->m_sAppr2;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sOwner]->m_sAppr3;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sOwner]->m_sAppr4;
                        cp += 2;
                        iSize += 2;

                        ip = (int *)cp;
                        *ip = m_pClientList[pTile->m_sOwner]->m_iApprColor;
                        cp += 4;
                        iSize += 4;

                        ip = (int *)cp;

                        if (m_pClientList[iClientH]->m_cSide != m_pClientList[pTile->m_sOwner]->m_cSide)
                        {
                            if (iClientH != pTile->m_sOwner)
                            {
                                iTemp = m_pClientList[pTile->m_sOwner]->m_iStatus & 0x0F0FFFF7F;
                            }
                            else
                            {
                                iTemp = m_pClientList[pTile->m_sOwner]->m_iStatus;
                            }
                        }
                        else
                        {
                            iTemp = m_pClientList[pTile->m_sOwner]->m_iStatus;
                        }

                        iTemp = 0x0FFFFFFF & iTemp;
                        iTemp2 = iGetPlayerABSStatus(pTile->m_sOwner, iClientH);
                        iTemp = (iTemp | (iTemp2 << 28));
                        *ip = iTemp;
                        cp += 4;
                        iSize += 4;

                        memcpy(cp, m_pClientList[pTile->m_sOwner]->m_cCharName, 10);
                        cp += 10;
                        iSize += 10;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        sp = (short *)cp;
                        *sp = pTile->m_sOwner + 10000;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pNpcList[pTile->m_sOwner]->m_sType;
                        cp += 2;
                        iSize += 2;

                        *cp = m_pNpcList[pTile->m_sOwner]->m_cDir;
                        cp++;
                        iSize++;

                        sp = (short *)cp;
                        *sp = m_pNpcList[pTile->m_sOwner]->m_sAppr2;
                        cp += 2;
                        iSize += 2;

                        ip = (int *)cp;
                        iTemp = m_pNpcList[pTile->m_sOwner]->m_iStatus;
                        iTemp = 0x0FFFFFFF & iTemp;
                        iTemp2 = iGetNpcRelationship(pTile->m_sOwner, iClientH);
                        iTemp = (iTemp | (iTemp2 << 28));
                        *ip = iTemp;
                        cp += 4;
                        iSize += 4;

                        memcpy(cp, m_pNpcList[pTile->m_sOwner]->m_cName, 5);
                        cp += 5;
                        iSize += 5;
                }
            }

            if ((ucHeader & 0x02) != 0)
            {
                switch (pTile->m_cDeadOwnerClass)
                {
                    case DEF_OWNERTYPE_PLAYER:

                        sp = (short *)cp;
                        *sp = pTile->m_sDeadOwner;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sDeadOwner]->m_sType;
                        cp += 2;
                        iSize += 2;

                        *cp = m_pClientList[pTile->m_sDeadOwner]->m_cDir;
                        cp++;
                        iSize++;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr1;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr2;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr3;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr4;
                        cp += 2;
                        iSize += 2;

                        ip = (int *)cp;
                        *ip = m_pClientList[pTile->m_sDeadOwner]->m_iApprColor;
                        cp += 4;
                        iSize += 4;

                        ip = (int *)cp;

                        if (m_pClientList[iClientH]->m_cSide != m_pClientList[pTile->m_sDeadOwner]->m_cSide)
                        {
                            if (iClientH != pTile->m_sDeadOwner)
                            {
                                iTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus & 0x0F0FFFF7F;
                            }
                            else
                            {
                                iTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus;
                            }
                        }
                        else
                        {
                            iTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus;
                        }

                        iTemp = 0x0FFFFFFF & iTemp;

                        iTemp2 = iGetPlayerABSStatus(pTile->m_sDeadOwner, iClientH);
                        iTemp = (iTemp | (iTemp2 << 28));
                        *ip = iTemp;
                        cp += 4;
                        iSize += 4;

                        memcpy(cp, m_pClientList[pTile->m_sDeadOwner]->m_cCharName, 10);
                        cp += 10;
                        iSize += 10;
                        break;

                    case DEF_OWNERTYPE_NPC:
                        sp = (short *)cp;
                        *sp = pTile->m_sDeadOwner + 10000;
                        cp += 2;
                        iSize += 2;

                        sp = (short *)cp;
                        *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sType;
                        cp += 2;
                        iSize += 2;

                        *cp = m_pNpcList[pTile->m_sDeadOwner]->m_cDir;
                        cp++;
                        iSize++;

                        sp = (short *)cp;
                        *sp = m_pNpcList[pTile->m_sDeadOwner]->m_sAppr2;
                        cp += 2;
                        iSize += 2;

                        ip = (int *)cp;

                        iTemp = m_pNpcList[pTile->m_sDeadOwner]->m_iStatus;
                        iTemp = 0x0FFFFFFF & iTemp;
                        iTemp2 = iGetNpcRelationship(pTile->m_sDeadOwner, iClientH);
                        iTemp = (iTemp | (iTemp2 << 28));
                        *ip = iTemp;

                        cp += 4;
                        iSize += 4;

                        memcpy(cp, m_pNpcList[pTile->m_sDeadOwner]->m_cName, 5);
                        cp += 5;
                        iSize += 5;
                        break;
                }
            }

            if (pTile->m_pItem[0] != NULL)
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

            if (pTile->m_sDynamicObjectType != NULL)
            {

                wp = (WORD *)cp;
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
    return iSize;
}

void CGame::ResponseCreateNewGuildHandler(char * pData, DWORD dwMsgSize)
{
    int i;
    WORD * wp, wResult;
    DWORD * dwp;
    char * cp, cCharName[11], cData[100], cTxt[120];
    int iRet;

    ZeroMemory(cCharName, sizeof(cCharName));
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memcpy(cCharName, cp, 10);
    cp += 10;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0) &&
            (m_pClientList[i]->m_iLevel >= 20) && (m_pClientList[i]->m_iCharisma >= 20))
        {

            wp = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
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
                    ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
                    memcpy(m_pClientList[i]->m_cGuildName, "NONE", 4);
                    m_pClientList[i]->m_iGuildRank = -1;
                    m_pClientList[i]->m_iGuildGUID = -1;
                    wsprintf(cTxt, "(!) New guild(%s) creation Fail! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
                    log->info(cTxt);
                    break;
            }

            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_CREATENEWGUILD;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = wResult;

            iRet = m_pClientList[i]->iSendMsg(cData, 6);
            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:
                    DeleteClient(i, TRUE, TRUE);
                    return;
            }

            return;
        }

    wsprintf(cTxt, "(!)Non-existing player data received from Log server(2): CharName(%s)", cCharName);
    log->info(cTxt);
}

void CGame::ResponseDisbandGuildHandler(char * pData, DWORD dwMsgSize)
{
    int i;
    WORD * wp, wResult;
    DWORD * dwp;
    char * cp, cCharName[11], cData[100], cTxt[120];
    int iRet;

    ZeroMemory(cCharName, sizeof(cCharName));
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
    memcpy(cCharName, cp, 10);
    cp += 10;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0))
        {

            wp = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
            switch (*wp)
            {
                case DEF_LOGRESMSGTYPE_CONFIRM:
                    wResult = DEF_MSGTYPE_CONFIRM;
                    wsprintf(cTxt, "(!) Disband guild(%s) success! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
                    log->info(cTxt);

                    SendGuildMsg(i, DEF_NOTIFY_GUILDDISBANDED, NULL, NULL, NULL);

                    ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
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

            dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
            *dwp = MSGID_RESPONSE_DISBANDGUILD;
            wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
            *wp = wResult;

            iRet = m_pClientList[i]->iSendMsg(cData, 6);
            switch (iRet)
            {
                case DEF_XSOCKEVENT_QUENEFULL:
                case DEF_XSOCKEVENT_SOCKETERROR:
                case DEF_XSOCKEVENT_CRITICALERROR:
                case DEF_XSOCKEVENT_SOCKETCLOSED:
                    DeleteClient(i, TRUE, TRUE);
                    return;
            }
            return;
        }

    wsprintf(cTxt, "(!)Non-existing player data received from Log server(2): CharName(%s)", cCharName);
    log->info(cTxt);
}

void CGame::ResponsePlayerDataHandler(char * pData, DWORD dwSize)
{
    WORD * wp;
    char * cp, cCharName[11], cTxt[120];
    int  i;

    ZeroMemory(cCharName, sizeof(cCharName));
    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    memcpy(cCharName, cp, 10);
    cp += 10;

    for (i = 1; i < DEF_MAXCLIENTS; i++)
        if (m_pClientList[i] != NULL)
        {
            if (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0)
            {
                wp = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
                switch (*wp)
                {
                    case DEF_LOGRESMSGTYPE_CONFIRM:
                        InitPlayerData(i, pData, dwSize);
                        break;

                    case DEF_LOGRESMSGTYPE_REJECT:
                        wsprintf(G_cTxt, "(HACK?) Not existing character(%s) data request! Rejected!", m_pClientList[i]->m_cCharName);
                        log->info(G_cTxt);

                        DeleteClient(i, FALSE, FALSE);
                        break;

                    default:
                        break;
                }

                return;
            }
        }

    wsprintf(cTxt, "(!)Non-existing player data received from Log server: CharName(%s)", cCharName);
    log->info(cTxt);
}

void CGame::InitPlayerData(int iClientH, char * pData, DWORD dwSize)
{
    char * cp, cName[11], cData[256], cTxt[256], cGuildStatus, cQuestRemain;
    DWORD * dwp;
    WORD * wp;
    int     iRet, i, iTotalPoints;
    BOOL    bRet;

    if (m_pClientList[iClientH] == NULL) return;
    if (m_pClientList[iClientH]->m_bIsInitComplete == TRUE) return;

    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

    ZeroMemory(cName, sizeof(cName));
    memcpy(cName, cp, 10);
    cp += 10;

    //m_pClientList[iClientH]->m_cAccountStatus = *cp;
    cp++;

    cGuildStatus = *cp;
    cp++;

    m_pClientList[iClientH]->m_iHitRatio = 0;
    m_pClientList[iClientH]->m_iDefenseRatio = 0;
    m_pClientList[iClientH]->m_cSide = 0;

    bRet = _bDecodePlayerDatafileContents(iClientH, cp, dwSize - 19);
    if (bRet == FALSE)
    {
        wsprintf(G_cTxt, "(HACK?) Character(%s) data error!", m_pClientList[iClientH]->m_cCharName);
        DeleteClient(iClientH, FALSE, TRUE);
        return;
    }

    ___RestorePlayerCharacteristics(iClientH);

    ___RestorePlayerRating(iClientH);

    if ((m_pClientList[iClientH]->m_sX == -1) && (m_pClientList[iClientH]->m_sY == -1))
    {
        GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cLocation);
    }

    SetPlayingStatus(iClientH);
    int iTemp, iTemp2;
    iTemp = m_pClientList[iClientH]->m_iStatus;
    iTemp = 0x0FFFFFFF & iTemp;
    iTemp2 = iGetPlayerABSStatus(iClientH);
    iTemp = iTemp | (iTemp2 << 28);
    m_pClientList[iClientH]->m_iStatus = iTemp;

    if (m_pClientList[iClientH]->m_iLevel > 100)
        if (m_pClientList[iClientH]->m_bIsPlayerCivil == TRUE)
            ForceChangePlayMode(iClientH, FALSE);

    m_pClientList[iClientH]->m_iNextLevelExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel + 1];

    CalcTotalItemEffect(iClientH, -1, TRUE);
    iCalcTotalWeight(iClientH);

    if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
    {
        SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, TRUE);
    }

    if ((m_pClientList[iClientH]->m_iLevel > 2) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0) &&
        (m_pClientList[iClientH]->m_iExp < iGetLevelExp(m_pClientList[iClientH]->m_iLevel - 1) - 3000))
    {
        try
        {
            wsprintf(G_cTxt, "Data Error: (%s) Player: (%s) CurrentExp: %d --- Minimum Exp: %d", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iExp, (iGetLevelExp(m_pClientList[iClientH]->m_iLevel) - 1));
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {
        }
        return;
    }

    iTotalPoints = 0;
    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        iTotalPoints += m_pClientList[iClientH]->m_cSkillMastery[i];
    if ((iTotalPoints - 21 > m_sCharSkillLimit) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0))
    {
        try
        {
            wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) - has more than allowed skill points (%d).", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, iTotalPoints);
            log->info(G_cTxt);
            DeleteClient(iClientH, TRUE, TRUE);
        }
        catch (...)
        {
        }
        return;
    }

    CheckSpecialEvent(iClientH);
    bCheckMagicInt(iClientH);

    if ((cGuildStatus == 0) && (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0))
    {
        ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
        strcpy(m_pClientList[iClientH]->m_cGuildName, "NONE");
        m_pClientList[iClientH]->m_iGuildRank = -1;
        m_pClientList[iClientH]->m_iGuildGUID = -1;

        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_GUILDDISBANDED, NULL, NULL, NULL, m_pClientList[iClientH]->m_cGuildName);
    }

    if (m_pClientList[iClientH]->m_iQuest != NULL)
    {
        cQuestRemain = (m_pQuestConfigList[m_pClientList[iClientH]->m_iQuest]->m_iMaxCount - m_pClientList[iClientH]->m_iCurQuestCount);
        SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_QUESTCOUNTER, cQuestRemain, NULL, NULL, NULL);
        _bCheckIsQuestCompleted(iClientH);
    }


    if (m_pClientList[iClientH] == NULL)
    {
        wsprintf(cTxt, "<%d> InitPlayerData error - Socket error! Disconnected.", iClientH);
        log->info(cTxt);
        return;
    }

    ZeroMemory(cData, sizeof(cData));
    dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
    *dwp = MSGID_RESPONSE_INITPLAYER;
    wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
    *wp = DEF_MSGTYPE_CONFIRM;

    iRet = m_pClientList[iClientH]->iSendMsg(cData, 6);
    switch (iRet)
    {
        case DEF_XSOCKEVENT_QUENEFULL:
        case DEF_XSOCKEVENT_SOCKETERROR:
        case DEF_XSOCKEVENT_CRITICALERROR:
        case DEF_XSOCKEVENT_SOCKETCLOSED:
            wsprintf(cTxt, "<%d> InitPlayerData - Socket error! Disconnected.", iClientH);
            log->info(cTxt);

            DeleteClient(iClientH, FALSE, TRUE);
            return;
    }

    m_pClientList[iClientH]->m_bIsInitComplete = TRUE;

    //bSendMsgToLS(MSGID_ENTERGAMECONFIRM, iClientH);

    if (m_iTotalClients > DEF_MAXONESERVERUSERS)
    {
        switch (iDice(1, 2))
        {
            case 1:
                RequestTeleportHandler(iClientH, "2   ", "bisle", -1, -1);
                break;
            case 2:
                switch (m_pClientList[iClientH]->m_cSide)
                {
                    case 0: RequestTeleportHandler(iClientH, "2   ", "resurr1", -1, -1); break;
                    case 1: RequestTeleportHandler(iClientH, "2   ", "resurr1", -1, -1); break;
                    case 2: RequestTeleportHandler(iClientH, "2   ", "resurr2", -1, -1); break;
                }
                break;
        }
    }

    return;
}
