#include "Item.h"

ItemEquip::ItemEquip(int32 code, int32 type, int32 gold, int32 attack, int32 speed) : _code(code), _type(type), _gold(gold), _attack(attack), _speed(speed)
{
}

ItemEquip::~ItemEquip()
{
}

void ItemEquip::SetCode(int32 code)
{
    _code = code;
}

void ItemEquip::SetType(int32 type)
{
    _type = type;
}

void ItemEquip::SetGold(int32 gold)
{
    _gold = gold;
}

void ItemEquip::SetAttack(int32 attack)
{
    _attack = attack;
}

void ItemEquip::SetSpeed(int32 speed)
{
    _speed = speed;
}

ItemEtc::ItemEtc(int32 code, int32 type, int32 gold, int32 cnt) : _code(code), _type(type), _gold(gold), _cnt(cnt)
{
}

ItemEtc::~ItemEtc()
{
}

void ItemEtc::SetCode(int32 code)
{
    _code = code;
}

void ItemEtc::SetType(int32 type)
{
    _type = type;
}

void ItemEtc::SetGold(int32 gold)
{
    _gold = gold;
}

void ItemEtc::SetMaxCnt(int32 cnt)
{
    _cnt = cnt;
}
