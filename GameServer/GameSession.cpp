#include "pch.h"
#include "GameSession.h"
#include "GameGlobal.h"
#include "GameItem.h"
#include "GameService.pb.h"
#include "PacketHeader.h"
#include "GamePacketHandler.h"
#include "GameRoomManager.h"
#include "GameObjectInfo.h"
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

void GameSession::AddExp(int32 exp)
{
    // IRoom에서 호출된다 lock 문제 x
    if (GetPlayer()->AddExp(exp))
    {
        SessionDB sdb;
        sdb.UpdateExp(_playerCode, GetPlayer()->GetExp(), GetPlayer()->GetLv());

        protocol::SExpLv pkt;

        pkt.set_uuid(GetPlayer()->GetUUid());
        pkt.set_exp(GetPlayer()->GetExp());
        pkt.set_lv(GetPlayer()->GetLv());

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
        EquipItem& item = inventory.AddItemEquip(dropItem.second);

        if (item._position >= 0)
        {
            protocol::ItemEquip* itemEquip = pkt.add_itemequips();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEquip->set_item_code(item._itemCode);
            itemEquip->set_item_type(item._equipType);
            itemEquip->set_unipeid(item._uniqueId);
            itemEquip->set_attack(item._attack);
            itemEquip->set_speed(item._speed);
            itemEquip->set_is_equip(item._isEquip);
            itemEquip->set_position(item._position);
        }
    }
    for (auto& dropItem : monsterDrop.GetDropEtcList())
    {
        EtcItem& item = inventory.AddItemEtc(dropItem.second);

        if (item._position >= 0)
        {
            protocol::ItemEtc* itemEtc = pkt.add_itemetcs();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEtc->set_item_code(item._itemCode);
            itemEtc->set_item_count(item._count);
            itemEtc->set_item_type(item._type);
            itemEtc->set_position(item._position);
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
        if (item._position >= 0)
        {
            protocol::ItemEquip* itemEquip = pkt.add_itemequips();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEquip->set_item_code(item._itemCode);
            itemEquip->set_item_type(item._equipType);
            itemEquip->set_unipeid(item._uniqueId);
            itemEquip->set_attack(item._attack);
            itemEquip->set_speed(item._speed);
            itemEquip->set_is_equip(item._isEquip);
            itemEquip->set_position(item._position);
        }
    }
    for (auto& item : inventory.GetUpdateEtcList())
    {
        if (item._position >= 0)
        {
            protocol::ItemEtc* itemEtc = pkt.add_itemetcs();
            // 인벤토리 빈공간 있는경우, 없으면 메일행
            itemEtc->set_item_code(item._itemCode);
            itemEtc->set_item_count(item._count);
            itemEtc->set_item_type(item._type);
            itemEtc->set_position(item._position);
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
        equip->set_unipeid(item._uniqueId);
        equip->set_is_equip(item._isEquip);
        equip->set_attack(item._attack);
        equip->set_speed(item._speed);
        equip->set_item_type(item._equipType);
        equip->set_position(item._position);
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
        etc->set_position(item._position);
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
    case protocol::MessageCode::C_ATTACK:
        {
            AttackHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
        }
        break;
    case protocol::MessageCode::S_UNITDEMAGE:
        {
            DamegaHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
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
    // case protocol::MessageCode::C_SENDMAIL:
    //     {
    //         SendMailHandler(buffer, header, static_cast<int32>(sizeof(PacketHeader)));
    //     }
    //     break;
    }
}

void GameSession::LoginHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::Login readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        protocol::LoginAccess logPkt;
        WCHAR wId[10] = {};
        WCHAR wPwd[10] = {};
        GameUtils::Utils::CharToWchar(readPkt.id().c_str(), wId);
        GameUtils::Utils::CharToWchar(readPkt.pwd().c_str(), wPwd);

        SessionDB sdb;
        int32 curCharaterCode = 0;
        int32 curWeaponCode = 0;
        int32 cash = 0;
        int32 weaponOne = 0;
        int32 weaponTwo = 0;
        int32 weaponThr = 0;
        bool isAccount = sdb.LoginDB(wId, _accountCode, curCharaterCode, curWeaponCode, cash);
        if (isAccount)
        {
            bool loginCheck = sdb.LoginCheck(wPwd);
            if (loginCheck)
            {
                if (GUserAccess->AccessUser(_accountCode, shared_from_this()))
                {
                    sdb.GetAccount(_accountCode, cash, curCharaterCode, curWeaponCode, weaponOne, weaponTwo, weaponThr);
                    logPkt.set_curcharatertype(curCharaterCode);
                    logPkt.set_curweapontype(curWeaponCode);
                    logPkt.set_cash(cash);
                    logPkt.add_weaponlist(weaponOne);
                    logPkt.add_weaponlist(weaponTwo);
                    logPkt.add_weaponlist(weaponThr);
                    _jobCode = curCharaterCode;
                    _weaponCode = curWeaponCode;
                    // 로그인 성공
                    if (sdb.PlayerDB(_accountCode))
                    {
                        int32 playerCode = 0;
                        int32 jobCode = 0;
                        int32 mapCode = 0;
                        int32 gold = 0;
                        int32 lv = 0;
                        int32 exp = 0;
                        WCHAR name[10] = {0,};
                        char nameC[20] = {0,};
                        String nameStr;
                        nameStr.reserve(20);

                        while (sdb.GetPlayerDBInfo(playerCode, name, jobCode, mapCode, gold, lv, exp) == true)
                        {
                            protocol::Charater* charater = logPkt.add_charater();
                            charater->set_code(jobCode);
                            charater->set_uuid(_sessionId);
                            charater->set_lv(lv);
                            nameStr = GameUtils::Utils::WcharToChar(name, nameC);
                            charater->set_name(nameStr);
                            memset(name, 0, 10);
                        }
                    }
                    logPkt.set_result(1);
                    _logId.append(readPkt.id());
                    std::cout << "Login Access ID: " << readPkt.id().c_str() << std::endl;
                }
                else
                {
                    logPkt.set_result(3);
                    std::cout << "Already connected to this Account : " << _accountCode << std::endl;
                }
            }
            else
            {
                logPkt.set_result(0);
            }
        }
        else
        {
            // 계정 생성
            bool result = sdb.CreateAccount(wId, wPwd, 10000);
            if (result)
            {
                std::cout << "Create Account ID: " << readPkt.id().c_str() << std::endl;
                logPkt.set_result(2);
                // 여기에 유저 추가 넣어야됨
                User user(_accountCode, wId);
                GUserAccess->AddUserList(user);
            }
            else
            {
                std::cout << "Create Failed Account ID: " << readPkt.id().c_str() << std::endl;
                logPkt.set_result(4);
            }
        }
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(logPkt, protocol::MessageCode::LOGINACCESS);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::UpdateAccountHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CUpdateAccount readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        SessionDB sdb;
        protocol::SCurrentInfo pkt;
        int32 charaterType = 0;
        int32 weaponType = 0;
        int32 cash = 0;
        int32 weaponOne = 0;
        int32 weaponTwo = 0;
        int32 weaponThr = 0;
        sdb.GetAccount(_accountCode, cash, charaterType, weaponType, weaponOne, weaponTwo, weaponThr);

        if (charaterType != readPkt.charatertype())
        {
            if (sdb.PlayerDB(_accountCode, readPkt.charatertype()))
            {
                int32 playerCode = 0;
                int32 jobCode = 0;
                int32 mapCode = 0;
                int32 gold = 0;
                WCHAR name[10] = {0,};
                int32 lv = 0;
                int32 exp = 0;
                char nameC[20] = {0,};
                String nameStr;
                nameStr.reserve(20);

                while (sdb.GetPlayerDBInfo(playerCode, name, jobCode, mapCode, gold, lv, exp))
                {
                    if (jobCode == readPkt.charatertype())
                    {
                        charaterType = jobCode;

                        protocol::Charater* charater = new protocol::Charater();
                        charater->set_code(jobCode);
                        charater->set_uuid(_sessionId);
                        nameStr = GameUtils::Utils::WcharToChar(name, nameC);
                        charater->set_name(nameStr);
                        charater->set_lv(lv);
                        pkt.set_allocated_charater(charater);
                        pkt.set_exp(exp);
                    }
                }
            }
        }
        if (weaponType != readPkt.weapontype())
        {
            if (readPkt.weapontype() == 1 && weaponOne == 1)
            {
                weaponType = 1;
            }
            else if (readPkt.weapontype() == 2 && weaponTwo == 1)
            {
                weaponType = 2;
            }
            else if (readPkt.weapontype() == 3 && weaponThr == 1)
            {
                weaponType = 3;
            }
        }
        if (cash >= readPkt.usecash())
        {
            cash -= readPkt.usecash();
        }

        pkt.set_charatertype(charaterType);
        pkt.set_weapontype(weaponType);
        pkt.set_cash(cash);

        _jobCode = charaterType;
        _weaponCode = weaponType;

        sdb.UpdateAccount(_accountCode, charaterType, weaponType, cash, weaponOne, weaponTwo, weaponThr);

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_CURRENTINFO);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::BuyCharaterHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CBuyCharater readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        SessionDB sdb;
        protocol::SBuyResult pkt;
        int32 charaterType = 0;
        int32 weaponType = 0;
        int32 cash = 0;
        int32 weaponOne = 0;
        int32 weaponTwo = 0;
        int32 weaponThr = 0;
        sdb.GetAccount(_accountCode, cash, charaterType, weaponType, weaponOne, weaponTwo, weaponThr);

        if (cash >= readPkt.usecash())
        {
            int32 buyCharaterType = readPkt.charatertype();
            WCHAR wId[10] = {};
            WCHAR* newName = GameUtils::Utils::CharToWchar(readPkt.name().c_str(), wId);
            pkt.set_result(1);

            // 해당 타입 캐릭터 있는지 체크!!
            if (sdb.PlayerDB(_accountCode, buyCharaterType))
            {
                int32 playerCode = 0;
                int32 jobCode = 0;
                int32 mapCode = 0;
                int32 gold = 0;
                int32 lv = 0;
                int32 exp = 0;
                WCHAR name[10] = {0,};

                bool flag = true;
                while (sdb.GetPlayerDBInfo(playerCode, name, jobCode, mapCode, gold, lv, exp) == true)
                {
                    flag = false;
                }

                if (flag)
                {
                    if (sdb.InsertCharater(_accountCode, buyCharaterType, newName))
                    {
                        cash -= readPkt.usecash();
                        pkt.set_result(1);
                        sdb.UpdateAccount(_accountCode, charaterType, weaponType, cash, weaponOne, weaponTwo, weaponThr);
                        Player player(playerCode, jobCode, _accountCode, newName);
                        GUserAccess->AddPlayerList(player);
                    }
                }
                else
                {
                    pkt.set_result(-1);
                }
            }

            if (pkt.result() > 0 && sdb.PlayerDB(_accountCode))
            {
                int32 playerCode = 0;
                int32 jobCode = 0;
                int32 mapCode = 0;
                int32 gold = 0;
                int32 lv = 0;
                int32 exp = 0;
                WCHAR name[10] = {0,};
                char nameC[20] = {0,};
                String nameStr;
                nameStr.reserve(20);

                while (sdb.GetPlayerDBInfo(playerCode, name, jobCode, mapCode, gold, lv, exp) == true)
                {
                    protocol::Charater* charater = pkt.add_charater();
                    charater->set_code(jobCode);
                    charater->set_uuid(_sessionId);
                    charater->set_lv(lv);
                    nameStr = GameUtils::Utils::WcharToChar(name, nameC);
                    charater->set_name(nameStr);
                    memset(name, 0, 10);
                }
            }
        }
        else
        {
            pkt.set_result(0);
        }
        pkt.set_cash(cash);

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_BUYRESULT);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::BuyWeaponHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CBuyWeapon readPkt;
    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        SessionDB sdb;
        protocol::SBuyResult pkt;
        int32 charaterType = 0;
        int32 weaponType = 0;
        int32 cash = 0;
        int32 weaponOne = 0;
        int32 weaponTwo = 0;
        int32 weaponThr = 0;
        sdb.GetAccount(_accountCode, cash, charaterType, weaponType, weaponOne, weaponTwo, weaponThr);

        if (cash >= readPkt.usecash())
        {
            int32 buyWeaponType = readPkt.weapontype();
            pkt.set_result(1);

            if (buyWeaponType == 1 && weaponOne == 0)
                weaponOne = 1;
            else if (buyWeaponType == 2 && weaponTwo == 0)
                weaponTwo = 1;
            else if (buyWeaponType == 3 && weaponThr == 0)
                weaponThr = 1;
            else
                pkt.set_result(-1);

            if (pkt.result() > 0)
            {
                cash -= readPkt.usecash();
                sdb.UpdateAccount(_accountCode, charaterType, weaponType, cash, weaponOne, weaponTwo, weaponThr);
                pkt.set_curcharatertype(charaterType);
                pkt.set_curweapontype(weaponType);
                pkt.add_weaponlist(weaponOne);
                pkt.add_weaponlist(weaponTwo);
                pkt.add_weaponlist(weaponThr);

                if (sdb.PlayerDB(_accountCode))
                {
                    int32 playerCode = 0;
                    int32 jobCode = 0;
                    int32 mapCode = 0;
                    int32 gold = 0;
                    int32 lv = 0;
                    int32 exp = 0;
                    WCHAR name[10] = {0,};
                    char nameC[20] = {0,};
                    String nameStr;
                    nameStr.reserve(20);

                    while (sdb.GetPlayerDBInfo(playerCode, name, jobCode, mapCode, gold, lv, exp) == true)
                    {
                        protocol::Charater* charater = pkt.add_charater();
                        charater->set_code(jobCode);
                        charater->set_uuid(_sessionId);
                        charater->set_lv(lv);
                        nameStr = GameUtils::Utils::WcharToChar(name, nameC);
                        charater->set_name(nameStr);
                        memset(name, 0, 10);
                    }
                }
            }
        }
        else
        {
            pkt.set_result(0);
        }
        pkt.set_cash(cash);

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_BUYRESULT);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::LoadHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CLoad readPkt;

    if (GamePacketHandler::ParsePacketHandler(readPkt, buffer, header->size - offset, offset))
    {
        SessionDB sdb;
        bool result = sdb.PlayerDB(_accountCode, _jobCode);
        auto& position = readPkt.position();
        if (result)
        {
            int32 jobCode = 0;
            int32 mapCode = 0;
            int32 gold = 0;
            int32 lv = 0;
            int32 exp = 0;
            WCHAR name[10] = {0,};
            char nameC[20] = {0,};
            String nameStr;
            nameStr.reserve(20);

            sdb.GetPlayerDBInfo(_playerCode, name, jobCode, mapCode, gold, lv, exp);
            sdb.ResetDBOrm();

            CreatePlayerInfo(jobCode, _weaponCode, lv);
            nameStr = GameUtils::Utils::WcharToChar(name, nameC);
            GetPlayer()->SetName(nameStr);
            GetPlayer()->SetPosition(position.x(), position.y());
            GetPlayer()->SetRotate(position.yaw());
            GetPlayer()->SetExp(exp);
            GetPlayer()->SetInventory(gold);
            GetPlayer()->SetFriend();
            GetPlayer()->SetMail();

            // 나를 확인용 메시지 전달.
            if (GetService() != nullptr)
            {
                protocol::SPlayerData sendPkt;
                protocol::Unit* unit = new protocol::Unit();
                unit->set_name(_player->GetName());
                unit->set_code(_player->GetCode());
                unit->set_uuid(_player->GetUUid());
                unit->set_hp(_player->GetHp());
                unit->set_lv(_player->GetLv());
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
                        char friendNamdC[20] = {0,};
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
        GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
    }
}

void GameSession::AttackHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CAttack pkt;
    if (GRoomManger->getRoom(GetRoomId()) != nullptr)
    {
        if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
        {
            int32 SkillCode = pkt.skill_code();
            int32 TargetUUid = pkt.target_uuid();

            auto& position = pkt.position();
            GetPlayer()->SetPosition(position.z(), position.x());
            GetPlayer()->SetRotate(position.yaw());
            GetPlayer()->SetTarget(TargetUUid);
            if (SkillCode == 0)
                GetPlayer()->SetObjecteState(ObjectState::ATTACK);
            else
                GetPlayer()->SetObjecteState(ObjectState::SKILL1);
            pkt.set_uuid(GetPlayer()->GetUUid());

            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::C_ATTACK);
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

void GameSession::DamegaHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::SUnitDemage pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        // 피격시 일단 룸에서 데미지 패킷을 보내는게 아니라 여기서 일단 바로 보낸다.
        // 피격 모션은 update에서 진행 
        for (auto& demage : pkt.demage())
        {
            int32 demageVal = demage.demage();
            int32 uuid = demage.uuid();
            int32 isMonster = demage.is_monster();
            bool isHeal = demage.is_heal();

            if (isHeal)
            {
                GRoomManger->getRoom(_roomId)->Heal(std::static_pointer_cast<GameSession>(shared_from_this()), demageVal, uuid);
            }
            else
            {
                if (!isMonster)
                {
                    GRoomManger->getRoom(_roomId)->Attack(std::static_pointer_cast<GameSession>(shared_from_this()), isMonster, demageVal, uuid);
                }
                else
                {
                    GRoomManger->getRoom(_roomId)->Attack(std::static_pointer_cast<GameSession>(shared_from_this()), isMonster, demageVal, uuid,
                                                          demage.position().x(), demage.position().y(), demage.position().yaw());
                }
            }
            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_UNITDEMAGE);
            GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
            // GetService()->BroadCast(sendBuffer);
        }
    }
}

