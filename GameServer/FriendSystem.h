#pragma once
#include "pch.h"

class FriendSystem
{
public:
    FriendSystem(int32 playerCode);
    ~FriendSystem();

    void LoadFriend();
    void AddFriend(int32 friendCode, bool Access);
    void RemoveFriend(int32 friendCode);
    void UpdateFriend(int32 friendCode, bool flag); // 접속여부
    void NotifyFriends(bool flag); // 나 접속, 종료
    void NotifyFriend(int32 friendCode, bool flag); // 추가
    Map<int32, int32>& GetFriendList();

private:
    Map<int32, int32> _friendList; // key : 친구 playerCode, value: 들어왔는지 여부 체크
    int32 _playerCode;
};
