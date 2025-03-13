//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"

int64_t CGame::create_db_character(pqxx::transaction_base & t, character_db & character)
{
    pqxx::row r{
        t.exec_prepared1("create_db_character", character.account_id, character.world_name, character.name,
            character.strength, character.vitality, character.dexterity, character.intelligence, character.magic, character.charisma,
            character.appr1, character.gender, character.skin, character.hairstyle, character.haircolor, character.underwear,
            character.hp, character.mp, character.sp)
    };
    return r["id"].as<uint64_t>();
}

void CGame::update_db_character(pqxx::transaction_base & t, character_db & character)
{
    t.exec_prepared("update_db_character", character.name, character.id1, character.id2, character.id3, character.level,
        character.strength, character.vitality, character.dexterity, character.intelligence, character.magic, character.charisma,
        character.experience, character.gender, character.skin, character.hairstyle, character.haircolor, character.underwear,
        character.apprcolor, character.appr1, character.appr2, character.appr3, character.appr4, character.nation,
        character.maploc, character.locx, character.locy, character.profile, character.adminlevel,
        character.contribution, character.leftspectime, character.lockmapname, character.lockmaptime, character.lastsavedate,
        character.blockdate, character.guild_id, character.fightnum, character.fightdate, character.fightticket, character.questnum,
        character.questid, character.questcount, character.questrewardtype, character.questrewardamount, character.questcompleted,
        character.eventid, character.warcon, character.crusadejob, character.crusadeid, character.crusadeconstructpoint, character.reputation,
        character.hp, character.mp, character.sp, character.ek, character.pk, character.rewardgold, character.downskillid, character.hunger,
        character.leftsac, character.leftshutuptime, character.leftreptime, character.leftforcerecalltime, character.leftfirmstaminatime,
        character.leftdeadpenaltytime, character.magicmastery, character.party_id, character.itemupgradeleft, character.totalek,
        character.totalpk, character.mmr, character.altmmr, character.head_appr, character.body_appr, character.arm_appr, character.leg_appr,
        character.gold, character.luck, character.world_name, character.id, character.account_id);
}

/**
 * @brief
 * @param pqxx::transaction_base & t
 * @param character_db & character - utilizes name and account_id
*/
void CGame::delete_db_character(pqxx::transaction_base & t, character_db & character)
{
    t.exec_params0("DELETE FROM characters WHERE name=$1 AND account_id=$2", character.name, character.account_id);
}

int64_t CGame::create_db_item(pqxx::transaction_base & t, item_db & _item)
{
    pqxx::row r{
        t.exec_prepared1("create_db_item", _item.char_id, _item.name, _item.count, _item.type, _item.id1, _item.id2, _item.id3,
        _item.color, _item.effect1, _item.effect2, _item.effect3, _item.durability, _item.attribute, _item.equipped, _item.itemposx,
        _item.itemposy, _item.itemloc, _item.item_id)
    };
    return r["id"].as<int64_t>();
}

int64_t CGame::create_db_item(CItem * _item, int32_t x, int32_t y, int32_t character_handle, bool equipped)
{
    std::shared_lock<std::shared_mutex> l(game_sql_mtx);
    pqxx::work txn{ *pq_game };
    item_db it;
    it.char_id = character_handle;
    it.id = _item->id;
    it.item_id = _item->m_sIDnum;
    it.name = _item->m_cName;
    it.count = _item->m_dwCount;
    it.color = _item->m_cItemColor;
    it.type = _item->m_sTouchEffectType;
    it.id1 = _item->m_sTouchEffectValue1;
    it.id2 = _item->m_sTouchEffectValue2;
    it.id3 = _item->m_sTouchEffectValue3;
    it.effect1 = _item->m_sItemEffectValue1;
    it.effect2 = _item->m_sItemEffectValue2;
    it.effect3 = _item->m_sItemEffectValue3;
    it.durability = _item->m_wCurLifeSpan;
    it.attribute = _item->m_dwAttribute;
    it.equipped = equipped;
    it.itemposx = x;
    it.itemposy = y;
    it.itemloc = "bag";
    auto result = create_db_item(txn, it);
    txn.commit();
    return result;
}

