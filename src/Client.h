//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Item.h"
#include "GuildsMan.h"
#include "Magic.h"
#include "defines.h"
#include "time_utils.h"

#include <chrono>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXConnectionState.h>

class stream_write;

#define DEF_MSGBUFFERSIZE	30000
#define DEF_MAXITEMS		50
#define DEF_MAXBANKITEMS	200
#define DEF_MAXGUILDSMAN	128

#define	DEF_MAXMAGICTYPE	100
#define DEF_MAXSKILLTYPE	60

#define DEF_MAXPARTYMEMBERS	9

#define DEF_SPECABLTYTIMESEC	1200

struct account_db
{
    int64_t id{};
    std::string email{};
    int32_t admin_level{};
    int64_t created{};
    int64_t last_login{};
    bool banned{};
    int64_t xcoins{};
    int64_t external_member_id{};
};

struct character_db
{
    int64_t id{};
    int64_t account_id{};
    std::string name{};
    int32_t id1{};
    int32_t id2{};
    int32_t id3{};
    int32_t level{};
    int32_t strength{};
    int32_t vitality{};
    int32_t dexterity{};
    int32_t intelligence{};
    int32_t magic{};
    int32_t charisma{};
    int64_t experience{};
    int16_t gender{};
    int32_t skin{};
    int32_t hairstyle{};
    int32_t haircolor{};
    int32_t underwear{};
    int32_t apprcolor{};
    int32_t appr1{};
    int32_t appr2{};
    int32_t appr3{};
    int32_t appr4{};
    std::string nation{};
    std::string maploc{};
    int32_t locx{};
    int32_t locy{};
    std::string profile{};
    int32_t adminlevel{};
    int32_t contribution{};
    int32_t leftspectime{};
    std::string lockmapname{};
    int32_t lockmaptime{};
    int64_t lastsavedate{};
    int64_t blockdate{};
    int64_t guild_id{};
    int32_t fightnum{};
    int32_t fightdate{};
    int32_t fightticket{};
    int32_t questnum{};
    int32_t questid{};
    int32_t questcount{};
    int32_t questrewardtype{};
    int32_t questrewardamount{};
    int32_t questcompleted{};
    int32_t eventid{};
    int32_t warcon{};
    int32_t crusadejob{};
    int32_t crusadeid{};
    int32_t crusadeconstructpoint{};
    int32_t reputation{};
    int32_t hp{};
    int32_t mp{};
    int32_t sp{};
    int32_t ek{};
    int32_t pk{};
    int32_t rewardgold{};
    int32_t downskillid{};
    int32_t hunger{};
    int32_t leftsac{};
    int32_t leftshutuptime{};
    int32_t leftreptime{};
    int32_t leftforcerecalltime{};
    int32_t leftfirmstaminatime{};
    int32_t leftdeadpenaltytime{};
    std::string magicmastery{};
    int64_t party_id{};
    int32_t itemupgradeleft{};
    int32_t totalek{};
    int32_t totalpk{};
    int32_t mmr{};
    int32_t altmmr{};
    int32_t head_appr{};
    int32_t body_appr{};
    int32_t arm_appr{};
    int32_t leg_appr{};
    int64_t gold{};
    int32_t luck{};
    std::string world_name{};
};

struct skill_db
{
    int64_t id{};
    int64_t character_id{};
    int64_t skill_id{};
    int32_t skill_level{};
    int32_t skill_exp{};
};

enum class client_status
{
    dead = 0,
    login_screen = 1,
    in_game = 2
};

class CClient : public std::enable_shared_from_this<CClient>, public ix::ConnectionState
{
public:

#ifdef DEF_NOTES
    struct
    {
        char m_cNotes[90];
    } m_stNotesList[20];
#endif

    // 0 = dead connect, 1 = login screen, 2 = in game
// can switch between 1 and 2 but can only be 0 when connection closing
    client_status currentstatus = client_status::login_screen;

    std::shared_ptr<CClient> get_ptr()
    {
        return shared_from_this();
    }

    std::list<stream_write *> outgoingqueue;

    std::string address;

private:
    bool connected = true;
    time_point<system_clock> connecttime = now();
    time_point<system_clock> disconnecttime = now();
    time_point<system_clock> lastpackettime = now();
    time_point<system_clock> lastchecktime = now();
    time_point<system_clock> logintime = now();
    std::weak_ptr<ix::WebSocket> websocket;

public:
    void set_connected(bool s = true);
    void set_connect_time(time_point<system_clock> t);
    void set_disconnect_time(time_point<system_clock> t);
    void set_last_packet_time(time_point<system_clock> t);
    void set_last_check_time(time_point<system_clock> t);
    void set_login_time(time_point<system_clock> t);

