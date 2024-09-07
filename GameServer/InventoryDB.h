#pragma once
#include "DBOrm.h"

class InventoryDB
{
public:
    InventoryDB();
    ~InventoryDB();

    void LoadEquipDB(int32 playerCode);
    bool GetEquipItem(int32& itemCode, int32& type, int32& use);
    void LoadEtcDB(int32 playerCode);
    bool GetEtcItem(int32& itemCode, int32& type, int32& count);

    void SaveInsertEquipDB(int32 playerCode, int32 itemCode, int32 type);
    void SaveUpdateEquipDB(int32 playerCode, int32 itemCode);
    void SaveInsertEtcDB(int32 playerCode, int32 itemCode, int32 type, int32 count);
    void SaveUpdateEtcDB(int32 playerCode, int32 itemCode, int32 count);
    void SaveDeleteEtcDB(int32 playerCode, int32 itemCode);

private:
    DBOrm _dbOrm;
    int32 _itemCode = -1;
    int32 _type = -1;
    int32 _count = -1;
    int32 _use = -1;
    wchar_t insertEquipQuery[100] = L"INSERT INTO InventoryEquip (playerCode, itemCode, itemType, itemUse) VALUES (?,?,?,0)"; // 사용 가능
    wchar_t updateEquipQuery[150] = L"WITH CTE AS ( SELECT TOP 1 * FROM InventoryEquip WHERE playerCode = ? AND itemCode = ? AND itemUse = 0) UPDATE CTE SET itemUse = 1"; // 사용 해제
    wchar_t insertEtcQuery[100] = L"INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount) VALUES (?,?,?,?)";
    wchar_t updateEtcQuery[100] = L"UPDATE InventoryEtc SET itemCount = ? WHERE playerCode = ? AND itemCode = ?";
    wchar_t deletEtcQuery[100] = L"DELETE FROM InventoryEtc WHERE playerCode = ? AND itemCode = ?";

    DBConnRef conn = nullptr;
};
