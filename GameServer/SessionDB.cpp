#include "pch.h"
#include "SessionDB.h"

SessionDB::SessionDB() : _dbOrm(10)
{
}

SessionDB::~SessionDB()
{
}

bool SessionDB::LoginDB(const wchar_t* id, int32& accountCode, int32& curCharaterType, int32& curWeaponType, int32& cash)
{
    const wchar_t* query = L"SELECT accountCode, id, pwd, cash, curPlayerType, curWeaponType FROM Account WHERE id = ?";
    if (conn == nullptr)
        conn = GDBPool->Pop();
    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamWchar(sizeof(id), (SQLPOINTER)id);
    result = conn->Execute();

    _dbOrm.BindColInt(sizeof(_accountCode), &_accountCode);
    _dbOrm.BindColWchar(sizeof(_id), &_id);
    _dbOrm.BindColWchar(sizeof(_pwd), &_pwd);
    _dbOrm.BindColInt(sizeof(_cash), &_cash);
    _dbOrm.BindColInt(sizeof(_curCharaterType), &_curCharaterType);
    _dbOrm.BindColInt(sizeof(_curWeaponType), &_curWeaponType);

    result = conn->Fetch();
    conn->CloseCursor();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();
    accountCode = _accountCode;
    curCharaterType = _curCharaterType;
    curWeaponType = _curWeaponType;
    cash = _cash;

    return result;
}

bool SessionDB::LoginCheck(const wchar_t* pwd)
{
    if (wcscmp(_pwd, pwd) == 0)
        return true;
    return false;
}

bool SessionDB::CreateAccount(const wchar_t* id, const wchar_t* pwd, int32 cash)
{
    const wchar_t* query = L"INSERT INTO Account (id, pwd, cash) VALUES (?, ?, ?)";
    if (conn == nullptr)
        conn = GDBPool->Pop();

    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamWchar(sizeof(id), (SQLPOINTER)id);
    _dbOrm.BindParamWchar(sizeof(pwd), (SQLPOINTER)pwd);
    _dbOrm.BindParamInt(&cash);
    result = conn->Execute();
    conn->EndTran();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();

    return result;
}

bool SessionDB::CreateCharacter(const wchar_t* name, int32 jobCode, int32 accountCode)
{
    const wchar_t* query = L"INSERT INTO Player (name, jobCode, mapCode, accountCode) VALUES (?, ?, 1, ?)";
    if (conn == nullptr)
        conn = GDBPool->Pop();

    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamWchar(sizeof(name), (SQLPOINTER)name);
    _dbOrm.BindParamInt(&jobCode);
    _dbOrm.BindParamInt(&accountCode);
    result = conn->Execute();
    conn->EndTran();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();

    return result;
}

bool SessionDB::PlayerDB(int32 accountCode)
{
    const wchar_t* query = L"SELECT playerCode, name, jobCode, mapCode, gold, lv, exp FROM Player WHERE accountCode = ?";
    if (conn == nullptr)
        conn = GDBPool->Pop();

    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    if (_accountCode > 0)
    {
        _dbOrm.BindParamInt(&_accountCode);
    }
    else
    {
        _dbOrm.BindParamInt(&accountCode);
    }
    result = conn->Execute();

    _dbOrm.BindColInt(sizeof(_playerCode), &_playerCode);
    _dbOrm.BindColWchar(sizeof(_name), &_name);
    _dbOrm.BindColInt(sizeof(_jobCode), &_jobCode);
    _dbOrm.BindColInt(sizeof(_mapCode), &_mapCode);
    _dbOrm.BindColInt(sizeof(_gold), &_gold);
    _dbOrm.BindColInt(sizeof(_lv), &_lv);
    _dbOrm.BindColInt(sizeof(_exp), &_exp);

    return result;
}

bool SessionDB::PlayerDB(int32 accountCode, int32 type)
{
    const wchar_t* query = L"SELECT playerCode, name, jobCode, mapCode, gold, lv, exp FROM Player WHERE accountCode = ? AND jobCode = ?";
    if (conn == nullptr)
        conn = GDBPool->Pop();

    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&accountCode);
    _dbOrm.BindParamInt(&type);
    result = conn->Execute();

    _dbOrm.BindColInt(sizeof(_playerCode), &_playerCode);
    _dbOrm.BindColWchar(sizeof(_name), &_name);
    _dbOrm.BindColInt(sizeof(_jobCode), &_jobCode);
    _dbOrm.BindColInt(sizeof(_mapCode), &_mapCode);
    _dbOrm.BindColInt(sizeof(_gold), &_gold);
    _dbOrm.BindColInt(sizeof(_lv), &_lv);
    _dbOrm.BindColInt(sizeof(_exp), &_exp);

    return result;
}

