//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <direct.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory.h>
#include <vector>
#include <format>

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <ixwebsocket/IXWebSocketServer.h>
#include <pqxx/pqxx>
#include <pqxx/except>
#include <shared_mutex>
#include <deque>
#include <string_view>
#include "socket_defines.h"

#include "str_tok.h"
#include "client.h"
#include "npc.h"
#include "action_id.h"
#include "net_messages.h"
#include "message_index.h"
#include "misc.h"
#include "msg.h"
#include "magic.h"
#include "skill.h"
#include "delay_event.h"
#include "fish.h"
#include "dynamic_object.h"
#include "dynamic_object_id.h"
#include "potion.h"
#include "mineral.h"
#include "quest.h"
#include "build_item.h"
#include "teleport_loc.h"
#include "global_def.h"
#include "englishitem.h"
#include "koreaitem.h"
#include "teleport.h"

#define DEF_MAXBANNED 500
#define DEF_MAXADMINS				50
#define DEF_MAXCLIENTS			2000
#define DEF_MAXNPCS				5000
#define DEF_MAXMAPS				100
#define DEF_MAXITEMTYPES		5000
#define DEF_MAXNPCTYPES			200//100
#define DEF_MAXBUILDITEMS		300
#define DEF_CLIENTTIMEOUT		3000*10		//(10ÃÊ)
#define DEF_AUTOSAVETIME		60000//60000*30
#define DEF_HPUPTIME			1000*15
#define DEF_MPUPTIME			1000*20
#define DEF_SPUPTIME			1000*10

#define DEF_HUNGERTIME			1000*60
#define DEF_POISONTIME			1000*12
#define DEF_SUMMONTIME			60000*5
#define DEF_NOTICETIME			80000
#define DEF_PLANTTIME			60000*5		//v2.20 2002-12-20 ³óÀÛ¹° »ì¾Æ ÀÖ´Â ½Ã°£.

#define DEF_EXPSTOCKTIME		1000*10
#define DEF_MSGQUENESIZE		100000
#define DEF_AUTOEXPTIME			1000*60*6
#define DEF_TOTALLEVELUPPOINT	3


#define DEF_MAXDYNAMICOBJECTS	60000
#define DEF_MAXDELAYEVENTS		60000
#define DEF_GUILDSTARTRANK		12

#define DEF_SSN_LIMIT_MULTIPLY_VALUE	2

#define DEF_MAXNOTIFYMSGS		1000
#define DEF_MAXSKILLPOINTS		10000//700
#define DEF_NIGHTTIME			40

#define DEF_CHARPOINTLIMIT		200//200
#define DEF_RAGPROTECTIONTIME	7000
#define DEF_MAXREWARDGOLD		99999999

#define DEF_ATTACKAI_NORMAL				1
#define DEF_ATTACKAI_EXCHANGEATTACK		2
#define DEF_ATTACKAI_TWOBYONEATTACK		3

#define DEF_MAXFISHS					200
#define DEF_MAXMINERALS					200
#define DEF_MAXENGAGINGFISH				30
#define DEF_MAXPORTIONTYPES				500

#define DEF_MOBEVENTTIME				300000 // 5ºÐ 
#define DEF_MAXQUESTTYPE				200

#define DEF_MAXSUBLOGSOCK				10

#define DEF_ITEMLOG_GIVE				1
#define DEF_ITEMLOG_DROP				2
#define DEF_ITEMLOG_GET					3
#define DEF_ITEMLOG_DEPLETE				4
#define DEF_ITEMLOG_NEWGENDROP			5
#define DEF_ITEMLOG_DUPITEMID			6
#define DEF_ITEMLOG_BUY					7
#define DEF_ITEMLOG_SELL				8     
#define DEF_ITEMLOG_RETRIEVE			9
#define DEF_ITEMLOG_DEPOSIT				10
#define DEF_ITEMLOG_EXCHANGE			11
#define DEF_ITEMLOG_MAGICLEARN			12
#define DEF_ITEMLOG_MAKE				13
#define DEF_ITEMLOG_SUMMONMONSTER		14
#define DEF_ITEMLOG_POISONED			15
#define DEF_ITEMLOG_SKILLLEARN			16
#define DEF_ITEMLOG_REPAIR				17
#define DEF_ITEMLOG_JOINGUILD           18
#define DEF_ITEMLOG_BANGUILD            19
#define DEF_ITEMLOG_RESERVEFIGZONE      20	//  "
#define DEF_ITEMLOG_APPLY               21	//  "
#define DEF_ITEMLOG_SHUTUP              22	//  "
#define DEF_ITEMLOG_CLOSECONN			23	//  "
#define DEF_ITEMLOG_SPELLFIELD			24	//  "
#define DEF_ITEMLOG_CREATEGUILD			25	//  "
#define DEF_ITEMLOG_GUILDDISMISS		26	//  "
#define DEF_ITEMLOG_SUMMONPLAYER        27	//  "
#define DEF_ITEMLOG_CREATE				28	//  "
#define DEF_ITEMLOG_UPGRADEFAIL         29
#define DEF_ITEMLOG_UPGRADESUCCESS      30


// v2.15 Àü¸éÀü ·Î±× °ü·Ã 
#define DEF_CRUSADELOG_ENDCRUSADE       1
#define DEF_CRUSADELOG_STARTCRUSADE     2
#define DEF_CRUSADELOG_SELECTDUTY       3
#define DEF_CRUSADELOG_GETEXP           4

#define DEF_MAXDUPITEMID				100

#define DEF_MAXGUILDS					1000
#define DEF_MAXONESERVERUSERS			800

#define DEF_MAXGATESERVERSTOCKMSGSIZE	30000


#define DEF_MAXCONSTRUCTNUM				10

#define DEF_MAXSCHEDULE					10


#define DEF_MAXFIGHTZONE 10 

//============================
// #define DEF_LEVELLIMIT		130
#define DEF_LEVELLIMIT		20
//============================

#define DEF_MINIMUMHITRATIO 15
#define DEF_MAXIMUMHITRATIO	99

#define DEF_PLAYERMAXLEVEL	180

#define DEF_GMGMANACONSUMEUNIT	15

#define DEF_MAXCONSTRUCTIONPOINT 30000

#define DEF_MAXWARCONTRIBUTION	 500000

#define DEF_MAXPARTYNUM			5000

#define DEF_MAXGIZONPOINT		300

#define DEF_NETURAL             0
#define DEF_ARESDEN             1
#define DEF_ELVINE              2
#define DEF_BOTHSIDE			100

#define DEF_PK					0
#define DEF_NONPK				1
#define DEF_NEVERNONPK			2

#define DEF_MAX_CRUSADESUMMONMOB	500

#define DEF_LIMITHUNTERLEVEL 100

#define DEF_STR 0x01
#define DEF_DEX 0x03
#define DEF_INT 0x04
#define DEF_VIT 0x02
#define DEF_MAG 0x05
#define DEF_CHR 0x06

