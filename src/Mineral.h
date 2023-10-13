//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

class CMineral  
{
public:
	CMineral(char cType, char cMapIndex, int sX, int sY, int iRemain);
	~CMineral() = default;
	
	char  m_cType;

	char  m_cMapIndex;
	int   m_sX, m_sY;
	int   m_iDifficulty;
	short m_sDynamicObjectHandle;
	
	int   m_iRemain;
};
