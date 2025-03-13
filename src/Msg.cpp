//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "msg.h"
#include <cstring>

CMsg::CMsg()
{
	m_pData  = nullptr;
	m_dwSize = 0;
}

CMsg::~CMsg()						   
{
	if (m_pData != nullptr) delete[] m_pData;
}

bool CMsg::bPut(char cFrom, char * pData, uint32_t dwSize, int iIndex)
{
	m_pData = new char [dwSize + 1];
	if (m_pData == 0) return false;
	memset(m_pData, 0, dwSize + 1);
	memcpy(m_pData, pData, dwSize);

	m_dwSize = dwSize;
	m_cFrom  = cFrom;
	m_iIndex = iIndex;

	return true;
}

void CMsg::Get(char * pFrom, char * pData, uint32_t * pSize, int * pIndex)
{
	*pFrom  = m_cFrom;
	memcpy(pData, m_pData, m_dwSize);
	*pSize  = m_dwSize;
	*pIndex = m_iIndex;
}
