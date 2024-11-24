//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include "item.h"

class CFish
{
public:
    CFish(char cMapIndex, short sX, short sY, short sType, CItem * pItem, int iDifficulty);
    ~CFish();

    char  m_cMapIndex;
    short m_sX, m_sY;

    short m_sType;
    CItem * m_pItem;

    short m_sDynamicObjectHandle;

    short m_sEngagingCount;
    int   m_iDifficulty;
};
