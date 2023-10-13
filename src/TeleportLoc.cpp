//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "TeleportLoc.h"

CTeleportLoc::CTeleportLoc()
{

	ZeroMemory(m_cDestMapName, sizeof(m_cDestMapName));
	m_sSrcX   = -1;
	m_sSrcY	  = -1;
	m_sDestX  = -1;								    
	m_sDestY  = -1;
	m_sDestX2 = -1;
	m_sDestY2 = -1;

	m_iV1     = NULL;
	m_iV2     = NULL;
	m_dwTime  = NULL;
	m_dwTime2 = NULL;

}
