#pragma once
#include "DBOrm.h"
#include "Inventory.h"

static const wchar_t* selectEquipItemQuery = L"SELECT itemCode, equipType, attack, speed, isEquip, position FROM InventoryEquip WHERE playerCode = ?";
static const wchar_t* selectEtcItemQuery = L"SELECT itemCode, itemType, itemCount, position FROM InventoryEtc WHERE playerCode = ?";
    
static const wchar_t* insertEquipQuery = L"INSERT INTO InventoryEquip (playerCode, itemCode, equipType, attack, speed, isEquip, position) VALUES (?,?,?,?,?,?,?)";
static const wchar_t* deleteEquipQuery = L"DELETE FROM InventoryEquip WHERE playerCode = ?";
static const wchar_t* insertEtcQuery = L"INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount, position) VALUES (?,?,?,?,?)";
static const wchar_t* updateEtcQuery = L"UPDATE InventoryEtc SET itemCount = ?, position = ? WHERE playerCode = ? AND itemCode = ?";
static const wchar_t* deletEtcQuery = L"DELETE FROM InventoryEtc WHERE playerCode = ? AND itemCode = ?";

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
    void SaveInsertEtcDB(int32 playerCode, EtcItem& item);
    void SaveUpdateEtcDB(int32 playerCode, EtcItem& item);
    void SaveDeleteEtcDB(int32 playerCode, EtcItem& item);

private:
    DBOrm _dbOrm;
    DBConnRef conn = nullptr;
    
    EquipItem _equipItem;
    EtcItem _etcItem;
};
