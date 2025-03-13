//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "client.h"
#include "streams.h"
#include "time_utils.h"
#include <ixwebsocket/IXWebSocket.h>

CClient::CClient()
{
    int i;

#ifdef DEF_NOTES
    for (i = 0; i < 20; i++)
        memset(m_stNotesList[i].m_cNotes, 0, 90);
#endif
    m_iMaxHP = 0;

    memset(m_cProfile, 0, sizeof(m_cProfile));
    strcpy(m_cProfile, "__________");

    memset(m_cCharName, 0, sizeof(m_cCharName));
    memset(m_cAccountName, 0, sizeof(m_cAccountName));
    memset(m_cAccountPassword, 0, sizeof(m_cAccountPassword));

    memset(m_cGuildName, 0, sizeof(m_cGuildName));
    memset(m_cLocation, 0, sizeof(m_cLocation));
    memset(m_cMapName, 0, sizeof(m_cMapName));

    strcpy(m_cLocation, "NONE");
    m_iGuildRank = -1;
    m_iGuildGUID = -1;

    m_bIsInitComplete = false;

    //m_cLU_Str = m_cLU_Int = m_cLU_Vit = m_cLU_Dex = m_cLU_Mag = m_cLU_Char = 0;
    m_iLU_Point = 0;
    m_dwLastWalk = m_dwLastRun = m_dwLastMagic = m_dwLastMotion = 0;

    //m_iHitRatio_ItemEffect_SM = 0;
    //m_iHitRatio_ItemEffect_L  = 0;

    m_iEnemyKillCount = 0;
    m_iPKCount = 0;
    m_iRewardGold = 0;
    m_iCurWeightLoad = 0;

    m_bIsSafeAttackMode = false;

    for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
        m_sItemEquipmentStatus[i] = -1;

    for (i = 0; i < DEF_MAXITEMS; i++)
    {
        m_pItemList[i] = nullptr;
        m_ItemPosList[i].x = 40;
        m_ItemPosList[i].y = 30;
        m_bIsItemEquipped[i] = false;
    }
    m_cArrowIndex = -1;

    for (i = 0; i < DEF_MAXBANKITEMS; i++)
    {
        m_pItemInBankList[i] = nullptr;
    }

    for (i = 0; i < DEF_MAXMAGICTYPE; i++)
        m_cMagicMastery[i] = 0;

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
        m_cSkillMastery[i] = 0;

    for (i = 0; i < DEF_MAXSKILLTYPE; i++)
    {
        m_bSkillUsingStatus[i] = false;
        m_iSkillUsingTimeID[i] = 0;
    }

    m_cMapIndex = -1;
    m_sX = -1;
    m_sY = -1;
    m_cDir = 5;
    m_sType = 0;
    m_sOriginalType = 0;
    m_sAppr1 = 0;
    m_sAppr2 = 0;
    m_sAppr3 = 0;
    m_sAppr4 = 0;
    m_iApprColor = 0;
    m_iStatus = 0;

    m_cSex = 0;
    m_cSkin = 0;
    m_cHairStyle = 0;
    m_cHairColor = 0;
    m_cUnderwear = 0;

    m_cAttackDiceThrow_SM = 0;
    m_cAttackDiceRange_SM = 0;
    m_cAttackDiceThrow_L = 0;
    m_cAttackDiceRange_L = 0;
    m_cAttackBonus_SM = 0;
    m_cAttackBonus_L = 0;

    m_cSide = 0;

    m_iHitRatio = 0;
    m_iDefenseRatio = 0;

    for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) m_iDamageAbsorption_Armor[i] = 0;
    m_iDamageAbsorption_Shield = 0;

    m_iHPstock = 0;
    m_iHPStatic_stock = 0;

    m_bIsKilled = false;

    for (i = 0; i < DEF_MAXMAGICEFFECTS; i++)
        m_cMagicEffectStatus[i] = 0;

    m_iWhisperPlayerIndex = -1;
    memset(m_cWhisperPlayerName, 0, sizeof(m_cWhisperPlayerName));

    m_iHungerStatus = 100;

    m_bIsWarLocation = false;

    m_bIsPoisoned = false;
    m_iPoisonLevel = 0;

    m_iAdminUserLevel = 0;
    m_iRating = 0;
    m_iTimeLeft_ShutUp = 0;
    m_iTimeLeft_Rating = 0;
    m_iTimeLeft_ForceRecall = 0;
    m_iTimeLeft_FirmStaminar = 0;

    m_bIsOnServerChange = false;

    m_iExpStock = 0;

    m_iAllocatedFish = 0;
    m_iFishChance = 0;

    memset(m_cIPaddress, 0, sizeof(m_cIPaddress));
    m_bIsOnWaitingProcess = false;

    m_iSuperAttackLeft = 0;
    m_iSuperAttackCount = 0;

    m_sUsingWeaponSkill = 5;

    m_iManaSaveRatio = 0;
    m_iAddResistMagic = 0;
    m_iAddPhysicalDamage = 0;
    m_iAddMagicalDamage = 0;
    m_bIsLuckyEffect = false;
    m_iSideEffect_MaxHPdown = 0;

    m_iAddAbsAir = 0;
    m_iAddAbsEarth = 0;
    m_iAddAbsFire = 0;
    m_iAddAbsWater = 0;

    m_iComboAttackCount = 0;
    m_iDownSkillIndex = -1;

    m_iMagicDamageSaveItemIndex = -1;

    m_sCharIDnum1 = m_sCharIDnum2 = m_sCharIDnum3 = 0;

    m_iPartyRank = -1;
    m_iPartyMemberCount = 0;
    m_iPartyGUID = 0;

    for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
    {
        m_stPartyMemberName[i].iIndex = 0;
        memset(m_stPartyMemberName[i].cName, 0, sizeof(m_stPartyMemberName[i].cName));
    }

    m_iAbuseCount = 0;
    m_bIsExchangeMode = false;

    m_iFightZoneTicketNumber = m_iFightzoneNumber = m_iReserveTime = 0;

    m_iPenaltyBlockYear = m_iPenaltyBlockMonth = m_iPenaltyBlockDay = 0;

    m_iExchangeH = 0;
    memset(m_cExchangeName, 0, sizeof(m_cExchangeName));
    memset(m_cExchangeItemName, 0, sizeof(m_cExchangeItemName));

    for (i = 0; i < 4; i++)
    {
        m_cExchangeItemIndex[i] = -1;
        m_iExchangeItemAmount[i] = 0;
    }

    m_bIsExchangeConfirm = false;



    m_iQuest = 0;
    m_iQuestID = 0;
    m_iAskedQuest = 0;
    m_iCurQuestCount = 0;

    m_iQuestRewardType = 0;
    m_iQuestRewardAmount = 0;

    m_iContribution = 0;
    m_bQuestMatchFlag_Loc = false;
    m_bIsQuestCompleted = false;

    m_bIsNeutral = false;
    m_bIsObserverMode = false;


    m_iSpecialEventID = 200081;

    m_iSpecialWeaponEffectType = 0;
    m_iSpecialWeaponEffectValue = 0;

    m_iAddHP = m_iAddSP = m_iAddMP = 0;
    m_iAddAR = m_iAddPR = m_iAddDR = 0;
    m_iAddAbsPD = m_iAddAbsMD = 0;
    m_iAddCD = m_iAddExp = m_iAddGold = 0;

    m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
    m_iSpecialAbilityType = 0;
    m_bIsSpecialAbilityEnabled = false;
    m_iSpecialAbilityLastSec = 0;

    m_iSpecialAbilityEquipPos = 0;

    m_iMoveMsgRecvCount = 0;
    m_iAttackMsgRecvCount = 0;
    m_iRunMsgRecvCount = 0;
    m_iSkillMsgRecvCount = 0;

    m_bIsAdminCreateItemEnabled = false;
    m_bIsAdminCommandEnabled = false;
    m_iAlterItemDropIndex = -1;

    m_iAutoExpAmount = 0;
    m_iWarContribution = 0;

    m_dwMoveLAT = m_dwRunLAT = m_dwAttackLAT = 0;

    m_dwInitCCTimeRcv = 0;
    m_dwInitCCTime = 0;

    memset(m_cLockedMapName, 0, sizeof(m_cLockedMapName));
    strcpy(m_cLockedMapName, "NONE");
    m_iLockedMapTime = 0;
    m_iDeadPenaltyTime = 0;

    m_iCrusadeDuty = 0;
    m_dwCrusadeGUID = 0;

    for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
    {
        m_stCrusadeStructureInfo[i].cType = 0;
        m_stCrusadeStructureInfo[i].cSide = 0;
        m_stCrusadeStructureInfo[i].sX = 0;
        m_stCrusadeStructureInfo[i].sY = 0;
    }
    m_iCSIsendPoint = 0;

    m_bIsSendingMapStatus = false;
    memset(m_cSendingMapName, 0, sizeof(m_cSendingMapName));

    m_iConstructionPoint = 0;

    memset(m_cConstructMapName, 0, sizeof(m_cConstructMapName));
    m_iConstructLocX = m_iConstructLocY = -1;

    m_dwFightzoneDeadTime = 0;

    m_iPartyID = 0;
    m_iPartyStatus = DEF_PARTYSTATUS_NULL;

    m_iReqJoinPartyClientH = 0;
    memset(m_cReqJoinPartyName, 0, sizeof(m_cReqJoinPartyName));

    m_dwLastActionTime = 0;
    m_bIsCheckingWhisperPlayer = false;


    m_bIsBankModified = false;

    m_iGizonItemUpgradeLeft = 0;

    m_dwAttackFreqTime = m_dwMagicFreqTime = m_dwMoveFreqTime = 0;
    m_bIsMoveBlocked = false;
    m_bIsAttackModeChange = false;
    m_iIsOnTown = false;
    m_bIsOnShop = false;
    m_bIsOnTower = false;
    m_bIsOnWarehouse = false;
    m_bIsHunter = false;
    m_dwWarmEffectTime = 0;
    m_bIsInBuilding = false;

    m_bHeroArmorBonus = 0;
    m_bIsInitComplete2 = false;

    m_bInitComplete = false;
    m_bViewGuildChat = false;
    m_bWhisperView = false;
    memset(m_cWhisperView, 0, 11);
    m_iPlayerMuteCount = 0;
    m_bHasMuted = false;
    memset(m_cLastChat, 0, 130);
    m_bHasFakeName = 0;
    memset(m_cFakeName, 0, 11);
    m_iAntiHackLevel = 0;
    m_bIsQuiet = 0;

    for (i = 0; i < 50; i++)
    {
        memset(m_stFriendsList[i].m_cCharName, 0, sizeof(m_stFriendsList[i].m_cCharName));
        memset(m_stFriendsList[i].m_cMapName, 0, sizeof(m_stFriendsList[i].m_cMapName));
        m_stFriendsList[i].m_cSide = 0;
        m_stFriendsList[i].m_bUsed = 0;
    }

    m_bIsInitComplete2 = false;
    m_bInhibition = false;
    m_bEnabled = false;
}

