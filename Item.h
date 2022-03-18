// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)
#define AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_

#include <windows.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
										   
#define DEF_MAXITEMEQUIPPOS		15
#define DEF_EQUIPPOS_NONE		0	
#define DEF_EQUIPPOS_HEAD		1	//66Ah	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD]
#define DEF_EQUIPPOS_BODY		2	//66Ch	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY]
#define DEF_EQUIPPOS_ARMS		3	//66Eh	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS]
#define DEF_EQUIPPOS_PANTS		4	//670h	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_PANTS]
#define DEF_EQUIPPOS_LEGGINGS	5	//672h	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS]
#define DEF_EQUIPPOS_NECK		6   //674h	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_NECK]
#define DEF_EQUIPPOS_LHAND		7	//676h  m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND]
#define DEF_EQUIPPOS_RHAND		8	//678h	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]
#define DEF_EQUIPPOS_TWOHAND	9	//67Ah	m_pClientList[]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]
#define DEF_EQUIPPOS_RFINGER	10	//67Ch
#define DEF_EQUIPPOS_LFINGER	11	//67Eh
#define DEF_EQUIPPOS_BACK		12  //680h
#define DEF_EQUIPPOS_RELEASEALL	13	//682h
 
#define DEF_ITEMTYPE_NOTUSED	-1	// v1.4 ╠Ба╦©║ ╩Г©К╣г╬Зю╦Ё╙ гЖюГ╥н╪╜╢б ╩Г©К╣гаЖ ╬й╢б ╬фюлеш: фВ╩С╠щю╦╥н ╢Кц╪╣х╢ы.
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
#define DEF_ITEMEFFECTTYPE_ATTACK			1		// ╟Ь╟щд║: value1 D value2 + value3
#define DEF_ITEMEFFECTTYPE_DEFENSE			2		// ╧Ф╬Н╢и╥б 
#define DEF_ITEMEFFECTTYPE_ATTACK_ARROW		3		// х╜╩Лю╩ ╩Г©Кго╢б ╟Ь╟щ╧╚╠Б. ╧╚╠Б юзц╪╢б ╟Ь╟щ╥бюл ╬Ь╢ы.		
#define DEF_ITEMEFFECTTYPE_HP   		    4		// HP╟Э╥ц х©╟З 
#define DEF_ITEMEFFECTTYPE_MP   		    5		// MP╟Э╥ц х©╟З 
#define DEF_ITEMEFFECTTYPE_SP   		    6		// SP╟Э╥ц х©╟З 
#define DEF_ITEMEFFECTTYPE_HPSTOCK 		    7		// HP╟Э╥ц х©╟З. аО╟╒юШю╦╥н ╧щюююл Ё╙е╦Ё╙аЖ╢б ╬й╢б╢ы. 
#define DEF_ITEMEFFECTTYPE_GET			    8		// ╬Р╢б╢ы. ╧╟╟М╠БЁ╙ ╠╓╧╟╣Н 
#define DEF_ITEMEFFECTTYPE_STUDYSKILL		9		// ╠Б╪Зю╩ ╧Х©Л╢б ╬фюлешюл╢ы.
#define DEF_ITEMEFFECTTYPE_SHOWLOCATION		10		// ю╖д║╦╕ г╔╫цго╢б ╬фюлеш. 
#define DEF_ITEMEFFECTTYPE_MAGIC			11		// ╩Г©Кго╦И ╦╤╧Щ х©╟З╦╕ ╬Р╢б ╬фюлеш 
#define DEF_ITEMEFFECTTYPE_CHANGEATTR		12		// гц╥╧юл╬Нюг ╪с╪╨ю╩ ╨╞х╞╫це╡╢ы. ╪╨╨╟, ╦с╦╝╦П╬Г, гг╨н╩Ж, гЛ╬Н╫╨е╦юо 
#define DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE	13		// ╟Ь╟щх©╟З©м ╢У╨р╬Н ╦╤Ё╙юг юЩ╬Ю х©╟З╟║ юж╢ы.(╦╤╧Щ╩Г©К)
#define DEF_ITEMEFFECTTYPE_ADDEFFECT	    14		// цъ╟║ х©╟З
#define DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE	15		// ╦╤╧Щю╦╥н юнгя ╟Ь╟щюг ╢К╧лаЖ╦╕ юЩ╟╗гя╢ы. ╧щаЖЁ╙ ╦Я╟июл╥Ы 
#define DEF_ITEMEFFECTTYPE_OCCUPYFLAG		16		// а║╥и ╠Й╧ъ 
#define DEF_ITEMEFFECTTYPE_DYE				17		// ©╟╩Ж╬Ю. 
#define DEF_ITEMEFFECTTYPE_STUDYMAGIC		18		// ╦╤╧Щю╩ ╧Х©Л╢б ╬фюлешюл╢ы.
#define DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN	19		// ╟Ь╟щ х©╟З©м гт╡╡ цж╢К  HP ╧в HP х╦╨╧╥╝юл ╤Ё╬НаЖ╢б ╬фюлеш 
#define DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE	20		// ╟Ь╟щ х©╟З©м гт╡╡ ╟╘©й╟З ╟╟юл ╧╟╦╝ ╧Ф╬Н╠╦ х©╟З╟║ юж╢б ╬фюлеш 
#define DEF_ITEMEFFECTTYPE_MATERIAL_ATTR	21		// ╧╚╠Б а╕юш юГ╥А ╪с╪╨ 
#define DEF_ITEMEFFECTTYPE_FIRMSTAMINAR		22		// ╫╨еб╧лЁй ╟Ма╓ ╬фюлеш 
#define DEF_ITEMEFFECTTYPE_LOTTERY			23		// ╨╧╠г ╬фюлеnш
#define DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY		24	// ╟Ь╟щ ╬фюлеш ф╞╪Ж х©╟З 
#define DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY	25	// ╧Ф╬Н╠╦ ╬фюлеш ф╞╪Ж х©╟З 
#define DEF_ITEMEFFECTTYPE_ALTERITEMDROP		26	// ╢ы╦╔ ╬фюлеш ╤Ё╬НаЗ ╟м ╢К╫е юл ╬фюлешюл ╤Ё╬НаЬ╢ы.
#define DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT		27	// ╟гцЮ е╟ф╝
#define DEF_ITEMEFFECTTYPE_WARM				28		// Unfreeze pot
#define DEF_ITEMEFFECTTYPE_FARMING			30
#define DEF_ITEMEFFECTTYPE_SLATES			31 // Ancient Tablets
#define DEF_ITEMEFFECTTYPE_ARMORDYE			32 // Armor Dyes
#define DEF_ITEMEFFECTTYPE_CRITKOMM			33 // Crit Candy
#define DEF_ITEMEFFECTTYPE_WEAPONDYE		34 // Weapons Dyes

