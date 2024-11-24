//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <windows.h>

class CNpcItem
{
public:
    CNpcItem() = default;
    ~CNpcItem() = default;

    char m_cName[30];
    short m_sItemID;
    short m_sFirstProbability;
    short m_sSecondProbability;
    char m_cFirstTargetValue;
    char m_cSecondTargetValue;

};
