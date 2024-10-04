#pragma once
#include "DBOdbc.h"

static const wchar_t* selectFriendQuery = L"SELECT friendCode FROM Friend WHERE playerCode = ?";
static const wchar_t* insertFriendQuery = L"INSERT INTO Friend (playerCode, friendCode) VALUES (?,?)";
static const wchar_t* deleteFriendQuery = L"DELETE FROM Friend WHERE playerCode = ? AND friendCode = ?";

class FriendDB
{
public:
    FriendDB();
    ~FriendDB();

    void LoadFriend(int32 playerCode);
    bool GetFriend(int32& friendCode);
    void InsertFriend(int32 playerCode, int32 friendCode);
    void DeleteFriend(int32 playerCode, int32 friendCode);

private:
    DBOdbc _dbOdbc;
    int32 _friendCode = -1;
    DBConnRef conn;
};
