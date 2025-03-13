//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "teleport.h"
#include <cstring>

CTeleport::CTeleport()
{
    memset(m_cNpcname, 0, sizeof(m_cNpcname));
    memset(m_cSourceMap, 0, sizeof(m_cSourceMap));
    memset(m_cTargetMap, 0, sizeof(m_cTargetMap));

    m_iX = m_iY = -5;
    m_iCost = 0;
    m_iMinLvl = 0;
    m_iMaxLvl = 0;
    m_iSide = 0;
    m_bHunter = true;
    m_bNetural = true;
    m_bCriminal = true;
}

