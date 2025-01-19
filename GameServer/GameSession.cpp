#include "pch.h"
#include "GameSession.h"

#include "GameEvent.h"
#include "GameGlobal.h"
#include "GameItem.h"
#include "GameService.pb.h"
#include "PacketHeader.h"
#include "GamePacketHandler.h"
#include "GameRoomManager.h"
#include "GameObjectInfo.h"
#include "GameMosterInfo.h"
#include "GamePlayerInfo.h"
#include "GameUserAccess.h"
#include "IRoom.h"
#include "SessionDB.h"

GameSession::GameSession(EndPointUtil& ep) : Session(ep)
{
    static Atomic<uint16> GameSessionId(0);
    GameSessionId.fetch_add(1);
    _sessionId = GameSessionId;
}

GameSession::~GameSession()
{
}

int32 GameSession::OnRecv(BYTE* buffer, int32 len)
{
    int32 offset = 0;
    while (true)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(&buffer[offset]);
        if (!GamePacketHandler::CheckPacketHeader(header, offset, len))
            break;
        HandlePacket(buffer, header);
        offset += header->size;
    }
    return offset;
}

void GameSession::CreatePlayerInfo(int32 jobCode, int32 weaponCode, int32 lv)
{
    _player = std::make_shared<GamePlayerInfo>(std::reinterpret_pointer_cast<GameSession>(shared_from_this()), _playerCode, _sessionId, jobCode, weaponCode,
                                               lv);
}

std::shared_ptr<GamePlayerInfo> GameSession::GetPlayer()
{
    return _player;
}

void GameSession::SetRoomId(uint32 id)
{
    _roomId = id;
}

void GameSession::AddExp(int32 exp)
{
    // IRoom에서 호출된다 lock 문제 x
    if (GetPlayer()->AddExp(exp))
    {
        SessionDB sdb;
        sdb.UpdateExp(_playerCode, GetPlayer()->GetAbility().exp, GetPlayer()->GetAbility().lv);

        protocol::SExpLv pkt;

        pkt.set_uuid(GetPlayer()->GetUUid());
        pkt.set_exp(GetPlayer()->GetAbility().exp);
        pkt.set_lv(GetPlayer()->GetAbility().lv);

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_EXPLV);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::DropItem(std::shared_ptr<GameMosterInfo> monster)
{
    // IRoom에서 호출된다 lock 문제 x
    if (!monster) return;

    protocol::UpdateInventory pkt;
    auto& inventory = GetPlayer()->GetInventory();
    auto& monsterDrop = monster->GetDropSystem();
    for (auto& dropItem : monsterDrop.GetDropEquipList())
    {
        EquipItem& item = inventory.AddItemEquip(dropItem);

        if (item._invenPos >= 0)
        {
            protocol::ItemEquip* itemEquip = pkt.add_itemequips();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEquip->set_item_code(item._itemCode);
            itemEquip->set_item_type(item._equipType);
            itemEquip->set_attack(item._attack);
            itemEquip->set_speed(item._speed);
            itemEquip->set_equippos(item._equipPos);
            itemEquip->set_invenpos(item._invenPos);
        }
        else
        {
            GEvent->DropEquipMail(_playerCode, item);
        }
    }
    for (auto& dropItem : monsterDrop.GetDropEtcList())
    {
        EtcItem& item = inventory.AddItemEtc(dropItem);

        if (item._invenPos >= 0)
        {
            protocol::ItemEtc* itemEtc = pkt.add_itemetcs();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEtc->set_item_code(item._itemCode);
            itemEtc->set_item_count(item._count);
            itemEtc->set_item_type(item._type);
            itemEtc->set_invenpos(item._invenPos);
        }
        else
        {
            GEvent->DropEtcMail(_playerCode, item);
        }
    }
    inventory.AddGold(monsterDrop.GetDropGold());
    int32 gold = inventory.GetGold();
    pkt.set_gold(gold);

    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::UPDATEINVENTROY);
    AsyncWrite(sendBuffer);
}

