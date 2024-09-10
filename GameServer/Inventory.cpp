#include "Inventory.h"
#include "DBConnectPool.h"
#include "InventoryDB.h"
#include "SessionDB.h"

EquipItem::EquipItem(int32 uniqueId, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 isEquip, int32 use):
    _uniqueId(uniqueId),
    _itemCode(itemCode),
    _equipType(equipType),
    _attack(attack),
    _speed(speed),
    _isEquip(isEquip),
    _use(use)
{
}

EquipItem::~EquipItem()
{
}

EquipItem& EquipItem::operator=(const EquipItem& other)
{
    _uniqueId = other._uniqueId;
    _itemCode = other._itemCode;
    _equipType = other._equipType;
    _attack = other._attack;
    _speed = other._speed;
    _isEquip = other._isEquip;
    _use = other._use;
    return *this;
}

void EquipItem::UpdateItem(int32 use)
{
    _use = use;
}

EtcItem::EtcItem(int32 itemCode, int32 type, int32 count, bool isNew): _itemCode(itemCode), _count(count), _type(type), _isNew(isNew)
{
}

EtcItem::~EtcItem()
{
}

EtcItem& EtcItem::operator=(const EtcItem& other)
{
    _itemCode = other._itemCode;
    _count = other._count;
    _type = other._type;
    _isNew = other._isNew;
    return *this;
}

void EtcItem::UpdateItem(int32 count)
{
    _count = count;
}

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

void Inventory::Init(int32 playerCode, int32 gold)
{
    _playerCode = playerCode;
    
    InventoryDB inventoryDB;
    inventoryDB.LoadEquipDB(_playerCode);
    EquipItem equip(0,0,0,0,0,0,0);
    while(inventoryDB.GetEquipItem(equip))
    {
        AddItemEquip(equip);
    }
    
    inventoryDB.LoadEtcDB(_playerCode);
    EtcItem etc(0,0,0);
    while(inventoryDB.GetEtcItem(etc))
    {
        AddItemEtc(etc);
    }
    _gold = gold;
}

bool Inventory::CheckItemEquip(int32 uniqueId)
{
    auto it = _inventoryEquipItemList.find(uniqueId);
    if (it != _inventoryEquipItemList.end())
    {
        return true;
    }
    return false;
}

bool Inventory::UseItemEquip(int32 uniqueId)
{
    WriteLockGuard writeLock(lock, "inventory");
    auto it = _inventoryEquipItemList.find(uniqueId);
    if (it != _inventoryEquipItemList.end())
    {
        // it->second.UpdateItem();
        _inventoryEquipItemList.erase(uniqueId);
        return true;
    }
    return false;
}

EquipItem& Inventory::AddItemEquip(EquipItem& equip)
{
    _uniqueNum.fetch_add(1);
    equip._uniqueId = _uniqueNum.load();
    _inventoryEquipItemList.emplace(equip._uniqueId, equip);
    return equip;
}

bool Inventory::CheckItemEtc(int32 code, int32 count)
{
    auto it = _inventoryEtcItemList.find(code);
    if (it != _inventoryEtcItemList.end())
    {
        if (it->second._count >= count)
            return true;
    }
    return false;
}

bool Inventory::UseItemEtc(int32 itemCode, int32 count)
{
    WriteLockGuard writeLock(lock, "inventory");
    if (CheckItemEtc(itemCode, count))
    {
        auto it = _inventoryEtcItemList.find(itemCode);
        it->second._count = it->second._count - count;
        return true;
    }
    return false;
}

EtcItem& Inventory::AddItemEtc(EtcItem& etc)
{
    int32 itemCode = etc._itemCode;
    auto it = _inventoryEtcItemList.find(itemCode);
    if (it != _inventoryEtcItemList.end())
    {
        int curCount = it->second._count;
        it->second.UpdateItem(curCount + etc._count);
    }
    else
    {
        _inventoryEtcItemList.emplace(itemCode, etc);
        it = _inventoryEtcItemList.find(itemCode);
    }
    return it->second;
}

bool Inventory::CheckGold(int32 gold)
{
    if (_gold >= gold)
        return true;
    return false;
}

bool Inventory::UseGold(int32 gold)
{
    if (CheckGold(gold))
    {
        _gold -= gold;
        return true;
    }
    return false;
}

void Inventory::AddGold(int32 gold)
{
    _gold += gold;
}

void Inventory::SaveDB()
{
    InventoryDB inventoryDB;

    inventoryDB.SaveDeleteEquipDB(_playerCode);
    for (auto& inventoryItem : _inventoryEquipItemList)
    {
        if (inventoryItem.second._use == 0)
        {
            inventoryDB.SaveInsertEquipDB(_playerCode, inventoryItem.second);
        }
    }
    
    for (auto& inventoryItem : _inventoryEtcItemList)
    {
        EtcItem& item = inventoryItem.second;

        if (item._isNew)
        {
            // 신규추가
            if (item._count > 0)
            {
                inventoryDB.SaveInsertEtcDB(_playerCode, item._itemCode, item._type, item._count);
            }
        }
        else
        {
            // 기존 업데이트
            if (item._count == 0)
            {
                inventoryDB.SaveDeleteEtcDB(_playerCode, item._count);
            }
            else
            {
                inventoryDB.SaveUpdateEtcDB(_playerCode, item._itemCode, item._count);
            }
        }
    }
    
    SessionDB sessionDB;
    sessionDB.SavePlayerDB(_playerCode, _gold);
}
