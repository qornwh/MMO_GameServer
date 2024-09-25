#pragma once
#include "pch.h"
#include "Types.h"
#include "ReadWriteLock.h"
#include "UserDB.h"

class GameUserAccess : public std::enable_shared_from_this<GameUserAccess>
{
public:
    void ClearUser();
    void LoadUserList();
    void AddUserList(User& userDB);
    void AddPlayerList(Player& player);
    bool AccessUser(int32 userCode, SessionRef session);
    bool AccessPlayer(int32 playerCode, SessionRef session);

    bool IsAccessPlayer(int32 playerCode);
    bool IsAccessUser(int32 userCode);

    Map<int32, std::weak_ptr<class GameSession>>& GetUserAccess();
    Map<int32, std::weak_ptr<class GameSession>>& GetPlayerAccess();
    Map<int32, User>& GetUserList();
    Map<int32, Player>& GetPlayerList();
    Map<WString, int32>& GetPlayerNameList();

private:
    Lock lock;

    // 일단 dual hash로 할수 밖에 없다...
    Map<int32, User> _userList;
    Map<int32, Player> _playerList;
    Map<WString, int32> _playerToName;

    Map<int32, std::weak_ptr<class GameSession>> _userAccess;
    Map<int32, std::weak_ptr<class GameSession>> _playerAccess;
};
