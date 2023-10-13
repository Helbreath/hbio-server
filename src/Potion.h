//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <windows.h>

class CPotion  
{
public:
	CPotion();
	~CPotion();

	char  m_cName[30];
	short m_sArray[12];

	int   m_iSkillLimit, m_iDifficulty;

};
