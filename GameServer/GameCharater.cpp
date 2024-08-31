#include "GameCharater.h"

Charater::Charater(int32 code, float attack, float moveSpeed, int32 hp, int32 exp): _code(code), _attack(attack), _moveSpeed(moveSpeed), _hp(hp), _exp(exp)
{
}

Charater::Charater(const Charater& other)
{
    _code = other._code;
    _attack = other._attack;
    _moveSpeed = other._moveSpeed;
    _hp = other._hp;
    _exp = other._exp;
}

GameCharater::GameCharater()
{
}

void GameCharater::ClearCharater()
{
    _charater.clear();
}
