// Client.h: interface for the CClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Item.h"
#include "GuildsMan.h"
#include "Magic.h"
#include "GlobalDef.h"

#include "connection_state_hb.h"
#include <chrono>

class stream_write;

#define DEF_CLIENTSOCKETBLOCKLIMIT	15

#define DEF_MSGBUFFERSIZE	30000
#define DEF_MAXITEMS		50
#define DEF_MAXBANKITEMS	200
#define DEF_MAXGUILDSMAN	128 // ÃÖ´ë ±æµå¿ø ¼ö 

#define	DEF_MAXMAGICTYPE	100	// º¯°æÇÏ·Á¸é ·Î±×¼­¹ö³»¿ëµµ ¹Ù²Ù¾î¾ß ÇÑ´Ù.
#define DEF_MAXSKILLTYPE	60

#define DEF_MAXPARTYMEMBERS	9

#define DEF_SPECABLTYTIMESEC	1200

class CClient: public std::enable_shared_from_this<CClient>
{
public:

    // 0 = dead connect, 1 = login screen, 2 = in game
    // can switch between 1 and 2 but can only be 0 when connection closing
    uint8_t currentstatus;

    std::shared_ptr<CClient> get_ptr()
    {
        return shared_from_this();
    }

    std::list<stream_write *> outgoingqueue;

    std::string address;

    std::weak_ptr<ix::ConnectionState> connection_state;

    bool disconnected = false;
    std::chrono::time_point<std::chrono::steady_clock> connecttime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> disconnecttime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastpackettime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastchecktime = std::chrono::steady_clock::now();

    void set_disconnected(bool s);
    void set_connect_time(std::chrono::time_point<std::chrono::steady_clock> t);
    void set_disconnect_time(std::chrono::time_point<std::chrono::steady_clock> t);
    void set_last_packet_time(std::chrono::time_point<std::chrono::steady_clock> t);
    void set_last_check_time(std::chrono::time_point<std::chrono::steady_clock> t);

    bool get_disconnected();
    std::chrono::time_point<std::chrono::steady_clock> get_connect_time();
    std::chrono::time_point<std::chrono::steady_clock> get_disconnect_time();
    std::chrono::time_point<std::chrono::steady_clock> get_last_packet_time();
    std::chrono::time_point<std::chrono::steady_clock> get_last_check_time();

    int iSendMsg(const char * pMsg, std::size_t iSize, char key = 0)
    {
        auto data = ix::IXWebSocketSendData{ pMsg, (std::size_t)iSize };
        auto wsres = get_websocket()->sendBinary(data);
        if (!wsres.success)
            return -129;// DEF_XSOCKEVENT_SOCKETERROR
        return wsres.payloadSize;
    }

    connection_state_hb * get_connection_state()
    {
        auto connstate = connection_state.lock();
        if (!connstate) return nullptr;
        return reinterpret_cast<connection_state_hb *>(connstate.get());
    }

    ix::WebSocket * get_websocket()
    {
        auto connstate = get_connection_state();
        if (!connstate) return nullptr;
        auto ws = connstate->websocket.lock();
        if (!ws) return nullptr;
        return ws.get();
    }

    uint64_t socknum;
    bool logged_in = false;


		
	char m_cVar;
	int m_iRecentWalkTime;
	int m_iRecentRunTime;
	short m_sV1;
	char m_cHeroArmourBonus;

	BOOL bCreateNewParty();

	// Hack Checkers
	DWORD m_dwMagicFreqTime, m_dwMoveFreqTime, m_dwAttackFreqTime;
	BOOL m_bIsMoveBlocked, m_bMagicItem;
	DWORD dwClientTime;
	BOOL m_bMagicConfirm;
	int m_iSpellCount;
	BOOL m_bMagicPauseTime;
	//int m_iUninteruptibleCheck;
	//char m_cConnectionCheck;

	CClient(HWND hWnd);
	virtual ~CClient();

	char m_cCharName[11];
	char m_cAccountName[11];
	char m_cAccountPassword[11];

	BOOL  m_bIsInitComplete;
	BOOL  m_bIsMsgSendAvailable;

	char  m_cMapName[11];
	char  m_cMapIndex;
	short m_sX, m_sY;
	
