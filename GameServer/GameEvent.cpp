#include "GameEvent.h"

#include "MailDB.h"
#include "MailSystem.h"

void GameEvent::CreateCharacterMail(int32 playerCode)
{
    MailDB mailDB;
    Mail mail = Mail::FirstMail();
    EquipItem item1{-1, 11, 1, 90, 0, 0, 1, 0};
    EquipItem item2{-1, 12, 2, 0, 90, 0, 2, 0};

    mailDB.InsertMail(mail, playerCode);
    mailDB.InsertEquipItemMail(item1, mail._code, playerCode);
    mailDB.InsertEquipItemMail(item2, mail._code, playerCode);
}

void GameEvent::DropEquipMail(int32 playerCode, EquipItem& item)
{
    MailDB mailDB;
    Mail mail{-1, 0, 0, 1, 1, 0, 0, nullptr, nullptr};
    wcscpy_s(mail._title, 10, L"System");
    wcscpy_s(mail._message, 50, L"Inventory poll !!!");

    mailDB.InsertMail(mail, playerCode);
    item._position = 1;
    mailDB.InsertEquipItemMail(item, mail._code, playerCode);
}

void GameEvent::DropEtcMail(int32 playerCode, EtcItem& item)
{
    MailDB mailDB;
    Mail mail{-1, 0, 0, 1, 2, 0, 0, nullptr, nullptr};
    wcscpy_s(mail._title, 10, L"System");
    wcscpy_s(mail._message, 50, L"Inventory poll !!!");

    mailDB.InsertMail(mail, playerCode);
    item._position = 1;
    mailDB.InsertEtcItemMail(item, mail._code, playerCode);
}
