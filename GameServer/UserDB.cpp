﻿#include "UserDB.h"

UserDB::UserDB() : _dbOrm(4)
{
    conn = GDBPool->Pop();
    _dbOrm.SetDBConn(conn);
}

UserDB::~UserDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void UserDB::LoadAccountDB()
{
    _dbOrm.SetDBConn(conn);
    conn->Prepare(selectAccountQuery);
    conn->Execute();

    _dbOrm.BindColInt(sizeof(_user.accountCode), &_user.accountCode);
    _dbOrm.BindColWchar(sizeof(_user.id), &_user.id);
    _dbOrm.ReSetIdx();
}

void UserDB::LoadPlayerDB()
{
    _dbOrm.SetDBConn(conn);
    conn->Prepare(selectPlayerQuery);
    conn->Execute();

    _dbOrm.BindColInt(sizeof(_player.playerCode), &_player.playerCode);
    _dbOrm.BindColWchar(sizeof(_player.name), &_player.name);
    _dbOrm.BindColInt(sizeof(_player.jobCode), &_player.jobCode);
    _dbOrm.BindColInt(sizeof(_player.accountCode), &_player.accountCode);
    _dbOrm.ReSetIdx();
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
        _dbOrm.ReSetIdx();
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
        _dbOrm.ReSetIdx();
        return false;
    }
}