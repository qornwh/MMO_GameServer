#include "FriendDB.h"

FriendDB::FriendDB() : _dbOdbc(2)
{
    conn = GDBPool->Pop();
    _dbOdbc.SetDBConn(conn);
}

FriendDB::~FriendDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void FriendDB::LoadFriend(int32 playerCode)
{
    conn->Prepare(selectFriendQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(selectFriendQuery);

    _dbOdbc.BindColInt(sizeof(_friendCode), &_friendCode);
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
        _dbOdbc.ReSetIdx();
        return false;
    }
}

void FriendDB::InsertFriend(int32 playerCode, int32 friendCode)
{
    conn->Prepare(insertFriendQuery);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&friendCode);
    conn->Exec(insertFriendQuery);

    _dbOdbc.ReSetIdx();
}

void FriendDB::DeleteFriend(int32 playerCode, int32 friendCode)
{
    conn->Prepare(deleteFriendQuery);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&friendCode);
    conn->Exec(deleteFriendQuery);

    _dbOdbc.ReSetIdx();
}