void GameSession::UpdateItems()
{
    protocol::UpdateInventory pkt;
    auto& inventory = GetPlayer()->GetInventory();

    for (auto& item : inventory.GetUpdateEquipList())
    {
        if (item._invenPos >= 0)
        {
            protocol::ItemEquip* itemEquip = pkt.add_itemequips();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEquip->set_item_code(item._itemCode);
            itemEquip->set_item_type(item._equipType);
            itemEquip->set_attack(item._attack);
            itemEquip->set_speed(item._speed);
            itemEquip->set_equippos(item._equipPos);
            itemEquip->set_invenpos(item._invenPos);
        }
    }
    for (auto& item : inventory.GetUpdateEtcList())
    {
        if (item._invenPos >= 0)
        {
            protocol::ItemEtc* itemEtc = pkt.add_itemetcs();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEtc->set_item_code(item._itemCode);
            itemEtc->set_item_count(item._count);
            itemEtc->set_item_type(item._type);
            itemEtc->set_invenpos(item._invenPos);
        }
    }
    int32 gold = inventory.GetGold();
    pkt.set_gold(gold);

    inventory.ResetUpdateItems();
    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::UPDATEINVENTROY);
    AsyncWrite(sendBuffer);
}

void GameSession::LoadMails()
{
    protocol::CLoadMail sendPktMail;
    sendPktMail.set_type(0);
    MailSystem& mailSystem = GetPlayer()->GetMail();

    // 메일 
    char titleCStr[20] = {0,};
    char messageCStr[100] = {0,};
    String titleStr;
    String messageStr;
    titleStr.reserve(20);
    messageStr.reserve(100);
    for (auto& entry : mailSystem.GetMail())
    {
        auto& mailInfo = entry.second;
        protocol::Mail* mail = sendPktMail.add_mails();
        mail->set_code(mailInfo._code);
        mail->set_read(mailInfo._read);
        mail->set_gold(mailInfo._gold);
        mail->set_socket1(mailInfo._socket1);
        mail->set_socket1type(mailInfo._socket1Type);
        mail->set_socket2(mailInfo._socket2);
        mail->set_socket2type(mailInfo._socket2Type);
        titleStr = GameUtils::Utils::WcharToChar(mailInfo._title, titleCStr);
        mail->set_title(titleStr);
        messageStr = GameUtils::Utils::WcharToChar(mailInfo._message, messageCStr);
        mail->set_message(messageStr);
    }

    for (auto& entry : mailSystem.GetMailEquip())
    {
        int32 mailCode = entry.first.first;
        int32 socket = entry.first.second;
        auto& item = entry.second;
        protocol::MailEquipItem* mailEquip = sendPktMail.add_equipitems();
        protocol::ItemEquip* equip = new protocol::ItemEquip();
        equip->set_item_code(item._itemCode);
        equip->set_equippos(item._equipPos);
        equip->set_attack(item._attack);
        equip->set_speed(item._speed);
        equip->set_item_type(item._equipType);
        equip->set_invenpos(item._invenPos);
        mailEquip->set_allocated_item(equip);
        mailEquip->set_socket(socket);
        mailEquip->set_mailcode(mailCode);
    }

    for (auto& entry : mailSystem.GetMailEtc())
    {
        int32 mailCode = entry.first.first;
        int32 socket = entry.first.second;
        auto& item = entry.second;
        protocol::MailEtcItem* mailEtc = sendPktMail.add_etcitems();
        protocol::ItemEtc* etc = new protocol::ItemEtc();
        etc->set_item_code(item._itemCode);
        etc->set_item_type(item._type);
        etc->set_item_count(item._count);
        etc->set_invenpos(item._invenPos);
        mailEtc->set_allocated_item(etc);
        mailEtc->set_socket(socket);
        mailEtc->set_mailcode(mailCode);
    }

    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPktMail, protocol::MessageCode::C_LOADMAIL);
    AsyncWrite(sendBuffer);
}

void GameSession::D_LoadHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::DLoad pkt;
    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        protocol::Unit unit = pkt.unit();
        protocol::Position position = unit.position();

        // 더미정보 셋팅
        _playerCode = -1;
        CreatePlayerInfo(unit.code(), unit.weaponcode(), unit.lv());
        GetPlayer()->SetDummy(true);
        GetPlayer()->SetName(unit.name());
        GetPlayer()->SetPosition(position.x(), position.y());
        GetPlayer()->SetRotate(position.yaw());
        GetPlayer()->SetExp(0);
    }
}


