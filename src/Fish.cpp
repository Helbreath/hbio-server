//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "fish.h"

CFish::CFish(char cMapIndex, short sX, short sY, short sType, CItem * pItem, int iDifficulty)
{
    m_cMapIndex = cMapIndex;
    m_sX = sX;
    m_sY = sY;
    m_sType = sType;
    m_pItem = pItem;

    m_sEngagingCount = 0;
    m_iDifficulty = iDifficulty;

    if (m_iDifficulty <= 0)
        m_iDifficulty = 1;

    m_sDynamicObjectHandle = 0;
}

CFish::~CFish()
{
    if (m_pItem != 0) delete m_pItem;
}
