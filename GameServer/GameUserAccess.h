#pragma once
#include "Types.h"
#include "ReadWriteLock.h"

class GameUserAccess : public std::enable_shared_from_this<GameUserAccess>
{
public:
    void ClearUser();
    bool AccessUser(int32 userCode, int32 sessionId);
    void ReleaseUser(int32 userCode);
    bool AccessPlayer(int32 playerCode, int32 sessionId);
    void ReleasePlayer(int32 playerCode);

private:
    Lock lock;
    Map<int32, int32> _userAccess;
    Map<int32, int32> _playerAccess;
};