#define DEF_ITET_UNIQUE_OWNER				1		// ItemTouchEffect: ажюню╩ ╟╝╢б ╬фюлеш 
#define DEF_ITET_ID							2		// ╠вЁи ╬фюлеш ╬фюл╣П
#define DEF_ITET_DATE						3		// ╩Г©К╟║╢игя Ё╞б╔╟║ аЖа╓╣х ╬фюлеш 


class CItem  
{
public:
	CItem();
	virtual ~CItem();

	char  m_cName[21];
	
	short m_sIDnum;					// ╬фюлешюг ╟Мю╞ ╧Ьхё
	char  m_cItemType;
	char  m_cEquipPos;
	short m_sItemEffectType;     
	short m_sItemEffectValue1, m_sItemEffectValue2, m_sItemEffectValue3; 
	short m_sItemEffectValue4, m_sItemEffectValue5, m_sItemEffectValue6; 
	WORD  m_wMaxLifeSpan;
	short m_sSpecialEffect;
	
	//short m_sSM_HitRatio, m_sL_HitRatio;
	//v1.432 ╦Маъ╥Э ╟║╟╗ ╩Г©К ╬хгя╢ы. ╢К╫е ф╞╪Ж ╢и╥б ╪Жд║╟║ ╣И╬Н╟ё╢ы.
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
	char  m_cItemColor; // v1.4 ©║╪╜ ╬фюлеш ╩Жю╦╥н ╨╞╟Ф╣г╬З╢ы. 
	short m_sItemSpecEffectValue1, m_sItemSpecEffectValue2, m_sItemSpecEffectValue3;
	WORD  m_wCurLifeSpan;
	DWORD m_dwAttribute;				// aaaa bbbb cccc dddd eeee ffff xxxx xxx1 
										// 1: Custom-Made Item flag 
										// a: Item ╪с╪╨ а╬╥Ы 
										// b: Item ╪с╪╨ а╓╣╣
										// c: ф╞╪Ж ╬фюлеш ╪╨аЗ Flag 
										// d: ф╞╪Ж ╬фюлеш ╪╨аЗ а╓╣╣ 
										// e: ф╞╪Ж ╬фюлеш цъ╟║ ╪╨аЗ Flag
										// f: ф╞╪Ж ╬фюлеш цъ╟║ ╪╨аЗ а╓╣╣ 
										 

};

