//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#if defined(WIN32)
#include <SDKDDKVer.h>
#include <tchar.h>
#include <direct.h>
#include <process.h>
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <memory.h>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>
#include <cstdint>
#include <cstdarg>
#include <algorithm>
#include <cstdio>
#include <cstring>

static inline constexpr uint16_t UPPER_VERSION = 0;
static inline constexpr uint16_t LOWER_VERSION = 0;
static inline constexpr uint16_t PATCH_VERSION = 1;

enum DaysOfWeek
{
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

#define MAXSIDES 3

static inline const std::string sideName[MAXSIDES] = { "Traveler", "Aresden", "Elvine" };
static inline const std::string sideMap[MAXSIDES] = { "default", "aresden", "elvine" };
static inline const std::string sideMapJail[MAXSIDES] = { "default", "arejail", "elvjail" };
static inline const std::string sideMapFarm[MAXSIDES] = { "default", "arefarm", "elvfarm" };
static inline const std::string sideMapRes[MAXSIDES] = { "default", "resurr1", "resurr2" };

#define GUILDPARTYBONUS_RANGE_X		FLAGRANGE_X
#define GUILDPARTYBONUS_RANGE_Y		FLAGRANGE_Y

#define DEF_MAXCRUSADESTRUCTURES		300

#define MAXCLIENTS			10000
#define MAXNPCS				15000
#define MAXMAPS				120
#define MAXBUILDITEMS		300
#define CLIENTTIMEOUT		18000
#define AUTOSAVETIME		3*60000
#define HPUPTIME			8000
#define MPUPTIME			15000 // Changed 9->15 Sec
#define SPUPTIME			10000

#define HUNGERTIME			1*60000
#define POISONTIME			12000
#define SUMMONTIME			5*60000
#define NOTICETIME			80000
#define PLANTTIME			5*60000

#define EXPSTOCKTIME		10000
#define AUTOEXPTIME			6*60000



#define TOTALLEVELUPPOINT	7 // Level Stat Bonus xRisenx

#define MAXDYNAMICOBJECTS	60000
#define MAXDELAYEVENTS		60000

#define SSN_LIMIT_MULTIPLY_VALUE	2

#define MAXNOTIFYMSGS		1000
#define NIGHTTIME			40

#define CHARPOINTLIMIT		700 // Max Stats xRisenx
#define RAGPROTECTIONTIME	7000
#define MAXREWARDGOLD		99999999

#define MAXSUPERATTACK		10000 // Max Critical Limit xRisenx
#define MAXLUCKY			5000 // Max Lucky Limit xRisenx
#define MAXREP				10000 // Max Rep Limit xRisenx
#define MAXNEGAREP			-10000 // Max Negative Rep Limit xRisenx

#define ATTACKAI_NORMAL				1
#define ATTACKAI_EXCHANGEATTACK		2
#define ATTACKAI_TWOBYONEATTACK		3

#define MAXFISHS					200
#define MAXMINERALS					200
#define MAXENGAGINGFISH				30
#define MAXPOTIONTYPES				500
#define MAXCRAFTING					120

#define MOBEVENTTIME				5*60000
#define MAXQUESTTYPE				200

#define MAXSUBLOGSOCK				5

#define ITEMLOG_GIVE				1
#define ITEMLOG_DROP				2
#define ITEMLOG_GET					3
#define ITEMLOG_DEPLETE				4
#define ITEMLOG_NEWGENDROP			5
#define ITEMLOG_DUPITEMID			6
#define ITEMLOG_BUY					7
#define ITEMLOG_SELL				8
#define ITEMLOG_RETRIEVE			9
#define ITEMLOG_DEPOSIT				10
#define ITEMLOG_EXCHANGE			11
#define ITEMLOG_MAGICLEARN			12
#define ITEMLOG_MAKE				13
#define ITEMLOG_SUMMONMONSTER		14
#define ITEMLOG_POISONED			15
#define ITEMLOG_SKILLLEARN			16
#define ITEMLOG_REPAIR				17
#define ITEMLOG_JOINGUILD           18
#define ITEMLOG_BANGUILD            19
#define ITEMLOG_RESERVEFIGZONE      20	//  "
#define ITEMLOG_APPLY               21	//  "
#define ITEMLOG_SHUTUP              22	//  "
#define ITEMLOG_CLOSECONN			23	//  "
#define ITEMLOG_SPELLFIELD			24	//  "
#define ITEMLOG_CREATEGUILD			25	//  "
#define ITEMLOG_GUILDDISMISS		26	//  "
#define ITEMLOG_SUMMONPLAYER        27	//  "
#define ITEMLOG_CREATE				28	//  "
#define ITEMLOG_UPGRADEFAIL         29
#define ITEMLOG_UPGRADESUCCESS      30
#define ITEMLOG_MAILRETRIEVE			31
#define ITEMLOG_MAILSEND				32
#define ITEMLOG_GWHDEPOSIT				33
#define ITEMLOG_GWHRETRIEVE			34


#define ITEMSPREAD_RANDOM			1
#define ITEMSPREAD_FIXED			2
//#define MAX_NPCITEMDROP				95


#define CRUSADELOG_ENDCRUSADE       1
#define CRUSADELOG_STARTCRUSADE     2
#define CRUSADELOG_SELECTDUTY       3
#define CRUSADELOG_GETEXP           4
#define CRUSADELOG_APOCALYPSE		5

#define PKLOG_BYPLAYER				1
#define PKLOG_BYPK					2
#define PKLOG_BYENERMY				3
#define PKLOG_BYNPC					4
#define PKLOG_BYOTHER				5
#define PKLOG_REDUCECRIMINAL        6


#define MAXDUPITEMID				100

#define MAXGUILDS					1000
#define MAXONESERVERUSERS			800
#define MAXGATESERVERSTOCKMSGSIZE	30000


#define MAXCONSTRUCTNUM			20 //raised from 10 to 20 to in corporate 4mana stones

#define MAXSCHEDULE				15 // Raised from 10 to 15



#define MAXFIGHTZONE 10

//============================
// #define LEVELLIMIT		130
#define LEVELLIMIT		50
//============================

//============================
#define MAGICHITRATIO 85.5f // xRisenx
//#define MAGICHITRATIO 204.5f // xRisenx
//============================

//============================
#define MINIMUMHITRATIO 15
//============================

//============================
#define MAXIMUMHITRATIO	99
//============================

//============================
#define PLAYERMAXLEVEL	205
//============================

//============================
#define GMGMANACONSUMEUNIT	15
//============================

#define MAXCONSTRUCTIONPOINT 30000

#define MAXWARCONTRIBUTION	 500000

#define MAXPARTYNUM			200
//#define MAXARENAPLAYERS     200 // Gladiator Arena xRisenx

#define ALLSIDES			100
#define ATTACKER			101
#define DEFENDER			102

//v2.19 2002-11-19
#define PK					0
#define NONPK				1
#define NEVERNONPK			2

#define MAX_CRUSADESUMMONMOB	100

#define CRUSADELOG_ENDHELDENIAN		11
#define CRUSADELOG_STARTHELDENIAN	12
#define CRUSADELOG_GETHELDENIANEXP	14
#define CRUSADELOG_HELDENIANVICTORY	15
#define CRUSADELOG_HELDENIANWARNOW	16

#define MAX_HELDENIANTOWER			200
#define MAXHELDENIAN				10
#define MAXHELDENIANSUMMONPOINT		12000
#ifdef RestatScroll
#define STR 0x01 //Correct
#define DEX 0x02 //Correct
#define INT	0x03 //Correct
#define VIT 0x04 //Correct
#define MAG	0x05 //Correct
#define CHR 0x06 //Correct
#endif

#define DEF_SOCKETTHREADS 2
#pragma endregion


#define MAXWAYPOINTS			10

#define MOVETYPE_STOP			0
#define MOVETYPE_SEQWAYPOINT	1
#define MOVETYPE_RANDOMWAYPOINT	2
#define MOVETYPE_FOLLOW			3
#define MOVETYPE_RANDOMAREA		4
#define MOVETYPE_RANDOM			5
#define MOVETYPE_GUARD			6

#define BEHAVIOR_STOP			0
#define BEHAVIOR_MOVE			1
#define BEHAVIOR_ATTACK			2
#define BEHAVIOR_FLEE			3
#define BEHAVIOR_DEAD			4


enum NPCType
{
    NPC_INVALID = -1,
    NPC_SLIME = 10,
    NPC_RABBIT = 55,
    NPC_CAT = 56,
    NPC_GIANT_ANT = 16,
    NPC_AMPHIS = 22,
    NPC_ORC = 14,
    NPC_ZOMBIE = 18,
    NPC_SCORPION = 17,
    NPC_SKELETON = 11,
    NPC_ORC_MAGE = 14,
    NPC_CLAY_GOLEM = 23,
    NPC_STONE_GOLEM = 12,
    NPC_HELLBOUND = 27,
    NPC_GIANT_FROG = 57,
    NPC_CYCLOPS = 13,
    NPC_TROLL = 28,
    NPC_BEHOLDER = 53,
    NPC_CANNIBAL_PLANT = 60,
    NPC_OGRE = 29,
    NPC_MOUNTAIN_GIANT = 58,
    NPC_LICHE = 30,
    NPC_STALKER = 48,
    NPC_WEREWOLF = 33,
    NPC_DARK_ELF = 54,
    NPC_ETTIN = 59,
    NPC_DEMON = 31,
    NPC_UNICORN = 32,
    NPC_GAGOYLE = 52,
    NPC_HELLCLAW = 49,
    NPC_TIGERWORM = 50,
    NPC_RUDOLPH = 61,
    NPC_DIREBOAR = 62,
    NPC_FROST = 63,
    NPC_ICE_GOLEM = 65,
    NPC_WYVERN = 66,
    NPC_BARLOG = 70,
    NPC_CENTAURUS = 71,
    NPC_CLAW_TURTLE = 72,
    NPC_FIRE_WYVERN = 73,
    NPC_GIANT_CRAYFISH = 74,
    NPC_GIANT_LIZARD = 75,
    NPC_GIANT_PLANT = 76,
    NPC_MASTERMAGE_ORC = 77,
    NPC_MINOTAURS = 78,
    NPC_NIZIE = 79,
    NPC_TENTOCLE = 80,
    NPC_ABADDON = 81,
    NPC_CROPS = 64,

    NPC_MCGAFFIN = 67,
    NPC_PERRY = 68,
    NPC_DEVLIN = 69,

    NPC_DUMMY = 34,
    NPC_ATTACK_DUMMY = 34,
    NPC_ENERGY_SPHERE = 35,

    NPC_SHOPKEEPER_W = 15,
    NPC_GANDLF = 19,
    NPC_HOWARD = 20,

    NPC_GUARD = 21,

    NPC_TOM = 24,
    NPC_WILLIAM = 25,
    NPC_KENNEDY = 26,

    NPC_AGT = 36,
    NPC_CGT = 37,
    NPC_MS = 38,
    NPC_DT = 39,
    NPC_ESG = 40,
    NPC_GMG = 41,
    NPC_MANASTONE = 42,

    NPC_LWB = 43,
    NPC_GHK = 44,
    NPC_GHKABS = 45,
    NPC_TK = 46,
    NPC_BG = 47,

    NPC_YB = 1,
    NPC_CP = 51,


    NPC_GAIL = 90,

    NPC_SOR = 82,
    NPC_ATK = 83,
    NPC_ELF = 84,
    NPC_DSK = 85,
    NPC_HBT = 86,
    NPC_CT = 87,
    NPC_BAR = 88,
    NPC_AGC = 89,

    NPC_GATE = 91,
    NPC_GATE_A = 91,
    NPC_GATE_E = 91,

    NPC_SCARECROW = 92,

    NPC_GHOST = 93,
    NPC_VAMPIRE = 94,

    NPC_FIRE_PIXIE = 95,
    NPC_ICE_PIXIE = 96,
    NPC_EARTH_PIXIE = 97,
    NPC_AIR_PIXIE = 98,