	char  m_cGuildName[21];		// ±æµåÀÇ ÀÌ¸§ 
	char  m_cLocation[11];      //  <- ¸ÊÀÌ¸§ÀÌ ±×´ë·Î ÀúÀåµÈ´Ù. ¼Ò¼Ó ¸¶À» 
	int   m_iGuildRank;			// -1ÀÌ¸é ¹«ÀÇ¹Ì. 0ÀÌ¸é ±æµå ¸¶½ºÅÍ. ¾ç¼ö´Â ±æµå³»¿¡¼­ÀÇ ¼­¿­ 
	int   m_iGuildGUID;
	
	char  m_cDir;
	short m_sType;				// ÇöÀç Å¸ÀÔ <- Æú¸®¸ðÇÁ ½Ã¿¡ º¯ÇÑ´Ù. 
	short m_sOriginalType;		// ¿À¸®Áö³¯ Å¸ÀÔ
	short m_sAppr1;
	short m_sAppr2;
	short m_sAppr3;
	short m_sAppr4;
	int   m_iApprColor;			// v1.4 ¿ÜÇü ÄÃ·¯Å×ÀÌºí
	int   m_iStatus;

	DWORD m_dwTime, m_dwHPTime, m_dwMPTime, m_dwSPTime, m_dwAutoSaveTime, m_dwHungerTime, m_dwWarmEffectTime;
	// Player Æ¯¼ºÄ¡ 

	char m_cSex, m_cSkin, m_cHairStyle, m_cHairColor, m_cUnderwear;

	int  m_iHP;						// Hit Point
	int  m_iHPstock;
	int  m_iMP;
	int  m_iSP;
	int  m_iExp;
	int m_iNextLevelExp;
	BOOL m_bIsKilled;

	int  m_iDefenseRatio;		// Defense Ratio
	int  m_iHitRatio;			// Hit Ratio

	// v1.432 »ç¿ëÇÏÁö ¾Ê´Â´Ù.
	//int  m_iHitRatio_ItemEffect_SM; // ¾ÆÀÌÅÛ Âø¿ëÀ¸·Î ÀÎÇÑ HitRatio º¯°æ°ª
	//int  m_iHitRatio_ItemEffect_L;

	int  m_iDamageAbsorption_Armor[DEF_MAXITEMEQUIPPOS];		// °©¿Ê Âø¿ëÀ¸·Î ÀÎÇÑ Damage Èí¼ö È¿°ú
	int  m_iDamageAbsorption_Shield;	// Parrying ±â¼ú ¼º°ø½ÃÀÇ Damage Èí¼ö È¿°ú 

	int  m_iLevel;
	int  m_iStr, m_iInt, m_iVit, m_iDex, m_iMag, m_iCharisma;
	//char m_cLU_Str, m_cLU_Int, m_cLU_Vit, m_cLU_Dex, m_cLU_Mag, m_cLU_Char;   // ·¹º§¾÷½Ã¿¡ ÇÒ´çµÇ¾î ¿Ã¶ó°¡´Â Æ¯¼ºÄ¡°ª.
	int  m_iLuck; 
	int  m_iLU_Pool;
	char m_cAura;
	//MOG var - 3.51
	int m_iGizonItemUpgradeLeft;

	int m_iAddTransMana, m_iAddChargeCritical;

	int  m_iEnemyKillCount, m_iPKCount, m_iRewardGold;
	int  m_iCurWeightLoad;		// ÇöÀç ÃÑ ¼ÒÁöÇ° ¹«°Ô 

	char m_cSide;				// ÇÃ·¹ÀÌ¾îÀÇ Æí 
	
	BOOL m_bInhibition;

	char m_cAttackDiceThrow_SM;	// °ø°ÝÄ¡ ÁÖ»çÀ§ ´øÁö´Â È¸¼ö 
	char m_cAttackDiceRange_SM;	// °ø°ÝÄ¡ ÁÖ»çÀ§ ¹üÀ§ 
	char m_cAttackDiceThrow_L;	// °ø°ÝÄ¡ ÁÖ»çÀ§ ´øÁö´Â È¸¼ö 
	char m_cAttackDiceRange_L;	// °ø°ÝÄ¡ ÁÖ»çÀ§ ¹üÀ§ 
	char m_cAttackBonus_SM;		// °ø°Ý º¸³Ê½º
	char m_cAttackBonus_L;		// °ø°Ý º¸³Ê½º

