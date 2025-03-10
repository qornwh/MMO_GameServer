﻿#pragma once
#include <random>
#include "GameGlobal.h"
#include "GameRoomManager.h"
#include "GameSession.h"
#include "GameObjectInfo.h"
#include "GameMosterInfo.h"
#include "GamePlayerInfo.h"
#include "OverlappedTask.h"

class IRoom : public std::enable_shared_from_this<IRoom>
{
public:
    IRoom(uint32 id) : _id(id)
    {
        _taskIo = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, (u_long)0, 0);
    }
    
    void task()
    {
        WriteLockGuard wl(lock);
        DWORD numOfBytes = 0;
        ULONG_PTR key = 0;
        OverlappedTask* overlappedPtr = nullptr;
        
        while (GetQueuedCompletionStatus(_taskIo, &numOfBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&overlappedPtr), 10))
        {
            overlappedPtr->Execute();
            delete overlappedPtr;
        }
        int errorCode = WSAGetLastError();
        if (errorCode != WAIT_TIMEOUT)
        {
            std::cout << "Task ErrorCode: " << errorCode << std::endl;
            assert(-1);
        }
    }
protected:
    HANDLE _taskIo;
    Atomic<uint64> _taskId{0};
    
public:
    virtual void EnterSession(SessionRef session)
    {
        DWORD dwNumberOfBytesTransferred = 0;
        ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
        OverlappedTask* overlapped = new OverlappedTask();
        overlapped->f = [this, session]()
        {
            _sessionList.insert(session);
        };
        PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
    }
    virtual void OutSession(SessionRef session)
    {
        DWORD dwNumberOfBytesTransferred = 0;
        ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
        OverlappedTask* overlapped = new OverlappedTask();
        overlapped->f = [this, session]()
        {   
            _sessionList.erase(session);
        };
        PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
    }
    virtual void BroadCast(SendBufferRef sendBuffer)
    {
        DWORD dwNumberOfBytesTransferred = 0;
        ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
        OverlappedTask* overlapped = new OverlappedTask();
        overlapped->f = [this, sendBuffer]()
        {   
            for (auto& session : _sessionList)
            {
                session->AsyncWrite(sendBuffer);
            }
        };
        PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
    }

    virtual ~IRoom()
    {
    }

protected:
    Lock lock;
    Set<SessionRef> _sessionList;
    int32 _id;
    uint32 _type;
    int32 _frameTime = 50; // 50ms, 20프레임
};

class GameRoom : public IRoom
{
public:
    GameRoom(uint32 id) : IRoom(id)
    {
        _type = GRoomManger->RoomType::space;
        _timer = std::chrono::system_clock::now();
    }
private:
    std::chrono::system_clock::time_point _timer;
    int32 _timerDelay = 100;
    Atomic<bool> isLoopTask{false};

public:
    ~GameRoom() override
    {
    }

    void EnterSession(SessionRef session) override;
    void OutSession(SessionRef session) override;
    void StartGameRoom();
    void Tick();
    void Update();
    GameMapInfoRef CreateMapInfo(int32 type);
    void CreateMonster(int32 type, int32 count);
    void BroadCastAnother(SendBufferRef sendBuffer, int32 uuid);
    void AttackObject(GameSessionRef session, int32 attackNumber, int32 skillCode, float x = 0, float y = 0, float yaw = 0);
    void AttackObjectMove(GameSessionRef session, int32 attackNumber, int32 skillCode, Vector<int32> targets, float x = 0, float y = 0, float yaw = 0);
    void AttackPlayer(int32 uuid);
    void HealObject(GameSessionRef session, int32 skillCode);

    GameMosterInfoRef GetMonster(int32 Code)
    {
        if (_monsterMap.find(Code) != _monsterMap.end())
            return _monsterMap[Code];
        return nullptr;
    }
    
    GamePlayerInfoRef GetPlayer(int32 Code)
    {
        if (_playerMap.find(Code) != _playerMap.end())
            return _playerMap[Code];
        return nullptr;
    }

    Map<int32, GameMosterInfoRef> GetMonsterMap() { return _monsterMap; }
    Map<int32, GamePlayerInfoRef> GetPlayerMap() { return _playerMap; }
    GameMapInfoRef GetGameMap() { return _gameMapInfo; }
    protocol::SUnitStates& GetUnitPacket() { return _unitPkt; }

private:
    int32 _monsterCount = 0;
    GameMapInfoRef _gameMapInfo;
    Map<int32, GameMosterInfoRef> _monsterMap;
    Map<int32, GamePlayerInfoRef> _playerMap;
    protocol::SUnitStates _unitPkt;
    std::mt19937_64 rng;

    //더미용
public:
    void EnterDummySession(SessionRef session);
};