void GameSession::HandlePacket(BYTE* buffer, PacketHeader* header)
{
    uint16 id = header->id;

    switch (id)
    {
    case protocol::MessageCode::LOGIN:
        {
            LoginHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_UPDATEACCOUNT:
        {
            UpdateAccountHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_BUYCHARATER:
        {
            BuyCharaterHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_BUYWEAPON:
        {
            BuyWeaponHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_LOAD:
        {
            LoadHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));

            if (GRoomManger->getRoom(0) != nullptr)
            {
                GRoomManger->getRoom(0)->EnterSession(std::static_pointer_cast<GameSession>(shared_from_this()));
            }
        }
        break;
    case protocol::MessageCode::S_MOVE:
        {
            MoveHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::S_CHAT:
        {
            ChatHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_PLAYERAIM:
        {
            PlayerAimHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_PLAYERJUMP:
        {
            PlayerJumpHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_SELLITEMS:
        {
            SellItemsHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_UPDATEITEMS:
        {
            UpdateItemsHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::D_LOAD:
        {
            D_LoadHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
            if (GRoomManger->getRoom(0) != nullptr)
            {
                GRoomManger->getRoom(0)->EnterSession(std::static_pointer_cast<GameSession>(shared_from_this()));
            }
        }
        break;
    case protocol::MessageCode::S_CLOSEPLAYER:
        {
            ClosePlayerHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_FRIEND:
        {
            FriendUpdateHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_UPDATEMAIL:
        {
            UpdateMailHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_ALLUPDATEMAIL:
        {
            AllUpdateMailHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_SENDMAIL:
        {
            SendMailHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::C_ATTACK:
        {
            AttackHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
    break;
    case protocol::MessageCode::C_ATTACKS:
        {
            UserAttackHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    }
}

void GameSession::LoginHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::Login readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        nlohmann::json loginJson;
        loginJson["id"] = readPkt.id().c_str();
        loginJson["pwd"] = readPkt.pwd().c_str();
        std::string loginJsonStr = loginJson.dump();

        cpr::Response res = cpr::Post(
            cpr::Url{ "http://localhost:3000/login" },
            cpr::Body{ loginJsonStr },
            cpr::Header{ { "Content-Type", "application/json" } });

        if (res.status_code == 200)
        {
            protocol::LoginAccess loginPkt;
            nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
            int32 ret = bodyData["ret"].get<int32>();
            
            if (ret == 1)
            {
                int32 accountCode = bodyData["accountCode"].get<int32>();
                int32 curCharaterType = bodyData["curPlayerType"].get<int32>();
                int32 curWeaponType = bodyData["curWeaponType"].get<int32>();
                int32 cash = bodyData["cash"].get<int32>();
                int32 weaponOne = bodyData["weaponOne"].get<int32>();
                int32 weaponTwo = bodyData["weaponTwo"].get<int32>();
                int32 weaponThr = bodyData["weaponThr"].get<int32>();

                loginPkt.set_curcharatertype(curCharaterType);
                loginPkt.set_curweapontype(curWeaponType);
                loginPkt.set_cash(cash);
                loginPkt.add_weaponlist(weaponOne);
                loginPkt.add_weaponlist(weaponTwo);
                loginPkt.add_weaponlist(weaponThr);

                nlohmann::json characterJson;
                characterJson["accountCode"] = accountCode;
                std::string characterJsonStr = characterJson.dump();

                res = cpr::Post(
                    cpr::Url{ "http://localhost:3000/getPlayers" },
                    cpr::Body{ characterJsonStr },
                    cpr::Header{ { "Content-Type", "application/json" } });

                nlohmann::json characterData = GameUtils::JsonParser::GetStrParser(res.text);
                if (characterData["ret"].get<int>() > 0)
                {
                    nlohmann::json characters = GameUtils::JsonParser::Parser("characters", characterData);
                    for (auto& charaterData : characters)
                    {
                        protocol::Charater* charater = loginPkt.add_charater();
                        charater->set_code(charaterData["jobCode"].get<int32>());
                        charater->set_uuid(_sessionId);
                        charater->set_lv(charaterData["lv"].get<int32>());
                        charater->set_name(charaterData["name"].get<String>());
                    }
                }

                _accountCode = accountCode;
                _jobCode = curCharaterType;
                _weaponCode = curWeaponType;
                _logId.append(readPkt.id());
                WCHAR wId[10] = {};
                GameUtils::Utils::CharToWchar(readPkt.id().c_str(), wId);
                User user(_accountCode, wId);
                GUserAccess->AddUserList(user);
                std::cout << "Login Access ID: " << readPkt.id().c_str() << std::endl;
            }

            loginPkt.set_result(ret);
            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(loginPkt, protocol::MessageCode::LOGINACCESS);
            AsyncWrite(sendBuffer);
        }
    }
}

void GameSession::UpdateAccountHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CUpdateAccount readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        protocol::SCurrentInfo pkt;
        nlohmann::json loginJson;
        loginJson["accountCode"] = _accountCode;
        loginJson["characterType"] = readPkt.charatertype();
        loginJson["weaponType"] = readPkt.weapontype();
        std::string loginJsonStr = loginJson.dump();

        cpr::Response res = cpr::Post(
            cpr::Url{ "http://localhost:3000/updateAccount" },
            cpr::Body{ loginJsonStr },
            cpr::Header{ { "Content-Type", "application/json" } });
        
        if (res.status_code == 200)
        {
            nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
            int32 ret = bodyData["ret"].get<int32>();

            if (ret > 0)
            {
                _jobCode = bodyData["curPlayerType"].get<int32>();
                _weaponCode = bodyData["curWeaponType"].get<int32>();

                res = cpr::Post(
                    cpr::Url{ "http://localhost:3000/getPlayer" },
                    cpr::Body{ loginJsonStr },
                    cpr::Header{ { "Content-Type", "application/json" } });

                nlohmann::json characterJson = GameUtils::JsonParser::GetStrParser(res.text);
                ret = characterJson["ret"].get<int32>();
                if (ret == 1)
                {
                    nlohmann::json characterData = GameUtils::JsonParser::Parser("character", characterJson)[0];
                    protocol::Charater* charater = new protocol::Charater();
                    charater->set_code(characterData.at("jobCode").get<int32>());
                    charater->set_uuid(_sessionId);
                    charater->set_name(characterData.at("name").get<String>());
                    charater->set_lv(characterData.at("lv").get<int32>());
                    pkt.set_allocated_charater(charater);
                    pkt.set_exp(characterData.at("exp").get<int32>());
                }

                pkt.set_charatertype(_jobCode);
                pkt.set_weapontype(_weaponCode);
                pkt.set_cash(bodyData["cash"].get<int32>());
                SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_CURRENTINFO);
                AsyncWrite(sendBuffer);
            }
        }
    }
}

void GameSession::BuyCharaterHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CBuyCharater readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        protocol::SBuyResult pkt;
        pkt.set_result(-1);
        nlohmann::json buyCharacterJson;
        buyCharacterJson["accountCode"] = _accountCode;
        buyCharacterJson["useCash"] = readPkt.usecash();
        buyCharacterJson["characterType"] = readPkt.charatertype();
        buyCharacterJson["characterName"] = readPkt.name();
        std::string buyCharacterJsonStr = buyCharacterJson.dump();

        cpr::Response res = cpr::Post(
            cpr::Url{ "http://localhost:3000/buyCharacter" },
            cpr::Body{ buyCharacterJsonStr },
            cpr::Header{ { "Content-Type", "application/json" } });

        if (res.status_code == 200)
        {
            nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
            int32 ret = bodyData["ret"].get<int32>();

            if (ret == 1)
            {
                pkt.set_result(1);
                pkt.set_cash(bodyData["cash"].get<int32>());

                res = cpr::Post(
                    cpr::Url{ "http://localhost:3000/getPlayer" },
                    cpr::Body{ buyCharacterJsonStr },
                    cpr::Header{ { "Content-Type", "application/json" } });

                nlohmann::json characterJson = GameUtils::JsonParser::GetStrParser(res.text);
                ret = characterJson["ret"].get<int32>();
                if (ret == 1)
                {
                    nlohmann::json characterData = GameUtils::JsonParser::Parser("character", characterJson)[0];
                    protocol::Charater* charater = pkt.add_charater();
                    charater->set_code(characterData.at("jobCode").get<int32>());
                    charater->set_uuid(_sessionId);
                    charater->set_name(characterData.at("name").get<String>());
                    charater->set_lv(characterData.at("lv").get<int32>());
                }
            }
        }
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_BUYRESULT);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::BuyWeaponHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CBuyWeapon readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        protocol::SBuyResult pkt;
        pkt.set_result(-1);
        nlohmann::json buyWeaponJson;
        buyWeaponJson["accountCode"] = _accountCode;
        buyWeaponJson["useCash"] = readPkt.usecash();
        buyWeaponJson["weaponType"] = readPkt.weapontype();
        std::string buyWeaponJsonStr = buyWeaponJson.dump();

        cpr::Response res = cpr::Post(
            cpr::Url{ "http://localhost:3000/buyWeapon" },
            cpr::Body{ buyWeaponJsonStr },
            cpr::Header{ { "Content-Type", "application/json" } });

        if (res.status_code == 200)
        {
            nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
            int32 ret = bodyData["ret"].get<int32>();

            if (ret > 0)
            {
                pkt.set_result(1);
                pkt.set_curcharatertype(bodyData["curPlayerType"].get<int32>());
                pkt.set_curweapontype(bodyData["curWeaponType"].get<int32>());
                pkt.set_cash(bodyData["cash"].get<int32>());
                pkt.add_weaponlist(bodyData["weaponOne"].get<int32>());
                pkt.add_weaponlist(bodyData["weaponTwo"].get<int32>());
                pkt.add_weaponlist(bodyData["weaponThr"].get<int32>());
            }
            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_BUYRESULT);
            AsyncWrite(sendBuffer);
        }
    }
}

void GameSession::LoadHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CLoad readPkt;

    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        nlohmann::json characterJson;
        characterJson["accountCode"] = _accountCode;
        characterJson["characterType"] = _jobCode;
        std::string characterJsonStr = characterJson.dump();

        cpr::Response res = cpr::Post(
            cpr::Url{ "http://localhost:3000/getPlayer" },
            cpr::Body{ characterJsonStr },
            cpr::Header{ { "Content-Type", "application/json" } });

        if (res.status_code == 200)
        {
            nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
            int32 ret = bodyData["ret"].get<int32>();

            if (ret > 0)
            {
                nlohmann::json characterJson = GameUtils::JsonParser::GetStrParser(res.text);
                ret = characterJson["ret"].get<int32>();

                nlohmann::json characterData = GameUtils::JsonParser::Parser("character", characterJson)[0];
                int32 jobCode = characterData.at("jobCode").get<int32>();
                int32 mapCode = characterData.at("mapCode").get<int32>();
                int32 gold = characterData.at("gold").get<int32>();
                int32 lv = characterData.at("lv").get<int32>();
                int32 exp = characterData.at("exp").get<int32>();
                int32 playerCode = characterData.at("playerCode").get<int32>();
                String nameStr = characterData.at("name").get<String>();

                _playerCode = playerCode;
                auto& position = readPkt.position();
                CreatePlayerInfo(jobCode, _weaponCode, lv);
                GetPlayer()->SetName(nameStr);
                GetPlayer()->SetPosition(position.x(), position.y());
                GetPlayer()->SetRotate(position.yaw());
                GetPlayer()->SetExp(exp);
                GetPlayer()->SetInventory(gold);
                GetPlayer()->SetFriend();
                GetPlayer()->SetMail();
                GetPlayer()->UpdateAblity();

                // 나를 확인용 메시지 전달.
                if (GetService() != nullptr)
                {
                    protocol::SPlayerData sendPkt;
                    protocol::Unit* unit = new protocol::Unit();
                    unit->set_name(_player->GetName());
                    unit->set_code(_player->GetCode());
                    unit->set_uuid(_player->GetUUid());
                    unit->set_hp(_player->GetAbility().hp);
                    unit->set_lv(_player->GetAbility().lv);
                    unit->set_weaponcode(_player->GetWeapon());
                    sendPkt.set_allocated_player(unit);
                    sendPkt.set_exp(exp);

                    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_PLAYERDATA);
                    AsyncWrite(sendBuffer);
                }

                // 친구 전달
                {
                    protocol::SFriendSystem sendPkt;

                    for (auto myFriend : GetPlayer()->GetFriend().GetFriendList())
                    {
                        auto it = GUserAccess->GetPlayerList().find(myFriend.first);
                        if (it != GUserAccess->GetPlayerList().end())
                        {
                            char friendNamdC[20] = { 0, };
                            String friendNameCStr = GameUtils::Utils::WcharToChar(it->second.name, friendNamdC);
                            int32 friendCode = myFriend.first;
                            int32 friendAccess = myFriend.second;

                            protocol::Friend* addFriend = sendPkt.add_friend_();
                            addFriend->set_playercode(friendCode);
                            addFriend->set_playername(friendNameCStr);
                            addFriend->set_access(friendAccess);
                            addFriend->set_add(true);
                        }
                    }
                    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_FRIENDSYSTEM);
                    AsyncWrite(sendBuffer);
                }
            }
        }
    }
}

void GameSession::MoveHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::SMove pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        auto& position = pkt.position();

        GetPlayer()->SetPosition(position.x(), position.y());
        GetPlayer()->SetRotate(position.yaw());
        pkt.set_uuid(_sessionId);
        protocol::Position* playerPosition = new protocol::Position;
        playerPosition->set_x(position.x());
        playerPosition->set_y(position.y());
        playerPosition->set_z(position.z());
        playerPosition->set_yaw(position.yaw());
        pkt.set_allocated_position(playerPosition);
        pkt.set_is_monster(false);

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_MOVE);
        
        if (GRoomManger->getRoom(GetRoomId()) != nullptr)
        {
            GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
        }
    }
}

