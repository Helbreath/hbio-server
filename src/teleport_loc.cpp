//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "teleport_loc.h"
#include <cstring>

CTeleportLoc::CTeleportLoc()
{
	memset(m_cDestMapName, 0, sizeof(m_cDestMapName));
	memset(m_cDestMapName2, 0, sizeof(m_cDestMapName2));
	m_sSrcX = -1;
	m_sSrcY = -1;
	m_sDestX = -1;								    
	m_sDestY = -1;
	m_sDestX2 = -1;
	m_sDestY2 = -1;

	m_iV1 = 0;
	m_iV2 = 0;
	m_dwTime = 0;
	m_dwTime2 = 0;

	m_iNumSummonNpc = 0;
}
