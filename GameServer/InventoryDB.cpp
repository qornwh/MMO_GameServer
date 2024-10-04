#include "InventoryDB.h"

InventoryDB::InventoryDB() : _dbOdbc(7), _equipItem(EquipItem::EmptyEquipItem()), _etcItem(EtcItem::EmptyEtcItem())
{
    conn = GDBPool->Pop();
    _dbOdbc.SetDBConn(conn);
}

InventoryDB::~InventoryDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void InventoryDB::LoadEquipDB(int32 playerCode)
{
    bool result = conn->Prepare(selectEquipItemQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(selectEquipItemQuery);

    _dbOdbc.BindColInt(sizeof(_equipItem._itemCode), &_equipItem._itemCode);
    _dbOdbc.BindColInt(sizeof(_equipItem._equipType), &_equipItem._equipType);
    _dbOdbc.BindColInt(sizeof(_equipItem._attack), &_equipItem._attack);
    _dbOdbc.BindColInt(sizeof(_equipItem._speed), &_equipItem._speed);
    _dbOdbc.BindColInt(sizeof(_equipItem._isEquip), &_equipItem._isEquip);
    _dbOdbc.BindColInt(sizeof(_equipItem._position), &_equipItem._position);
}

bool InventoryDB::GetEquipItem(EquipItem& item)
{
    if (conn->Fetch())
    {
        item = _equipItem;
        return true;
    }
    
    conn->CloseCursor();
    _dbOdbc.ReSetIdx();
    return false;
}

void InventoryDB::LoadEtcDB(int32 playerCode)
{
    conn->Prepare(selectEtcItemQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(selectEtcItemQuery);

    _dbOdbc.BindColInt(sizeof(_etcItem._itemCode), &_etcItem._itemCode);
    _dbOdbc.BindColInt(sizeof(_etcItem._type), &_etcItem._type);
    _dbOdbc.BindColInt(sizeof(_etcItem._count), &_etcItem._count);
    _dbOdbc.BindColInt(sizeof(_etcItem._position), &_etcItem._position);
}

bool InventoryDB::GetEtcItem(EtcItem& item)
{
    if (conn->Fetch())
    {
        item = _etcItem;
        return true;
    }
    
    conn->CloseCursor();
    _dbOdbc.ReSetIdx();
    return false;
}

void InventoryDB::SaveInsertEquipDB(int32 playerCode, EquipItem& item)
{
    conn->Prepare(insertEquipQuery);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    _dbOdbc.BindParamInt(&item._equipType);
    _dbOdbc.BindParamInt(&item._attack);
    _dbOdbc.BindParamInt(&item._speed);
    _dbOdbc.BindParamInt(&item._isEquip);
    _dbOdbc.BindParamInt(&item._position);
    conn->Exec(insertEquipQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveDeleteEquipDB(int32 playerCode)
{
    conn->Prepare(deleteEquipQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(deleteEquipQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveInsertEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(insertEtcQuery);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    _dbOdbc.BindParamInt(&item._type);
    _dbOdbc.BindParamInt(&item._count);
    _dbOdbc.BindParamInt(&item._position);
    conn->Exec(insertEtcQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveUpdateEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(updateEtcQuery);
    _dbOdbc.BindParamInt(&item._count);
    _dbOdbc.BindParamInt(&item._position);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    conn->Exec(updateEtcQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveDeleteEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(deletEtcQuery);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    conn->Exec(deletEtcQuery);

    _dbOdbc.ReSetIdx();
}
