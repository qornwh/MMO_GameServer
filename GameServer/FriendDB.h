#pragma once
#include "DBOrm.h"

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
    DBOrm _dbOrm;
    int32 _friendCode = -1;
    DBConnRef conn;
};
