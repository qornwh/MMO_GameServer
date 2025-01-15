#include "GamePlayerInfo.h"
#include "GameGlobal.h"
#include "GameMapInfo.h"
#include "GameExpLv.h"
#include "IRoom.h"

GamePlayerInfo::GamePlayerInfo(GameSessionRef gameSession, int32 playerCode, int32 uuid, int32 jobCode, int32 weaponCode, int32 lv) :
    GameObjectInfo(GRoomManger->getRoom(gameSession->GetRoomId()), uuid, jobCode), _targetCode(-1), _playerCode(-1), _gameSession(gameSession), _exp(0), _friendSystem(playerCode)
{
    auto it = GPlayer->GetCharater().find(jobCode);
    CrashFunc(it != GPlayer->GetCharater().end());
    _maxHp = it->second._hp;
    _hp = _maxHp;
    _weaponCode = weaponCode;
    _playerCode = playerCode;
    _lv = lv;
}

GamePlayerInfo::~GamePlayerInfo()
{
    if (IsDummy()) return;
    _inventory.SaveDB();
}

void GamePlayerInfo::Update()
{
    // GameObjectInfo::Update();
    if (_gameSession.lock() == nullptr)
        return;

    GameRoomRef room = GRoomManger->getRoom(_gameSession.lock()->GetRoomId());
    if (room == nullptr)
        return;
}

void GamePlayerInfo::Attack(GameObjectInfoRef target, Vector<int32>& attackList)
{
}

void GamePlayerInfo::SetTarget(int32 uuid)
{
    _targetCode = uuid;
}

void GamePlayerInfo::SetInventory(int32 gold)
{
    _inventory.Init(_playerCode, gold);
}

void GamePlayerInfo::SetFriend()
{
    _friendSystem.LoadFriend();
}

void GamePlayerInfo::SetMail()
{
    _mailSystem.LoadMail(_playerCode);
    _mailSystem.SetPlayer(shared_from_this());
}

void GamePlayerInfo::ReSpawn()
{
    _hp = _maxHp;
}

bool GamePlayerInfo::AddExp(int32 exp)
{
    auto it = GExpLv->GetExpLv().find(_lv);
    CrashFunc(it != GExpLv->GetExpLv().end());

    if (it->second._exp <= 0)
        return false; // ¸¸·¾

    _exp += exp;
    if (it->second._exp <= _exp)
    {
        // ·¹º§¾÷
        _lv += 1;
        _exp = _exp - it->second._exp;
    }

    return true;
}

void GamePlayerInfo::SetExp(int32 exp)
{
    _exp = exp;
}

void GamePlayerInfo::SetDummy(bool flag)
{
    _dummy = flag;
}