    NPC_GUARDB_ARESDEN = 99,
    NPC_GUARDB_ELVINE = 100,
    NPC_WILLIAMB = 101,
    NPC_PRINCESS = 102,
    NPC_AS = 103,
    //NPC_ORCLORD = 103,
    NPC_DARKUNICORN = 104,
    NPC_BLACK_DEMON = 105,
    NPC_EARTH_WYVERN = 106,
    NPC_WIND_WYVERN = 107,
    NPC_DARK_WYVERN = 108,
    NPC_GRASS_WYVERN = 109,
    NPC_SKY_WYVERN = 110,
    NPC_SEA_WYVERN = 111,
    NPC_METAL_WYVERN = 112,
    NPC_VOID_WYVERN = 113,
    NPC_ETERNAL_DRAGON = 114,
    /*NPC_MOTHER_DRAGON = 115,
    NPC_HELL_DRAGON = 116,
    NPC_ALLIGATOR = 117,
    NPC_PUMA = 118,
    NPC_ZOMBIE_KING = 119,
    NPC_SLIME_KING = 120,
    NPC_SLIME_PRINESS = 121,*/
    NPC_ENRAGED_TROLL = 122,
    NPC_ENRAGED_CYCLOPS = 123,
    NPC_ENRAGED_STALKER = 124,
    NPC_ENRAGED_GAGOYLE = 125,
    NPC_ENRAGED_HELLCLAW = 126,
    NPC_ENRAGED_TIGERWORM = 127
};


// for Client Data
#define MAXTELEPORTLOC		200
#define MAXWAYPOINTCFG		200
#define MAXMGAR				50
#define MAXNMR				50
#define MAXSPOTMOBGENERATOR	100
#define MAXFISHPOINT		200
#define MAXMINERALPOINT		200
#define MAXOCCUPYFLAG		20001 //3001
#define MAXINITIALPOINT		20

#define MAPTYPE_NORMAL				0
#define MAPTYPE_NOPENALTY_NOREWARD	1
#define MAXENERGYSPHERES			10

#define MAXSTRATEGICPOINTS			200
#define MAXSECTORS					60
#define MAXSTRIKEPOINTS				20

#define MAXITEMEVENTS				50

#define MAXHELDENIANDOOR	10 
#define MAXDYNAMICGATES		10
#define MAXHELDENIANTOWER	200

enum ApocMobGenType
{
    AMGT_NONE,
    AMGT_OPENGATE,
    AMGT_SPAWNBOSS
};



// for Client Data
#define DEF_MAPDATASIZEX		30
#define DEF_MAPDATASIZEY		25
#define DEF_MAXTELEPORTLOC		200
#define DEF_MAXWAYPOINTCFG		200
#define DEF_MAXMGAR				50
#define DEF_MAXNMR				50
#define DEF_MAXSPOTMOBGENERATOR	100
#define DEF_MAXFISHPOINT		200
#define DEF_MAXMINERALPOINT		200
#define DEF_MAXOCCUPYFLAG		20001 //3001
#define	DEF_MAXINITIALPOINT		20


#define DEF_MAPTYPE_NORMAL				0
#define DEF_MAPTYPE_NOPENALTY_NOREWARD	1
#define DEF_MAXENERGYSPHERES			10

#define DEF_MAXSTRATEGICPOINTS			200
#define DEF_MAXSECTORS					60
#define DEF_MAXSTRIKEPOINTS				20

//#define DEF_MAXITEMEVENTS				50


#define DEF_CLIENTSOCKETBLOCKLIMIT	15

#define DEF_MSGBUFFERSIZE	30000
#define DEF_MAXITEMS		50
//#define DEF_MAXBANKITEMS	120
#define DEF_MAXGUILDSMAN	128

#define	DEF_MAXMAGICTYPE	100
#define DEF_MAXSKILLTYPE	60

#define DEF_MAXTELEPORTLIST	20

//#define DEF_MAXPARTYMEMBERS	8

#define DEF_SPECABLTYTIMESEC	1200

#define DEF_MAXMAGICEFFECTS			100

enum faction
{
    neutral,
    aresden,
    elvine,
    hostile
};

enum magic_type
{
    mt_damage_spot = 1,
    mt_hpup_spot,
    mt_damage_area,
    mt_spdown_spot,
    mt_spdown_area,
    mt_spup_spot,
    mt_spup_area,
    mt_teleport,
    mt_summon,
    mt_create,
    mt_protect,
    mt_hold_object,
    mt_invisibility,
    mt_create_dynamic,
    mt_possession,
    mt_confuse,
    mt_poison,
    mt_berserk,
    mt_damage_linear,
    mt_polymorph,
    mt_damage_area_nospot,
    mt_tremor,
    mt_ice,
    mt_damage_spot_spdown,
    mt_damage_area_nospot_spdown,
    mt_damage_area_armor_break,
    mt_ice_linear,
    mt_cancellation,
    mt_inhibition,
    mt_damage_linear_spdown,
    mt_scan,
    mt_resurrection,
    mt_damage_area_move,
    mt_sotg_linear, // ?
    mt_cotg_linear, // ?
    mt_fsw_linear, // ?
    mt_mb_linear // ?
};

#define MAXMAGICEFFECTS			40

enum magicValuesIndices
{
    MAGICV_TYPE = 0,
    MAGICV_THROW = 0,
    MAGICV_RANGE = 1,
    MAGICV_POISONLVL = 1,
    MAGICV_BONUS = 2,
    MAGICV_LINEARTHROW = 3,
    MAGICV_LINEARRANGE = 4,
    MAGICV_LINEARBONUS = 5,
    MAGICV_ICEDURATION = 6,
    MAGICV_ARMORDEC = 6,
    MAGICV_DYNAMICOBJ = 6,
    MAGICV_DYNAMICTYPE = 7
};

enum magicTypes
{
    MAGICTYPE_NONE,
    MAGICTYPE_DAMAGE_SPOT,
    MAGICTYPE_HPUP_SPOT,
    MAGICTYPE_DAMAGE_AREA,
    MAGICTYPE_SPDOWN_SPOT,	// unused
    MAGICTYPE_SPDOWN_AREA,
    MAGICTYPE_SPUP_SPOT,		// unused
    MAGICTYPE_SPUP_AREA,
    MAGICTYPE_TELEPORT,
    MAGICTYPE_SUMMON,
    MAGICTYPE_CREATE,
    MAGICTYPE_PROTECT,
    MAGICTYPE_HOLDOBJECT,
    MAGICTYPE_INVISIBILITY,
    MAGICTYPE_CREATE_DYNAMIC,
    MAGICTYPE_POSSESSION,
    MAGICTYPE_CONFUSE,
    MAGICTYPE_POISON,
    MAGICTYPE_BERSERK,
    MAGICTYPE_DAMAGE_LINEAR,
    MAGICTYPE_POLYMORPH,
    MAGICTYPE_DAMAGE_AREA_NOSPOT,
    MAGICTYPE_TREMOR,
    MAGICTYPE_ICE,
    MAGICTYPE_DAMAGE_SPOT_SPDOWN,		// no spells
    MAGICTYPE_DAMAGE_AREA_NOSPOT_SPDOWN,

    MAGICTYPE_DAMAGE_AREA_ARMOR_BREAK,
    MAGICTYPE_ICE_LINEAR,

    MAGICTYPE_CANCELLATION,
    MAGICTYPE_INHIBITION,
    MAGICTYPE_DAMAGE_LINEAR_SPDOWN,
    MAGICTYPE_SCAN,
    MAGICTYPE_RESURRECTION,
    MAGICTYPE_DAMAGE_AREA_MOVE,
    MAGICTYPE_SOTG_LINEAR,
    MAGICTYPE_COTG_LINEAR,
    MAGICTYPE_FSW_LINEAR,
    MAGICTYPE_MB_LINEAR,

    MAGICTYPE_INVINCIBILITY_SOFT, // shows damage being taken, does not take damage
    MAGICTYPE_INVINCIBILITY_HARD  // does not take or show damage
};

enum magicChldProtect
{
    MAGICPROTECT_PFA = 1,
    MAGICPROTECT_PFM = 2,
    MAGICPROTECT_DS = 3,
    MAGICPROTECT_GDS = 4,
    MAGICPROTECT_AMP = 5,
    //MAGICPROTECT_GPFM =		6
};


#define DEF_MAXWAYPOINTS			10

enum class move_type
{
    stop,
    sequential_waypoint,
    random_waypoint,
    follow,
    random_area,
    random,
    guard
};

enum class behavior
{
    stop,
    move,
    attack,
    flee,
    dead
};

enum owner_type
{
    ot_none,
    ot_player,
    ot_npc,
    ot_player_indirect,
    ot_npc_indirect
};

enum class weather
{
    sunny,
    light_rain,
    medium_rain,
    heavy_rain,
    light_snow,
    medium_snow,
    heavy_snow
};

#define DEF_ATTRIBUTE_EARTH			1
#define DEF_ATTRIBUTE_AIR			2
#define DEF_ATTRIBUTE_FIRE			3
#define DEF_ATTRIBUTE_WATER			4

enum motion_id
{
    MSGIDTYPE_MOTION = 1 << 15,
    MOTION_STOP = MSGIDTYPE_MOTION,
    MOTION_MOVE,
    MOTION_RUN,
    MOTION_ATTACK,
    MOTION_MAGIC,
    MOTION_GET_ITEM,
    MOTION_DAMAGE,
    MOTION_DAMAGE_MOVE,
    MOTION_ATTACK_MOVE,
    MOTION_DYING,
    MOTION_NULL,
    MOTION_EVENT_CONFIRM,
    MOTION_EVENT_REJECT,
    MOTION_MAX
};

enum class log_message_id
{
    login = 25,
    //create_new_account = 26,
    create_new_character = 28,
    enter_game = 29,
    delete_character = 31
};

enum class log_rsp_message_id
{
    log = 27,
    enter_game = 30
};

enum message_id
{
    NOTIFY,
    REQUEST_INITPLAYER,
    RESPONSE_INITPLAYER,

    REQUEST_INITDATA,
    RESPONSE_INITDATA,

    RESPONSE_MOTION,

    EVENT_COMMON,
    COMMAND_COMMON,

    ITEMCONFIGURATIONCONTENTS,
    NPCCONFIGURATIONCONTENTS,
    MAGICCONFIGURATIONCONTENTS,
    SKILLCONFIGURATIONCONTENTS,
    PLAYERITEMLISTCONTENTS,
    PORTIONCONFIGURATIONCONTENTS,
    PLAYERCHARACTERCONTENTS,
    QUESTCONFIGURATIONCONTENTS,
    BUILDITEMCONFIGURATIONCONTENTS,
    DUPITEMIDFILECONTENTS,
    NOTICEMENTFILECONTENTS,

    COMMAND_CHECKCONNECTION,
    COMMAND_CHATMSG,

    REQUEST_REGISTERGAMESERVER,
    RESPONSE_REGISTERGAMESERVER,
    REQUEST_REGISTERDBSERVER,
    RESPONSE_REGISTERDBSERVER,

    REQUEST_LOGIN,
    REQUEST_CREATENEWACCOUNT,
    RESPONSE_LOG,
    REQUEST_CREATENEWCHARACTER,
    REQUEST_ENTERGAME,
    RESPONSE_ENTERGAME,
    REQUEST_DELETECHARACTER,
    REQUEST_CREATENEWGUILD = 32,
    RESPONSE_CREATENEWGUILD,
    REQUEST_DISBANDGUILD,
    RESPONSE_DISBANDGUILD,

    REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN,
    REQUEST_UPDATEGUILDINFO_DELGUILDSMAN,

    REQUEST_CIVILRIGHT,
    RESPONSE_CIVILRIGHT,

    REQUEST_CHANGEPASSWORD,
    RESPONSE_CHANGEPASSWORD,

    REQUEST_INPUTKEYCODE,
    RESPONSE_INPUTKEYCODE,

    REQUEST_ANGEL,
    REQUEST_DKITEM,

    REQUEST_UPDATE_FRIENDS,

    REQUEST_LGNPTS,
    RESPONSE_LGNPTS,
    REQUEST_LGNSVC,
    RESPONSE_LGNSVC,
    SUBCASH,
    ADDCASH,

    REQUEST_SETRECALLPNT,

    CONFIRMBUMP,

    REQUEST_PLAYERDATA,
    RESPONSE_PLAYERDATA,
    RESPONSE_SAVEPLAYERDATA_REPLY,
    REQUEST_SAVEPLAYERDATA,
    REQUEST_SAVEPLAYERDATA_REPLY,
    REQUEST_SAVEPLAYERDATALOGOUT,
    REQUEST_NOSAVELOGOUT,

    REQUEST_RETRIEVEITEM,
    RESPONSE_RETRIEVEITEM,

    REQUEST_RETRIEVEITEM_GUILDBANK,
    RESPONSE_RETRIEVEITEM_GUILDBANK,

    REQUEST_FULLOBJECTDATA,

    GUILDNOTIFY,
    REQUEST_TELEPORT,

    LEVELUPSETTINGS,
    DYNAMICOBJECT,

    GAMESERVERALIVE,

    GAMESERVERDOWN,
    TOTALGAMESERVERCLIENTS,

    ENTERGAMECONFIRM,

    REQUEST_FIGHTZONE_RESERVE,
    RESPONSE_FIGHTZONE_RESERVE,

    CONFIRMEDLOGIN,

    GAMESERVERINITALIZED,

    ANNOUNCEACCOUNT,

    ACCOUNTINFOCHANGE,
    IPINFOCHANGE,

    GAMESERVERSHUTDOWNED,
    ANNOUNCEACCOUNTNEWPASSWORD,

    REQUEST_IPIDSTATUS,
    RESPONSE_IPIDSTATUS,
    REQUEST_ACCOUNTCONNSTATUS,
    RESPONSE_ACCOUNTCONNSTATUS,
    REQUEST_CLEARACCOUNTCONNSTATUS,
    RESPONSE_CLEARACCOUNTCONNSTATUS,

    REQUEST_FORCEDISCONECTACCOUNT,
    REQUEST_NOCOUNTINGSAVELOGOUT,

    OCCUPYFLAGDATA,
    REQUEST_SAVEARESDENOCCUPYFLAGDATA,
    REQUEST_SAVEELVINEOCCUPYFLAGDATA,

    ARESDENOCCUPYFLAGSAVEFILECONTENTS,
    ELVINEOCCUPYFLAGSAVEFILECONTENTS,

    REQUEST_SETITEMPOS,

    SENDSERVERSHUTDOWNMSG,
    ITEMLOG,

    REQUEST_NOTICEMENT,
    RESPONSE_NOTICEMENT,

    REGISTER_WORLDSERVER,
    REGISTER_WORLDSERVERSOCKET,
    REGISTER_WORLDSERVER_GAMESERVER,

    REQUEST_CHARINFOLIST,
    RESPONSE_CHARINFOLIST,

    REQUEST_REMOVEGAMESERVER,
    REQUEST_CLEARACCOUNTSTATUS,

    REQUEST_SETACCOUNTINITSTATUS,
    REQUEST_SETACCOUNTWAITSTATUS,

    REQUEST_CHECKACCOUNTPASSWORD,
    WORLDSERVERACTIVATED,

    REQUEST_PANNING,
    RESPONSE_PANNING,

    REQUEST_RESTART,
    RESPONSE_REGISTER_WORLDSERVERSOCKET,

    REQUEST_BLOCKACCOUNT,
    IPTIMECHANGE,
    ACCOUNTTIMECHANGE,
    REQUEST_IPTIME,
    RESPONSE_IPTIME,

    REQUEST_SELLITEMLIST,

    REQUEST_ALL_SERVER_REBOOT,
    REQUEST_EXEC_1DOTBAT,
    REQUEST_EXEC_2DOTBAT,
    MONITORALIVE,

    COLLECTEDMANA,
    METEORSTRIKE,

    SERVERSTOCKMSG,
    GATEWAY_CURRENTCONN,
    GETMINIMUMLOADGATEWAY,

    CONFIRMFORCELOGOUT,

    REQUEST_TELEPORT_LIST,
    RESPONSE_TELEPORT_LIST,
    REQUEST_CHARGED_TELEPORT,
    RESPONSE_CHARGED_TELEPORT,

    //====HG CONFIG==============================================================================
    REQUEST_REGISTERGAMESERVERSOCKET,

    GAMEMASTERLOG,
    GAMEITEMLOG,
    GAMECRUSADELOG,
    PLAYERLOG,

    MANAGER_INIT,
    MANAGER_SHUTDOWN,

    TELEPORTLISTCONTENTS,
    UPDATECONFIGFILES,

    RESPONSE_CHECKCONNECTION,

    CRAFTINGCONFIGURATIONCONTENTS,

    REQUEST_HELDENIAN_WINNER,
    REQUEST_HELDENIAN_SCROLL,

    // Ressurection
    REQUEST_RESURRECTPLAYER_YES,
    REQUEST_RESURRECTPLAYER_NO,

    // MJ Stats Change
    STATECHANGEPOINT,
    GUILDINFO,
    REQUEST_GUILDBANK,
    RESPONSE_GUILDBANK,
    GUILDSMANSTATUS,
    REQGUILDSUMMONS,
    RSPGUILDSUMMONS,
    GUILDCONTRIBUTE,
    GUILDUPGRADE,
    GUILDSMANCHANGE,
    REQ_GUILDBOARD,
    RSP_GUILDBOARD,
    REQ_GUILDPOSTDATA,
    RSP_GUILDBOARDPOST,
    REQ_DELETEGUILDPOST,
    REQ_POSTGUILDBOARD,
    RSP_POSTGUILDBOARD,

    REQ_MAILBOX,
    RSP_MAILBOX,
    REQ_SENDMAIL,
    RSP_SENDMAIL,
    REQ_MAILDATA,
    RSP_MAILDATA,
    REQ_DELETEMAIL,
    REQ_RETRIEVEMAILITEM,
    RSP_RETRIEVEMAILITEM,

    REQUEST_LOAD_GUILDINFO,
    RESPONSE_LOAD_GUILDINFO,
    SAVE_GUILDINFO,
    REQUEST_SAVE_GUILDBANK,

    REQUEST_QUEST_LIST,
    RESPONSE_QUEST_LIST,
    REQUEST_ACCEPT_QUEST,

    PINGMAP,

    REQUEST_TITLES,
    SETTITLE,

    REQUEST_CRITICALS,

