#pragma once
#include "pch.h"

class ItemEquip : public std::enable_shared_from_this<ItemEquip>
{
public:
    ItemEquip(int32 code, int32 type, int32 gold, int32 attack, int32 speed);
    ~ItemEquip();

    int32 GetCode() { return _code; }
    void SetCode(int32 code);
    int32 GetType() { return _type; }
    void SetType(int32 type);
    int32 GetGold() { return _gold; }
    void SetGold(int32 gold);
    int32 GetAttack() { return _attack; }
    void SetAttack(int32 attack);
    int32 GetSpeed() { return _speed; }
    void SetSpeed(int32 speed);

private:
    int32 _code = 0;
    int32 _type = 0;
    int32 _gold = 0;
    int32 _attack = 0;
    int32 _speed = 0;
};

class ItemEtc : public std::enable_shared_from_this<ItemEtc>
{
public:
    ItemEtc(int32 code, int32 type, int32 gold, int32 cnt);
    ~ItemEtc();

    int32 GetCode() { return _code; }
    void SetCode(int32 code);
    int32 GetType() { return _type; }
    void SetType(int32 type);
    int32 GetGold() { return _gold; }
    void SetGold(int32 gold);
    int32 GetMaxCnt() { return _cnt; }
    void SetMaxCnt(int32 cnt);

private:
    int32 _code = 0;
    int32 _type = 0;
    int32 _gold = 0;
    int32 _cnt = 0;
};

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

