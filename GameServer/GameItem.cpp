#include "GameItem.h"

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

GameEquipItem::GameEquipItem()
{
    _itemMap.emplace(-1, std::make_shared<ItemEquip>(-1, -1, -1, -1, -1));
}

GameEquipItem::~GameEquipItem()
{
    _itemMap.clear();
}

void GameEquipItem::AddItem(int32 code, int32 type, int32 gold, int32 attack, int32 speed)
{
    if (_itemMap.find(code) != _itemMap.end())
    {
        return;
    }
    _itemMap.emplace(code, std::make_shared<ItemEquip>(code, type, gold, attack, speed));
}

ItemEquipRef GameEquipItem::GetItem(int32 code)
{
    if (_itemMap.find(code) == _itemMap.end())
    {
        return _itemMap[-1];
    }
    return _itemMap[code];
}

void GameEquipItem::ClearItem()
{
    _itemMap.clear();
}

GameEtcItem::GameEtcItem()
{
    _itemMap.emplace(-1, std::make_shared<ItemEtc>(-1, -1, -1, -1));
}

GameEtcItem::~GameEtcItem()
{
}

void GameEtcItem::AddItem(int32 code, int32 type, int32 gold, int32 cnt)
{
    if (_itemMap.find(code) != _itemMap.end())
    {
        return;
    }
    _itemMap.emplace(code, std::make_shared<ItemEtc>(code, type, gold, cnt));
}

ItemEtcRef GameEtcItem::GetItem(int32 code)
{
    if (_itemMap.find(code) == _itemMap.end())
    {
        return _itemMap[-1];
    }
    return _itemMap[code];
}

void GameEtcItem::ClearItem()
{
    _itemMap.clear();
}
