#pragma once
#include "Inventory.h"
#include <random>
#include "Types.h"

static std::mt19937_64 drop_rng{};

// 몬스터 드롭아이템 제너레이터
class DropGenSystem
{
public:
    DropGenSystem(int32 monsterCode);
    ~DropGenSystem();

    void InitGen();
    void InitEquip();
    void InitEtc();

    Vector<EquipItem>& GetDropEquipList() { return _equipItemList; }
    Vector<EtcItem>& GetDropEtcList() { return _etcItemList; }
    int32 GetDropGold() { return _gold; }

private:
    std::uniform_int_distribution<> genEquip;
    //Atomic<int32> _equipUniqueId{0};
    int32 _monsterCode = -1;
    Vector<EquipItem> _equipItemList;
    Vector<EtcItem> _etcItemList;
    int32 _gold;
};
