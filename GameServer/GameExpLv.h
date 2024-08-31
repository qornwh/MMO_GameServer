#pragma once
#include "pch.h"

class ExpLv
{
public:
    ExpLv(int32 lv, int32 exp);
    ExpLv(const ExpLv& other);
    
    int32 _lv;
    int32 _exp;
};

class GameExpLv
{
public:
    GameExpLv();

    void ClearExpLv();
    Map<int32, ExpLv>& GetExpLv() { return _expLv; }

private:
    Map<int32, ExpLv> _expLv;
};
