#include "GameUserAccess.h"

void GameUserAccess::ClearUser()
{
    _userAccess.clear();
    _playerAccess.clear();
}

bool GameUserAccess::AccessUser(int32 userCode, int32 sessionId)
{
    // sessionId : 들어온 세션 아이디
    // userCode : db상 unique 계정 번호
    if (_userAccess.find(userCode) != _userAccess.end())
    {
        if (_userAccess[userCode] > 0)
            return false;
    }
    _userAccess[userCode] = sessionId;
    return true;
}

void GameUserAccess::ReleaseUser(int32 userCode)
{
    _userAccess[userCode] = -1;
}

bool GameUserAccess::AccessPlayer(int32 playerCode, int32 sessionId)
{
    // sessionId : 들어온 세션 아이디
    // playerCode : db상 unique 캐릭터 번호
    if (_playerAccess.find(playerCode) != _playerAccess.end())
    {
        if (_playerAccess[playerCode] > 0)
            return false;
    }
    _playerAccess[playerCode] = sessionId;
    return true;
}

void GameUserAccess::ReleasePlayer(int32 playerCode)
{
    _playerAccess[playerCode] = -1;
}
