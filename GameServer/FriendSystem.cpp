#include "FriendSystem.h"

#include "FriendDB.h"
#include "GameGlobal.h"
#include "GameObjectInfo.h"
#include "GamePacketHandler.h"
#include "GameUserAccess.h"
#include "GameSession.h"

FriendSystem::FriendSystem(int32 playerCode) : _playerCode(playerCode)
{
    LoadFriend();
}

FriendSystem::~FriendSystem()
{
}
void FriendSystem::LoadFriend()
{
    FriendDB friendDB;
    friendDB.LoadFriend(_playerCode);

    int32 friendCode;
    auto PlayerAccess = GUserAccess->GetPlayerAccess();
    while (friendDB.GetFriend(friendCode) == true)
    {
        if (PlayerAccess[friendCode].lock() != nullptr)
        {
            _friendList.emplace(friendCode, 1);
        }
        else
        {
            _friendList.emplace(friendCode, 0);
        }
    }
}

void FriendSystem::AddFriend(int32 friendCode, bool Access)
{
    FriendDB friendDB;
    friendDB.InsertFriend(_playerCode, friendCode);

    if (Access)
    {
        _friendList[friendCode] = 1;
    }
    else
    {
        _friendList[friendCode] = 0;
    }
}

void FriendSystem::RemoveFriend(int32 friendCode)
{
    FriendDB friendDB;
    friendDB.DeleteFriend(_playerCode, friendCode);

    _friendList[friendCode] = 0;
}

void FriendSystem::UpdateFriend(int32 friendCode, bool flag)
{
    if (_friendList.find(friendCode) != _friendList.end())
    {
        _friendList[friendCode] = flag;
    }
}

Map<int32, int32>& FriendSystem::GetFriendList()
{
    return _friendList;
}