    bool get_connected();
    time_point<system_clock> get_connect_time();
    time_point<system_clock> get_disconnect_time();
    time_point<system_clock> get_last_packet_time();
    time_point<system_clock> get_last_check_time();
    time_point<system_clock> get_login_time();

    int32_t iSendMsg(const char * pMsg, std::size_t iSize, char key = 0)
    {
        auto data = ix::IXWebSocketSendData{ pMsg, (std::size_t)iSize };
        auto ws = get_websocket();
        if (!ws) return -128;// DEF_XSOCKEVENT_NOTCONNECTED
        auto wsres = ws->sendBinary(data);
        if (!wsres.success)
            return -129;// DEF_XSOCKEVENT_SOCKETERROR
        return (int32_t)wsres.payloadSize;
    }

    std::shared_ptr<ix::WebSocket> get_websocket()
    {
        auto ws = websocket.lock();
        if (!ws) return nullptr;
        return ws;
    }

    void set_websocket(std::shared_ptr<ix::WebSocket> ws)
    {
        websocket = ws;
    }

    uint32_t client_handle = 0;

    uint64_t socknum;
    bool logged_in = false;

    uint64_t connect_counter = 0;
    uint64_t disconnect_counter = 0;
    uint64_t packet_counter = 0;
    uint64_t login_counter = 0;

    std::string account;
    std::string password;
    uint64_t account_id = 0;
    uint64_t id = 0;

    std::size_t write(stream_write & sw);

    uint32_t screenwidth{};
    uint32_t screenheight{};
    uint32_t screenwidth_v{};
    uint32_t screenheight_v{};
    uint16_t screen_size_x{};
    uint16_t screen_size_y{};



    bool isForceSet;
    time_t m_iForceStart;

    //////////////////////////////////////////////////////////////////////////

    int m_iMaxHP;
    struct
    {
        char  m_cCharName[11];
        char  m_cMapName[11];
        char  m_cSide;
        int   m_iLevel;
        bool  m_bUsed;
    } m_stFriendsList[50];
    bool m_bIsInitComplete2;
    bool m_bIsQuiet;
    int m_iAntiHackLevel;
    bool m_bHasFakeName;
    char m_cFakeName[11];
    char m_cLastChat[130];
    bool m_bHasMuted;
    int  m_iPlayerMuteCount;
    bool m_bInitComplete; //<- init data hack
    bool m_bViewGuildChat; //<- view guild chats
    char m_cWhisperView[11]; //<- view whispers
    bool m_bWhisperView; //<- view whispers
    bool m_bInhibition;
    bool m_bEnabled;
    uint32_t m_dwLastMagic;
    uint32_t m_dwLastRun;
    uint32_t m_dwLastWalk;
    uint32_t m_dwLastMotion;

    char m_cVar;




    bool bCreateNewParty();

    CClient();
    ~CClient();

    char m_cCharName[12];
    char m_cAccountName[12];
    char m_cAccountPassword[12];

    bool  m_bIsInitComplete;
    bool  m_bIsMsgSendAvailable;
    bool  m_bIsCheckingWhisperPlayer;

    char  m_cMapName[12];
    char  m_cMapIndex;
    short m_sX, m_sY;

    char  m_cGuildName[22];
    char  m_cLocation[12];
    int   m_iGuildRank;
    int   m_iGuildGUID;

    char  m_cDir;
    short m_sType;
    short m_sOriginalType;
    short m_sAppr1;
    short m_sAppr2;
    short m_sAppr3;
    short m_sAppr4;
    int   m_iApprColor;
    int   m_iStatus;

    uint32_t m_dwTime, m_dwHPTime, m_dwMPTime, m_dwSPTime, m_dwAutoSaveTime, m_dwHungerTime;

    char m_cSex, m_cSkin, m_cHairStyle, m_cHairColor, m_cUnderwear;

    int  m_iHP;
    int  m_iHPstock;
    int  m_iHPStatic_stock;
    int  m_iMP;
    int  m_iSP;
    int  m_iExp, m_iNextLevelExp;
    bool m_bIsKilled;

    int  m_iDefenseRatio;
    int  m_iHitRatio;

