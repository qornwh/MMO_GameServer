#pragma once
#include <random>
#include "GameUtils.h"
#include "Collider.h"
#include "DropGenerater.h"
#include "GameDrop.h"
#include "FriendSystem.h"
#include "GameCharater.h"
#include "GameService.pb.h"

static std::mt19937_64 rng{};

enum ObjectState
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
    virtual void TakeDamage(int32 Damage);
    virtual void TakeHeal(int32 heal);

    void SetName(const std::string& name);
    std::string& GetName() { return _name; }
    int32 GetUUid() { return _uuid; }
    int32 GetMaxHp() { return _maxHp; }
    void SetHp(int32 hp);
    int32 GetHp() { return _hp; }
    void SetCode(int32 code);
    int32 GetCode() { return _code; }
    void SetLv(int32 lv);
    int32 GetLv() { return _lv; }
    void SetPosition(float x, float y);
    void SetRotate(float yaw);
    Collider& GetCollider() { return _collider; }
    virtual void SetObjecteState(ObjectState state);
    ObjectState GetObjectState() { return _state; }
    void SetGameRoom(GameRoomRef gameRoom);
    GameRoomRef GetGameRoom() { return _gameRoomRef.lock(); }

protected:
    std::string _name;
    int32 _uuid;
    int32 _damage;
    int32 _code;
    int32 _maxHp = 0;
    int32 _hp = 0;
    int32 _lv = 0;

    Collider _collider;
    ObjectState _state = ObjectState::IDLE;
    std::weak_ptr<GameRoom> _gameRoomRef;
};

