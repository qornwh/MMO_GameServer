#pragma once
#include "Types.h"

class Weapon
{
public:
    Weapon(int32 code, float attackSpeed);
    Weapon(const Weapon& other);
    
    int32 _code;
    float _attackSpeed;
};

class GameWeapon : public std::enable_shared_from_this<GameWeapon>
{
public:
    GameWeapon();

    void ClearWeapon();
    Map<int32, Weapon>& GetWeapon() { return _weapon; }

private:
    Map<int32, Weapon> _weapon;
};