	class CItem * m_pItemList[DEF_MAXITEMS];
	POINT m_ItemPosList[DEF_MAXITEMS];
	class CItem * m_pItemInBankList[DEF_MAXBANKITEMS];
	
	BOOL  m_bIsItemEquipped[DEF_MAXITEMS];
	short m_sItemEquipmentStatus[DEF_MAXITEMEQUIPPOS];
	char  m_cArrowIndex;		// ÇÃ·¹ÀÌ¾î°¡ È°À» »ç¿ëÇÒ¶§ È­»ì ¾ÆÀÌÅÛ ÀÎµ¦½º. ÃÊ±â°ªÀº -1(ÇÒ´ç ¾ÈµÊ)

	char           m_cMagicMastery[DEF_MAXMAGICTYPE];
	unsigned char  m_cSkillMastery[DEF_MAXSKILLTYPE]; // v1.4

	int   m_iSkillSSN[DEF_MAXSKILLTYPE];
	BOOL  m_bSkillUsingStatus[DEF_MAXSKILLTYPE];
	int   m_iSkillUsingTimeID[DEF_MAXSKILLTYPE]; //v1.12

	char  m_cMagicEffectStatus[DEF_MAXMAGICEFFECTS];

	int   m_iWhisperPlayerIndex;
	char  m_cProfile[256];

	int   m_iHungerStatus;		// ¹è°íÇÄ Æ÷ÀÎÆ®. ÀÌ°Ô 0ÀÌµÇ¸é ½ºÅÂ¹Ì³Ê°¡ ¿À¸£Áö ¾ÊÀ¸¸ç Ã¼·Âµµ Àý¹ÝÀÌ»ó Â÷Áö ¾Ê½À´Ï´Ù. 

	DWORD m_dwWarBeginTime;		// Àû±¹¿¡ µé¾î°¡´Â ¼ø°£¿¡ ±â·ÏµÇ´Â ½Ã°£. 
	BOOL  m_bIsWarLocation;		// ÇöÀç Àû±¹¿¡ ÀÖ´ÂÁö¸¦ Ç¥½Ã 

	BOOL  m_bIsPoisoned;		// Áßµ¶µÇ¾ú´ÂÁöÀÇ ¿©ºÎ 
	int   m_iPoisonLevel;       // µ¶ÀÇ °­µµ 
	DWORD m_dwPoisonTime;		// Áßµ¶ ½Ã°£.
	
	int   m_iPenaltyBlockYear, m_iPenaltyBlockMonth, m_iPenaltyBlockDay; // v1.4

	//v1.4311-3 Ãß°¡ º¯¼ö ¼±¾ð »çÅõÀå ¹øÈ£¿Í »çÅõÀå ¿¹¾àÇÑ ½Ã°£
	int   m_iFightzoneNumber , m_iReserveTime, m_iFightZoneTicketNumber ; 

	int   m_iAdminUserLevel;     // °ü¸®ÀÚ ·¹º§. 0ÀÌ¸é ¹«È¿. ¹øÈ£°¡ ¿Ã¶ó°¥ ¼ö·Ï ±ÇÇÑÀÌ Ä¿Áø´Ù.
	int   m_iRating;

	int   m_iTimeLeft_ShutUp;	 // ÀÌ °ªÀÌ 0ÀÌ ¾Æ´Ï¸é Ã¤ÆÃ¸Þ½ÃÁö°¡ Àü´ÞµÇÁö ¾Ê´Â´Ù.
	int   m_iTimeLeft_Rating;	 // ´Ù¸¥ ÇÃ·¹ÀÌ¾îÀÇ ÆòÆÇÀ» ¸Å±â±â À§ÇØ ³²Àº ½Ã°£. 0ÀÌ¸é »ç¿ë °¡´É  
	int   m_iTimeLeft_ForceRecall;  // °­Á¦ ¸®ÄÝµÇ±â À§ÇØ ³²¾ÆÀÖ´Â ½Ã°£Æ½ 
	int   m_iTimeLeft_FirmStaminar; // ½ºÅÂ¹Ì³Ê°¡ ´Þ¾Æ ¾ø¾îÁöÁö ¾Ê´Â ½Ã°£ ÅÒ 

