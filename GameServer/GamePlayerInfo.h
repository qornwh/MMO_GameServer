#pragma once
#include "GameObjectInfo.h"
#include "Inventory.h"
#include "MailSystem.h"
#include "GameAttackInfo.h"
#include "ObjectAbility.h"

class GamePlayerInfo : public GameObjectInfo
{
public:
    GamePlayerInfo(GameSessionRef gameSession, int32 playerCode, int32 uuid, int32 jobCode, int32 weaponCode, int32 lv);
    ~GamePlayerInfo() override;
    void Update() override;
    void AttackObject(bool isCreate, int32 attackNumber, int32 skillCode, float x, float y, float yaw);
    void AttackObjectMove(int32 attackNumber, float x, float y, float yaw);
    bool AttackObjectCollision(int32 attackNumber, int32 target);

    void UpdateAblity();
    void SetTarget(int32 uuid);
    int32 GetTarget() { return _targetCode; }
    GameSessionRef GetGameSession() { return _gameSession.lock(); }
    void SetInventory(int32 gold);
    void SetFriend();
    void SetMail();
    const int32 GetPlayerCode() { return _playerCode; }
    int32 GetWeapon() { return _weaponCode; }
    void ReSpawn();

    int32 TakeHeal(int32 Heal);
    int32 TakeDamage(int32 target, int32 Damage);
    bool AddExp(int32 exp);
    void SetExp(int32 exp);
    ObjectAbility& GetAbility() { return _ability; }
    Inventory& GetInventory() { return _inventory; }
    FriendSystem& GetFriend() { return _friendSystem; }
    MailSystem& GetMail() { return _mailSystem; }

private:
    int32 _targetCode;
    int32 _playerCode;
    int32 _weaponCode;
    Inventory _inventory;
    FriendSystem _friendSystem;
    MailSystem _mailSystem;
    ObjectAbility _ability;

    Map<int32, std::shared_ptr<GameAttackInfo>> _attackObjs;
    std::weak_ptr<GameSession> _gameSession;

    //´õ¹Ì¿ë
public:
    void SetDummy(bool flag);
    bool IsDummy() { return _dummy; }

private:
    bool _dummy = false;
};