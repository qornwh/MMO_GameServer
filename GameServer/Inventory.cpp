#include "Inventory.h"
#include "DBConnectPool.h"
#include "InventoryDB.h"
#include "SessionDB.h"

EquipItem::EquipItem(int32 uniqueId, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 isEquip, int32 position, int32 use):
    _uniqueId(uniqueId),
    _itemCode(itemCode),
    _equipType(equipType),
    _attack(attack),
    _speed(speed),
    _isEquip(isEquip),
    _position(position),
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
    _position = other._position;
    _use = other._use;
    return *this;
}

EquipItem EquipItem::EmptyEquipItem()
{
    return EquipItem{-1, -1, -1, -1, -1, -1, -1, 0};
}

void EquipItem::UpdateItem(int32 use)
{
    _use = use;
}

EtcItem::EtcItem(int32 itemCode, int32 type, int32 count, int32 position, bool isNew): _itemCode(itemCode), _count(count), _type(type), _position(position),
                                                                                       _isNew(isNew)
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
    _position = other._position;
    _isNew = other._isNew;
    return *this;
}

EtcItem EtcItem::EmptyEtcItem()
{
    return EtcItem{-1, -1, -1, -1};
}

void EtcItem::UpdateItem(int32 count)
{
    _count = count;
}

Inventory::Inventory() : _emptyEquip(EquipItem::EmptyEquipItem())
{
}

Inventory::~Inventory()
{
}

void Inventory::Init(int32 playerCode, int32 gold)
{
    _playerCode = playerCode;

    Vector<int32> notEmptyEquipInven(_invenSize, 0);
    InventoryDB inventoryDB;
    inventoryDB.LoadEquipDB(_playerCode);
    EquipItem equip = EquipItem::EmptyEquipItem();
    while (inventoryDB.GetEquipItem(equip))
    {
        equip._use = 0;
        EquipItem& curEquip = AddItemEquip(equip);
        if (curEquip._position >= 0)
        {
            notEmptyEquipInven[equip._position] = 1;
        }
    }
    
    Vector<int32> notEmptyEtcInven(_invenSize, 0);
    inventoryDB.LoadEtcDB(_playerCode);
    EtcItem etc = EtcItem::EmptyEtcItem();
    while (inventoryDB.GetEtcItem(etc))
    {
        EtcItem& curEtc = AddItemEtc(etc);
        if (curEtc._position >= 0)
        {
            notEmptyEtcInven[etc._position] = 1;
        }
    }
    _gold = gold;

    // 인벤토리 빈공간 큐 적재
    for (int32 i = 0; i < _invenSize; i++)
    {
        if (notEmptyEquipInven[i] == 0)
            _emptyEquipInvenList.emplace(i);
        if (notEmptyEtcInven[i] == 0)
            _emptyEtcInvenList.emplace(i);
    }
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
        int32 invenPosition = it->second._position;
        if (invenPosition >= 0)
        {
            _emptyEquipInvenList.emplace(invenPosition);
        }
        _inventoryEquipItemList.erase(uniqueId);
        return true;
    }
    return false;
}

EquipItem& Inventory::ItemEquipped(int32 uniqueId, int32 equipped)
{
    WriteLockGuard writeLock(lock, "inventory");
    auto it = _inventoryEquipItemList.find(uniqueId);
    if (it != _inventoryEquipItemList.end())
    {
        it->second._isEquip = equipped;
        if (equipped == 1)
        {
            // 장착
            int32 socketIdx = _weaponSocket;
            if (it->second._equipType == 1)
            {
                socketIdx = _weaponSocket;
                _weaponSocket = it->second._uniqueId;
            }
            else if (it->second._equipType == 2)
            {
                socketIdx = _shoseSocket;
                _shoseSocket = it->second._uniqueId;
            }
            int32 invenPosition = it->second._position;
            it->second._position = -1;
            if (socketIdx > 0)
            {
                auto EquippedIt = _inventoryEquipItemList.find(socketIdx);
                if (EquippedIt != _inventoryEquipItemList.end())
                {
                    if (EquippedIt->second._equipType == it->second._equipType)
                    {
                        EquippedIt->second._isEquip = 0;
                        EquippedIt->second._position = invenPosition;
                        return EquippedIt->second;
                    }
                }
            }
            else
            {
                _emptyEquipInvenList.emplace(invenPosition);
            }
        }
        else if (equipped == 0)
        {
            // 장착 해제
            if (it->second._equipType == 1)
            {
                _weaponSocket = -1;
            }
            else if (it->second._equipType == 2)
            {
                _shoseSocket = -1;
            }
            int32 idx = _emptyEquipInvenList.top();
            _emptyEquipInvenList.pop();
            it->second._position = idx;
        }
    }
    return _emptyEquip;
}

bool Inventory::CheckEquipped(int32 uniqueId, int32 equipped)
{
    ReadLockGuard writeLock(lock, "inventory");
    auto it = _inventoryEquipItemList.find(uniqueId);
    if (it != _inventoryEquipItemList.end())
    {
        if (it->second._isEquip == equipped)
        {
            return true;
        }
    }
    return false;
}

EquipItem& Inventory::AddItemEquip(EquipItem& equip)
{
    if (equip._position < 0 && equip._isEquip == 0)
    {
        if (_emptyEquipInvenList.size() == 0)
        {
            // 인벤토리 가득참 메일로 전송
            return equip;
        }

        int position = _emptyEquipInvenList.top();
        _emptyEquipInvenList.pop();
        equip._position = position;
    }
    _uniqueNum.fetch_add(1);
    equip._uniqueId = _uniqueNum.load();
    _inventoryEquipItemList.emplace(equip._uniqueId, equip);
    if (equip._isEquip == 1)
    {
        if (equip._equipType == 1)
        {
            _weaponSocket = equip._uniqueId;
        }
        else if (equip._equipType == 2)
        {
            _shoseSocket = equip._uniqueId;
        }
    }
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
    if (etc._position < 0)
    {
        if (_emptyEtcInvenList.size() == 0)
        {
            // 인벤토리 가득참 메일로 전송
            return etc;
        }

        int position = _emptyEtcInvenList.top();
        _emptyEtcInvenList.pop();
        etc._position = position;
    }
    
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
                inventoryDB.SaveInsertEtcDB(_playerCode, item);
            }
        }
        else
        {
            // 기존 업데이트
            if (item._count == 0)
            {
                inventoryDB.SaveDeleteEtcDB(_playerCode, item);
            }
            else
            {
                inventoryDB.SaveUpdateEtcDB(_playerCode, item);
            }
        }
    }

    SessionDB sessionDB;
    sessionDB.SavePlayerDB(_playerCode, _gold);
}
