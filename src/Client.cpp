//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Client.h"
#include "streams.h"

CClient::CClient()
{
    int i;

    socknum = 0;
    disconnecttime = steady_clock::now();
    lastpackettime = steady_clock::now();

	ZeroMemory(m_cProfile, sizeof(m_cProfile));
	strcpy(m_cProfile, "__________");

	ZeroMemory(m_cCharName, sizeof(m_cCharName));
	ZeroMemory(m_cAccountName, sizeof(m_cAccountName));
	ZeroMemory(m_cAccountPassword, sizeof(m_cAccountPassword));

	ZeroMemory(m_cGuildName, sizeof(m_cGuildName));
	ZeroMemory(m_cLocation, sizeof(m_cLocation));
	strcpy(m_cLocation, "NONE");
	m_iGuildRank = -1;
	m_iGuildGUID = -1;

	m_bIsInitComplete = FALSE;

	//m_cLU_Str = m_cLU_Int = m_cLU_Vit = m_cLU_Dex = m_cLU_Mag = m_cLU_Char = 0;
	m_iLU_Pool = 0;
	m_cAura = 0;

	//m_iHitRatio_ItemEffect_SM = 0;
	//m_iHitRatio_ItemEffect_L  = 0;
	m_cVar = 0;
	m_iEnemyKillCount = 0;
	m_iPKCount = 0;
	m_iRewardGold = 0;
	m_iCurWeightLoad = 0;
	m_dwLogoutHackCheck = 0;

	m_iAddTransMana = 0;
	m_iAddChargeCritical = 0;

	m_bIsSafeAttackMode  = FALSE;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) 
		m_sItemEquipmentStatus[i] = -1;
	
	for (i = 0; i < DEF_MAXITEMS; i++) {
		m_pItemList[i]       = NULL;
		m_ItemPosList[i].x   = 40;
		m_ItemPosList[i].y   = 30;
		m_bIsItemEquipped[i] = FALSE;
	}
	m_cArrowIndex = -1;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		m_pItemInBankList[i] = NULL;

	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
		m_cMagicMastery[i] = NULL;
	
	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		m_cSkillMastery[i] = NULL;

	for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
		m_bSkillUsingStatus[i] = FALSE;
		m_iSkillUsingTimeID[i] = NULL;
	}

	m_cMapIndex = -1;
	m_sX = -1;
	m_sY = -1;
	m_cDir = 5; 
	m_sType   = 0;
	m_sOriginalType = 0;
	m_sAppr1  = 0;
	m_sAppr2  = 0;
	m_sAppr3  = 0;
	m_sAppr4  = 0;
	m_iApprColor = 0;
	m_iStatus = 0;

	m_cSex  = 0;
	m_cSkin = 0;
	m_cHairStyle  = 0;
	m_cHairColor  = 0;
	m_cUnderwear  = 0;

	m_cAttackDiceThrow_SM = 0;
	m_cAttackDiceRange_SM = 0;
	m_cAttackDiceThrow_L = 0;
	m_cAttackDiceRange_L = 0;
	m_cAttackBonus_SM    = 0;
	m_cAttackBonus_L     = 0;
	
	m_cSide = 0;

	m_iHitRatio = 0;
	m_iDefenseRatio = 0;
	
	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) m_iDamageAbsorption_Armor[i]  = 0;
	m_iDamageAbsorption_Shield = 0;

	m_iHPstock = 0;
	m_bIsKilled = FALSE;

	for (i = 0; i < DEF_MAXMAGICEFFECTS; i++) 
		m_cMagicEffectStatus[i]	= 0;

	m_iWhisperPlayerIndex = -1;
	ZeroMemory(m_cWhisperPlayerName, sizeof(m_cWhisperPlayerName));

	m_iHungerStatus  = 100;
	
	m_bIsWarLocation = FALSE;

	m_bIsPoisoned    = FALSE;
	m_iPoisonLevel   = NULL;

	m_iAdminUserLevel  = 0;
	m_iRating          = 0;
	m_iTimeLeft_ShutUp = 0;
	m_iTimeLeft_Rating = 0;
	m_iTimeLeft_ForceRecall  = 0;
	m_iTimeLeft_FirmStaminar = 0;
	
	m_iRecentWalkTime  = 0;
	m_iRecentRunTime   = 0;
	m_sV1			   = 0;

	m_bIsOnServerChange  = FALSE;
	m_bInhibition = FALSE;

	m_iExpStock = 0;

	m_iAllocatedFish = NULL;
	m_iFishChance    = 0;

	ZeroMemory(m_cIPaddress, sizeof(m_cIPaddress)); 
	m_bIsOnWaitingProcess = FALSE;

	m_iSuperAttackLeft  = 0;
	m_iSuperAttackCount = 0;

	m_sUsingWeaponSkill = 5;

	m_iManaSaveRatio   = 0;
	m_iAddResistMagic  = 0;
	m_iAddPhysicalDamage = 0;
	m_iAddMagicalDamage  = 0;
	m_bIsLuckyEffect     = FALSE;
	m_iSideEffect_MaxHPdown = 0;

	m_iAddAbsAir   = 0;
	m_iAddAbsEarth = 0;
	m_iAddAbsFire  = 0;
	m_iAddAbsWater = 0;

	m_iComboAttackCount = 0;
	m_iDownSkillIndex   = -1;
	m_bInRecallImpossibleMap = 0;

	m_iMagicDamageSaveItemIndex = -1;

	m_sCharIDnum1 = m_sCharIDnum2 = m_sCharIDnum3 = 0;

	m_iPartyID = 0;
	m_iPartyStatus = 0;
	m_iReqJoinPartyClientH = 0;
	ZeroMemory(m_cReqJoinPartyName,sizeof(m_cReqJoinPartyName));

	/*m_iPartyRank = -1;
	m_iPartyMemberCount = 0;
	m_iPartyGUID        = 0;

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMemberName[i].iIndex = 0;
		ZeroMemory(m_stPartyMemberName[i].cName, sizeof(m_stPartyMemberName[i].cName));
	}*/

	m_iAbuseCount     = 0;
	m_bIsBWMonitor    = FALSE;
	m_bIsExchangeMode = FALSE;

	isForceSet = FALSE;

    m_iFightZoneTicketNumber =	m_iFightzoneNumber = m_iReserveTime = 0 ;            

	m_iPenaltyBlockYear = m_iPenaltyBlockMonth = m_iPenaltyBlockDay = 0;

	m_iExchangeH = NULL;
	ZeroMemory(m_cExchangeName, sizeof(m_cExchangeName));
	ZeroMemory(m_cExchangeItemName, sizeof(m_cExchangeItemName));

	for(i=0; i<4; i++){
		m_cExchangeItemIndex[i]  = -1; 
		m_iExchangeItemAmount[i] = 0;
	}

	m_bIsExchangeConfirm = FALSE;

	m_iQuest		 = NULL;
	m_iQuestID       = NULL;
	m_iAskedQuest	 = NULL;
	m_iCurQuestCount = NULL;

	m_iQuestRewardType	 = NULL;
	m_iQuestRewardAmount = NULL;

	m_iContribution = NULL;
	m_bQuestMatchFlag_Loc = FALSE;
	m_bIsQuestCompleted   = FALSE;

	m_cHeroArmorBonus = 0;

	m_bIsNeutral      = FALSE;
	m_bIsObserverMode = FALSE;

	m_iSpecialEventID = 200081;

	m_iSpecialWeaponEffectType  = 0;
	m_iSpecialWeaponEffectValue = 0;

	m_iAddHP = m_iAddSP = m_iAddMP = 0; 
	m_iAddAR = m_iAddPR = m_iAddDR = 0;
	m_iAddAbsPD = m_iAddAbsMD = 0;
	m_iAddCD = m_iAddExp = m_iAddGold = 0;
		
	m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
	m_iSpecialAbilityType = NULL;
	m_bIsSpecialAbilityEnabled = FALSE;
	m_iSpecialAbilityLastSec   = 0;

	m_iSpecialAbilityEquipPos  = 0;

	m_iMoveMsgRecvCount   = 0;
	m_iAttackMsgRecvCount = 0;
	m_iRunMsgRecvCount    = 0;
	m_iSkillMsgRecvCount  = 0;

	m_bIsAdminCommandEnabled = FALSE;
	m_iAlterItemDropIndex = -1;

	m_iAutoExpAmount = 0;
	m_iWarContribution = 0;

	m_dwMoveLAT = m_dwRunLAT = m_dwAttackLAT = 0;

	m_dwInitCCTimeRcv = 0;
	m_dwInitCCTime = 0;

	ZeroMemory(m_cLockedMapName, sizeof(m_cLockedMapName));
	strcpy(m_cLockedMapName, "NONE");
	m_iLockedMapTime = NULL;

	m_iCrusadeDuty  = NULL;
	m_dwCrusadeGUID = NULL;
	m_dwHeldenianGUID = NULL;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
		m_stCrusadeStructureInfo[i].cType = NULL;
		m_stCrusadeStructureInfo[i].cSide = NULL;
		m_stCrusadeStructureInfo[i].sX = NULL;
		m_stCrusadeStructureInfo[i].sY = NULL;
	}

	m_iCSIsendPoint = NULL;

	m_bIsSendingMapStatus = FALSE;
	ZeroMemory(m_cSendingMapName, sizeof(m_cSendingMapName));

	m_iConstructionPoint = NULL;

	ZeroMemory(m_cConstructMapName, sizeof(m_cConstructMapName));
	m_iConstructLocX = m_iConstructLocY = -1;

	m_bIsAdminOrderGoto = FALSE;
	m_bIsInsideWarehouse = FALSE;
	m_bIsInsideWizardTower = FALSE;
	m_bIsInsideOwnTown = FALSE;
	m_bIsCheckingWhisperPlayer = FALSE;
	m_bIsOwnLocation = FALSE;
	m_pIsProcessingAllowed = FALSE;

	m_cHeroArmorBonus = 0;

	m_bIsBeingResurrected = FALSE;
	m_bMagicConfirm = FALSE;
	m_bMagicItem = FALSE;
	m_iSpellCount = 0;
	m_bMagicPauseTime = FALSE;
}