#define DEF_XSOCKEVENT_SOCKETMISMATCH			-121
#define DEF_XSOCKEVENT_CONNECTIONESTABLISH		-122
#define DEF_XSOCKEVENT_RETRYINGCONNECTION		-123
#define DEF_XSOCKEVENT_ONREAD					-124
#define DEF_XSOCKEVENT_READCOMPLETE				-125
#define DEF_XSOCKEVENT_UNKNOWN					-126
#define DEF_XSOCKEVENT_SOCKETCLOSED				-127
#define DEF_XSOCKEVENT_BLOCK					-128
#define DEF_XSOCKEVENT_SOCKETERROR				-129
#define DEF_XSOCKEVENT_CRITICALERROR			-130
#define DEF_XSOCKEVENT_NOTINITIALIZED			-131
#define DEF_XSOCKEVENT_MSGSIZETOOLARGE			-132
#define DEF_XSOCKEVENT_CONFIRMCODENOTMATCH		-133
#define DEF_XSOCKEVENT_QUENEFULL                -134
#define DEF_XSOCKEVENT_UNSENTDATASENDBLOCK		-135
#define DEF_XSOCKEVENT_UNSENTDATASENDCOMPLETE	-136

enum class server_status
{
    uninitialized = 0,
    running,
    shutdown,
    offline
};

enum class game_server_status
{
    uninitialized = 0,
    running,
    running_full,
    shutdown,
    offline
};

enum class login_server_status
{
    uninitialized = 0,
    running,
    running_queue,
    shutdown,
    offline
};

class CMap;

class CGame
{
public:
    std::vector<spdlog::sink_ptr> sinks;
    std::shared_ptr<spdlog::logger> log;

    std::unique_ptr<ix::WebSocketServer> server;
    std::set<std::pair<std::shared_ptr<ix::WebSocket>, std::shared_ptr<CClient>>> websocket_clients;
    std::recursive_mutex websocket_list;

    spdlog::level::level_enum loglevel = spdlog::level::level_enum::info;
    std::string log_formatting;
    bool state_valid = true;
    bool config_loaded = false;

    std::string world_name;

    std::string login_sqluser;
    std::string login_sqlpass;
    std::string login_sqldb;
    std::string login_sqlhost;
    uint16_t login_sqlport;

    std::string game_sqluser;
    std::string game_sqlpass;
    std::string game_sqldb;
    std::string game_sqlhost;
    uint16_t game_sqlport;

    std::string login_auth_url;
    std::string login_auth_key;

    std::string bindip;
    uint16_t bindport;

    std::unique_ptr<pqxx::connection> pq_login;
    std::unique_ptr<pqxx::connection> pq_game;
    std::shared_mutex login_sql_mtx;
    std::shared_mutex game_sql_mtx;

    void load_config();
    void run();
    void auto_save(std::vector<character_db> char_data);
    character_db build_character_data_for_save(std::shared_ptr<CClient> client);

    server_status get_server_state() const noexcept { return server_status_; }
    void set_server_state(server_status s) noexcept { server_status_ = s; }

    game_server_status get_game_server_state() const noexcept { return game_status_; }
    void set_game_server_state(game_server_status s) noexcept { game_status_ = s; }

    login_server_status get_login_server_state() const noexcept { return login_status_; }
    void set_login_server_state(login_server_status s) noexcept { login_status_ = s; }

    server_status server_status_ = server_status::uninitialized;
    game_server_status game_status_ = game_server_status::uninitialized;
    login_server_status login_status_ = login_server_status::uninitialized;

    void on_message(std::shared_ptr<CClient> player, ix::WebSocket & websocket, const ix::WebSocketMessagePtr & message);
    void process_binary_message(socket_message sm);
    void load_configs();

    void handle_login_server_message(socket_message & sm);

    std::deque<std::unique_ptr<socket_message>> packet_queue;
    std::mutex packet_mtx;

    uint64_t check_account_auth(std::shared_ptr<CClient> player, std::string & account, std::string & pass);
    std::set<std::shared_ptr<CClient>> client_list;
    std::set<std::shared_ptr<CNpc>> npc_list;
    std::recursive_mutex client_list_mtx;
    std::shared_mutex npc_list_mtx;
    std::vector<std::string> maps_loaded;

    void build_character_list(CClient * client, stream_write & sw);
    bool RequestLogin(std::string account, std::string password);
    void create_character(CClient * client, stream_read & sr);
    void delete_character(CClient * client, stream_read & sr);
    void enter_game(CClient * client, stream_read & sr);

    int64_t create_db_character(pqxx::transaction_base & t, character_db & character);
    void update_db_character(pqxx::transaction_base & t, character_db & character);
    void delete_db_character(pqxx::transaction_base & t, character_db & character);
    int64_t create_db_item(CItem * _item, int32_t x, int32_t y, int32_t character_handle, bool equipped = false);
    void update_db_bag_item(CItem * _item, int32_t x, int32_t y, int32_t character_handle = 0, bool equipped = false);
    void update_db_bank_item(CItem * _item, int32_t character_handle = 0);
    void delete_db_item(int64_t id);
    void force_delete_db_item(int64_t id);
    int64_t create_db_item(pqxx::transaction_base & t, item_db & _item);
    void update_db_item(pqxx::transaction_base & t, item_db & _item);
    void delete_db_item(pqxx::transaction_base & t, int64_t id);
    void force_delete_db_item(pqxx::transaction_base & t, int64_t id);

    int64_t create_db_skill(pqxx::transaction_base & t, skill_db & _item);
    void update_db_skill(pqxx::transaction_base & t, skill_db & _item);
    void delete_db_skills(pqxx::transaction_base & t, int64_t id);

    std::vector<item_db> get_db_items(pqxx::transaction_base & t, uint64_t character_id);
    std::vector<skill_db> get_db_skills(pqxx::transaction_base & t, uint64_t character_id);

    void prepare_login_statements();
    void prepare_game_statements();
    bool is_account_in_use(int64_t account_id);

    void delete_client_nolock(std::shared_ptr<CClient> client, bool save = false, bool deleteobj = false);
    void delete_client_lock(std::shared_ptr<CClient> client, bool save = false, bool deleteobj = false);

    bool save_player_data(std::shared_ptr<CClient> client);

    uint16_t add_bag_item(CClient * client, item_db & item);
    uint16_t add_bank_item(CClient * client, item_db & item);
    uint16_t add_mail_item(CClient * client, item_db & item);
    CItem * fill_item(CClient * player, item_db & item);



    void _CreateApocalypseGUID(uint32_t dwApocalypseGUID);
    void LocalEndApocalypse();
    void LocalStartApocalypse(uint32_t dwApocalypseGUID);
    void GlobalEndApocalypseMode();
    void GlobalStartApocalypseMode(int iClientH, char * pData, uint32_t dwMsgSize);
    void OpenApocalypseGate(int iClientH);

    int m_iCrusadeCount;
    bool m_bIsCrusadeMode;
    bool m_bIsApocalypseMode;
    bool m_bIsHeldenianMode;
    bool m_bIsHeldenianTeleport;
    char m_cHeldenianType;

