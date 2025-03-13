//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "map.h"

CMap::CMap(CGame * pGame, int map_index)
    : m_bIsSnowEnabled(false)
    , m_cMapIndex(map_index)
{
    int i, ix, iy;

    for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
        m_pTeleportLoc[i] = 0;

    for (i = 0; i < DEF_MAXWAYPOINTCFG; i++)
    {
        m_WaypointList[i].x = -1;
        m_WaypointList[i].y = -1;
    }

    for (i = 0; i < DEF_MAXMGAR; i++)
    {
        m_rcMobGenAvoidRect[i].top = -1;
        m_rcMobGenAvoidRect[i].left = -1;
    }

    for (i = 0; i < DEF_MAXNMR; i++)
    {
        m_rcNoAttackRect[i].top = -1;
        m_rcNoAttackRect[i].left = -1;
    }

    for (i = 0; i < DEF_MAXSPOTMOBGENERATOR; i++)
    {
        m_stSpotMobGenerator[i].bDefined = false;
        m_stSpotMobGenerator[i].iTotalActiveMob = 0;
    }

    for (i = 0; i < DEF_MAXFISHPOINT; i++)
    {
        m_FishPointList[i].x = -1;
        m_FishPointList[i].y = -1;
    }

    for (i = 0; i < DEF_MAXMINERALPOINT; i++)
    {
        m_MineralPointList[i].x = -1;
        m_MineralPointList[i].y = -1;
    }

    for (i = 0; i < DEF_MAXINITIALPOINT; i++)
    {
        m_pInitialPoint[i].x = -1;
        m_pInitialPoint[i].y = -1;
    }

    for (i = 0; i < 1000; i++)
        m_bNamingValueUsingStatus[i] = false;

    for (i = 0; i < DEF_MAXOCCUPYFLAG; i++)
        m_pOccupyFlag[i] = 0;

    for (i = 0; i < DEF_MAXSTRATEGICPOINTS; i++)
        m_pStrategicPointList[i] = 0;

    for (i = 0; i < DEF_MAXENERGYSPHERES; i++)
    {
        m_stEnergySphereCreationList[i].cType = 0;
        m_stEnergySphereGoalList[i].cResult = 0;
    }

    for (i = 0; i < DEF_MAXITEMEVENTS; i++)
    {
        memset(m_stItemEventList[i].cItemName, 0, sizeof(m_stItemEventList[i].cItemName));
        m_stItemEventList[i].iAmount = 0;
        m_stItemEventList[i].iTotalNum = 0;
        m_stItemEventList[i].iCurNum = 0;
        m_stItemEventList[i].iMonth = 0;
        m_stItemEventList[i].iDay = 0;
    }
    m_iTotalItemEvents = 0;

    m_iTotalActiveObject = 0;
    m_iTotalAliveObject = 0;

    //m_sInitialPointX = 0;
    //m_sInitialPointY = 0;

    m_bIsFixedDayMode = false;
    m_bIsFixedSnowMode = false;

    m_iTotalFishPoint = 0;
    m_iMaxFish = 0;
    m_iCurFish = 0;

    m_iTotalMineralPoint = 0;
    m_iMaxMineral = 0;
    m_iCurMineral = 0;

    m_pTile = 0;

    m_cWhetherStatus = 0;
    m_cType = DEF_MAPTYPE_NORMAL;

    m_pGame = pGame;

    m_iLevelLimit = 0;
    m_iUpperLevelLimit = 0;
    m_bMineralGenerator = false;

    m_iTotalOccupyFlags = 0;

    m_bIsAttackEnabled = true;
    m_cRandomMobGeneratorLevel = 0;

    m_bIsFightZone = false;

    m_iTotalEnergySphereCreationPoint = 0;
    m_iTotalEnergySphereGoalPoint = 0;

    m_bIsEnergySphereGoalEnabled = false;
    m_iCurEnergySphereGoalPointIndex = -1;

    for (ix = 0; ix < DEF_MAXSECTORS; ix++)
        for (iy = 0; iy < DEF_MAXSECTORS; iy++)
        {
            m_stSectorInfo[ix][iy].iNeutralActivity = 0;
            m_stSectorInfo[ix][iy].iAresdenActivity = 0;
            m_stSectorInfo[ix][iy].iElvineActivity = 0;
            m_stSectorInfo[ix][iy].iMonsterActivity = 0;
            m_stSectorInfo[ix][iy].iPlayerActivity = 0;

            m_stTempSectorInfo[ix][iy].iNeutralActivity = 0;
            m_stTempSectorInfo[ix][iy].iAresdenActivity = 0;
            m_stTempSectorInfo[ix][iy].iElvineActivity = 0;
            m_stTempSectorInfo[ix][iy].iMonsterActivity = 0;
            m_stTempSectorInfo[ix][iy].iPlayerActivity = 0;
        }

    m_iMaxNx = m_iMaxNy = m_iMaxAx = m_iMaxAy = m_iMaxEx = m_iMaxEy = m_iMaxMx = m_iMaxMy = m_iMaxPx = m_iMaxPy = 0;

    for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++)
    {
        m_stStrikePoint[i].dX = 0;
        m_stStrikePoint[i].dY = 0;
        m_stStrikePoint[i].iHP = 0;
        m_stStrikePoint[i].iMapIndex = -1;
        memset(m_stStrikePoint[i].cRelatedMapName, 0, sizeof(m_stStrikePoint[i].cRelatedMapName));
    }
    m_iTotalStrikePoints = 0;
    m_bIsDisabled = false;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
    {
        m_stCrusadeStructureInfo[i].cType = 0;
        m_stCrusadeStructureInfo[i].cSide = 0;
        m_stCrusadeStructureInfo[i].sX = 0;
        m_stCrusadeStructureInfo[i].sY = 0;
    }
    m_iTotalCrusadeStructures = 0;
    m_iTotalAgriculture = 0;

    m_bIsEnergySphereAutoCreation = false;


    sMobEventAmount = 15;
}