void GameSession::ChatHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::SChat pkt;
    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        pkt.set_uuid(GetPlayer()->GetUUid());
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_CHAT);
        // 해당 함수는 보류다 바로바로 가야 되므로 strand처리를 제외한다.
        if (GRoomManger->getRoom(_roomId) != nullptr)
        {
            GRoomManger->getRoom(_roomId)->BroadCast(sendBuffer);
        }
    }
}

void GameSession::ChangeRoomHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CMovePotal readPkt;

    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        int32 currentRoomId = readPkt.pre_room_id();
        int32 nextRoomId = readPkt.next_room_id();

        if (GRoomManger->getRoom(GetRoomId()) != nullptr)
        {
            GRoomManger->getRoom(GetRoomId())->OutSession(std::static_pointer_cast<GameSession>(shared_from_this()));
        }

        if (currentRoomId == 1)
        {
            OnDisconnect();
        }
        else if (GRoomManger->getRoom(nextRoomId) != nullptr)
        {
            // 초기 위치 설정
            GetPlayer()->SetPosition(-20, 0);
            GetPlayer()->SetRotate(0);
            GRoomManger->getRoom(nextRoomId)->EnterSession(std::static_pointer_cast<GameSession>(shared_from_this()));
        }
    }
}

void GameSession::PlayerAimHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CPlayerAim pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::C_PLAYERAIM);
        if (GRoomManger->getRoom(GetRoomId()) != nullptr)
        {
            GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
        }
    }
}

