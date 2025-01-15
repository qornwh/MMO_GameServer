#pragma once
#include "GameObjectInfo.h"
#include "Inventory.h"
#include "MailSystem.h"

class GamePlayerInfo : public GameObjectInfo
{
#pragma region field
public:
    GamePlayerInfo(GameSessionRef gameSession, int32 playerCode, int32 uuid, int32 jobCode, int32 weaponCode, int32 lv);
    ~GamePlayerInfo() override;
    void Update() override;
    void Attack(GameObjectInfoRef target, Vector<int32>& attackList);
    void SetTarget(int32 uuid);
    int32 GetTarget() { return _targetCode; }
    GameSessionRef GetGameSession() { return _gameSession.lock(); }
    void SetInventory(int32 gold);
    void SetFriend();
    void SetMail();
    const int32 GetPlayerCode() { return _playerCode; }
    int32 GetWeapon() { return _weaponCode; }
    void ReSpawn();

    bool AddExp(int32 exp);
    void SetExp(int32 exp);
    int32 GetExp() { return _exp; }
    Inventory& GetInventory() { return _inventory; }
    FriendSystem& GetFriend() { return _friendSystem; }
    MailSystem& GetMail() { return _mailSystem; }

private:
    int32 _targetCode;
    int32 _playerCode;
    int32 _weaponCode;
    int32 _exp;
    std::weak_ptr<GameSession> _gameSession;
#pragma endregion field
    Inventory _inventory;
    FriendSystem _friendSystem;
    MailSystem _mailSystem;

    //´õ¹Ì¿ë
public:
    void SetDummy(bool flag);
    bool IsDummy() { return _dummy; }

private:
    bool _dummy = false;
};