#include "Inventory.h"
#include "DBConnectPool.h"
#include "GameGlobal.h"
#include "GameItem.h"
#include "InventoryDB.h"
#include "SessionDB.h"

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
    
    int32 itemCode = 0;
    int32 itemType = 0;
    int32 itemCount = 0;
    int32 use = 0;

    while(inventoryDB.GetEquipItem(itemCode, itemType, use))
    {
        if (use == 1) // 이미 사용됬으면 그냥 넘긴다.
            continue;
        if (_inventoryEquipItemList.find(itemCode) == _inventoryEquipItemList.end())
            _inventoryEquipItemList.emplace(itemCode, Vector<InventoryEquipItem>());
        _inventoryEquipItemList[itemCode].emplace_back(InventoryEquipItem(itemCode, itemType, false, InventoryItemState::NONE));
    }

    inventoryDB.LoadEtcDB(_playerCode);
    while(inventoryDB.GetEtcItem(itemCode, itemType, itemCount))
    {
        _inventoryEtcItemList.emplace(itemCode, InventoryEtcItem(itemCode, itemType, itemCount, InventoryItemState::NONE));
    }
    _gold = gold;
}

Map<int32, Vector<InventoryEquipItem>>& Inventory::GetEquipItemInfo()
{
    return _inventoryEquipItemList;
}

Map<int32, InventoryEtcItem>& Inventory::GetEtcItemInfo()
{
    return _inventoryEtcItemList;
}

bool Inventory::CheckItemEquip(int32 code)
{
    if (_inventoryEquipItemList.find(code) != _inventoryEquipItemList.end())
    {
        if (!_inventoryEquipItemList[code].empty())
            return true;
    }
    return false;
}

bool Inventory::UseItemEquip(int32 itemCode)
{
    WriteLockGuard writeLock(lock, "inventory");
    if (CheckItemEquip(itemCode))
    {
        for (auto& item : _inventoryEquipItemList[itemCode])
        {
            if (!item.IsUse())
            {
                item.UpdateItem(true);
                return true;
            }
        }
    }
    return false;
}

void Inventory::AddItemEquip(int32 itemCode)
{
    if (_inventoryEquipItemList.find(itemCode) == _inventoryEquipItemList.end())
    {
        _inventoryEquipItemList.emplace(itemCode, Vector<InventoryEquipItem>());
    }
    int32 type = GEquipItem->GetItem(itemCode)->GetType();
    _inventoryEquipItemList[itemCode].emplace_back(InventoryEquipItem(itemCode, type, false, InventoryItemState::INSERT));
}

bool Inventory::CheckItemEtc(int32 code, int32 count)
{
    if (_inventoryEtcItemList.find(code) != _inventoryEtcItemList.end())
    {
        if (_inventoryEtcItemList[code].GetCount() >= count)
            return true;
    }
    return false;
}

bool Inventory::UseItemEtc(int32 itemCode, int32 count)
{
    WriteLockGuard writeLock(lock, "inventory");
    if (CheckItemEtc(itemCode, count))
    {
        int curCount = _inventoryEtcItemList[itemCode].GetCount();
        _inventoryEtcItemList[itemCode].UpdateItem(curCount - count);
        return true;
    }
    return false;
}

void Inventory::AddItemEtc(int32 itemCode, int32 count)
{
    if (_inventoryEtcItemList.find(itemCode) != _inventoryEtcItemList.end())
    {
        int curCount = _inventoryEtcItemList[itemCode].GetCount();
        _inventoryEtcItemList[itemCode].UpdateItem(curCount + count);
    }
    else
    {
        int32 type = GEtcItem->GetItem(itemCode)->GetType();
        _inventoryEtcItemList.emplace(itemCode, InventoryEtcItem(itemCode, type, count, InventoryItemState::INSERT));
    }
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
        gold += gold;
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

    for (auto& inventoryItem : _inventoryEquipItemList)
    {
        int32 code = inventoryItem.first;
        for (auto& item : inventoryItem.second)
        {
            switch (item.GetState())
            {
            case InventoryItemState::INSERT:
                {
                    inventoryDB.SaveInsertEquipDB(_playerCode, item.GetCode(), item.GetType());
                }
                break;
            case InventoryItemState::UPDATE:
                {
                    inventoryDB.SaveUpdateEquipDB(_playerCode, item.GetCode());
                }
                break;
            default:
                break;
            }
        }
    }
    
    for (auto& inventoryItem : _inventoryEtcItemList)
    {
        InventoryEtcItem& item = inventoryItem.second;

        switch (item.GetState())
        {
        case InventoryItemState::INSERT:
            {
                inventoryDB.SaveInsertEtcDB(_playerCode, item.GetCode(), item.GetType(), item.GetCount());
            }
            break;
        case InventoryItemState::UPDATE:
            {
                if (item.GetCount() == 0)
                {
                    inventoryDB.SaveDeleteEtcDB(_playerCode, item.GetCode());
                }
                else
                {
                    inventoryDB.SaveUpdateEtcDB(_playerCode, item.GetCode(), item.GetCount());
                }
            }
            break;
        default:
            break;
        }
    }

    // 골드 DB기록
    SessionDB sessionDB;
    sessionDB.SavePlayerDB(_playerCode, _gold);
}
