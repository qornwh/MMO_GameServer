#pragma once
#include "DBOrm.h"
#include "Inventory.h"

class InventoryDB
{
public:
    InventoryDB();
    ~InventoryDB();

    void LoadEquipDB(int32 playerCode);
    bool GetEquipItem(EquipItem& item);
    void LoadEtcDB(int32 playerCode);
    bool GetEtcItem(EtcItem& item);

    void SaveInsertEquipDB(int32 playerCode, EquipItem& item);
    void SaveDeleteEquipDB(int32 playerCode);
    void SaveInsertEtcDB(int32 playerCode, int32 itemCode, int32 type, int32 count);
    void SaveUpdateEtcDB(int32 playerCode, int32 itemCode, int32 count);
    void SaveDeleteEtcDB(int32 playerCode, int32 itemCode);

private:
    DBOrm _dbOrm;
    DBConnRef conn = nullptr;
    
    wchar_t insertEquipQuery[110] = L"INSERT INTO InventoryEquip (playerCode, itemCode, equipType, attack, speed, isEquip) VALUES (?,?,?,?,?,?)";
    wchar_t deleteEquipQuery[150] = L"DELETE FROM InventoryEquip WHERE playerCode = ?";
    wchar_t insertEtcQuery[100] = L"INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount) VALUES (?,?,?,?)";
    wchar_t updateEtcQuery[100] = L"UPDATE InventoryEtc SET itemCount = ? WHERE playerCode = ? AND itemCode = ?";
    wchar_t deletEtcQuery[100] = L"DELETE FROM InventoryEtc WHERE playerCode = ? AND itemCode = ?";
    
    EquipItem _equipItem;
    EtcItem _etcItem;
};