void CGame::update_db_item(pqxx::transaction_base & t, item_db & _item)
{
    t.exec_prepared("update_db_item", _item.id, _item.char_id, _item.name, _item.count, _item.type, _item.id1, _item.id2, _item.id3,
        _item.color, _item.effect1, _item.effect2, _item.effect3, _item.durability, _item.attribute, _item.equipped, _item.itemposx,
        _item.itemposy, _item.itemloc, _item.item_id);
}

void CGame::update_db_bag_item(CItem * _item, int32_t x, int32_t y, int32_t character_handle, bool equipped)
{
    std::shared_lock<std::shared_mutex> l(game_sql_mtx);
    pqxx::work txn{ *pq_game };
    item_db it;
    it.char_id = character_handle;
    it.id = _item->id;
    it.item_id = _item->m_sIDnum;
    it.name = _item->m_cName;
    it.count = _item->m_dwCount;
    it.color = _item->m_cItemColor;
    it.type = _item->m_sTouchEffectType;
    it.id1 = _item->m_sTouchEffectValue1;
    it.id2 = _item->m_sTouchEffectValue2;
    it.id3 = _item->m_sTouchEffectValue3;
    it.effect1 = _item->m_sItemEffectValue1;
    it.effect2 = _item->m_sItemEffectValue2;
    it.effect3 = _item->m_sItemEffectValue3;
    it.durability = _item->m_wCurLifeSpan;
    it.attribute = _item->m_dwAttribute;
    it.equipped = equipped;
    it.itemposx = x;
    it.itemposy = y;
    it.itemloc = "bag";
    update_db_item(txn, it);
    txn.commit();
}

void CGame::update_db_bank_item(CItem * _item, int32_t character_handle)
{
    std::shared_lock<std::shared_mutex> l(game_sql_mtx);
    pqxx::work txn{ *pq_game };
    item_db it;
    it.char_id = character_handle;
    it.id = _item->id;
    it.item_id = _item->m_sIDnum;
    it.name = _item->m_cName;
    it.count = _item->m_dwCount;
    it.color = _item->m_cItemColor;
    it.type = _item->m_sTouchEffectType;
    it.id1 = _item->m_sTouchEffectValue1;
    it.id2 = _item->m_sTouchEffectValue2;
    it.id3 = _item->m_sTouchEffectValue3;
    it.effect1 = _item->m_sItemEffectValue1;
    it.effect2 = _item->m_sItemEffectValue2;
    it.effect3 = _item->m_sItemEffectValue3;
    it.durability = _item->m_wCurLifeSpan;
    it.attribute = _item->m_dwAttribute;
    it.equipped = false;
    it.itemposx = 0;
    it.itemposy = 0;
    it.itemloc = "bank";
    update_db_item(txn, it);
    txn.commit();
}

void CGame::delete_db_item(pqxx::transaction_base & t, int64_t id)
{
    t.exec_prepared("delete_db_item", id);
}

void CGame::delete_db_item(int64_t id)
{
    std::shared_lock<std::shared_mutex> l(game_sql_mtx);
    pqxx::work txn{ *pq_game };
    txn.exec_prepared("delete_db_item", id);
    txn.commit();
}

void CGame::force_delete_db_item(pqxx::transaction_base & t, int64_t id)
{
    t.exec_prepared("force_delete_db_item", id);
}

void CGame::force_delete_db_item(int64_t id)
{
    std::shared_lock<std::shared_mutex> l(game_sql_mtx);
    pqxx::work txn{ *pq_game };
    txn.exec_prepared("force_delete_db_item", id);
    txn.commit();
}

int64_t CGame::create_db_skill(pqxx::transaction_base & t, skill_db & skill)
{
    pqxx::row r{
        t.exec_prepared1("create_db_skill", skill.character_id, skill.skill_id, skill.skill_level, skill.skill_exp)
    };
    return r["id"].as<int64_t>();
}

