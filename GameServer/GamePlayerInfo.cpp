#include "GamePlayerInfo.h"
#include "GameGlobal.h"
#include "GameMapInfo.h"
#include "GameExpLv.h"
#include "IRoom.h"

GamePlayerInfo::GamePlayerInfo(GameSessionRef gameSession, int32 playerCode, int32 uuid, int32 jobCode, int32 weaponCode, int32 lv) :
    GameObjectInfo(GRoomManger->getRoom(gameSession->GetRoomId()), uuid, jobCode), _targetCode(-1), _playerCode(-1), _gameSession(gameSession), _friendSystem(playerCode)
{
    _weaponCode = weaponCode;
    _playerCode = playerCode;
    _collider.SetRadius(40.f);
    _collider.SetHeight(40.f);

    auto it = GPlayer->GetCharater().find(GetCode());
    CrashFunc(it != GPlayer->GetCharater().end());
    _ability.maxHp = it->second._hp;
    _ability.hp = _ability.maxHp;
    _ability.lv = lv;
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

void GamePlayerInfo::AttackObject(bool isCreate, int32 attackNumber, float x, float y, float yaw)
{
    if (isCreate)
    {
        if (_attackObjs.find(attackNumber) == _attackObjs.end())
        {
            _attackObjs.emplace(attackNumber, std::make_shared<GameAttackInfo>(GetGameRoom(), attackNumber));
        }

        auto it = _attackObjs.find(attackNumber);
        auto attackInfoRef = it->second;
        attackInfoRef->StartAttack();
        attackInfoRef->SetPosition(x, y);
        attackInfoRef->SetRotate(yaw);
        attackInfoRef->SetPresentPosition(attackInfoRef->GetPosition());
    }
    else
    {
        auto it = _attackObjs.find(attackNumber);
        if (it != _attackObjs.end())
        {
            auto attackInfoRef = it->second;
            attackInfoRef->EndAttack();
        }
    }
}

void GamePlayerInfo::AttackObjectMove(int32 attackNumber, float x, float y, float yaw)
{
    if (_attackObjs.find(attackNumber) != _attackObjs.end())
    {
        auto attackInfoRef = _attackObjs.find(attackNumber)->second;
        attackInfoRef->SetPresentPosition(attackInfoRef->GetPosition());
        attackInfoRef->SetPosition(x, y);
    }
}

bool GamePlayerInfo::AttackObjectCollisionAndDamage(int32 attackNumber, int32 target)
{
    if (_attackObjs.find(attackNumber) != _attackObjs.end())
    {
        GameRoomRef room = GRoomManger->getRoom(_gameSession.lock()->GetRoomId());
        auto attackInfoRef = _attackObjs.find(attackNumber)->second;

        GameMosterInfoRef monster = room->GetMonster(target);
        if (monster != nullptr && monster->GetObjectState() != ObjectStateType::DIE)
        {
            bool result = monster->GetCollider().Trigger(attackInfoRef->GetCollider(), attackInfoRef->GetPresentPosition(), attackInfoRef->GetPosition());

            if (result)
            {
                // 공격 성공
                cout << "attack OCK KK  " << '\n';
                monster->TakeDamage(_uuid, _ability.attack);
                return true;
            }
        }
    }
    return false;
}

void GamePlayerInfo::UpdateAblity()
{
    auto it = GPlayer->GetCharater().find(GetCode());
    float per = _ability.lv + static_cast<float>(_ability.lv - 1);
    int32 attack = static_cast<int32>(it->second._attack * per);
    int32 moveSpeed = static_cast<int32>(it->second._moveSpeed * per);

    for (auto& item : _inventory.GetEquippedItemInfo())
    {
        if (item._attack > 0)
            attack += item._attack;
        if (item._speed > 0)
            moveSpeed += item._speed;
    }

    _ability.attack = attack;
    _ability.speed = moveSpeed;
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
    _ability.hp = _ability.maxHp;
}

bool GamePlayerInfo::AddExp(int32 exp)
{
    int32 lv = _ability.lv;
    auto it = GExpLv->GetExpLv().find(lv);
    CrashFunc(it != GExpLv->GetExpLv().end());

    if (it->second._exp <= 0)
        return false; // 만렙

    _ability.exp += exp;
    if (it->second._exp <= _ability.exp)
    {
        // 레벨업
        _ability.lv += 1;
        _ability.exp = _ability.exp - it->second._exp;
    }

    return true;
}

void GamePlayerInfo::SetExp(int32 exp)
{
    _ability.exp = exp;
}

void GamePlayerInfo::SetDummy(bool flag)
{
    _dummy = flag;
}
