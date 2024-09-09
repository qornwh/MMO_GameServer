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

    Map<int32, std::weak_ptr<class GameSession>>& GetUserAccess();
    Map<int32, std::weak_ptr<class GameSession>>& GetPlayerAccess();
    Map<int32, User>& GetUserList();
    Map<int32, Player>& GetPlayerList();

private:
    Lock lock;

    Map<int32, User> _userList;
    Map<int32, Player> _playerList;

    Map<int32, std::weak_ptr<class GameSession>> _userAccess;
    Map<int32, std::weak_ptr<class GameSession>> _playerAccess;
};
