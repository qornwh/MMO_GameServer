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
    conn->Prepare(selectFriendQuery);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(selectFriendQuery);

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
    conn->Prepare(insertFriendQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&friendCode);
    conn->Exec(insertFriendQuery);

    _dbOrm.ReSetIdx();
}

void FriendDB::DeleteFriend(int32 playerCode, int32 friendCode)
{
    conn->Prepare(deleteFriendQuery);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&friendCode);
    conn->Exec(deleteFriendQuery);

    _dbOrm.ReSetIdx();
}
