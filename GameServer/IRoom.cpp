#include "IRoom.h"
#include "GameMapInfo.h"
#include "GamePacketHandler.h"
#include "GameService.pb.h"
#include "GameUserAccess.h"
#include "GameSkill.h"
#include "MailSystem.h"

void GameRoom::EnterSession(SessionRef session)
{
    GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
    _playerMap.emplace(gameSession->GetPlayer()->GetUUid(), gameSession->GetPlayer());
    gameSession->SetRoomId(_id);
    {
        protocol::SInsertplayer sendPktPlayer;
        protocol::Unit* unit = new protocol::Unit();
        auto player = gameSession->GetPlayer();
        unit->set_name(player->GetName());
        unit->set_code(player->GetCode());
        unit->set_weaponcode(player->GetWeapon());
        unit->set_uuid(player->GetUUid());
        unit->set_hp(player->GetAbility().hp);
        unit->set_lv(player->GetAbility().lv);

        protocol::Position* position = new protocol::Position;
        position->set_x(player->GetCollider().GetPosition().X);
        position->set_y(player->GetCollider().GetPosition().Y);
        position->set_z(0);
        position->set_yaw(player->GetCollider().GetRotate());
        unit->set_allocated_position(position);
        sendPktPlayer.set_allocated_player(unit);

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPktPlayer, protocol::MessageCode::S_INSERTPLAYER);
        BroadCastAnother(sendBuffer, player->GetUUid());
        GUserAccess->AccessPlayer(player->GetPlayerCode(), session);
    }

    {
        protocol::SLoad sendPktLoad;
        sendPktLoad.set_room_id(_id);
        for (auto it : _sessionList)
        {
            if (it != nullptr)
            {
                GameSessionRef anthorGameSession = std::static_pointer_cast<GameSession>(it);

                if (anthorGameSession->GetSessionId() == gameSession->GetSessionId())
                    continue;

                if (anthorGameSession->GetPlayer() != nullptr)
                {
                    std::shared_ptr<GamePlayerInfo> info = anthorGameSession->GetPlayer();
                    protocol::UnitState* unitState = sendPktLoad.add_unit();
                    protocol::Unit* unit = new protocol::Unit();
                    unit->set_name(info->GetName());
                    unit->set_code(info->GetCode());
                    unit->set_uuid(info->GetUUid());
                    unit->set_hp(info->GetAbility().hp);
                    unit->set_lv(info->GetAbility().lv);
                    unit->set_weaponcode(info->GetWeapon());
                    protocol::Position* position = new protocol::Position;
                    position->set_x(info->GetCollider().GetPosition().X);
                    position->set_y(info->GetCollider().GetPosition().Y);
                    position->set_z(0);
                    position->set_yaw(info->GetCollider().GetRotate());
                    unit->set_allocated_position(position);
                    unitState->set_allocated_unit(unit);
                    unitState->set_is_monster(false);
                }
            }
        }

        for (auto& it : _monsterMap)
        {
            int32 uuid = it.first;
            GameMosterInfoRef info = it.second;
            protocol::UnitState* unitState = sendPktLoad.add_unit();
            protocol::Unit* unit = new protocol::Unit();
            unit->set_code(info->GetCode());
            unit->set_uuid(info->GetUUid());
            unit->set_hp(info->GetAbility().hp);
            unit->set_lv(info->GetAbility().lv);
            protocol::Position* position = new protocol::Position;
            position->set_x(info->GetCollider().GetPosition().X);
            position->set_y(info->GetCollider().GetPosition().Y);
            position->set_z(0);
            position->set_yaw(info->GetCollider().GetRotate());
            unit->set_allocated_position(position);
            unit->set_state(info->GetObjectState());
            unitState->set_allocated_unit(unit);
            unitState->set_is_monster(true);
        }

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPktLoad, protocol::MessageCode::S_LOAD);
        session->AsyncWrite(sendBuffer);
    }

    {
        protocol::SLoadInventory sendPktInven;
        Inventory& inventory = gameSession->GetPlayer()->GetInventory();
        sendPktInven.set_gold(inventory.GetGold());

        for (auto& item : inventory.GetEquipItemInfo())
        {
            if (item.IsEmpty()) continue;
            protocol::ItemEquip* itemEquip = sendPktInven.add_itemequips();
            itemEquip->set_equippos(item._equipPos);
            itemEquip->set_item_type(item._equipType);
            itemEquip->set_attack(item._attack);
            itemEquip->set_speed(item._speed);
            itemEquip->set_item_code(item._itemCode);
            itemEquip->set_invenpos(item._invenPos);
        }

        for (auto& item : inventory.GetEquippedItemInfo())
        {
            if (item.IsEmpty()) continue;
            protocol::ItemEquip* itemEquip = sendPktInven.add_itemequips();
            itemEquip->set_equippos(item._equipPos);
            itemEquip->set_item_type(item._equipType);
            itemEquip->set_attack(item._attack);
            itemEquip->set_speed(item._speed);
            itemEquip->set_item_code(item._itemCode);
            itemEquip->set_invenpos(item._invenPos);
        }

        for (auto& item : inventory.GetEtcItemInfo())
        {
            if (item.IsEmpty()) continue;
            protocol::ItemEtc* itemEtc = sendPktInven.add_itemetcs();
            itemEtc->set_item_code(item._itemCode);
            itemEtc->set_item_count(item._count);
            itemEtc->set_item_type(item._type);
            itemEtc->set_invenpos(item._invenPos);
        }
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPktInven, protocol::MessageCode::S_LOADINVENTORY);
        session->AsyncWrite(sendBuffer);
    }

    {
        gameSession->LoadMails();
    }

    IRoom::EnterSession(session);
    std::cout << "Enter SessionID: " << gameSession->GetPlayer()->GetUUid() << " Name: " << gameSession->GetPlayer()->GetName() << std::endl;
}