CMap::~CMap()
{

    int i;

    if (m_pTile != 0)
        delete[]m_pTile;

    for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
        if (m_pTeleportLoc[i] != 0) delete m_pTeleportLoc[i];

    for (i = 0; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pOccupyFlag[i] != 0) delete m_pOccupyFlag[i];

    for (i = 0; i < DEF_MAXSTRATEGICPOINTS; i++)
        if (m_pStrategicPointList[i] != 0) delete m_pStrategicPointList[i];
}

void CMap::SetOwner(short sOwner, char cOwnerClass, short sX, short sY)
{
    CTile * pTile;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);
    pTile->m_sOwner = sOwner;
    pTile->m_cOwnerClass = cOwnerClass;
}


void CMap::SetDeadOwner(short sOwner, char cOwnerClass, short sX, short sY)
{
    CTile * pTile;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);
    pTile->m_sDeadOwner = sOwner;
    pTile->m_cDeadOwnerClass = cOwnerClass;
}


void CMap::GetOwner(short * pOwner, char * pOwnerClass, short sX, short sY)
{
    CTile * pTile;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY))
    {
        *pOwner = 0;
        *pOwnerClass = 0;
        return;
    }

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);
    *pOwner = pTile->m_sOwner;
    *pOwnerClass = pTile->m_cOwnerClass;

    if (pTile->m_sOwner == 0) *pOwnerClass = 0;
}


void CMap::GetDeadOwner(short * pOwner, char * pOwnerClass, short sX, short sY)
{
    CTile * pTile;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY))
    {
        *pOwner = 0;
        *pOwnerClass = 0;
        return;
    }

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);
    *pOwner = pTile->m_sDeadOwner;
    *pOwnerClass = pTile->m_cDeadOwnerClass;
}


bool CMap::bGetMoveable(short dX, short dY, short * pDOtype, CItem * pTopItem) // v2.172
{
    CTile * pTile;

    if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return false;

    pTile = (CTile *)(m_pTile + dX + dY * m_sSizeY);

    if (pDOtype != 0) *pDOtype = pTile->m_sDynamicObjectType; // v1.4
    if (pTopItem != 0) pTopItem = pTile->m_pItem[0]; // v2.172

    if (pTile->m_sOwner != 0) return false;
    if (pTile->m_bIsMoveAllowed == false) return false;
    if (pTile->m_bIsTempMoveAllowed == false) return false;

    return true;
}

