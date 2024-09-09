#pragma once
#include "pch.h"

class DropItem : public std::enable_shared_from_this<DropItem>
{
public:
    DropItem(int32 itemCode, int32 cnt);
    ~DropItem();

    int32 GetItemCode() { return _itemCode; }
    int32 GetCnt() { return _cnt; }

private:
    int32 _itemCode;
    int32 _cnt;
};

class DropEquipItem : public std::enable_shared_from_this<DropEquipItem>
{
public:
    DropEquipItem(int32 itemCode);
    ~DropEquipItem();

    int32 GetItemCode() { return _itemCode; }

private:
    int32 _itemCode;
    int32 _cnt;
};

class GameDrop : public std::enable_shared_from_this<GameDrop>
{
public:
    GameDrop() {}
    ~GameDrop() {}

    void AddDropItem(int32 monsterCode, int32 itemCode, int32 cnt);
    void AddDropEquipItem(int32 monsterCode, int32 itemCode);
    void AddGold(int32 monsterCode, int32 gold);
    Vector<DropItem>& GetMonsterDropList(int32 monsterCode);
    Vector<DropEquipItem>& GetMonsterDropEquipList(int32 monsterCode);
    int32 GetMonsterGold(int32 monsterCode);
    void ClearItem();

private:
    Map<int32, Tuple<Vector<DropItem>, int32>> _monsterDropList;
    Map<int32, Vector<DropEquipItem>> _monsterDropEquipList;
};