    SETITEMTO_AUCTIONHOUSE_WORLD,
    SETITEMTO_AUCTIONHOUSE_WORLD2,
    AUCTIONHOUSE_SENDITEMLIST,

    MODIFYTILE,
    MAX,
};

enum class common_type_id
{
    ITEMDROP = 0x0a01,
    EQUIPITEM,
    REQ_LISTCONTENTS,
    REQ_PURCHASEITEM,
    GIVEITEMTOCHAR,
    JOINGUILDAPPROVE,
    JOINGUILDREJECT,
    DISMISSGUILDAPPROVE,
    DISMISSGUILDREJECT,
    RELEASEITEM,
    TOGGLECOMBATMODE,
    SETITEM,
    MAGIC,
    REQ_STUDYMAGIC,
    REQ_TRAINSKILL,
    REQ_GETREWARDMONEY,
    REQ_USEITEM,
    REQ_USESKILL,
    REQ_SELLITEM,
    REQ_REPAIRITEM,
    REQ_SELLITEMCONFIRM,
    REQ_REPAIRITEMCONFIRM,
    REQ_GETFISHTHISTIME,
    TOGGLESAFEATTACKMODE,
    REQ_CREATEPOTION,
    TALKTONPC,
    REQ_SETDOWNSKILLINDEX,
    REQ_GETOCCUPYFLAG,
    REQ_GETHEROMANTLE,
    EXCHANGEITEMTOCHAR,
    SETEXCHANGEITEM,
    CONFIRMEXCHANGEITEM,
    CANCELEXCHANGEITEM,
    QUESTACCEPTED,
    BUILDITEM,
    CRAFTITEM,
    GETMAGICABILITY,
    CLEARGUILDNAME,

    REQ_GETOCCUPYFIGHTZONETICKET,
    BANGUILD,

    REQUEST_ACCEPTJOINPARTY,
    REQUEST_JOINPARTY,
    RESPONSE_JOINPARTY,
    REQUEST_ACTIVATESPECABLTY,
    REQUEST_CANCELQUEST,
    REQUEST_SELECTCRUSADEDUTY,
    REQUEST_MAPSTATUS,
    REQUEST_HUNTMODE,

    SETGUILDTELEPORTLOC,
    GUILDTELEPORT,
    SUMMONWARUNIT,
    SETGUILDCONSTRUCTLOC,
    UPGRADEITEM,
    REQGUILDNAME,
    REQ_REPAIRALL,
    REQ_REPAIRALLCONFIRM,

    REQ_CREATESLATE,
    REQTITLE,
    REQARENASTATUS,
    REQ_GETTRADEEKITEM,
    REQ_GETTRADEBALLITEM,
    HACK,
    DLLHACK,
    MAX,
};

// enum msg_type
// {
//     MSGTYPE_CONFIRM = 0x0F14,
//     MSGTYPE_REJECT = 0x0F15
// };

enum class log_res_msg
{
    LOG_RES_MSG_CONFIRM = 0x0f14,
    LOG_RES_MSG_REJECT = 0x0f15,
    PASSWORDMISMATCH = 0x0f16,
    NOTEXISTINGACCOUNT = 0x0f17,
    NEWACCOUNTCREATED = 0x0f18,
    NEWACCOUNTFAILED = 0x0f19,
    ALREADYEXISTINGACCOUNT = 0x0f1a,
    NOTEXISTINGCHARACTER = 0x0f1b,
    NEWCHARACTERCREATED = 0x0f1c,
    NEWCHARACTERFAILED = 0x0f1d,
    ALREADYEXISTINGCHARACTER = 0x0f1e,
    CHARACTERDELETED = 0x0f1f,
    NOTENOUGHPOINT = 0x0f30,
    ACCOUNTLOCKED = 0x0f31,
    SERVICENOTAVAILABLE = 0x0f32,
    PASSWORDCHANGESUCCESS = 0x0a00,
    PASSWORDCHANGEFAIL = 0x0a01,
    NOTEXISTINGWORLDSERVER = 0x0a02,
    INPUTKEYCODE = 0x0a03,
    REALACCOUNT = 0x0a04,
    FORCECHANGEPASSWORD = 0x0a05,
    INVALIDKOREANSSN = 0x0a06,
    LESSTHENFIFTEEN = 0x0a07,
};

enum class enter_game_msg
{
    NEW = 0x0f1c,
    NOENTER_FORCEDISCONN = 0x0f1d,
    CHANGINGSERVER = 0x0f1e,
    PLAYING = 0x0f20,
    REJECT = 0x0f21,
    CONFIRM = 0x0f22,
    FORCEDISCONN = 0x0f23,
};

enum class reject_type
{
    CHARABOVETRIALLEVEL = 0x01,
    MAXREGISTEREDIPREACHED = 0x02,
    GAMESERVERNOTONLINE = 0x03,
    DATADIFFERENCE = 0x04,
    GAMESERVEROFFLINE = 0x05,
    MAXSERVERUSERLIMITREACHED = 0x06,
    WORLDSERVERISFULL = 0x07,
    LOGINERROR = 0x08,
};

enum class mail_type
{
    guild,
    player,
    system
};

#include "netmessages.h"

//#define TOTALCHARACTERS		120
// todo - make sure this is what it should be
#define TOTALCHARACTERS		130
#define TOTALACTION			15

#define OBJECTSTOP			0
#define OBJECTMOVE			1
#define OBJECTRUN				2
#define OBJECTATTACK			3
#define OBJECTMAGIC			4
#define OBJECTGETITEM		5
#define OBJECTDAMAGE			6
#define OBJECTDAMAGEMOVE	7
#define OBJECTATTACKMOVE	8
#define OBJECTDYING			10			   
#define OBJECTNULLACTION	100
#define OBJECTDEAD			101


#define OBJECTMOVE_CONFIRM				1001
#define OBJECTMOVE_REJECT				1010
#define OBJECTMOTION_CONFIRM			1020
#define OBJECTMOTION_ATTACK_CONFIRM		1030
#define OBJECTMOTION_REJECT				1040

const char motion_to_actionid[MOTION_MAX] =
{
    OBJECTSTOP, OBJECTMOVE, OBJECTRUN, OBJECTATTACK, OBJECTMAGIC, OBJECTGETITEM, OBJECTDAMAGE,
    OBJECTDAMAGEMOVE, OBJECTATTACKMOVE, OBJECTDYING, OBJECTNULLACTION, OBJECTNULLACTION, OBJECTNULLACTION
};

#define GET_MOTION_ACTIONID(x)	( motion_to_actionid[x & ~MSGIDTYPE_MOTION] )


#define DEFAULTBAGSIZE	20
#define DEFAULTEXTRABAGSIZE	5
#define DEFAULTBANKSIZE 20
#define MAXITEMTYPES			2000 // 1500->2000 xRisenx
#define MAXNPCTYPES			200
#define MAXNPCTIERS			10
#define MAXPARTYMEMBERS		40
#define MAXBANKITEMS			120 // 120->300 xRisenx
#define MAXITEMS		75			// 50->75 xRisenx
#define MAXGUILDSMAN	128
#define MAXMAGICTYPE	100
#define MAXSKILLTYPE	25
#define MAXTELEPORTLIST	20

#define DEF_LIMITHUNTERLEVEL 100

#define SPECABLTYTIMESEC	1200

// todo - change to enum class for all enums
enum GuildUpgrades
{
    GU_WAREHOUSE,
    GU_SUMMONS,
    GU_CAPTAINCY,
    GU_RAIDMASTERS,
    GU_HUNTMASTERS,
    GU_MAX
};

struct GuildUpgrade
{
    std::string name;
    GuildUpgrades type;
    uint8_t maxLvl;
    uint32_t costGold[5];
    uint32_t costMaj[5];
    uint32_t costCont[5];
};

static inline const GuildUpgrade gldUps[int(GuildUpgrades::GU_MAX)] =
{
    {
        "Warehouse", GuildUpgrades::GU_WAREHOUSE, 4,
        {0, 200000, 500000, 1000000, 5000000},
        {0, 1000, 2000, 10000, 50000},
        {0, 1000, 3000, 8000, 20000},
    },
    {
        "Summons", GuildUpgrades::GU_SUMMONS, 4,
        {0, 200000, 500000, 1000000, 5000000},
        {0, 1000, 2000, 10000, 50000},
        {0, 1000, 3000, 8000, 20000},
    },
    {
        "Captaincy", GuildUpgrades::GU_CAPTAINCY, 4,
        {0, 100000, 400000, 1000000, 5000000},
        {0, 500, 1000, 8000, 20000},
        {0, 2000, 5000, 10000, 50000},
    },
    {
        "Raidmasters", GuildUpgrades::GU_RAIDMASTERS, 4,
        {0, 500000, 1000000, 3000000, 6000000},
        {0, 1500, 3000, 12000, 24000},
        {0, 4000, 8000, 24000, 48000},
    },
    {
        "Huntmasters", GuildUpgrades::GU_HUNTMASTERS, 4,
        {0, 500000, 1000000, 3000000, 6000000},
        {0, 1500, 3000, 12000, 24000},
        {0, 4000, 8000, 24000, 48000},
    },
};

static inline const uint32_t maxGWHItems[5] = { 0, 75, 75 * 2, 75 * 3, 75 * 4 };


enum GuildRank
{
    GUILDRANK_NONE = 0,
    GUILDRANK_MASTER,
    GUILDRANK_CAPTAIN,
    GUILDRANK_HUNTMASTER,
    GUILDRANK_RAIDMASTER,
    GUILDRANK_GUILDSMAN,
    GUILDRANK_RECRUIT,
    GUILDRANK_MAX
};

using UnitStatus = uint32_t;

//TODO: Make this customizable in game via UI?
struct GuildRankPermissions
{
    bool canDisband;
    bool canInvite;
    bool canBan;
    bool canSummon;
    bool canWithdrawWH;
    bool crusadeCommander;
    bool canPromote;
    bool canPing;
};

static inline const GuildRankPermissions gldRankPerm[GUILDRANK_MAX] = {
    {true, true, true, true, true, true, true, true},		// GUILDRANK_MASTER
    {false, true, true, false, true, true, true, true},	// GUILDRANK_CAPTAIN
    {false, false, false, false, true, false, false, true},	// GUILDRANK_HUNTMASTER
    {false, false, false, false, true, false, false, true},	// GUILDRANK_RAIDMASTER
    {false, false, false, false, true, false, false, false},	// GUILDRANK_GUILDSMAN
    {false, false, false, false, false, false, false, false}	// GUILDRANK_RECRUIT
};

#define GUILDSTARTRANK		GUILDRANK_RECRUIT

//-----------------------------------------------------------------------------------------------------------------------

enum ChatType
{
    CHAT_NORMAL,
    CHAT_GUILD,
    CHAT_SHOUT,
    CHAT_NATIONSHOUT,
    CHAT_PARTY,
    CHAT_GM = 10,
    CHAT_WHISPER = 20,
    CHAT_MAX
};

//-----------------------------------------------------------------------------------------------------------------------

enum StatusFlags
{
    STATUS_INVISIBILITY = 1 << 4,	// 0x00000010
    STATUS_BERSERK = 1 << 5,	// 0x00000020
    STATUS_FROZEN = 1 << 6,	// 0x00000040
    STATUS_POISON = 1 << 7,	// 0x00000080

    STATUS_ANGELSTR = 1 << 12,	// 0x00001000
    STATUS_ANGELDEX = 1 << 13,	// 0x00002000
    STATUS_ANGELINT = 1 << 14,	// 0x00004000
    STATUS_ANGELMAG = 1 << 15,	// 0x00008000