    //int  m_iHitRatio_ItemEffect_SM;
    //int  m_iHitRatio_ItemEffect_L;

    int  m_iDamageAbsorption_Armor[DEF_MAXITEMEQUIPPOS];
    int  m_iDamageAbsorption_Shield;

    int  m_iLevel;
    int  m_iStr, m_iInt, m_iVit, m_iDex, m_iMag, m_iCharisma;
    //char m_cLU_Str, m_cLU_Int, m_cLU_Vit, m_cLU_Dex, m_cLU_Mag, m_cLU_Char;
    int  m_iLU_Point;
    int  m_iLuck;

    int  m_iEnemyKillCount, m_iPKCount, m_iRewardGold;
    int  m_iCurWeightLoad;

    char m_cSide;

    char m_cAttackDiceThrow_SM;
    char m_cAttackDiceRange_SM;
    char m_cAttackDiceThrow_L;
    char m_cAttackDiceRange_L;
    char m_cAttackBonus_SM;
    char m_cAttackBonus_L;

    CItem * m_pItemList[DEF_MAXITEMS];
    POINT m_ItemPosList[DEF_MAXITEMS];
    CItem * m_pItemInBankList[DEF_MAXBANKITEMS];

    bool  m_bIsItemEquipped[DEF_MAXITEMS];
    short m_sItemEquipmentStatus[DEF_MAXITEMEQUIPPOS];
    char  m_cArrowIndex;

    char           m_cMagicMastery[DEF_MAXMAGICTYPE];
    unsigned char  m_cSkillMastery[DEF_MAXSKILLTYPE];
    int64_t m_cSkillId[DEF_MAXSKILLTYPE]{};

    int   m_iSkillSSN[DEF_MAXSKILLTYPE];
    bool  m_bSkillUsingStatus[DEF_MAXSKILLTYPE];
    int   m_iSkillUsingTimeID[DEF_MAXSKILLTYPE];

    char  m_cMagicEffectStatus[DEF_MAXMAGICEFFECTS];

    int   m_iWhisperPlayerIndex;
    char  m_cWhisperPlayerName[12];
    char  m_cProfile[256];

    int   m_iHungerStatus;

    uint32_t m_dwWarBeginTime;
    bool  m_bIsWarLocation;

    bool  m_bIsPoisoned;
    int   m_iPoisonLevel;
    uint32_t m_dwPoisonTime;

    int   m_iPenaltyBlockYear, m_iPenaltyBlockMonth, m_iPenaltyBlockDay;

    int   m_iFightzoneNumber, m_iReserveTime, m_iFightZoneTicketNumber;

    int   m_iAdminUserLevel;
    int   m_iRating;

    int   m_iTimeLeft_ShutUp;
    int   m_iTimeLeft_Rating;
    int   m_iTimeLeft_ForceRecall;
    int   m_iTimeLeft_FirmStaminar;

    bool  m_bIsOnServerChange;

    int   m_iExpStock;
    uint32_t m_dwExpStockTime;

    int   m_iAutoExpAmount;
    uint32_t m_dwAutoExpTime;

    uint32_t m_dwRecentAttackTime;
    uint32_t m_dwLastActionTime;

    int   m_iAllocatedFish;
    int   m_iFishChance;

    char  m_cIPaddress[21];
    bool  m_bIsSafeAttackMode;

    bool  m_bIsOnWaitingProcess;

    int   m_iSuperAttackLeft;
    int   m_iSuperAttackCount;

    short m_sUsingWeaponSkill;

    int   m_iManaSaveRatio;

    bool  m_bIsLuckyEffect;
    int   m_iSideEffect_MaxHPdown;

    int   m_iComboAttackCount;
    int   m_iDownSkillIndex;

    int   m_iMagicDamageSaveItemIndex;

    short m_sCharIDnum1, m_sCharIDnum2, m_sCharIDnum3;

    int   m_iPartyRank;
    int   m_iPartyMemberCount;
    int   m_iPartyGUID;
    struct
    {
        int  iIndex;
        char cName[11];

    } m_stPartyMemberName[DEF_MAXPARTYMEMBERS];

    int   m_iAbuseCount;

    //////////////////////////////////////////////////////////////////////////
    //Multi trade

    char  m_cExchangeMode;
    char  m_iExchangeCount;
    char  m_iExchangeAlterIndex[4];
    int   m_iExchangeComplete;

