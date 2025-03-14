//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "item.h"

CItem::CItem()
{
    memset(m_cName, 0, sizeof(m_cName));
    m_sSprite = 0;
    m_sSpriteFrame = 0;

    m_sItemEffectValue1 = 0;
    m_sItemEffectValue2 = 0;
    m_sItemEffectValue3 = 0;

    m_sItemEffectValue4 = 0;
    m_sItemEffectValue5 = 0;
    m_sItemEffectValue6 = 0;

    m_dwCount = 1;
    m_sTouchEffectType = 0;
    m_sTouchEffectValue1 = 0;
    m_sTouchEffectValue2 = 0;
    m_sTouchEffectValue3 = 0;

    m_cItemColor = 0;
    m_sItemSpecEffectValue1 = 0;
    m_sItemSpecEffectValue2 = 0;
    m_sItemSpecEffectValue3 = 0;

    m_sSpecialEffectValue1 = 0;
    m_sSpecialEffectValue2 = 0;

    m_wCurLifeSpan = 0;
    m_dwAttribute = 0;

    m_cCategory = 0;
    m_sIDnum = 0;

    m_bIsForSale = false;
}
