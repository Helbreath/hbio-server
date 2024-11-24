//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>

#define DEF_MSGFROM_CLIENT		1

class CMsg  								 
{
public:

	void Get(char * pFrom, char * pData, uint32_t * pSize, int * pIndex);
	bool bPut(char cFrom, char * pData, uint32_t dwSize, int iIndex);
	CMsg();
	~CMsg();

	char   m_cFrom;

	char * m_pData;
    uint32_t  m_dwSize;

	int    m_iIndex;
};