    //bool _bDecodeNpcItemConfigFileContents(char * pData, DWORD dwMsgSize);
    //void CalculateEnduranceDecrement(short sTargetH, short sAttackerH, char cTargetType, int iArmorType);
    bool bCalculateEnduranceDecrement(short sTargetH, short sAttackerH, char cTargetType, int iArmorType);
    char _cCheckHeroItemEquipped(CClient * client);
    bool bPlantSeedBag(int iMapIndex, int dX, int dY, int iItemEffectValue1, int iItemEffectValue2, int iClientH);
    void _CheckFarmingAction(short sAttackerH, short sTargetH, bool bType);


    void CalcTotalItemEffect(CClient * client, int iEquipItemID, bool bNotify = true)
    {
        CalcTotalItemEffect(get_client_handle(client), iEquipItemID, bNotify);
    }
    int16_t get_client_handle(std::shared_ptr<CClient> player) const
    {
        return get_client_handle(player.get());
    }
    int16_t get_client_handle(CClient * player) const
    {
        for (int i = 0; i < DEF_MAXCLIENTS; i++)
            if (m_pClientList[i] == player)
                return i;
        throw std::runtime_error("client not found");
    }
    int16_t get_map_index(std::string_view mapname);

    bool bEquipItemHandler(CClient * client, short sItemIndex, bool bNotify = true)
    {
        return bEquipItemHandler(get_client_handle(client), sItemIndex, bNotify);
    }
    void CheckConnectionHandler(int iClientH, char * pData);


    //////////////////////////////////////////////////////////////////////////




    //3.51 Zero - 2.20->3.51 project
    bool bChangeState(char cStateChange, char * cStr, char * cVit, char * cDex, char * cInt, char * cMag, char * cChar);
    bool bCheckMagicInt(int iClientH);
    void StateChangeHandler(int iClientH, char * pData, uint32_t dwMsgSize);

    //Notes
#ifdef DEF_NOTES
    void ViewNotes(int iClientH);
    void AddNote(int iClientH, char * pData, uint32_t dwMsgSize);
    void DeleteNote(int iClientH, char * pData, uint32_t dwMsgSize);
#endif
    void SetHeroFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SetInhibitionCastingFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SetDefenseShieldFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SetMagicProtectionFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SetProtectionFromArrowFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SetIllusionMovementFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SetIllusionFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SetPoisonFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void RequestPlayerNameHandler(int iClientH, int iObjectID, int iIndex);

