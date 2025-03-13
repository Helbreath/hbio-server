//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>
#include "magic.h"
#include "defines.h"

#define DEF_MAXWAYPOINTS			10

#define DEF_MOVETYPE_STOP			0
#define DEF_MOVETYPE_SEQWAYPOINT	1
#define DEF_MOVETYPE_RANDOMWAYPOINT	2
#define DEF_MOVETYPE_FOLLOW			3
#define DEF_MOVETYPE_RANDOMAREA		4
#define DEF_MOVETYPE_RANDOM			5
#define DEF_MOVETYPE_GUARD			6

#define DEF_BEHAVIOR_STOP			0
#define DEF_BEHAVIOR_MOVE			1
#define DEF_BEHAVIOR_ATTACK			2
#define DEF_BEHAVIOR_FLEE			3
#define DEF_BEHAVIOR_DEAD			4

#define DEF_ATTRIBUTE_EARTH			1
#define DEF_ATTRIBUTE_AIR			2
#define DEF_ATTRIBUTE_FIRE			3
#define DEF_ATTRIBUTE_WATER			4

class CNpcItem
{
public:
	CNpcItem() = default;

	char m_cName[21];
	short m_sItemID;
	short m_sFirstProbability;
	char m_cFirstTargetValue;
	short m_sSecondProbability;
	char m_cSecondTargetValue;
};

class CNpc
{
public:
    CNpc(char * pName5);
    ~CNpc() = default;

    char m_cNpcName[21]{};

    char m_cName[6]{};
    char m_cMapIndex{};
    short m_sX{}, m_sY{};
    short m_dX{}, m_dY{};
    short m_vX{}, m_vY{};
    int m_tmp_iError{};
    hbxrect m_rcRandomArea{};

    char m_cDir{};
    char m_cAction{};
    char m_cTurn{};

    short m_sType{};
    short m_sOriginalType{};
    short m_sAppr2{};
    int m_iStatus{};

    uint32_t m_dwTime{};
    uint32_t m_dwActionTime{};
    uint32_t m_dwHPupTime{}, m_dwMPupTime{};
    uint32_t m_dwDeadTime{}, m_dwRegenTime{};

    int m_iMaxHP{};
    int m_iHP{};
    int m_iExp{};

    int m_iHitDice{};
    int m_iDefenseRatio{};
    int m_iHitRatio{};
    int m_iMagicHitRatio{};
    int m_iMinBravery{};
    int m_iExpDiceMin{};
    int m_iExpDiceMax{};
    int m_iGoldDiceMin{};
    int m_iGoldDiceMax{};

	char m_cSide{};
	char m_cActionLimit{};

    // 0: Small-Medium 1: Large
	char m_cSize{};
	char m_cAttackDiceThrow{};
	char m_cAttackDiceRange{};
	char m_cAttackBonus{};
	char m_cBravery{};
	char m_cResistMagic{};
	char m_cMagicLevel{};
	char m_cDayOfWeekLimit{};
	char m_cChatMsgPresence{};
	int m_iMana{};
	int m_iMaxMana{};

	char m_cMoveType{};
	char m_cBehavior{};
	short m_sBehaviorTurnCount{};
	char m_cTargetSearchRange{};

	int m_iFollowOwnerIndex{};
	char m_cFollowOwnerType{};
	bool m_bIsSummoned{};
	uint32_t m_dwSummonedTime{};

	int m_iTargetIndex{};
	char m_cTargetType{};
	char m_cCurWaypoint{};
	char m_cTotalWaypoint{};

	int m_iSpotMobIndex{};
	int m_iWayPointIndex[DEF_MAXWAYPOINTS+1]{};
	char m_cMagicEffectStatus[DEF_MAXMAGICEFFECTS]{};

	bool m_bIsPermAttackMode{};
	int m_iNoDieRemainExp{};
	int m_iAttackStrategy{};
    /*
        AI-Level
        1:
        2:
        3:
    */
    int m_iAILevel{};
	int m_iAttackRange = 1;

	int m_iAttackCount{};
	bool m_bIsKilled{};
	bool m_bIsUnsummoned{};

	int m_iLastDamage{};
	int m_iSummonControlMode{};
	char m_cAttribute{};
	int m_iAbsDamage{};

	int m_iItemRatio{};
	int m_iAssignedItem{};

	/*
        Special Ability
        case 0: break;
        case 1:  "Penetrating Invisibility"
        case 2:  "Breaking Magic Protection"
        case 3:  "Absorbing Physical Damage"
        case 4:  "Absorbing Magical Damage"
        case 5:  "Poisonous"
        case 6:  "Extremely Poisonous"
        case 7:  "Explosive"
        case 8:  "Hi-Explosive"
	*/
    char m_cSpecialAbility{};

	int m_iBuildCount{};

	int m_iManaStock{};
	bool m_bIsMaster{};
	int m_iGuildGUID{};

	int m_iV1{};

	std::vector<CNpcItem> m_vNpcItem{};

	int m_iNpcItemType{};


	int m_iNpcItemMax{};

	int m_iNpcCrops{};
	int m_iCropsSkillLV{};
};
