//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <windows.h>

class CTeleport  
{
public:
	CTeleport(); 
	~CTeleport() = default;

	char  m_cTeleportNum;
	char  m_cTeleportNpcName[30]; // The npc with teleport ability.
	char  m_cSourceMap[10]; // The map the teleport npc is located in.
	char  m_cTargetMap[10]; // The map the player will be teleported to.
	short m_sDestinationX, m_sDestinationY; // The destination coordinates on the map.
	short m_sTeleportCost; // The amount of gold paid to teleport.
	short m_sTeleportMinLevel, m_sTeleportMaxLevel; // The required level range for the player.
	char  m_cTeleportSide[7]; // The side the player must be for a successful teleport.
			// both
			// elvine
			// aresden
	BOOL m_bTeleportHunt, m_bTeleportNtrl, m_bTeleportCrmnl; // Check if player is a Civilian, Neutral, or Criminal.
};
