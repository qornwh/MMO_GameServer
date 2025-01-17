#include "GameMosterInfo.h"
#include "GameGlobal.h"
#include "GameMapInfo.h"
#include "IRoom.h"

GameMosterInfo::GameMosterInfo(GameRoomRef gameRoom, int32 uuid, int32 code, int32 lv, int32 startX, int32 startY) : GameObjectInfo(gameRoom, uuid, code),
_startX(startX), _startY(startY), _genYaw(0, 360), _dropGenSystem(code)
{
    _lv = lv;

    _collider.SetRadius(40.f);
    _collider.SetHeight(40.f);
}

GameMosterInfo::~GameMosterInfo()
{
}

void GameMosterInfo::Spawn()
{
    SetPosition(static_cast<float>(_startX), static_cast<float>(_startY));
    SetObjecteState(ObjectState::IDLE);
    _dropGenSystem.InitGen();

    auto it = GMonster->GetCharater().find(_code);
    CrashFunc(it != GMonster->GetCharater().end());
    _maxHp = it->second._hp;
    _hp = _maxHp;
    _moveSpeed = it->second._moveSpeed;
    _exp = it->second._exp;
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
        _idleCounter.ResetTic();
        break;
    case ObjectState::DIE:
        _dieCounter.ResetTic();
        break;
    case ObjectState::HITED:
        _hitCounter.ResetTic();
        break;
    case ObjectState::MOVE:
        _moveCounter.ResetTic();
        break;
    case ObjectState::ATTACK:
        _attackCounter.ResetTic();
        break;
    default:
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
    }

    if (_yawCounter.Add() == 0)
    {
        SetRotate(static_cast<float>(_genYaw(rng)));
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
    int32 value = _attackCounter.Add(count);
    if (value == _attackCounter.GetTickValue() - 1)
        SetObjecteState(ObjectState::MOVE);
    return value;
}

int32 GameMosterInfo::AddIdleCounter(int32 count)
{
    int32 value = _idleCounter.Add(count);
    if (value == _idleCounter.GetTickValue() - 1)
        SetObjecteState(ObjectState::MOVE);
    return value;
}

int32 GameMosterInfo::AddHitCounter(int32 count)
{
    int32 value = _hitCounter.Add(count);
    if (value == _hitCounter.GetTickValue() - 1)
        SetObjecteState(ObjectState::MOVE);
    return value;
}

int32 GameMosterInfo::AddMoveCounter(int32 count)
{
    return _moveCounter.Add(count);
}

int32 GameMosterInfo::AddDieCounter(int32 count)
{
    int32 value = _dieCounter.Add(count);
    if (value == _dieCounter.GetTickValue() - 1)
    {
        Spawn();
    }
    return value;
}