#include "MailSystem.h"

#include "GamePlayerInfo.h"
#include "MailDB.h"

Mail::Mail(int32 code, int32 read, int32 gold, int32 socket1, int32 socket1Type, int32 socket2, int32 socket2Type, wchar_t* title, wchar_t* message)
    : _code(code), _read(read), _gold(gold), _socket1(socket1), _socket1Type(socket1Type), _socket2(socket2), _socket2Type(socket2Type)
{
    if (title != nullptr)
        wcscpy_s(_title, 10, title);
    if (message != nullptr)
        wcscpy_s(_message, 50, message);
}

Mail::Mail(const Mail& other)
    : _code(other._code), _read(other._read), _gold(other._gold), _socket1(other._socket1), _socket1Type(other._socket1Type), _socket2(other._socket2),
      _socket2Type(other._socket2Type)
{
    if (other._title != nullptr)
        wcscpy_s(_title, 10, other._title);
    if (other._message != nullptr)
        wcscpy_s(_message, 50, other._message);
}

Mail::Mail(Mail&& other) noexcept
    : _code(other._code), _read(other._read), _gold(other._gold), _socket1(other._socket1), _socket1Type(other._socket1Type), _socket2(other._socket2),
      _socket2Type(other._socket2Type)
{
    if (other._title != nullptr)
        wcscpy_s(_title, 10, other._title);
    if (other._message != nullptr)
        wcscpy_s(_message, 50, other._message);
}

Mail::~Mail()
{
}

Mail& Mail::operator=(const Mail& other)
{
    _code = other._code;
    _read = other._read;
    _gold = other._gold;
    _socket1 = other._socket1;
    _socket1Type = other._socket1Type;
    _socket2 = other._socket2;
    _socket2Type = other._socket2Type;
    if (other._title != nullptr)
        wcscpy_s(_title, 10, other._title);
    if (other._message != nullptr)
        wcscpy_s(_message, 50, other._message);

    return *this;
}

Mail& Mail::operator=(Mail&& other) noexcept
{
    _code = other._code;
    _read = other._read;
    _gold = other._gold;
    _socket1 = other._socket1;
    _socket1Type = other._socket1Type;
    _socket2 = other._socket2;
    _socket2Type = other._socket2Type;
    if (other._title != nullptr)
        wcscpy_s(_title, 10, other._title);
    if (other._message != nullptr)
        wcscpy_s(_message, 50, other._message);

    return *this;
}

Mail Mail::EmptyEtcItem()
{
    return Mail{-1, -1, 0, -1, -1, -1, -1, nullptr, nullptr};
}

Mail Mail::FirstMail()
{
    Mail First = Mail{-1, 0, 10000, 1, 1, 1, 1, nullptr, nullptr};
    wcscpy_s(First._title, 10, L"System");
    wcscpy_s(First._message, 50, L"캐릭터 생성을 축하합니다 !!!");
    return First;
}

MailSystem::MailSystem()
{
}

MailSystem::~MailSystem()
{
}

void MailSystem::LoadMail(int32 playerCode)
{
    MailDB mailDB;
    Mail emptyMail = Mail::EmptyEtcItem();

    mailDB.LoadMailDB(playerCode);
    while (mailDB.GetMail(emptyMail))
    {
        Mail mail = emptyMail;
        _mailList.emplace(emptyMail._code, mail);
    }

    int32 mailCode = -1;
    mailDB.LoadMailEquipDB(playerCode);
    EquipItem emptyEquip = EquipItem::EmptyEquipItem();
    while (mailDB.GetMailEquip(emptyEquip, mailCode))
    {
        if (mailCode > 0)
        {
            Tuple<int,int> key = {mailCode, emptyEquip._invenPos};
            EquipItem item = emptyEquip;
            _mailEquipList.emplace(key, item);
        }
        mailCode = -1;
    }

    mailDB.LoadMailEtcDB(playerCode);
    EtcItem emptyEtc = EtcItem::EmptyEtcItem();
    while (mailDB.GetMailEtc(emptyEtc, mailCode))
    {
        if (mailCode > 0)
        {
            Tuple<int,int> key = {mailCode, emptyEtc._invenPos};
            EtcItem item = emptyEtc;
            _mailEtcList.emplace(key, item);
        }
        mailCode = -1;
    }
}

bool MailSystem::ReadMail(int32 mailCode, int32 playerCode)
{
    auto it = _mailList.find(mailCode);
    if (it != _mailList.end())
    {
        Mail& mail = it->second;
        if (mail._read == 0)
        {
            MailDB mailDB;
            mail._read = 1;
            if (mailDB.UpdateMail(mail, playerCode))
            {
                return true;
            }
        }
    }
    return false;
}

int32 MailSystem::CountEquipItemMail(int32 mailCode)
{
    int32 count = 0;
    for (int i = 1; i <= _itemMaxSize; i++)
    {
        Tuple<int, int> key = {mailCode, i};
        auto it = _mailEquipList.find(key);
        if (it != _mailEquipList.end())
        {
            count++;
        }
    }

    return count;
}

int32 MailSystem::CountEtcItemMail(int32 mailCode)
{
    int32 count = 0;
    for (int i = 1; i <= _itemMaxSize; i++)
    {
        Tuple<int, int> key = {mailCode, i};
        auto it = _mailEtcList.find(key);
        if (it != _mailEtcList.end())
        {
            count++;
        }
    }
    return count;
}

