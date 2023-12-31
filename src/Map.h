//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <windows.h>
#include "OccupyFlag.h"
#include "Tile.h"
#include "StrategicPoint.h"
#include "defines.h"

class CGame;

#define DEF_OWNERTYPE_PLAYER			1
#define DEF_OWNERTYPE_NPC				2
#define DEF_OWNERTYPE_PLAYER_INDIRECT	3

#define DEF_MAPDATASIZEX		30
#define DEF_MAPDATASIZEY		25
#define DEF_MAXTELEPORTLOC		200
#define DEF_MAXWAYPOINTCFG		200
#define DEF_MAXMGAR				50
#define DEF_MAXNMR				50
#define DEF_MAXSPOTMOBGENERATOR	100
#define DEF_MAXFISHPOINT		200
#define DEF_MAXITEMEVENTS		200
#define DEF_MAXMINERALPOINT		200
#define DEF_MAXHELDENIANDOOR	200
#define DEF_MAXOCCUPYFLAG		20001
#define	DEF_MAXINITIALPOINT		20
#define DEF_MAXAGRICULTURE		200
#define DEF_MAXDYNAMICGATES		10
#define DEF_MAXHELDENIANTOWER	200

#include <windows.h>
#include "Game.h"
#include "StrTok.h"
#include "TeleportLoc.h"

#define DEF_MAPTYPE_NORMAL				0
#define DEF_MAPTYPE_NOPENALTY_NOREWARD	1
#define DEF_MAXENERGYSPHERES			10

#define DEF_MAXSTRATEGICPOINTS			200
#define DEF_MAXSECTORS					60
#define DEF_MAXSTRIKEPOINTS				20

class CMap  
{
public:
    //todo: fix this dumb shit
    void * operator new (size_t size)
    {
        return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    };

