#include "Inventory.h"
#include "DBConnectPool.h"
#include "InventoryDB.h"
#include "SessionDB.h"

EquipItem::EquipItem(UUID& uuid, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 equipPos, int32 invenPos) :
    _itemCode(itemCode),
    _equipType(equipType),
    _attack(attack),
    _speed(speed),
    _equipPos(equipPos),
    _invenPos(invenPos)
{
    GameUtils::Utils::UUIDToArray(&uuid, reinterpret_cast<char*>(_uuid));
}

EquipItem::EquipItem(BYTE* uuid, int32 itemCode, int32 equipType, int32 attack, int32 speed, int32 equipPos, int32 invenPos) :
    _itemCode(itemCode),
    _equipType(equipType),
    _attack(attack),
    _speed(speed),
    _equipPos(equipPos),
    _invenPos(invenPos)
{
    if (uuid != nullptr)
        memcpy(_uuid, uuid, UUIDSIZE);
    else
        ZeroMemory(_uuid, UUIDSIZE);
}

EquipItem::EquipItem(const EquipItem& other)
{
    _itemCode = other._itemCode;
    _equipType = other._equipType;
    _attack = other._attack;
    _speed = other._speed;
    _equipPos = other._equipPos;
    _invenPos = other._invenPos;

    if (other._uuid != nullptr)
        memcpy(_uuid, other._uuid, UUIDSIZE);
    else
        ZeroMemory(_uuid, UUIDSIZE);
}

EquipItem::EquipItem(EquipItem&& other) noexcept
{
    _itemCode = other._itemCode;
    _equipType = other._equipType;
    _attack = other._attack;
    _speed = other._speed;
    _equipPos = other._equipPos;
    _invenPos = other._invenPos;

    if (other._uuid != nullptr)
        memcpy(_uuid, other._uuid, UUIDSIZE);
    else
        ZeroMemory(_uuid, UUIDSIZE);
}

EquipItem::~EquipItem()
{
}

EquipItem& EquipItem::operator=(const EquipItem& other)
{
    if (other._uuid != nullptr)
        memcpy(_uuid, other._uuid, UUIDSIZE);
    else
        ZeroMemory(_uuid, UUIDSIZE);
    _itemCode = other._itemCode;
    _equipType = other._equipType;
    _attack = other._attack;
    _speed = other._speed;
    _equipPos = other._equipPos;
    _invenPos = other._invenPos;
    return *this;
}

EquipItem& EquipItem::operator=(EquipItem&& other) noexcept
{
    if (other._uuid != nullptr)
        memcpy(_uuid, other._uuid, UUIDSIZE);
    else
        ZeroMemory(_uuid, UUIDSIZE);
    _itemCode = other._itemCode;
    _equipType = other._equipType;
    _attack = other._attack;
    _speed = other._speed;
    _equipPos = other._equipPos;
    _invenPos = other._invenPos;
    return *this;
}

EquipItem EquipItem::EmptyEquipItem()
{
    return EquipItem{nullptr, -1, -1, -1, -1, -1, -1};
}

bool EquipItem::IsEmpty() const
{
    if (_itemCode == -1)
        return true;
    return false;
}

EtcItem::EtcItem(int32 itemCode, int32 type, int32 count, int32 position, bool isNew): _itemCode(itemCode), _count(count), _type(type), _invenPos(position),
                                                                                       _isNew(isNew)
{
}

EtcItem::EtcItem(const EtcItem& other)
{
    _itemCode = other._itemCode;
    _count = other._count;
    _type = other._type;
    _invenPos = other._invenPos;
    _isNew = other._isNew;
}

EtcItem::EtcItem(EtcItem&& other) noexcept
{
    _itemCode = other._itemCode;
    _count = other._count;
    _type = other._type;
    _invenPos = other._invenPos;
    _isNew = other._isNew;
}

EtcItem::~EtcItem()
{
}

EtcItem& EtcItem::operator=(const EtcItem& other)
{
    _itemCode = other._itemCode;
    _count = other._count;
    _type = other._type;
    _invenPos = other._invenPos;
    _isNew = other._isNew;
    return *this;
}

EtcItem& EtcItem::operator=(EtcItem&& other) noexcept
{
    _itemCode = other._itemCode;
    _count = other._count;
    _type = other._type;
    _invenPos = other._invenPos;
    _isNew = other._isNew;
    return *this;
}

EtcItem EtcItem::EmptyEtcItem()
{
    return EtcItem{-1, -1, -1, -1};
}

bool EtcItem::IsEmpty() const
{
    if (_itemCode == -1)
        return true;
    return false;
}

void EtcItem::UpdateItem(int32 count)
{
    _count = count;
}