bool MailSystem::ReciveItemMail(int32 mailCode, int32 playerCode)
{
    // 미리 인벤 체크한다.
    WriteLockGuard writeLock(lock);
    auto it = _mailList.find(mailCode);
    if (it != _mailList.end())
    {
        Mail& mail = it->second;
        MailDB mailDB;
        if (mail._socket1 == 1)
        {
            if (mail._socket1Type == 1)
            {
                // 장비
                ReciveItemEquipMail(mailCode, 1);
            }
            else if (mail._socket1Type == 2)
            {
                // 기타
                ReciveItemEtcMail(mailCode, 1);
            }
            mail._socket1 = 0;
        }
        if (mail._socket2 == 1)
        {
            if (mail._socket2Type == 1)
            {
                // 장비
                ReciveItemEquipMail(mailCode, 2);
            }
            else if (mail._socket2Type == 2)
            {
                // 기타
                ReciveItemEtcMail(mailCode, 2);
            }
            mail._socket2 = 0;
        }
        _playerInfo.lock()->GetInventory().AddGold(mail._gold);
        mail._gold = 0;
        if (mailDB.UpdateMail(mail, playerCode))
        {
            return true;
        }
    }
    return false;
}

bool MailSystem::ReciveItemEquipMail(int32 mailCode, int32 position)
{
    auto key = Tuple<int,int>{mailCode, position};
    auto it = _mailEquipList.find(key);
    if (it != _mailEquipList.end())
    {
        if (_playerInfo.lock()->GetInventory().AddMailItemEquip(it->second))
        {
            return true;
        }
    }
    return false;
}

bool MailSystem::ReciveItemEtcMail(int32 mailCode, int32 position)
{
    auto key = Tuple<int,int>{mailCode, position};
    auto it = _mailEtcList.find(key);
    if (it != _mailEtcList.end())
    {
        if (_playerInfo.lock()->GetInventory().AddMailItemEtc(it->second))
        {
            return true;
        }
    }
    return false;
}

void MailSystem::ReciveItemMailAll(int32 playerCode)
{
    // 아마 메일 업데이트 유무 판단없이 현재 메일리스트 전부 보내야될듯함 => 클라.
    for (auto& entry : _mailList)
    {
        ReciveItemMail(entry.first, playerCode);
    }
}

bool MailSystem::RemoveMail(int32 mailCode, int32 playerCode)
{
    WriteLockGuard writeLock(lock);
    MailDB mailDB;
    
    auto it = _mailList.find(mailCode);
    if (it != _mailList.end())
    {
        auto mail = it->second;
        if (mail._gold == 0 && mail._socket1 <= 0 && mail._socket2 <= 0)
        {
            RemoveItemEqipMail(mailCode);
            RemoveItemEtcMail(mailCode);
            mailDB.RemoveMail(mailCode, playerCode);
            _mailList.erase(mailCode);
            return true;
        }
    }
    return false;
}

void MailSystem::RemoveItemEqipMail(int32 mailCode)
{
    for (int32 position = 1; position <= _itemMaxSize; position++)
    {
        auto key = Tuple<int,int>{mailCode, position};
        auto it = _mailEquipList.find(key);
        if (it != _mailEquipList.end())
        {
            _mailEquipList.erase(it);
        }
    }
}

void MailSystem::RemoveItemEtcMail(int32 mailCode)
{
    for (int32 position = 1; position <= _itemMaxSize; position++)
    {
        auto key = Tuple<int,int>{mailCode, position};
        auto it = _mailEtcList.find(key);
        if (it != _mailEtcList.end())
        {
            _mailEtcList.erase(it);
        }
    }
}

void MailSystem::RemoveMailAll(int32 playerCode)
{
    // 아마 메일 업데이트 유무 판단없이 현재 메일리스트 전부 보내야될듯함 => 클라.
    MailDB mailDB;
    for (auto it = _mailList.begin(); it != _mailList.end();)
    {
        int32 mailCode = it->first;
        ++it;
        RemoveMail(mailCode, playerCode);
    }
}

void MailSystem::SendMail(Mail& mail, int32 playerCode)
{
    // 나아닌 다른 플레이어에게 전달
    MailDB mailDB;
    mailDB.InsertMail(mail, playerCode);
}

void MailSystem::SendEquipItemMail(Mail& mail, int32 playerCode, EquipItem& item)
{
    // 나아닌 다른 플레이어에게 전달
    MailDB mailDB;
    int32 mailCode = mail._code;
    mailDB.InsertEquipItemMail(item, mailCode, playerCode);
}

void MailSystem::SendEtcItemMail(Mail& mail, int32 playerCode, EtcItem& item)
{
    // 나아닌 다른 플레이어에게 전달
    MailDB mailDB;
    int32 mailCode = mail._code;
    mailDB.InsertEtcItemMail(item, mailCode, playerCode);
}

void MailSystem::ReLoadMail(int32 playerCode)
{
    // 아마 메일 로드는 메일창 열때 주는걸로 한다.
    // 메일 새로고침때에도 이 함수를 이용한다.
    
    WriteLockGuard writeLock(lock);
    _mailList.clear();
    _mailEquipList.clear();
    _mailEtcList.clear();
    LoadMail(playerCode);
}

void MailSystem::SetPlayer(GameObjectInfoRef player)
{
    _playerInfo = std::static_pointer_cast<GamePlayerInfo>(player);
}
