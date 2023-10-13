//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <windows.h>
#include "Item.h"

#define DEF_TILE_PER_ITEMS	12

class CTile  
{												  
public:
	CTile();
	~CTile();

    //todo: fix this dumb shit
    void * operator new (size_t size)
    {
        return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    };

    void operator delete(void * mem)
    {
        HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, mem);
    };

	char  m_cOwnerClass;
	short m_sOwner;

	char  m_cDeadOwnerClass;
	short m_sDeadOwner;

	CItem * m_pItem[DEF_TILE_PER_ITEMS];
	char  m_cTotalItem;

	WORD  m_wDynamicObjectID;
	short m_sDynamicObjectType;
	DWORD m_dwDynamicObjectRegisterTime;

	BOOL  m_bIsMoveAllowed, m_bIsTeleport, m_bIsWater, m_bIsFarm, m_bIsTempMoveAllowed;

	int   m_iOccupyStatus;
	int   m_iOccupyFlagIndex;

	int	  m_iAttribute;
};
