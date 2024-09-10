#pragma once
#include "pch.h"
#include "ReadWriteLock.h"

struct EquipItem
{
    EquipItem(int32 uniqueId, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 isEquip, int32 use);
    ~EquipItem();

    EquipItem& operator=(const EquipItem& other);

    void UpdateItem(int32 use = 1);
    int32 _uniqueId;
    int32 _itemCode;
    int32 _equipType;
    int32 _attack;
    int32 _speed;
    int32 _isEquip;
    int32 _use;
};

struct EtcItem
{
    EtcItem(int32 itemCode, int32 type, int32 count, bool isNew = false);
    ~EtcItem();

    EtcItem& operator=(const EtcItem& other);

    void UpdateItem(int32 count = 1);
    int32 _itemCode;
    int32 _count;
    int32 _type;
    bool _isNew;
};

class Inventory
{
public:
    Inventory();
    ~Inventory();

    void Init(int32 playerCode, int32 gold);
    bool CheckItemEquip(int32 uniqueId);
    bool UseItemEquip(int32 uniqueId);
    EquipItem& AddItemEquip(EquipItem& equip);
    bool CheckItemEtc(int32 code, int32 count = 1);
    bool UseItemEtc(int32 itemCode, int32 count = 1);
    EtcItem& AddItemEtc(EtcItem& etc);
    bool CheckGold(int32 gold);
    bool UseGold(int32 gold);
    void AddGold(int32 gold);
    void SaveDB();
    int32 GetGold() { return _gold; }
    Map<int32, EquipItem>& GetEquipItemInfo() { return _inventoryEquipItemList; }
    Map<int32, EtcItem>& GetEtcItemInfo() { return _inventoryEtcItemList; }

private:
    Lock lock;
    int32 _gold = 0;
    int32 _playerCode = -1;
    std::weak_ptr<GamePlayerInfo> _playerInfo;
    Map<int32, EquipItem> _inventoryEquipItemList;
    Map<int32, EtcItem> _inventoryEtcItemList;

    Atomic<int32> _uniqueNum{0};
};