CClient::~CClient()
{
 int i;
	
	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pItemList[i] != NULL) {
			delete m_pItemList[i];
			m_pItemList[i] = NULL;
		}
	for(i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pItemInBankList[i] != NULL) {
			delete m_pItemInBankList[i];
			m_pItemInBankList[i]=NULL;
		}
}

BOOL CClient::bCreateNewParty()
{
 int i;

	if (m_iPartyRank != -1) return FALSE;

	m_iPartyRank = 0;
	m_iPartyMemberCount = 0;
	m_iPartyGUID = (rand() % 999999) + timeGetTime();

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMemberName[i].iIndex = 0;
		ZeroMemory(m_stPartyMemberName[i].cName, sizeof(m_stPartyMemberName[i].cName));
	}

	return TRUE;
}

void CClient::set_disconnected(bool s)
{
    disconnected = s;
    auto conn = get_connection_state();
    if (conn) conn->disconnected = s;
    if (s) set_disconnect_time(now());
}

void CClient::set_connect_time(time_point<steady_clock> t)
{
    connecttime = t;
    auto conn = get_connection_state();
    if (conn) conn->connecttime = t;
    connect_counter++;
}

void CClient::set_disconnect_time(time_point<steady_clock> t)
{
    disconnecttime = t;
    auto conn = get_connection_state();
    if (conn) conn->disconnecttime = t;
    disconnect_counter++;
}