bool CMap::bGetIsMoveAllowedTile(short dX, short dY)
{
    CTile * pTile;

    if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return false;

    pTile = (CTile *)(m_pTile + dX + dY * m_sSizeY);

    if (pTile->m_bIsMoveAllowed == false) return false;
    if (pTile->m_bIsTempMoveAllowed == false) return false;

    return true;
}

bool CMap::bGetIsTeleport(short dX, short dY)
{
    CTile * pTile;

    if ((dX < 14) || (dX >= m_sSizeX - 16) || (dY < 12) || (dY >= m_sSizeY - 14)) return false;

    pTile = (CTile *)(m_pTile + dX + dY * m_sSizeY);

    if (pTile->m_bIsTeleport == false) return false;

    return true;
}

void CMap::ClearOwner(int iDebugCode, short sOwnerH, char cOwnerType, short sX, short sY)
{
    CTile * pTile;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);


    if ((pTile->m_sOwner == sOwnerH) && (pTile->m_cOwnerClass == cOwnerType))
    {
        pTile->m_sOwner = 0;
        pTile->m_cOwnerClass = 0;
    }

    // 
    if ((pTile->m_sDeadOwner == sOwnerH) && (pTile->m_cDeadOwnerClass == cOwnerType))
    {
        pTile->m_sDeadOwner = 0;
        pTile->m_cDeadOwnerClass = 0;
    }
}

void CMap::ClearDeadOwner(short sX, short sY)
{
    CTile * pTile;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);
    pTile->m_sDeadOwner = 0;
    pTile->m_cDeadOwnerClass = 0;
}

bool CMap::bSetItem(short sX, short sY, CItem * pItem)
{
    CTile * pTile;
    int i;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return 0;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);


    if (pTile->m_pItem[DEF_TILE_PER_ITEMS - 1] != 0)
        delete pTile->m_pItem[DEF_TILE_PER_ITEMS - 1];
    else pTile->m_cTotalItem++;

    for (i = DEF_TILE_PER_ITEMS - 2; i >= 0; i--)
        pTile->m_pItem[i + 1] = pTile->m_pItem[i];

    pTile->m_pItem[0] = pItem;
    //pTile->m_cTotalItem++;
    return true;
}

 CItem * CMap::pGetItem(short sX, short sY, short * pRemainItemSprite, short * pRemainItemSpriteFrame, char * pRemainItemColor) //v1.4 color
{
    CTile * pTile;
    CItem * pItem;
    int i;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return 0;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);
    pItem = pTile->m_pItem[0];
    if (pTile->m_cTotalItem == 0) return 0;

    for (i = 0; i <= DEF_TILE_PER_ITEMS - 2; i++)
        pTile->m_pItem[i] = pTile->m_pItem[i + 1];
    pTile->m_cTotalItem--;
    pTile->m_pItem[pTile->m_cTotalItem] = 0;

    if (pTile->m_pItem[0] == 0)
    {
        *pRemainItemSprite = 0;
        *pRemainItemSpriteFrame = 0;
        *pRemainItemColor = 0;
    }
    else
    {
        *pRemainItemSprite = pTile->m_pItem[0]->m_sSprite;
        *pRemainItemSpriteFrame = pTile->m_pItem[0]->m_sSpriteFrame;
        *pRemainItemColor = pTile->m_pItem[0]->m_cItemColor;
    }

    return pItem;
}


int CMap::iCheckItem(short sX, short sY)
{
    CTile * pTile;
    CItem * pItem;

    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return 0;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);
    pItem = pTile->m_pItem[0];
    if (pTile->m_cTotalItem == 0) return 0;

    return pItem->m_sIDnum;
}


bool CMap::bIsValidLoc(short sX, short sY)
{
    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return false;
    return true;
}

bool CMap::bInit(char * pName)
{
    int i;

    memset(m_cName, 0, sizeof(m_cName));
    strcpy(m_cName, pName);

    memset(m_cLocationName, 0, sizeof(m_cLocationName));

    if (_bDecodeMapDataFileContents() == false)
        return false;

    for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
        m_pTeleportLoc[i] = 0;

    return true;
}