    STATUS_RELICHOLDER = 1 << 17,	// 0x00020000 - previously STATUS_HEROFLAG
    STATUS_AFK = 1 << 18,	// 0x00040000
    STATUS_GREENSLATE = 1 << 16,	// 0x00010000
    STATUS_REDSLATE = 1 << 22,	// 0x00400000
    STATUS_BLUESLATE = 1 << 23,	// 0x00800000
    STATUS_ILLUSIONMOVEMENT = 1 << 21,	// 0x00200000
    STATUS_ILLUSION = 1 << 24,	// 0x01000000
    STATUS_DEFENSESHIELD = 1 << 25,	// 0x02000000
    STATUS_PFM = 1 << 26,	// 0x04000000
    STATUS_PFA = 1 << 27,	// 0x08000000
    STATUS_PK = 1 << 20,	// 0x00100000
    STATUS_SIDE = 0xF0000000,
    STATUS_ALL = 0xFFFFFFFF,
    STATUS_ENEMYFLAGS = STATUS_ALL - (STATUS_PFA |
    STATUS_PFM |
        STATUS_DEFENSESHIELD |
        STATUS_ILLUSION |
        STATUS_POISON)
};

//-----------------------------------------------------------------------------------------------------------------------

enum GMFlags
{
    GMFLAG_INVINCIBLE = 1,
    GMFLAG_NOAGGRO = 1 << 1,
    GMFLAG_ETHEREAL = 1 << 2
};

enum Weather
{
    WEATHER_SUNNY,
    WEATHER_LIGHTRAIN,
    WEATHER_MEDIUMRAIN,
    WEATHER_HEAVYRAIN,
    WEATHER_LIGHTSNOW,
    WEATHER_MEDIUMSNOW,
    WEATHER_HEAVYSNOW
};

enum Element
{
    ELEMENT_NONE = 0,
    ELEMENT_EARTH = 1,
    ELEMENT_AIR = 2,
    ELEMENT_FIRE = 3,
    ELEMENT_WATER = 4,
    ELEMENT_MAX
};
//----------------------------------Events-------------------------------------------------------------------------------------

#define RELICVICTORYTIME		(10 * 1000)

enum EventType
{
    ET_NONE,
    ET_CAPTURE,
    ET_DESTROY_SHIELD,
    ET_CRUSADE,
    ET_APOCALYPSE,
    ET_MONSTEREVENT, // Monster Kill Event xRisenx
    ET_MAX
};
//static const char * eventName[] = { "", "Capture the Relic", "Destroy the Shield", "Crusade", "Apocalypse", "MonsterEvent"};
static inline const std::string eventName[] = { "", "Capture the Relic", "Destroy the Shield", "Crusade", "Apocalypse", "MonsterEvent" };

struct Casualties
{
    uint32_t deaths;
    uint32_t kills;
};

//----------------------------------------------------------------------------------------------------------------------------------------

enum HeroReq
{
    HR_CAPE,
    HR_HELM,
    HR_CAP,
    HR_PLATE,
    HR_ROBE,
    HR_HAUBERK,
    HR_LEGGINGS,
    HR_MAX
};

struct HeroItemCost
{
    uint32_t EK;
    uint32_t contribution;
};

const HeroItemCost heroPrice[HR_MAX] =
{
    {300,0},		// HR_CAPE
    {150,20},	// HR_HELM
    {100,20},	// HR_CAP
    {300,30},	// HR_PLATE
    {200,20},	// HR_ROBE
    {100,10},	// HR_HAUBERK
    {150,15}		// HR_LEGGINGS
};

static inline const uint32_t HeroItemID[HR_MAX][MAXSIDES - 1][2] =
{
    {	// HR_CAPE
        {400, 400},		// ARESDEN
        {401, 401},		// ELVINE
    },
    {	// HR_HELM
        {403, 404},		// ARESDEN
        {405, 406},		// ELVINE
    },
    {	// HR_CAP
        {407, 408},		// ARESDEN
        {409, 410},		// ELVINE
    },
    {	// HR_PLATE
        {411, 412},		// ARESDEN
        {413, 414},		// ELVINE
    },
    {	// HR_ROBE
        {415, 416},		// ARESDEN
        {417, 418},		// ELVINE
    },
    {	// HR_HAUBERK
        {419, 420},		// ARESDEN
        {421, 422},		// ELVINE
    },
    {	// HR_LEGGINGS
        {423, 424},		// ARESDEN
        {425, 426},		// ELVINE
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------

enum Directions
{
    CENTER,
    NORTH,
    NORTHEAST,
    EAST,
    SOUTHEAST,
    SOUTH,
    SOUTHWEST,
    WEST,
    NORTHWEST
};

//----------------------------------------------------------------------------------------------------------------------------------------

#define atoul(str)			strtoul(str, 0, 10)
#define atoull(str)			_strtoui64(str, 0, 10)
#define strcpyn(dst, src)	strncpy(dst, src, sizeof(dst))

//#define logb(n, b)	(log(n)/log(b))

struct Point
{
    int32_t x, y;

    bool operator == (Point p)
    {
        return (p.x == x && p.y == y) ? true : false;
    }

    bool operator != (Point p)
    {
        return !(p == *this);
    }

    Point operator ++ (int)
    {
        Point copy = *this;
        x++;
        y++;
        return copy;
    }

    Point operator -- (int)
    {
        Point copy = *this;
        x--;
        y--;
        return copy;
    }

    Point & operator += (const Point & p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    Point & operator -= (const Point & p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    Point operator + (const Point & p) const
    {
        Point copy = *this;
        return (copy += p);
    }

    Point operator - (const Point & p) const
    {
        Point copy = *this;
        return (copy -= p);
    }
};


#define OBJECTMOVE_CONFIRM				1001
#define OBJECTMOVE_REJECT				1010
#define OBJECTMOTION_CONFIRM			1020
#define OBJECTMOTION_ATTACK_CONFIRM		1030
#define OBJECTMOTION_REJECT				1040

#define HITRATIOFACTOR		80.0f
#define MAXSKILLPOINTS		3000

struct hbxrect
{
    hbxrect(int32_t a, int32_t b, int32_t c, int32_t d) { left = a; top = b; right = c; bottom = d; }
    hbxrect() { left = top = right = bottom = 0; };
    hbxrect(const hbxrect & r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

struct hbxpoint
{
    hbxpoint(int32_t a, int32_t b) { x = a; y = b; }
    hbxpoint() { x = y = 0; };
    int32_t  x;
    int32_t  y;
};


#define DYNAMICOBJECT_FIRE			1
#define DYNAMICOBJECT_FISH			2
#define DYNAMICOBJECT_FISHOBJECT	3
#define DYNAMICOBJECT_MINERAL1		4
#define DYNAMICOBJECT_MINERAL2		5
#define DYNAMICOBJECT_ARESDENFLAG	6
#define DYNAMICOBJECT_ELVINEFLAG	7
#define DYNAMICOBJECT_ICESTORM		8
#define DYNAMICOBJECT_SPIKE			9
#define DYNAMICOBJECT_PCLOUD_BEGIN	10
#define DYNAMICOBJECT_PCLOUD_LOOP	11
#define DYNAMICOBJECT_PCLOUD_END	12
#define DYNAMICOBJECT_FIRE2			13
#define DYNAMICOBJECT_FIRE3			14 //for FireBow




//#include <cmath>
//#include <math.h>

//TODO: need a new way to detect speed hacking and not "you can speed hack 10 steps then slow down"
#ifdef _DEBUG
#define SPEEDCHECKTURNS 1
#else
#define DOSPEEDCHECK
#define SPEEDCHECKTURNS 10
#endif

#define MAXCRUSADESTRUCTURES		300
#define MAXAGRICULTURE				200

#define PFMABSORBVAL					0.50

//TODO: time you can spend in enemy maps - needs some changing to the system of raiding
#define RAIDTIME				600

//TODO: should not be hard coded
#define GOLDDROPMULTIPLIER			2
#define EXPMULTIPLIER				355

//TODO: what are these and why do they exist hardcoded?
#define BISLEGOLDCOST				100
#define ARENAGOLDCOST				500


#define HITRATIOFACTOR		80.0f
#define MAXSKILLPOINTS		3000


#define SERVERSTATUS_STOPPED 1
#define SERVERSTATUS_STARTING 2
#define SERVERSTATUS_ONLINE 3
#define SERVERSTATUS_SHUTDOWN 4


#define ByteSwap(x) ByteSwap5((unsigned char *) &x, sizeof(x))


// todo - fix old shit str tokenizer
#if !defined(WIN32)
#define strtok_s strtok_r
#define _atoi64 atoll
#define sprintf_s snprintf
#define strcpy_s(a,b,c) strcpy(a,c)
#endif

#define DBL "%f"
#define DBL2 "f"
#define XI64 "%lld"

#define SLEEP(a) { struct timespec req={0}; req.tv_sec = 0; req.tv_nsec = 1000000 * a; nanosleep(&req,0); }

#define CLIENTSOCKETBLOCKLIMIT	15


enum PartyStatus
{
    PS_NOTINPARTY,
    PS_PROCESSING,
    PS_INPARTY
};

enum Sex
{
    NONE,
    MALE,
    FEMALE
};

#define FLAGRANGE_X		11
#define FLAGRANGE_Y		9

enum skillIndexes
{
    SKILL_MINING,			// 0
    SKILL_FISHING,			// 1
    SKILL_FARMING,			// 2
    SKILL_MAGICRES,			// 3
    SKILL_MAGIC,			// 4
    SKILL_HANDATTACK,		// 5
    SKILL_ARCHERY,			// 6
    SKILL_SHORTSWORD,		// 7
    SKILL_LONGSWORD,		// 8
    SKILL_FENCING,			// 9
    SKILL_AXE,				// 10
    SKILL_SHIELD,			// 11
    SKILL_ALCHEMY,			// 12
    SKILL_MANUFACTURING,	// 13
    SKILL_HAMMER,			// 14
    SKILL_15,			    // 15	
    SKILL_CRAFTING,			// 16	
    SKILL_17,
    SKILL_18,
    SKILL_PRETENDCORPSE,	// 19
    SKILL_20,
    SKILL_STAFF,			// 21
    SKILL_22,
    SKILL_POISONRES			// 23
};

#define DELAYEVENTTYPE_DAMAGEOBJECT				1
#define DELAYEVENTTYPE_MAGICRELEASE				2
#define DELAYEVENTTYPE_USEITEM_SKILL			3
#define DELAYEVENTTYPE_METEORSTRIKE				4
#define DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE		5
#define DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT	6
#define DELAYEVENTTYPE_ANCIENT_TABLET			7

#define DELAYEVENTTYPE_END_APOCALYPSE			8
#define DELAYEVENTTYPE_KILL_ABADDON				9

#define DEFAULTINVENTORYSIZE 20
#define DEFAULTBANKSIZE 20

enum items
{
    ITEM_INVALID = -2,
    ITEM_NONE = -1,

    ITEM_DAGGER = 1,
    ITEM_DAGGER_SC = 2,
    ITEM_DAGGER_SWDBREAKER = 3,
    ITEM_DAGGER_ADD1 = 4,
    ITEM_DAGGER_ADD2 = 5,
    ITEM_KIGHTDAGGER = 6,
    ITEM_DIRK = 7,

    ITEM_SHORTSWORD = 8,
    ITEM_SHORTSWORD_ADD1 = 9,
    ITEM_SHORTSWORD_SC = 11,

    ITEM_MAINGAUCHE = 12,
    ITEM_MAINGAUCHE_ADD1 = 13,
    ITEM_MAINGAUCHE_SC = 14,

    ITEM_GRADIUS = 15,
    ITEM_GRADIUS_ADD1 = 16,

    ITEM_LONGSWORD = 17,
    ITEM_LONGSWORD_ADD1 = 18,
    ITEM_LONGSWORD_ADD2 = 19,
    ITEM_EXCALIBER = 20,
    ITEM_LONGSWORD_SC = 22,

    ITEM_SABRE = 23,
    ITEM_SABRE_ADD1 = 24,

    ITEM_SCIMITAR = 25,
    ITEM_SCIMITAR_ADD1 = 26,
    ITEM_SCIMITAR_ADD2 = 27,

    ITEM_FALCHION = 28,
    ITEM_FALCHION_ADD1 = 29,
    ITEM_FALCHION_ADD2 = 30,

    ITEM_ESTERK = 31,
    ITEM_ESTERK_ADD1 = 32,
    ITEM_ESTERK_ADD2 = 33,

    ITEM_RAPIER = 34,
    ITEM_RAPIER_ADD1 = 35,
    ITEM_RAPIER_ADD2 = 36,

    ITEM_TEMPLERSWORD = 37,

    ITEM_BROADSWORD = 38,
    ITEM_BROADSWORD_ADD1 = 39,
    ITEM_BROADSWORD_ADD2 = 40,
    ITEM_BROADSWORD_SC = 41,

    ITEM_BASTADSWORD = 42,
    ITEM_BASTADSWORD_ADD1 = 43,
    ITEM_BASTADSWORD_ADD2 = 44,
    ITEM_BASTADSWORD_SC = 45,

    ITEM_CLAYMORE = 46,
    ITEM_CLAYMORE_ADD1 = 47,
    ITEM_CLAYMORE_ADD2 = 48,
    ITEM_CLAYMORE_SC = 49,

    ITEM_GREATSWORD = 50,
    ITEM_GREATSWORD_ADD1 = 51,
    ITEM_GREATSWORD_ADD2 = 52,
    ITEM_GREATSWORD_SC = 53,


    ITEM_FLAMEBERGE = 54,
    ITEM_FLAMEBERGE_ADD1 = 55,
    ITEM_FLAMEBERGE_ADD2 = 56,
    ITEM_FLAMEBERGE_SC = 57,

    ITEM_LIGHTAXE = 59,
    ITEM_LIGHTAXE_ADD1 = 60,
    ITEM_LIGHTAXE_ADD2 = 61,
    ITEM_TOMAHOC = 62,
    ITEM_TOMAHOC_ADD1 = 63,
    ITEM_TOMAHOC_ADD2 = 64,

    ITEM_SEXONAXE = 65,
    ITEM_SEXONAXE_ADD1 = 66,
    ITEM_SEXONAXE_ADD2 = 67,

    ITEM_DOUBLEAXE = 68,
    ITEM_DOUBLEAXE_ADD1 = 69,
    ITEM_DOUBLEAXE_ADD2 = 70,

    ITEM_WARAXE = 71,
    ITEM_WARAXE_ADD1 = 72,
    ITEM_WARAXE_ADD2 = 73,
    ITEM_4BLADEGOLDENAXE = 74,

    ITEM_SHORTBOW = 75,
    ITEM_LONGBOW = 76,
    ITEM_ARROW = 77,
    ITEM_POISONARROW = 78,

    ITEM_WOODSHIELD = 79,
    ITEM_LEATHERSHIELD = 80,
    ITEM_TARGESHIELD = 81,
    ITEM_SCOOTERMSHIELD = 82,
    ITEM_BLONDESHIELD = 83,
    ITEM_IRONSHIELD = 84,
    ITEM_LAGISHIELD = 85,
    ITEM_KNIGHTSHIELD = 86,
    ITEM_TOWERSHIELD = 87,

    ITEM_GUILDADMISSIONTICKET = 88,
    ITEM_GUILDSECESSIONTICKET = 89,
    ITEM_GOLD = 90,

    ITEM_REDPOTION = 91,
    ITEM_BIGREDPOTION = 92,
    ITEM_BLUEPOTION = 93,
    ITEM_BIGBLUEPOTION = 94,
    ITEM_GREENPOTION = 95,
    ITEM_BIGGREENPOTION = 96,

    ITEM_DILUTIONPOTION = 97,

    ITEM_BAGUETTE = 98,
    ITEM_MEAT = 99,
    ITEM_FISH = 100,

    ITEM_REDFISH = 101,
    ITEM_GREENFISH = 102,
    ITEM_YELLOWFISH = 103,

    ITEM_MAP = 104,
    ITEM_FISHINGROD = 105,

    ITEM_PRETENDCORPSEMANUAL = 106,
    ITEM_ARCHERYMANUAL = 107,
    ITEM_SHIELDMANUAL = 108,
    ITEM_LONGSWORDMANUAL = 109,
    ITEM_FENCINGMANUAL = 110,
    ITEM_FISHINGMANUAL = 111,
    ITEM_AXEMANUAL = 112,
    ITEM_MAGICRESISTMANUAL = 113,

    ITEM_RECALLSCROLL = 114,
    ITEM_INVISIBILITYSCROLL = 115,
    ITEM_DETECTINVISCROLL = 116,

    ITEM_BLEEDINGISLANDTICKET = 117,

    ITEM_SNAKEMEAT = 188,
    ITEM_SNAKESKIN = 189,
    ITEM_SNAKETEETH = 190,
    ITEM_SNAKETONGUE = 191,

    ITEM_ANTLEG = 192,
    ITEM_ANTFEELER = 193,

    ITEM_CYCLOPSEYE = 194,
    ITEM_CYCLOPSHANDEDGE = 195,
    ITEM_CYCLOPSHEART = 196,
    ITEM_CYCLOPSMEAT = 197,
    ITEM_CYCLOPSLEATHER = 198,

    ITEM_HELBOUNDHEART = 199,
    ITEM_HELBOUNDLEATHER = 200,
    ITEM_HELBOUNDTAIL = 201,
    ITEM_HELBOUNDTEETH = 202,
    ITEM_HELBOUNDCLAW = 203,
    ITEM_HELBOUNDTONGUE = 204,

    ITEM_LUMPOFCLAY = 205,

    ITEM_ORCMEAT = 206,
    ITEM_ORCLEATHER = 207,
    ITEM_ORCTEETH = 208,

    ITEM_OGREHAIR = 209,
    ITEM_OGREHEART = 210,
    ITEM_OGREMEAT = 211,
    ITEM_OGRELEATHER = 212,
    ITEM_OGRETEETH = 213,
    ITEM_OGRECLAW = 214,

    ITEM_SCORPIONPINCERS = 215,
    ITEM_SCORPIONMEAT = 216,
    ITEM_SCORPIONSTING = 217,
    ITEM_SCORPIONSKIN = 218,

    ITEM_SKELETONBONES = 219,

    ITEM_SLIMEJELLY = 220,
    ITEM_STONEGOLEMPIECE = 221,

    ITEM_TROLLHEART = 222,
    ITEM_TROLLMEAT = 223,
    ITEM_TROLLLEATHER = 224,
    ITEM_TROLLCLAW = 225,

    ITEM_ALCHEMYMANUAL = 226,
    ITEM_ALCHEMYBOWL = 227,

    ITEM_MININGMANUAL = 230,
    ITEM_PICKAXE = 231,
    ITEM_HOE = 232,

    ITEM_MANUFACTURINGMANUAL = 235,
    ITEM_MANUFACTURINGHAMMER = 236,
    ITEM_HAMMERATTACKMANUAL = 237,

    ITEM_ARESDENFLAG_MASTER = 245,
    ITEM_ELVINEFLAG_MASTER = 246,
    ITEM_ARESDENFLAG = 247,
    ITEM_ELVINEFLAG = 248,

    ITEM_WANDATTACKMANUAL = 250,

    ITEM_MAGICWAND_MS20 = 256,
    ITEM_MAGICWAND_MS10 = 257,
    ITEM_MAGICWAND_MS0 = 258,
    ITEM_MAGICWAND_MSHIELD = 259,

    ITEM_HAIRCOLORPOTION = 270,
    ITEM_HAIRSTYLEPOTION = 271,
    ITEM_SKINCOLORPOTION = 272,
    ITEM_INVISIBILITYPOTION = 273,
    ITEM_SEXCHANGEPOTION = 274,
    ITEM_OGREPOTION = 275,
    ITEM_UNDERWEARPOTION = 276,

    ITEM_FLAMEBERGE_ADD3_LLF = 290,
    ITEM_MAGICWAND_MS30_LLF = 291,
    ITEM_GOLDENAXE_LLF = 292,

    ITEM_MAGICNECKLACE_RM10 = 300,
    ITEM_MAGICNECKLACE_DM1 = 305,
    ITEM_MAGICNECKLACE_MS10 = 308,
    ITEM_MAGICNECKLACE_DF10 = 311,

    ITEM_GOLDNECKLACE = 315,
    ITEM_SILVERNECKLACE = 316,

    ITEM_GOLDRING = 331,
    ITEM_SILVERRING = 332,
    ITEM_PLATINUMRING = 333,

    ITEM_LUCKYGOLDRING = 334,
    ITEM_EMERALDRING = 335,
    ITEM_SAPPHIRERING = 336,
    ITEM_RUBYRING = 337,
    ITEM_MEMORIALRING = 338,

    ITEM_DIAMOND = 350,
    ITEM_RUBY = 351,
    ITEM_SAPPHIRE = 352,
    ITEM_EMERALD = 353,
    ITEM_GOLDNUGGET = 354,
    ITEM_COAL = 355,
    ITEM_SILVERNUGGET = 356,
    ITEM_IRONORE = 357,
    ITEM_CRYSTAL = 358,

    ITEM_DYE_INDIGO = 360,
    ITEM_DYE_CRIMSON_RED = 361,
    ITEM_DYE_BROWN = 362,
    ITEM_DYE_GOLD = 363,
    ITEM_DYE_GREEN = 364,
    ITEM_DYE_GRAY = 365,
    ITEM_DYE_AQUA = 366,
    ITEM_DYE_PINK = 367,
    ITEM_DYE_VIOLET = 368,
    ITEM_DYE_BLUE = 369,
    ITEM_DYE_TAN = 370,
    ITEM_DYE_KHAKI = 371,
    ITEM_DYE_YELLOW = 372,
    ITEM_DYE_RED = 373,
    ITEM_DYE_BLACK = 374,
    ITEM_DECOLORATIONPOTION = 375,
    ITEM_DYE_BRIGHT_GREEN = 376,

    ITEM_ICESTORMMANUAL = 380,
    ITEM_MASSFIRESTRIKEMANUAL = 381,
    ITEM_BLOODYSHOCKWMANUAL = 382,

    ITEM_HANDATTACKMANUAL = 385,
    ITEM_SHORTSWORDMANUAL = 386,

    ITEM_POWERGREENPOTION = 390,
    ITEM_SUPERGREENPOTION = 391,

    ITEM_ARESDENHEROCAPE = 400,
    ITEM_ELVINEHEROCAPE = 401,
    ITEM_CAPE = 402,
    ITEM_AHEROHELM_M = 403,
    ITEM_AHEROHELM_W = 404,
    ITEM_EHEROHELM_M = 405,
    ITEM_EHEROHELM_W = 406,
    ITEM_AHEROCAP_M = 407,
    ITEM_AHEROCAP_W = 408,
    ITEM_EHEROCAP_M = 409,
    ITEM_EHEROCAP_W = 410,
    ITEM_AHEROARMOR_M = 411,
    ITEM_AHEROARMOR_W = 412,
    ITEM_EHEROARMOR_M = 413,
    ITEM_EHEROARMOR_W = 414,
    ITEM_AHEROROBE_M = 415,
    ITEM_AHEROROBE_W = 416,
    ITEM_EHEROROBE_M = 417,
    ITEM_EHEROROBE_W = 418,
    ITEM_AHEROHAUBERK_M = 419,
    ITEM_AHEROHAUBERK_W = 420,
    ITEM_EHEROHAUBERK_M = 421,
    ITEM_EHEROHAUBERK_W = 422,
    ITEM_AHEROLEGGINGS_M = 423,
    ITEM_AHEROLEGGINGS_W = 424,
    ITEM_EHEROLEGGINGS_M = 425,
    ITEM_EHEROLEGGINGS_W = 426,
    ITEM_ARESDENHEROCAPE_ADD1 = 427,
    ITEM_ELVINEHEROCAPE_ADD1 = 428,
    ITEM_CAPE_ADD1 = 429,

    ITEM_SHOES = 450,
    ITEM_LONGBOOTS = 451,

    ITEM_SHIRT_M = 453,

    ITEM_HAUBERK_M = 454,
    ITEM_LEATHERARMOR_M = 455,
    ITEM_CHAINMAIL_M = 456,
    ITEM_SCALEMAIL_M = 457,
    ITEM_PLATEMAIL_M = 458,

    ITEM_TROUSERS_M = 459,
    ITEM_KNEETROUSERS_M = 460,

    ITEM_CHAINHOSE_M = 461,
    ITEM_PLATELEGGINGS_M = 462,

    ITEM_CHEMISE_W = 470,
    ITEM_SHIRT_W = 471,

    ITEM_HAUBERK_W = 472,

    ITEM_BODICE_W = 473,
    ITEM_LONGBODICE_W = 474,

    ITEM_LEATHERARMOR_W = 475,
    ITEM_CHAINMAIL_W = 476,
    ITEM_SCALEMAIL_W = 477,
    ITEM_PLATEMAIL_W = 478,

    ITEM_SKIRT_W = 479,
    ITEM_TROUSERS_W = 480,
    ITEM_KNEETROUSERS_W = 481,

    ITEM_CHAINHOSE_W = 482,
    ITEM_PLATELEGGINGS_W = 483,

    ITEM_TUNIC_M = 484,


    ITEM_BLOODSWORD = 490,
    ITEM_BLOODAXE = 491,
    ITEM_BLOODRAPIER = 492,

    ITEM_IRONINGOT = 500,
    ITEM_SUPERCOAL = 501,
    ITEM_ULTRACOAL = 502,
    ITEM_GOLDINGOT = 503,
    ITEM_SILVERINGOT = 504,
    ITEM_BLONDEINGOT = 505,
    ITEM_MITHRALINGOT = 506,

    ITEM_BLONDESTONE = 507,
    ITEM_MITHRAL = 508,


    ITEM_ARENATICKET = 511,
    ITEM_ARENATICKET_2 = 513,
    ITEM_ARENATICKET_3 = 515,
    ITEM_ARENATICKET_4 = 517,

    ITEM_BOUQUETTE = 520,
    ITEM_FLOWERBASKET = 521,
    ITEM_FLOWERPOT = 522,

    ITEM_ARENATICKET_5 = 530,
    ITEM_ARENATICKET_6 = 531,
    ITEM_ARENATICKET_7 = 532,
    ITEM_ARENATICKET_8 = 533,
    ITEM_ARENATICKET_9 = 534,

    ITEM_DEMONEYE = 540,
    ITEM_DEMONHEART = 541,
    ITEM_DEMONMEAT = 542,
    ITEM_DEMONLEATHER = 543,

    ITEM_UNICORNHEART = 544,
    ITEM_UNICORNHORN = 545,
    ITEM_UNICORNMEAT = 546,
    ITEM_UNICORNLEATHER = 547,

    ITEM_WEREWOLFHEART = 548,
    ITEM_WEREWOLFNAIL = 549,
    ITEM_WEREWOLFMEAT = 550,
    ITEM_WEREWOLFTAIL = 551,
    ITEM_WEREWOLFTEETH = 552,
    ITEM_WEREWOLFLEATHER = 553,
    ITEM_WEREWOLFCLAW = 554,

    ITEM_BATTLEAXE = 560,

    ITEM_REDCARP = 570,
    ITEM_GREENCARP = 571,
    ITEM_GOLDCARP = 572,

    ITEM_CRUCIANCARP = 573,
    ITEM_BLUESEABREAM = 574,
    ITEM_SALMON = 575,
    ITEM_REDSEABREAM = 576,
    ITEM_GRAYMULLET = 577,

    ITEM_BATTLEAXE_ADD1 = 580,
    ITEM_BATTLEAXE_ADD2 = 581,
    ITEM_SABRE_ADD2 = 582,

    ITEM_ROBE_M = 590,
    ITEM_ROBE_W = 591,

    ITEM_HELM_M = 600,
    ITEM_FULLHELM_M = 601,

    ITEM_HELM_W = 602,
    ITEM_FULLHELM_W = 603,

    ITEM_XELIMABLADE = 610,
    ITEM_XELIMAAXE = 611,
    ITEM_XELIMARAPIER = 612,
    ITEM_SWORDOFMEDUSA = 613,
    ITEM_SWORDOFICEELEMENTAL = 614,
    ITEM_GIANTSWORD = 615,
    ITEM_DEMONSLAYER = 616,
    ITEM_COMPOSITEBOW = 617,
    ITEM_DARKELFBOW = 618,

    ITEM_MERIENSHIELD = 620,
    ITEM_MERIENPLATEMAIL_M = 621,
    ITEM_MERIENPLATEMAIL_W = 622,
    ITEM_GM_SHIELD = 623,

    ITEM_RINGOFTHEXELIMA = 630,
    ITEM_RINGOFTHEABADDON = 631,
    ITEM_RINGOFOGREPOWER = 632,
    ITEM_RINGOFDEMONPOWER = 633,
    ITEM_RINGOFWIZARD = 634,
    ITEM_RINGOFMAGE = 635,
    ITEM_RINGOFGRANDMAGE = 636,

    ITEM_NECKLACEOFLIGHTPRO = 637,
    ITEM_NECKLACEOFFIREPRO = 638,

    ITEM_NECKLACEOFPOISONPRO = 639,
    ITEM_NECKLACEOFSUFFERENT = 640,
    ITEM_NECKLACEOFMEDUSA = 641,

    ITEM_NECKLACEOFICEPRO = 642,
    ITEM_NECKLACEOFICEELE = 643,

    ITEM_NECKLACEOFAIRELE = 644,
    ITEM_NECKLACEOFEFREET = 645,
    ITEM_NECKLACEOFBEHOLDER = 646,
    ITEM_NECKLACEOFSTONEGOL = 647,
    ITEM_NECKLACEOFLICHE = 648,

    ITEM_ZEM = 650,
    ITEM_GREENBALL = 651,
    ITEM_REDBALL = 652,
    ITEM_YELLOWBALL = 653,
    ITEM_BLUEBALL = 654,
    ITEM_PEARLBALL = 655,
    ITEM_STONEOFXELIMA = 656,
    ITEM_STONEOFMERIEN = 657,

    ITEM_ARESDENMINEPOTION = 658,
    ITEM_ELVINEMINEPOTION = 659,
    ITEM_UNFREEZEPOTION = 660,

    ITEM_KNIGHTRAPIER = 671,
    ITEM_KNIGHTGREATSWORD = 672,
    ITEM_KNIGHTFLAMEBERGE = 673,
    ITEM_KNIGHTWARAXE = 674,
    ITEM_KNIGHTPLATEMAIL_M = 675,
    ITEM_KNIGHTPLATEMAIL_W = 676,
    ITEM_KNIGHTPLATELEG_M = 677,
    ITEM_KNIGHTPLATELEG_W = 678,
    ITEM_KNIGHTFULLHELM_M = 679,
    ITEM_KNIGHTFULLHELM_W = 680,
    ITEM_WIZARDHAUBERK_M = 681,
    ITEM_WIZARDHAUBERK_W = 682,
    ITEM_WIZMAGICWAND_MS20 = 683,
    ITEM_WIZMAGICWAND_MS10 = 684,
    ITEM_WIZARDROBE_M = 685,
    ITEM_WIZARDROBE_W = 686,
    ITEM_KNIGHTHAUBERK_M = 687,
    ITEM_KNIGHTHAUBERK_W = 688,

    ITEM_DKFULLHELM_M = 700,
    ITEM_DKHAUBERK_M = 701,
    ITEM_DKPLATEMAIL_M = 702,
    ITEM_DKLEGGINGS_M = 703,

    ITEM_DKFULLHELM_W = 709,
    ITEM_DKHAUBERK_W = 710,
    ITEM_DKPLATEMAIL_W = 712,
    ITEM_DKLEGGINGS_W = 713,

    ITEM_DKRAPIER = 722,
    ITEM_DKGREATSWORD = 723,

    ITEM_DKFLAMEBERG = 721,
    ITEM_DKGIANTSWORD = 719,
    ITEM_BLACKKNIGHTTEMPLE = 720,

    ITEM_DMMAGICSTAFF = 724,
    ITEM_DMMAGICWAND = 725,
    ITEM_BLACKMAGETEMPLE = 726,

    ITEM_DMHAT_M = 704,
    ITEM_DMHAUBERK_M = 705,
    ITEM_DMROBE_M = 708,
    ITEM_DMCHAINMAIL_M = 706,
    ITEM_DMLEGGINGS_M = 707,

    ITEM_FANTASYHAT_M = 1001,
    ITEM_FANTASYHAT_W = 1002,
    ITEM_FANTASYHELM_M = 1003,
    ITEM_FANTASYHELM_W = 1004,

    ITEM_DMHAT_W = 714,
    ITEM_DMHAUBERK_W = 715,
    ITEM_DMROBE_W = 718,
    ITEM_DMCHAINMAIL_W = 716,
    ITEM_DMLEGGINGS_W = 717,

    ITEM_SONGPYON = 730,
    ITEM_GINSENG = 731,
    ITEM_BEEFRIBSET = 732,
    ITEM_WINE = 733,

    ITEM_RINGOFARCHMAGE = 734,
    ITEM_RINGOFDRAGONPOWER = 735,

    ITEM_SANGAHGIANTSWORD = 736,

    ITEM_5000GOLDPOCKET = 740,
    ITEM_10000GOLDPOCKET = 741,
    ITEM_50000GOLDPOCKET = 742,
    ITEM_100000GOLDPOCKET = 743,
    ITEM_1000000GOLDPOCKET = 744,


    ITEM_HORNED_HELM_M = 750,
    ITEM_WINGS_HELM_M = 751,
    ITEM_WIZARD_CAP_M = 752,
    ITEM_WIZARD_HAT_M = 753,

    ITEM_HORNED_HELM_W = 754,
    ITEM_WINGS_HELM_W = 755,
    ITEM_WIZARD_CAP_W = 756,
    ITEM_WIZARD_HAT_W = 757,

    ITEM_HAMMER = 760,
    ITEM_BHAMMER = 761,
    ITEM_GBHAMMER = 762,

    ITEM_THIRDMEMORIALRING = 765,
    ITEM_SANTACOSTUME_M = 770,
    ITEM_SANTACOSTUME_W = 771,


    ITEM_REDCANDY = 780,
    ITEM_BLUECANDY = 781,
    ITEM_GREENCANDY = 782,

    ITEM_FARMINGMANUAL = 800,
    ITEM_SEEDBAG_WATERMELON = 801,
    ITEM_SEEDBAG_PUMPKIN = 802,
    ITEM_SEEDBAG_GARLIC = 803,
    ITEM_SEEDBAG_BARLEY = 804,
    ITEM_SEEDBAG_CARROT = 805,
    ITEM_SEEDBAG_RADISH = 806,
    ITEM_SEEDBAG_CORN = 807,
    ITEM_SEEDBAG_CBELLFLOWER = 808,
    ITEM_SEEDBAG_MELONE = 809,
    ITEM_SEEDBAG_TOMMATO = 810,
    ITEM_SEEDBAG_GRAPES = 811,
    ITEM_SEEDBAG_BLUEGRAPES = 812,
    ITEM_SEEDBAG_MUSHROOM = 813,
    ITEM_SEEDBAG_GINSENG = 814,

    ITEM_WATERMELON = 820,
    ITEM_PUMPKIN = 821,
    ITEM_GARLIC = 822,
    ITEM_BARLEY = 823,
    ITEM_CARROT = 824,
    ITEM_RADISH = 825,
    ITEM_CORN = 826,
    ITEM_CHINESEBELLFLOWER = 827,
    ITEM_MELONE = 828,
    ITEM_TOMMATO = 829,
    ITEM_GRAPES = 830,
    ITEM_BLUEGRAPES = 831,
    ITEM_MUSHROOM = 832,

    ITEM_SUPERREDPOTION = 840,
    ITEM_SUPERBLUEPOTION = 841,

    ITEM_BARBARIANHAMMER = 843,
    ITEM_BLACKSHADOWSWORD = 844,
    ITEM_STORMBRINGER = 845,
    ITEM_THE_DEVASTATOR = 846,
    ITEM_DARKEXECUTOR = 847,
    ITEM_LIGHTINGBLADE = 848,
    ITEM_KLONESSBLADE = 849,
    ITEM_KLONESSAXE = 850,
    ITEM_KLONESSESTERK = 851,

    ITEM_CANCELMANUAL = 852,
    ITEM_ESWMANUAL = 853,

    ITEM_PORTAL_TOKEN = 856,

    ITEM_IMCMANUAL = 857,

    ITEM_NECKLACEOFMERIEN = 858,
    ITEM_NECKLACEOFKLONESS = 859,
    ITEM_NECKLACEOFXELIMA = 860,

    ITEM_BERSERKWAND_MS20 = 861,
    ITEM_BERSERKWAND_MS10 = 862,
    ITEM_KLONESSWAND_MS20 = 863,
    ITEM_KLONESSWAND_MS10 = 864,
    ITEM_RESURWAND_MS20 = 865,
    ITEM_RESURWAND_MS10 = 866,

    ITEM_ANCIENTTABLET = 867,
    ITEM_ANCIENTTABLET_LU = 868,
    ITEM_ANCIENTTABLET_LD = 869,
    ITEM_ANCIENTTABLET_RU = 870,
    ITEM_ANCIENTTABLET_RD = 871,

    ITEM_FIRE_BOW = 873,
    ITEM_DIRECTION_BOW = 874,

    ITEM_SUMMONSCROLL_SOR = 875,
    ITEM_SUMMONSCROLL_ATK = 876,
    ITEM_SUMMONSCROLL_ELF = 877,
    ITEM_SUMMONSCROLL_DSK = 878,
    ITEM_SUMMONSCROLL_HBT = 879,
    ITEM_SUMMONSCROLL_BAR = 880,

    ITEM_MAGINDIAMOND = 881,
    ITEM_MAGINRUBY = 882,
    ITEM_MAGICEMERALD = 883,
    ITEM_MAGICSAPPHIRE = 884,

    ITEM_LUCKYPRIZETICKET = 885,

    ITEM_MAGICNECKLACE_DF15 = 886,
    ITEM_MAGICNECKLACE_DF20 = 887,
    ITEM_MAGICNECKLACE_DF25 = 888,
    ITEM_MAGICNECKLACE_DF30 = 889,

    ITEM_MAGICNECKLACE_DM2 = 890,
    ITEM_MAGICNECKLACE_DM3 = 891,
    ITEM_MAGICNECKLACE_DM4 = 892,
    ITEM_MAGICNECKLACE_DM5 = 893,

    ITEM_MAGICNECKLACE_MS12 = 894,
    ITEM_MAGICNECKLACE_MS14 = 895,
    ITEM_MAGICNECKLACE_MS16 = 896,
    ITEM_MAGICNECKLACE_MS18 = 897,

    ITEM_MAGICNECKLACE_RM15 = 898,
    ITEM_MAGICNECKLACE_RM20 = 899,
    ITEM_MAGICNECKLACE_RM25 = 900,
    ITEM_MAGICNECKLACE_RM30 = 901,

    ITEM_DIAMONDWARE = 902,
    ITEM_RUBYWARE = 903,
    ITEM_SAPPHIREWARE = 904,
    ITEM_EMERALDWARE = 905,
    ITEM_CRYSTALWARE = 906,

    ITEM_CRAFTINGVESSEL = 907,

    ITEM_ANGELICPENDANT_STR = 908,
    ITEM_ANGELICPENDANT_DEX = 909,
    ITEM_ANGELICPENDANT_INT = 910,
    ITEM_ANGELICPENDANT_MAG = 911,

    ITEM_BRONZE_SCAV_TOKEN = 917,
    ITEM_SILVER_SCAV_TOKEN = 918,
    ITEM_GOLD_SCAV_TOKEN = 919,
    ITEM_PLATINUM_SCAV_TOKEN = 920,

    ITEM_GMBOW = 921,
    ITEM_GMWAND = 922,

    ITEM_GM_HP_RING = 926,

    ITEM_MIGHTY_BLUE_CANDY = 928,

    ITEM_MIGHTY_RED_CANDY = 937,

    ITEM_TAMINGMANUAL = 938,

    ITEM_ARMORDYE_INDIGO = 950,
    ITEM_ARMORDYE_CRIMSONRED = 951,
    ITEM_ARMORDYE_GOLD = 952,
    ITEM_ARMORDYE_AQUA = 953,
    ITEM_ARMORDYE_PINK = 954,
    ITEM_ARMORDYE_VIOLET = 955,
    ITEM_ARMORDYE_BLUE = 956,
    ITEM_ARMORDYE_KHAKI = 957,
    ITEM_ARMORDYE_YELLOW = 958,
    ITEM_ARMORDYE_RED = 959,

    ITEM_RELIC = 1005,
    ITEM_CK_AGT = 1006,
    ITEM_CK_CGT = 1007,
    ITEM_CK_DT = 1008,

    ITEM_REJUGEM7 = 1009,
    ITEM_REJUGEM14 = 1010,
    ITEM_REJUGEM21 = 1011,
    ITEM_BLOODGEM7 = 1012,
    ITEM_BLOODGEM14 = 1013,
    ITEM_BLOODGEM21 = 1014,
    ITEM_MINDGEM7 = 1015,
    ITEM_MINDGEM14 = 1016,
    ITEM_MINDGEM21 = 1017,
    ITEM_ARMORGEM7 = 1018,
    ITEM_ARMORGEM14 = 1019,
    ITEM_ARMORGEM21 = 1020,
    ITEM_ENCHANTEDGEM2 = 1021,
    ITEM_ENCHANTEDGEM4 = 1022,
    ITEM_ENCHANTEDGEM6 = 1023,
    ITEM_TACTICALGEM3 = 1024,
    ITEM_TACTICALGEM5 = 1025,
    ITEM_TACTICALGEM7 = 1026,
    ITEM_VORTEXGEM = 1027,

    ITEM_FANTASYTOKEN = 1100,
    ITEM_SCROLLOFRENOWN = 1101,
    ITEM_FANTASYDONATETOKEN = 1102,
    ITEM_CRITPOT = 1110,
    ITEM_BEGHAUBERK_M = 1150,
    ITEM_BEGHAUBERK_W = 1151,
    ITEM_HOSE_M = 1152,
    ITEM_HOSE_W = 1153,
    ITEM_BEGPLATELEG_M = 1154,
    ITEM_BEGPLATELEG_W = 1155,
    ITEM_BEGPLATEMAIL_M = 1156,
    ITEM_BEGPLATEMAIL_W = 1157,
    ITEM_BEGPHELM_M = 1158,
    ITEM_BEGPHELM_W = 1159,
    ITEM_BEGFULLHELM_M = 1160,
    ITEM_BEGFULLHELM_W = 1161,
    ITEM_BEGWINGHELM_M = 1162,
    ITEM_BEGWINGHELM_W = 1163,
    ITEM_BEGHORNS_M = 1164,
    ITEM_BEGHORNS_W = 1165,
    ITEM_BEGWIZARDHAUBERK_M = 1166,
    ITEM_BEGWIZARDHAUBERK_W = 1167,
    ITEM_BEGWIZARDROBE_M = 1168,
    ITEM_BEGWIZARDROBE_W = 1169,
    ITEM_BEGHAT_M = 1170,
    ITEM_BEGHAT_W = 1171,
    ITEM_BEGCHAIN_M = 1172,
    ITEM_BEGCHAIN_W = 1173,
    ITEM_DAMEHAUBERK_M = 1174,
    ITEM_DAMEHAUBERK_W = 1175,
    ITEM_DAMEHOSE_M = 1176,
    ITEM_DAMEHOSE_W = 1177,
    ITEM_DAMEPLATELEG_M = 1178,
    ITEM_DAMEPLATELEG_W = 1179,
    ITEM_DAMEPLATEMAIL_M = 1180,
    ITEM_DAMEPLATEMAIL_W = 1181,
    ITEM_DAMEPHELM_M = 1182,
    ITEM_DAMEPHELM_W = 1183,
    ITEM_DAMEFULLHELM_M = 1184,
    ITEM_DAMEFULLHELM_W = 1185,
    ITEM_DAMEWINGHELM_M = 1186,
    ITEM_DAMEWINGHELM_W = 1187,
    ITEM_DAMEHORNS_M = 1188,
    ITEM_DAMEHORNS_W = 1189,
    ITEM_DAMEWIZARDHAUBERK_M = 1190,
    ITEM_DAMEWIZARDHAUBERK_W = 1191,
    ITEM_DAMEWIZARDROBE_M = 1192,
    ITEM_DAMEWIZARDROBE_W = 1193,
    ITEM_DAMEHAT_M = 1194,
    ITEM_DAMEHAT_W = 1195,
    ITEM_DAMECHAIN_M = 1196,
    ITEM_DAMECHAIN_W = 1197,
    ITEM_MASTERHAUBERK_M = 1198,
    ITEM_MASTERHAUBERK_W = 1199,
    ITEM_MASTERHOSE_M = 1200,
    ITEM_MASTERHOSE_W = 1201,
    ITEM_MASTERPLATELEG_M = 1202,
    ITEM_MASTERPLATELEG_W = 1203,
    ITEM_MASTERPLATEMAIL_M = 1204,
    ITEM_MASTERPLATEMAIL_W = 1205,
    ITEM_MASTERPHELM_M = 1206,
    ITEM_MASTERPHELM_W = 1207,
    ITEM_MASTERFULLHELM_M = 1208,
    ITEM_MASTERFULLHELM_W = 1209,
    ITEM_MASTERWINGHELM_M = 1210,
    ITEM_MASTERWINGHELM_W = 1211,
    ITEM_MASTERHORNS_M = 1212,
    ITEM_MASTERHORNS_W = 1213,
    ITEM_MASTERWIZARDHAUBERK_M = 1214,
    ITEM_MASTERWIZARDHAUBERK_W = 1215,
    ITEM_MASTERWIZARDROBE_M = 1216,
    ITEM_MASTERWIZARDROBE_W = 1217,
    ITEM_MASTERHAT_M = 1218,
    ITEM_MASTERHAT_W = 1219,
    ITEM_MASTERCHAIN_M = 1220,
    ITEM_MASTERCHAIN_W = 1221,
    ITEM_EMPHAUBERK_M = 1222,
    ITEM_EMPHAUBERK_W = 1223,
    ITEM_EMPHOSE_M = 1224,
    ITEM_EMPHOSE_W = 1225,
    ITEM_EMPPLATELEG_M = 1226,
    ITEM_EMPPLATELEG_W = 1227,
    ITEM_EMPPLATEMAIL_M = 1228,
    ITEM_EMPPLATEMAIL_W = 1229,
    ITEM_EMPPHELM_M = 1230,
    ITEM_EMPPHELM_W = 1231,
    ITEM_EMPFULLHELM_M = 1232,
    ITEM_EMPFULLHELM_W = 1233,
    ITEM_EMPWINGHELM_M = 1234,
    ITEM_EMPWINGHELM_W = 1235,
    ITEM_EMPHORNS_M = 1236,
    ITEM_EMPHORNS_W = 1237,
    ITEM_EMPWIZARDHAUBERK_M = 1238,
    ITEM_EMPWIZARDHAUBERK_W = 1239,
    ITEM_EMPWIZARDROBE_M = 1240,
    ITEM_EMPWIZARDROBE_W = 1241,
    ITEM_EMPHAT_M = 1242,
    ITEM_EMPHAT_W = 1243,
    ITEM_EMPCHAIN_M = 1244,
    ITEM_EMPCHAIN_W = 1245,
    ITEM_ANCHAUBERK_M = 1246,
    ITEM_ANCHAUBERK_W = 1247,
    ITEM_ANCHOSE_M = 1248,
    ITEM_ANCHOSE_W = 1249,
    ITEM_ANCPLATELEG_M = 1250,
    ITEM_ANCPLATELEG_W = 1251,
    ITEM_ANCPLATEMAIL_M = 1252,
    ITEM_ANCPLATEMAIL_W = 1253,
    ITEM_ANCPHELM_M = 1254,
    ITEM_ANCPHELM_W = 1255,
    ITEM_ANCFULLHELM_M = 1256,
    ITEM_ANCFULLHELM_W = 1257,
    ITEM_ANCWINGHELM_M = 1258,
    ITEM_ANCWINGHELM_W = 1259,
    ITEM_ANCHORNS_M = 1260,
    ITEM_ANCHORNS_W = 1261,
    ITEM_ANCWIZARDHAUBERK_M = 1262,
    ITEM_ANCWIZARDHAUBERK_W = 1263,
    ITEM_ANCWIZARDROBE_M = 1264,
    ITEM_ANCWIZARDROBE_W = 1265,
    ITEM_ANCHAT_M = 1266,
    ITEM_ANCHAT_W = 1267,
    ITEM_ANCCHAIN_M = 1268,
    ITEM_ANCCHAIN_W = 1269,
    ITEM_BEGSWORD = 1300,
    ITEM_BEGLONGSWORD = 1301,
    ITEM_BEGRAIPER = 1202,
    ITEM_BEGAXE = 1203,
    ITEM_BEGWAND = 1304,
    ITEM_BEGSHORTBOW = 1305,
    ITEM_DAMESWORD = 1306,
    ITEM_DAMEESTERK = 1307,
    ITEM_DAMEAXE = 1308,
    ITEM_DAMEHAMMER = 1309,
    ITEM_DAMEWAND = 1310,
    ITEM_DAMESHORTBOW = 1311,
    ITEM_DAMELONGBOW = 1312,
    ITEM_MASTERSWORD = 1313,
    ITEM_MASTERESTERK = 1314,
    ITEM_MASTERAXE = 1315,
    ITEM_MASTERRAIPER = 1316,
    ITEM_MASTERHAMMER = 1317,
    ITEM_MASTERWAND = 1318,
    ITEM_MASTERSHORTBOW = 1319,
    ITEM_MASTERLONGBOW = 1320,
    ITEM_MASTERCOMPBOW = 1321,
    ITEM_LEGENDRAIPER = 1322,
    ITEM_LEGENDESTERK = 1323,
    ITEM_LEGENDAXE = 1324,
    ITEM_LEGENDSWORD = 1325,
    ITEM_LEGENDHAMMER = 1326,
    ITEM_LEGENDWAND = 1327,
    ITEM_LEGENDSHORTBOW = 1328,
    ITEM_LEGENDLONGBOW = 1329,
    ITEM_LEGENDCOMPBOW = 1330,
    ITEM_RESETSCROLLSTR = 1346,
    ITEM_RESETSCROLLDEX = 1347,
    ITEM_RESETSCROLLINT = 1348,
    ITEM_RESETSCROLLVIT = 1349,
    ITEM_RESETSCROLLMAG = 1350,
    ITEM_RESETSCROLLCHR = 1351,
    ITEM_RESETSCROLLALL = 1352,
    ITEM_SCANMANUAL = 1353,
    ITEM_ISMANUAL = 1354,
    ITEM_BSWMANUAL = 1355,
    ITEM_MHMANUAL = 1356,
    ITEM_MSMANUAL = 1357,
    ITEM_COTGMANUAL = 1358,
    ITEM_SOTGMANUAL = 1359,
    ITEM_MBMANUAL = 1360,
    ITEM_FSWMANUAL = 1361,
    ITEM_LCMANUAL = 1362,
    ITEM_MSRUNE = 1363,
    ITEM_DMRUNE = 1364,
    ITEM_DFRUNE = 1365,
    ITEM_RMRUNE = 1366,
    ITEM_DIARUNE = 1367,
    ITEM_RUBYRUNE = 1368,
    ITEM_EMERUNE = 1369,
    ITEM_SAPPRUNE = 1370,
    ITEM_MASTERDFRUNE = 1371,
    ITEM_MASTERRMRUNE = 1372,
    ITEM_MASTERDMRUNE = 1373,
    ITEM_MASTERMSRUNE = 1374,
    ITEM_MAGICDRAGON_AMP = 1500,
    ITEM_MAGICDRAGON_DEF = 1501,
    ITEM_AANCHEROCAPE = 1523,
    ITEM_AANCHEROHAUBERK_M = 1524,
    ITEM_AANCHEROHAUBERK_W = 1525,
    ITEM_AANCHEROLEGGINGS_M = 1526,
    ITEM_AANCHEROLEGGINGS_W = 1527,
    ITEM_AANCHEROARMOR_M = 1528,
    ITEM_AANCHEROARMOR_W = 1529,
    ITEM_AANCHEROROBE_M = 1530,
    ITEM_AANCHEROROBE_W = 1531,
    ITEM_AANCHEROHELM_M = 1532,
    ITEM_AANCHEROHELM_W = 1533,
    ITEM_AANCHEROCAP_M = 1534,
    ITEM_AANCHEROCAP_W = 1535,

    ITEM_EANCHEROCAPE = 1536,
    ITEM_EANCHEROHAUBERK_M = 1537,
    ITEM_EANCHEROHAUBERK_W = 1538,
    ITEM_EANCHEROLEGGINGS_M = 1539,
    ITEM_EANCHEROLEGGINGS_W = 1540,
    ITEM_EANCHEROARMOR_M = 1541,
    ITEM_EANCHEROARMOR_W = 1542,
    ITEM_EANCHEROROBE_M = 1543,
    ITEM_EANCHEROROBE_W = 1544,
    ITEM_EANCHEROHELM_M = 1545,
    ITEM_EANCHEROHELM_W = 1546,
    ITEM_EANCHEROCAP_M = 1547,
    ITEM_EANCHEROCAP_W = 1548,

    ITEM_ALEGHEROCAPE = 1549,
    ITEM_ALEGHEROHAUBERK_M = 1550,
    ITEM_ALEGHEROHAUBERK_W = 1551,
    ITEM_ALEGHEROLEGGINGS_M = 1552,
    ITEM_ALEGHEROLEGGINGS_W = 1553,
    ITEM_ALEGHEROARMOR_M = 1554,
    ITEM_ALEGHEROARMOR_W = 1555,
    ITEM_ALEGHEROROBE_M = 1556,
    ITEM_ALEGHEROROBE_W = 1557,
    ITEM_ALEGHEROHELM_M = 1558,
    ITEM_ALEGHEROHELM_W = 1559,
    ITEM_ALEGHEROCAP_M = 1560,
    ITEM_ALEGHEROCAP_W = 1561,

    ITEM_ELEGHEROCAPE = 1562,
    ITEM_ELEGHEROHAUBERK_M = 1563,
    ITEM_ELEGHEROHAUBERK_W = 1564,
    ITEM_ELEGHEROLEGGINGS_M = 1565,
    ITEM_ELEGHEROLEGGINGS_W = 1566,
    ITEM_ELEGHEROARMOR_M = 1567,
    ITEM_ELEGHEROARMOR_W = 1568,
    ITEM_ELEGHEROROBE_M = 1569,
    ITEM_ELEGHEROROBE_W = 1570,
    ITEM_ELEGHEROHELM_M = 1571,
    ITEM_ELEGHEROHELM_W = 1572,
    ITEM_ELEGHEROCAP_M = 1573,
    ITEM_ELEGHEROCAP_W = 1574,
    ITEM_STONEOFHERO = 1575,
    ITEM_DKLONESSBLADE = 1688,
    ITEM_DKLONESSAXE = 1689,
    ITEM_DKLONESSESTERK = 1690,
    ITEM_NECKLACEOFDKLONESS = 1691,
    ITEM_DKLONESSWAND_MS10 = 1692,
    ITEM_DKLONESSWAND_MS20 = 1693
};

#define DEF_MAXITEMEQUIPPOS		15
#define DEF_EQUIPPOS_NONE		0
#define DEF_EQUIPPOS_HEAD		1
#define DEF_EQUIPPOS_BODY		2
#define DEF_EQUIPPOS_ARMS		3
#define DEF_EQUIPPOS_PANTS		4
#define DEF_EQUIPPOS_LEGGINGS	5
#define DEF_EQUIPPOS_NECK		6
#define DEF_EQUIPPOS_LHAND		7
#define DEF_EQUIPPOS_RHAND		8
#define DEF_EQUIPPOS_TWOHAND	9
#define DEF_EQUIPPOS_RFINGER	10
#define DEF_EQUIPPOS_LFINGER	11
#define DEF_EQUIPPOS_BACK		12
#define DEF_EQUIPPOS_FULLBODY	13

#define DEF_ITEMTYPE_NOTUSED	-1
#define DEF_ITEMTYPE_NONE		 0
#define DEF_ITEMTYPE_EQUIP		 1
#define DEF_ITEMTYPE_APPLY		 2
#define DEF_ITEMTYPE_USE_DEPLETE 3       
#define DEF_ITEMTYPE_INSTALL	 4
#define DEF_ITEMTYPE_CONSUME	 5
#define DEF_ITEMTYPE_ARROW		 6
#define DEF_ITEMTYPE_EAT		 7
#define DEF_ITEMTYPE_USE_SKILL   8
#define DEF_ITEMTYPE_USE_PERM    9
#define DEF_ITEMTYPE_USE_SKILL_ENABLEDIALOGBOX	10
#define DEF_ITEMTYPE_USE_DEPLETE_DEST			11
#define DEF_ITEMTYPE_MATERIAL					12


#define DEF_ITEMEFFECTTYPE_NONE				0
#define DEF_ITEMEFFECTTYPE_ATTACK			1
#define DEF_ITEMEFFECTTYPE_DEFENSE			2
#define DEF_ITEMEFFECTTYPE_ATTACK_ARROW		3
#define DEF_ITEMEFFECTTYPE_HP   		    4
#define DEF_ITEMEFFECTTYPE_MP   		    5
#define DEF_ITEMEFFECTTYPE_SP   		    6
#define DEF_ITEMEFFECTTYPE_HPSTOCK 		    7
#define DEF_ITEMEFFECTTYPE_GET			    8
#define DEF_ITEMEFFECTTYPE_STUDYSKILL		9
#define DEF_ITEMEFFECTTYPE_SHOWLOCATION		10
#define DEF_ITEMEFFECTTYPE_MAGIC			11
#define DEF_ITEMEFFECTTYPE_CHANGEATTR		12
#define DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE	13
#define DEF_ITEMEFFECTTYPE_ADDEFFECT	    14
#define DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE	15 
#define DEF_ITEMEFFECTTYPE_OCCUPYFLAG		16
#define DEF_ITEMEFFECTTYPE_DYE				17
#define DEF_ITEMEFFECTTYPE_STUDYMAGIC		18
#define DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN	19
#define DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE	20
#define DEF_ITEMEFFECTTYPE_MATERIAL_ATTR	21
#define DEF_ITEMEFFECTTYPE_FIRMSTAMINAR		22
#define DEF_ITEMEFFECTTYPE_LOTTERY			23

#define DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY		24
#define DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY	25
#define DEF_ITEMEFFECTTYPE_ALTERITEMDROP		26

#define DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT		27
#define DEF_ITEMEFFECTTYPE_WARM					28
#define DEF_ITEMEFFECTTYPE_DEFENSE_ANTIMINE		29
#define DEF_ITEMEFFECTTYPE_ITEMFARM				30

#define DEF_ITET_UNIQUE_OWNER				1
#define DEF_ITET_ID							2
#define DEF_ITET_DATE						3


#define ARGB(a,r,g,b)          ((uint32_t)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint32_t)(uint8_t)(b))<<16)|(((uint32_t)(uint8_t)(a))<<24)))

enum ItemStat
{
    ITEMSTAT_NONE,
    ITEMSTAT_CRITICAL,
    ITEMSTAT_POISONING,
    ITEMSTAT_RIGHTEOUS,
    ITEMSTAT_UNUSED,		// UNUSED
    ITEMSTAT_AGILE,
    ITEMSTAT_LIGHT,
    ITEMSTAT_SHARP,
    ITEMSTAT_STRONG,
    ITEMSTAT_ANCIENT,
    ITEMSTAT_CASTPROB,
    ITEMSTAT_MANACONV,
    ITEMSTAT_CRITICAL2,
    ITEMSTAT_FLAWLESSSHARP, // Flawless Sharp
    ITEMSTAT_FLAWLESSANCIENT // Flawless Ancient
};

enum ItemStat2
{
    ITEMSTAT2_NONE,
    ITEMSTAT2_PSNRES,
    ITEMSTAT2_HITPROB,
    ITEMSTAT2_DEF,
    ITEMSTAT2_HPREC,
    ITEMSTAT2_SPREC,
    ITEMSTAT2_MPREC,
    ITEMSTAT2_MR,
    ITEMSTAT2_PA,
    ITEMSTAT2_MA,
    ITEMSTAT2_CAD,
    ITEMSTAT2_EXP,
    ITEMSTAT2_GOLD
};

struct ItemStats2
{
    std::string desc;
    uint32_t mult;
};

static inline const ItemStats2 itemStats2[] = {
    { "", 0 },
    { "PSN", 7 },
    { "HIT", 7 },
    { "DF", 7 },
    { "HP", 7},
    { "SP", 7 },
    { "MP", 7 },
    { "MR", 7 },
    { "PA", 3 },
    { "MA", 3 },
    { "CAD", 1 },
    { "XP", 10 },
    { "GOLD", 10 },
    { "", 0 }
};

#define MAXITEMEQUIPPOS		20
#define EQUIPPOS_NONE		0
#define EQUIPPOS_HEAD		1
#define EQUIPPOS_BODY		2
#define EQUIPPOS_ARMS		3
#define EQUIPPOS_PANTS		4
#define EQUIPPOS_BOOTS		5
#define EQUIPPOS_NECK		6
#define EQUIPPOS_LHAND		7
#define EQUIPPOS_RHAND		8
#define EQUIPPOS_TWOHAND	9
#define EQUIPPOS_RFINGER	10
#define EQUIPPOS_LFINGER	11
#define EQUIPPOS_BACK		12
#define EQUIPPOS_FULLBODY	13     

enum ItemTypes
{
    ITEMTYPE_NOTUSED = -1,
    ITEMTYPE_NONE,				// 0
    ITEMTYPE_EQUIP,			// 1
    ITEMTYPE_APPLY,			// 2
    ITEMTYPE_USE_DEPLETE,	// 3
    ITEMTYPE_INSTALL,			// 4
    ITEMTYPE_CONSUME,			// 5
    ITEMTYPE_ARROW,			// 6
    ITEMTYPE_EAT,				// 7
    ITEMTYPE_USE_SKILL,		// 8
    ITEMTYPE_USE_PERM,		// 9
    ITEMTYPE_USE_SKILL_ENABLEDIALOGBOX,	// 10
    ITEMTYPE_USE_DEPLETE_DEST,				// 11
    ITEMTYPE_MATERIAL,		// 12
    ITEMTYPE_RELIC				// 13
};


#define ITEMEFFECTTYPE_NONE				0
#define ITEMEFFECTTYPE_ATTACK			1
#define ITEMEFFECTTYPE_DEFENSE			2
#define ITEMEFFECTTYPE_ATTACK_ARROW		3
#define ITEMEFFECTTYPE_HP   		    4
#define ITEMEFFECTTYPE_MP   		    5
#define ITEMEFFECTTYPE_SP   		    6
#define ITEMEFFECTTYPE_HPSTOCK 		    7
#define ITEMEFFECTTYPE_GET			    8
#define ITEMEFFECTTYPE_STUDYSKILL		9
#define ITEMEFFECTTYPE_SHOWLOCATION		10
#define ITEMEFFECTTYPE_MAGIC			11
#define ITEMEFFECTTYPE_CHANGEATTR		12
#define ITEMEFFECTTYPE_ATTACK_MANASAVE	13
#define ITEMEFFECTTYPE_ADDEFFECT	    14
#define ITEMEFFECTTYPE_MAGICDAMAGESAVE	15
#define ITEMEFFECTTYPE_OCCUPYFLAG		16
#define ITEMEFFECTTYPE_DYE				17
#define ITEMEFFECTTYPE_STUDYMAGIC		18
#define ITEMEFFECTTYPE_ATTACK_MAXHPDOWN	19
#define ITEMEFFECTTYPE_ATTACK_DEFENSE	20
#define ITEMEFFECTTYPE_MATERIAL_ATTR	21
#define ITEMEFFECTTYPE_FIRMSTAMINAR		22
#define ITEMEFFECTTYPE_LOTTERY			23

#define ITEMEFFECTTYPE_ATTACK_SPECABLTY		24
#define ITEMEFFECTTYPE_DEFENSE_SPECABLTY	25
#define ITEMEFFECTTYPE_ALTERITEMDROP		26

#define ITEMEFFECTTYPE_CONSTRUCTIONKIT		27
#define ITEMEFFECTTYPE_WARM					28
#define ITEMEFFECTTYPE_DEFENSE_ANTIMINE		29
#define ITEMEFFECTTYPE_ITEMFARM				30  
#define ITEMEFFECTTYPE_SLATES			31
#define ITEMEFFECTTYPE_JEWELRY			32
#define ITEMEFFECTTYPE_SOCKET			33
#define ITEMEFFECTTYPE_VARIABLEADD			34
#define ITEMEFFECTTYPE_ATTACK_MAGICITEM		50
#define ITEMEFFECTTYPE_CRIT				51
#define ITEMEFFECTTYPE_ARMORDYE			52
#define ITEMEFFECTTYPE_WEAPONDYE		53
#define ITEMEFFECTTYPE_ADDSKILLS		54
#define ITEMEFFECTTYPE_ADDLUCK			55
#define ITEMEFFECTTYPE_ADDREP			56
#define ITEMEFFECTTYPE_ADDBALLPOINTS	57

enum WandEffectTypes
{
    WET_NONE,
    WET_ADDMAGICDMGPCT
};

#define ITET_UNIQUE_OWNER			1
#define ITET_ID						2
#define ITET_DATE						3
#define ITET_GM_MADE					4


#define SKILLEFFECTTYPE_GET			    1
#define SKILLEFFECTTYPE_PRETEND			2
#define SKILLEFFECTTYPE_TAMING			3

#define MAXMAPNAMES	92

static inline const char * mapNames[MAXMAPNAMES] = {
    /* 0 */  "NONE",
    /* 1 */	"arefarm",
    /* 2 */	"elvfarm",
    /* 3 */	"default",
    /* 4 */	"arebrk11",
    /* 5 */	"arebrk12",
    /* 6 */	"arebrk21",
    /* 7 */	"arebrk22",
    /* 8 */	"elvbrk11",
    /* 9 */	"elvbrk12",
    /* 10 */	"elvbrk21",
    /* 11 */	"elvbrk22",
    /* 12 */	"fightzone1",
    /* 13 */	"fightzone2",
    /* 14 */	"fightzone3",
    /* 15 */	"fightzone4",
    /* 16 */	"fightzone5",
    /* 17 */	"fightzone6",
    /* 18 */	"fightzone7",
    /* 19 */	"fightzone8",
    /* 20 */	"fightzone9",



    /* 21 */	"elvined1",
    /* 22 */	"aresden",
    /* 23 */	"aresdend1",
    /* 24 */	"elvine",
    /* 25 */	"dglv2",
    /* 26 */	"dglv3",
    /* 27 */	"bisle",
    /* 28 */	"middleland",
    /* 29 */	"huntzone1",
    /* 30 */	"huntzone2",
    /* 31 */	"huntzone3",
    /* 32 */	"huntzone4",
    /* 33 */	"bsmith_1",
    /* 34 */	"bsmith_2",
    /* 35 */	"cityhall_1",
    /* 36 */	"cityhall_2",
    /* 37 */	"gldhall_1",
    /* 38 */	"gldhall_2",
    /* 39 */	"gshop_1",
    /* 40 */	"gshop_2",
    /* 41 */	"wzdtwr_1",
    /* 42 */	"wzdtwr_2",
    /* 43 */	"wrhus_1",
    /* 44 */	"wrhus_2",
    /* 45 */	"arewrhus",
    /* 46 */	"elvwrhus",
    /* 47 */	"cath_1",
    /* 48 */	"cath_2",
    /* 49 */	"Aresden-Middle DMZ",
    /* 50 */	"Aresden-Middle DMZ",
    /* 51 */	"Elvine-Middle DMZ",
    /* 52 */	"Elvine-Middle DMZ",
    /* 53 */	"dglv4",
    /* 54 */	"resurr1",
    /* 55 */	"resurr2",
    /* 56 */	"areuni",
    /* 57 */	"elvuni",
    /* 58 */	"middled1x",
    /* 59 */	"middled1n",
    /* 60 */	"toh1",
    /* 61 */	"toh2",
    /* 62 */	"toh3",
    /* 63 */	"arejail",
    /* 64 */	"elvjail",
    /* 65 */	"2ndmiddle",
    /* 66 */	"icebound",

    /* 67 */	"Unknown",
    /* 68 */	"Unknown",
    /* 69 */	"Unknown",

    /* 70 */	"druncncity",
    /* 71 */	"inferniaA",
    /* 72 */	"inferniaB",
    /* 73 */	"maze",
    /* 74 */	"procella",
    /* 75 */	"abaddon",
    /* 76 */	"BtField",
    /* 77 */	"GodH",
    /* 78 */	"HRampart",
    /* 79 */	"CmdHall",
    /* 80 */	"Barracks1",
    /* 81 */	"Barracks2",
    /* 82 */	"Bsmith",
    /* 83 */	"default2",
    /* 84 */ "astoria",
    /* 85 */ "CmdHall_1",
    /* 86 */ "CmdHall_2",
    /* 87 */ "pvpbarrack",
    ///* 88 */ "darkcrypt",
    ///* 89 */ "pvpbar2",
    /* 88 */ "arebar",
    /* 89 */ "elvbar",
    /* 90 */ "catacombs",
    /* 91 */ "outlands"
};

#define GetMapName(i) ( ((uint32_t)i < MAXMAPNAMES) ? mapNames[i] : mapNames[0] )


#define DYNAMICOBJECT_FIRE			1
#define DYNAMICOBJECT_FISH			2
#define DYNAMICOBJECT_FISHOBJECT	3
#define DYNAMICOBJECT_MINERAL1		4
#define DYNAMICOBJECT_MINERAL2		5
#define DYNAMICOBJECT_ARESDENFLAG	6
#define DYNAMICOBJECT_ELVINEFLAG	7
#define DYNAMICOBJECT_ICESTORM		8
#define DYNAMICOBJECT_SPIKE			9
#define DYNAMICOBJECT_PCLOUD_BEGIN	10
#define DYNAMICOBJECT_PCLOUD_LOOP	11
#define DYNAMICOBJECT_PCLOUD_END	12
#define DYNAMICOBJECT_FIRE2			13
#define DYNAMICOBJECT_FIRE3			14 //for FireBow


#define EQUIPPOS_NONE		0
#define EQUIPPOS_HEAD		1
#define EQUIPPOS_BODY		2
#define EQUIPPOS_ARMS		3
#define EQUIPPOS_PANTS		4
#define EQUIPPOS_BOOTS		5
#define EQUIPPOS_NECK		6
#define EQUIPPOS_LHAND		7
#define EQUIPPOS_RHAND		8
#define EQUIPPOS_TWOHAND	9
#define EQUIPPOS_RFINGER	10
#define EQUIPPOS_LFINGER	11
#define EQUIPPOS_BACK		12
#define EQUIPPOS_FULLBODY	13     



#define ITEMEFFECTTYPE_NONE				0
#define ITEMEFFECTTYPE_ATTACK			1
#define ITEMEFFECTTYPE_DEFENSE			2
#define ITEMEFFECTTYPE_ATTACK_ARROW		3
#define ITEMEFFECTTYPE_HP   		    4
#define ITEMEFFECTTYPE_MP   		    5
#define ITEMEFFECTTYPE_SP   		    6
#define ITEMEFFECTTYPE_HPSTOCK 		    7
#define ITEMEFFECTTYPE_GET			    8
#define ITEMEFFECTTYPE_STUDYSKILL		9
#define ITEMEFFECTTYPE_SHOWLOCATION		10
#define ITEMEFFECTTYPE_MAGIC			11
#define ITEMEFFECTTYPE_CHANGEATTR		12
#define ITEMEFFECTTYPE_ATTACK_MANASAVE	13
#define ITEMEFFECTTYPE_ADDEFFECT	    14
#define ITEMEFFECTTYPE_MAGICDAMAGESAVE	15
#define ITEMEFFECTTYPE_OCCUPYFLAG		16
#define ITEMEFFECTTYPE_DYE				17
#define ITEMEFFECTTYPE_STUDYMAGIC		18
#define ITEMEFFECTTYPE_ATTACK_MAXHPDOWN	19
#define ITEMEFFECTTYPE_ATTACK_DEFENSE	20
#define ITEMEFFECTTYPE_MATERIAL_ATTR	21
#define ITEMEFFECTTYPE_FIRMSTAMINAR		22
#define ITEMEFFECTTYPE_LOTTERY			23

#define ITEMEFFECTTYPE_ATTACK_SPECABLTY		24
#define ITEMEFFECTTYPE_DEFENSE_SPECABLTY	25
#define ITEMEFFECTTYPE_ALTERITEMDROP		26

#define ITEMEFFECTTYPE_CONSTRUCTIONKIT		27
#define ITEMEFFECTTYPE_WARM					28
#define ITEMEFFECTTYPE_DEFENSE_ANTIMINE		29
#define ITEMEFFECTTYPE_ITEMFARM				30  
#define ITEMEFFECTTYPE_SLATES			31
#define ITEMEFFECTTYPE_JEWELRY			32
#define ITEMEFFECTTYPE_SOCKET			33
#define ITEMEFFECTTYPE_VARIABLEADD			34

#define ITET_UNIQUE_OWNER			1
#define ITET_ID						2
#define ITET_DATE						3
#define ITET_GM_MADE					4

#pragma region movedirection
//#ifndef 800x600
// static inline const int _tmp_iMoveLocX[9][44] = {
//     // 0
//         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         // 1 NORTH
//         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//         10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//         20, 21, 22, 23, 24, -1, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //2
//         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//         10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//         20, 21, 22, 23, 24, 24, 24, 24, 24, 24,
//         24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, -1 },//done
//         //3 EAST
//         { 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
//         24, 24, 24, 24, 24, 24, 24, 24, -1, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //4
//         { 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
//         24, 24, 24, 24, 24, 24, 24, 24, 24, 23, 22, 21,
//         20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
//         10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1 },//done
//         //5 SOUTH
//         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//         10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//         20, 21, 22, 23, 24, -1, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //6
//         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2,
//         3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
//         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, -1 },//done
//         //7 WEST
//         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //8
//         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3,
//         4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
//         14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, -1 }//done
// };
// 
// static inline const int _tmp_iMoveLocY[9][44] = {
//     // 0
//         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //1 NORTH
//         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, -1, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //2
//         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6,
//         7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 - 1 },//done
//         //3 EAST
//         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//         10, 11, 12, 13, 14, 15, 16, 17, 18, -1, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //4
//         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//         10, 11, 12, 13, 14, 15, 16, 17, 18, 18,
//         18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
//         18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, -1 },//done
//         //5 SOUTH
//         { 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
//         18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
//         18, 18, 18, 18, 18, -1, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         //6
//         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//         10, 11, 12, 13, 14, 15, 16, 17, 18, 18,
//         18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
//         18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, -1 },//done
//         //7 WEST
//         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
//         10, 11, 12, 13, 14, 15, 16, 17, 18, -1, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//done
//         // 8
//         { 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8,
//         7, 6, 5, 4, 3, 2, 1, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 }//done
// };
// 
// static inline const char _tmp_cTmpDirX[9] = { 0, 0, 1, 1, 1, 0, -1, -1, -1 };
// static inline const char _tmp_cTmpDirY[9] = { 0, -1, -1, 0, 1, 1, 1, 0, -1 };
// 
// static inline const char _tmp_cEmptyPosX[] = { 0, 1, 1, 0, -1, -1, -1, 0, 1, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2 };
// static inline const char _tmp_cEmptyPosY[] = { 0, 0, 1, 1, 1, 0, -1, -1, -1, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2 };

static inline const int _tmp_iMoveLocX[9][37] = {
    // 0
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    // 1
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //2
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,-1},
    //3
    {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //4
    {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,-1},
    //5
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //6
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,-1},
    //7
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //8
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,-1}
};

static inline const int _tmp_iMoveLocY[9][37] = {
    // 0
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //1
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //2
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,-1},
    //3
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //4
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,-1},
    //5
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //6
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,-1},
    //7
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    // 8
    {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1}
};

static inline const char _tmp_cTmpDirX[9] = { 0,0,1,1,1,0,-1,-1,-1 };
static inline const char _tmp_cTmpDirY[9] = { 0,-1,-1,0,1,1,1,0,-1 };

static inline const char _tmp_cEmptyPosX[] = { 0, 1, 1, 0, -1, -1, -1, 0 ,1, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2 };
static inline const char _tmp_cEmptyPosY[] = { 0, 0, 1, 1, 1, 0, -1, -1, -1, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2 };
#pragma endregion

#ifndef WIN32
#define ZeroMemory(a,b) memset(a, 0, b)
typedef struct tagRECT
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} RECT, * LPRECT;

void SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom)
{
    lprc->left = xLeft;
    lprc->top = yTop;
    lprc->right = xRight;
    lprc->bottom = yBottom;
}
#endif

