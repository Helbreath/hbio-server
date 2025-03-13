//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include "occupy_flag.h"
#include "tile.h"
#include "strategic_point.h"
#include "game.h"
#include "str_tok.h"
#include "teleport_loc.h"
#include "global_def.h"
#include "defines.h"

class CMap
{
public:
    int m_cMapIndex{};

    void RestoreStrikePoints();
    bool bRemoveCrusadeStructureInfo(short sX, short sY);
    bool bAddCrusadeStructureInfo(char cType, short sX, short sY, char cSide);
    int iGetAttribute(int dX, int dY, int iBitMask);
    void _SetupNoAttackArea();
    void ClearTempSectorInfo();
    void ClearSectorInfo();
    int iRegisterOccupyFlag(int dX, int dY, int iSide, int iEKNum, int iDOI);
    int  iCheckItem(short sX, short sY);
    void SetTempMoveAllowedFlag(int dX, int dY, bool bFlag);
    int iAnalyze(char cType, int * pX, int * pY, int * pV1, int * pV2, int * pV3);
    bool bGetIsWater(short dX, short dY);
    bool bGetIsFarm(short dX, short dY);
    bool bAddCropsTotalSum();
    bool bRemoveCropsTotalSum();
    void GetDeadOwner(short * pOwner, char * pOwnerClass, short sX, short sY);
    bool bGetIsMoveAllowedTile(short dX, short dY);
    void SetNamingValueEmpty(int iValue);
    int iGetEmptyNamingValue();
    bool bGetDynamicObject(short sX, short sY, short * pType, uint32_t * pRegisterTime, int * pIndex = 0);
    void SetDynamicObject(uint16_t wID, short sType, short sX, short sY, uint32_t dwRegisterTime);
    bool bGetIsTeleport(short dX, short dY);
    bool bSearchTeleportDest(int sX, int sY, char * pMapName, int * pDx, int * pDy, char * pDir);
    bool bInit(char * pName);
    bool bIsValidLoc(short sX, short sY);
    CItem * pGetItem(short sX, short sY, short * pRemainItemSprite, short * pRemainItemSpriteFrame, char * pRemainItemColor);
    bool bSetItem(short sX, short sY, CItem * pItem);
    void ClearDeadOwner(short sX, short sY);
    void ClearOwner(int iDebugCode, short sOwnerH, char cOwnerType, short sX, short sY);
    bool bGetMoveable(short dX, short dY, short * pDOtype = 0, CItem * pTopItem = 0); // v2.172
    void GetOwner(short * pOwner, char * pOwnerClass, short sX, short sY);
    void SetOwner(short sOwner, char cOwnerClass, short sX, short sY);
    void SetDeadOwner(short sOwner, char cOwnerClass, short sX, short sY);
    CMap(CGame * pGame, int map_index);
    ~CMap();

    CTile * m_pTile;
    CGame * m_pGame;
    char  m_cName[11];
    char  m_cLocationName[11];
    short m_sSizeX, m_sSizeY, m_sTileDataSize;
    CTeleportLoc * m_pTeleportLoc[DEF_MAXTELEPORTLOC];

    //short m_sInitialPointX, m_sInitialPointY;
    hbxpoint m_pInitialPoint[DEF_MAXINITIALPOINT];

    bool m_bNamingValueUsingStatus[1000];
    bool m_bRandomMobGenerator;
    char m_cRandomMobGeneratorLevel;
    int m_iTotalActiveObject;
    int m_iTotalAliveObject;
    int m_iMaximumObject;

    char m_cType;

    bool m_bIsFixedDayMode;
    bool m_bIsFixedSnowMode;
    bool m_bIsSnowEnabled;
    bool m_bIsRecallImpossible;
    bool m_bIsApocalypseMap;
    bool m_bIsHeldenianMap;

    struct
    {
        bool bDefined;
        char cType;				// 1:RANDOMAREA   2:RANDOMWAYPOINT

        char cWaypoint[10];
        hbxrect rcRect;

        int iTotalActiveMob;
        int iMobType;
        int iMaxMobs;
        int iCurMobs;

    } m_stSpotMobGenerator[DEF_MAXSPOTMOBGENERATOR];

    hbxpoint m_WaypointList[DEF_MAXWAYPOINTCFG];
    hbxrect m_rcMobGenAvoidRect[DEF_MAXMGAR];
    hbxrect m_rcNoAttackRect[DEF_MAXNMR];

