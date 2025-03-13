//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

class COccupyFlag
{
public:
    COccupyFlag(int dX, int dY, char cSide, int iEKNum, int iDOI);
    ~COccupyFlag() = default;

    char m_cSide;
    int  m_iEKCount;
    int  m_sX, m_sY;

    int  m_iDynamicObjectIndex;
};