/*
╬фюлеш а╓югфдюо©║ гй©Дгя Ё╩©К - ╟тюс ╪╜╧Ж©К

char cItemType
 - ╬фюлешюг а╬╥Ы. юЕбЬ╣г╢б ╟мюнаЖ ╪р╦П╣г╢б ╟мюнаЖ©║ ╢Кгя а╓юг

char cEquipPos
 - юЕбЬ╣г╢б ╬фюлешюл╤С╦И ╠в ю╖д║ 

short sItemEffectType     
  - ╬фюлешюг х©╟З а╬╥Ы.  

short sItemEffectValue1, 2, 3 ... 
 - ╬фюлеш х©╟Зюг ╪Жд║ 1, 2, 3...

WORD wMaxLifeSpan
 - ╬фюлешюг цж╢К ╪Ж╦М 

short sMaxFixCount
 - цж╢К ╟Мд╔╪Ж юж╢б х╫╪Ж

short sSprite
short sSpriteFrame
 - ╬фюлеш ╫╨га╤Сюлф╝ ╧Ьхё 

WORD  wPrice 
 - ╬фюлеш ╟║╟щ 
WORD  wWeight
 - ╬фюлеш ╧╚╟т 
short sLevelLimit
 - ╬фюлеш ╥╧╨╖ а╕гя 
char  cGenderLimit
 - ╬фюлеш ╪╨╨╟ а╕гя 

short m_SM_HitRatio, m_sL_HitRatio
 - ╦Яг╔╨╟ ╦Маъ╥Э ╟║╟╗д║ (╧╚╠Бюо ╟Ф©Л)

short sRelatedSkill
 - ©╛╟А╣х Skill

гц╥╧юл╬Н ╣╔юлем фдюо©║ юЗюЕ╣г╬Н╬ъ гр а╓╨╦ - ╥н╠в ╪╜╧Ж©К 

DWORD dwCount
 - ╬фюлешюг ╟╧╪Ж. (╟╟ю╨ а╬╥Ыюг х╜╩ЛюлЁ╙ фВ╪г, Gold╣Н╦╦юл юг╧л╦╕ ╟╝╢б╢ы)

WORD wCurLifeSpan
 - гЖюГ Ё╡ю╨ ╪Ж╦М 

short sCurFixCount
 - гЖюГ╠НаЖ ╪Ж╦╝гя х╫╪Ж 

short sTouchEffectType
 - ╬фюлешю╩ цЁю╫ ╟г╣Е╥хю╩╤╖ юш╣©╣г╢б х©╟З а╬╥Ы 

short sTouchEffectValue1, 2, 3...
 - ╬фюлеш а╒цк х©╟Зюг ╪Жд║ 

short sItemSpecEffectType
 - ╬фюлешюг ф╞╪Жх©╟З а╬╥Ы (ex:╦╤╧Щюл ╟и╦╟ ╧╚╠Б╤С╦И ╟Ь╟щ╥бюлЁ╙ ╧Ф╬Н╥б©║ ©╣гБюл южю╩╪Ж╣╣)

short sItemSpecEffectValue1, 2, 3...
 - ╬фюлеш ф╞╪Жх©╟Зюг ╪Жд║ 1, 2, 3...


©й ╬фюлешюн ╟Ф©Л(Equip Positionюл Body, Leggings, Armsюн ╟Ф©Л)
sItemEffectValue4: ©йюг ф╞а╓ ф╞╪╨д║ а╕гя а╬╥Ы╦╕ Ё╙е╦Ё╫╢ы. 

  10: Str
  11: Dex
  12: Vit
  13: Int
  14: Mag
  15: Chr

sItemEffectValue5: ©йюг ф╞а╓ ф╞╪╨д║ а╕гя ╪Жаь



*/

#endif // !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)
