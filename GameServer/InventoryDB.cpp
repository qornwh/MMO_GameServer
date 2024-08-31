#include "InventoryDB.h"

InventoryDB::InventoryDB() : _dbOrm(4)
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
    const wchar_t* query = L"SELECT itemCode, itemType, itemUse FROM InventoryEquip WHERE playerCode = ?";
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(query);

    _dbOrm.BindColInt(sizeof(_itemCode), &_itemCode);
    _dbOrm.BindColInt(sizeof(_type), &_type);
    _dbOrm.BindColInt(sizeof(_use), &_use);
}

bool InventoryDB::GetEquipItem(int32& itemCode, int32& type, int32& use)
{
    if(conn->Fetch())
    {
        itemCode = _itemCode;
        type = _type;
        use = _use;
        return true;
    }
    else
    {
        ResetDBOrm();
        return false;
    }
}

void InventoryDB::LoadEtcDB(int32 playerCode)
{
    const wchar_t* query = L"SELECT itemCode, itemType, itemCount FROM InventoryEtc WHERE playerCode = ?";
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&playerCode);
    result = conn->Exec(query);

    _dbOrm.BindColInt(sizeof(_itemCode), &_itemCode);
    _dbOrm.BindColInt(sizeof(_type), &_type);
    _dbOrm.BindColInt(sizeof(_count), &_count);
}

bool InventoryDB::GetEtcItem(int32& itemCode, int32& type, int32& count)
{
    if(conn->Fetch())
    {
        itemCode = _itemCode;
        type = _type;
        count = _count;
        return true;
    }
    else
    {
        ResetDBOrm();
        return false;
    }
}

void InventoryDB::SaveInsertEquipDB(int32 playerCode, int32 itemCode, int32 type)
{
    bool result = conn->Prepare(insertEquipQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&itemCode);
    _dbOrm.BindParamInt(&type);
    result = conn->Exec(insertEquipQuery);
    
    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveUpdateEquipDB(int32 playerCode, int32 itemCode)
{
    bool result = conn->Prepare(updateEquipQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&itemCode);
    result = conn->Exec(updateEquipQuery);
    
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

void InventoryDB::ResetDBOrm()
{
    conn->CloseCursor();
    conn->EndTran();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();
}
