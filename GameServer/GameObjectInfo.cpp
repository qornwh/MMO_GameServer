#include "GameObjectInfo.h"

#include "GameCharater.h"
#include "GameDrop.h"
#include "GameGlobal.h"
#include "GameMapInfo.h"
#include "GamePacketHandler.h"
#include "GameSession.h"
#include "IRoom.h"

GameObjectInfo::GameObjectInfo(GameRoomRef gameRoom, int32 uuid, int32 code): _gameRoomRef(gameRoom), _uuid(uuid), _hp(0), _maxHp(0), _damage(0), _code(code), _lv(0), _collider(0.f, 0.f)
{
}

GameObjectInfo::~GameObjectInfo()
{
}

void GameObjectInfo::TakeDamage(int32 Damage)
{
    _hp -= Damage;
    if (_hp < 0)
        _hp = 0;
    _damage += Damage;
}

void GameObjectInfo::TakeHeal(int32 heal)
{
    _hp += heal;
    if (_hp > _maxHp)
        _hp = _maxHp;
}

void GameObjectInfo::SetName(const std::string& name)
{
    _name = name;
}

void GameObjectInfo::SetHp(int32 hp)
{
    _hp = hp;
}

void GameObjectInfo::SetCode(int32 code)
{
    _code = code;
}

void GameObjectInfo::SetLv(int32 lv)
{
    _lv = lv;
}

void GameObjectInfo::SetPosition(float x, float y)
{
    _collider.SetPosition(x, y);
}

void GameObjectInfo::SetRotate(float yaw)
{
    _collider.SetRotate(yaw);
}


void GameObjectInfo::SetObjecteState(ObjectState state)
{
    _state = state;
}

void GameObjectInfo::SetGameRoom(GameRoomRef gameRoom)
{
    _gameRoomRef = gameRoom;
}
