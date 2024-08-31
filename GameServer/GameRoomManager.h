#pragma once
#include "pch.h"

class GameRoomManager : public std::enable_shared_from_this<GameRoomManager>
{
public:
    
    enum RoomType
    {
        space, // 지역
    };

    GameRoomManager();
    ~GameRoomManager();
    void EreaseRoom(int32 id);
    GameRoomRef getRoom(int32 id);
    void ClearRoom();
    GameRoomRef CreateRoom(int32 id);

private:
    Atomic<int32> _id{0};
    Map<int32, GameRoomRef> _roomMap;
};

extern std::shared_ptr<GameRoomManager> GBSRoomManger;
