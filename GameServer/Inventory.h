#pragma once
#include "pch.h"
#include "ReadWriteLock.h"
#include "GameUtils.h"

struct EquipItem
{
    EquipItem(UUID& uuid, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 equipPos, int32 invenPos);
    EquipItem(BYTE* uuid, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 equipPos, int32 invenPos);
    EquipItem(const EquipItem& other);
    EquipItem(EquipItem&& other) noexcept;
    ~EquipItem();

    EquipItem& operator=(const EquipItem& other);
    EquipItem& operator=(EquipItem&& other) noexcept;

    static EquipItem EmptyEquipItem();
    bool IsEmpty() const;

    BYTE _uuid[UUIDSIZE];
    int32 _itemCode;
    int32 _equipType;
    int32 _attack;
    int32 _speed;
    int32 _equipPos;
    int32 _invenPos;
};

struct EtcItem
{
    EtcItem(int32 itemCode, int32 type, int32 count, int32 invenPos, bool isNew = false);
    EtcItem(const EtcItem& other);
    EtcItem(EtcItem&& other) noexcept;
    ~EtcItem();

    EtcItem& operator=(const EtcItem& other);
    EtcItem& operator=(EtcItem&& other) noexcept;

    static EtcItem EmptyEtcItem();
    bool IsEmpty() const;
    void UpdateItem(int32 count = 1);

    int32 _itemCode;
    int32 _count;
    int32 _type;
    int32 _invenPos;
    bool _isNew;
};

class Inventory
{
public:
    Inventory();
    ~Inventory();

    void Init(int32 playerCode, int32 gold);
    bool CheckItemEquip(int32 invenPos);
    bool UseItemEquip(int32 invenPos);
    bool ItemEquipped(int32& invenPos, int32& equipPos);
    bool CheckEquipped(int32 invenPos);
    EquipItem& AddItemEquip(EquipItem& equip);
    bool CheckItemEtc(int32 invenPos, int32 count = 1);
    bool UseItemEtc(int32 invenPos, int32 count = 1);
    EtcItem& AddItemEtc(EtcItem& etc);
    bool CheckGold(int32 gold);
    bool UseGold(int32 gold);
    void AddGold(int32 gold);
    void SaveDB();
    int32 GetGold() { return _gold; }
    Vector<EquipItem>& GetEquipItemInfo() { return _inventoryEquipItemList; }
    Vector<EtcItem>& GetEtcItemInfo() { return _inventoryEtcItemList; }
    Vector<EquipItem>& GetEquippedItemInfo() { return _equippedItemList; }
    
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
    Vector<EquipItem> _inventoryEquipItemList; // key : inventory position
    Vector<EtcItem> _inventoryEtcItemList; // key : inventory position
    PriorityQueueDes<int32> _emptyEquipInvenList;
    PriorityQueueDes<int32> _emptyEtcInvenList;

    Vector<EquipItem> _equippedItemList; // key : 장착된 위치

    // 인벤토리 사이즈 고정
    const int32 _invenSize = 20;
    // 장비 소켓 사이즈 고정
    const int32 _equipSize = 3;

    // 맵에 비어있을때 빈 아이템사용함. 
    EquipItem _emptyEquip;
    EtcItem _emptyEtc;

    // 업데이트될 아이템 리스트
    Vector<EquipItem> _updateEquipList;
    Vector<EtcItem> _updateEtcList;
};
