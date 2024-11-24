//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

/*
[CONFIG]

//---------NUM--NPC_NAME--------SOURCE_MAP------TARGET_MAP------X-------Y-------COST----MinLvl--MaxLvl--Side----bHunt---bNtrl---bCrmnl--
teleport = 1	William		cityhall_1	middleland	200	200	10000	30	100	aresden	1	1	0
teleport = 2	William		cityhall_2	middleland	200	200	10000	30	100	elvine	1	1	0

[END]

Side   : all, aresden, elvine
bHunt  : (0,1)
bNtrl  : (0,1)
bCrmnl : (0,1)
*/

class CTeleport
{
public:
    CTeleport();
    ~CTeleport() = default;

    char m_cNpcname[21]{};
    char m_cSourceMap[11]{};
    char m_cTargetMap[11]{};
    int m_iX{}, m_iY{};
    int m_iCost{};
    int m_iMinLvl{};
    int m_iMaxLvl{};
    int m_iSide{};
    bool m_bHunter{};
    bool m_bNetural{};
    bool m_bCriminal{};
};
