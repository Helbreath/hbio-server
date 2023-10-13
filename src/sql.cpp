//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

int64_t CGame::create_db_character(pqxx::transaction_base & t, character_db & character)
{
    pqxx::row r{
        t.exec_prepared1("create_db_character", character.account_id, pq_game->quote(character.world_name), pq_game->quote(character.name),
            character.strength, character.vitality, character.dexterity, character.intelligence, character.magic, character.charisma,
            character.appr1, character.gender, character.skin, character.hairstyle, character.haircolor, character.underwear,
            character.hp, character.mp, character.sp)
    };
    return r["id"].as<int64_t>();
}

void CGame::update_db_character(pqxx::transaction_base & t, character_db & character)
{
    t.exec_prepared("update_db_character", pq_game->quote(character.name), character.id1, character.id2, character.id3, character.level,
        character.strength, character.vitality, character.dexterity, character.intelligence, character.magic, character.charisma,
        character.experience, character.gender, character.skin, character.hairstyle, character.haircolor, character.underwear,
        character.apprcolor, character.appr1, character.appr2, character.appr3, character.appr4, pq_game->quote(character.nation),
        pq_game->quote(character.maploc), character.locx, character.locy, pq_game->quote(character.profile), character.adminlevel,
        character.contribution, character.leftspectime, pq_game->quote(character.lockmapname), character.lockmaptime, character.lastsavedate,
        character.blockdate, character.guild_id, character.fightnum, character.fightdate, character.fightticket, character.questnum,
        character.questid, character.questcount, character.questrewardtype, character.questrewardamount, character.questcompleted,
        character.eventid, character.warcon, character.crusadejob, character.crusadeid, character.crusadeconstructpoint, character.reputation,
        character.hp, character.mp, character.sp, character.ek, character.pk, character.rewardgold, character.downskillid, character.hunger,
        character.leftsac, character.leftshutuptime, character.leftreptime, character.leftforcerecalltime, character.leftfirmstaminatime,
        character.leftdeadpenaltytime, pq_game->quote(character.magicmastery), character.party_id, character.itemupgradeleft, character.totalek,
        character.totalpk, character.mmr, character.altmmr, character.head_appr, character.body_appr, character.arm_appr, character.leg_appr,
        character.gold, character.luck, pq_game->quote(character.world_name), character.id, character.account_id);
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
    pq_game->prepare("get_characters_by_account_id_wn", R"(SELECT * FROM characters WHERE account_id=$1 AND world_name=$2)");
    pq_game->prepare("get_character_by_id_wn", R"(SELECT * FROM characters WHERE id=$1 AND world_name=$2)");
    pq_game->prepare("get_character_by_name_wn", R"(SELECT * FROM characters WHERE name=$1 AND world_name=$2)");
}
