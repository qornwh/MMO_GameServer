#include "GameObjectInfo.h"

#include "GameCharater.h"
#include "GameDrop.h"
#include "GameExpLv.h"
#include "GameGlobal.h"
#include "GameMapInfo.h"
#include "GamePacketHandler.h"
#include "GameSession.h"
#include "GameUserAccess.h"
#include "IRoom.h"

GameObjectInfo::GameObjectInfo(GameRoomRef gameRoom, int32 uuid, int32 code): _gameRoomRef(gameRoom), _uuid(uuid), _hp(0), _maxHp(0), _damage(0), _code(code),
                                                                              _lv(0), _collider(20.f)
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

GameMosterInfo::GameMosterInfo(GameRoomRef gameRoom, int32 uuid, int32 code, int32 lv, int32 startX, int32 startY): GameObjectInfo(gameRoom, uuid, code),
    _startX(startX), _startY(startY), genYaw(0, 360)
{
    _lv = lv;
}

GameMosterInfo::~GameMosterInfo()
{
}

void GameMosterInfo::Spawn()
{
    SetPosition(static_cast<float>(_startX), static_cast<float>(_startY));
    SetObjecteState(ObjectState::IDLE);
    DropInit();

    auto it = GMonster->GetCharater().find(_code);
    CrashFunc(it != GMonster->GetCharater().end());
    _maxHp = it->second._hp;
    _hp = _maxHp;
    _moveSpeed = it->second._moveSpeed;
    _exp = it->second._exp;
}

void GameMosterInfo::DropInit()
{
    _dropGold = 0;
    _itemEquipList.clear();
    _itemList.clear();
    for (auto& dropItem : GDropItem->GetMonsterDropEquipList(GetCode()))
    {
        _itemEquipList.emplace_back(dropItem.GetItemCode());
    }
    for (auto& dropItem : GDropItem->GetMonsterDropList(GetCode()))
    {
        _itemList.emplace_back(dropItem.GetItemCode(), dropItem.GetCnt());
    }
    _dropGold = GDropItem->GetMonsterGold(GetCode());
}

void GameMosterInfo::Update()
{
    GameObjectInfo::Update();
    GameRoomRef room = GetGameRoom();

    switch (GetObjectState())
    {
    case ObjectState::IDLE:
        {
            if (AddIdleCounter() == 0)
            {
                SetTarget(-1);
                protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
                childPkt->set_is_monster(true);
                protocol::Unit* monster = new protocol::Unit();
                monster->set_state(GetObjectState());
                monster->set_uuid(GetUUid());
                monster->set_hp(GetHp());
                monster->set_code(GetCode());
                monster->set_lv(GetLv());
                protocol::Position* position = new protocol::Position();
                position->set_x(GetCollider().GetPosition().X);
                position->set_y(GetCollider().GetPosition().X);
                position->set_z(0);
                position->set_yaw(GetCollider().GetRotate());
                monster->set_allocated_position(position);
                childPkt->set_allocated_unit(monster);
            }
        }
        break;
    case ObjectState::MOVE:
        {
            if (AddMoveCounter() == 0)
            {
                Move();
                protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
                childPkt->set_is_monster(true);
                protocol::Unit* monster = new protocol::Unit();
                monster->set_state(ObjectState::MOVE);
                monster->set_uuid(GetUUid());
                protocol::Position* position = new protocol::Position();
                position->set_x(GetCollider().GetPosition().X);
                position->set_y(GetCollider().GetPosition().Y);
                position->set_z(0);
                position->set_yaw(GetCollider().GetRotate());
                monster->set_allocated_position(position);
                childPkt->set_allocated_unit(monster);
            }
        }
        break;
    case ObjectState::ATTACK:
        {
            if (AddAttackCounter() == 0)
            {
                protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
                childPkt->set_is_monster(true);
                protocol::Unit* unit = new protocol::Unit();
                unit->set_state(GetObjectState());
                unit->set_uuid(GetUUid());
                childPkt->set_allocated_unit(unit);
                protocol::Attack* attack = new protocol::Attack();
                attack->set_skill_code(0);
                attack->set_target_uuid(_targetUUid);
                childPkt->set_allocated_attack(attack);
            }
        }
        break;
    case ObjectState::HITED:
        {
            if (AddHitCounter() == 0)
            {
                protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
                childPkt->set_is_monster(true);
                protocol::Unit* unit = new protocol::Unit();
                unit->set_uuid(GetUUid());
                unit->set_hp(GetHp());
                unit->set_state(GetObjectState());
                protocol::Position* position = new protocol::Position();
                position->set_x(GetCollider().GetPosition().X);
                position->set_y(GetCollider().GetPosition().Y);
                position->set_z(0);
                position->set_yaw(GetCollider().GetRotate());
                unit->set_allocated_position(position);
                childPkt->set_allocated_unit(unit);
            }
        }
        break;
    case ObjectState::DIE:
        {
            if (AddDieCounter() == 0)
            {
                protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
                childPkt->set_is_monster(true);
                protocol::Unit* unit = new protocol::Unit();
                unit->set_state(ObjectState::DIE);
                unit->set_uuid(GetUUid());
                childPkt->set_allocated_unit(unit);
            }
        }
        break;
    default:
        break;
    }
}

