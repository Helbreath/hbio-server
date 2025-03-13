//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "game.h"

// todo: reduce down to a single function?
void CGame::SetInvisibilityFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000010;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFFEF;

            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000010;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFFEF;

            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetInhibitionCastingFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00100000;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFEFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00100000;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFEFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetBerserkFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000020;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFFDF;

            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000020;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFFDF;

            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetIceFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000040;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFFBF;

            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000040;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFFBF;

            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetPoisonFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00000080;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFFFF7F;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00000080;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFFFF7F;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetIllusionFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x01000000;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFEFFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x01000000;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFEFFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetHeroFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00020000;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFDFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x00020000;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFFFDFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetDefenseShieldFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x02000000;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFDFFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x02000000;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFDFFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetMagicProtectionFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x04000000;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFBFFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x04000000;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xFBFFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetProtectionFromArrowFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x08000000;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xF7FFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;

        case DEF_OWNERTYPE_NPC:
            if (m_pNpcList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus | 0x08000000;
            else m_pNpcList[sOwnerH]->m_iStatus = m_pNpcList[sOwnerH]->m_iStatus & 0xF7FFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}

void CGame::SetIllusionMovementFlag(short sOwnerH, char cOwnerType, bool bStatus)
{
    switch (cOwnerType)
    {
        case DEF_OWNERTYPE_PLAYER:
            if (m_pClientList[sOwnerH] == 0) return;
            if (bStatus == true)
                m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00200000;
            else m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFDFFFFF;
            SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
            break;
    }
}
