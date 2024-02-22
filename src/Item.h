//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <windows.h>
#include <cstdint>
#include <string>

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
#define DEF_EQUIPPOS_RELEASEALL	13
 
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
#define DEF_ITEMEFFECTTYPE_WARM				28
#define DEF_ITEMEFFECTTYPE_FARMING			30
#define DEF_ITEMEFFECTTYPE_SLATES			31
#define DEF_ITEMEFFECTTYPE_ARMORDYE			32
#define DEF_ITEMEFFECTTYPE_CRITKOMM			33
#define DEF_ITEMEFFECTTYPE_WEAPONDYE		34

#define DEF_ITET_UNIQUE_OWNER				1
#define DEF_ITET_ID							2
#define DEF_ITET_DATE						3

struct item_db
{
    int64_t id{};
    int64_t char_id{};
    int64_t item_id{};
    std::string name;
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
    std::string itemloc;
};

class CItem  
{
public:
	CItem();
	~CItem() = default;

    char  m_cName[30]{};

    int64_t id{};
	
	short m_sIDnum;
	char  m_cItemType;
	char  m_cEquipPos;
	short m_sItemEffectType;     
	short m_sItemEffectValue1, m_sItemEffectValue2, m_sItemEffectValue3; 
	short m_sItemEffectValue4, m_sItemEffectValue5, m_sItemEffectValue6; 
	WORD  m_wMaxLifeSpan;
	short m_sSpecialEffect;
	
	short m_sSpecialEffectValue1, m_sSpecialEffectValue2; 

	short m_sSprite;
	short m_sSpriteFrame;

	char  m_cApprValue;
	char  m_cSpeed;

	DWORD m_wPrice; 
	WORD  m_wWeight;
	short m_sLevelLimit;	
	char  m_cGenderLimit;

	short m_sRelatedSkill;
	
	char  m_cCategory;
	BOOL  m_bIsForSale;

	DWORD m_dwCount;
	short m_sTouchEffectType;
	short m_sTouchEffectValue1, m_sTouchEffectValue2, m_sTouchEffectValue3;
	char  m_cItemColor;
	short m_sItemSpecEffectValue1, m_sItemSpecEffectValue2, m_sItemSpecEffectValue3;
	WORD  m_wCurLifeSpan;
	DWORD m_dwAttribute;
};
