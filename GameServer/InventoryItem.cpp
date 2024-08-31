#include "InventoryItem.h"

InventoryEquipItem::InventoryEquipItem(int32 itemCode, int32 type, bool use, InventoryItemState state): _itemCode(itemCode), _type(type), _use(use), _state(state)
{
}

InventoryEquipItem::InventoryEquipItem(): _itemCode(0), _type(0), _state(InventoryItemState::NONE)
{
}

InventoryEquipItem::~InventoryEquipItem()
{
}

void InventoryEquipItem::UpdateItem(bool use)
{
    if (_state != InventoryItemState::INSERT)
    {
        _state = InventoryItemState::UPDATE;
    }
    else
    {
        if (use == 1)
        {
            _state = InventoryItemState::EMPTY;
        }
    }
    _use = use;
}

InventoryEtcItem::InventoryEtcItem(int32 itemCode, int32 type, int32 count, InventoryItemState state): _itemCode(itemCode), _count(count), _type(type), _state(state)
{
}

InventoryEtcItem::InventoryEtcItem(): _itemCode(0), _count(0), _type(0), _state(InventoryItemState::NONE)
{
}

InventoryEtcItem::~InventoryEtcItem()
{
}

void InventoryEtcItem::UpdateItem(int32 count)
{
    if (_state != InventoryItemState::INSERT)
    {
        _state = InventoryItemState::UPDATE;
    }
    _count = count;
}