	BOOL isForceSet;   //hbest
	time_t m_iForceStart;

	BOOL  m_bIsOnServerChange;     // ÀÌ °ªÀÌ È°¼ºÈ­ µÇ¾î ÀÖÀ¸¸é »èÁ¦½Ã µ¥ÀÌÅÍ ÀúÀå ¹× Ä«¿îÆÃÀ» ÇÏÁö ¾Ê´Â´Ù.

	int   m_iExpStock;			 // ½×¿©ÀÖ´Â °æÇèÄ¡ 
	DWORD m_dwExpStockTime;		 // ExpStock °è»ê ½Ã°£.

	int   m_iAutoExpAmount;		 // Auto-Exp ½Ã°£ µ¿¾È ¾òÀº °æÇèÄ¡ 
	DWORD m_dwAutoExpTime;		 // Auto-Exp °è»ê ½Ã°£.

	DWORD m_dwRecentAttackTime;  // °¡Àå ÃÖ±Ù¿¡ °ø°ÝÇß´ø ½Ã°£ 

	int   m_iAllocatedFish;		 // ÀÌ °ªÀÌ 0ÀÌ ¾Æ´Ï¸é ÀÌº¥Æ® ³¬½Ã¸ðµå¶ó´Â ÀÌ¾ß±â´Ù. 
	int   m_iFishChance;		 // ÇöÀç ³¬À» »óÅÂ 
	
	char  m_cIPaddress[21];		 // Å¬¶óÀÌ¾ðÆ®ÀÇ IP address
	BOOL  m_bIsSafeAttackMode;

	BOOL  m_bIsOnWaitingProcess; // ÅÚ·¹Æ÷Æ®µî Ã³¸®¸¦ ±â´Ù¸®´Â »óÅÂ¶ó¸é 
	
	int   m_iSuperAttackLeft;	 // v1.2 ÇÊ»ì±â »ç¿ë °¡´É È½¼ö 
	int   m_iSuperAttackCount;   // v1.2 ÇÊ»ì±â »ç¿ë °¡´É Ä«¿îÆ®. ÀÌ Ä«¿îÆ®°¡ ´Ù Â÷¸é ÇÊ»ì±â »ç¿ë È½¼ö°¡ ´Ã¾î³­´Ù. 

	short m_sUsingWeaponSkill;	 // v1.2 ÇöÀç »ç¿ëÇÏ´Â ¹«±âÀÇ ½ºÅ³ ÀÎµ¦½º 

	int   m_iManaSaveRatio;		 // v1.2 ¸¶³ª Àý¾à Æ÷ÀÎÆ® 
	
	BOOL  m_bIsLuckyEffect;		 // v1.2 Çà¿î È¿°ú 
	int   m_iSideEffect_MaxHPdown; // v1.4 ÃÖ´ë HP Àý°¨ È¿°ú 

	int   m_iComboAttackCount;   // v1.3 ¿¬Å¸ °ø°Ý Ä«¿îÆ® 
	int   m_iDownSkillIndex;	 // v1.3 ´Ù¸¥ ½ºÅ³ÀÌ ¿Ã¶ó°¥¶§ ³»¸± ½ºÅ³ ÀÎµ¦½º 

	int   m_iMagicDamageSaveItemIndex; // v1.3 ¸¶¹ý °ø°Ý ´ë¹ÌÁö Àý°¨ ¾ÆÀÌÅÛ ÀÎµ¦½º (º¸ÅëÀº -1)

	short m_sCharIDnum1, m_sCharIDnum2, m_sCharIDnum3; // v1.3 ±× Ä³¸¯ÅÍ°¡ °®´Â °íÀ¯°ª!

	int   m_iAbuseCount;		// ÇØÅ· ¿ëÀÇÀÚ ÆÄ¾Ç¿ë 
	
	BOOL  m_bIsBWMonitor;		// BadWord ¸ð´ÏÅÍÀÎ°¡?

