#include "GameDrop.h"

DropItem::DropItem(int32 itemCode, int32 cnt) : _itemCode(itemCode), _cnt(cnt)
{
}

DropItem::~DropItem()
{
}

DropEquipItem::DropEquipItem(int32 itemCode) : _itemCode(itemCode), _cnt(1)
{
}

DropEquipItem::~DropEquipItem()
{
}

void GameDrop::AddDropItem(int32 monsterCode, int32 itemCode, int32 cnt)
{
    if (_monsterDropList.find(monsterCode) == _monsterDropList.end())
    {
        _monsterDropList.emplace(monsterCode, Tuple<Vector<DropItem>, int>{Vector<DropItem>(), 0});
    }
    _monsterDropList[monsterCode].first.emplace_back(DropItem(itemCode, cnt));
}

void GameDrop::AddDropEquipItem(int32 monsterCode, int32 itemCode)
{
    if (_monsterDropEquipList.find(monsterCode) == _monsterDropEquipList.end())
    {
        _monsterDropEquipList.emplace(monsterCode, Vector<DropEquipItem>{});
    }
    _monsterDropEquipList[monsterCode].emplace_back(DropEquipItem(itemCode));
}

void GameDrop::AddGold(int32 monsterCode, int32 gold)
{
    if (_monsterDropList.find(monsterCode) == _monsterDropList.end())
    {
        _monsterDropList.emplace(monsterCode, Tuple<Vector<DropItem>, int>{Vector<DropItem>(), 0});
    }
    _monsterDropList[monsterCode].second = gold;
}

Vector<DropItem>& GameDrop::GetMonsterDropList(int32 monsterCode)
{
    return _monsterDropList[monsterCode].first;
}

Vector<DropEquipItem>& GameDrop::GetMonsterDropEquipList(int32 monsterCode)
{
    return _monsterDropEquipList[monsterCode];
}

int32 GameDrop::GetMonsterGold(int32 monsterCode)
{
    return _monsterDropList[monsterCode].second;
}

void GameDrop::ClearItem()
{
    _monsterDropList.clear();
}
