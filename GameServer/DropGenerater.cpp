#include "DropGenerater.h"

#include "GameDrop.h"
#include "GameGlobal.h"
#include "GameItem.h"
#include "GameUtils.h"

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
        UUID uuid;
        while (!GameUtils::Utils::GenUUID(&uuid)) { }

        equipItem.GetItemCode();
        auto itemInfo = GEquipItem->GetItem(equipItem.GetItemCode());
        _equipItemList.emplace_back(EquipItem(uuid, equipItem.GetItemCode(), itemInfo->GetType(), itemInfo->GetAttack(), itemInfo->GetSpeed(), 0, -1));
    }
}

void DropGenSystem::InitEtc()
{
    _etcItemList.clear();
    for (auto& etcItem : GDropItem->GetMonsterDropList(_monsterCode))
    {
        auto itemInfo = GEtcItem->GetItem(etcItem.GetItemCode());
        _etcItemList.emplace_back(EtcItem(etcItem.GetItemCode(), itemInfo->GetType(), etcItem.GetCnt(), -1, true));
    }
}
