#pragma once
#include "GameObjectInfo.h"

class GameMosterInfo : public GameObjectInfo
{
#pragma region field
public:
    GameMosterInfo(GameRoomRef gameRoom, int32 uuid, int32 code, int32 lv, int32 startX, int32 startY);
    ~GameMosterInfo() override;

    void Update() override;
    void Move() override;
    void Spawn();
    virtual void MoveTarget(GamePlayerInfoRef target);
    bool CheckAttackTarget(GamePlayerInfoRef target);
    void TakeDamage(int32 Damage) override;
    void SetObjecteState(ObjectState state) override;
    void GetStartPosition(int32& x, int32& y);
    void SetTarget(int32 uuid);
    int32 GetTarget() { return _targetUUid; }
    int32 GetExp() { return _exp; }
    DropGenSystem& GetDropSystem() { return _dropGenSystem; }

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
    std::uniform_int_distribution<> _genYaw;
#pragma endregion field
    DropGenSystem _dropGenSystem;

protected:
    GameUtils::TickCounter _yawCounter{ 4 };
    GameUtils::TickCounter _moveCounter{ 10 };
    GameUtils::TickCounter _idleCounter{ 3 };
    GameUtils::TickCounter _hitCounter{ 3 };
    GameUtils::TickCounter _dieCounter{ 30 };
    GameUtils::TickCounter _attackCounter{ 10 };
    GameUtils::TickCounter _readyAttackCounter{ 10 };
};

