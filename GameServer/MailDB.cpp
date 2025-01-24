#include "MailDB.h"

MailDB::MailDB() : _dbOdbc(10), _mail(Mail::EmptyEtcItem()), _equipItem(EquipItem::EmptyEquipItem()), _etcItem(EtcItem::EmptyEtcItem()), _mailCode(-1)
{
    conn = GDBPool->Pop();
    _dbOdbc.SetDBConn(conn);
}

MailDB::~MailDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void MailDB::LoadMailDB(int32 playerCode)
{
    conn->Prepare(selectMailQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(selectMailQuery);

    _dbOdbc.BindColInt(sizeof(_mail._code), &_mail._code);
    _dbOdbc.BindColWchar(sizeof(_mail._title), &_mail._title);
    _dbOdbc.BindColWchar(sizeof(_mail._message), &_mail._message);
    _dbOdbc.BindColInt(sizeof(_mail._gold), &_mail._gold);
    _dbOdbc.BindColInt(sizeof(_mail._read), &_mail._read);
    _dbOdbc.BindColInt(sizeof(_mail._socket1), &_mail._socket1);
    _dbOdbc.BindColInt(sizeof(_mail._socket1Type), &_mail._socket1Type);
    _dbOdbc.BindColInt(sizeof(_mail._socket2), &_mail._socket2);
    _dbOdbc.BindColInt(sizeof(_mail._socket2Type), &_mail._socket2Type);
}

bool MailDB::GetMail(Mail& mail)
{
    if (conn->Fetch())
    {
        mail = _mail;
        return true;
    }
    
    conn->CloseCursor();
    _dbOdbc.ReSetIdx();
    return false;
}

void MailDB::LoadMailEquipDB(int32 playerCode)
{
    conn->Prepare(selectMailEquipQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(selectMailEquipQuery);
    
    _dbOdbc.BindColInt(sizeof(_mailCode), &_mailCode);
    _dbOdbc.BindColChar(sizeof(_equipItem._uuid), &_equipItem._uuid);
    _dbOdbc.BindColInt(sizeof(_equipItem._itemCode), &_equipItem._itemCode);
    _dbOdbc.BindColInt(sizeof(_equipItem._equipType), &_equipItem._equipType);
    _dbOdbc.BindColInt(sizeof(_equipItem._attack), &_equipItem._attack);
    _dbOdbc.BindColInt(sizeof(_equipItem._speed), &_equipItem._speed);
    _dbOdbc.BindColInt(sizeof(_equipItem._invenPos), &_equipItem._invenPos);
}

bool MailDB::GetMailEquip(EquipItem& equipItem, int32& mailCode)
{
    if (conn->Fetch())
    {
        equipItem = _equipItem;
        mailCode = _mailCode;
        return true;
    }
    
    conn->CloseCursor();
    _dbOdbc.ReSetIdx();
    return false;
}

void MailDB::LoadMailEtcDB(int32 playerCode)
{
    conn->Prepare(selectMailEtcQuery);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(selectMailEtcQuery);

    _dbOdbc.BindColInt(sizeof(_mailCode), &_mailCode);
    _dbOdbc.BindColInt(sizeof(_etcItem._itemCode), &_etcItem._itemCode);
    _dbOdbc.BindColInt(sizeof(_etcItem._type), &_etcItem._type);
    _dbOdbc.BindColInt(sizeof(_etcItem._count), &_etcItem._count);
    _dbOdbc.BindColInt(sizeof(_etcItem._invenPos), &_etcItem._invenPos);
}

bool MailDB::GetMailEtc(EtcItem& etcItem, int32& mailCode)
{
    if (conn->Fetch())
    {
        etcItem = _etcItem;
        mailCode = _mailCode;
        return true;
    }
    
    conn->CloseCursor();
    _dbOdbc.ReSetIdx();
    return false;
}

bool MailDB::UpdateMail(Mail& mail, int32 playerCode)
{
    conn->Prepare(updateMailQuery);
    _dbOdbc.BindParamInt(&mail._gold);
    _dbOdbc.BindParamInt(&mail._read);
    _dbOdbc.BindParamInt(&mail._socket1);
    _dbOdbc.BindParamInt(&mail._socket2);
    _dbOdbc.BindParamInt(&mail._code);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(updateMailQuery);
    _dbOdbc.ReSetIdx();
    
    return true;
}

void MailDB::RemoveItemEquip(int32 mailCode, int32 playerCode)
{
    conn->Prepare(updateRemoveEquipMailQuery);
    _dbOdbc.BindParamInt(&mailCode);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(updateRemoveEquipMailQuery);
    _dbOdbc.ReSetIdx();
}

void MailDB::RemoveItemEtc(int32 mailCode, int32 playerCode)
{
    conn->Prepare(updateRemoveEtcMailQuery);
    _dbOdbc.BindParamInt(&mailCode);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(updateRemoveEtcMailQuery);
    _dbOdbc.ReSetIdx();
}

void MailDB::RemoveMail(int32 mailCode, int32 playerCode)
{
    conn->Prepare(deleteMailQuery);
    _dbOdbc.BindParamInt(&mailCode);
    _dbOdbc.BindParamInt(&playerCode);
    conn->Exec(deleteMailQuery);
    _dbOdbc.ReSetIdx();
}

void MailDB::InsertRemoveMail(Mail& mail, int32 playerCode)
{
    conn->Prepare(insertRemoveMailQuery);
    _dbOdbc.BindParamInt(&mail._code);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamWchar(sizeof(mail._title), (SQLPOINTER)mail._title);
    _dbOdbc.BindParamWchar(sizeof(mail._message), (SQLPOINTER)mail._message);
    _dbOdbc.BindParamInt(&mail._gold);
    _dbOdbc.BindParamInt(&mail._read);
    _dbOdbc.BindParamInt(&mail._socket1);
    _dbOdbc.BindParamInt(&mail._socket1Type);
    _dbOdbc.BindParamInt(&mail._socket2);
    _dbOdbc.BindParamInt(&mail._socket2Type);
    conn->Exec(insertRemoveMailQuery);
    _dbOdbc.ReSetIdx();
}

void MailDB::InsertMail(Mail& mail, int32 playerCode)
{
    // insert 문
    conn->Prepare(insertMailQuery);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamWchar(sizeof(mail._title), (SQLPOINTER)mail._title);
    _dbOdbc.BindParamWchar(sizeof(mail._message), (SQLPOINTER)mail._message);
    _dbOdbc.BindParamInt(&mail._gold);
    _dbOdbc.BindParamInt(&mail._read);
    _dbOdbc.BindParamInt(&mail._socket1);
    _dbOdbc.BindParamInt(&mail._socket1Type);
    _dbOdbc.BindParamInt(&mail._socket2);
    _dbOdbc.BindParamInt(&mail._socket2Type);
    conn->Exec(insertMailQuery);

    // output으로 메일 code 받아온다.
    _dbOdbc.BindColInt(sizeof(mail._code), &mail._code);
    conn->Fetch();

    // fetch 진행후 커서 제거
    conn->CloseCursor();
    _dbOdbc.ReSetIdx();
}

void MailDB::InsertEquipItemMail(EquipItem& item, int32 mailCode, int32 playerCode)
{
    conn->Prepare(insertMailEquipQuery);
    _dbOdbc.BindParamInt(&mailCode);
    _dbOdbc.BindParamChar(36, &item._uuid);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    _dbOdbc.BindParamInt(&item._equipType);
    _dbOdbc.BindParamInt(&item._attack);
    _dbOdbc.BindParamInt(&item._speed);
    _dbOdbc.BindParamInt(&item._invenPos);
    conn->Exec(insertMailEquipQuery);
    _dbOdbc.ReSetIdx();
}

void MailDB::InsertEtcItemMail(EtcItem& item, int32 mailCode, int32 playerCode)
{
    conn->Prepare(insertMailEtcQuery);
    _dbOdbc.BindParamInt(&mailCode);
    _dbOdbc.BindParamInt(&playerCode);
    _dbOdbc.BindParamInt(&item._itemCode);
    _dbOdbc.BindParamInt(&item._type);
    _dbOdbc.BindParamInt(&item._count);
    _dbOdbc.BindParamInt(&item._invenPos);
    conn->Exec(insertMailEtcQuery);
    _dbOdbc.ReSetIdx();
}
