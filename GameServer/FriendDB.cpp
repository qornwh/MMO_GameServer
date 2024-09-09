#include "FriendDB.h"

FriendDB::FriendDB() : _dbOrm(2)
{
    conn = GDBPool->Pop();
    _dbOrm.SetDBConn(conn);
}

FriendDB::~FriendDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void FriendDB::LoadFriend(int32 playerCode)
{
    const wchar_t* query = L"SELECT friendCode FROM Friend WHERE playerCode = ?";
    conn->Prepare(query);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(query);

    _dbOrm.BindColInt(sizeof(_friendCode), &_friendCode);
}

bool FriendDB::GetFriend(int32& friendCode)
{
    if(conn->Fetch())
    {
        friendCode = _friendCode;
        return true;
    }
    else
    {
        conn->CloseCursor();
        _dbOrm.ReSetIdx();
        return false;
    }
}

void FriendDB::InsertFriend(int32 playerCode, int32 friendCode)
{
    const wchar_t* query = L"INSERT INTO Friend (playerCode, friendCode) VALUES (?,?,?,?) = ?";
    conn->Prepare(query);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&friendCode);
    conn->Exec(query);

    _dbOrm.ReSetIdx();
}

void FriendDB::DeleteFriend(int32 playerCode, int32 friendCode)
{
    const wchar_t* query = L"DELETE FROM Friend WHERE playerCode = ? AND friendCode = ?";
    conn->Prepare(query);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&friendCode);
    conn->Exec(query);

    _dbOrm.ReSetIdx();
}