void GameRoom::OutSession(SessionRef session)
{
    GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
    _playerMap.erase(gameSession->GetPlayer()->GetUUid());
    gameSession->SetRoomId(-1);

    protocol::SClosePlayer sendPkt;
    sendPkt.set_uuid(gameSession->GetPlayer()->GetUUid());
    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_CLOSEPLAYER);
    BroadCastAnother(sendBuffer, gameSession->GetPlayer()->GetUUid());
    IRoom::OutSession(session);
    
    std::cout << "Out SessionID: " << gameSession->GetPlayer()->GetUUid() << " Name: " << gameSession->GetPlayer()->GetName() << std::endl;
}

void GameRoom::StartGameRoom()
{
    std::wcout << "GameRoom number: " << _id << " start" << std::endl;
    Tick();
}

void GameRoom::Tick()
{
    task();
    bool curLoopTask = isLoopTask.load();
    if (!curLoopTask && isLoopTask.compare_exchange_strong(curLoopTask, true))
    {
        std::chrono::system_clock::time_point current = std::chrono::system_clock::now();
        std::chrono::duration<double> sec = current - _timer;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(sec).count() >= _timerDelay)
        {
            DWORD dwNumberOfBytesTransferred = 0;
            ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
            OverlappedTask* overlapped = new OverlappedTask();
            overlapped->f = [this] { Update(); };
            _timer = current;
            PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
        }
        isLoopTask.exchange(false);
    }
}

void GameRoom::Update()
{
    const MapType mapType = _gameMapInfo->GetMonsterMapInfo()->GetMapType();
    const MapInfoRef monsterMap = _gameMapInfo->GetMonsterMapInfo();
    for (auto& it : _monsterMap)
    {
        GameMosterInfoRef info = it.second;
        info->Update();
    }

    if (_unitPkt.unit_state_size() > 0)
    {
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(_unitPkt, protocol::MessageCode::S_UNITSTATES);
        BroadCast(sendBuffer);
        _unitPkt.clear_unit_state();
    }
}