bool CMap::_bDecodeMapDataFileContents()
{
    // todo: use stdlib
    HANDLE hFile;
    char  cMapFileName[256]{}, cHeader[260]{}, cTemp[100]{};
    DWORD dwFileSize, nRead;
    int i, ix, iy;
    char * token, cReadMode;
    char seps[] = "= \t\n";
    CStrTok * pStrTok = 0;
    CTile * pTile;
    short * sp;

    memset(cMapFileName, 0, sizeof(cMapFileName));
    strcat(cMapFileName, "mapdata\\");
    strcat(cMapFileName, m_cName);
    strcat(cMapFileName, ".amd");

    hFile = CreateFile(cMapFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    dwFileSize = GetFileSize(hFile, 0);

    memset(cHeader, 0, sizeof(cHeader));
    ReadFile(hFile, (char *)cHeader, 256, &nRead, 0);

    for (i = 0; i < 256; i++)
        if (cHeader[i] == 0) cHeader[i] = ' ';

    cReadMode = 0;

    pStrTok = new CStrTok(cHeader, seps);
    token = pStrTok->pGet();
    //token = strtok( cHeader, seps );   
    while (token != 0)
    {

        if (cReadMode != 0)
        {
            switch (cReadMode)
            {
                case 1:
                    m_sSizeX = atoi(token);
                    cReadMode = 0;
                    break;
                case 2:
                    m_sSizeY = atoi(token);
                    cReadMode = 0;
                    break;
                case 3:
                    m_sTileDataSize = atoi(token);
                    cReadMode = 0;
                    break;
            }
        }
        else
        {
            if (memcmp(token, "MAPSIZEX", 8) == 0) cReadMode = 1;
            if (memcmp(token, "MAPSIZEY", 8) == 0) cReadMode = 2;
            if (memcmp(token, "TILESIZE", 8) == 0) cReadMode = 3;
        }

        //token = strtok( 0, seps );
        token = pStrTok->pGet();
    }

    m_pTile = (CTile *)new CTile[m_sSizeX * m_sSizeY];

    for (iy = 0; iy < m_sSizeY; iy++)
        for (ix = 0; ix < m_sSizeX; ix++)
        {
            ReadFile(hFile, (char *)cTemp, m_sTileDataSize, &nRead, 0);
            pTile = (CTile *)(m_pTile + ix + iy * m_sSizeY);
            if ((cTemp[8] & 0x80) != 0)
            {

                pTile->m_bIsMoveAllowed = false;
            }
            else pTile->m_bIsMoveAllowed = true;

            if ((cTemp[8] & 0x40) != 0)
            {

                pTile->m_bIsTeleport = true;
            }
            else pTile->m_bIsTeleport = false;

            //v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã :: ³óÀÛ¹°À» ½ÉÀ»¼ö ÀÖ´Â °÷ÀÌ´Ù.
            if ((cTemp[8] & 0x20) != 0)
            {

                pTile->m_bIsFarmingAllowed = true;
            }
            else pTile->m_bIsFarmingAllowed = false;

            sp = (short *)&cTemp[0];
            if (*sp == 19)
            {

                pTile->m_bIsWater = true;
            }
            else pTile->m_bIsWater = false;
        }

    CloseHandle(hFile);

    if (pStrTok != 0) delete pStrTok;
    return true;
}


bool CMap::bSearchTeleportDest(int sX, int sY, char * pMapName, int * pDx, int * pDy, char * pDir)
{
    int i;

    for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
        if ((m_pTeleportLoc[i] != 0) && (m_pTeleportLoc[i]->m_sSrcX == sX) && (m_pTeleportLoc[i]->m_sSrcY == sY))
        {

            memcpy(pMapName, m_pTeleportLoc[i]->m_cDestMapName, 10);
            *pDx = m_pTeleportLoc[i]->m_sDestX;
            *pDy = m_pTeleportLoc[i]->m_sDestY;
            *pDir = m_pTeleportLoc[i]->m_cDir;
            return true;
        }

    return false;
}

void CMap::SetDynamicObject(uint16_t wID, short sType, short sX, short sY, uint32_t dwRegisterTime)
{
    CTile * pTile;


    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);

    pTile->m_wDynamicObjectID = wID;
    pTile->m_sDynamicObjectType = sType;
    pTile->m_dwDynamicObjectRegisterTime = dwRegisterTime;
}