enum Side
{
    NEUTRAL,
    ARESDEN,
    ELVINE,
    HOSTILE
};

#define ITEMS_PER_TILE	12
#define MAXGROUNDITEMS	5000

// todo - do times better with stdlib
#define TILECLEANTIME		MINUTE(15)
#define TILECLEANTIMEPLAYER	MINUTE(30)

#define SPECABLTYTIMESEC	1200


#define SEND_MAIL_COST			300
#define SEND_MAIL_ITEM_COST	700
#define GUILDBOARD_POST_COST	500
#define MAX_MAIL_MSG_LENGTH	400+1
#define MAX_MAIL_ITEMS			10

#define MAXITEMSOCKETS		4

// ----------------------------------Guild-------------------------------------------------------------------------------------
#define MAXGUILDBANKITEMS		75*4
#define GUILDSUMMONSGOLDCOST	5000
#define GUILDSUMMONSTIME		MINUTE(5)

//-----------------------------------------------------------------------------------------------------------------------

#define GetBoolText(val)	( val ? "True" : "False" )


//----------------------------------------------- TYPEDEFS ----------------------------------------------------------------------------------

//typedef std::map<int16_t, CGuild*> GuildMap;
//typedef GuildMap::iterator GuildMapIter;
//typedef GuildMap::const_iterator GuildMapCIter;
//typedef std::shared_ptr<CAstoria> Astoria;

//typedef std::list<CClient*> ClientList;
//typedef ClientList::iterator ClientListIter;
//typedef ClientList::const_iterator ClientListCIter;

using CharID = uint32_t;

struct rect
{
    rect(int32_t a, int32_t b, int32_t c, int32_t d) { left = a; top = b; right = c; bottom = d; }
    rect() { left = top = right = bottom = 0; };
    rect(const rect & r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

struct point
{
    point(int32_t a, int32_t b) { x = a; y = b; }
    point() { x = y = 0; };
    int32_t  x;
    int32_t  y;
};