void CGame::update_db_skill(pqxx::transaction_base & t, skill_db & skill)
{
    t.exec_prepared("update_db_skill", skill.id, skill.character_id, skill.skill_id, skill.skill_level, skill.skill_exp);
}

void CGame::delete_db_skills(pqxx::transaction_base & t, int64_t id)
{
    t.exec_prepared("delete_db_skills", id);
}

std::vector<item_db> CGame::get_db_items(pqxx::transaction_base & t, uint64_t character_id)
{
    pqxx::result result{ t.exec_prepared("get_items_by_character_id", character_id) };

    std::vector<item_db> items;
    for (const auto & row : result)
    {
        item_db item;
        item.id = row["id"].as<int64_t>();
        item.char_id = row["char_id"].as<int64_t>();
        item.name = row["name"].as<std::string>();
        item.count = row["count"].as<int64_t>();
        item.type = row["type"].as<int32_t>();
        item.id1 = row["id1"].as<int32_t>();
        item.id2 = row["id2"].as<int32_t>();
        item.id3 = row["id3"].as<int32_t>();
        item.color = row["color"].as<int32_t>();
        item.effect1 = row["effect1"].as<int32_t>();
        item.effect2 = row["effect2"].as<int32_t>();
        item.effect3 = row["effect3"].as<int32_t>();
        item.durability = row["durability"].as<int32_t>();
        item.attribute = row["attribute"].as<int64_t>();
        item.equipped = row["equipped"].as<bool>();
        item.itemposx = row["itemposx"].as<int32_t>();
        item.itemposy = row["itemposy"].as<int32_t>();
        item.itemloc = row["itemloc"].as<std::string>();
        item.item_id = row["item_id"].as<int64_t>();
        items.push_back(item);
    }

    return items;
}

std::vector<skill_db> CGame::get_db_skills(pqxx::transaction_base & t, uint64_t character_id)
{
    pqxx::result result{ t.exec_prepared("get_skills_by_character_id", character_id) };

    std::vector<skill_db> skills;
    for (const auto & row : result)
    {
        skill_db skill;
        skill.id = row["id"].as<int64_t>();
        skill.character_id = row["char_id"].as<int64_t>();
        skill.skill_id = row["skill_id"].as<int64_t>();
        skill.skill_level = row["mastery"].as<int32_t>();
        skill.skill_exp = row["experience"].as<int32_t>();
        skills.push_back(skill);
    }

    return skills;
}

void CGame::prepare_login_statements()
{

}