void GameSession::PlayerAimHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CPlayerAim pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::C_PLAYERAIM);
        GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
    }
}

void GameSession::PlayerJumpHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    protocol::CPlayerJump pkt;

    if (GamePacketHandler::ParsePacketHandler(pkt, buffer, header->size - offset, offset))
    {
        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::C_PLAYERJUMP);
        GRoomManger->getRoom(GetRoomId())->BroadCastAnother(sendBuffer, GetPlayer()->GetUUid());
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
            if (GetPlayer()->GetInventory().UseItemEquip(item.unipeid()))
            {
                useGold += GEquipItem->GetItem(item.item_code())->GetGold();
                auto accessItem = sendPkt.add_itemequips();
                accessItem->set_unipeid(item.unipeid());
            }
        }

        for (auto& item : pkt.itemetcs())
        {
            if (GetPlayer()->GetInventory().UseItemEtc(item.item_code(), item.item_count()))
            {
                useGold += (GEtcItem->GetItem(item.item_code())->GetGold() * item.item_count());
                auto accessItem = sendPkt.add_itemetcs();
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
        protocol::CUpdateItems sendPkt;
        for (auto& item : pkt.itemequips())
        {
            EquipItem& unequipItem = GetPlayer()->GetInventory().ItemEquipped(item.unipeid(), item.is_equip());
            if (GetPlayer()->GetInventory().CheckEquipped(item.unipeid(), item.is_equip()))
            {
                auto& targetItem = GetPlayer()->GetInventory().GetEquipItemInfo().find(item.unipeid())->second;
                auto accessItem = sendPkt.add_itemequips();
                accessItem->set_unipeid(targetItem._uniqueId);
                accessItem->set_is_equip(targetItem._isEquip);
                accessItem->set_attack(targetItem._attack);
                accessItem->set_speed(targetItem._speed);
                accessItem->set_item_type(targetItem._equipType);
                accessItem->set_item_code(targetItem._itemCode);
                accessItem->set_position(targetItem._position);

                if (unequipItem._uniqueId > 0)
                {
                    auto otherItem = sendPkt.add_itemequips();
                    otherItem->set_unipeid(unequipItem._uniqueId);
                    otherItem->set_is_equip(unequipItem._isEquip);
                    otherItem->set_attack(unequipItem._attack);
                    otherItem->set_speed(unequipItem._speed);
                    otherItem->set_item_type(unequipItem._equipType);
                    otherItem->set_item_code(unequipItem._itemCode);
                    otherItem->set_position(unequipItem._position);
                }
            }
        }

        SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(sendPkt, protocol::MessageCode::C_UPDATEITEMS);
        AsyncWrite(sendBuffer);
    }
}

void GameSession::ClosePlayerHandler(BYTE* buffer, PacketHeader* header, int32 offset)
{
    if (GRoomManger->getRoom(GetRoomId()) != nullptr)
    {
        GRoomManger->getRoom(GetRoomId())->OutSession(std::static_pointer_cast<GameSession>(shared_from_this()));
    }
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