	//BOOL  m_bIsExchangeMode;		// ÇöÀç ¾ÆÀÌÅÛ ±³È¯ ¸ðµåÀÎ°¡? 
	//int   m_iExchangeH;				// ±³È¯ÇÒ ´ë»óÀÇ ÀÎµ¦½º 
	//char  m_cExchangeName[11];		// ±³È¯ÇÒ ´ë»óÀÇ ÀÌ¸§ 
	//char  m_cExchangeItemName[21];	// ±³È¯ÇÏ°íÀÚ ÇÏ´Â ¾ÆÀÌÅÛ ÀÌ¸§ 
	//char  m_cExchangeItemIndex;  // ±³È¯ÇÒ ¾ÆÀÌÅÛ ÀÎµ¦½º 
	//int   m_iExchangeItemAmount; // ±³È¯ÇÒ ¾ÆÀÌÅÛ °¹¼ö 
	//BOOL  m_bIsExchangeConfirm;  // ±³È¯ È®ÀÎ 

	BOOL  m_bIsExchangeMode;			// Is In Exchange Mode? 
	int   m_iExchangeH;					// Client ID to Exchanging with 
	char  m_cExchangeName[11];			// Name of Client to Exchanging with 
	char  m_cExchangeItemName[4][21];	// Name of Item to exchange 

	char  m_cExchangeItemIndex[4];		// ItemID to Exchange
	int   m_iExchangeItemAmount[4];		// Ammount to exchange with

	BOOL  m_bIsExchangeConfirm;			// Has the user hit confirm? 
	int	  iExchangeCount;				//Keeps track of items which are on list

	int   m_iQuest;				 // ÇöÀç ÇÒ´çµÈ Quest 
	int   m_iQuestID;			 // ÇÒ´ç¹ÞÀº QuestÀÇ ID°ª 
	int   m_iAskedQuest;		 // ¹°¾îº» Äù½ºÆ® 
	int   m_iCurQuestCount;		 // ÇöÀç Äù½ºÆ® »óÅÂ 
	
	int   m_iQuestRewardType;	 // Äù½ºÆ® ÇØ°á½Ã »óÇ° Á¾·ù -> ¾ÆÀÌÅÛÀÇ ID°ªÀÌ´Ù.
	int   m_iQuestRewardAmount;	 // »óÇ° °¹¼ö 

	int   m_iContribution;		 // µµ½Ã¿¡ ´ëÇÑ °øÇåµµ. 

	BOOL  m_bQuestMatchFlag_Loc; // Äù½ºÆ® ¼öÇà Á¶°ÇÀ» ÆÇ´ÜÇÏ±â À§ÇÔ.
	BOOL  m_bIsQuestCompleted;   // Äù½ºÆ®°¡ ¿Ï·áµÇ¾ú´Â°¡? 

	int   m_iCustomItemValue_Attack;
	int   m_iCustomItemValue_Defense;

	int   m_iMinAP_SM;			// Custom-ItemÀÇ È¿°ú·Î ÀÎÇÑ ÃÖ¼Ò AP
	int   m_iMinAP_L;

	int   m_iMaxAP_SM;			// Custom-ItemÀÇ È¿°ú·Î ÀÎÇÑ ÃÖ´ë AP
	int   m_iMaxAP_L;

	BOOL  m_bIsNeutral;			// v1.5 Áß¸³¿©ºÎ¸¦ ÆÇº°ÇÏ±â À§ÇÑ ÇÃ·¡±×. Ã³¸® ¼Óµµ¸¦ ³ôÀÌ±â À§ÇÔÀÌ´Ù.
	BOOL  m_bIsObserverMode;	// v1.5 °ü¶÷ÀÚ ¸ðµåÀÎÁö ÆÇº° 

	int   m_iSpecialEventID;	// Æ¯¼ö Çà»ç Âü°¡È®ÀÎ¿ë ÇÃ·¡±× 

