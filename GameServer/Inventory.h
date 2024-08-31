#pragma once
#include "InventoryItem.h"
#include "pch.h"
#include "ReadWriteLock.h"

class Inventory
{
public:
    Inventory();
    ~Inventory();

    void Init(int32 playerCode, int32 gold); // 초기 DB 데이터 로드
    Map<int32, Vector<InventoryEquipItem>>& GetEquipItemInfo(); // 아이템 정보 들고오기
    Map<int32, InventoryEtcItem>& GetEtcItemInfo(); // 아이템 정보 들고오기
    bool CheckItemEquip(int32 code); // 아이템, 머니 있는지 확인
    bool UseItemEquip(int32 itemCode);
    void AddItemEquip(int32 itemCode);
    bool CheckItemEtc(int32 code, int32 count = 1); // 아이템, 머니 있는지 확인
    bool UseItemEtc(int32 itemCode, int32 count = 1);
    void AddItemEtc(int32 itemCode, int32 count);
    bool CheckGold(int32 gold); // 아이템, 머니 있는지 확인
    bool UseGold(int32 gold);
    void AddGold(int32 gold);
    void SaveDB();
    int32 GetGold() { return _gold; }

private:
    Lock lock;
    int32 _gold = 0;
    int32 _playerCode = -1;
    std::weak_ptr<GamePlayerInfo> _playerInfo;
    Map<int32, Vector<InventoryEquipItem>> _inventoryEquipItemList;
    Map<int32, InventoryEtcItem> _inventoryEtcItemList;
};
