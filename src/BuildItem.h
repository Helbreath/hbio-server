//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>

class CBuildItem
{
public:
    CBuildItem();
    ~CBuildItem() = default;

    char  m_cName[30];
    short m_sItemID;

    int  m_iSkillLimit;

    int  m_iMaterialItemID[6];
    int  m_iMaterialItemCount[6];
    int  m_iMaterialItemValue[6];
    int  m_iIndex[6];

    int	 m_iMaxValue;
    int  m_iAverageValue;
    int   m_iMaxSkill;
    uint16_t  m_wAttribute;
};
