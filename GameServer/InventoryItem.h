#pragma once
#include "pch.h"

enum InventoryItemState
{
    NONE,
    UPDATE,
    INSERT,
    EMPTY
};

class InventoryEquipItem
{
public:
    InventoryEquipItem(int32 itemCode, int32 type, bool use, InventoryItemState state = InventoryItemState::NONE);
    InventoryEquipItem();
    ~InventoryEquipItem();

    void UpdateItem(bool use = true);
    InventoryItemState GetState() { return _state; }
    int32 GetCode() { return _itemCode; }
    int32 GetType() { return _type; }
    bool IsUse() { return _use; }

private:
    int32 _itemCode;
    int32 _type;
    bool _use;
    InventoryItemState _state;
};

class InventoryEtcItem
{
public:
    InventoryEtcItem(int32 itemCode, int32 type, int32 count, InventoryItemState state = InventoryItemState::NONE);
    InventoryEtcItem();
    ~InventoryEtcItem();

    void UpdateItem(int32 count = 1);
    InventoryItemState GetState() { return _state; }
    int32 GetCode() { return _itemCode; }
    int32 GetCount() { return _count; }
    int32 GetType() { return _type; }

private:
    int32 _itemCode;
    int32 _count;
    int32 _type;
    InventoryItemState _state;
};