void GameSession::PlayerJumpHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CPlayerJump pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::C_PLAYERJUMP);
        if (GRoomManger->getRoom(GetRoomId()) != nullptr)
        {
            GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
        }
    }
}

void GameSession::SellItemsHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CSellItems pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        // 판매성공만 보낸다.
        int32 useGold = 0;
        protocol::CSellItems sendPkt;
        for (auto& item : pkt.itemequips())
        {
            if (GetPlayer()->GetInventory().UseItemEquip(item.invenpos()))
            {
                useGold += GEquipItem->GetItem(item.item_code())->GetGold();
                auto accessItem = sendPkt.add_itemequips();
                accessItem->set_invenpos(item.invenpos());
            }
        }

        for (auto& item : pkt.itemetcs())
        {
            if (GetPlayer()->GetInventory().UseItemEtc(item.item_code(), item.item_count()))
            {
                useGold += (GEtcItem->GetItem(item.item_code())->GetGold() * item.item_count());
                auto accessItem = sendPkt.add_itemetcs();
                accessItem->set_invenpos(item.invenpos());
                accessItem->set_item_code(item.item_code());
                accessItem->set_item_count(item.item_count());
            }
        }
        GetPlayer()->GetInventory().AddGold(useGold);
        sendPkt.set_gold(GetPlayer()->GetInventory().GetGold());
        sendPkt.set_result(true);

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::C_SELLITEMS);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::UpdateItemsHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CUpdateItems pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        int32 invenPos = pkt.invenpos();
        int32 equipPos = pkt.equippos();
        if (GetPlayer()->GetInventory().ItemEquipped(invenPos, equipPos))
        {
            protocol::CUpdateItems sendPkt;
            sendPkt.set_invenpos(invenPos);
            sendPkt.set_equippos(equipPos);
            GetPlayer()->UpdateAblity();

            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::C_UPDATEITEMS);
            AsyncWrite(sendBuffer);
        }
    }
}

