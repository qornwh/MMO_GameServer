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

void FriendSystem::AddFriend(int32 friendCode, GamePlayerInfoRef friendSession)
{
    FriendDB friendDB;
    friendDB.InsertFriend(_playerCode, friendCode);

    _friendList[friendCode] = 1;
}

void FriendSystem::RemoveFriend(int32 friendCode)
{
    FriendDB friendDB;
    friendDB.DeleteFriend(_playerCode, friendCode);

    _friendList[friendCode] = 0;
}

void FriendSystem::UpdateFriend(int32 friendCode, bool flag)
{
    auto PlayerAccess = GUserAccess->GetPlayerAccess();
    if (_friendList.find(friendCode) != _friendList.end())
    {
        _friendList[friendCode] = flag;
    }
}

void FriendSystem::NotifyFriend(bool flag)
{
    auto PlayerAccess = GUserAccess->GetPlayerAccess();
    
    protocol::SFriendSystem sendPkt;
    protocol::Friend* addFriend = sendPkt.add_friend_();
    addFriend->set_playercode(_playerCode);
    addFriend->set_access(flag);
    addFriend->set_add(false);
    for (auto& entry : _friendList)
    {
        auto session = PlayerAccess[entry.first].lock();
        if (session)
        {
            auto gameSession = std::static_pointer_cast<GameSession>(session);
            gameSession->GetPlayer()->GetFriend().UpdateFriend(_playerCode, flag);

            // 각자 친구들에게 내 접속여부를 던진다.
            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_FRIENDSYSTEM);
            gameSession->AsyncWrite(sendBuffer);
        }
    }
}

Map<int32, int32>& FriendSystem::GetFriendList()
{
    return _friendList;
}
