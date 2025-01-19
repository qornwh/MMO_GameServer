#pragma once
#include <random>
#include "GameUtils.h"
#include "CapsuleCollider.h"
#include "DropGenerater.h"
#include "GameDrop.h"
#include "FriendSystem.h"
#include "GameCharater.h"
#include "GameService.pb.h"

static std::mt19937_64 rng{};

enum ObjectStateType
{
    IDLE = 0,
    DIE = 1,
    HITED = 2,
    MOVE = 3,
    ATTACK = 4,
    SKILL1 = 5,
    SKILL2 = 6,
    HEAL = 7,
    READY_ATTACK = 14,
};

enum GameObjectType
{
    PLAYER = 0,
    MOMSTER = 1,
    OBJECT = 2,
};

class GameObjectInfo : public std::enable_shared_from_this<GameObjectInfo>
{
public:
    GameObjectInfo(GameRoomRef gameRoom, int32 uuid, int32 code);
    GameObjectInfo(const GameObjectInfo& other) = delete;
    GameObjectInfo(GameObjectInfo&& other) = delete;
    virtual ~GameObjectInfo();
    
    virtual void Update() {}
    virtual void Move() {}
    //virtual void TakeDamage(int32 Damage);
    //virtual void TakeHeal(int32 heal);

    void SetName(const std::string& name);
    std::string& GetName() { return _name; }
    int32 GetUUid() { return _uuid; }
    void SetCode(int32 code);
    int32 GetCode() { return _code; }
    void SetPosition(float x, float y);
    Vector2& GetPosition() { return _collider.GetPosition(); }
    void SetRotate(float yaw);
    float GetRotate() { return _collider.GetRotate(); }

    CapsuleCollider& GetCollider() { return _collider; }
    virtual void SetObjecteState(ObjectStateType state);
    ObjectStateType GetObjectState() { return _state; }
    void SetGameRoom(GameRoomRef gameRoom);
    GameRoomRef GetGameRoom() { return _gameRoomRef.lock(); }

protected:
    std::string _name;
    int32 _uuid;
    int32 _damage;
    int32 _code;

    CapsuleCollider _collider;
    ObjectStateType _state = ObjectStateType::IDLE;
    std::weak_ptr<GameRoom> _gameRoomRef;
};

