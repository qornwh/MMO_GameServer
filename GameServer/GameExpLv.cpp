#include "GameExpLv.h"

ExpLv::ExpLv(int32 lv, int32 exp) : _lv(lv), _exp(exp)
{
}

ExpLv::ExpLv(const ExpLv& other)
{
    _lv = other._lv;
    _exp = other._exp;
}

GameExpLv::GameExpLv()
{
}

void GameExpLv::ClearExpLv()
{
    _expLv.clear();
}