void GameSession::ClosePlayerHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    if (GRoomManger->getRoom(GetRoomId()) != nullptr)
    {
        GRoomManger->getRoom(GetRoomId())->OutSession(std::static_pointer_cast<GameSession>(shared_from_this()));
    }
}

void GameSession::FriendUpdateHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CFriend pkt;
    protocol::SFriendSystem sendPkt;

    int32 result = -1;
    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        if (pkt.type() == 0)
        {
            // 친구 추가
            if (!pkt.friend_().playername().empty())
            {
                WCHAR name[10] = {0,};
                WString nameStr = GameUtils::Utils::CharToWchar(pkt.friend_().playername().c_str(), name);
                auto it = GUserAccess->GetPlayerNameList().find(nameStr);
                if (it != GUserAccess->GetPlayerNameList().end())
                {
                    int32 friendCode = it->second;
                    result = 1;
                    protocol::Friend* newFriend = sendPkt.add_friend_();
                    newFriend->set_add(true);
                    newFriend->set_playercode(friendCode);
                    newFriend->set_playername(pkt.friend_().playername());
                    newFriend->set_access(false);
                    bool friendAccess = GUserAccess->IsAccessPlayer(friendCode);
                    newFriend->set_access(friendAccess);
                    GetPlayer()->GetFriend().AddFriend(friendCode, friendAccess);
                }
            }
        }
    }
    sendPkt.set_result(result);

    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::S_FRIENDSYSTEM);
    AsyncWrite(sendBuffer);
}

