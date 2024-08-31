#pragma once
#include "Types.h"

class Charater
{
public:
    Charater(int32 code, float attack, float moveSpeed, int32 hp, int32 exp = 0);
    Charater(const Charater& other);
    
    int32 _code;
    float _attack;
    float _moveSpeed;
    int32 _hp;
    int32 _exp;
};

class GameCharater : public std::enable_shared_from_this<GameCharater>
{
public:
    GameCharater();

    void ClearCharater();
    Map<int32, Charater>& GetCharater() { return _charater; }

private:
    Map<int32, Charater> _charater;
};
