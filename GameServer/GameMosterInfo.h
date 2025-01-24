#pragma once
#include "GameObjectInfo.h"
#include "ObjectAbility.h"

class GameMosterInfo : public GameObjectInfo
{
public:
    GameMosterInfo(GameRoomRef gameRoom, int32 uuid, int32 code, int32 lv, int32 startX, int32 startY);
    ~GameMosterInfo() override;

    void Update() override;
    void Move() override;
    void Spawn();
    void MoveTarget(GamePlayerInfoRef target);
    bool AttackObjectCollision(int32 attackCode = 0);
    int32 TakeDamage(int32 target, int32 Damage);
    void SetObjecteState(ObjectStateType state) override;
    void GetStartPosition(int32& x, int32& y);
    void SetTarget(int32 uuid);
    int32 GetTarget() { return _targetUUid; }
    ObjectAbility& GetAbility() { return _ability; }
    DropGenSystem& GetDropSystem() { return _dropGenSystem; }

    virtual int32 AddReadyAttackCounter(int32 count = 1);
    virtual int32 AddAttackCounter(int32 count = 1);
    virtual int32 AddIdleCounter(int32 count = 1);
    virtual int32 AddHitCounter(int32 count = 1);
    virtual int32 AddMoveCounter(int32 count = 1);
    virtual int32 AddDieCounter(int32 count = 1);

private:
    int32 _startX;
    int32 _startY;
    int32 _targetUUid = -1;
    std::uniform_int_distribution<> _genYaw;

    ObjectAbility _ability;
    DropGenSystem _dropGenSystem;

protected:
    GameUtils::TickCounter _yawCounter{ 40 };
    GameUtils::TickCounter _moveCounter{ 1 };
    GameUtils::TickCounter _idleCounter{ 20 };
    GameUtils::TickCounter _hitCounter{ 15 };
    GameUtils::TickCounter _dieCounter{ 10 };
    GameUtils::TickCounter _attackCounter{ 5 };
    GameUtils::TickCounter _readyAttackCounter{ 5 };
};

