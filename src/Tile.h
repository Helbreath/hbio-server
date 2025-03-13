//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>
#include "item.h"

#define DEF_TILE_PER_ITEMS	12

class CTile
{
public:
    CTile();
    ~CTile();

    char m_cOwnerClass{};		// DEF_OT_PLAYER / DEF_OT_NPC
    short m_sOwner{};

    char m_cDeadOwnerClass{};
    short m_sDeadOwner{};

    CItem * m_pItem[DEF_TILE_PER_ITEMS]{};
    char m_cTotalItem{};

    uint16_t m_wDynamicObjectID{};
    short m_sDynamicObjectType{};
    uint32_t m_dwDynamicObjectRegisterTime{};

    bool m_bIsMoveAllowed{}, m_bIsTeleport{}, m_bIsWater{}, m_bIsTempMoveAllowed{};
    bool m_bIsFarmingAllowed{};


    int m_iOccupyStatus{};
    int m_iOccupyFlagIndex{};

    // Crusade
    int m_iAttribute{};


};