void CGame::prepare_game_statements()
{
    pq_game->prepare(
        "create_db_character",
        R"(
            INSERT INTO characters
            (
                account_id, world_name, name,
                strength, vitality, dexterity,
                intelligence, magic, charisma,
                appr1, gender, skin, hairstyle,
                haircolor, underwear, hp, mp, sp
            )
            VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18)
            RETURNING id
        )"
    );
    pq_game->prepare(
        "update_db_character",
        R"(
            UPDATE characters
            SET
                name=$1,
                id1=$2,
                id2=$3,
                id3=$4,
                level=$5,
                strength=$6,
                vitality=$7,
                dexterity=$8,
                intelligence=$9,
                magic=$10,
                charisma=$11,
                experience=$12,
                gender=$13,
                skin=$14,
                hairstyle=$15,
                haircolor=$16,
                underwear=$17,
                apprcolor=$18,
                appr1=$19,
                appr2=$20,
                appr3=$21,
                appr4=$22,
                nation=$23,
                maploc=$24,
                locx=$25,
                locy=$26,
                profile=$27,
                adminlevel=$28,
                contribution=$29,
                leftspectime=$30,
                lockmapname=$31,
                lockmaptime=$32,
                lastsavedate=$33,
                blockdate=$34,
                guild_id=$35,
                fightnum=$36,
                fightdate=$37,
                fightticket=$38,
                questnum=$39,
                questid=$40,
                questcount=$41,
                questrewardtype=$42,
                questrewardamount=$43,
                questcompleted=$44,
                eventid=$45,
                warcon=$46,
                crusadejob=$47,
                crusadeid=$48,
                crusadeconstructpoint=$49,
                reputation=$50,
                hp=$51,
                mp=$52,
                sp=$53,
                ek=$54,
                pk=$55,
                rewardgold=$56,
                downskillid=$57,
                hunger=$58,
                leftsac=$59,
                leftshutuptime=$60,
                leftreptime=$61,
                leftforcerecalltime=$62,
                leftfirmstaminatime=$63,
                leftdeadpenaltytime=$64,
                magicmastery=$65,
                party_id=$66,
                itemupgradeleft=$67,
                totalek=$68,
                totalpk=$69,
                mmr=$70,
                altmmr=$71,
                head_appr=$72,
                body_appr=$73,
                arm_appr=$74,
                leg_appr=$75,
                gold=$76,
                luck=$77,
                world_name=$78
            WHERE id=$79 and account_id=$80
        )"
    );
    pq_game->prepare("check_character_count_by_account_id_wn", R"(SELECT COUNT(*) FROM characters WHERE account_id=$1 AND world_name=$2)");
    pq_game->prepare("check_character_count_by_name_wn", R"(SELECT COUNT(*) FROM characters WHERE name=$1 AND world_name=$2)");
    pq_game->prepare("get_characters_by_account_id_wn", R"(SELECT * FROM characters WHERE account_id=$1 AND world_name=$2 ORDER BY id ASC)");
    pq_game->prepare("get_character_by_id_wn", R"(SELECT * FROM characters WHERE id=$1 AND world_name=$2 ORDER BY id ASC)");
    pq_game->prepare("get_character_by_name_wn", R"(SELECT * FROM characters WHERE name=$1 AND world_name=$2 ORDER BY id ASC)");
    pq_game->prepare("delete_character_by_id_wn", R"(SELECT * FROM characters WHERE name=$1 AND world_name=$2 ORDER BY id ASC)");
    pq_game->prepare("delete_item_by_id", R"(SELECT * FROM characters WHERE name=$1 AND world_name=$2 ORDER BY id ASC)");
    pq_game->prepare("get_items_by_character_id", R"(SELECT * FROM items WHERE char_id=$1 ORDER BY id ASC)");
    pq_game->prepare("get_skills_by_character_id", R"(SELECT * FROM skills WHERE char_id=$1 ORDER BY id ASC)");
    pq_game->prepare(
        "create_db_item",
        R"(
            INSERT INTO items
            (
                char_id, name, count,
                type, id1, id2, id3, color, effect1,
                effect2, effect3, durability,
                attribute, equipped, itemposx,
                itemposy, itemloc, item_id
            )
            VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18)
            RETURNING id
        )"
    );
    pq_game->prepare(
        "update_db_item",
        R"(
            UPDATE items
            SET
                char_id=$2,
                name=$3,
                count=$4,
                type=$5,
                id1=$6,
                id2=$7,
                id3=$8,
                color=$9,
                effect1=$10,
                effect2=$11,
                effect3=$12,
                durability=$13,
                attribute=$14,
                equipped=$15,
                itemposx=$16,
                itemposy=$17,
                itemloc=$18,
                item_id=$19
            WHERE id=$1
        )"
    );
    pq_game->prepare(
        "delete_db_item",
        R"(UPDATE items SET char_id=0 WHERE id=$1)"
    );
    pq_game->prepare(
        "force_delete_db_item",
        R"(DELETE FROM items WHERE id=$1)"
    );
    pq_game->prepare(
        "create_db_skill",
        R"(
            INSERT INTO skills
            (
                char_id, skill_id, mastery, experience
            )
            VALUES ($1,$2,$3,$4)
            RETURNING id
        )"
    );
    pq_game->prepare(
        "update_db_skill",
        R"(
            UPDATE skills
            SET
                mastery=$4,
                experience=$5
            WHERE id=$1 AND char_id=$2 AND skill_id=$3
        )"
    );
    pq_game->prepare(
        "delete_db_skills",
        R"(DELETE FROM skills WHERE char_id=$1)"
    );
}
