//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

void CGame::NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType)
{
    CItem * pItem;
    char  cColor, cItemName[30];
    BOOL  bIsGold;
    int   iGenLevel, iResult, iItemID{};
    DWORD dwType, dwValue;
    double dTmp1, dTmp2, dTmp3;

    if (m_pNpcList[iNpcH] == NULL) return;
    if ((cAttackerType != DEF_OWNERTYPE_PLAYER) || (m_pNpcList[iNpcH]->m_bIsSummoned == TRUE)) return;
    if (m_pNpcList[iNpcH]->m_bIsUnsummoned == TRUE) return;

    ZeroMemory(cItemName, sizeof(cItemName));
    bIsGold = FALSE;

    switch (m_pNpcList[iNpcH]->m_sType)
    {
        // NPC not dropping Gold
        case 21: // Guard
        case 34: // Dummy
        case 64: // Crop
            return;
    }

    // 6500 default; the lower the greater the Weapon/Armor/Wand Drop
    if (iDice(1, 10000) >= m_iPrimaryDropRate)
    {
        // 35% Drop 60% of that is gold
        // 35% Chance of drop (35/100)
        if (iDice(1, 10000) <= 6000)
        {
            iItemID = 90; // Gold: (35/100) * (60/100) = 21%
            // If a non-existing itemID is given create no item
            pItem = new CItem;
            if (_bInitItemAttr(pItem, iItemID) == FALSE)
            {
                delete pItem;
                return;
            }

            pItem->m_dwCount = (DWORD)(iDice(1, (m_pNpcList[iNpcH]->m_iGoldDiceMax - m_pNpcList[iNpcH]->m_iGoldDiceMin)) + m_pNpcList[iNpcH]->m_iGoldDiceMin);

            if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_iAddGold != NULL))
            {
                dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddGold;
                dTmp2 = (double)pItem->m_dwCount;
                dTmp3 = (dTmp1 / 100.0f) * dTmp2;
                pItem->m_dwCount += (int)dTmp3;
            }
        }
        else
        {
            // 9000 default; the lower the greater the Weapon/Armor/Wand Drop
            // 35% Drop 40% of that is an Item 
            dTmp1 = m_pClientList[sAttackerH]->m_iRating * m_cRepDropModifier;
            if (dTmp1 > 1000) dTmp1 = 1000;
            if (dTmp1 < -1000) dTmp1 = -1000;
            dTmp2 = (m_iSecondaryDropRate - (dTmp1));
            if (iDice(1, 10000) <= dTmp2)
            {
                // 40% Drop 90% of that is a standard drop
                // Standard Drop Calculation: (35/100) * (40/100) * (90/100) = 12.6%
                iResult = iDice(1, 12000);
                if ((iResult >= 1) && (iResult <= 3000))          dwValue = 1;
                else if ((iResult >= 3001) && (iResult <= 4000))  dwValue = 2;
                else if ((iResult >= 4001) && (iResult <= 5500))  dwValue = 3;
                else if ((iResult >= 5501) && (iResult <= 7000))  dwValue = 4;
                else if ((iResult >= 7001) && (iResult <= 8500))  dwValue = 5;
                else if ((iResult >= 8501) && (iResult <= 9200))  dwValue = 6;
                else if ((iResult >= 9201) && (iResult <= 9800))  dwValue = 7;
                else if ((iResult >= 9801) && (iResult <= 10000)) dwValue = 8;
                else if ((iResult >= 10001) && (iResult <= 12000)) dwValue = 9;

                switch (dwValue)
                {
                    case 1: iItemID = 95; break; // Green Potion
                    case 2: iItemID = 91; break; // Red Potion
                    case 3: iItemID = 93; break; // Blue Potion
                    case 4: iItemID = 96; break; // Big Green Potion
                    case 5: iItemID = 92; break; // Big Red Potion
                    case 6: iItemID = 94; break; // Big Blue Potion
                    case 7: switch (iDice(1, 6))
                    {
                        case 1: iItemID = 390; break; // Power Green Potion
                        case 2: iItemID = 95;  break; // Green Potion
                        case 3: iItemID = 780; break; // RedCandy
                        case 4: iItemID = 781; break; // BlueCandy
                        case 5: iItemID = 782; break; // GreenCandy
                        case 6: iItemID = 970; break; // CritCandy
                    }
                          break;
                    case 8: switch (iDice(1, 10))
                    {
                        case 1: iItemID = 391; break; // Super Power Green Potion
                        case 2: iItemID = 650; break; // Zemstone of Sacrifice
                        case 3: iItemID = 656; break; // Xelima Stone
                        case 4: iItemID = 657; break; // Merien Stone
                        case 5: iItemID = 95;  break; // Green Potion
                        case 6: iItemID = 868; break; // AcientTablet(LU)
                        case 7: iItemID = 869; break; // AcientTablet(LD)
                        case 8: iItemID = 870; break; // AcientTablet(RU)
                        case 9: iItemID = 871; break; // AcientTablet(RD)
                        case 10: switch (iDice(1, 5))
                        {
                            case 1: iItemID = 651; break; // GreenBall
                            case 2: iItemID = 652; break; // RedBall
                            case 3: iItemID = 653; break; // YellowBall
                            case 4: iItemID = 654; break; // BlueBall
                            case 5: iItemID = 655; break; // PearlBall
                        }
                               break;
                    }
                          break;
                    case 9:
                        SYSTEMTIME SysTime;
                        GetLocalTime(&SysTime);
                        if (((short)SysTime.wMonth == 12) && (m_pNpcList[iNpcH]->m_sType == 61 || 55))
                        {
                            switch (iDice(1, 3))
                            {
                                case 1: iItemID = 780; break; // Red Candy
                                case 2: iItemID = 781; break; // Blue Candy
                                case 3: iItemID = 782; break; // Green Candy
                            }
                        }
                        break;
                }
                // If a non-existing item is created then delete the item
                pItem = new CItem;
                if (_bInitItemAttr(pItem, iItemID) == FALSE)
                {
                    delete pItem;
                    return;
                }
            }
            else
            {
                // Valuable Drop Calculation: (35/100) * (40/100) * (10/100) = 1.4%
                // Define iGenLevel using Npc.cfg#
                switch (m_pNpcList[iNpcH]->m_sType)
                {

                    case 10: // Slime
                    case 16: // Giant-Ant
                    case 22: // Amphis
                    case 55: // Rabbit
                    case 56: //	Cat
                        iGenLevel = 1;
                        break;

                    case 11: // Skeleton
                    case 14: // Orc, Orc-Mage
                    case 17: // Scorpion
                    case 18: // Zombie
                        iGenLevel = 2;
                        break;

                    case 12: // Stone-Golem
                    case 23: // Clay-Golem
                        iGenLevel = 3;
                        break;

                    case 27: // Hellbound
                    case 61: // Rudolph
                        iGenLevel = 4;
                        break;

                    case 72: // Claw-Turtle
                    case 76: // Giant-Plant
                    case 74: // Giant-Crayfish
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
                    case 78: // Minotaurus
                        iGenLevel = 6;
                        break;

                    case 70: // Balrogs
                    case 71: // Centaurus
                    case 30: // Liche
                    case 63: // Frost
                    case 79: // Nizie
                        iGenLevel = 7;
                        break;

                    case 31: // Demon
                    case 32: // Unicorn
                    case 49: // Hellclaw
                    case 50: // Tigerworm
                    case 52: // Gagoyle
                        iGenLevel = 8;
                        break;

                    case 58: // MountainGiant
                        iGenLevel = 9;
                        break;

                    case 77: // MasterMage-Orc
                    case 59: // Ettin
                    case 75: // Lizards
                        iGenLevel = 10;
                        break;
                }

                if (iGenLevel == 0) return;

                // Weapon Drop: 
                // 1.4% chance Valuable Drop 60% that it is a Weapon
                if (iDice(1, 10000) <= 6000)
                {
                    if (iDice(1, 10000) <= 8000)
                    {
                        // 70% the Weapon is Melee
                        switch (iGenLevel)
                        {

                            case 1: // Slime, Giant-Ant, Amphis, Rabbit, Cat
                                switch (iDice(1, 3))
                                {
                                    case 1: iItemID = 1;  break; // Dagger
                                    case 2: iItemID = 8;  break; // ShortSword
                                    case 3: iItemID = 59; break; // LightAxe
                                }
                                break;

                            case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
                                switch (iDice(1, 6))
                                {
                                    case 1: iItemID = 12;  break; // MainGauche
                                    case 2: iItemID = 15;  break; // Gradius
                                    case 3: iItemID = 65;  break; // SexonAxe
                                    case 4: iItemID = 62;  break; // Tomahoc
                                    case 5: iItemID = 23;  break; // Sabre
                                    case 6: iItemID = 31;  break; // Esterk
                                }
                                break;

                            case 3: // Stone-Golem, Clay-Golem
                                switch (iDice(1, 4))
                                {
                                    case 1: iItemID = 50;  break; // GreatSword
                                    case 2: iItemID = 68;  break; // DoubleAxe
                                    case 3: iItemID = 23;  break; // Sabre
                                    case 4: iItemID = 31;  break; // Esterk
                                }
                                break;

                            case 4: // Hellbound, Rudolph
                                switch (iDice(1, 5))
                                {
                                    case 1: iItemID = 25;  break; // Scimitar
                                    case 2: iItemID = 28;  break; // Falchion
                                    case 3: iItemID = 31;  break; // Esterk
                                    case 4: iItemID = 34;  break; // Rapier
                                    case 5: iItemID = 71;  break; // WarAxe
                                }
                                break;

                            case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
                                // Claw-Turtle, Giant-Plant, Giant-Crayfish
                                switch (iDice(1, 4))
                                {
                                    case 1: iItemID = 31;  break; // Esterk
                                    case 2: iItemID = 34;  break; // Rapier
                                    case 3: iItemID = 72;  break; // WarAxe+1
                                    case 4: iItemID = 844; break; // BlackShadowSword
                                }
                                break;

                            case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem, Minotaurus
                                switch (iDice(1, 7))
                                {
                                    case 1: iItemID = 47;  break; // Claymore+1
                                    case 2: iItemID = 51;  break; // GreatSword+
                                    case 3: iItemID = 55;  break; // Flameberge+1
                                    case 4: iItemID = 34;  break; // GiantSword
                                    case 5: iItemID = 74;  break; // GoldenAxe
                                    case 6: iItemID = 848; break; // HolyBlade
                                    case 7: iItemID = 924; break; // MageSword
                                }
                                break;

                            case 7: // Liche, Frost, Balrogs, Centaurus, Nizie
                                switch (iDice(1, 6))
                                {
                                    case 1: iItemID = 47;  break; // Claymore+1
                                    case 2: iItemID = 50;  break; // GreatSword
                                    case 3: iItemID = 54;  break; // Flameberge+1
                                    case 4: iItemID = 74;  break; // GoldenAxe
                                    case 5: iItemID = 850; break; // KlonessAxe
                                    case 6: iItemID = 923; break; // BMageSword
                                }
                                break;

                            case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
                                switch (iDice(1, 5))
                                {
                                    case 1: iItemID = 50;  break; // GreatSword
                                    case 2: iItemID = 560; break; // BattleAxe
                                    case 3: iItemID = 615; break; // GiantSword
                                    case 4: iItemID = 56;  break; // Flameberge+2
                                    case 5: iItemID = 846; break; // The_Devastator
                                }
                                break;

                            case 9: // MountainGiant
                                switch (iDice(1, 5))
                                {
                                    case 1: iItemID = 55;  break; // Flameberge+1
                                    case 2: iItemID = 615; break; // GiantSword
                                    case 3: iItemID = 761; break; // BattleHammer
                                    case 4: iItemID = 762; break; // GiantBattleHammer
                                    case 5: iItemID = 857; break; // I.M.C Manual
                                }
                                break;

                            case 10: // Ettin, MasterMage-Orc, Giant-Lizard
                                switch (iDice(1, 9))
                                {
                                    case 1: iItemID = 50;  break; // GreatSword
                                    case 2: iItemID = 51;  break; // GreatSword+1
                                    case 3: iItemID = 55;  break; // Flameberge+1
                                    case 4: iItemID = 56;  break; // Flameberge+2
                                    case 5: iItemID = 615; break; // GiantSword
                                    case 6: iItemID = 761; break; // BattleHammer
                                    case 7: iItemID = 762; break; // GiantBattleHammer
                                    case 8: iItemID = 843; break; // BarbarianHammer
                                    case 9: iItemID = 853; break; // E.S.W Manual
                                }
                                break;

                        }
                    }
                    else
                    {
                        // 30% the weapon is a Wand
                        switch (iGenLevel)
                        {

                            case 2:
                            case 3:
                                iItemID = 258; break; // MagicWand(MS20)
                            case 4:
                            case 5:
                            case 6:
                                iItemID = 257; break; // MagicWand(MS20)
                            case 7:
                            case 8:
                                iItemID = 256; break; // MagicWand(MS20)
                            case 9:
                            case 10:
                                break;
                        }
                    }
                }
                else
                {
                    // 1.4% chance Valuable Drop 40% that drop is an Armor/Shield
                    switch (iGenLevel)
                    {

                        case 1: // Slime, Giant-Ant, Amphis, Rabbit, Cat
                        case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
                            switch (iDice(1, 2))
                            {
                                case 1: iItemID = 79;  break; // WoodShield
                                case 2: iItemID = 81;  break; // TargeShield
                            }
                            break;

                        case 3: // Stone-Golem, Clay-Golem
                            switch (iDice(1, 5))
                            {
                                case 1: iItemID = 85;  break; // LagiShield
                                case 2: iItemID = 454; break; // Hauberk(M)
                                case 3: iItemID = 472; break; // Hauberk(W)
                                case 4: iItemID = 461; break; // ChainHose(M)
                                case 5: iItemID = 482; break; // ChainHose(W)
                            }
                            break;

                        case 4: // Hellbound, Rudolph
                            switch (iDice(1, 5))
                            {
                                case 1: iItemID = 454; break; // Hauberk(M)
                                case 2: iItemID = 472; break; // Hauberk(W)
                                case 3: iItemID = 461; break; // ChainHose(M)
                                case 4: iItemID = 482; break; // ChainHose(W)
                                case 5: iItemID = 86;  break; // KnightShield
                            }
                            break;

                        case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
                            // Claw-Turtle, Giant-Plant, Giant-Crayfish
                            switch (iDice(1, 7))
                            {
                                case 1: iItemID = 455; break; // LeatherArmor(M)
                                case 2: iItemID = 475; break; // LeatherArmor(W)
                                case 3: iItemID = 87;  break; // TowerShield
                                case 4: iItemID = 454; break; // Hauberk(M)
                                case 5: iItemID = 472; break; // Hauberk(W)
                                case 6: iItemID = 461; break; // ChainHose(M)
                                case 7: iItemID = 482; break; // ChainHose(W)
                            }
                            break;

                        case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem, Minotaurus
                            switch (iDice(1, 6))
                            {
                                case 1: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 456; break; // ChainMail(M)
                                    case 2: iItemID = 476; break; // ChainMail(W)
                                }
                                      break;
                                case 2: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 458; break; // PlateMail(M)
                                    case 2: iItemID = 478; break; // PlateMail(W)
                                }
                                      break;
                                case 3: iItemID = 87; break; // TowerShield
                                case 4: switch (iDice(1, 8))
                                {
                                    case 1: iItemID = 750; break; // Horned-Helm(M)
                                    case 2: iItemID = 751; break; // Wings-Helm(M)
                                    case 3: iItemID = 754; break; // Horned-Helm(W)
                                    case 4: iItemID = 755; break; // Wings-Helm(W)
                                    case 5: iItemID = 752; break; // Wizard-Cap(M) 
                                    case 6: iItemID = 753; break; // Wizard-Hat(M)
                                    case 7: iItemID = 756; break; // Wizard-Cap(W) 
                                    case 8: iItemID = 757; break; // Wizard-Hat(W) 
                                }
                                      break;
                                case 5: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 454; break; // Hauberk(M)
                                    case 2: iItemID = 472; break; // Hauberk(W)
                                }
                                      break;
                                case 6: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 461; break; // ChainHose(M)
                                    case 2: iItemID = 482; break; // ChainHose(W)
                                }
                                      break;
                            }
                            break;


                        case 7: // Liche, Frost, Balrogs, Centaurus, Nizie
                            switch (iDice(1, 6))
                            {
                                case 1: switch (iDice(1, 6))
                                {
                                    case 1: iItemID = 457; break; // ScaleMail(M)
                                    case 2: iItemID = 477; break; // ScaleMail(W)
                                    case 3: iItemID = 454; break; // Hauberk(M)
                                    case 4: iItemID = 472; break; // Hauberk(W)
                                    case 5: iItemID = 461; break; // ChainHose(M)
                                    case 6: iItemID = 482; break; // ChainHose(W)
                                }
                                      break;
                                case 2: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 458; break; // PlateMail(M)
                                    case 2: iItemID = 478; break; // PlateMail(W)
                                }
                                      break;
                                case 3: iItemID = 86; break; // KnightShield
                                case 4: iItemID = 87; break; // TowerShield
                                case 5: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 600; break; // Helm(M)
                                    case 2: iItemID = 602; break; // Helm(M)
                                }
                                      break;
                                case 6: switch (iDice(1, 2))
                                {
                                    case 1: iItemID = 601; break; // Full-Helm(M)
                                    case 2: iItemID = 603; break; // Full-Helm(M)
                                }
                                      break;
                            }
                            break;

                        case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
                            switch (iDice(1, 4))
                            {
                                case 1: iItemID = 402; break; // Cape
                                case 2: iItemID = 451; break; // Boots
                                case 3: iItemID = 926; break; // ShieldOfFaith
                                case 4: iItemID = 927; break; // ShieldOfBrave
                            }
                            break;

                        case 9: // Mountain-Giant
                            switch (iDice(1, 2))
                            {
                                case 1: iItemID = 402; break; // Cape
                                case 2: iItemID = 451; break; // Boots
                            }
                            break;

                        case 10: //Ettin, MasterMage-Orc, Giant-Lizard
                            switch (iDice(1, 4))
                            {
                                case 1: iItemID = 457; break; // ScaleMail(M)
                                case 2: iItemID = 477; break; // ScaleMail(W)
                                case 4: iItemID = 600; break; // Helm(M)
                                case 5: iItemID = 602; break; // Helm(W)
                            }
                            break;
                    }
                }
                pItem = new CItem;
                if (_bInitItemAttr(pItem, iItemID) == FALSE)
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
                    else dwValue = 1;

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

                    pItem->m_dwAttribute = NULL;
                    dwType = dwType << 20;
                    dwValue = dwValue << 16;
                    pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

                    if (iDice(1, 10000) >= 6000)
                    {
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
                        else dwValue = 1;

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
                    else dwValue = 1;

                    if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

                    pItem->m_dwAttribute = NULL;
                    dwType = dwType << 20;
                    dwValue = dwValue << 16;
                    pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

                    if (iDice(1, 10000) >= 6000)
                    {
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
                        else dwValue = 1;

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
                    else dwValue = 1;

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
                            dwValue = (dwValue + 1) / 2;
                            if (dwValue < 1) dwValue = 1;
                            if ((iGenLevel <= 3) && (dwValue > 2)) dwValue = 2;
                            break;
                    }
                    if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

                    pItem->m_dwAttribute = NULL;
                    dwType = dwType << 20;
                    dwValue = dwValue << 16;
                    pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

                    if (iDice(1, 10000) >= 6000)
                    {
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
                        else dwValue = 1;

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

                _AdjustRareItemValue(pItem);
            }
        }

        pItem->m_sTouchEffectType = DEF_ITET_ID;
        pItem->m_sTouchEffectValue1 = iDice(1, 100000);
        pItem->m_sTouchEffectValue2 = iDice(1, 100000);
        SYSTEMTIME SysTime;
        char cTemp[256];
        GetLocalTime(&SysTime);
        ZeroMemory(cTemp, sizeof(cTemp));
        wsprintf(cTemp, "%d%2d", (short)SysTime.wMonth, (short)SysTime.wDay);
        pItem->m_sTouchEffectValue3 = atoi(cTemp);

        m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX,
            m_pNpcList[iNpcH]->m_sY,
            pItem);

        SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
            m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
            pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);

        _bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, NULL, pItem);
    }
}
