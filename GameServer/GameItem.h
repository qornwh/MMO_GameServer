#pragma once
#include "Item.h"

class GameEquipItem : public std::enable_shared_from_this<GameEquipItem>
{
public:
    GameEquipItem();
    ~GameEquipItem();
    void AddItem(int32 code, int32 type, int32 gold, int32 attack, int32 speed);
    ItemEquipRef GetItem(int32 code);
    void ClearItem();

private:
    Map<int32, ItemEquipRef> _itemMap;
};

class GameEtcItem : public std::enable_shared_from_this<GameEtcItem>
{
public:
    GameEtcItem();
    ~GameEtcItem();
    void AddItem(int32 code, int32 type, int32 gold, int32 cnt);
    ItemEtcRef GetItem(int32 code);
    void ClearItem();

private:
    Map<int32, ItemEtcRef> _itemMap;
};