    bool  m_bIsExchangeMode;			// Is In Exchange Mode? 
    int   m_iExchangeH;					// Client ID to Exchanging with 
    char  m_cExchangeName[11];			// Name of Client to Exchanging with 
    char  m_cExchangeItemName[4][21];	// Name of Item to exchange 

    char  m_cExchangeItemIndex[4];		// ItemID to Exchange
    int   m_iExchangeItemAmount[4];		// Amount to exchange with

    bool  m_bIsExchangeConfirm;			// Has the user hit confirm? 
    int	  iExchangeCount;				//Keeps track of items which are on list

    //////////////////////////////////////////////////////////////////////////

    int   m_iQuest;
    int   m_iQuestID;
    int   m_iAskedQuest;
    int   m_iCurQuestCount;

    int   m_iQuestRewardType;
    int   m_iQuestRewardAmount;

    int   m_iContribution;

    bool  m_bQuestMatchFlag_Loc;
    bool  m_bIsQuestCompleted;

    int   m_iCustomItemValue_Attack;
    int   m_iCustomItemValue_Defense;

    int   m_iMinAP_SM;
    int   m_iMinAP_L;

    int   m_iMaxAP_SM;
    int   m_iMaxAP_L;

    bool  m_bIsNeutral;
    bool  m_bIsObserverMode;

    int   m_iSpecialEventID;

    int   m_iSpecialWeaponEffectType;
    int   m_iSpecialWeaponEffectValue;

    int   m_iAddHP, m_iAddSP, m_iAddMP;
    int   m_iAddAR, m_iAddPR, m_iAddDR;
    int   m_iAddMR, m_iAddAbsPD, m_iAddAbsMD;
    int   m_iAddCD, m_iAddExp, m_iAddGold;

    int   m_iAddTransMana, m_iAddChargeCritical;

    int   m_iAddResistMagic;
    int   m_iAddPhysicalDamage;
    int   m_iAddMagicalDamage;

    int   m_iAddAbsAir;
    int   m_iAddAbsEarth;
    int   m_iAddAbsFire;
    int   m_iAddAbsWater;

    int   m_iLastDamage;

    int   m_iMoveMsgRecvCount, m_iAttackMsgRecvCount, m_iRunMsgRecvCount, m_iSkillMsgRecvCount;
    uint32_t m_dwMoveLAT, m_dwRunLAT, m_dwAttackLAT;

    int   m_iSpecialAbilityTime;
    bool  m_bIsSpecialAbilityEnabled;
    uint32_t m_dwSpecialAbilityStartTime;
    int   m_iSpecialAbilityLastSec;

    int   m_iSpecialAbilityType;

    int   m_iSpecialAbilityEquipPos;
    bool  m_bIsAdminCreateItemEnabled;
    bool  m_bIsAdminCommandEnabled;
    int   m_iAlterItemDropIndex;

    int   m_iWarContribution;

    uint32_t m_dwSpeedHackCheckTime;
    int   m_iSpeedHackCheckExp;

    uint32_t m_dwInitCCTimeRcv;
    uint32_t m_dwInitCCTime;

    char  m_cLockedMapName[12];
    int   m_iLockedMapTime;
    int   m_iDeadPenaltyTime;

    int   m_iCrusadeDuty;
    uint32_t m_dwCrusadeGUID;

    struct
    {
        char cType;
        char cSide;
        short sX, sY;
    } m_stCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
    int m_iCSIsendPoint;

    char m_cSendingMapName[12];
    bool m_bIsSendingMapStatus;

    int  m_iConstructionPoint;

    char m_cConstructMapName[12];
    int  m_iConstructLocX, m_iConstructLocY;

    uint32_t m_dwFightzoneDeadTime;

    bool m_bIsBankModified;

    uint32_t m_dwCharID;

    int m_iPartyID;
    int m_iPartyStatus;
    int m_iReqJoinPartyClientH;
    char m_cReqJoinPartyName[12];

    int m_iGizonItemUpgradeLeft;

    uint32_t m_dwAttackFreqTime, m_dwMagicFreqTime, m_dwMoveFreqTime;
    bool m_bIsMoveBlocked;
    bool m_bIsPlayerCivil;
    bool m_bIsAttackModeChange;
    int  m_iIsOnTown;
    bool m_bIsOnShop;
    bool m_bIsHunter;
    bool m_bIsOnTower;
    bool m_bIsOnWarehouse;
    bool m_bIsInBuilding;

    uint32_t m_dwWarmEffectTime;

    bool m_bHeroArmorBonus;

    bool m_bMagicPauseTime = false;
};