    int  iSetSide(int iClientH);
    void RequestHuntmode(int iClientH);
    void SetNoHunterMode(int iClientH, bool bSendMSG = false);
    bool _bCrusadeLog(int iAction, int iClientH, int iData, char * cName);
    void SetForceRecallTime(int iClientH);
    bool bCheckClientMoveFrequency(int iClientH, uint32_t dwClientTime);
    bool bCheckClientMagicFrequency(int iClientH, uint32_t dwClientTime);
    bool bCheckClientAttackFrequency(int iClientH, uint32_t dwClientTime);
    void RequestGuildNameHandler(int iClientH, int iObjectID, int iIndex);
    void ArmorLifeDecrement(int iClientH, int sTargetH, char cOwnerType, int iValue);
    bool bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH, bool bBonus = false);
    void RequestItemUpgradeHandler(int iClientH, int iItemIndex);
    bool bSerchMaster(int iNpcH);
    void GetExp(int iClientH, int iExp, bool bIsAttackerOwn = false);
    void PartyOperationResult_Dismiss(int iClientH, char * pName, int iResult, int iPartyID);
    void RequestAcceptJoinPartyHandler(int iClientH, int iResult);
    void RequestDeletePartyHandler(int iClientH);
    void PartyOperationResult_Info(int iClientH, char * pName, int iTotal, char * pNameList);
    void GetPartyInfoHandler(int iClientH);
    void RequestDismissPartyHandler(int iClientH);
    void PartyOperationResult_Join(int iClientH, char * pName, int iResult, int iPartyID);
    void RequestJoinPartyHandler(int iClientH, char * pData, uint32_t dwMsgSize);
    void PartyOperationResult_Delete(int iPartyID);
    void PartyOperationResult_Create(int iClientH, char * pName, int iResult, int iPartyID);
    void PartyOperationResultHandler(char * pData);
    void RequestCreatePartyHandler(int iClientH);
    bool bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex);
    void ResurrectPlayer(int iClientH);
    void KillCrusadeObjects();

    bool bReadCrusadeScheduleConfigFile(char * pFn);
    void CrusadeWarStarter();
    bool bCopyItemContents(CItem * pOriginal, CItem * pCopy);

    int  iGetMapLocationSide(char * pMapName);

    void ManualEndCrusadeMode(int iWinnerSide);
    bool bReadCrusadeGUIDFile(char * cFn);
    void _CreateCrusadeGUID(uint32_t dwCrusadeGUID, int iWinnerSide);
    void RemoveClientShortCut(int iClientH);
    bool bAddClientShortCut(int iClientH);
    void RequestSetGuildConstructLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName);
    void CheckCommanderConstructionPoint(int iClientH);
    void GlobalStartCrusadeMode();
    void SyncMiddlelandMapInfo();

    void GrandMagicResultHandler(char * cMapName, int iCrashedStructureNum, int iStructureDamageAmount, int iCasualities, int iActiveStructure, int iSTCount, char * pData);
    void CalcMeteorStrikeEffectHandler(int iMapIndex);
    void DoMeteorStrikeDamageHandler(int iMapIndex);

    void RequestHelpHandler(int iClientH);
    void RemoveCrusadeStructures();


    void RecallHunterPlayer();

    void _SendMapStatus(int iClientH);
    void MapStatusHandler(int iClientH, int iMode, char * pMapName);
    void SelectCrusadeDutyHandler(int iClientH, int iDuty);

    void RequestSummonWarUnitHandler(int iClientH, int dX, int dY, char cType, char cNum, char cMode);
    void RequestGuildTeleportHandler(int iClientH);
    void RequestSetGuildTeleportLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName);
    void MeteorStrikeHandler(int iMapIndex);
    void _LinkStrikePointMapIndex();
    void MeteorStrikeMsgHandler(char cAttackerSide);
    void _NpcBehavior_GrandMagicGenerator(int iNpcH);
    void CollectedManaHandler(uint16_t wAresdenMana, uint16_t wElvineMana);
    void SendCollectedMana();
    void CreateCrusadeStructures();

    bool bReadCrusadeStructureConfigFile(char * cFn);
    void SaveOccupyFlagData();
    void LocalEndCrusadeMode(int iWinnerSide);
    void LocalStartCrusadeMode(uint32_t dwGuildGUID);
    void CheckCrusadeResultCalculation(int iClientH);
    bool _bNpcBehavior_Detector(int iNpcH);
    bool _bNpcBehavior_ManaCollector(int iNpcH);
    bool __bSetConstructionKit(int iMapIndex, int dX, int dY, int iType, int iTimeCost, int iClientH);
    bool __bSetAgricultureItem(int iMapIndex, int dX, int dY, int iType, int iSsn, int iClientH); 
    bool bCropsItemDrop(int iClientH, short iTargetH, bool bMobDropPos = false);
    int bProbabilityTable(int x, int y, int iTable);
    void AgingMapSectorInfo();
    void UpdateMapSectorInfo();

    bool bGetItemNameWhenDeleteNpc(int & iItemID, short sNpcType, int iItemprobability);

    int iGetItemWeight(CItem * pItem, int iCount);
    void CancelQuestHandler(int iClientH);
    void ActivateSpecialAbilityHandler(int iClientH);
    void EnergySphereProcessor(bool bIsAdminCreate = false, int iClientH = 0);
    bool bCheckEnergySphereDestination(int iNpcH, short sAttackerH, char cAttackerType);
    void JoinPartyHandler(int iClientH, int iV1, char * pMemberName);
    void CreateNewPartyHandler(int iClientH);
    void _DeleteRandomOccupyFlag(int iMapIndex);
    void RequestSellItemListHandler(int iClientH, char * pData);

    void RequestRestartHandler(int iClientH);
    int iRequestPanningMapDataRequest(int iClientH, char * pData);
    void GetMagicAbilityHandler(int iClientH);
    void Effect_Damage_Spot_DamageMove(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, bool bExp, int iAttr);
    void _TamingHandler(int iClientH, int iSkillNum, char cMapIndex, int dX, int dY);
    void RequestCheckAccountPasswordHandler(char * pData, uint32_t dwMsgSize);
    int _iTalkToNpcResult_Guard(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    void SetIceFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void _bDecodeNoticementFileContents(char * pData, uint32_t dwMsgSize);
    void RequestNoticementHandler(int iClientH, char * pData);
    void _AdjustRareItemValue(CItem * pItem);
    bool _bCheckDupItemID(CItem * pItem);
    bool _bDecodeDupItemIDFileContents(char * pData, uint32_t dwMsgSize);
    void NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType);
    // int  iGetPlayerABSStatus(int iWhatH, int iRecvH);
    int  iGetPlayerABSStatus(int iWhatH, int iRecvH); // 2002-12-2
    void CheckSpecialEvent(int iClientH);
    void CheckSpecialEventThirdYear(int iClientH);

    bool _bDecodeNpcItemConfigFileContents(char * pData, uint32_t dwMsgSize);

    char _cGetSpecialAbility(int iKindSA);

    void BuildItemHandler(int iClientH, char * pData);
    bool _bDecodeBuildItemConfigFileContents(char * pData, uint32_t dwMsgSize);

    bool _bItemLog(int iAction, int iGiveH, int iRecvH, CItem * pItem, bool bForceItemLog = false);
    bool _bItemLog(int iAction, int iClientH, char * cName, CItem * pItem);
    bool _bPKLog(int iAction, int iAttackerH, int iVictumH, char * cNPC);
    bool _bCheckGoodItem(CItem * pItem);

    void _CheckStrategicPointOccupyStatus(char cMapIndex);
    void GetMapInitialPoint(int iMapIndex, short * pX, short * pY, char * pPlayerLocation = 0);
    int  iGetMaxHP(int iClientH);
    void _ClearQuestStatus(int iClientH);
    bool _bCheckItemReceiveCondition(int iClientH, CItem * pItem);
    void SendItemNotifyMsg(int iClientH, uint16_t wMsgType, CItem * pItem, int iV1);

    int _iTalkToNpcResult_WTower(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    int _iTalkToNpcResult_WHouse(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    int _iTalkToNpcResult_BSmith(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    int _iTalkToNpcResult_GShop(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    int _iTalkToNpcResult_GuildHall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    bool _bCheckIsQuestCompleted(int iClientH);
    void _CheckQuestEnvironment(int iClientH);
    void _SendQuestContents(int iClientH);
    void QuestAcceptedHandler(int iClientH);
    bool _bDecodeQuestConfigFileContents(char * pData, uint32_t dwMsgSize);
    void CancelExchangeItem(int iClientH);
    bool bAddItem(int iClientH, CItem * pItem, char cMode);
    void ConfirmExchangeItem(int iClientH);
    //void SetExchangeItem(int iClientH, int iItemIndex, int iAmount);
    void SetExchangeItem(int iClientH, int iItemIndex, int iAmount, int sItemIndex);
    void ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, uint16_t wObjectID, char * pItemName);

    void CheckUniqueItemEquipment(int iClientH);
    void _SetItemPos(int iClientH, char * pData);
    void GetHeroMantleHandler(int iClientH);

    bool _bDecodeOccupyFlagSaveFileContents(char * pData, uint32_t dwMsgSize);
    void GetOccupyFlagHandler(int iClientH);
    int  _iComposeFlagStatusContents(char * pData);
    void SetSummonMobAction(int iClientH, int iMode, uint32_t dwMsgSize, char * pData = 0);
    bool __bSetOccupyFlag(char cMapIndex, int dX, int dY, int iSide, int iEKNum, int iClientH, bool bAdminFlag);
    bool _bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID);
    void SetDownSkillIndexHandler(int iClientH, int iSkillIndex);
    int iGetComboAttackBonus(int iSkill, int iComboCount);
    int  _iGetWeaponSkillType(int iClientH);
    void CheckFireBluring(char cMapIndex, int sX, int sY);
    void NpcTalkHandler(int iClientH, int iWho);
    bool bDeleteMineral(int iIndex);
    void _CheckMiningAction(int iClientH, int dX, int dY);
    int iCreateMineral(char cMapIndex, int tX, int tY, char cLevel);
    void MineralGenerator();
    bool _bDecodePortionConfigFileContents(char * pData, uint32_t dwMsgSize);
    void ReqCreatePortionHandler(int iClientH, char * pData);
    void _CheckAttackType(int iClientH, short * spType);
    bool bOnClose();
    void ForceDisconnectAccount(char * pAccountName, uint16_t wCount);
    void NpcRequestAssistance(int iNpcH);
    void ToggleSafeAttackModeHandler(int iClientH);
    void SetBerserkFlag(short sOwnerH, char cOwnerType, bool bStatus);
    void SpecialEventHandler();
    int iGetPlayerRelationship_SendEvent(int iClientH, int iOpponentH);
    int iGetNpcRelationship_SendEvent(int iNpcH, int iOpponentH);
    int _iForcePlayerDisconect(int iNum);
    int iGetMapIndex(char * pMapName);
    //int iGetNpcRelationship(int iClientH, int iOpponentH);
    int iGetNpcRelationship(int iWhatH, int iRecvH);
    int iGetPlayerRelationship(int iClientH, int iOpponentH);
    int iGetWhetherMagicBonusEffect(short sType, char cWheatherStatus);
    void WhetherProcessor();
    int _iCalcPlayerNum(char cMapIndex, short dX, short dY, char cRadius);
    void FishGenerator();
    void ReqGetFishThisTimeHandler(int iClientH);
    void FishProcessor();
    int iCheckFish(int iClientH, char cMapIndex, short dX, short dY);
    bool bDeleteFish(int iHandle, int iDelMode);
    int  iCreateFish(char cMapIndex, short sX, short sY, short sDifficulty, CItem * pItem, int iDifficulty, uint32_t dwLastTime);
    void UserCommand_DissmissGuild(int iClientH, char * pData, uint32_t dwMsgSize);

    void UserCommand_BanGuildsman(int iClientH, char * pData, uint32_t dwMsgSize);

    int iGetExpLevel(int iExp);
    void ___RestorePlayerRating(int iClientH);
    void CalcExpStock(int iClientH);
    void ResponseSavePlayerDataReplyHandler(char * pData, uint32_t dwMsgSize);
    void NoticeHandler();
    bool bReadNotifyMsgListFile(char * cFn);
    void SetPlayerReputation(int iClientH, char * pMsg, char cValue, uint32_t dwMsgSize);
    void ShutUpPlayer(int iClientH, char * pMsg, uint32_t dwMsgSize);
    void CheckDayOrNightMode();
    bool bCheckBadWord(char * pString);
    bool bCheckResistingPoisonSuccess(short sOwnerH, char cOwnerType);
    void PoisonEffect(int iClientH, int iV1);
    void bSetNpcAttackMode(char * cName, int iTargetH, char cTargetType, bool bIsPermAttack);
    bool _bGetIsPlayerHostile(int iClientH, int sOwnerH);
    bool bAnalyzeCriminalAction(int iClientH, short dX, short dY, bool bIsCheck = false);
    void RequestAdminUserMode(int iClientH, char * pData);
    int _iGetPlayerNumberOnSpot(short dX, short dY, char cMapIndex, char cRange);
    void CalcTotalItemEffect(int iClientH, int iEquipItemID, bool bNotify = true);
    void ___RestorePlayerCharacteristics(int iClientH);
    void GetPlayerProfile(int iClientH, char * pMsg, uint32_t dwMsgSize);
    void SetPlayerProfile(int iClientH, char * pMsg, uint32_t dwMsgSize);
    void ToggleWhisperPlayer(int iClientH, char * pMsg, uint32_t dwMsgSize);
    void CheckAndNotifyPlayerConnection(int iClientH, char * pMsg, uint32_t dwSize);
    int iCalcTotalWeight(int iClientH);
    void ReqRepairItemCofirmHandler(int iClientH, char cItemID, char * pString);
    void ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char * pString);
    void ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char * pString);
    void ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char * pItemName);
    void UseSkillHandler(int iClientH, int iV1, int iV2, int iV3);
    int  iCalculateUseSkillItemEffect(int iOwnerH, char cOwnerType, char cOwnerSkill, int iSkillNum, char cMapIndex, int dX, int dY);
    void ClearSkillUsingStatus(int iClientH);
    void DynamicObjectEffectProcessor();
    int _iGetTotalClients();
    void SendObjectMotionRejectMsg(int iClientH);
    void SetInvisibilityFlag(short sOwnerH, char cOwnerType, bool bStatus);
    bool bRemoveFromDelayEventList(int iH, char cType, int iEffectType);
    void DelayEventProcessor();
    bool bRegisterDelayEvent(int iDelayType, int iEffectType, uint32_t dwLastTime, int iTargetH, char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3);
    int iGetFollowerNumber(short sOwnerH, char cOwnerType);
    int  _iCalcSkillSSNpoint(int iLevel);
    bool bCheckTotalSkillMasteryPoints(int iClientH, int iSkill);
    bool bSetItemToBankItem(int iClientH, CItem * pItem);
    void NpcMagicHandler(int iNpcH, short dX, short dY, short sType);
    bool bCheckResistingIceSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio);
    bool bCheckResistingMagicSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio);
    void Effect_SpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
    void Effect_SpDown_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
    void Effect_HpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
    void Effect_Damage_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3, bool bExp, int iAttr = 0);
    void UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID);
    void NpcBehavior_Stop(int iNpcH);
    // v2.15 
    void ItemDepleteHandler(int iClientH, short sItemIndex, bool bIsUseItemResult, bool bIsLog = true);
    int _iGetArrowItemIndex(int iClientH);
    void RequestFullObjectData(int iClientH, char * pData);
    void DeleteNpc(int iNpcH);
    void CalcNextWayPointDestination(int iNpcH);
    void MobGenerator();
    void CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue);
    void CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue);
    void CheckDynamicObjectList();
    int  iAddDynamicObjectList(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, uint32_t dwLastTime, int iV1 = 0);
    int _iCalcMaxLoad(int iClientH);
    void GetRewardMoneyHandler(int iClientH);
    void _PenaltyItemDrop(int iClientH, int iTotal, bool bIsSAattacked = false, bool bItemDrop = false);
    void ApplyCombatKilledPenalty(int iClientH, char cPenaltyLevel, bool bIsSAattacked = false, bool bItemDrop = false);
    void EnemyKillRewardHandler(int iAttackerH, int iClientH);
    void PK_KillRewardHandler(short sAttackerH, short sVictumH);
    void ApplyPKpenalty(short sAttackerH, short sVictumLevel);
    bool bSetItemToBankItem(int iClientH, short sItemIndex);
    void RequestRetrieveItemHandler(int iClientH, char * pData);
    void RequestCivilRightHandler(int iClientH, char * pData);
    bool bCheckLimitedUser(int iClientH);
    void LevelUpSettingsHandler(int iClientH, char * pData, uint32_t dwMsgSize);

    void FightzoneReserveHandler(int iClientH, char * pData, uint32_t dwMsgSize);
    bool bCheckLevelUp(int iClientH);
    int iGetLevelExp(int iLevel);
    void TimeManaPointsUp(int iClientH);
    void TimeStaminarPointsUp(int iClientH);
    void Quit();
    bool __bReadMapInfo(int iMapIndex);

    int  _iGetSkillNumber(char * pSkillName);
    void TrainSkillResponse(bool bSuccess, int iClientH, int iSkillNum, int iSkillLevel);
    int _iGetMagicNumber(char * pMagicName, int * pReqInt, int * pCost);
    void RequestStudyMagicHandler(int iClientH, char * pName, bool bIsPurchase = true);
    bool _bDecodeSkillConfigFileContents(char * pData, uint32_t dwMsgSize);
    bool _bDecodeMagicConfigFileContents(char * pData, uint32_t dwMsgSize);
    void ReleaseFollowMode(short sOwnerH, char cOwnerType);
    bool bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType);
    void RequestTeleportHandler(int iClientH, char * pData, char * cMapName = 0, int dX = -1, int dY = -1);
    void PlayerMagicHandler(int iClientH, int dX, int dY, short sType, bool bItemEffect = false, int iV1 = 0);
    int  iClientMotion_Magic_Handler(int iClientH, short sX, short sY, char cDir);
    void ToggleCombatModeHandler(int iClientH);
    void GuildNotifyHandler(char * pData, uint32_t dwMsgSize);
    void SendGuildMsg(int iClientH, uint16_t wNotifyMsgType, short sV1, short sV2, char * pString);
    void DelayEventProcess();
    void TimeHitPointsUp(int iClientH);
    void CalculateGuildEffect(int iVictimH, char cVictimType, short sAttackerH);
    void OnStartGameSignal();
    int iDice(int iThrow, int iRange);
    bool _bInitNpcAttr(CNpc * pNpc, char * pNpcName, short sClass, char cSA);
    bool _bDecodeNpcConfigFileContents(char * pData, uint32_t dwMsgSize);
    void ReleaseItemHandler(int iClientH, short sItemIndex, bool bNotice);
    void ClientKilledHandler(int iClientH, int iAttackerH, char cAttackerType, short sDamage);
    int  SetItemCount(int iClientH, char * pItemName, uint32_t dwCount);
    int  SetItemCount(int iClientH, int iItemIndex, uint32_t dwCount);
    uint32_t dwGetItemCount(int iClientH, char * pName);
    void DismissGuildRejectHandler(int iClientH, char * pName);
    void DismissGuildApproveHandler(int iClientH, char * pName);
    void JoinGuildRejectHandler(int iClientH, char * pName);
    void JoinGuildApproveHandler(int iClientH, char * pName);
    void SendNotifyMsg(int iFromH, int iToH, uint16_t wMsgType, uint32_t sV1, uint32_t sV2, uint32_t sV3, char * pString, uint32_t sV4 = 0, uint32_t sV5 = 0, uint32_t sV6 = 0, uint32_t sV7 = 0, uint32_t sV8 = 0, uint32_t sV9 = 0, char * pString2 = 0);
    void GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, uint16_t wObjectID, char * pItemName);
    void RequestPurchaseItemHandler(int iClientH, char * pItemName, int iNum);
    void ResponseDisbandGuildHandler(char * pData, uint32_t dwMsgSize);
    void RequestDisbandGuildHandler(int iClientH, char * pData, uint32_t dwMsgSize);
    void RequestCreateNewGuildHandler(int iClientH, char * pData, uint32_t dwMsgSize);
    void ResponseCreateNewGuildHandler(char * pData, uint32_t dwMsgSize);
    int  iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir);

    bool bEquipItemHandler(int iClientH, short sItemIndex, bool bNotify = true);
    bool _bAddClientItemList(int iClientH, CItem * pItem, int * pDelReq);
    int  iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir);
    void DropItemHandler(int iClientH, short sItemIndex, int iAmount, char * pItemName, bool bByPlayer = false);
    void ClientCommonHandler(int iClientH, char * pData, uint32_t size);
    void MsgProcess();
    void PutMsgQueue(std::unique_ptr<socket_message> & sm);
    void NpcBehavior_Flee(int iNpcH);
    int iGetDangerValue(int iNpcH, short dX, short dY);
    void NpcBehavior_Dead(int iNpcH);
    void NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage);
    int  iCalculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, bool bNearAttack = false, bool bIsDash = false);
    void RemoveFromTarget(short sTargetH, char cTargetType, int iCode = 0);
    void NpcBehavior_Attack(int iNpcH);
    void TargetSearch(int iNpcH, short * pTarget, char * pTargetType);
    void NpcBehavior_Move(int iNpcH);
    bool bGetEmptyPosition(short * pX, short * pY, char cMapIndex);
    char cGetNextMoveDir(short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn, int * pError);
    int  iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType, char cDir, uint16_t wTargetObjectID, bool bRespose = true, bool bIsDash = false);
    void ChatMsgHandler(int iClientH, char * pData, uint32_t dwMsgSize);
    void NpcProcess();
    bool bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType, int * poX, int * poY, char * pWaypointList, hbxrect * pArea, int iSpotMobIndex, char cChangeSide, bool bHideGenMode, bool bIsSummoned = false, bool bFirmBerserk = false, bool bIsMaster = false, int iGuildGUID = 0);
    bool _bReadMapInfoFiles(int iMapIndex);

    bool _bGetIsStringIsNumber(char * pStr);
    bool _bInitItemAttr(CItem * pItem, char * pItemName);

    bool _bInitItemAttr(CItem * pItem, int iItemID);
    bool bReadProgramConfigFile(char * cFn);
    void GameProcess();
    void ResponsePlayerDataHandler(char * pData, uint32_t dwSize);

    void CheckClientResponseTime();
    void OnTimer();
    int iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char * pData);
    void SendEventToNearClient_TypeB(uint32_t dwMsgID, uint16_t wMsgType, char cMapIndex, short sX, short sY, short sV1 = 0, short sV2 = 0, short sV3 = 0, short sV4 = 0);
    void SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, uint32_t dwMsgID, uint16_t wMsgType, short sV1, short sV2, short sV3);
    void DeleteClient(int iClientH, bool bSave, bool bNotify, bool bCountLogout = true, bool bForceCloseConn = false);
    int  iComposeInitMapData(short sX, short sY, int iClientH, char * pData);
    void RequestInitDataHandler(int iClientH, char * pData, char cKey, bool bIsNoNameCheck = false);
    int  iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, bool bIsRun);
    void ClientMotionHandler(int iClientH, char * pData);
    bool bInit();

    void GetFightzoneTicketHandler(int iClientH);

    void FightzoneReserveProcessor();

    // 2002-10-23 Item Event
    bool NpcDeadItemGeneratorWithItemEvent(int iNpcH, short sAttackerH, char cAttackerType);

    bool bCheckInItemEventList(int iItemID, int iNpcH);


    bool _bDecodeTeleportListConfigFileContents(char * pData, uint32_t dwMsgSize);

    void RequestTeleportListHandler(int iClientH, char * pData, uint32_t dwMsgSize);
    void RequestChargedTeleportHandler(int iClientH, char * pData, uint32_t dwMsgSize);

    void CritInc(int iClientH);
    void AddGizon(int iClientH);
    void CheckTimeOut(int iClientH);
    void SetTimeOut(int iClientH);
    void ForceRecallProcess();
    void SkillCheck(int sTargetH);
    bool IsEnemyZone(int i);

    CGame();
    ~CGame();

    char m_cServerName[12];
    char m_cGameServerAddr[16];
    char m_cLogServerAddr[16];
    char m_cGateServerAddr[16];
    int m_iGameServerPort;
    int m_iLogServerPort;
    int m_iGateServerPort;
    int m_iWorldLogServerPort;

    int m_iLimitedUserExp, m_iLevelExp20;

    //private:
    bool _bDecodeItemConfigFileContents(char * pData, uint32_t dwMsgSize);
    bool _bRegisterMap(char * pName);

    CClient * m_pClientList[DEF_MAXCLIENTS];
    CNpc * m_pNpcList[DEF_MAXNPCS];
    CMap * m_pMapList[DEF_MAXMAPS];
    CDynamicObject * m_pDynamicObjectList[DEF_MAXDYNAMICOBJECTS];
    CDelayEvent * m_pDelayEventList[DEF_MAXDELAYEVENTS];

    CMsg * m_pMsgQuene[DEF_MSGQUENESIZE];
    int m_iQueneHead, m_iQueneTail;
    int m_iTotalMaps;
    int m_iGateSockConnRetryTimes;
    CMisc m_Misc;
    bool m_bIsGameStarted;
    bool m_bIsLogSockAvailable, m_bIsGateSockAvailable;
    bool m_bIsItemAvailable, m_bIsBuildItemAvailable, m_bIsNpcAvailable, m_bIsMagicAvailable;
    bool m_bIsSkillAvailable, m_bIsPortionAvailable, m_bIsQuestAvailable, m_bIsWLServerAvailable;
    CItem * m_pItemConfigList[DEF_MAXITEMTYPES];
    CNpc * m_pNpcConfigList[DEF_MAXNPCTYPES];
    CMagic * m_pMagicConfigList[DEF_MAXMAGICTYPE];
    CSkill * m_pSkillConfigList[DEF_MAXSKILLTYPE];
    CQuest * m_pQuestConfigList[DEF_MAXQUESTTYPE];
    char m_pMsgBuffer[DEF_MSGBUFFERSIZE + 1];


    CTeleport * m_pTeleportConfigList[DEF_MAXTELEPORTLIST];

    HWND m_hWnd;
    int m_iTotalClients, m_iMaxClients, m_iTotalGameServerClients, m_iTotalGameServerMaxClients;
    SYSTEMTIME m_MaxUserSysTime;

    bool m_bF1pressed, m_bF4pressed, m_bF12pressed;
    bool m_bOnExitProcess;
    uint32_t m_dwExitProcessTime;

    uint32_t m_dwWhetherTime, m_dwGameTime1, m_dwGameTime2, m_dwGameTime3, m_dwGameTime4, m_dwGameTime5, m_dwGameTime6, m_dwFishTime;

    bool m_cDayOrNight;
    int m_iSkillSSNpoint[102];

    CMsg * m_pNoticeMsgList[DEF_MAXNOTIFYMSGS];
    int m_iTotalNoticeMsg, m_iPrevSendNoticeMsg;
    uint32_t m_dwNoticeTime, m_dwSpecialEventTime;
    bool m_bIsSpecialEventTime;
    char m_cSpecialEventType;

    int   m_iLevelExpTable[300];
    CFish * m_pFish[DEF_MAXFISHS];
    CPotion * m_pPortionConfigList[DEF_MAXPORTIONTYPES];

    bool m_bIsServerShutdowned;
    char m_cShutDownCode;
    CMineral * m_pMineral[DEF_MAXMINERALS];

    int m_iMiddlelandMapIndex;
    int m_iAresdenMapIndex;
    int m_iElvineMapIndex;
    int m_iAresdenOccupyTiles;
    int m_iElvineOccupyTiles;
    int m_iCurMsgs, m_iMaxMsgs;


    uint32_t m_dwCanFightzoneReserveTime;

    int m_iFightZoneReserve[DEF_MAXFIGHTZONE];

    int m_iFightzoneNoForceRecall;

    struct
    {
        int64_t iFunds;
        int64_t iCrimes;
        int64_t iWins;
    } m_stCityStatus[3];

    int m_iStrategicStatus;

    CBuildItem * m_pBuildItemList[DEF_MAXBUILDITEMS];
    CItem * m_pDupItemIDList[DEF_MAXDUPITEMID];

    char * m_pNoticementData;
    uint32_t m_dwNoticementDataSize;

    uint32_t m_dwMapSectorInfoTime;
    int m_iMapSectorInfoUpdateCount;

    struct
    {
        char cMapName[11];
        char cType;
        int dX, dY;
    } m_stCrusadeStructures[DEF_MAXCRUSADESTRUCTURES];

    int m_iCollectedMana[3];
    int m_iAresdenMana, m_iElvineMana;

    CTeleportLoc m_pGuildTeleportLoc[DEF_MAXGUILDS];
    //

    uint16_t  m_wServerID_GSS;
    char m_cGateServerStockMsg[DEF_MAXGATESERVERSTOCKMSGSIZE];
    int m_iIndexGSS;

    struct
    {
        int iCrashedStructureNum;
        int iStructureDamageAmount;
        int iCasualties;
    } m_stMeteorStrikeResult;

    int m_iLastCrusadeWinner;

    struct
    {
        char cType;
        char cSide;
        short sX, sY;
    } m_stMiddleCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
    int m_iTotalMiddleCrusadeStructures;


    int m_iClientShortCut[DEF_MAXCLIENTS + 1];

    int m_iNpcConstructionPoint[DEF_MAXNPCTYPES];
    uint32_t m_dwCrusadeGUID;
    int m_iCrusadeWinnerSide;
    int m_iWinnerSide;
    int m_iNonAttackArea;

    int m_iPlayerMaxLevel;
    int m_iWorldMaxUser;

    short m_sForceRecallTime;

    int m_iFinalShutdownCount;

    struct
    {
        int iTotalMembers;
        int iIndex[DEF_MAXPARTYMEMBERS];
    } m_stPartyInfo[DEF_MAXPARTYNUM];

    CItem * m_pGold;

    bool m_bReceivedItemList;



    // todo: remove this all
    bool bReadAdminSetConfigFile(char * cFn);
    int m_iAdminLevelWho;
    int m_iAdminLevelGMKill;
    int m_iAdminLevelGMRevive;
    int m_iAdminLevelGMCloseconn;
    int m_iAdminLevelGMCheckRep;
    int m_iAdminLevelEnergySphere;
    int m_iAdminLevelShutdown;
    int m_iAdminLevelObserver;
    int m_iAdminLevelShutup;
    int m_iAdminLevelCallGaurd;
    int m_iAdminLevelSummonDemon;
    int m_iAdminLevelSummonDeath;
    int m_iAdminLevelReserveFightzone;
    int m_iAdminLevelCreateFish;
    int m_iAdminLevelTeleport;
    int m_iAdminLevelCheckIP;
    int m_iAdminLevelPolymorph;
    int m_iAdminLevelSetInvis;
    int m_iAdminLevelSetZerk;
    int m_iAdminLevelSetIce;
    int m_iAdminLevelGetNpcStatus;
    int m_iAdminLevelSetAttackMode;
    int m_iAdminLevelUnsummonAll;
    int m_iAdminLevelUnsummonDemon;
    int m_iAdminLevelSummon;
    int m_iAdminLevelSummonAll;
    int m_iAdminLevelSummonPlayer;
    int m_iAdminLevelDisconnectAll;
    int m_iAdminLevelEnableCreateItem;
    int m_iAdminLevelCreateItem;
    int m_iAdminLevelStorm;
    int m_iAdminLevelWeather;
    int m_iAdminLevelSetStatus;
    int m_iAdminLevelGoto;
    int m_iAdminLevelMonsterCount;
    int m_iAdminLevelSetRecallTime;
    int m_iAdminLevelUnsummonBoss;
    int m_iAdminLevelClearNpc;
    int m_iAdminLevelTime;
    int m_iAdminLevelPushPlayer;
    int m_iAdminLevelSummonGuild;
    int m_iAdminLevelCheckStatus;
    int m_iAdminLevelCleanMap;

