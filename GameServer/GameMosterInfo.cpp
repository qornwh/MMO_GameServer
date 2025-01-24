#include "GameMosterInfo.h"
#include "GameGlobal.h"
#include "GameMapInfo.h"
#include "IRoom.h"

GameMosterInfo::GameMosterInfo(GameRoomRef gameRoom, int32 uuid, int32 code, int32 lv, int32 startX, int32 startY) : GameObjectInfo(gameRoom, uuid, code),
_startX(startX), _startY(startY), _genYaw(0, 360), _dropGenSystem(code)
{
    _collider.SetRadius(40.f);
    _collider.SetHeight(40.f);

    _ability.lv = lv;
}

GameMosterInfo::~GameMosterInfo()
{
}

void GameMosterInfo::Spawn()
{
    SetPosition(static_cast<float>(_startX), static_cast<float>(_startY));
    SetObjecteState(ObjectStateType::IDLE);
    _dropGenSystem.InitGen();

    auto it = GMonster->GetCharater().find(GetCode());
    CrashFunc(it != GMonster->GetCharater().end());
    _ability.maxHp = it->second._hp;
    _ability.hp = _ability.maxHp;
    _ability.speed = it->second._moveSpeed;
    _ability.exp = it->second._exp;
}

void GameMosterInfo::Update()
{
    GameObjectInfo::Update();
    GameRoomRef room = GetGameRoom();

    switch (GetObjectState())
    {
    case ObjectStateType::IDLE:
    {
        if (AddIdleCounter() == 0)
        {
            SetTarget(-1);
            protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
            childPkt->set_is_monster(true);
            protocol::Unit* monster = new protocol::Unit();
            monster->set_state(GetObjectState());
            monster->set_uuid(GetUUid());
            monster->set_code(GetCode());
            monster->set_hp(_ability.hp);
            monster->set_lv(_ability.lv);
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
    case ObjectStateType::MOVE:
    {
        if (AddMoveCounter() == 0)
        {
            Move();
            protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
            childPkt->set_is_monster(true);
            protocol::Unit* monster = new protocol::Unit();
            monster->set_state(ObjectStateType::MOVE);
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
    case ObjectStateType::READY_ATTACK:
    {
        if (AddReadyAttackCounter() == 0)
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
    case ObjectStateType::ATTACK:
    {
        if (AddAttackCounter() == 0)
        {
            room->AttackPlayer(_uuid);
            protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
            childPkt->set_is_monster(true);
            protocol::Unit* unit = new protocol::Unit();
            unit->set_state(GetObjectState());
            unit->set_uuid(GetUUid());
            childPkt->set_allocated_unit(unit);
        }
    }
    break;
    case ObjectStateType::HITED:
    {
        if (AddHitCounter() == 0)
        {
            protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
            childPkt->set_is_monster(true);
            protocol::Unit* unit = new protocol::Unit();
            unit->set_uuid(GetUUid());
            unit->set_hp(_ability.hp);
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
    case ObjectStateType::DIE:
    {
        if (AddDieCounter() == 0)
        {
            protocol::UnitState* childPkt = room->GetUnitPacket().add_unit_state();
            childPkt->set_is_monster(true);
            protocol::Unit* unit = new protocol::Unit();
            unit->set_state(ObjectStateType::DIE);
            unit->set_uuid(GetUUid());
            childPkt->set_allocated_unit(unit);
        }
    }
    break;
    default:
        break;
    }
}

void GameMosterInfo::SetObjecteState(ObjectStateType state)
{
    GameObjectInfo::SetObjecteState(state);

    switch (_state)
    {
    case ObjectStateType::IDLE:
        _idleCounter.ResetTic();
        break;
    case ObjectStateType::DIE:
        _dieCounter.ResetTic();
        break;
    case ObjectStateType::HITED:
        _hitCounter.ResetTic();
        break;
    case ObjectStateType::MOVE:
        _moveCounter.ResetTic();
        break;
    case ObjectStateType::ATTACK:
        _attackCounter.ResetTic();
        break;
    case ObjectStateType::READY_ATTACK:
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

    float moveX = GetCollider().GetPosition().X + GameEngine::MathUtils::GetSin(GetCollider().GetRotate()) * _ability.speed;
    float moveY = GetCollider().GetPosition().Y + GameEngine::MathUtils::GetCos(GetCollider().GetRotate()) * _ability.speed;
    GetGameRoom()->GetGameMap()->GetMonsterMapInfo()->InSetRect(moveX, moveY);
    SetPosition(moveX, moveY);
}

void GameMosterInfo::MoveTarget(GamePlayerInfoRef target)
{
    float theta = Vector2::CalculateAngle(GetCollider().GetPosition(), target->GetCollider().GetPosition());
    SetRotate(theta);

    float dist = Vector2::Magnitude(_collider.GetPosition() - target->GetCollider().GetPosition());
    if (dist < 150.f)
    {
        // 몬스터의 공격상태로 변경시킨다
        SetObjecteState(ObjectStateType::READY_ATTACK);
        cout << "Monster READY_ATTACK !!!" << '\n';
    }
    else
    {
        Vector2& curPos = GetCollider().GetPosition();
        if (dist > _ability.speed)
        {
            curPos.X = curPos.X + (GameEngine::MathUtils::GetCos(GetCollider().GetRotate()) * _ability.speed);
            curPos.Y = curPos.Y + (GameEngine::MathUtils::GetSin(GetCollider().GetRotate()) * _ability.speed);
        }
        else
        {
            curPos.X = curPos.X + (GameEngine::MathUtils::GetCos(GetCollider().GetRotate()) * dist);
            curPos.Y = curPos.Y + (GameEngine::MathUtils::GetSin(GetCollider().GetRotate()) * dist);
        }
    }
}

bool GameMosterInfo::AttackObjectCollision(int32 attackCode)
{
    auto targetPlayer = GetGameRoom()->GetPlayer(_targetUUid);
    if (_targetUUid > 0 && targetPlayer)
    {
        Vector2& pos = targetPlayer->GetCollider().GetPosition();
        if (GetGameRoom()->GetGameMap()->GetMonsterMapInfo()->InRect(pos.X, pos.Y))
        {
            float dist = Vector2::Magnitude(_collider.GetPosition() - targetPlayer->GetCollider().GetPosition());
            if (dist <= 250.f)
            {
                return true;
            }
        }
    }
    return false;
}

int32 GameMosterInfo::TakeDamage(int32 target, int32 Damage)
{
    _ability.hp -= Damage;

    if (_ability.hp <= 0)
    {
        _ability.hp = 0;
        SetObjecteState(ObjectStateType::DIE);
    }
    else
    {
        _targetUUid = target;
        SetObjecteState(ObjectStateType::HITED);
    }
    return _ability.hp;
}

int32 GameMosterInfo::AddReadyAttackCounter(int32 count)
{
    int32 value = _readyAttackCounter.Add(count);
    if (value == _readyAttackCounter.GetTickValue() - 1)
        SetObjecteState(ObjectStateType::ATTACK);
    return value;
}

int32 GameMosterInfo::AddAttackCounter(int32 count)
{
    int32 value = _attackCounter.Add(count);
    if (value == _attackCounter.GetTickValue() - 1)
        SetObjecteState(ObjectStateType::MOVE);
    return value;
}

int32 GameMosterInfo::AddIdleCounter(int32 count)
{
    int32 value = _idleCounter.Add(count);
    if (value == _idleCounter.GetTickValue() - 1)
        SetObjecteState(ObjectStateType::MOVE);
    return value;
}

int32 GameMosterInfo::AddHitCounter(int32 count)
{
    int32 value = _hitCounter.Add(count);
    if (value == _hitCounter.GetTickValue() - 1)
        SetObjecteState(ObjectStateType::MOVE);
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