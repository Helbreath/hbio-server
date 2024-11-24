//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

bool CGame::save_player_data(std::shared_ptr<CClient> client)
{
    if (!client)
        return false;

    character_db character = build_character_data_for_save(client);

    std::shared_lock<std::shared_mutex> l(game_sql_mtx);
    pqxx::work txn{ *pq_game };

    try
    {
        update_db_character(txn, character);
        for (auto & skill : character.skills)
        {
            if (skill.id == 0)
                skill.id = create_db_skill(txn, skill);
            else
                update_db_skill(txn, skill);
        }
        for (auto & item : character.items)
        {
            if (item.id == 0)
                item.id = create_db_item(txn, item);
            else
                update_db_item(txn, item);
        }
        for (auto & item : character.bank_items)
        {
            if (item.id == 0)
                item.id = create_db_item(txn, item);
            else
                update_db_item(txn, item);
        }
        txn.commit();
    }
    catch (std::exception & e)
    {
        log->error("Error saving character data for character <{}> - <{}>", client->m_cCharName, e.what());
        return false;
    }
    return true;
}

character_db CGame::build_character_data_for_save(std::shared_ptr<CClient> client)
{
    character_db character{};

    character.id = client->id;
    character.account_id = client->account_id;
    character.name = client->m_cCharName;
    character.id1 = client->m_sCharIDnum1;
    character.id2 = client->m_sCharIDnum2;
    character.id3 = client->m_sCharIDnum3;
    character.level = client->m_iLevel;
    character.strength = client->m_iStr;
    character.vitality = client->m_iVit;
    character.dexterity = client->m_iDex;
    character.intelligence = client->m_iInt;
    character.magic = client->m_iMag;
    character.charisma = client->m_iCharisma;
    character.experience = client->m_iExp;
    character.gender = client->m_cSex;
    character.skin = client->m_cSkin;
    character.hairstyle = client->m_cHairStyle;
    character.haircolor = client->m_cHairColor;
    character.underwear = client->m_cUnderwear;
    character.apprcolor = client->m_iApprColor;
    character.appr1 = client->m_sAppr1;
    character.appr2 = client->m_sAppr2;
    character.appr3 = client->m_sAppr3;
    character.appr4 = client->m_sAppr4;
    character.nation = client->m_cLocation;
    character.maploc = client->m_cMapName;
    character.locx = client->m_sX;
    character.locy = client->m_sY;
    character.profile = client->m_cProfile;
    character.adminlevel = client->m_iAdminUserLevel;
    character.contribution = client->m_iContribution;
    character.leftspectime = client->m_iSpecialAbilityTime;
    character.lockmapname = client->m_cLockedMapName;
    character.lockmaptime = client->m_iLockedMapTime;
    character.lastsavedate = duration_cast<seconds>(now().time_since_epoch()).count();
    character.blockdate = 0;
    character.guild_id = client->m_iGuildGUID;
    character.fightnum = client->m_iFightzoneNumber;
    character.fightdate = 0;
    character.fightticket = client->m_iFightZoneTicketNumber;
    character.questnum = client->m_iQuest;
    character.questid = client->m_iQuestID;
    character.questcount = client->m_iCurQuestCount;
    character.questrewardtype = client->m_iQuestRewardType;
    character.questrewardamount = client->m_iQuestRewardAmount;
    character.questcompleted = client->m_bIsQuestCompleted;
    character.eventid = client->m_iSpecialEventID;
    character.warcon = client->m_iWarContribution;
    character.crusadejob = client->m_iCrusadeDuty;
    character.crusadeid = client->m_dwCrusadeGUID;
    character.crusadeconstructpoint = client->m_iConstructionPoint;
    character.reputation = client->m_iRating;
    character.hp = client->m_iHP;
    character.mp = client->m_iMP;
    character.sp = client->m_iSP;
    character.ek = client->m_iEnemyKillCount;
    character.pk = client->m_iPKCount;
    character.rewardgold = client->m_iRewardGold;
    character.downskillid = client->m_iDownSkillIndex;
    character.hunger = client->m_iHungerStatus;
    character.leftsac = client->m_iSuperAttackLeft;
    character.leftshutuptime = client->m_iTimeLeft_ShutUp;
    character.leftreptime = client->m_iTimeLeft_Rating;
    character.leftforcerecalltime = client->m_iTimeLeft_ForceRecall;
    character.leftfirmstaminatime = client->m_iTimeLeft_FirmStaminar;
    character.leftdeadpenaltytime = client->m_iDeadPenaltyTime;
    std::string mastery;
    for (char m : client->m_cMagicMastery)
    {
        mastery += m == 0 ? '0' : '1';
    }
    character.magicmastery = mastery;
    character.party_id = client->m_iPartyID;
    character.itemupgradeleft = client->m_iGizonItemUpgradeLeft;
    character.totalek = client->m_iEnemyKillCount;
    character.totalpk = client->m_iPKCount;
    //character.mmr = client->;
    //character.altmmr = client->;
    //character.head_appr = client->;
    //character.body_appr = client->;
    //character.arm_appr = client->;
    //character.leg_appr = client->;
    //character.gold = client->;
    character.luck = client->m_iLuck;
    character.world_name = world_name;

    for (int i = 0; i < DEF_MAXSKILLTYPE; ++i)
    {
        skill_db _skill{};
        _skill.id = client->m_cSkillId[i];
        _skill.character_id = character.id;
        _skill.skill_id = i;
        _skill.skill_level = client->m_cSkillMastery[i];
        _skill.skill_exp = client->m_iSkillSSN[i];

        character.skills.push_back(_skill);
    }
    for (int i = 0; i < DEF_MAXITEMS; ++i)
    {
        CItem * item = client->m_pItemList[i];
        if (item == nullptr)
            continue;
        item_db _item{};
        _item.id = item->id;
        _item.item_id = item->m_sIDnum;
        _item.char_id = character.id;
        _item.name = item->m_cName;
        _item.count = item->m_dwCount;
        _item.color = item->m_cItemColor;
        _item.type = item->m_sTouchEffectType;
        _item.id1 = item->m_sTouchEffectValue1;
        _item.id2 = item->m_sTouchEffectValue2;
        _item.id3 = item->m_sTouchEffectValue3;
        _item.effect1 = item->m_sItemEffectType;
        _item.effect2 = item->m_sItemEffectValue1;
        _item.effect3 = item->m_sItemEffectValue2;
        _item.durability = item->m_wCurLifeSpan;
        _item.attribute = item->m_dwAttribute;
        _item.equipped = client->m_bIsItemEquipped[i] == true;
        _item.itemposx = client->m_ItemPosList[i].x;
        _item.itemposy = client->m_ItemPosList[i].y;
        _item.itemloc = "bag";

        character.items.push_back(_item);
    }
    for (int i = 0; i < DEF_MAXBANKITEMS; ++i)
    {
        CItem * item = client->m_pItemInBankList[i];
        if (item == nullptr)
            continue;
        item_db _item{};
        _item.id = item->id;
        _item.char_id = character.id;
        _item.name = item->m_cName;
        _item.count = item->m_dwCount;
        _item.color = item->m_cItemColor;
        _item.type = item->m_sTouchEffectType;
        _item.id1 = item->m_sTouchEffectValue1;
        _item.id2 = item->m_sTouchEffectValue2;
        _item.id3 = item->m_sTouchEffectValue3;
        _item.effect1 = item->m_sItemEffectType;
        _item.effect2 = item->m_sItemEffectValue1;
        _item.effect3 = item->m_sItemEffectValue2;
        _item.durability = item->m_wCurLifeSpan;
        _item.attribute = item->m_dwAttribute;
        _item.equipped = false;
        _item.itemloc = "bank";

        character.bank_items.push_back(_item);
    }

    return character;
}
