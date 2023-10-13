//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <windows.h>

class CTeleportLoc  
{
public:
	CTeleportLoc(); 
	~CTeleportLoc() = default;
												  
	short m_sSrcX, m_sSrcY;

	char  m_cDestMapName[11],  m_cDir;
	char  m_cDestMapName2[11];
	short m_sDestX,  m_sDestY;
	short m_sDestX2, m_sDestY2;

	int   m_iV1, m_iV2;
	DWORD m_dwTime, m_dwTime2;

};
