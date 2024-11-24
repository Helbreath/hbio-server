//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"
#include "Map.h"
#include "Npc.h"
#include <windows.h>

extern char G_cTxt[512];

void CGame::NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType)
{
    CItem * pItem;
    char  cColor, cItemName[21], cTemp[20];
    bool  bIsGold;
    int   i = 0, iGenLevel = 0, iResult = 0, iT1 = 0, iT2 = 0, iT3 = 0, iItemID = 0;
    uint32_t dwType = 0, dwValue = 0;
    double dTmp1 = 0, dTmp2 = 0, dTmp3 = 0;
    SYSTEMTIME SysTime, SysTime2{};

    if (m_pNpcList[iNpcH] == 0) return;
    if ((cAttackerType != DEF_OWNERTYPE_PLAYER) || (m_pNpcList[iNpcH]->m_bIsSummoned == true)) return;

    memset(cItemName, 0, sizeof(cItemName));
    bIsGold = false;

    switch (m_pNpcList[iNpcH]->m_sType)
    {
        case 21: // Guard
        case 34: // Dummy
        case 66: // Wyvern
            return;
    }


    if (NpcDeadItemGeneratorWithItemEvent(iNpcH, sAttackerH, cAttackerType) == true)
        return;



    int iItemprobability = 7500;//8500 ;
    if ((m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING))
    {
        iItemprobability -= 900;
    }


#ifdef DEF_TESTSERVER
    iItemprobability -= 3000;
#endif

#ifdef DEF_ITEMDROPRATINGUP
    iItemprobability -= DEF_ITEMDROPRATINGUP;
#endif


    if (iDice(1, 10000) >= iItemprobability) //Change iItemprobability
    {

        if (iDice(1, 10000) <= 2000)
        {
            // 10% Áß 50%´Â Gold´Ù.
            // Gold: (10/100) * (50/100) = 5%


            strcpy(cItemName, "Gold");

            pItem = new CItem;
            if (_bInitItemAttr(pItem, cItemName) == false)
            {
                delete pItem;
                return;
            }


            pItem->m_dwCount = (iDice(1, (m_pNpcList[iNpcH]->m_iGoldDiceMax - m_pNpcList[iNpcH]->m_iGoldDiceMin))) + m_pNpcList[iNpcH]->m_iGoldDiceMin;

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_iAddGold != 0))
            {
                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddGold;
                dTmp2 = (double)pItem->m_dwCount;
                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                pItem->m_dwCount += (int)dTmp3;
            }

#ifdef DEF_TESTSERVER
            pItem->m_dwCount = (pItem->m_dwCount * 10);
#endif
            pItem->m_dwCount = (pItem->m_dwCount / 3);

        }
        else
        {

            if (iDice(1, 10000) <= 3000)
            {
                iResult = iDice(1, 10000);
                if ((iResult >= 1) && (iResult <= 3000))          dwValue = 1;
                else if ((iResult >= 3001) && (iResult <= 4000))  dwValue = 2;
                else if ((iResult >= 4001) && (iResult <= 5500))  dwValue = 3;
                else if ((iResult >= 5501) && (iResult <= 7000))  dwValue = 4;
                else if ((iResult >= 7001) && (iResult <= 8500))  dwValue = 5;
                else if ((iResult >= 8501) && (iResult <= 9100))  dwValue = 6;
                else if ((iResult >= 9101) && (iResult <= 9900))  dwValue = 7;
                else if ((iResult >= 9901) && (iResult <= 10000)) dwValue = 8;
                //else if ((iResult >= 10001) && (iResult <= 12000)) dwValue = 9

                switch (dwValue)
                {
                    case 1: iItemID = 95;  break;
                    case 2: iItemID = 91;  break;
                    case 3: iItemID = 93;  break;
                    case 4: iItemID = 96;  break;
                    case 5: iItemID = 92;  break;
                    case 6: iItemID = 94;  break;
                    case 7:
                        if (iDice(1, 3) == 1)
                            iItemID = 390;
                        else iItemID = 95;
                        break;
                    case 8:

#ifdef DEF_CONSULTATION
                        switch (iDice(1, 8))
                        {
                            case 1:
                            case 2:
                            case 3:
                            case 4:
                            case 5: // v2.172
                            case 6: // v2.172
                                if (iDice(1, 50) == 1)
                                    iItemID = 391;
                                else iItemID = 95;
                                break;

                            case 7:
                            case 8:
                                switch (iDice(1, 8))
                                { // v2.172
                                    case 1: iItemID = 391;
                                        break;

                                    case 2: if (iDice(1, 200) == 1)
                                        iItemID = 652;
                                          else iItemID = 651;
                                        break;
                                    case 3: if (iDice(1, 200) == 3)
                                        iItemID = 653;
                                          else iItemID = 651;
                                        break;
                                    case 4: if (iDice(1, 300) == 5)
                                        iItemID = 654;
                                          else iItemID = 391;
                                        break;
                                    case 5:
                                        if (iDice(1, 100000) == 11)
                                            iItemID = 655;
                                        else iItemID = 391;
                                        break;
                                    case 6:
                                        if (iDice(1, 50) == 3)
                                            iItemID = 656;
                                        else iItemID = 391;
                                        break;
                                    case 7:
                                        if (iDice(1, 50) == 3)
                                            iItemID = 657;
                                        else iItemID = 391;
                                        break;

                                    case 8:
                                        iItemID = 95;
                                        break;
                                }

#else // #ifdef DEF_CONSULTATION
                        switch (iDice(1, 8))
                        {
                            case 1:
                            case 2: if (iDice(1, 3) == 1) iItemID = 650;
                                break;
                            case 3:
                            case 4:
                            case 5: // v2.172
                            case 6: // v2.172
                                if (iDice(1, 3) == 1)
                                    iItemID = 391;
                                else iItemID = 95;
                                break;

                            case 7:
                            case 8:
#ifdef DEF_BALLEVENT
                                switch (iDice(1, 8))
                                { // v2.172
                                    case 1: if (iDice(1, 50) == 11)
                                        iItemID = 651;
                                          else iItemID = 95;
                                        break;

                                    case 2: if (iDice(1, 50) == 13)
                                        iItemID = 652;
                                          else iItemID = 95;
                                        break;
                                    case 3: if (iDice(1, 50) == 13)
                                        iItemID = 653;
                                          else iItemID = 95;
                                        break;
                                    case 4: if (iDice(1, 50) == 33)
                                        iItemID = 654;
                                          else iItemID = 95;
                                        break;
                                    case 5:
                                        if (iDice(1, 50) == 11)
                                            iItemID = 655;
                                        else iItemID = 95;
                                        break;
                                    case 6:
                                        if (iDice(1, 50) == 3)
                                            iItemID = 656;
                                        else iItemID = 391;
                                        break;
                                    case 7:
                                        if (iDice(1, 50) == 3)
                                            iItemID = 657;
                                        else iItemID = 391;
                                        break;
                                    case 8:
                                        iItemID = 95;
                                        break;
                                }
#else 
                                switch (iDice(1, 8))
                                { // v2.172
                                    case 1:
                                    case 2:
                                    case 3:
                                    case 4:
                                    case 5:
                                        iItemID = 95;
                                        break;
                                    case 6:
                                        if (iDice(1, 50) == 3)
                                            iItemID = 656;
                                        else iItemID = 391;
                                        break;
                                    case 7:
                                        if (iDice(1, 50) == 3)
                                            iItemID = 657;
                                        else iItemID = 391;
                                        break;
                                    case 8:
                                        break;
                                }

#endif // #ifdef DEF_BALLEVENT						

#endif // #ifdef DEF_CONSULTATION
                        }
                        //					case 9: //v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã
                        //#ifdef DEF_CHINESECANDYEVENT
                        //						GetLocalTime(&SysTime2);
                        //						if( SysTime2.wYear == 2002 && SysTime2.wMonth == 12 ) {
                        //							if(m_pNpcList[iNpcH]->m_sType == 55 || m_pNpcList[iNpcH]->m_sType == 61)  {					
                        //								switch( iDice(1, 3) ) {
                        //								case 1: iItemID = 780; break;
                        //								case 2: iItemID = 781; break;
                        //								case 3: iItemID = 782; break;
                        //								}
                        //							}
                        //						}
                        //						else
                        //						{
                        //						}
                        //#else
                        //						if(m_pNpcList[iNpcH]->m_sType == 55 || m_pNpcList[iNpcH]->m_sType == 61)  {					
                        //							switch( iDice(1, 3) ) {
                        //							case 1: iItemID = 780; break;
                        //							case 2: iItemID = 781; break;
                        //							case 3: iItemID = 782; break;
                        //							}
                        //						}
                        //#endif
                        //						break;

                        }

                /*
                if( iItemID >= 651 && iItemID <= 655 )
                {
                GetLocalTime(&SysTime);
                if( SysTime.wYear == 2002 && SysTime.wMonth == 11 && SysTime.wDay >=1 && SysTime.wDay <= 7 )
                return;
                }
                */


                pItem = new CItem;
                if (_bInitItemAttr(pItem, iItemID) == false)
                {
                    delete pItem;
                    return;
                }


                }
            else
            {


                switch (m_pNpcList[iNpcH]->m_sType)
                {
                    case 10: // Slime
                    case 16: // Giant-Ant
                    case 22: // Amphis 
                    case 55: // Rabbit	//v2.19 2002-12-9
                    case 56: // Cat 
                        iGenLevel = 1;
                        break;

                    case 14: // Orc
                    case 18: // Zombie
                    case 17: // Scorpion
                    case 11: // Skeleton
                        iGenLevel = 2;
                        break;

                    case 23: // clay-golem
                    case 12: // stone-golem
                        iGenLevel = 3;
                        break;

                    case 61: //v2.19 2002-12-9 ·çµ¹ÇÁ Ãß°¡ °ü·Ã
                    case 27: // hellhound
                        iGenLevel = 4;
                        break;

                    case 13: // Cyclops
                    case 28: // Troll
                    case 53: // Beholder

                    case 60: // Cannibal-Plant
                    case 62: // DireBoar
                        iGenLevel = 5;
                        break;

                    case 29: // Orge
                    case 33: // WereWolf
                    case 48: // Stalker
                    case 54: // Dark-Elf
                    case 65: // Ice-Golem
                        iGenLevel = 6;
                        break;

                    case 30: // Liche
                    case 63: // Frost
                        iGenLevel = 7;
                        break;

                    case 31: // Demon
                    case 32: // Unicorn
                    case 49: // Hellclaw
                    case 50: // Tigerworm
                    case 52: // Gagoyle
                        iGenLevel = 8;
                        break;


                    case 58: // Mountain-Giant
                        if (iDice(1, 3) == 1)
                            iGenLevel = 9;
                        break;

                    case 59: // Ettin
                        if (iDice(1, 2) == 1)
                            iGenLevel = 10;
                        break;

                }

                if (iGenLevel == 0) return;






                if (iDice(1, 10000) <= 6500)
                {

                    if (iDice(1, 10000) <= 8000)
                    {

                        switch (iGenLevel)
                        {
                            case 1:
                                switch (iDice(1, 3))
                                {
                                    case 1: iItemID = 1; break;
                                    case 2: iItemID = 8; break;
                                    case 3: iItemID = 59; break;
                                }
                                break;

                            case 2:
                                switch (iDice(1, 6))
                                {
                                    case 1: iItemID = 12; break;  //"¸Á°í½´"
                                    case 2: iItemID = 15; break;  //"±×¶óµð¿ì½º"
                                    case 3: iItemID = 65; break;  //"»ö½¼-¾×½º"
                                    case 4: iItemID = 62; break;
                                    case 5: iItemID = 23; break;
                                    case 6: iItemID = 31; break;  //"¿¡½ºÅÍÅ©"
                                }
                                break;

                            case 3:
                                switch (iDice(1, 4))
                                {
                                    case 1: iItemID = 17; break;  //"·Õ-¼Òµå"
                                    case 2: iItemID = 68; break;  //"´õºí¾×½º"
                                    case 3: iItemID = 23; break;  //"¼¼ÀÌ¹ö"
                                    case 4: iItemID = 31; break;  //"¿¡½ºÅÍÅ©"
                                }
                                break;

                            case 4:
                                switch (iDice(1, 5))
                                {
                                    case 1: iItemID = 23; break;   //"¼¼ÀÌ¹ö"
                                    case 2: iItemID = 25; break;   //"½Ã¹ÌÅ¸"
                                    case 3: iItemID = 28; break;   //"ÆÈÄ¡¿Â"
                                    case 4: iItemID = 31; break;   //"¿¡½ºÅÍÅ©"
                                    case 5: iItemID = 34; break;   //"·¹ÀÌÇÇ¾î"
                                }
                                break;

                            case 5:
                                switch (iDice(1, 3))
                                {
                                    case 1: iItemID = 31; break;   //"¿¡½ºÅÍÅ©"
                                    case 2: iItemID = 34; break;   //"·¹ÀÌÇÇ¾î"
                                    case 3: iItemID = 71; break;   //"¿ö-¾×½º"
                                }
                                break;

                            case 6:
                                switch (iDice(1, 6))
                                {
                                    case 1: iItemID = 50; break;   //"±×·¹ÀÌÆ®-¼Òµå"
                                    case 2: iItemID = 54; break;   //"ÇÃ·¥¹ö±×"
                                    case 3: iItemID = 46; break;   //"Å¬·¹ÀÌ¸ð¾î"
                                    case 4: iItemID = 31; break;   //"¿¡½ºÅÍÅ©"
                                    case 5: iItemID = 34; break;   //"·¹ÀÌÇÇ¾î"
                                    case 6: iItemID = 617; break;  //"ÄÞÆ÷ÁöÆ®-º¸¿ì"
                                }
                                break;

                            case 7:
                                switch (iDice(1, 4))
                                {
                                    case 1: iItemID = 50; break;   //"±×·¹ÀÌÆ®-¼Òµå"
                                    case 2: iItemID = 54; break;   //"ÇÃ·¥¹ö±×"
                                    case 3: iItemID = 31; break;   //"¿¡½ºÅÍÅ©"
                                    case 4: iItemID = 34; break;   //"·¹ÀÌÇÇ¾î"
                                }
                                break;

                            case 8:
                                switch (iDice(1, 7))
                                {
                                    case 1: iItemID = 50; break;   //"±×·¹ÀÌÆ®-¼Òµå"
                                    case 2: iItemID = 54; break;   //"ÇÃ·¥¹ö±×"
                                    case 3: iItemID = 560; break;  //"¹èÆ²-¾×½º"
                                    case 4: iItemID = 31; break;   //"¿¡½ºÅÍÅ©"
                                    case 5: iItemID = 34; break;   //"·¹ÀÌÇÇ¾î"
                                    case 6: iItemID = 55; break;   //"ÇÃ·¥¹ö±×+1"
                                    case 7: iItemID = 615; break;  //"ÀÚÀÌ¾ÈÆ®-¼Òµå"
                                }
                                break;

                            case 9: // Mountain-Giant
                                switch (iDice(1, 6))
                                {
                                    case 1: iItemID = 23; break;   //"¼¼ÀÌ¹ö"
                                    case 2: iItemID = 25; break;   //"½Ã¹ÌÅ¸"
                                    case 3: iItemID = 28; break;   //"ÆÈÄ¡¿Â"
                                    case 4: iItemID = 31; break;   //"¿¡½ºÅÍÅ©"
                                    case 5: iItemID = 34; break;   //"·¹ÀÌÇÇ¾î"
                                    case 6: iItemID = 760; break;  //"Çì¸Ó"
                                    default: break;
                                }
                                break;

                            case 10: // Ettin
                                switch (iDice(1, 5))
                                {
                                    case 1: iItemID = 46; break;   //"Å¬·¹ÀÌ¸ð¾î"
                                    case 2: iItemID = 31; break;   //"¿¡½ºÅÍÅ©"
                                    case 3: iItemID = 34; break;   //"·¹ÀÌÇÇ¾î"
                                    case 4: iItemID = 760; break;
                                    case 5: iItemID = 761; break;
                                    default: break;
                                }
                                break;


                        }
                    }
                    else
                    {

                        switch (iGenLevel)
                        {
                            case 1:	break;
                            case 2:
                            case 3:	if (iDice(1, 2) == 1) iItemID = 258; break;
                            case 4:
                            case 5:
                            case 6: iItemID = 257; break;
                            case 7:
                            case 8:	iItemID = 256; break;
                        }
                    }
                }
                else
                {

                    switch (iDice(1, iGenLevel))
                    {
                        case 1:
                        case 2:
                            switch (iDice(1, 2))
                            {
                                case 1: iItemID = 79; break;
                                case 2: iItemID = 81; break;
                            }
                            break;

                        case 3:
                            iItemID = 81;  break;
                            break;

                        case 4:
                            switch (iDice(1, 5))
                            {
                                case 1: iItemID = 454; break;
                                case 2: iItemID = 472; break;
                                case 3: iItemID = 461; break;
                                case 4: iItemID = 482; break;
                                case 5: iItemID = 83;  break;
                            }
                            break;

                        case 5:
                            switch (iDice(1, 3))
                            {
                                case 1: iItemID = 455; break;
                                case 2: iItemID = 475; break;
                                case 3: iItemID = 84;  break;
                            }
                            break;

                        case 6:
                            switch (iDice(1, 3))
                            {
                                case 1:
                                    switch (iDice(1, 2))
                                    {
                                        case 1: iItemID = 456; break;
                                        case 2: iItemID = 476; break;
                                    }
                                    break;
                                case 2:
                                    switch (iDice(1, 2))
                                    {
                                        case 1: iItemID = 458; break;
                                        case 2: iItemID = 478; break;
                                    }
                                    break;
                                case 3: iItemID = 85; break;
                            }
                            break;

                        case 7:
                            switch (iDice(1, 6))
                            {
                                case 1:
                                    switch (iDice(1, 2))
                                    {
                                        case 1: iItemID = 457; break;
                                        case 2: iItemID = 477; break;
                                    }
                                    break;
                                case 2:
                                    switch (iDice(1, 2))
                                    {
                                        case 1: iItemID = 458; break;
                                        case 2: iItemID = 478; break;
                                    }
                                    break;
                                case 3: iItemID = 86; break;
                                case 4: iItemID = 87; break;
                                case 5:
                                    switch (iDice(1, 2))
                                    {
                                        case 1: iItemID = 600; break;
                                        case 2: iItemID = 602; break;
                                    }
                                    break;
                                case 6: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 601; break;
                                    case 2: iItemID = 603; break;
                                }
                                      break;
                            }
                            break;

                        case 8:	iItemID = 402; break;
                    }
                }

                //2003-02-10 Á¦´ë·ÎµÈ°ªÀÌ ¾Æ´Ï¸é ¸®ÅÏ...
                if (0 == iItemID)
                    return;


                pItem = new CItem;

                if (_bInitItemAttr(pItem, iItemID) == false)
                {
                    delete pItem;
                    return;
                }


                if (bCheckInItemEventList(iItemID, iNpcH) == true)
                {
                    delete pItem;
                    return;
                }




                if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK)
                {


                    iResult = iDice(1, 10000);
                    if ((iResult >= 1) && (iResult <= 299))
                    {
                        dwType = 6;
                        cColor = 2;
                    }
                    else if ((iResult >= 300) && (iResult <= 999))
                    {
                        dwType = 8;
                        cColor = 3;
                    }
                    else if ((iResult >= 1000) && (iResult <= 2499))
                    {
                        dwType = 1;
                        cColor = 5;
                    }
                    else if ((iResult >= 2500) && (iResult <= 4499))
                    {
                        dwType = 5;
                        cColor = 1;
                    }
                    else if ((iResult >= 4500) && (iResult <= 6499))
                    {
                        dwType = 3;
                        cColor = 7;
                    }
                    else if ((iResult >= 6500) && (iResult <= 8099))
                    {
                        dwType = 2;
                        cColor = 4;
                    }
                    else if ((iResult >= 8100) && (iResult <= 9699))
                    {
                        dwType = 7;
                        cColor = 6;
                    }
                    else if ((iResult >= 9700) && (iResult <= 10000))
                    {
                        dwType = 9;
                        cColor = 8;
                    }


                    pItem->m_cItemColor = cColor;





                    iResult = iDice(1, 30000);
                    if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // 10000/29348 = 34%
                    else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // 6600/29348 = 22.4%
                    else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // 4356/29348 = 14.8%
                    else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // 2874/29348 = 9.7%
                    else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // 1897/29348 = 6.4%
                    else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // 1252/29348 = 4.2%
                    else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // 826/29348 = 2.8%
                    else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // 545/29348 = 1.85%
                    else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // 360/29348 = 1.2%
                    else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // 237/29348 = 0.8%
                    else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // 156/29348 = 0.5%
                    else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // 103/29348 = 0.3%
                    else if ((iResult >= 29970) && (iResult <= 30000))  dwValue = 13; // 68/29348 = 0.1%
                    else dwValue = 1; // v2.03 906


                    switch (dwType)
                    {
                        case 1:
                            if (dwValue <= 5) dwValue = 5;
                            break;
                        case 2:
                            if (dwValue <= 4) dwValue = 4;
                            break;
                        case 6:
                            if (dwValue <= 4) dwValue = 4;
                            break;
                        case 8:
                            if (dwValue <= 2) dwValue = 2;
                            break;
                    }

                    if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;


                    pItem->m_dwAttribute = 0;
                    dwType = dwType << 20;
                    dwValue = dwValue << 16;
                    pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;


                    if (iDice(1, 10000) >= 6000)
                    {


                        //Ãß°¡ µ¶¼ºÀúÇ×(1), Ãß°¡ ¸íÁß°ª(2), Ãß°¡ ¹æ¾î°ª(3), HP È¸º¹·® Ãß°¡(4), SP È¸º¹·® Ãß°¡(5)
                        //MP È¸º¹·® Ãß°¡(6), Ãß°¡ ¸¶¹ýÀúÇ×(7), ¹°¸® ´ë¹ÌÁö Èí¼ö(8), ¸¶¹ý ´ë¹ÌÁö Èí¼ö(9)
                        //¿¬Å¸ ´ë¹ÌÁö Ãß°¡(10), ´õ ¸¹Àº °æÇèÄ¡(11), ´õ¸¹Àº Gold(12)


                        iResult = iDice(1, 10000);
                        if ((iResult >= 1) && (iResult <= 4999))          dwType = 2;
                        else if ((iResult >= 5000) && (iResult <= 8499))  dwType = 10;
                        else if ((iResult >= 8500) && (iResult <= 9499))  dwType = 12;
                        else if ((iResult >= 9500) && (iResult <= 10000)) dwType = 11;


                        iResult = iDice(1, 30000);
                        if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // 10000/29348 = 34%
                        else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // 6600/29348 = 22.4%
                        else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // 4356/29348 = 14.8%
                        else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // 2874/29348 = 9.7%
                        else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // 1897/29348 = 6.4%
                        else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // 1252/29348 = 4.2%
                        else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // 826/29348 = 2.8%
                        else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // 545/29348 = 1.85%
                        else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // 360/29348 = 1.2%
                        else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // 237/29348 = 0.8%
                        else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // 156/29348 = 0.5%
                        else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // 103/29348 = 0.3%
                        else if ((iResult >= 29970) && (iResult <= 30000))  dwValue = 13; // 68/29348 = 0.1%
                        else dwValue = 1; // v2.03 906


                        switch (dwType)
                        {
                            case 2:
                                if (dwValue <= 3) dwValue = 3;
                                break;
                            case 10:
                                if (dwValue > 7) dwValue = 7;
                                break;
                            case 11:
                                dwValue = 2;
                                break;
                            case 12:
                                dwValue = 5;
                                break;
                        }

                        if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;


                        dwType = dwType << 12;
                        dwValue = dwValue << 8;

                        pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
                    }
                }
                else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE)
                {



                    iResult = iDice(1, 10000);
                    if ((iResult >= 1) && (iResult <= 5999))          dwType = 8;
                    else if ((iResult >= 6000) && (iResult <= 8999))  dwType = 6;
                    else if ((iResult >= 9000) && (iResult <= 9554))  dwType = 11; //dwType = 11;
                    else if ((iResult >= 9555) && (iResult <= 10000)) dwType = 12; //dwType = 12;


                    iResult = iDice(1, 30000);
                    if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // 10000/29348 = 34%
                    else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // 6600/29348 = 22.4%
                    else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // 4356/29348 = 14.8%
                    else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // 2874/29348 = 9.7%
                    else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // 1897/29348 = 6.4%
                    else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // 1252/29348 = 4.2%
                    else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // 826/29348 = 2.8%
                    else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // 545/29348 = 1.85%
                    else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // 360/29348 = 1.2%
                    else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // 237/29348 = 0.8%
                    else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // 156/29348 = 0.5%
                    else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // 103/29348 = 0.3%
                    else if ((iResult >= 29970) && (iResult <= 30000))  dwValue = 13; // 68/29348 = 0.1%
                    else dwValue = 1; // v2.03 906


                    switch (dwType)
                    {
                        case 6:
                            if (dwValue <= 4) dwValue = 4;
                            break;
                        case 8:
                            if (dwValue <= 2) dwValue = 2;
                            break;

                        case 11:
                        case 12:
                            // v2.04
                            dwValue = (dwValue + 1) / 2;
                            if (dwValue < 1) dwValue = 1;
                            if ((iGenLevel <= 3) && (dwValue > 2)) dwValue = 2;
                            break;
                    }

                    if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;


                    pItem->m_dwAttribute = 0;
                    dwType = dwType << 20;
                    dwValue = dwValue << 16;
                    pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;


                    if (iDice(1, 10000) >= 6000)
                    {


                        //Ãß°¡ µ¶¼ºÀúÇ×(1), Ãß°¡ ¸íÁß°ª(2), Ãß°¡ ¹æ¾î°ª(3), HP È¸º¹·® Ãß°¡(4), SP È¸º¹·® Ãß°¡(5)
                        //MP È¸º¹·® Ãß°¡(6), Ãß°¡ ¸¶¹ýÀúÇ×(7), ¹°¸® ´ë¹ÌÁö Èí¼ö(8), ¸¶¹ý ´ë¹ÌÁö Èí¼ö(9)
                        //¿¬Å¸ ´ë¹ÌÁö Ãß°¡(10), ´õ ¸¹Àº °æÇèÄ¡(11), ´õ¸¹Àº Gold(12)




                        iResult = iDice(1, 10000);
                        if ((iResult >= 1) && (iResult <= 999))           dwType = 3;
                        else if ((iResult >= 1000) && (iResult <= 3999))  dwType = 1;
                        else if ((iResult >= 4000) && (iResult <= 5499))  dwType = 5;
                        else if ((iResult >= 5500) && (iResult <= 6499))  dwType = 4;
                        else if ((iResult >= 6500) && (iResult <= 7499))  dwType = 6;
                        else if ((iResult >= 7500) && (iResult <= 9399))  dwType = 7;
                        else if ((iResult >= 9400) && (iResult <= 9799))  dwType = 8;
                        else if ((iResult >= 9800) && (iResult <= 10000)) dwType = 9;


                        iResult = iDice(1, 30000);
                        if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // 10000/29348 = 34%
                        else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // 6600/29348 = 22.4%
                        else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // 4356/29348 = 14.8%
                        else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // 2874/29348 = 9.7%
                        else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // 1897/29348 = 6.4%
                        else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // 1252/29348 = 4.2%
                        else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // 826/29348 = 2.8%
                        else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // 545/29348 = 1.85%
                        else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // 360/29348 = 1.2%
                        else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // 237/29348 = 0.8%
                        else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // 156/29348 = 0.5%
                        else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // 103/29348 = 0.3%
                        else if ((iResult >= 29970) && (iResult <= 30000))  dwValue = 13; // 68/29348 = 0.1%
                        else dwValue = 1; // v2.03 906


                        switch (dwType)
                        {
                            case 1:
                            case 3:
                            case 7:
                            case 8:
                            case 9:
                                if (dwValue <= 3) dwValue = 3;
                                break;
                        }

                        if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;


                        dwType = dwType << 12;
                        dwValue = dwValue << 8;
                        pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
                    }
                }
                else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE)
                {


                    dwType = 10;
                    cColor = 5;


                    pItem->m_cItemColor = cColor;

                    iResult = iDice(1, 30000);
                    if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // 10000/29348 = 34%
                    else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // 6600/29348 = 22.4%
                    else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // 4356/29348 = 14.8%
                    else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // 2874/29348 = 9.7%
                    else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // 1897/29348 = 6.4%
                    else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // 1252/29348 = 4.2%
                    else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // 826/29348 = 2.8%
                    else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // 545/29348 = 1.85%
                    else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // 360/29348 = 1.2%
                    else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // 237/29348 = 0.8%
                    else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // 156/29348 = 0.5%
                    else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // 103/29348 = 0.3%
                    else if ((iResult >= 29970) && (iResult <= 30000))  dwValue = 13; // 68/29348 = 0.1%
                    else dwValue = 1; // v2.03 906


                    if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;


                    pItem->m_dwAttribute = 0;
                    dwType = dwType << 20;
                    dwValue = dwValue << 16;
                    pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;


                    if (iDice(1, 10000) >= 6000)
                    {


                        //Ãß°¡ µ¶¼ºÀúÇ×(1), Ãß°¡ ¸íÁß°ª(2), Ãß°¡ ¹æ¾î°ª(3), HP È¸º¹·® Ãß°¡(4), SP È¸º¹·® Ãß°¡(5)
                        //MP È¸º¹·® Ãß°¡(6), Ãß°¡ ¸¶¹ýÀúÇ×(7), ¹°¸® ´ë¹ÌÁö Èí¼ö(8), ¸¶¹ý ´ë¹ÌÁö Èí¼ö(9)
                        //¿¬Å¸ ´ë¹ÌÁö Ãß°¡(10), ´õ ¸¹Àº °æÇèÄ¡(11), ´õ¸¹Àº Gold(12)


                        iResult = iDice(1, 10000);
                        if ((iResult >= 1) && (iResult <= 4999))          dwType = 2;
                        else if ((iResult >= 5000) && (iResult <= 8499))  dwType = 10;
                        else if ((iResult >= 8500) && (iResult <= 9499))  dwType = 12;
                        else if ((iResult >= 9500) && (iResult <= 10000)) dwType = 11;


                        iResult = iDice(1, 30000);
                        if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // 10000/29348 = 34%
                        else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // 6600/29348 = 22.4%
                        else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // 4356/29348 = 14.8%
                        else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // 2874/29348 = 9.7%
                        else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // 1897/29348 = 6.4%
                        else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // 1252/29348 = 4.2%
                        else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // 826/29348 = 2.8%
                        else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // 545/29348 = 1.85%
                        else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // 360/29348 = 1.2%
                        else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // 237/29348 = 0.8%
                        else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // 156/29348 = 0.5%
                        else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // 103/29348 = 0.3%
                        else if ((iResult >= 29970) && (iResult <= 30000))  dwValue = 13; // 68/29348 = 0.1%
                        else dwValue = 1; // v2.03 906


                        if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;


                        switch (dwType)
                        {
                            case 2:
                                if (dwValue <= 3) dwValue = 3;
                                break;
                            case 10:
                                if (dwValue > 7) dwValue = 7;
                                break;
                            case 11:
                                dwValue = 2;
                                break;
                            case 12:
                                dwValue = 5;
                                break;
                        }


                        dwType = dwType << 12;
                        dwValue = dwValue << 8;
                        pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
                    }
                }


                _AdjustRareItemValue(pItem);
            }
            }


        pItem->m_sTouchEffectType = DEF_ITET_ID;
        pItem->m_sTouchEffectValue1 = iDice(1, 100000);
        pItem->m_sTouchEffectValue2 = iDice(1, 100000);
