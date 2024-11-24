//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#define DEF_SKILLEFFECTTYPE_GET			    1
#define DEF_SKILLEFFECTTYPE_PRETEND			2
#define DEF_SKILLEFFECTTYPE_TAMING			3

class CSkill
{
public:
    CSkill() = default;
    ~CSkill() = default;

    char m_cName[30]{};

    short m_sType{};
    short m_sValue1{}, m_sValue2{}, m_sValue3{}, m_sValue4{}, m_sValue5{}, m_sValue6{};
};
