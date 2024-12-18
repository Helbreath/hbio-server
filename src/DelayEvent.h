//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>

#define DEF_DELAYEVENTTYPE_DAMAGEOBJECT				1
#define DEF_DELAYEVENTTYPE_MAGICRELEASE				2
#define DEF_DELAYEVENTTYPE_USEITEM_SKILL			3
#define DEF_DELAYEVENTTYPE_METEORSTRIKE				4
#define DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE		5
#define DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT	6
#define DEF_DELAYEVENTTYPE_ANCIENT_TABLET			7

class CDelayEvent
{
public:
    CDelayEvent() = default;
    ~CDelayEvent() = default;

    int m_iDelayType;
    int m_iEffectType;

    char m_cMapIndex;
    int m_dX, m_dY;

    int  m_iTargetH;
    char m_cTargetType;
    int m_iV1, m_iV2, m_iV3;

    uint32_t m_dwTriggerTime;
};