GameMapInfoRef GameRoom::CreateMapInfo(int32 type)
{
    _gameMapInfo = std::make_shared<GameMapInfo>(0, 0, 0, 0);
    _gameMapInfo->CreateMonsterMapInfo(0, 0, 0, 0, MapType::MONSTER);

    return _gameMapInfo;
}

void GameRoom::CreateMonster(int32 type, int32 count)
{
    Rect& rect = _gameMapInfo->GetMonsterMapInfo()->GetRect();
    std::uniform_int_distribution<> genX(rect.StartX(), rect.EndX());
    std::uniform_int_distribution<> genY(rect.StartY(), rect.EndY());
    
    for (int i = 0; i < count; i++)
    {
        ++_monsterCount;
        int32 startX = genX(rng);
        int32 startZ = genY(rng);
        GameMosterInfoRef info = std::make_shared<GameMosterInfo>(std::static_pointer_cast<GameRoom>(shared_from_this()), _monsterCount, type, 1, startX, startZ);
        info->Spawn();
        _monsterMap[_monsterCount] = info;
    }
}

void GameRoom::BroadCastAnother(SendBufferRef sendBuffer, int32 uuid)
{
    DWORD dwNumberOfBytesTransferred = 0;
    ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
    OverlappedTask* overlapped = new OverlappedTask();
    overlapped->f = [this, sendBuffer, uuid]()
    {
        for (auto session : _sessionList)
        {
            GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
            if (gameSession->GetPlayer()->GetUUid() != uuid)
            {
                gameSession->AsyncWrite(sendBuffer);
            }
        }
    };
    PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
}

void GameRoom::AttackObject(GameSessionRef session, int32 attackNumber, int32 skillCode, float x, float y, float yaw)
{
    DWORD dwNumberOfBytesTransferred = 0;
    ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
    OverlappedTask* overlapped = new OverlappedTask();
    overlapped->f = [this, session, attackNumber, skillCode, x, y, yaw]
    {
        GamePlayerInfoRef info = session->GetPlayer();
        info->AttackObject(true, attackNumber, skillCode, x, y, yaw);
    };
    PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));

}

void GameRoom::AttackObjectMove(GameSessionRef session, int32 attackNumber, int32 skillCode, Vector<int32> targets, float x, float y, float yaw)
{
    DWORD dwNumberOfBytesTransferred = 0;
    ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
    OverlappedTask* overlapped = new OverlappedTask();
    overlapped->f = [this, session, attackNumber, skillCode, targets, x, y, yaw]
    {
        GamePlayerInfoRef info = session->GetPlayer();
        info->AttackObjectMove(attackNumber, x, y, yaw);

        if (targets.size() > 0)
        {
            protocol::SUnitDemage sendPkt;
            for (int32 target : targets)
            {
                bool result = info->AttackObjectCollision(attackNumber, target);
                if (result)
                {
                    GameMosterInfoRef monsterInfo = GetMonster(target);

                    int32 damage = info->GetAbility().attack;
                    auto it = GSkill->GetSkill().find(skillCode);
                    if (it != GSkill->GetSkill().end())
                    {
                        damage = (it->second._value / 100.f) * damage;
                    }
                    monsterInfo->TakeDamage(info->GetUUid(), damage);

                    protocol::Demage* unit = sendPkt.add_demage();
                    unit->set_uuid(target);
                    unit->set_is_heal(false);
                    unit->set_is_monster(true);
                    unit->set_demage(damage);

                    if (monsterInfo->GetObjectState() == ObjectStateType::DIE)
                    {
                        // 경험치 증가
                        session->AddExp(monsterInfo->GetAbility().exp);
                        // 아이템 드롭
                        session->DropItem(monsterInfo);
                    }
                }
            }

            if (sendPkt.demage_size() > 0)
            {
                SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_UNITDEMAGE);
                BroadCast(sendBuffer);
            }
        }
    };
    PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
}