void GameSession::UpdateMailHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CUpdateMail pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        int32 type = pkt.type();
        int32 mailCode = pkt.mail().code();
        auto& MailBox = GetPlayer()->GetMail();

        if (type == 2)
        {
            // 메일 아이템 받기
            if (GetPlayer()->GetMail().ReciveItemMail(mailCode, _playerCode))
            {
                auto it = MailBox.GetMail().find(mailCode);
                if (it != MailBox.GetMail().end())
                {
                    auto& mail = it->second;
                    pkt.mutable_mail()->set_socket1(mail._socket1);
                    pkt.mutable_mail()->set_socket2(mail._socket2);
                    pkt.mutable_mail()->set_gold(mail._gold);
                }
                UpdateItems();
            }
        }
        else if (type == 3)
        {
            if (GetPlayer()->GetMail().RemoveMail(mailCode, _playerCode))
            {
                // 메일 삭제
                pkt.mutable_mail()->set_code(mailCode);
            }
            else
            {
                // 삭제 실패
                pkt.mutable_mail()->set_code(-1);
            }
        }
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::C_UPDATEMAIL);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::AllUpdateMailHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CAllUpdateMail pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        int32 type = pkt.type();
        auto& MailBox = GetPlayer()->GetMail();
        if (type == 1)
        {
            // 메일 갱신
            MailBox.ReLoadMail(_playerCode);
        }
        else if (type == 2)
        {
            // 메일 아이템 전부 받기
            MailBox.ReciveItemMailAll(_playerCode);
            UpdateItems();
        }
        else if (type == 3)
        {
            // 메일 전부 삭제
            MailBox.RemoveMailAll(_playerCode);
        }
        LoadMails();
    }
}

