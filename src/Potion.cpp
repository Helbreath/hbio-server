//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Potion.h"
#include <cstring>

CPotion::CPotion()
{
    memset(m_cName, 0, sizeof(m_cName));
    m_iSkillLimit = 0;
    m_iDifficulty = 0;

    for (int i = 0; i < 12; i++)
        m_sArray[i] = -1;
}