private:
    int __iSearchForQuest(int iClientH, int iWho, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    int _iTalkToNpcResult_Cityhall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
    void _ClearExchangeStatus(int iClientH);
    int _iGetItemSpaceLeft(int iClientH);

    void ScreenSettingsHandler(std::shared_ptr<CClient> player, char * pData, uint32_t dwMsgSize);
public:
    void AdminOrder_EnableAdminCreateItem(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_Add(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_View(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SummonMOB(int iClientH);
    void AdminOrder_GetFightzoneTicket(int iClientH);



    void AdminOrder_GoTo(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_MonsterCount(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SetForceRecallTime(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_UnsummonBoss(int iClientH);
    void AdminOrder_Time(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_Pushplayer(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_CheckRep(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_ClearNpc(int iClientH);
    void AdminOrder_Weather(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SendMSG(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SetStatus(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SummonStorm(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_CallMagic(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SummonDeath(int iClientH);
    void AdminOrder_SetZerk(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SetFreeze(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_Kill(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_Revive(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SetObserverMode(int iClientH);
    void AdminOrder_EnableAdminCommand(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_CreateItem(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_Summon(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SummonAll(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SummonPlayer(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_UnsummonDemon(int iClientH);
    void AdminOrder_UnsummonAll(int iClientH);
    void AdminOrder_SetAttackMode(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_SummonDemon(int iClientH);
    void AdminOrder_SetInvi(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_Polymorph(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_GetNpcStatus(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_CheckIP(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_CreateFish(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_Teleport(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_ReserveFightzone(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_CloseConn(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_CallGuard(int iClientH, char * pData, uint32_t dwMsgSize);
    void AdminOrder_DisconnectAll(int iClientH, char * pData, uint32_t dwMsgSize);

    void RemoveCrusadeNpcs(void);
    void RemoveCrusadeRecallTime(void);
    int iGetPlayerABSStatus(int iClientH);
    void ReqCreateSlateHandler(int iClientH, char * pData);
    void SetSlateFlag(int iClientH, short sType, bool bFlag);
    void CheckForceRecallTime(int iClientH);
    void SetPlayingStatus(int iClientH);
    void ForceChangePlayMode(int iClientH, bool bNotify);
    void ShowVersion(int iClientH);
    void ShowClientMsg(int iClientH, char * pMsg);
    void RequestResurrectPlayer(int iClientH, bool bResurrect);
    void LoteryHandler(int iClientH);
    void SetSkillAll(int iClientH, char * pData, uint32_t dwMsgSize);
    void EKAnnounce(int iClientH, char * pMsg);
    void ApocalypseEnder();
    void SetHeldenianMode();
    void AutomatedHeldenianTimer();
    void LocalStartHeldenianMode(short sV1, short sV2, uint32_t dwHeldenianGUID);
    void GlobalStartHeldenianMode();
    void HeldenianWarEnder();
    void HeldenianWarStarter();
    bool UpdateHeldenianStatus();
    void _CreateHeldenianGUID(uint32_t dwHeldenianGUID, int iWinnerSide);
    void ManualStartHeldenianMode(int iClientH, char * pData, uint32_t dwMsgSize);
    void ManualEndHeldenianMode(int iClientH, char * pData, uint32_t dwMsgSize);
    void NotifyStartHeldenianMode();
    void GlobalEndHeldenianMode();
    void LocalEndHeldenianMode();
    bool bNotifyHeldenianWinner();
    void RemoveHeldenianNpc(int iNpcH);
    void RequestHeldenianTeleport(int iClientH, char * pData, uint32_t dwMsgSize);
    bool bCheckHeldenianMap(int sAttackerH, int iMapIndex, char cType);

    bool bReadSettingsConfigFile(char * cFn);
    bool bReadAdminListConfigFile(char * cFn);
    bool bReadBannedListConfigFile(char * cFn);

    bool bReadScheduleConfigFile(char * pFn);

    bool bReadHeldenianGUIDFile(char * cFn);
    bool bReadApocalypseGUIDFile(char * cFn);

    bool var_89C, var_8A0;
    char m_cHeldenianVictoryType, m_sLastHeldenianWinner, m_cHeldenianModeType;
    int m_iHeldenianAresdenDead, m_iHeldenianElvineDead, var_A38, var_88C;
    int m_iHeldenianAresdenLeftTower, m_iHeldenianElvineLeftTower;
    uint32_t m_dwHeldenianGUID, m_dwHeldenianStartHour, m_dwHeldenianStartMinute, m_dwHeldenianStartTime, m_dwHeldenianFinishTime;
    bool m_bHeldenianInitiated;
    bool m_bHeldenianRunning;

    bool m_bIsCrusadeWarStarter;
    bool m_bIsApocalypseStarter;
    int m_iLatestCrusadeDayOfWeek;
    uint32_t m_dwApocalypseGUID;

    struct
    {
        char m_cBannedIPaddress[30];
    } m_stBannedList[DEF_MAXBANNED];

    struct
    {
        char m_cGMName[11];
    } m_stAdminList[DEF_MAXADMINS];

    struct
    {
        int iDay;
        int iHour;
        int iMinute;
    } m_stCrusadeWarSchedule[DEF_MAXSCHEDULE];

    struct
    {
        int iDay;
        int iHour;
        int iMinute;
    } m_stApocalypseScheduleStart[DEF_MAXAPOCALYPSE];

    struct
    {
        int iDay;
        int StartiHour;
        int StartiMinute;
        int EndiHour;
        int EndiMinute;
    } m_stHeldenianSchedule[DEF_MAXHELDENIAN];

    struct
    {
        int iDay;
        int iHour;
        int iMinute;
    } m_stApocalypseScheduleEnd[DEF_MAXAPOCALYPSE];

    void RemoveOccupyFlags(int iMapIndex);

    /*void GetAngelMantleHandler(int iClientH,int iItemID,char * pString);
    void CheckAngelUnequip(int iClientH, int iAngelID);
    int iAngelEquip(int iClientH);*/

    int	 m_iPrimaryDropRate, m_iSecondaryDropRate;

    short m_sRaidTimeMonday;
    short m_sRaidTimeTuesday;
    short m_sRaidTimeWednesday;
    short m_sRaidTimeThursday;
    short m_sRaidTimeFriday;
    short m_sRaidTimeSaturday;
    short m_sRaidTimeSunday;

    int m_sCharPointLimit;

    short m_sCharStatLimit;
    bool m_bAllow100AllSkill;
    short m_sCharSkillLimit;
    char m_cRepDropModifier;
    char  m_cSecurityNumber[11];
    short m_sMaxPlayerLevel;

    short m_sSlateSuccessRate = 0;

    bool m_bEnemyKillMode;
    int m_iEnemyKillAdjust;
    bool m_bAdminSecurity;
    int m_bLogChatOption;
    int m_iSummonGuildCost;
};
