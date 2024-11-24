//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <cstdint>

class CMisc
{
public:
    bool bCheckValidName(char * pStr);
    bool bDecode(char cKey, char * pStr);
    bool bEncode(char cKey, char * pStr);
    void GetDirPoint(char cDir, int * pX, int * pY);
    void GetPoint2(int x0, int y0, int x1, int y1, int * pX, int * pY, int * pError, int iCount);
    void GetPoint(int x0, int y0, int x1, int y1, int * pX, int * pY, int * pError);
    char cGetNextMoveDir(short sX, short sY, short dX, short dY);
    CMisc() = default;
    ~CMisc() = default;
};