	int   m_iSpecialWeaponEffectType;	// Èñ±Í ¾ÆÀÌÅÛ È¿°ú Á¾·ù
	int   m_iSpecialWeaponEffectValue;	// Èñ±Í ¾ÆÀÌÅÛ È¿°ú °ª
	// Èñ±Í ¾ÆÀÌÅÛ È¿°ú Á¾·ù: 
	// 0-None 1-ÇÊ»ì±â´ë¹ÌÁöÃß°¡ 2-Áßµ¶È¿°ú 3-Á¤ÀÇÀÇ  
	// 5-¹ÎÃ¸ÀÇ 6-°¡º­¿î 7-¿¹¸®ÇÑ 8-°­È­µÈ 9-°í´ë¹®¸íÀÇ

	// v1.42
	int   m_iAddHP, m_iAddSP, m_iAddMP; 
	int   m_iAddAR, m_iAddPR, m_iAddDR;
	int   m_iAddMR, m_iAddAbsPD, m_iAddAbsMD; 
	int   m_iAddCD, m_iAddExp, m_iAddGold;		// °íÁ¤ ¸¶¹ý ´ë¹ÌÁö Èí¼öÀ². ¹ÝÁö·ù¿Í´Â µ¶¸³ÀûÀ¸·Î °è»êµÈ´Ù.

	int   m_iAddResistMagic;					// v1.2 Ãß°¡ ¸¶¹ý ÀúÇ× 
	int   m_iAddPhysicalDamage;					// v1.2 °íÁ¤ ´ë¹ÌÁö Ãß°¡ Æ÷ÀÎÆ® 
	int   m_iAddMagicalDamage;	

	int   m_iAddAbsAir;							// ¼Ó¼ºº° ´ë¹ÌÁö Èí¼ö
	int   m_iAddAbsEarth;
	int   m_iAddAbsFire;
	int   m_iAddAbsWater;
	
	int   m_iLastDamage;

	int   m_iMoveMsgRecvCount, m_iAttackMsgRecvCount, m_iRunMsgRecvCount, m_iSkillMsgRecvCount;
	DWORD m_dwMoveLAT, m_dwRunLAT, m_dwAttackLAT;

	int   m_iSpecialAbilityTime;				// Æ¯¼ö ´É·ÂÀ» »ç¿ëÇÏ±â À§ÇØ¼­´Â ÀÌ °ªÀÌ 0ÀÌ µÇ¾î¾ß ÇÑ´Ù. 
	BOOL  m_bIsSpecialAbilityEnabled;			// Æ¯¼ö ´É·ÂÀÌ È°¼ºÈ­ µÈ »óÅÂÀÎ°¡?
	DWORD m_dwSpecialAbilityStartTime;			// Æ¯¼ö ´É·ÂÀ» »ç¿ëÇÏ±â ½ÃÀÛÇÑ ½Ã°£
	int   m_iSpecialAbilityLastSec;				// Æ¯¼ö ´É·Â Áö¼Ó ½Ã°£.

	int   m_iSpecialAbilityType;				// ÇÒ´çµÈ Æ¯¼ö ´É·Â Á¾·ù
												// °ø°ÝÇü
												// 0:´É·Â ¾øÀ½  1:°ø°Ý½Ã Àû HP 50% °¨¼Ò  2:³Ãµ¿ È¿°ú  3: ¸¶ºñ È¿°ú  4: ¿ø¼¦ ¿øÅ³  5:ÀÔÈù ´ë¹ÌÁö ¸¸Å­ÀÇ HP¸¦ ¾ò´Â´Ù.
												// ¹æ¾îÇü
												// 50: ¹«±â ¼ö¸í 0·Î ¸¸µë. 51:ÇØ´ç ºÎÀ§ ´ë¹ÌÁö ¹«È¿È­  52: ¸ð5µç ºÎÀ§ ´ë¹ÌÁö ¹«È¿È­
	int   m_iSpecialAbilityEquipPos;			// ¹æ¾î±¸ÀÎ °æ¿ì Æ¯¼öÈ¿°ú°¡ Àû¿ëµÇ´Â ºÎÀ§¸¦ ÀÇ¹ÌÇÔ.
	BOOL  m_bIsAdminCommandEnabled;
	int   m_iAlterItemDropIndex;				// ¾ÆÀÌÅÛ ´ë½Å ¶³¾îÁö´Â ¾ÆÀÌÅÛ ÀÎµ¦½º 

