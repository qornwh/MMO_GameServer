#pragma once
#include <random>
#include "GameUtils.h"
#include "Collider.h"
#include "DropItem.h"
#include "GameCharater.h"
#include "GameService.pb.h"
#include "Inventory.h"

static std::mt19937_64 rng;

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
    std::string& GetName() { return _name; };
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

class GameMosterInfo : public GameObjectInfo
{
public:
    GameMosterInfo(GameRoomRef gameRoom, int32 uuid, int32 code, int32 lv, int32 startX, int32 startY);
    ~GameMosterInfo() override;

    void Update() override;
    void Move() override;
    void Spawn();
    void DropInit();
    const Vector<std::pair<int32, int32>>& GetDropList() { return _itemList; }
    const Vector<int32>& GetDropEquipList() { return _itemEquipList; }
    const int32& GetDropGold() { return _dropGold; }
    virtual void MoveTarget(GamePlayerInfoRef target);
    bool CheckAttackTarget(GamePlayerInfoRef target);
    void TakeDamage(int32 Damage) override;
    
    void SetObjecteState(ObjectState state) override;
    void GetStartPosition(int32& x, int32& y);
    void SetTarget(int32 uuid);
    int32 GetTarget() { return _targetUUid; }
    int32 GetExp() { return _exp; }
    
    virtual int32 AddAttackCounter(int32 count = 1);
    virtual int32 AddIdleCounter(int32 count = 1);
    virtual int32 AddHitCounter(int32 count = 1);
    virtual int32 AddMoveCounter(int32 count = 1);
    virtual int32 AddDieCounter(int32 count = 1);

private:
    int32 _startX;
    int32 _startY;
    float _moveSpeed = 0.f;
    int32 _targetUUid = -1;
    int32 _exp = 0;

    std::uniform_int_distribution<> genYaw;
    std::uniform_int_distribution<> genEquip;
    std::uniform_int_distribution<> genItem;

protected:
    GameUtils::TickCounter _YawCounter{4};
    GameUtils::TickCounter _MoveCounter{10};
    GameUtils::TickCounter _IdleCounter{3};
    GameUtils::TickCounter _HitCounter{3};
    GameUtils::TickCounter _DieCounter{30};
    GameUtils::TickCounter _AttackCounter{10};
    GameUtils::TickCounter _ReadyAttackCounter{10};
    
    Vector<std::pair<int32, int32>> _itemList; // 드롭 리스트
    Vector<int32> _itemEquipList; // 드롭 장비 리스트
    int32 _dropGold; // 드롭 골드
};

class GamePlayerInfo : public GameObjectInfo
{
public:
    GamePlayerInfo(GameSessionRef gameSession, int32 uuid, int32 code, int32 lv);
    ~GamePlayerInfo() override;
    void Update() override;
    void Attack(GameObjectInfoRef target, Vector<int32>& attackList);
    void SetTarget(int32 uuid);
    int32 GetTarget() { return _targetCode; }
    GameSessionRef GetGameSession() { return _gameSession.lock(); }
    void SetPlayerCode(int32 playerCode, int32 weaponCode, int32 gold);
    int32 GetWeapon() { return _weaponCode; }
    void ReSpawn();

    bool AddExp(int32 exp);
    void SetExp(int32 exp);
    int32 GetExp() { return _exp; }
    Inventory& GetInventory() { return _inventory; }

private:
    int32 _targetCode;
    int32 _playerCode;
    int32 _weaponCode;
    int32 _exp;
    std::weak_ptr<GameSession> _gameSession;
    Inventory _inventory;

    //더미용
public:
    void SetDummy(bool flag);
    bool IsDummy() { return _dummy; }

private:
    bool _dummy = false;
};
