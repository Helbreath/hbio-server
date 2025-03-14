//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>

class CDynamicObject  
{
public:
	CDynamicObject(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, uint32_t dwRegisterTime, uint32_t dwLastTime, int iV1);
	~CDynamicObject() = default;

	short m_sOwner;
	char  m_cOwnerType;

	short m_sType;
	char  m_cMapIndex;
	short m_sX, m_sY; 
	uint32_t m_dwRegisterTime;
    uint32_t m_dwLastTime;

	int   m_iCount;
	int   m_iV1;
};
