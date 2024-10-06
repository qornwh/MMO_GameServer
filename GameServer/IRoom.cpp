#include "IRoom.h"
#include "GameMapInfo.h"
#include "GamePacketHandler.h"
#include "GameService.pb.h"
#include "GameUserAccess.h"
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
        unit->set_hp(player->GetHp());
        unit->set_lv(player->GetLv());

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
        ReadLockGuard readLock(lock);
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
                    unit->set_hp(info->GetHp());
                    unit->set_lv(info->GetLv());
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
            // unit->set_name(info->GetName()); // 몬스터는 이름 필요 x
            unit->set_code(info->GetCode());
            unit->set_uuid(info->GetUUid());
            unit->set_hp(info->GetHp());
            unit->set_lv(info->GetLv());
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

        for (auto& entry : inventory.GetEquipItemInfo())
        {
            auto& item = entry.second;
            if (item._use == 1) continue;
            protocol::ItemEquip* itemEquip = sendPktInven.add_itemequips();
            itemEquip->set_unipeid(item._uniqueId);
            itemEquip->set_is_equip(item._isEquip);
            itemEquip->set_item_type(item._equipType);
            itemEquip->set_attack(item._attack);
            itemEquip->set_speed(item._speed);
            itemEquip->set_item_code(item._itemCode);
            itemEquip->set_position(item._position);
        }

        for (auto& entry : inventory.GetEtcItemInfo())
        {
            auto& item = entry.second;
            if (item._count == 0) continue;
            protocol::ItemEtc* itemEtc = sendPktInven.add_itemetcs();
            itemEtc->set_item_code(item._itemCode);
            itemEtc->set_item_count(item._count);
            itemEtc->set_item_type(item._type);
            itemEtc->set_position(item._position);
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
            Work();
            _timer = current;
        }
        isLoopTask.exchange(false);
    }
}

void GameRoom::Work()
{
#pragma region Work
    const MapType mapType = _gameMapInfo->GetMonsterMapInfo()->GetMapType();
    const MapInfoRef monsterMap = _gameMapInfo->GetMonsterMapInfo();
#pragma endregion
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

void GameRoom::Attack(GameSessionRef session, bool isMonster, int32 demage, int32 uuid, float x, float y, float yaw)
{
    DWORD dwNumberOfBytesTransferred = 0;
    ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
    OverlappedTask* overlapped = new OverlappedTask();
    overlapped->f = [this, session, isMonster, demage, uuid, x, y, yaw]
    {
        if (!isMonster)
        {
            session->GetPlayer()->TakeDamage(demage);
            protocol::SUnitStates sendPkt;
            protocol::UnitState* unitState = sendPkt.add_unit_state();
            protocol::Unit* unit = new protocol::Unit();
            unitState->set_is_monster(false);
            unit->set_uuid(session->GetPlayer()->GetUUid());
            if (session->GetPlayer()->GetHp() <= 0)
            {
                unit->set_state(ObjectState::DIE);
                session->GetPlayer()->ReSpawn();
            }
            else
            {
                unit->set_state(ObjectState::HITED);
            }
            unit->set_hp(session->GetPlayer()->GetHp());
            unitState->set_allocated_unit(unit);

            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_UNITSTATES);
            BroadCast(sendBuffer);
        }
        else
        {
            if (_monsterMap[uuid]->GetObjectState() == ObjectState::DIE) return; // 이미 처리됨
            _monsterMap[uuid]->SetTarget(session->GetPlayer()->GetUUid());
            _monsterMap[uuid]->SetPosition(x, y);
            _monsterMap[uuid]->SetRotate(yaw);
            _monsterMap[uuid]->SetObjecteState(ObjectState::HITED);
            _monsterMap[uuid]->TakeDamage(demage);
            if (_monsterMap[uuid]->GetObjectState() == ObjectState::DIE)
            {
                // 경험치 증가
                session->AddExp(_monsterMap[uuid]->GetExp());
                // 아이템 드롭
                session->DropItem(_monsterMap[uuid]);
            }
        }
    };
    PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
}

void GameRoom::Heal(GameSessionRef session, int32 heal, int32 uuid)
{
    DWORD dwNumberOfBytesTransferred = 0;
    ULONG_PTR dwCompletionKey = _taskId.fetch_add(1);
    OverlappedTask* overlapped = new OverlappedTask();
    overlapped->f = [this, session, heal, uuid]
    {
        session->GetPlayer()->TakeHeal(heal);
        protocol::SUnitStates sendPkt;
        protocol::UnitState* unitState = sendPkt.add_unit_state();
        protocol::Unit* unit = new protocol::Unit();
        unitState->set_is_monster(false);
        unit->set_uuid(uuid);
        unit->set_state(ObjectState::HEAL);
        unit->set_hp(session->GetPlayer()->GetHp());
        unitState->set_allocated_unit(unit);
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_UNITSTATES);
        BroadCast(sendBuffer);
    };
    PostQueuedCompletionStatus(_taskIo, dwNumberOfBytesTransferred, dwCompletionKey, reinterpret_cast<LPOVERLAPPED>(overlapped));
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
        ReadLockGuard readLock(lock);
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
    unit->set_hp(gameSession->GetPlayer()->GetHp());
    unit->set_lv(gameSession->GetPlayer()->GetLv());

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