	int   m_iWarContribution;					// ÀüÀï °øÇåµµ 

	DWORD m_dwSpeedHackCheckTime;				// ¼Óµµ¹ö±× °Ë»ç ·çÆ¾ 
	int   m_iSpeedHackCheckExp;		
	DWORD m_dwLogoutHackCheck;

	DWORD m_dwInitCCTimeRcv;
	DWORD m_dwInitCCTime;

	char  m_cLockedMapName[11];					// °®Èù ¸Ê ÀÌ¸§
	int   m_iLockedMapTime;						// ÀÌ °ªÀÌ 0 ÀÌ»óÀÌ¸é ¾îµð·Î ÅÚ·¹Æ÷Æ® ÇØµµ À§ÀÇ ¸ÊÀ¸·Î °£´Ù.

	int   m_iCrusadeDuty;						// Å©·ç¼¼ÀÌµå¿¡¼­ ¸ÃÀº ¿ªÇÒ: 1-¿ëº´. 2-°Ç¼³ÀÚ. 3-ÁöÈÖ°ü
	DWORD m_dwCrusadeGUID;						// Å©·ç¼¼ÀÌµå GUID
	DWORD m_dwHeldenianGUID;
	BOOL m_bInRecallImpossibleMap;

	// ÀÌ ½ºÆ®·°ÃÄ´Â ¸ÊÀÇ ³»¿ëÀ» º¹»çÇÏ´Â °ÍÀÌ´Ù. ÇÑ¹ø¿¡ º¸³» ÁÙ ¼ö ¾ø±â ¶§¹®¿¡ ¿©·¯¹ø¿¡ °ÉÃÄ ³ª´©¾î Àü¼ÛÇÑ´Ù.
	struct {
		char cType;
		char cSide;
		short sX, sY;
	} m_stCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
	int m_iCSIsendPoint;

	char m_cSendingMapName[11];
	BOOL m_bIsSendingMapStatus;

	// ÁöÈÖ°üÀÌ °Ç¼³ÇÒ ¼ö ÀÖ´Â Æ÷ÀÎÆ®. ÀÏ¹Ý ÇÃ·¹ÀÌ¾î¶ó¸é ÀÚ½ÅÀÇ Çàµ¿¿¡ ´ëÇÑ ´©Àû°ªÀÌ´Ù.
	int  m_iConstructionPoint;

	char m_cConstructMapName[11];
	int  m_iConstructLocX, m_iConstructLocY;
	
	// 2.06
	BOOL m_bIsPlayerCivil;
	BOOL m_bIsAttackModeChange;

	// New 06/05/2004
	// Party Stuff
	int m_iPartyID;
	int m_iPartyStatus;
	int m_iReqJoinPartyClientH;
	char m_cReqJoinPartyName[12];

	int   m_iPartyRank;										// Party³»¿¡¼­ÀÇ À§Ä¡. -1ÀÌ¸é ¹«ÀÇ¹Ì. 1ÀÌ¸é ÆÄÆ¼ »ý¼ºÀÚ. 12¸é ¸â¹ö 
	int   m_iPartyMemberCount;								// ÆÄÆ¼ ÀÎ¿ø Á¦ÇÑ¿ë 
	int   m_iPartyGUID;										// v1.42 Party GUID
	struct {
	int  iIndex;
	char cName[11];
	} m_stPartyMemberName[DEF_MAXPARTYMEMBERS];

	// New 07/05/2004
	DWORD m_dwLastActionTime;
	int m_iDeadPenaltyTime;

	// New 16/05/2004
	char m_cWhisperPlayerName[11];
	BOOL m_bIsAdminOrderGoto;
	BOOL m_bIsInsideWarehouse;
	BOOL m_bIsInsideWizardTower;
	BOOL m_bIsInsideOwnTown;
	BOOL m_bIsCheckingWhisperPlayer;
	BOOL m_bIsOwnLocation;
	BOOL m_pIsProcessingAllowed;

	// Updated 10/11/2004 - 24/05/2004
	char m_cHeroArmorBonus;

	// New 25/05/2004
	BOOL m_bIsBeingResurrected;

	DWORD m_dwFightzoneDeadTime;
	char m_cSaveCount;

};

#endif // !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)

