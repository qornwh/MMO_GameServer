#include "MailDB.h"

MailDB::MailDB() : _dbOrm(10), _mail(Mail::EmptyEtcItem()), _equipItem(EquipItem::EmptyEquipItem()), _etcItem(EtcItem::EmptyEtcItem()), _mailCode(-1)
{
    conn = GDBPool->Pop();
    _dbOrm.SetDBConn(conn);
}

MailDB::~MailDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

void MailDB::LoadMailDB(int32 playerCode)
{
    conn->Prepare(selectMailQuery);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(selectMailQuery);

    _dbOrm.BindColInt(sizeof(_mail._code), &_mail._code);
    _dbOrm.BindColWchar(sizeof(_mail._title), &_mail._title);
    _dbOrm.BindColWchar(sizeof(_mail._message), &_mail._message);
    _dbOrm.BindColInt(sizeof(_mail._gold), &_mail._gold);
    _dbOrm.BindColInt(sizeof(_mail._read), &_mail._read);
    _dbOrm.BindColInt(sizeof(_mail._socket1), &_mail._socket1);
    _dbOrm.BindColInt(sizeof(_mail._socket1Type), &_mail._socket1Type);
    _dbOrm.BindColInt(sizeof(_mail._socket2), &_mail._socket2);
    _dbOrm.BindColInt(sizeof(_mail._socket2Type), &_mail._socket2Type);
}

bool MailDB::GetMail(Mail& mail)
{
    if (conn->Fetch())
    {
        mail = _mail;
        return true;
    }
    
    conn->CloseCursor();
    _dbOrm.ReSetIdx();
    return false;
}

void MailDB::LoadMailEquipDB(int32 playerCode)
{
    conn->Prepare(selectMailEquipQuery);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(selectMailEquipQuery);
    
    _dbOrm.BindColInt(sizeof(_mailCode), &_mailCode);
    _dbOrm.BindColInt(sizeof(_equipItem._itemCode), &_equipItem._itemCode);
    _dbOrm.BindColInt(sizeof(_equipItem._equipType), &_equipItem._equipType);
    _dbOrm.BindColInt(sizeof(_equipItem._attack), &_equipItem._attack);
    _dbOrm.BindColInt(sizeof(_equipItem._speed), &_equipItem._speed);
    _dbOrm.BindColInt(sizeof(_equipItem._position), &_equipItem._position);
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
    _dbOrm.ReSetIdx();
    return false;
}

void MailDB::LoadMailEtcDB(int32 playerCode)
{
    conn->Prepare(selectMailEtcQuery);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(selectMailEtcQuery);

    _dbOrm.BindColInt(sizeof(_mailCode), &_mailCode);
    _dbOrm.BindColInt(sizeof(_etcItem._itemCode), &_etcItem._itemCode);
    _dbOrm.BindColInt(sizeof(_etcItem._type), &_etcItem._type);
    _dbOrm.BindColInt(sizeof(_etcItem._count), &_etcItem._count);
    _dbOrm.BindColInt(sizeof(_etcItem._position), &_etcItem._position);
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
    _dbOrm.ReSetIdx();
    return false;
}

bool MailDB::UpdateMail(Mail& mail, int32 playerCode)
{
    conn->Prepare(updateMailQuery);
    _dbOrm.BindParamInt(&mail._gold);
    _dbOrm.BindParamInt(&mail._read);
    _dbOrm.BindParamInt(&mail._socket1);
    _dbOrm.BindParamInt(&mail._socket2);
    _dbOrm.BindParamInt(&mail._code);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(updateMailQuery);
    _dbOrm.ReSetIdx();
    
    return true;
}

void MailDB::RemoveItemEquip(int32 mailCode, int32 playerCode)
{
    conn->Prepare(updateRemoveEquipMailQuery);
    _dbOrm.BindParamInt(&mailCode);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(updateRemoveEquipMailQuery);
    _dbOrm.ReSetIdx();
}

void MailDB::RemoveItemEtc(int32 mailCode, int32 playerCode)
{
    conn->Prepare(updateRemoveEtcMailQuery);
    _dbOrm.BindParamInt(&mailCode);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(updateRemoveEtcMailQuery);
    _dbOrm.ReSetIdx();
}

void MailDB::RemoveMail(int32 mailCode, int32 playerCode)
{
    conn->Prepare(deleteMailQuery);
    _dbOrm.BindParamInt(&mailCode);
    _dbOrm.BindParamInt(&playerCode);
    conn->Exec(deleteMailQuery);
    _dbOrm.ReSetIdx();
}

void MailDB::InsertRemoveMail(Mail& mail, int32 playerCode)
{
    conn->Prepare(insertRemoveMailQuery);
    _dbOrm.BindParamInt(&mail._code);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamWchar(sizeof(mail._title), (SQLPOINTER)mail._title);
    _dbOrm.BindParamWchar(sizeof(mail._message), (SQLPOINTER)mail._message);
    _dbOrm.BindParamInt(&mail._gold);
    _dbOrm.BindParamInt(&mail._read);
    _dbOrm.BindParamInt(&mail._socket1);
    _dbOrm.BindParamInt(&mail._socket1Type);
    _dbOrm.BindParamInt(&mail._socket2);
    _dbOrm.BindParamInt(&mail._socket2Type);
    conn->Exec(insertRemoveMailQuery);
    _dbOrm.ReSetIdx();
}

void MailDB::InsertMail(Mail& mail, int32 playerCode)
{
    // insert 문
    conn->Prepare(insertMailQuery);
    _dbOrm.BindParamInt(&mail._code);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamWchar(sizeof(mail._title), (SQLPOINTER)mail._title);
    _dbOrm.BindParamWchar(sizeof(mail._message), (SQLPOINTER)mail._message);
    _dbOrm.BindParamInt(&mail._gold);
    _dbOrm.BindParamInt(&mail._read);
    _dbOrm.BindParamInt(&mail._socket1);
    _dbOrm.BindParamInt(&mail._socket1Type);
    _dbOrm.BindParamInt(&mail._socket2);
    _dbOrm.BindParamInt(&mail._socket2Type);
    conn->Exec(insertMailQuery);

    // output으로 메일 code 받아온다.
    _dbOrm.BindColInt(sizeof(mail._code), &mail._code);
    conn->Fetch();

    // fetch 진행후 커서 제거
    conn->CloseCursor();
    _dbOrm.ReSetIdx();
}

void MailDB::InsertEquipItemMail(EquipItem& item, int32 mailCode, int32 playerCode)
{
    conn->Prepare(insertMailEquipQuery);
    _dbOrm.BindParamInt(&mailCode);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&item._itemCode);
    _dbOrm.BindParamInt(&item._equipType);
    _dbOrm.BindParamInt(&item._attack);
    _dbOrm.BindParamInt(&item._speed);
    _dbOrm.BindParamInt(&item._position);
    conn->Exec(insertMailEquipQuery);
    _dbOrm.ReSetIdx();
}

void MailDB::InsertEtcItemMail(EtcItem& item, int32 mailCode, int32 playerCode)
{
    conn->Prepare(insertMailEtcQuery);
    _dbOrm.BindParamInt(&mailCode);
    _dbOrm.BindParamInt(&playerCode);
    _dbOrm.BindParamInt(&item._itemCode);
    _dbOrm.BindParamInt(&item._type);
    _dbOrm.BindParamInt(&item._count);
    _dbOrm.BindParamInt(&item._position);
    conn->Exec(insertMailEtcQuery);
    _dbOrm.ReSetIdx();
}
