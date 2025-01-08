#include "InventoryDB.h"

InventoryDB::InventoryDB() : _dbOdbc(8), _equipItem(EquipItem::EmptyEquipItem()), _etcItem(EtcItem::EmptyEtcItem())
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

    _dbOdbc.BindColChar(sizeof(_equipItem._uuid), &_equipItem._uuid);
    _dbOdbc.BindColInt(sizeof(_equipItem._itemCode), &_equipItem._itemCode);
    _dbOdbc.BindColInt(sizeof(_equipItem._equipType), &_equipItem._equipType);
    _dbOdbc.BindColInt(sizeof(_equipItem._attack), &_equipItem._attack);
    _dbOdbc.BindColInt(sizeof(_equipItem._speed), &_equipItem._speed);
    _dbOdbc.BindColInt(sizeof(_equipItem._equipPos), &_equipItem._equipPos);
    _dbOdbc.BindColInt(sizeof(_equipItem._invenPos), &_equipItem._invenPos);
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
    _dbOdbc.BindColInt(sizeof(_etcItem._invenPos), &_etcItem._invenPos);
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
    _dbOdbc.BindParamChar(36, &item._uuid);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    _dbOdbc.BindParamInt(&item._equipType);
    _dbOdbc.BindParamInt(&item._attack);
    _dbOdbc.BindParamInt(&item._speed);
    _dbOdbc.BindParamInt(&item._equipPos);
    _dbOdbc.BindParamInt(&item._invenPos);
    conn->Exec(insertEquipQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveDeleteEquipDBAll(int32 playerCode)
{
    conn->Prepare(deleteEquipAllQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(deleteEquipAllQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveInsertEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(insertEtcQuery);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    _dbOdbc.BindParamInt(&item._type);
    _dbOdbc.BindParamInt(&item._count);
    _dbOdbc.BindParamInt(&item._invenPos);
    conn->Exec(insertEtcQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveUpdateEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(updateEtcQuery);
    _dbOdbc.BindParamInt(&item._count);
    _dbOdbc.BindParamInt(&item._invenPos);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    conn->Exec(updateEtcQuery);

    _dbOdbc.ReSetIdx();
}

void InventoryDB::SaveDeleteEtcDBAll(int32 playerCode)
{
    conn->Prepare(deletEtcAllQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(deletEtcAllQuery);

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