#ifdef DEF_LOGTIME
        pItem->m_sTouchEffectValue3 = timeGetTime();
#else 
        GetLocalTime(&SysTime);
        memset(cTemp, 0, sizeof(cTemp));
        //		wsprintf(cTemp, "%d%02d%02d",  (short)SysTime.wMonth, (short)SysTime.wDay,(short) SysTime.wHour);
        wsprintf(cTemp, "%d%02d%", (short)SysTime.wMonth, (short)SysTime.wDay);

        pItem->m_sTouchEffectValue3 = atoi(cTemp);
#endif


        /*
        if( pItem->m_sIDnum >= 651 &&  pItem->m_sIDnum <= 655 )
        {

        if(  SysTime.wYear == 2002 && SysTime.wMonth == 11 && (SysTime.wDay >= 1 && SysTime.wDay <=7 ) )
        pItem->m_sItemSpecEffectValue2 = 113;
        }
        */


        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX,
            m_pNpcList[iNpcH]->m_sY,
            pItem);


        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
            m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color

        // ·Î±× ³²±ä´Ù.
        _bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, m_pNpcList[iNpcH]->m_cNpcName, pItem);
        }
    }

// 2002-10-23 Item Event ±â´É Ãß°¡
/*
; mapdata¿¡ µé¾î°¥ ³»¿ë
;item-event = index	item_name	amount	TotalNumber	month	day		type	mob_list[Max:5]
;	type = 0 : ÇØ´ç ¾ÆÀÌÅÛÀÌ ÀÏ¹Ý ¾ÆÀÌÅÛ »ý¼º°úÁ¤¿¡¼­ »ý¼ºµÇÁö ¾Ê´Â´Ù.
;	       1 : Item Event¿¡ ÀÇÇØ¼­µµ »ý¼ºµÇ°í ÀÏ¹Ý ¾ÆÀÌÅÛ »ý¼º°úÁ¤À» ÅëÇØ¼­µµ »ý¼ºµÈ´Ù.

item-event = 	1	Àû»ö¼Ò¿ø±¸	1		10			11		1		0		Cannibal-Plant Ettin EOL
item-event = 	2	³ì»ö¼Ò¿ø±¸	1		10			11		1		0		Giant-Frog Scorpion EOL
*/
bool CGame::NpcDeadItemGeneratorWithItemEvent(int iNpcH, short sAttackerH, char cAttackerType)
{
    CItem * pItem;
    char  cColor{}, cItemName[21]{}, cTemp[20]{};
    bool  bIsGold{};
    int   i, j, iGenLevel = 0, iResult = 0, iT1 = 0, iT2 = 0, iT3 = 0, iItemID = 0;
    int	iNumMob;
    double dTmp1{}, dTmp2{}, dTmp3{};
    SYSTEMTIME SysTime;


    if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalItemEvents != 0)
    {
        GetLocalTime(&SysTime);
        for (i = 0; i < m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalItemEvents; i++)
            if (// ³¯Â¥ °°°í,
                (SysTime.wMonth == m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iMonth) &&
                (SysTime.wDay == m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iDay) &&

                (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iCurNum < m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iTotalNum)
                )
            {

                iNumMob = m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iNumMob;


                for (j = 0; j < iNumMob; j++)
                {
                    if (strcmp(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].cMob[j], m_pNpcList[iNpcH]->m_cNpcName) == 0)
                        break;
                }


                if (j == iNumMob)
                    continue;

                if (SysTime.wHour < 12)
                {
                    if (iDice(1, 9000) != 6433)
                        continue;
                }
                else if (SysTime.wHour < 18)
                {
                    if (iDice(1, 3000) != 1433)
                        continue;
                }




                iT1 = 1440 / m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iTotalNum;
                iT2 = iT1 * m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iCurNum;
                iT3 = (SysTime.wHour * 60) + SysTime.wMinute;


                if ((iT1 / 2) > iT3)
                    continue;

                if (((iT2 <= iT3) && (iT3 <= iT2 + iT1)) || (iT2 + iT1 < iT3))
                {





                    int	iT4 = iT1 / 4;
                    if ((iT4 > 10) && iDice(1, iT4) != 5)
                        continue;

                    pItem = new CItem;
                    if (_bInitItemAttr(pItem, m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].cItemName) == false)
                    {
                        delete pItem;
                        pItem = 0;
                    }
                    else
                    {

                        pItem->m_sTouchEffectType = DEF_ITET_ID;
                        pItem->m_sTouchEffectValue1 = iDice(1, 100000);
                        pItem->m_sTouchEffectValue2 = iDice(1, 100000);

#ifdef DEF_LOGTIME
                        pItem->m_sTouchEffectValue3 = timeGetTime();
#else 

                        memset(cTemp, 0, sizeof(cTemp));
                        //					wsprintf(cTemp, "%d%02d%02d",  (short)SysTime.wMonth, (short)SysTime.wDay,(short) SysTime.wHour);
                        wsprintf(cTemp, "%d%02d", (short)SysTime.wMonth, (short)SysTime.wDay);
                        pItem->m_sTouchEffectValue3 = atoi(cTemp);
#endif


                        /*
                        .			if( pItem->m_sIDnum >= 651 &&  pItem->m_sIDnum <= 655 )
                        {

                        if(  SysTime.wYear == 2002 && SysTime.wMonth == 11 && (SysTime.wDay >= 1 && SysTime.wDay <=7 ) )
                        pItem->m_sItemSpecEffectValue2 = 113;
                        }
                        */


                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX,
                            m_pNpcList[iNpcH]->m_sY,
                            pItem);


                        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
                            m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
                            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color

                        // ·Î±× ³²±ä´Ù.
                        _bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, m_pNpcList[iNpcH]->m_cNpcName, pItem);
                        wsprintf(G_cTxt, "Event Item (%s)", pItem->m_cName);
                        log->info(G_cTxt);

                        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stItemEventList[i].iCurNum++;

                        return true;
                    }

                    // return false;
                }
            }
    }

    return false;
} // NpcDeadItemGeneratorWithItemEvent
