#include "GameWeapon.h"

Weapon::Weapon(int32 code, float attackSpeed): _code(code), _attackSpeed(attackSpeed)
{
}

Weapon::Weapon(const Weapon& other)
{
    _code = other._code;
    _attackSpeed = other._attackSpeed;
}

GameWeapon::GameWeapon()
{
}

void GameWeapon::ClearWeapon()
{
    _weapon.clear();
}
