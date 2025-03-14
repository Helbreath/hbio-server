//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "tile.h"

CTile::CTile()
{
    int i;

    m_bIsMoveAllowed = true;
    m_bIsTeleport = false;
    m_bIsWater = false;
    m_bIsFarmingAllowed = false;

    m_sOwner = 0;
    m_cOwnerClass = 0;

    m_sDeadOwner = 0;
    m_cDeadOwnerClass = 0;

    for (i = 0; i < DEF_TILE_PER_ITEMS; i++)
        m_pItem[i] = nullptr;
    m_cTotalItem = 0;

    m_wDynamicObjectID = 0;
    m_sDynamicObjectType = 0;

    m_bIsTempMoveAllowed = true;

    m_iOccupyStatus = 0;
    m_iOccupyFlagIndex = 0;

    m_iAttribute = 0;
}

CTile::~CTile()
{
    for (int i = 0; i < DEF_TILE_PER_ITEMS; i++)
        if (m_pItem[i] != nullptr)
            delete m_pItem[i];
}
