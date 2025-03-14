//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "dynamic_object.h"

CDynamicObject::CDynamicObject(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, uint32_t dwRegisterTime, uint32_t dwLastTime, int iV1)
{
    m_sOwner = sOwner;
    m_cOwnerType = cOwnerType;

    m_sType = sType;

    m_cMapIndex = cMapIndex;
    m_sX = sX;
    m_sY = sY;

    m_dwRegisterTime = dwRegisterTime;
    m_dwLastTime = dwLastTime;

    m_iCount = 0;
    m_iV1 = iV1;
}
