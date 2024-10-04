#include "UserDB.h"

UserDB::UserDB() : _dbOdbc(4)
{
    conn = GDBPool->Pop();
    _dbOdbc.SetDBConn(conn);
}

UserDB::~UserDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void UserDB::LoadAccountDB()
{
    _dbOdbc.SetDBConn(conn);
    conn->Prepare(selectAccountQuery);
    conn->Execute();

    _dbOdbc.BindColInt(sizeof(_user.accountCode), &_user.accountCode);
    _dbOdbc.BindColWchar(sizeof(_user.id), &_user.id);
    _dbOdbc.ReSetIdx();
}

void UserDB::LoadPlayerDB()
{
    _dbOdbc.SetDBConn(conn);
    conn->Prepare(selectPlayerQuery);
    conn->Execute();

    _dbOdbc.BindColInt(sizeof(_player.playerCode), &_player.playerCode);
    _dbOdbc.BindColWchar(sizeof(_player.name), &_player.name);
    _dbOdbc.BindColInt(sizeof(_player.jobCode), &_player.jobCode);
    _dbOdbc.BindColInt(sizeof(_player.accountCode), &_player.accountCode);
    _dbOdbc.ReSetIdx();
}

bool UserDB::GetAccount(User& user)
{
    if (conn->Fetch())
    {
        user.accountCode = _user.accountCode;
        wcscpy_s(user.id, 10, _user.id);
        return true;
    }
    else
    {
        conn->CloseCursor();
        _dbOdbc.ReSetIdx();
        return false;
    }
}

bool UserDB::GetPlayer(Player& player)
{
    if (conn->Fetch())
    {
        player.accountCode = _player.accountCode;
        player.playerCode = _player.playerCode;
        player.jobCode = _player.jobCode;
        wcscpy_s(player.name, 10, _player.name);
        return true;
    }
    else
    {
        conn->CloseCursor();
        _dbOdbc.ReSetIdx();
        return false;
    }
}
