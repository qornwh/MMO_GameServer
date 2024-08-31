#pragma once
#include "pch.h"

class DropItem : public std::enable_shared_from_this<DropItem>
{
public:
    DropItem(int32 itemCode, int32 cnt);
    ~DropItem();

    int32 GetItemCode() { return _itemCode; }
    int32 GetCnt() { return _cnt; }

private:
    int32 _itemCode;
    int32 _cnt;
};

class DropEquipItem : public std::enable_shared_from_this<DropEquipItem>
{
public:
    DropEquipItem(int32 itemCode);
    ~DropEquipItem();

    int32 GetItemCode() { return _itemCode; }

private:
    int32 _itemCode;
    int32 _cnt;
};
