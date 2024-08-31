#include "GameBossInfo.h"
#include "GameGlobal.h"
#include "GameSkill.h"
#include "IRoom.h"

GameBossInfo::GameBossInfo(GameRoomRef gameRoom, int32 uuid, int32 code, int32 hp, int32 startX, int32 startY):
    GameMosterInfo(gameRoom, uuid, code, hp, startX, startY),
    _targetCode(-1), genYaw(0, 360)
{
    // 보스는 공격을 3초에 한번씩 한다고 친다. ( 일정 범위에 있으면 맞음 )
    // 보스는 항상 슈퍼아머상태이다.
    // 보스는 70, 40인 경우 특수 공격을 한다. ( 큐에 함수를 넣는 방범을 생각해 본다 )
    // 보스는 이동 x, 방향 x
    // 보스가 공격, 특수 공격 진행시에는 데미지가 들어가지 않는다. ( 사실상 무조건 패턴 발동 )

    _SkillQueue.emplace_back([objRef = this](const int hp)
    {
        return objRef->ReadyAttackSkill2(hp);
    });

    _SkillQueue.emplace_back([objRef = this](const int hp)
    {
        return objRef->ReadyAttackSkill1(hp);
    });

    // 일단 이거 좀 예쁘게 만들자 !!!
    _AttackCounter = GameUtils::TickCounter(20);
    _collider.ResetCollider(0.9f);
    _collider.SetRotate(180);
    _maxHp = 300;
}

GameBossInfo::~GameBossInfo()
{
}

void GameBossInfo::SetObjecteState(ObjectState state)
{
    if (state != ObjectState::HITED)
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
        // _HitCounter.ResetTic();
        // 보스는 슈퍼아머상태.
        break;
    case ObjectState::MOVE:
        _MoveCounter.ResetTic();
        break;
    case ObjectState::ATTACK:
    case ObjectState::SKILL1:
        _AttackCounter.ResetTic();
        break;
    case ObjectState::READY_ATTACK:
        _ReadyAttackCounter.ResetTic();
        break;
    }
}

void GameBossInfo::Move()
{
    // 이동 x
    SetObjecteState(ObjectState::READY_ATTACK);
    if (!_SkillQueue.empty())
    {
        auto& fn = _SkillQueue.back();
        if (fn(GetHp()))
        {
            _SkillQueue.pop_back();
        }
    }
}

void GameBossInfo::MoveTarget(GamePlayerInfoRef target)
{
    // 이동 x
}

int32 GameBossInfo::AddReadyAttackCounter(int count)
{
    int32 value = _ReadyAttackCounter.Add(count);
    if (value == _ReadyAttackCounter.GetTickValue() - 1)
        SetObjecteState(ObjectState::ATTACK);
    return value;
}

int32 GameBossInfo::AddDieCounter(int count)
{
    int32 value = _DieCounter.Add(count);
    if (value == _DieCounter.GetTickValue() - 1)
    {
        SetPosition(_startX, _startY);
        _prePosition = _collider.GetPosition();
        _increaseX = 0;
        _increaseY = 0;
        _hp = 100;
        _targetCode = -1;
        SetObjecteState(ObjectState::IDLE);
    }
    return value;
}

bool GameBossInfo::ReadyAttackSkill1(int hp)
{
    if (hp <= _maxHp * 0.7f)
    {
        SetObjecteState(ObjectState::SKILL1);
        return true;
    }
    return false;
}

bool GameBossInfo::ReadyAttackSkill2(int hp)
{
    if (hp <= _maxHp * 0.4f)
    {
        SetObjecteState(ObjectState::SKILL2);
        return true;
    }
    return false;
}

void GameBossInfo::Attack(GamePlayerInfoRef target, Vector<int32>& attackList)
{
    // Skill& skill = GSkill->GetMonsterSkill()[GetType()].GetSkillMap()[GetObjectState()];
    // int32 skillType = skill._type;
}

bool GameBossInfo::AttackRect(Vector2 position, GameObjectInfoRef target)
{
    // float rangeX = GSkill->GetMonsterSkill()[GetType()].GetSkillMap()[GetObjectState()]._width;
    // float rangeY = GSkill->GetMonsterSkill()[GetType()].GetSkillMap()[GetObjectState()]._height;
    //
    // Collider attackCollider(rangeX, rangeY);
    // attackCollider.SetPosition(GetPosition().X, GetPosition().Y);
    // attackCollider.SetRotate(_collider.GetRotate());
    // if (attackCollider.IsTrigger(target->GetCollider()))
    //     return true;
    return false;
}

bool GameBossInfo::AttackCircle(Vector2 position, GameObjectInfoRef target)
{
    // float radius = GSkill->GetMonsterSkill()[GetType()].GetSkillMap()[GetObjectState()]._radius;
    //
    // Collider attackCollider(radius);
    // attackCollider.SetPosition(GetPosition().X, GetPosition().Y);
    // attackCollider.SetRotate(_collider.GetRotate());
    //
    // if (attackCollider.IsTrigger(target->GetCollider()))
    //     return true;

    return false;
}

void GameBossInfo::Update()
{
    // GameMosterInfo::Update();

    GameRoomRef room = GetGameRoom();

    if (room == nullptr)
    {
        // crash !!!
        return;
    }

    
}

GameBossArrow::GameBossArrow(GameRoomRef gameRoom, int32 uuid, int32 code, int32 hp, int32 startX, int32 startZ) :
    GameObjectInfo(gameRoom, uuid, code)
{
    _collider.ResetCollider(4, 10);
    _increaseY = _speed / _MoveCounter.GetTickValue();
}

GameBossArrow::~GameBossArrow()
{
}

void GameBossArrow::Update()
{
    // GameObjectInfo::Update();

    _prePosition.Y += _increaseY;

    GameRoomRef room = GetGameRoom();

    Vector<int32> players;
    for (auto it : room->GetPlayerMap())
    {
        GamePlayerInfoRef player = it.second;

        if (GetCollider().IsTrigger(player->GetCollider()))
        {
            if (_players.find(player->GetCode()) != _players.end())
            {
                if (_players[player->GetCode()] == 0)
                {
                    _players[player->GetCode()] = 1;
                }
                else if (_players[player->GetCode()] == 1)
                {
                    _players[player->GetCode()] = 2;
                }
                else if (_players[player->GetCode()] == 2)
                {
                    // 그대로 충돌 상태 유지
                }
            }
            else
            {
                _players[player->GetCode()] = 1;
            }
        }
        else
        {
            if (_players.find(player->GetCode()) != _players.end())
            {
                _players[player->GetCode()] = 0;
            }
        }
    }

    for (auto it : _players)
    {
        if (it.second == 1)
        {
            // 데미지 전달.
            GamePlayerInfoRef playerInfo = room->GetPlayerMap()[it.first];
            
            // 플레이어 히트
            // protocol::UnitState* childPkt2 = room->GetUnitPacket().add_unit_state();
            // childPkt2->set_is_monster(false);
            // childPkt2->set_demage(playerInfo->GetDamage());
            // playerInfo->ResetDamage();
            // protocol::Player* _player = new protocol::Player();
            // protocol::Unit* unit2 = new protocol::Unit();
            // unit2->set_code(playerInfo->GetCode());
            // unit2->set_hp(playerInfo->GetHp());
            // _player->set_allocated_unit(unit2);
            // childPkt2->set_allocated_player(_player);
        }
    }
}
