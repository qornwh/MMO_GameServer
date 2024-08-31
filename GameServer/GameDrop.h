﻿#pragma once
#include "DropItem.h"
#include "pch.h"

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