CClient::~CClient()
{
    int i;

    for (i = 0; i < DEF_MAXITEMS; i++)
        if (m_pItemList[i] != nullptr)
        {
            delete m_pItemList[i];
            m_pItemList[i] = nullptr;
        }
    for (i = 0; i < DEF_MAXBANKITEMS; i++)
        if (m_pItemInBankList[i] != nullptr)
        {
            delete m_pItemInBankList[i];
            m_pItemInBankList[i] = nullptr;
        }
}

bool CClient::bCreateNewParty()
{
    int i;

    if (m_iPartyRank != -1) return false;

    m_iPartyRank = 0;
    m_iPartyMemberCount = 0;
    m_iPartyGUID = (rand() % 999999) + timeGetTime();

    for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
    {
        m_stPartyMemberName[i].iIndex = 0;
        memset(m_stPartyMemberName[i].cName, 0, sizeof(m_stPartyMemberName[i].cName));
    }

    return true;
}

void CClient::set_connected(bool s)
{
    connected = s;
    if (s) set_connect_time(now());
    else set_disconnect_time(now());
}

void CClient::set_connect_time(time_point<system_clock> t)
{
    connecttime = t;
    connect_counter++;
}

void CClient::set_disconnect_time(time_point<system_clock> t)
{
    disconnecttime = t;
    disconnect_counter++;
}

void CClient::set_last_packet_time(time_point<system_clock> t)
{
    lastpackettime = t;
    packet_counter++;
}

void CClient::set_last_check_time(time_point<system_clock> t)
{
    lastchecktime = t;
}

void CClient::set_login_time(time_point<system_clock> t)
{
    logintime = t;
    login_counter++;
}

bool CClient::get_connected()
{
    return connected;
}

time_point<system_clock> CClient::get_connect_time()
{
    return connecttime;
}

time_point<system_clock> CClient::get_disconnect_time()
{
    return disconnecttime;
}

time_point<system_clock> CClient::get_last_packet_time()
{
    return lastpackettime;
}

time_point<system_clock> CClient::get_last_check_time()
{
    return lastchecktime;
}

time_point<system_clock> CClient::get_login_time()
{
    return logintime;
}

// todo - improve this heavily
std::size_t CClient::write(stream_write & sw)
{
    //outgoingqueue.push_back(new StreamWrite(sw));
    ix::IXWebSocketSendData data{ sw.data, sw.position };
    auto ws = websocket.lock();
    if (ws) return ws->sendBinary(data).payloadSize;
    return 0;
}