void GameMosterInfo::SetObjecteState(ObjectState state)
{
    GameObjectInfo::SetObjecteState(state);

    switch (_state)
    {
    case ObjectState::IDLE:
        _IdleCounter.ResetTic();
        break;
    case ObjectState::DIE:
        _DieCounter.ResetTic();
        break;
    case ObjectState::HITED:
        _HitCounter.ResetTic();
        break;
    case ObjectState::MOVE:
        _MoveCounter.ResetTic();
        break;
    case ObjectState::ATTACK:
        _AttackCounter.ResetTic();
        break;
    }
}

void GameMosterInfo::GetStartPosition(int32& x, int32& y)
{
    x = _startX;
    y = _startY;
}

void GameMosterInfo::SetTarget(int32 uuid)
{
    _targetUUid = uuid;
}

void GameMosterInfo::Move()
{
    auto targetPlayer = GetGameRoom()->GetPlayer(_targetUUid);
    if (_targetUUid > 0 && targetPlayer)
    {
        Vector2& pos = targetPlayer->GetCollider().GetPosition();
        if (GetGameRoom()->GetGameMap()->GetMonsterMapInfo()->InRect(pos.X, pos.Y))
        {
            MoveTarget(targetPlayer);
            return;
        }
        else
        {
            // 몬스터 구간 넘어가면 타겟팅 초기화 한다.
            _targetUUid = -1;
        }
    }

    if (_YawCounter.Add() == 0)
    {
        SetRotate(genYaw(rng));
    }

    float moveX = GetCollider().GetPosition().X + GameEngine::MathUtils::GetSin(GetCollider().GetRotate()) * _moveSpeed;
    float moveY = GetCollider().GetPosition().Y + GameEngine::MathUtils::GetCos(GetCollider().GetRotate()) * _moveSpeed;
    GetGameRoom()->GetGameMap()->GetMonsterMapInfo()->InSetRect(moveX, moveY);
    SetPosition(moveX, moveY);
}

void GameMosterInfo::MoveTarget(GamePlayerInfoRef target)
{
    float theta = Vector2::CalculateAngle(GetCollider().GetPosition(), target->GetCollider().GetPosition());
    SetRotate(theta);

    float dist = Vector2::Magnitude(_collider.GetPosition() - target->GetCollider().GetPosition());
    if (dist < 1000.f)
    {
        // 몬스터의 공격상태로 변경시킨다
        SetObjecteState(ObjectState::ATTACK);
    }
    else
    {
        Vector2& curPos = GetCollider().GetPosition();
        if (dist > _moveSpeed)
        {
            curPos.X = curPos.X + (GameEngine::MathUtils::GetCos(GetCollider().GetRotate()) * _moveSpeed);
            curPos.Y = curPos.Y + (GameEngine::MathUtils::GetSin(GetCollider().GetRotate()) * _moveSpeed);
        }
        else
        {
            curPos.X = curPos.X + (GameEngine::MathUtils::GetCos(GetCollider().GetRotate()) * dist);
            curPos.Y = curPos.Y + (GameEngine::MathUtils::GetSin(GetCollider().GetRotate()) * dist);
        }
    }
}

bool GameMosterInfo::CheckAttackTarget(GamePlayerInfoRef target)
{
    if (GetCollider().IsTrigger(target->GetCollider()))
    {
        return true;
    }
    return false;
}

void GameMosterInfo::TakeDamage(int32 Damage)
{
    GameObjectInfo::TakeDamage(Damage);

    if (_hp <= 0)
    {
        SetObjecteState(ObjectState::DIE);
    }
}

int32 GameMosterInfo::AddAttackCounter(int32 count)
{
    int32 value = _AttackCounter.Add(count);
    if (value == _AttackCounter.GetTickValue() - 1)
        SetObjecteState(ObjectState::MOVE);
    return value;
}

int32 GameMosterInfo::AddIdleCounter(int32 count)
{
    int32 value = _IdleCounter.Add(count);
    if (value == _IdleCounter.GetTickValue() - 1)
        SetObjecteState(ObjectState::MOVE);
    return value;
}

int32 GameMosterInfo::AddHitCounter(int32 count)
{
    int32 value = _HitCounter.Add(count);
    if (value == _HitCounter.GetTickValue() - 1)
        SetObjecteState(ObjectState::MOVE);
    return value;
}

int32 GameMosterInfo::AddMoveCounter(int32 count)
{
    return _MoveCounter.Add(count);
}

int32 GameMosterInfo::AddDieCounter(int32 count)
{
    int32 value = _DieCounter.Add(count);
    if (value == _DieCounter.GetTickValue() - 1)
    {
        Spawn();
    }
    return value;
}

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
    _friendSystem.NotifyFriend(false);
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
    _friendSystem.NotifyFriend(true);
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
        return false; // 만렙

    _exp += exp;
    if (it->second._exp <= _exp)
    {
        // 레벨업
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
