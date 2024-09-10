#include "DropGenerater.h"

#include "GameDrop.h"
#include "GameGlobal.h"
#include "GameItem.h"

DropGenSystem::DropGenSystem(int32 monsterCode) : genEquip(0, 10), _monsterCode(monsterCode)
{
}

DropGenSystem::~DropGenSystem()
{
}

void DropGenSystem::InitGen()
{
    InitEquip();
    InitEtc();
    _gold = GDropItem->GetMonsterGold(_monsterCode);
}

void DropGenSystem::InitEquip()
{
    _equipItemList.clear();
    for (auto& equipItem : GDropItem->GetMonsterDropEquipList(_monsterCode))
    {
        // if (genEquip(drop_rng) > 4)
        // {
            _equipUniqueId.fetch_add(1);
            equipItem.GetItemCode();
            auto itemInfo = GEquipItem->GetItem(equipItem.GetItemCode());
            _equipItemList.emplace(_equipUniqueId.load(),
                           EquipItem(_equipUniqueId.load(), equipItem.GetItemCode(), itemInfo->GetType(), itemInfo->GetAttack(), itemInfo->GetSpeed(), 0, 0));
        // }
    }
}

void DropGenSystem::InitEtc()
{
    _etcItemList.clear();
    for (auto& etcItem : GDropItem->GetMonsterDropList(_monsterCode))
    {
        auto itemInfo = GEtcItem->GetItem(etcItem.GetItemCode());
        _etcItemList.emplace(etcItem.GetItemCode(), EtcItem(etcItem.GetItemCode(), itemInfo->GetType(), etcItem.GetCnt(), true));
    }
}
