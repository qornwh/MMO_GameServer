#include "GameUserAccess.h"

#include "GameSession.h"
#include "Session.h"

void GameUserAccess::ClearUser()
{
    _userAccess.clear();
    _playerAccess.clear();
}

void GameUserAccess::LoadUserList()
{
    UserDB userDB;
    userDB.LoadAccountDB();

    User user;
    while (userDB.GetAccount(user))
    {
        int32 accountCode = user.accountCode;
        _userList.emplace(accountCode, user);
    }
    
    userDB.LoadPlayerDB();
    Player player;
    while (userDB.GetPlayer(player))
    {
        int32 playerCode = player.playerCode;
        _playerList.emplace(playerCode, player);
        _playerToName.emplace(player.name, playerCode);
    }
}

void GameUserAccess::AddUserList(User& user)
{
    WriteLockGuard writeLock(lock, "friend lock");
    int32 accountCode = user.accountCode;
    _userList.emplace(accountCode, user);
}

void GameUserAccess::AddPlayerList(Player& player)
{
    WriteLockGuard writeLock(lock, "friend lock");
    int32 playerCode = player.playerCode;
    _playerList.emplace(playerCode, player);
    _playerToName.emplace(player.name, playerCode);
}

bool GameUserAccess::AccessUser(int32 userCode, SessionRef session)
{
    // sessionId : 들어온 세션 아이디
    // userCode : db상 unique 계정 번호
    WriteLockGuard writeLock(lock, "friend lock");
    if (_userAccess.find(userCode) != _userAccess.end())
    {
        if (_userAccess[userCode].lock() != nullptr)
            return false;
    }
    _userAccess[userCode] = std::static_pointer_cast<GameSession>(session);
    return true;
}

bool GameUserAccess::AccessPlayer(int32 playerCode, SessionRef session)
{
    // sessionId : 들어온 세션 아이디
    // playerCode : db상 unique 캐릭터 번호
    WriteLockGuard writeLock(lock, "friend lock");
    if (_playerAccess.find(playerCode) != _playerAccess.end())
    {
        if (_playerAccess[playerCode].lock() != nullptr)
            return false;
    }
    _playerAccess[playerCode] = std::static_pointer_cast<GameSession>(session);
    return true;
}

bool GameUserAccess::IsAccessPlayer(int32 userCode)
{
    WriteLockGuard writeLock(lock, "friend lock");
    auto it = GetPlayerAccess().find(userCode);
    if (it != GetPlayerAccess().end())
    {
        if (it->second.lock())
        {
            return true;
        }
    }
    return false;
}

bool GameUserAccess::IsAccessUser(int32 playerCode)
{
    WriteLockGuard writeLock(lock, "friend lock");
    auto it = GetUserAccess().find(playerCode);
    if (it != GetUserAccess().end())
    {
        if (it->second.lock())
        {
            return true;
        }
    }
    return false;
}

Map<int32, User>& GameUserAccess::GetUserList()
{
    return _userList;
}

Map<int32, Player>& GameUserAccess::GetPlayerList()
{
    return _playerList;
}

Map<WString, int32>& GameUserAccess::GetPlayerNameList()
{
    return _playerToName;
}

Map<int32, std::weak_ptr<class GameSession>>& GameUserAccess::GetUserAccess()
{
    return _userAccess;
}

Map<int32, std::weak_ptr<class GameSession>>& GameUserAccess::GetPlayerAccess()
{
    return _playerAccess;
}
