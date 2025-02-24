﻿#include "GameEvent.h"

#include "MailDB.h"
#include "MailSystem.h"

void GameEvent::CreateCharacterMail(int32 playerCode)
{
    MailDB mailDB;
    Mail mail = Mail::FirstMail();
    UUID uuid1, uuid2;
    while (!GameUtils::Utils::GenUUID(&uuid1)) {};
    while (!GameUtils::Utils::GenUUID(&uuid2)) {};
    EquipItem item1{uuid1, 11, 1, 90, 0, 0, 1};
    EquipItem item2{uuid2, 12, 2, 0, 90, 0, 2};

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
    item._invenPos = 1;
    mailDB.InsertEquipItemMail(item, mail._code, playerCode);
}

void GameEvent::DropEtcMail(int32 playerCode, EtcItem& item)
{
    MailDB mailDB;
    Mail mail{-1, 0, 0, 1, 2, 0, 0, nullptr, nullptr};
    wcscpy_s(mail._title, 10, L"System");
    wcscpy_s(mail._message, 50, L"Inventory poll !!!");

    mailDB.InsertMail(mail, playerCode);
    item._invenPos = 1;
    mailDB.InsertEtcItemMail(item, mail._code, playerCode);
}
