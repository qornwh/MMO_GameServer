#include "GameItem.h"

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