Inventory::Inventory() : _emptyEquip(EquipItem::EmptyEquipItem()), _emptyEtc(EtcItem::EmptyEtcItem())
{
    _inventoryEquipItemList = Vector<EquipItem>(_invenSize, _emptyEquip);
    _inventoryEtcItemList = Vector<EtcItem>(_invenSize, _emptyEtc);
    _equippedItemList = Vector<EquipItem>(_equipSize, _emptyEquip);
    _updateEquipList.reserve(20);
    _updateEtcList.reserve(20);
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
        if (equip._invenPos >= 0)
        {
            _inventoryEquipItemList[equip._invenPos] = equip;
            notEmptyEquipInven[equip._invenPos] = 1;
        }
        else if (equip._equipPos > 0)
        {
            _equippedItemList[equip._equipPos] = equip;
        }
    }

    Vector<int32> notEmptyEtcInven(_invenSize, 0);
    inventoryDB.LoadEtcDB(_playerCode);
    EtcItem etc = EtcItem::EmptyEtcItem();
    while (inventoryDB.GetEtcItem(etc))
    {
        if (etc._invenPos >= 0)
        {
            _inventoryEtcItemList[etc._invenPos] = etc;
            notEmptyEtcInven[etc._invenPos] = 1;
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

bool Inventory::CheckItemEquip(int32 invenPos)
{
    const EquipItem& item = _inventoryEquipItemList[invenPos];
    if (!item.IsEmpty())
    {
        return true;
    }
    return false;
}

bool Inventory::UseItemEquip(int32 invenPos)
{
    WriteLockGuard writeLock(lock);
    if (CheckItemEquip(invenPos))
    {
        EquipItem& item = _inventoryEquipItemList[invenPos];
        _emptyEquipInvenList.emplace(invenPos);
        item.EmptyEquipItem();
        return true;
    }
    return false;
}

bool Inventory::ItemEquipped(int32& invenPos, int32& equipPos)
{
    WriteLockGuard writeLock(lock);

    if (invenPos < 0)
    {
        if (_emptyEquipInvenList.empty())
            return false;

        invenPos = _emptyEquipInvenList.top();
        _emptyEquipInvenList.pop();
    }

    EquipItem& invenItem = _inventoryEquipItemList[invenPos];
    EquipItem& equipItem = _equippedItemList[equipPos];

    if (invenItem.IsEmpty() && equipItem.IsEmpty())
        return false;

    if (equipItem.IsEmpty())
    {
        _emptyEquipInvenList.push(invenPos);
    }

    EquipItem tempItem = invenItem;
    invenItem = equipItem;
    equipItem = tempItem;
    if (!invenItem.IsEmpty())
    {
        invenItem._invenPos = invenPos;
        invenItem._equipPos = -1;
    }
    if (!equipItem.IsEmpty())
    {
        equipItem._invenPos = -1;
        equipItem._equipPos = equipPos;
    }
    return true;
}

bool Inventory::CheckEquipped(int32 invenPos)
{
    ReadLockGuard readLock(lock);
    if (CheckItemEquip(invenPos))
    {
        // 인벤토리에 있는경우만 성공
        EquipItem& item = _inventoryEquipItemList[invenPos];
        if (item._invenPos >= 0)
            return true; 
    }
    return false;
}

EquipItem& Inventory::AddItemEquip(EquipItem& equip)
{
    WriteLockGuard writeLock(lock);
    // 드롭및 수령받을때
    if (_emptyEquipInvenList.size() > 0)
    {
        int position = _emptyEquipInvenList.top();
        _emptyEquipInvenList.pop();
        equip._invenPos = position;
        _inventoryEquipItemList[position] = equip;
    }
    return equip;
}

bool Inventory::CheckItemEtc(int32 invenPos, int32 count)
{
    ReadLockGuard readLock(lock);
    const EtcItem& item = _inventoryEtcItemList[invenPos];
    if (!item.IsEmpty())
    {
        if (item._count >= count)
            return true;
    }
    return false;
}

bool Inventory::UseItemEtc(int32 invenPos, int32 count)
{
    WriteLockGuard writeLock(lock);
    if (CheckItemEtc(invenPos, count))
    {
        EtcItem& item = _inventoryEtcItemList[invenPos];
        item._count -= count;
        if (item._count == 0)
        {
            item.EmptyEtcItem();
            _emptyEtcInvenList.emplace(invenPos);
        }
        return true;
    }
    return false;
}

EtcItem& Inventory::AddItemEtc(EtcItem& etc)
{
    WriteLockGuard writeLock(lock);
    // 드롭및 수령받을때
    if (_emptyEtcInvenList.size() > 0)
    {
        for (auto& invenItem : _inventoryEtcItemList)
        {
            if (invenItem._itemCode == etc._itemCode)
            {
                invenItem._count += etc._count;
                return etc;
            }
        }

        int position = _emptyEtcInvenList.top();
        _emptyEtcInvenList.pop();
        etc._invenPos = position;
    }
    return etc;
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

    inventoryDB.SaveDeleteEquipDBAll(_playerCode);
    for (auto& item : _inventoryEquipItemList)
    {
        if (!item.IsEmpty())
        {
            inventoryDB.SaveInsertEquipDB(_playerCode, item);
        }
    }

    for (auto& item : _equippedItemList)
    {
        if (!item.IsEmpty())
        {
            inventoryDB.SaveInsertEquipDB(_playerCode, item);
        }
    }

    inventoryDB.SaveDeleteEtcDBAll(_playerCode);
    for (auto& item : _inventoryEtcItemList)
    {
        if (!item.IsEmpty())
        {
            inventoryDB.SaveInsertEtcDB(_playerCode, item);
        }
    }

    SessionDB sdb;
    sdb.SavePlayerDB(_playerCode, _gold);
}

void Inventory::ResetUpdateItems()
{
    _updateEquipList.clear();
    _updateEtcList.clear();
}

bool Inventory::AddMailItemEquip(EquipItem& equip)
{
    if (_emptyEquipInvenList.size() == 0)
    {
        return false;
    }

    equip._invenPos = -1;
    _updateEquipList.emplace_back(AddItemEquip(equip));
    return true;
}

bool Inventory::AddMailItemEtc(EtcItem& etc)
{
    if (_emptyEtcInvenList.size() == 0)
    {
        return false;
    }

    etc._invenPos = -1;
    _updateEtcList.emplace_back(AddItemEtc(etc));
    return true;
}
