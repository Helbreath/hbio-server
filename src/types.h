//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once
#include <cstdint>
#include <string>
#include <vector>

#define DEF_MSGBUFFERSIZE	30000
#define DEF_MAXITEMS		50
#define DEF_MAXBANKITEMS	200
#define DEF_MAXGUILDSMAN	128

#define	DEF_MAXMAGICTYPE	100
#define DEF_MAXSKILLTYPE	60

#define DEF_MAXPARTYMEMBERS	9

#define DEF_SPECABLTYTIMESEC	1200

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

enum class manager_status_t
{
    CONNECTED = 1,
    AUTHENTICATED = 2,
};

enum class client_status_t
{
    LOGIN_SCREEN = 1,
    CONNECTING_TO_GAME,
    PLAYING, // misc status?
};

enum class character_status_t
{
    ALIVE = 1,
    DEAD = 2,
};

enum class server_status_t
{
    UNINITIALIZED = 0,
    RUNNING,
    RUNNING_QUEUE,
    SHUTDOWN,
    OFFLINE,
};

enum class gender_t
{
    MALE = 0,
    FEMALE = 1,
    NONE = 2,
};

enum class character_item_position_t
{
    NONE = 0,
    HEAD = 1,
    BODY = 2,
    ARMS = 3,
    PANTS = 4,
    LEGGINGS = 5,
    NECK = 6,
    LHAND = 7,
    RHAND = 8,
    TWOHAND = 9,
    RFINGER = 10,
    LFINGER = 11,
    BACK = 12,
    FULLBODY = 13,
};

// todo
enum class item_type_t
{
    NONE = 0,
    EQUIP = 1,
    APPLY = 2,
    USE_DEPLETE = 3,
    INSTALL = 4,
    CONSUME = 5,
    ARROW = 6,
    EAT = 7,
    USE_SKILL = 8,
    USE_PERM = 9,
    USE_SKILL_ENABLEDIALOGBOX = 10,
    USE_DEPLETE_DEST = 11,
    MATERIAL = 12,
};

constexpr std::string_view character_status_to_string(character_status_t status)
{
    switch (status)
    {
        case character_status_t::ALIVE:
            return "ALIVE";
        case character_status_t::DEAD:
            return "DEAD";
        default:
            return "UNKNOWN";
    }
}

constexpr std::string_view client_status_to_string(client_status_t status)
{
    switch (status)
    {
        case client_status_t::LOGIN_SCREEN:
            return "LOGIN_SCREEN";
        case client_status_t::CONNECTING_TO_GAME:
            return "CONNECTING_TO_GAME";
        case client_status_t::PLAYING:
            return "PLAYING";
        default:
            return "UNKNOWN";
    }
}

constexpr std::string_view server_status_to_string(server_status_t status)
{
    switch (status)
    {
        case server_status_t::UNINITIALIZED:
            return "UNINITIALIZED";
        case server_status_t::RUNNING_QUEUE:
            return "RUNNING_QUEUE";
        case server_status_t::RUNNING:
            return "RUNNING";
        case server_status_t::SHUTDOWN:
            return "SHUTDOWN";
        case server_status_t::OFFLINE:
            return "OFFLINE";
        default:
            return "UNKNOWN";
    }
}

constexpr std::string_view character_item_position_to_string(character_item_position_t pos)
{
    switch (pos)
    {
        case character_item_position_t::NONE:
            return "NONE";
        case character_item_position_t::HEAD:
            return "HEAD";
        case character_item_position_t::BODY:
            return "BODY";
        case character_item_position_t::ARMS:
            return "ARMS";
        case character_item_position_t::PANTS:
            return "PANTS";
        case character_item_position_t::LEGGINGS:
            return "LEGGINGS";
        case character_item_position_t::NECK:
            return "NECK";
        case character_item_position_t::LHAND:
            return "LHAND";
        case character_item_position_t::RHAND:
            return "RHAND";
        case character_item_position_t::TWOHAND:
            return "TWOHAND";
        case character_item_position_t::RFINGER:
            return "RFINGER";
        case character_item_position_t::LFINGER:
            return "LFINGER";
        case character_item_position_t::BACK:
            return "BACK";
        case character_item_position_t::FULLBODY:
            return "FULLBODY";
        default:
            return "UNKNOWN";
    }
}

constexpr std::string_view item_type_to_string(item_type_t type)
{
    switch (type)
    {
        case item_type_t::NONE:
            return "NONE";
        case item_type_t::EQUIP:
            return "EQUIP";
        case item_type_t::APPLY:
            return "APPLY";
        case item_type_t::USE_DEPLETE:
            return "USE_DEPLETE";
        case item_type_t::INSTALL:
            return "INSTALL";
        case item_type_t::CONSUME:
            return "CONSUME";
        case item_type_t::ARROW:
            return "ARROW";
        case item_type_t::EAT:
            return "EAT";
        case item_type_t::USE_SKILL:
            return "USE_SKILL";
        case item_type_t::USE_PERM:
            return "USE_PERM";
        case item_type_t::USE_SKILL_ENABLEDIALOGBOX:
            return "USE_SKILL_ENABLEDIALOGBOX";
        case item_type_t::USE_DEPLETE_DEST:
            return "USE_DEPLETE_DEST";
        case item_type_t::MATERIAL:
            return "MATERIAL";
        default:
            return "UNKNOWN";
    }
}

struct item_db
{
    int64_t id{};
    int64_t char_id{};
    int64_t item_id{};
    std::string name{};
    int64_t count{};
    int32_t type{};
    int32_t color{};
    int32_t id1{};
    int32_t id2{};
    int32_t id3{};
    int32_t effect1{};
    int32_t effect2{};
    int32_t effect3{};
    int32_t durability{};
    int64_t attribute{};
    bool equipped{};
    int32_t itemposx{};
    int32_t itemposy{};
    std::string itemloc{};
};

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

struct skill_db
{
    int64_t id{};
    int64_t character_id{};
    int64_t skill_id{};
    int32_t skill_level{};
    int32_t skill_exp{};
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
    std::vector<skill_db> skills{};
    std::vector<item_db> items{};
    std::vector<item_db> bank_items{};
};
