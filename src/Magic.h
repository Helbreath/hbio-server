//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>

#define DEF_MAGICTYPE_DAMAGE_SPOT			1
#define DEF_MAGICTYPE_HPUP_SPOT				2
#define DEF_MAGICTYPE_DAMAGE_AREA			3
#define DEF_MAGICTYPE_SPDOWN_SPOT			4
#define DEF_MAGICTYPE_SPDOWN_AREA			5
#define DEF_MAGICTYPE_SPUP_SPOT				6
#define DEF_MAGICTYPE_SPUP_AREA				7
#define DEF_MAGICTYPE_TELEPORT				8
#define DEF_MAGICTYPE_SUMMON				9
#define DEF_MAGICTYPE_CREATE				10
#define DEF_MAGICTYPE_PROTECT				11
#define DEF_MAGICTYPE_HOLDOBJECT			12
#define DEF_MAGICTYPE_INVISIBILITY			13
#define DEF_MAGICTYPE_CREATE_DYNAMIC		14
#define DEF_MAGICTYPE_POSSESSION			15
#define DEF_MAGICTYPE_CONFUSE				16
#define DEF_MAGICTYPE_POISON				17
#define DEF_MAGICTYPE_BERSERK				18
#define DEF_MAGICTYPE_DAMAGE_LINEAR			19
#define DEF_MAGICTYPE_POLYMORPH				20
#define DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT		21
#define DEF_MAGICTYPE_TREMOR					22
#define DEF_MAGICTYPE_ICE						23
#define DEF_MAGICTYPE_DAMAGE_SPOT_SPDOWN		24
#define DEF_MAGICTYPE_DAMAGE_AREA_NOSPOT_SPDOWN	25
#define DEF_MAGICTYPE_DAMAGE_AREA_ARMOR_BREAK	26
#define DEF_MAGICTYPE_ICE_LINEAR				27

#define DEF_MAGICTYPE_CANCELLATION				29
#define DEF_MAGICTYPE_DAMAGE_LINEAR_SPDOWN		30
#define DEF_MAGICTYPE_INHIBITION				31
#define DEF_MAGICTYPE_RESURRECTION				32
#define DEF_MAXMAGICEFFECTS			100

class CMagic
{
public:
    CMagic() = default;
    ~CMagic() = default;

    char m_cName[31];

    short m_sType;
    uint32_t m_dwDelayTime, m_dwLastTime;
    short m_sValue1, m_sValue2, m_sValue3, m_sValue4, m_sValue5, m_sValue6;
    short m_sValue7, m_sValue8, m_sValue9, m_sValue10, m_sValue11, m_sValue12;
    short m_sIntLimit;
    int   m_iGoldCost;

    char  m_cCategory;
    int   m_iAttribute;
};