void GameRoom::AttackPlayer(int32 uuid)
{
    DWORD dwNumberOfBytesTransferred = 0;
    ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
    OverlappedTask* overlapped = new OverlappedTask();
    overlapped->f = [this, uuid]
    {
        GameMosterInfoRef monsterInfo = GetMonster(uuid);

        bool result = monsterInfo->AttackObjectCollision();
        if (result)
        {
            auto targetPlayer = GetPlayer(monsterInfo->GetTarget());
            int32 damage = monsterInfo->GetAbility().attack;
            targetPlayer->TakeDamage(monsterInfo->GetUUid(), damage);
            
            {
                protocol::SUnitDemage sendPkt;
                protocol::Demage* unit = sendPkt.add_demage();
                unit->set_uuid(targetPlayer->GetUUid());
                unit->set_is_heal(false);
                unit->set_is_monster(false);
                unit->set_demage(damage);

                SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_UNITDEMAGE);
                BroadCast(sendBuffer);
            }

            {
                protocol::SUnitStates sendPkt;
                protocol::UnitState* unitState = sendPkt.add_unit_state();
                protocol::Unit* unit = new protocol::Unit();
                unitState->set_is_monster(false);
                unit->set_uuid(targetPlayer->GetUUid());
                unit->set_state(ObjectStateType::HITED);
                unit->set_hp(targetPlayer->GetAbility().hp);
                unitState->set_allocated_unit(unit);
                SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_UNITSTATES);
                BroadCast(sendBuffer);
            }
        }
    };
    PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
}

void GameRoom::HealObject(GameSessionRef session, int32 skillCode)
{
    DWORD dwNumberOfBytesTransferred = 0;
    ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
    OverlappedTask* overlapped = new OverlappedTask();
    overlapped->f = [this, session, skillCode]
    {
        GamePlayerInfoRef info = session->GetPlayer();
        auto it = GSkill->GetSkill().find(skillCode);

        if (it != GSkill->GetSkill().end())
        {
            int32 heal = it->second._value;
            info->TakeHeal(heal);
        }

        protocol::SUnitStates sendPkt;
        protocol::UnitState* unitState = sendPkt.add_unit_state();
        protocol::Unit* unit = new protocol::Unit();
        unitState->set_is_monster(false);
        unit->set_uuid(info->GetUUid());
        unit->set_state(ObjectStateType::HEAL);
        unit->set_hp(info->GetAbility().hp);
        unitState->set_allocated_unit(unit);
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_UNITSTATES);
        BroadCast(sendBuffer);
    };
    PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
}

void GameRoom::EnterDummySession(SessionRef session)
{
    GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
    _playerMap.emplace(gameSession->GetPlayer()->GetUUid(), gameSession->GetPlayer());
    gameSession->SetRoomId(_id);

    protocol::SInsertplayer sendPkt;
    protocol::Unit* unit = new protocol::Unit();
    unit->set_name(gameSession->GetPlayer()->GetName());
    unit->set_code(gameSession->GetPlayer()->GetCode());
    unit->set_uuid(gameSession->GetPlayer()->GetUUid());
    unit->set_hp(100);
    unit->set_lv(1);

    protocol::Position* position = new protocol::Position;
    position->set_x(gameSession->GetPlayer()->GetCollider().GetPosition().X);
    position->set_y(gameSession->GetPlayer()->GetCollider().GetPosition().Y);
    position->set_z(0);
    position->set_yaw(gameSession->GetPlayer()->GetCollider().GetRotate());
    unit->set_allocated_position(position);
    sendPkt.set_allocated_player(unit);

    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_INSERTPLAYER);
    BroadCastAnother(sendBuffer, gameSession->GetPlayer()->GetUUid());

    IRoom::EnterSession(session);
    std::cout << "Enter SessionID: " << gameSession->GetPlayer()->GetUUid() << " Name: " << gameSession->GetPlayer()->GetName() << std::endl;
}
