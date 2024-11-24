//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

class CStrategicPoint
{
public:
    CStrategicPoint() = default;
    ~CStrategicPoint() = default;

    int m_iSide{};
    int m_iValue{};
    int m_iX{}, m_iY{};
};
