//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#define DEF_QUESTTYPE_MONSTERHUNT				1
#define DEF_QUESTTYPE_MONSTERHUNT_TIMELIMIT		2
#define DEF_QUESTTYPE_ASSASSINATION 			3
#define DEF_QUESTTYPE_DELIVERY					4
#define DEF_QUESTTYPE_ESCORT					5
#define DEF_QUESTTYPE_GUARD						6
#define DEF_QUESTTYPE_GOPLACE					7 
#define DEF_QUESTTYPE_BUILDSTRUCTURE			8
#define DEF_QUESTTYPE_SUPPLYBUILDSTRUCTURE		9
#define DEF_QUESTTYPE_STRATEGICSTRIKE			10
#define DEF_QUESTTYPE_SENDTOBATTLE				11
#define DEF_QUESTTYPE_SETOCCUPYFLAG				12

class CQuest
{
public:
    CQuest() = default;
    ~CQuest() = default;

    char m_cSide;

    int m_iType;
    int m_iTargetType;
    int m_iMaxCount;

    int m_iFrom;

    int m_iMinLevel;
    int m_iMaxLevel;

    int m_iRequiredSkillNum;
    int m_iRequiredSkillLevel;

    int m_iTimeLimit;
    int m_iAssignType;

    int m_iRewardType[4];
    int m_iRewardAmount[4];

    int m_iContribution;
    int m_iContributionLimit;

    int m_iResponseMode;

    char m_cTargetName[30];
    int  m_sX, m_sY, m_iRange;

    int  m_iQuestID;

    int  m_iReqContribution;
};