void GameSession::SendMailHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CSendMail pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        auto& mail = pkt.mails();

        Mail newMail(mail.code(), mail.read(), mail.gold(), mail.socket1(), mail.socket1type(), mail.socket2(), mail.socket2type(), nullptr, nullptr);
        GameUtils::Utils::CharToWchar(mail.title().c_str(), newMail._title);
        GameUtils::Utils::CharToWchar(mail.message().c_str(), newMail._message);

        protocol::SSendMail sendPtk;
        auto& mailBox = GetPlayer()->GetMail();
        auto& inventory = GetPlayer()->GetInventory();
        // 유저 체크
        WString userName = newMail._title;
        if (GUserAccess->IsCheckPlayer(newMail._title) && inventory.CheckGold(mail.gold()))
        {
            int32 targetPlayerCode = GUserAccess->GetPlayerNameList().find(newMail._title)->second;
            mailBox.SendMail(newMail, targetPlayerCode);
            for (auto& MailItem : pkt.equipitems())
            {
                auto& Item = MailItem.item();
                EquipItem EquipItem(nullptr, Item.item_code(), Item.item_type(), Item.attack(), Item.speed(), -1, Item.invenpos());
                if (Item.invenpos() == 1 && mail.socket1() == 1)
                {
                    mailBox.SendEquipItemMail(newMail, targetPlayerCode, EquipItem);
                    inventory.UseItemEquip(Item.invenpos());

                    auto removeItem = sendPtk.add_itemequips();
                    removeItem->set_invenpos(Item.invenpos());
                }
                else if (Item.invenpos() == 2 && mail.socket2() == 1)
                {
                    mailBox.SendEquipItemMail(newMail, targetPlayerCode, EquipItem);
                    inventory.UseItemEquip(Item.invenpos());

                    auto removeItem = sendPtk.add_itemequips();
                    removeItem->set_invenpos(Item.invenpos());
                }
            }
            inventory.UseGold(mail.gold());
            sendPtk.set_gold(inventory.GetGold());
            sendPtk.set_result(1);
        }
        else
        {
            // 유저 존재 하지 않음
            sendPtk.set_result(-1);
        }
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPtk, protocol::MessageCode::S_SENDMAIL);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::AttackHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CAttack pkt;
    if (GRoomManger->getRoom(GetRoomId()) != nullptr)
    {
        if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
        {
            int32 skillCode = pkt.skillcode();
            int32 attackNumber = pkt.attacknumber();
            auto& position = pkt.position();

            if (skillCode == 0)
                GetPlayer()->SetObjecteState(ObjectStateType::ATTACK);
            else
                GetPlayer()->SetObjecteState(ObjectStateType::SKILL1);
            pkt.set_uuid(GetPlayer()->GetUUid());

            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::C_ATTACK);
            GRoomManger->getRoom(GetRoomId())->AttackObject(std::static_pointer_cast<GameSession>(shared_from_this()), attackNumber, skillCode, position.x(), position.y(), position.yaw());
            GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
        }
    }
}

void GameSession::UserAttackHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::UserAttack pkt;
    if (GRoomManger->getRoom(GetRoomId()) != nullptr && GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        int32 attackNumber = pkt.attacknumber();
        int32 skillCode = pkt.skillcode();

        int32 targetSize = pkt.targetcodes().size();
        Vector<int32> targets(targetSize);
        for (int i = 0; i < targetSize; ++i)
            targets[i] = pkt.targetcodes().Get(i);
        auto& position = pkt.position();

        GRoomManger->getRoom(GetRoomId())->AttackObjectMove(std::static_pointer_cast<GameSession>(shared_from_this()), attackNumber, skillCode, targets, position.x(), position.y(), position.yaw());
    }
}