void CClient::set_last_packet_time(time_point<steady_clock> t)
{
    lastpackettime = t;
    auto conn = get_connection_state();
    if (conn) conn->lastpackettime = t;
    packet_counter++;
}

void CClient::set_last_check_time(time_point<steady_clock> t)
{
    lastchecktime = t;
    auto conn = get_connection_state();
    if (conn) conn->lastchecktime = t;
}

void CClient::set_login_time(time_point<steady_clock> t)
{
    logintime = t;
    auto conn = get_connection_state();
    if (conn) conn->logintime = t;
    login_counter++;
}

bool CClient::get_disconnected()
{
    auto conn = get_connection_state();
    if (!conn) return disconnected;
    if (conn->disconnected || disconnected)
        return true;
    return false;
}

time_point<steady_clock> CClient::get_connect_time()
{
    auto conn = get_connection_state();
    if (!conn) return connecttime;
    if (conn->connecttime > connecttime)
        return conn->connecttime;
    return connecttime;
}

time_point<steady_clock> CClient::get_disconnect_time()
{
    auto conn = get_connection_state();
    if (!conn) return disconnecttime;
    if (conn->disconnecttime > disconnecttime)
        return conn->disconnecttime;
    return connecttime;
}

time_point<steady_clock> CClient::get_last_packet_time()
{
    auto conn = get_connection_state();
    if (!conn) return lastpackettime;
    if (conn->lastpackettime > lastpackettime)
        return conn->lastpackettime;
    return lastpackettime;
}

time_point<steady_clock> CClient::get_last_check_time()
{
    auto conn = get_connection_state();
    if (!conn) return lastchecktime;
    if (conn->lastchecktime > lastchecktime)
        return conn->lastchecktime;
    return lastchecktime;
}

time_point<steady_clock> CClient::get_login_time()
{
    auto conn = get_connection_state();
    if (!conn) return logintime;
    if (conn->logintime > logintime)
        return conn->logintime;
    return logintime;
}

// todo - improve this heavily
std::size_t CClient::write(stream_write & sw)
{
    if (connection_state.expired() == true)
        return 0;

    auto connection = connection_state.lock();
    if (connection == nullptr)
        return 0;
    //outgoingqueue.push_back(new StreamWrite(sw));
    connection_state_hb * conn_state = reinterpret_cast<connection_state_hb *>(connection.get());
    ix::IXWebSocketSendData data{ sw.data, sw.position };
    auto ws = conn_state->websocket.lock();
    if (ws) return ws->sendBinary(data).payloadSize;
    return 0;
}
