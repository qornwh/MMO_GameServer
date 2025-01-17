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

    _collider.SetRadius(40.f);
    _collider.SetHeight(40.f);
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

void GamePlayerInfo::AttackObjectCollision(int32 attackNumber, Vector<int32> attackList)
{
    if (_attackObjs.find(attackNumber) != _attackObjs.end())
    {
        auto attackInfoRef = _attackObjs.find(attackNumber)->second;

        for (int32 monsterNumber : attackList)
        {
            GameMosterInfoRef monsnter = GetGameRoom()->GetMonster(monsterNumber);
            if (monsnter != nullptr)
            {
                bool result = monsnter->GetCollider().Trigger(attackInfoRef->GetCollider(), attackInfoRef->GetPresentPosition(), attackInfoRef->GetPosition());

                if (result)
                {
                    // 공격 성공
                    cout << "attack OCK KK  " << '\n';
                }
            }
        }
    }
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