    hbxpoint m_FishPointList[DEF_MAXFISHPOINT];
    int m_iTotalFishPoint, m_iMaxFish, m_iCurFish;

    int m_iApocalypseMobGenType, m_iApocalypseBossMobNpcID;
    short m_sApocalypseBossMobRectX1, m_sApocalypseBossMobRectY1, m_sApocalypseBossMobRectX2, m_sApocalypseBossMobRectY2;
    char m_cDynamicGateType;
    short m_sDynamicGateCoordRectX1, m_sDynamicGateCoordRectY1, m_sDynamicGateCoordRectX2, m_sDynamicGateCoordRectY2;
    char m_cDynamicGateCoordDestMap[11];
    short m_sDynamicGateCoordTgtX, m_sDynamicGateCoordTgtY;
    bool m_bIsCitizenLimit;
    short m_sHeldenianTowerType, m_sHeldenianTowerXPos, m_sHeldenianTowerYPos;
    char m_cHeldenianTowerSide;
    char m_cHeldenianModeMap;

    bool m_bMineralGenerator;
    char m_cMineralGeneratorLevel;
    hbxpoint m_MineralPointList[DEF_MAXMINERALPOINT];
    int m_iTotalMineralPoint, m_iMaxMineral, m_iCurMineral;

    char m_cWhetherStatus;
    uint32_t m_dwWhetherLastTime, m_dwWhetherStartTime;

    int m_iLevelLimit;
    int m_iUpperLevelLimit;

    COccupyFlag * m_pOccupyFlag[DEF_MAXOCCUPYFLAG];
    int m_iTotalOccupyFlags;

    CStrategicPoint * m_pStrategicPointList[DEF_MAXSTRATEGICPOINTS];
    bool m_bIsAttackEnabled;

    bool m_bIsFightZone;

    struct
    {
        char cType;
        int sX, sY;
    } m_stEnergySphereCreationList[DEF_MAXENERGYSPHERES];

    int m_iTotalEnergySphereCreationPoint;

    struct
    {
        char cResult;
        int aresdenX, aresdenY, elvineX, elvineY;
    } m_stEnergySphereGoalList[DEF_MAXENERGYSPHERES];

    int m_iTotalEnergySphereGoalPoint;

    bool m_bIsEnergySphereGoalEnabled;
    int m_iCurEnergySphereGoalPointIndex;


    struct
    {
        int iPlayerActivity;
        int iNeutralActivity;
        int iAresdenActivity;
        int iElvineActivity;
        int iMonsterActivity;

    } m_stSectorInfo[DEF_MAXSECTORS][DEF_MAXSECTORS], m_stTempSectorInfo[DEF_MAXSECTORS][DEF_MAXSECTORS];

    int m_iMaxNx, m_iMaxNy, m_iMaxAx, m_iMaxAy, m_iMaxEx, m_iMaxEy, m_iMaxMx, m_iMaxMy, m_iMaxPx, m_iMaxPy;

    struct
    {
        char cRelatedMapName[11];
        int iMapIndex;
        int dX, dY;
        int iHP, iInitHP;

        int iEffectX[5];
        int iEffectY[5];

    } m_stStrikePoint[DEF_MAXSTRIKEPOINTS];
    int m_iTotalStrikePoints;

    bool m_bIsDisabled;

    struct
    {
        char cType;
        char cSide;
        short sX, sY;
    } m_stCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
    int m_iTotalCrusadeStructures;
    int m_iTotalAgriculture;

    struct
    {
        char cItemName[21];
        int iAmount;
        int iTotalNum;
        int iMonth;
        int iDay;
        int iType;
        char * cMob[5];

        int iCurNum;
        int	iNumMob;
    } m_stItemEventList[DEF_MAXITEMEVENTS];
    int m_iTotalItemEvents;

    struct
    {
        char  cDir;
        short dX;
        short dY;
    } m_stHeldenianGateDoor[DEF_MAXHELDENIANDOOR];

    struct
    {
        short sTypeID;
        short dX;
        short dY;
        char  cSide;
    } m_stHeldenianTower[DEF_MAXHELDENIANTOWER];

    bool m_bIsEnergySphereAutoCreation;

    short sMobEventAmount;

private:
    bool _bDecodeMapDataFileContents();
};
