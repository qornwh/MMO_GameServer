#include "DropItem.h"

DropItem::DropItem(int32 itemCode, int32 cnt) : _itemCode(itemCode), _cnt(cnt)
{
}

DropItem::~DropItem()
{
}

DropEquipItem::DropEquipItem(int32 itemCode) : _itemCode(itemCode)
{
}

DropEquipItem::~DropEquipItem()
{
}
