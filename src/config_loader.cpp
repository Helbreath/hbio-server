//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Game.h"

void CGame::load_configs()
{
    char * cp;

    {
        std::ifstream config_file("GameConfigs\\BuildItem.cfg");
        if (!config_file.is_open()) { log->error("(!) BuildItem.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) BUILD-ITEM configuration contents received. Now decoding...");
        m_bIsBuildItemAvailable = _bDecodeBuildItemConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Item.cfg");
        if (!config_file.is_open()) { log->error("(!) Item.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) ITEM configuration contents received. Now decoding...");
        m_bIsItemAvailable = _bDecodeItemConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Item2.cfg");
        if (!config_file.is_open()) { log->error("(!) Item2.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) ITEM2 configuration contents received. Now decoding...");
        m_bIsItemAvailable = _bDecodeItemConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Item3.cfg");
        if (!config_file.is_open()) { log->error("(!) Item3.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) ITEM3 configuration contents received. Now decoding...");
        m_bIsItemAvailable = _bDecodeItemConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Npc.cfg");
        if (!config_file.is_open()) { log->error("(!) Npc.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) NPC configuration contents received. Now decoding...");
        m_bIsNpcAvailable = _bDecodeNpcConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Magic.cfg");
        if (!config_file.is_open()) { log->error("(!) Magic.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) MAGIC configuration contents received. Now decoding...");
        m_bIsMagicAvailable = _bDecodeMagicConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Skill.cfg");
        if (!config_file.is_open()) { log->error("(!) Skill.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) SKILL configuration contents received. Now decoding...");
        m_bIsSkillAvailable = _bDecodeSkillConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Quest.cfg");
        if (!config_file.is_open()) { log->error("(!) Quest.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) QUEST configuration contents received. Now decoding...");
        m_bIsQuestAvailable = _bDecodeQuestConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\Potion.cfg");
        if (!config_file.is_open()) { log->error("(!) Potion.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) POTION configuration contents received. Now decoding...");
        m_bIsPortionAvailable = _bDecodePortionConfigFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

    {
        std::ifstream config_file("GameConfigs\\notice.txt");
        if (!config_file.is_open()) { log->error("(!) BuildItem.cfg could not be loaded"); throw std::exception(); }
        config_file.seekg(0, std::ios::end); std::streampos length = config_file.tellg(); config_file.seekg(0, std::ios::beg);
        cp = new char[length]; ZeroMemory(cp, length); config_file.read(cp, length); config_file.close();
        log->info("(!) Noticement configuration contents received. Now decoding...");
        _bDecodeNoticementFileContents(cp, (uint32_t)length);
        delete[] cp;
    }

//     log->info("(!) DupItemID file contents received. Now decoding...");
//     _bDecodeDupItemIDFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
}
