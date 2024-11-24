//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "Npc.h"

CNpc::CNpc(char * pName5)
{
	memset(m_cName, 0, sizeof(m_cName));
    int len = strlen(pName5);
    if (len > 5)
        len = 5;
	memcpy(m_cName, pName5, len);

	for (int i = 0; i < DEF_MAXWAYPOINTS; i++)			  
		m_iWayPointIndex[i] = -1;
}

