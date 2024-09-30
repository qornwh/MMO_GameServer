#include "InventoryDB.h"

InventoryDB::InventoryDB() : _dbOrm(7), _equipItem(EquipItem::EmptyEquipItem()), _etcItem(EtcItem::EmptyEtcItem())
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
    bool result = conn->Prepare(selectEquipItemQuery);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(selectEquipItemQuery);

    _dbOrm.BindColInt(sizeof(_equipItem._itemCode), &_equipItem._itemCode);
    _dbOrm.BindColInt(sizeof(_equipItem._equipType), &_equipItem._equipType);
    _dbOrm.BindColInt(sizeof(_equipItem._attack), &_equipItem._attack);
    _dbOrm.BindColInt(sizeof(_equipItem._speed), &_equipItem._speed);
    _dbOrm.BindColInt(sizeof(_equipItem._isEquip), &_equipItem._isEquip);
    _dbOrm.BindColInt(sizeof(_equipItem._position), &_equipItem._position);
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
    conn->Prepare(selectEtcItemQuery);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(selectEtcItemQuery);

    _dbOrm.BindColInt(sizeof(_etcItem._itemCode), &_etcItem._itemCode);
    _dbOrm.BindColInt(sizeof(_etcItem._type), &_etcItem._type);
    _dbOrm.BindColInt(sizeof(_etcItem._count), &_etcItem._count);
    _dbOrm.BindColInt(sizeof(_etcItem._position), &_etcItem._position);
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
    conn->Prepare(insertEquipQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&item._itemCode);
    _dbOrm.BindParamInt(&item._equipType);
    _dbOrm.BindParamInt(&item._attack);
    _dbOrm.BindParamInt(&item._speed);
    _dbOrm.BindParamInt(&item._isEquip);
    _dbOrm.BindParamInt(&item._position);
    conn->Exec(insertEquipQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveDeleteEquipDB(int32 playerCode)
{
    conn->Prepare(deleteEquipQuery);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(deleteEquipQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveInsertEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(insertEtcQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&item._itemCode);
    _dbOrm.BindParamInt(&item._type);
    _dbOrm.BindParamInt(&item._count);
    _dbOrm.BindParamInt(&item._position);
    conn->Exec(insertEtcQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveUpdateEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(updateEtcQuery);
    _dbOrm.BindParamInt(&item._count);
    _dbOrm.BindParamInt(&item._position);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&item._itemCode);
    conn->Exec(updateEtcQuery);

    _dbOrm.ReSetIdx();
}

void InventoryDB::SaveDeleteEtcDB(int32 playerCode, EtcItem& item)
{
    conn->Prepare(deletEtcQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&item._itemCode);
    conn->Exec(deletEtcQuery);

    _dbOrm.ReSetIdx();
}