bool CMap::bGetDynamicObject(short sX, short sY, short * pType, uint32_t * pRegisterTime, int * pIndex)
{
    CTile * pTile;


    if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return false;

    pTile = (CTile *)(m_pTile + sX + sY * m_sSizeY);

    *pType = pTile->m_sDynamicObjectType;
    *pRegisterTime = pTile->m_dwDynamicObjectRegisterTime;
    if (pIndex != 0) *pIndex = pTile->m_wDynamicObjectID;

    return true;
}

int CMap::iGetEmptyNamingValue()
{
    int i;

    for (i = 0; i < 1000; i++)
        if (m_bNamingValueUsingStatus[i] == false)
        {

            m_bNamingValueUsingStatus[i] = true;
            return i;
        }

    return -1;
}

void CMap::SetNamingValueEmpty(int iValue)
{
    m_bNamingValueUsingStatus[iValue] = false;
}

bool CMap::bGetIsWater(short dX, short dY)
{
    CTile * pTile;

    if ((dX < 14) || (dX >= m_sSizeX - 16) || (dY < 12) || (dY >= m_sSizeY - 14)) return false;

    pTile = (CTile *)(m_pTile + dX + dY * m_sSizeY);

    if (pTile->m_bIsWater == false) return false;

    return true;
}


//v2.19 2002-12-16 ³ó»ç ½ºÅ³ °ü·Ã
bool CMap::bGetIsFarm(short dX, short dY)
{
    CTile * pTile;

    if ((dX < 14) || (dX >= m_sSizeX - 16) || (dY < 12) || (dY >= m_sSizeY - 14)) return false;

    pTile = (CTile *)(m_pTile + dX + dY * m_sSizeY);

    if (pTile->m_bIsFarmingAllowed == false) return false;

    return true;
}


int CMap::iAnalyze(char cType, int * pX, int * pY, int * pV1, int * pV2, int * pV3)
{



    switch (cType)
    {
        case 1:


            break;


    }

    return 0;
}

void CMap::SetTempMoveAllowedFlag(int dX, int dY, bool bFlag)
{
    CTile * pTile;

    if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return;

    pTile = (CTile *)(m_pTile + dX + dY * m_sSizeY);
    pTile->m_bIsTempMoveAllowed = bFlag;
}

int CMap::iRegisterOccupyFlag(int dX, int dY, int iSide, int iEKNum, int iDOI)
{
    int i;

    if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return -1;

    for (i = 1; i < DEF_MAXOCCUPYFLAG; i++)
        if (m_pOccupyFlag[i] == 0)
        {

            m_pOccupyFlag[i] = new COccupyFlag(dX, dY, iSide, iEKNum, iDOI);
            if (m_pOccupyFlag == 0) return -1;
            else return i;
        }

    return -1;
}



void CMap::ClearSectorInfo()
{
    int ix, iy;

    for (ix = 0; ix < DEF_MAXSECTORS; ix++)
        for (iy = 0; iy < DEF_MAXSECTORS; iy++)
        {
            m_stSectorInfo[ix][iy].iNeutralActivity = 0;
            m_stSectorInfo[ix][iy].iAresdenActivity = 0;
            m_stSectorInfo[ix][iy].iElvineActivity = 0;
            m_stSectorInfo[ix][iy].iMonsterActivity = 0;
            m_stSectorInfo[ix][iy].iPlayerActivity = 0;
        }
}

void CMap::ClearTempSectorInfo()
{
    int ix, iy;

    for (ix = 0; ix < DEF_MAXSECTORS; ix++)
        for (iy = 0; iy < DEF_MAXSECTORS; iy++)
        {
            m_stTempSectorInfo[ix][iy].iNeutralActivity = 0;
            m_stTempSectorInfo[ix][iy].iAresdenActivity = 0;
            m_stTempSectorInfo[ix][iy].iElvineActivity = 0;
            m_stTempSectorInfo[ix][iy].iMonsterActivity = 0;
            m_stTempSectorInfo[ix][iy].iPlayerActivity = 0;
        }
}

