#pragma once
#include "DBOdbc.h"
#include "Inventory.h"

static const wchar_t* selectEquipItemQuery = L"SELECT uniqueId, itemCode, equipType, attack, speed, equipPos, invenPos FROM InventoryEquip WHERE playerCode = ?";
static const wchar_t* selectEtcItemQuery = L"SELECT itemCode, itemType, itemCount, invenPos FROM InventoryEtc WHERE playerCode = ?";
    
static const wchar_t* insertEquipQuery = L"INSERT INTO InventoryEquip (uniqueId, playerCode, itemCode, equipType, attack, speed, equipPos, invenPos) VALUES (?,?,?,?,?,?,?,?)";
static const wchar_t* deleteEquipAllQuery = L"DELETE FROM InventoryEquip WHERE playerCode = ?";
static const wchar_t* insertEtcQuery = L"INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount, invenPos) VALUES (?,?,?,?,?)";
static const wchar_t* updateEtcQuery = L"UPDATE InventoryEtc SET itemCount = ?, invenPos = ? WHERE playerCode = ? AND itemCode = ?";
static const wchar_t* deletEtcAllQuery = L"DELETE FROM InventoryEtc WHERE playerCode = ?";
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
    void SaveDeleteEquipDBAll(int32 playerCode);
    void SaveInsertEtcDB(int32 playerCode, EtcItem& item);
    void SaveUpdateEtcDB(int32 playerCode, EtcItem& item);
    void SaveDeleteEtcDBAll(int32 playerCode);
    void SaveDeleteEtcDB(int32 playerCode, EtcItem& item);

private:
    DBOdbc _dbOdbc;
    DBConnRef conn = nullptr;
    
    EquipItem _equipItem;
    EtcItem _etcItem;
};