    void operator delete(void * mem)
    {
        HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, mem);
    };

    int m_cMapIndex{};

	BOOL bCheckFlySpaceAvailable(short sX, char sY, char cDir, short sOwner);
	BOOL bGetIsFarm(short tX, short tY);
	void RestoreStrikePoints();
	BOOL bRemoveCrusadeStructureInfo(short sX, short sY);
	BOOL bAddCrusadeStructureInfo(char cType, short sX, short sY, char cSide);
	int iGetAttribute(int dX, int dY, int iBitMask);
	void _SetupNoAttackArea();
	void ClearTempSectorInfo();
	void ClearSectorInfo();
	int iRegisterOccupyFlag(int dX, int dY, int iSide, int iEKNum, int iDOI);
	int  iCheckItem(short sX, short sY);
	void SetTempMoveAllowedFlag(int dX, int dY, BOOL bFlag);
	int iAnalyze(char cType, int *pX, int *pY, int * pV1, int *pV2, int * pV3);
	BOOL bGetIsWater(short dX, short dY);
	void GetDeadOwner(short * pOwner, char * pOwnerClass, short sX, short sY);
	BOOL bGetIsMoveAllowedTile(short dX, short dY);
	void SetNamingValueEmpty(int iValue);
	int iGetEmptyNamingValue();
	BOOL bGetDynamicObject(short sX, short sY, short * pType, DWORD * pRegisterTime, int * pIndex = NULL);
	void SetDynamicObject(WORD wID, short sType, short sX, short sY, DWORD dwRegisterTime);
	BOOL bGetIsTeleport(short dX, short dY);
	BOOL bSearchTeleportDest(int sX, int sY, char * pMapName, int * pDx, int * pDy, char * pDir);
	BOOL bInit(char * pName);
	BOOL bIsValidLoc(short sX, short sY);
	CItem * pGetItem(short sX, short sY, short * pRemainItemSprite, short * pRemainItemSpriteFrame, char * pRemainItemColor);
	BOOL bSetItem(short sX, short sY, CItem * pItem);
	void ClearDeadOwner(short sX, short sY);
	void ClearOwner(int iDebugCode, short sOwnerH, char cOwnerType, short sX, short sY);
	BOOL bGetMoveable(short dX, short dY, short * pDOtype = NULL, short * pTopItem = NULL);
	void GetOwner(short * pOwner, char * pOwnerClass, short sX, short sY);
	void SetOwner(short sOwner, char cOwnerClass, short sX, short sY);
	void SetDeadOwner(short sOwner, char cOwnerClass, short sX, short sY);
	BOOL bRemoveCropsTotalSum();
	BOOL bAddCropsTotalSum();
	void SetBigOwner(short sOwner, char cOwnerClass, short sX, short sY, char cArea);

	CMap(CGame * pGame, int map_index);
	~CMap();

	CTile * m_pTile;
	CGame * m_pGame;
	char  m_cName[11];
	char  m_cLocationName[11];
	short m_sSizeX, m_sSizeY, m_sTileDataSize;
	CTeleportLoc * m_pTeleportLoc[DEF_MAXTELEPORTLOC];
	
	//short m_sInitialPointX, m_sInitialPointY;
	POINT m_pInitialPoint[DEF_MAXINITIALPOINT];

	BOOL  m_bNamingValueUsingStatus[1000];
	BOOL  m_bRandomMobGenerator;
	char  m_cRandomMobGeneratorLevel;
	int   m_iTotalActiveObject;
	int   m_iTotalAliveObject;
	int   m_iMaximumObject;

	char  m_cType;

	BOOL  m_bIsFixedDayMode;

	struct {		    
		BOOL bDefined = 0;
		char cType = 0;
		
        char cWaypoint[10] = {};
        RECT rcRect = {};
		
		int  iTotalActiveMob = 0;
		int  iMobType = 0;
		int  iMaxMobs = 0;
		int  iCurMobs = 0;
		
	} m_stSpotMobGenerator[DEF_MAXSPOTMOBGENERATOR];

	POINT m_WaypointList[DEF_MAXWAYPOINTCFG];
	RECT  m_rcMobGenAvoidRect[DEF_MAXMGAR];
	RECT  m_rcNoAttackRect[DEF_MAXNMR];

	POINT m_FishPointList[DEF_MAXFISHPOINT];
	int   m_iTotalFishPoint, m_iMaxFish, m_iCurFish;
	
	int	  m_iApocalypseMobGenType, m_iApocalypseBossMobNpcID;
	short m_sApocalypseBossMobRectX1, m_sApocalypseBossMobRectY1, m_sApocalypseBossMobRectX2, m_sApocalypseBossMobRectY2;
	char  m_cDynamicGateType;
	short m_sDynamicGateCoordRectX1, m_sDynamicGateCoordRectY1, m_sDynamicGateCoordRectX2, m_sDynamicGateCoordRectY2;
	char  m_cDynamicGateCoordDestMap[11];
	short m_sDynamicGateCoordTgtX, m_sDynamicGateCoordTgtY;
	BOOL  m_bIsCitizenLimit;
	short m_sHeldenianTowerType, m_sHeldenianTowerXPos, m_sHeldenianTowerYPos;
	char  m_cHeldenianTowerSide;
	char  m_cHeldenianModeMap;

	BOOL  m_bMineralGenerator;
	char  m_cMineralGeneratorLevel;
	POINT m_MineralPointList[DEF_MAXMINERALPOINT];
	int   m_iTotalMineralPoint, m_iMaxMineral, m_iCurMineral;

	char  m_cWhetherStatus;
	DWORD m_dwWhetherLastTime, m_dwWhetherStartTime;

	int   m_iLevelLimit;
	int   m_iUpperLevelLimit;

	COccupyFlag * m_pOccupyFlag[DEF_MAXOCCUPYFLAG];
	int   m_iTotalOccupyFlags;
	
	CStrategicPoint * m_pStrategicPointList[DEF_MAXSTRATEGICPOINTS];
	BOOL  m_bIsAttackEnabled;

	BOOL  m_bIsFightZone;

	struct {
		char cType;
		int sX, sY;

	} m_stEnergySphereCreationList[DEF_MAXENERGYSPHERES];

	int m_iTotalEnergySphereCreationPoint;
	
	struct {
		char cResult;
		int aresdenX, aresdenY, elvineX, elvineY;
	} m_stEnergySphereGoalList[DEF_MAXENERGYSPHERES];

	int m_iTotalEnergySphereGoalPoint;

	BOOL m_bIsEnergySphereGoalEnabled;
	int m_iCurEnergySphereGoalPointIndex; 

	struct {
		BOOL m_bIsGateMap;
		char m_cDynamicGateMap[11];
		int m_iDynamicGateX;
		int m_iDynamicGateY;
	} m_stDynamicGateCoords[DEF_MAXDYNAMICGATES];

	struct {
		int iPlayerActivity;
		int iNeutralActivity;
		int iAresdenActivity;
		int iElvineActivity;
		int iMonsterActivity;

	} m_stSectorInfo[DEF_MAXSECTORS][DEF_MAXSECTORS], m_stTempSectorInfo[DEF_MAXSECTORS][DEF_MAXSECTORS];
	short sMobEventAmount;
	int m_iTotalItemEvents;
	struct {
		char cItemName[30];
		int iAmount;
		int iTotal;
		int iMonth;
		int iDay;
		int iTotalNum;
	} m_stItemEventList[DEF_MAXITEMEVENTS];

	struct {
		char  cDir;
		short dX;
		short dY;
	} m_stHeldenianGateDoor[DEF_MAXHELDENIANDOOR];

	struct {
		short sTypeID;
		short dX;
		short dY;
		char  cSide;
	} m_stHeldenianTower[DEF_MAXHELDENIANTOWER];

	int m_iMaxNx, m_iMaxNy, m_iMaxAx, m_iMaxAy, m_iMaxEx, m_iMaxEy, m_iMaxMx, m_iMaxMy, m_iMaxPx, m_iMaxPy;
	
	struct {
		char cRelatedMapName[11];
		int iMapIndex;
		int dX, dY;
		int iHP, iInitHP;

		int iEffectX[5];
		int iEffectY[5];
	
	} m_stStrikePoint[DEF_MAXSTRIKEPOINTS];
	int m_iTotalStrikePoints;

	BOOL m_bIsDisabled;
	int m_iTotalAgriculture;

	struct {
		char cType;
		char cSide;
		short sX, sY;
	} m_stCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
	int m_iTotalCrusadeStructures;
	BOOL m_bIsEnergySphereAutoCreation;
private:
	BOOL _bDecodeMapDataFileContents();
public:
	BOOL m_bIsSnowEnabled;
	BOOL m_bIsRecallImpossible;
	BOOL m_bIsApocalypseMap;
	BOOL m_bIsHeldenianMap;
};