bool SessionDB::GetPlayerDBInfo(int32& playerCode, wchar_t* name, int32& jobCode, int32& mapCode, int32& gold, int32& lv, int32& exp)
{
    if (conn->Fetch())
    {
        playerCode = _playerCode;
        jobCode = _jobCode;
        mapCode = _mapCode;
        gold = _gold;
        lv = _lv;
        exp = _exp;
        wcscpy_s(name, 10, _name);
        return true;
    }
    else
    {
        conn->CloseCursor();
        conn->FreeStmt();
        _dbOrm.ReSetIdx();
        return false;
    }
}

void SessionDB::SavePlayerDB(int32 playerCode, int32 gold)
{
    const wchar_t* query = L"UPDATE Player SET gold = ? WHERE playerCode = ?";
    if (conn == nullptr)
        conn = GDBPool->Pop();

    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&gold);
    _dbOrm.BindParamInt(&playerCode);
    result = conn->Execute();
    conn->EndTran();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();
}

bool SessionDB::InsertCharater(int32 accountCode, int32 type, const wchar_t* name, int32& playerCode)
{
    const wchar_t* query = L"INSERT INTO Player (name, jobCode, mapCode, accountCode, gold, lv) VALUES (?, ?, 1, ?, 0, 1)";
    if (conn == nullptr)
        conn = GDBPool->Pop();
    
    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamWchar(sizeof(name), (SQLPOINTER)name);
    _dbOrm.BindParamInt(&type);
    _dbOrm.BindParamInt(&accountCode);
    result = conn->Execute();
    conn->EndTran();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();

    return result;
}

bool SessionDB::GetAccount(int32 accountCode, int32& cash,
                           int32& curCharaterType, int32& curWeaponType, int32& weaponOne,
                           int32& weaponTwo, int32& weaponThr)
{
    const wchar_t* query =
        L"SELECT cash, curPlayerType, curWeaponType, weaponOne, weaponTwo, weaponThr FROM Account WHERE accountCode = ?";
    if (conn == nullptr)
        conn = GDBPool->Pop();

    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&accountCode);
    result = conn->Execute();

    _dbOrm.BindColInt(sizeof(cash), &cash);
    _dbOrm.BindColInt(sizeof(curCharaterType), &curCharaterType);
    _dbOrm.BindColInt(sizeof(curWeaponType), &curWeaponType);
    _dbOrm.BindColInt(sizeof(weaponOne), &weaponOne);
    _dbOrm.BindColInt(sizeof(weaponTwo), &weaponTwo);
    _dbOrm.BindColInt(sizeof(weaponThr), &weaponThr);
    result = conn->Fetch();

    conn->EndTran();
    conn->CloseCursor();
    conn->FreeStmt();

    _dbOrm.ReSetIdx();

    return result;
}

bool SessionDB::UpdateAccount(int32 accountCode, int32 curCharaterType, int32 curWeaponType, int32 cash,
                              int32 weaponOne, int32 weaponTwo, int32 weaponThr)
{
    const wchar_t* query =
        L"UPDATE Account SET cash = ?, curPlayerType = ?, curWeaponType = ?, weaponOne = ?, weaponTwo = ?, weaponThr = ? WHERE accountCode = ?";

    if (conn == nullptr)
        conn = GDBPool->Pop();

    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&cash);
    _dbOrm.BindParamInt(&curCharaterType);
    _dbOrm.BindParamInt(&curWeaponType);
    _dbOrm.BindParamInt(&weaponOne);
    _dbOrm.BindParamInt(&weaponTwo);
    _dbOrm.BindParamInt(&weaponThr);
    _dbOrm.BindParamInt(&accountCode);
    result = conn->Execute();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();
    return result;
}

bool SessionDB::UpdateExp(int32 playerCode, int32 exp, int32 lv)
{
    const wchar_t* query = L"UPDATE Player SET lv = ?, exp = ? WHERE playerCode = ?";

    if (conn == nullptr)
        conn = GDBPool->Pop();
    
    _dbOrm.SetDBConn(conn);
    bool result = conn->Prepare(query);
    _dbOrm.BindParamInt(&lv);
    _dbOrm.BindParamInt(&exp);
    _dbOrm.BindParamInt(&playerCode);
    result = conn->Execute();
    conn->EndTran();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();
    return result;
}

void SessionDB::ResetDBOrm()
{
    conn->CloseCursor();
    conn->FreeStmt();
    _dbOrm.ReSetIdx();
}
