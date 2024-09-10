#include "InventoryDB.h"

InventoryDB::InventoryDB() : _dbOrm(6), _equipItem(0, 0, 0, 0, 0, 0, 0), _etcItem(0, 0, 0)
{
    conn = GDBPool->Pop();
    _dbOrm.SetDBConn(conn);
}

InventoryDB::~InventoryDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void InventoryDB::LoadEquipDB(int32 playerCode)
{
    const wchar_t* query = L"SELECT itemCode, equipType, attack, speed, isEquip FROM InventoryEquip WHERE playerCode = ?";
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(query);

    _dbOrm.BindColInt(sizeof(_equipItem._itemCode), &_equipItem._itemCode);
    _dbOrm.BindColInt(sizeof(_equipItem._equipType), &_equipItem._equipType);
    _dbOrm.BindColInt(sizeof(_equipItem._attack), &_equipItem._attack);
    _dbOrm.BindColInt(sizeof(_equipItem._speed), &_equipItem._speed);
    _dbOrm.BindColInt(sizeof(_equipItem._isEquip), &_equipItem._isEquip);
}

bool InventoryDB::GetEquipItem(EquipItem& item)
{
    if (conn->Fetch())
    {
        item = _equipItem;
        return true;
    }
    
    conn->CloseCursor();
    _dbOrm.ReSetIdx();
    return false;
}

void InventoryDB::LoadEtcDB(int32 playerCode)
{
    const wchar_t* query = L"SELECT itemCode, itemType, itemCount FROM InventoryEtc WHERE playerCode = ?";
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&playerCode);
    result = conn->Exec(query);

    _dbOrm.BindColInt(sizeof(_etcItem._itemCode), &_etcItem._itemCode);
    _dbOrm.BindColInt(sizeof(_etcItem._type), &_etcItem._type);
    _dbOrm.BindColInt(sizeof(_etcItem._count), &_etcItem._count);
}

bool InventoryDB::GetEtcItem(EtcItem& item)
{
    if (conn->Fetch())
    {
        item = _etcItem;
        return true;
    }
    
    conn->CloseCursor();
    _dbOrm.ReSetIdx();
    return false;
}

void InventoryDB::SaveInsertEquipDB(int32 playerCode, EquipItem& item)
{
    bool result = conn->Prepare(insertEquipQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&item._itemCode);
    _dbOrm.BindParamInt(&item._equipType);
    _dbOrm.BindParamInt(&item._attack);
    _dbOrm.BindParamInt(&item._speed);
    _dbOrm.BindParamInt(&item._isEquip);
    result = conn->Exec(insertEquipQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveDeleteEquipDB(int32 playerCode)
{
    bool result = conn->Prepare(deleteEquipQuery);
    _dbOrm.BindParamInt(&playerCode);
    result = conn->Exec(deleteEquipQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveInsertEtcDB(int32 playerCode, int32 itemCode, int32 type, int32 count)
{
    bool result = conn->Prepare(insertEtcQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&itemCode);
    _dbOrm.BindParamInt(&type);
    _dbOrm.BindParamInt(&count);
    result = conn->Exec(insertEtcQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveUpdateEtcDB(int32 playerCode, int32 itemCode, int32 count)
{
    bool result = conn->Prepare(updateEtcQuery);
    _dbOrm.BindParamInt(&count);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&itemCode);
    result = conn->Exec(updateEtcQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveDeleteEtcDB(int32 playerCode, int32 itemCode)
{
    bool result = conn->Prepare(deletEtcQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&itemCode);
    result = conn->Exec(deletEtcQuery);

    _dbOrm.ReSetIdx();
}