void CMap::_SetupNoAttackArea()
{
    int i, ix, iy;
    CTile * pTile;

    for (i = 0; i < DEF_MAXNMR; i++)
    {
        if ((m_rcNoAttackRect[i].top > 0))
        {

            for (ix = m_rcNoAttackRect[i].left; ix <= m_rcNoAttackRect[i].right; ix++)
                for (iy = m_rcNoAttackRect[i].top; iy <= m_rcNoAttackRect[i].bottom; iy++)
                {
                    pTile = (CTile *)(m_pTile + ix + iy * m_sSizeY);
                    pTile->m_iAttribute = pTile->m_iAttribute | 0x00000004;
                }
        }
        else if (m_rcNoAttackRect[i].top == -10)
        {

            for (ix = 0; ix < m_sSizeX; ix++)
                for (iy = 0; iy < m_sSizeY; iy++)
                {
                    pTile = (CTile *)(m_pTile + ix + iy * m_sSizeY);
                    pTile->m_iAttribute = pTile->m_iAttribute | 0x00000004;
                }
        }
    }
}

int CMap::iGetAttribute(int dX, int dY, int iBitMask)
{
    CTile * pTile;

    if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return -1;

    pTile = (CTile *)(m_pTile + dX + dY * m_sSizeY);
    return (pTile->m_iAttribute & iBitMask);
}

bool CMap::bAddCrusadeStructureInfo(char cType, short sX, short sY, char cSide)
{
    int i;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
        if (m_stCrusadeStructureInfo[i].cType == 0)
        {
            m_stCrusadeStructureInfo[i].cType = cType;
            m_stCrusadeStructureInfo[i].cSide = cSide;
            m_stCrusadeStructureInfo[i].sX = sX;
            m_stCrusadeStructureInfo[i].sY = sY;

            m_iTotalCrusadeStructures++;
            return true;
        }

    return false;
}

bool CMap::bRemoveCrusadeStructureInfo(short sX, short sY)
{
    int i;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
        if ((m_stCrusadeStructureInfo[i].sX == sX) && (m_stCrusadeStructureInfo[i].sY == sY))
        {
            m_stCrusadeStructureInfo[i].cType = 0;
            m_stCrusadeStructureInfo[i].cSide = 0;
            m_stCrusadeStructureInfo[i].sX = 0;
            m_stCrusadeStructureInfo[i].sY = 0;
            goto RCSI_REARRANGE;
        }

    return false;

    RCSI_REARRANGE:;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES - 1; i++)
        if ((m_stCrusadeStructureInfo[i].cType == 0) && (m_stCrusadeStructureInfo[i + 1].cType != 0))
        {
            m_stCrusadeStructureInfo[i].cType = m_stCrusadeStructureInfo[i + 1].cType;
            m_stCrusadeStructureInfo[i].cSide = m_stCrusadeStructureInfo[i + 1].cSide;
            m_stCrusadeStructureInfo[i].sX = m_stCrusadeStructureInfo[i + 1].sX;
            m_stCrusadeStructureInfo[i].sY = m_stCrusadeStructureInfo[i + 1].sY;

            m_stCrusadeStructureInfo[i + 1].cType = 0;
            m_stCrusadeStructureInfo[i + 1].cSide = 0;
            m_stCrusadeStructureInfo[i + 1].sX = 0;
            m_stCrusadeStructureInfo[i + 1].sY = 0;
        }

    m_iTotalCrusadeStructures--;
    return true;
}


bool CMap::bAddCropsTotalSum()
{
    if (m_iTotalAgriculture < DEF_MAXAGRICULTURE)
    {
        m_iTotalAgriculture++;
        return true;
    }
    return false;
}

bool CMap::bRemoveCropsTotalSum()
{
    if (m_iTotalAgriculture < DEF_MAXAGRICULTURE)
    {
        m_iTotalAgriculture--;
        if (m_iTotalAgriculture < 0)
        {
            m_iTotalAgriculture = 0;
        }
        return true;
    }
    return false;
}

void CMap::RestoreStrikePoints()
{
    int i;

    for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++)
    {
        m_stStrikePoint[i].iHP = m_stStrikePoint[i].iInitHP;
    }
}
