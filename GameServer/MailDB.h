#pragma once
#include "DBOdbc.h"
#include "MailSystem.h"

static const wchar_t* insertMailQuery =
    L"INSERT INTO Mail (playerCode, title, message, gold, readCheck, socket1, socket1Type, socket2, socket2Type) OUTPUT inserted.mailCode VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
static const wchar_t* insertMailEquipQuery =
    L"INSERT INTO MailEquipItem (mailCode, uniqueId, playerCode, itemCode, equipType, attack, speed, position) VALUES (?, ?, ?, ?, ?, ?, ?)";
static const wchar_t* insertMailEtcQuery =
    L"INSERT INTO MailEtcItem (mailCode, playerCode, itemCode, itemType, itemCount, position) VALUES (?, ?, ?, ?, ?, ?)";

static const wchar_t* selectMailQuery =
    L"SELECT TOP 10 mailCode, title, message, gold, readCheck, socket1, socket1Type, socket2, socket2Type FROM Mail WHERE playerCode = ?";
static const wchar_t* selectMailEquipQuery =
    L"SELECT TOP 20 mailCode, uniqueId, itemCode, equipType, attack, speed, position FROM MailEquipItem WHERE playerCode = ? AND isRemove = 0";
static const wchar_t* selectMailEtcQuery = L"SELECT TOP 20 mailCode, itemCode, itemType, itemCount, position FROM MailEtcItem WHERE playerCode = ? AND isRemove = 0";

static const wchar_t* updateMailQuery = L"UPDATE Mail SET gold = ?, readCheck = ?, socket1 = ?, socket2 = ? WHERE mailCode = ? AND playerCode = ?";
static const wchar_t* updateRemoveEquipMailQuery = L"UPDATE MailEquipItem SET isRemove = 1 WHERE mailCode = ? AND playerCode = ?";
static const wchar_t* updateRemoveEtcMailQuery = L"UPDATE MailEtcItem SET isRemove = 1 WHERE mailCode = ? AND playerCode = ?";

static const wchar_t* deleteMailQuery = L"DELETE FROM Mail WHERE mailCode = ? AND playerCode = ?";

static const wchar_t* insertRemoveMailQuery =
    L"INSERT INTO RemoveMail (mailCode, playerCode, title, message, gold, readCheck, socket1, socket1Type, socket2, socket2Type) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

class MailDB
{
public:
    MailDB();
    ~MailDB();

    void LoadMailDB(int32 playerCode);
    bool GetMail(Mail& mail);
    void LoadMailEquipDB(int32 playerCode);
    bool GetMailEquip(EquipItem& equipItem, int32& mailCode);
    void LoadMailEtcDB(int32 playerCode);
    bool GetMailEtc(EtcItem& etcItem, int32& mailCode);

    bool UpdateMail(Mail& mail, int32 playerCode);
    void RemoveItemEquip(int32 mailCode, int32 playerCode);
    void RemoveItemEtc(int32 mailCode, int32 playerCode);
    void RemoveMail(int32 mailCode, int32 playerCode);
    void InsertRemoveMail(Mail& mail, int32 playerCode);
    void InsertMail(Mail& mail, int32 playerCode);
    void InsertEquipItemMail(EquipItem& item, int32 mailCode, int32 playerCode);
    void InsertEtcItemMail(EtcItem& item, int32 mailCode, int32 playerCode);

private:
    DBOdbc _dbOdbc;
    DBConnRef conn;

    Mail _mail;
    EquipItem _equipItem;
    EtcItem _etcItem;
    int32 _mailCode;
};
