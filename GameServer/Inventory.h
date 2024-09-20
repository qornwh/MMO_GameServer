#pragma once
#include "pch.h"
#include "ReadWriteLock.h"

struct EquipItem
{
    EquipItem(int32 uniqueId, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 isEquip, int32 position, int32 use);
    EquipItem(const EquipItem& other);
    EquipItem(EquipItem&& other) noexcept;
    ~EquipItem();

    EquipItem& operator=(const EquipItem& other);
    EquipItem& operator=(EquipItem&& other) noexcept;

    static EquipItem EmptyEquipItem();

    void UpdateItem(int32 use = 1);
    int32 _uniqueId;
    int32 _itemCode;
    int32 _equipType;
    int32 _attack;
    int32 _speed;
    int32 _isEquip;
    int32 _position;
    int32 _use;
};

struct EtcItem
{
    EtcItem(int32 itemCode, int32 type, int32 count, int32 position, bool isNew = false);
    EtcItem(const EtcItem& other);
    EtcItem(EtcItem&& other) noexcept;
    ~EtcItem();

    EtcItem& operator=(const EtcItem& other);
    EtcItem& operator=(EtcItem&& other) noexcept;

    static EtcItem EmptyEtcItem();

    void UpdateItem(int32 count = 1);
    int32 _itemCode;
    int32 _count;
    int32 _type;
    int32 _position;
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
    EquipItem& ItemEquipped(int32 uniqueId, int32 equipped);
    bool CheckEquipped(int32 uniqueId, int32 equipped);
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
    
    void ResetUpdateItems();
    Vector<EquipItem>& GetUpdateEquipList() { return _updateEquipList; }
    Vector<EtcItem>& GetUpdateEtcList() { return _updateEtcList; }
    bool AddMailItemEquip(EquipItem& equip);
    bool AddMailItemEtc(EtcItem& etc);

private:
    Lock lock;
    int32 _gold = 0;
    int32 _playerCode = -1;
    std::weak_ptr<GamePlayerInfo> _playerInfo;
    Map<int32, EquipItem> _inventoryEquipItemList;
    Map<int32, EtcItem> _inventoryEtcItemList;
    PriorityQueueDes<int32> _emptyEquipInvenList;
    PriorityQueueDes<int32> _emptyEtcInvenList;

    int32 _weaponSocket = -1;
    int32 _shoseSocket = -1;

    // 인벤토리 사이즈 고정
    const int32 _invenSize = 20;

    // 맵에 비어있을때 빈 아이템사용함. 
    EquipItem _emptyEquip;
    Atomic<int32> _uniqueNum{0};

    // 업데이트될 아이템 리스트
    Vector<EquipItem> _updateEquipList;
    Vector<EtcItem> _updateEtcList;
};
