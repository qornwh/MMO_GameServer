#include "pch.h"
#include "SessionDB.h"

SessionDB::SessionDB() : _dbOdbc(10)
{
    conn = GDBPool->Pop();
    _dbOdbc.SetDBConn(conn);
}

SessionDB::~SessionDB()
{
    conn->EndTran();
    conn->FreeStmt();
}

//bool SessionDB::LoginDB(const wchar_t* id, int32& accountCode, int32& curCharaterType, int32& curWeaponType, int32& cash)
//{
//    _dbOdbc.SetDBConn(conn);
//    bool result = conn->Prepare(selectSessionAccountQuery);
//    _dbOdbc.BindParamWchar(sizeof(id), (SQLPOINTER)id);
//    result = conn->Execute();
//
//    _dbOdbc.BindColInt(sizeof(_accountCode), &_accountCode);
//    _dbOdbc.BindColWchar(sizeof(_id), &_id);
//    _dbOdbc.BindColWchar(sizeof(_pwd), &_pwd);
//    _dbOdbc.BindColInt(sizeof(_cash), &_cash);
//    _dbOdbc.BindColInt(sizeof(_curCharaterType), &_curCharaterType);
//    _dbOdbc.BindColInt(sizeof(_curWeaponType), &_curWeaponType);
//
//    result = conn->Fetch();
//    conn->CloseCursor();
//    _dbOdbc.ReSetIdx();
//    accountCode = _accountCode;
//    curCharaterType = _curCharaterType;
//    curWeaponType = _curWeaponType;
//    cash = _cash;
//
//    return result;
//}

//bool SessionDB::LoginCheck(const wchar_t* pwd)
//{
//    if (wcscmp(_pwd, pwd) == 0)
//        return true;
//    return false;
//}

//bool SessionDB::CreateAccount(const wchar_t* id, const wchar_t* pwd, int32 cash)
//{
//    _dbOdbc.SetDBConn(conn);
//    bool result = conn->Prepare(insertSessionAccountquery);
//    _dbOdbc.BindParamWchar(sizeof(id), (SQLPOINTER)id);
//    _dbOdbc.BindParamWchar(sizeof(pwd), (SQLPOINTER)pwd);
//    _dbOdbc.BindParamInt(&cash);
//    result = conn->Execute();
//
//    _dbOdbc.ReSetIdx();
//    return result;
//}

//bool SessionDB::GetAccount(int32 accountCode, int32& cash, int32& curCharaterType, int32& curWeaponType, int32& weaponOne, int32& weaponTwo, int32& weaponThr)
//{
//    _dbOdbc.SetDBConn(conn);
//    bool result = conn->Prepare(selectSessionAccountCodeQuery);
//    _dbOdbc.BindParamInt(&accountCode);
//    result = conn->Execute();
//
//    _dbOdbc.BindColInt(sizeof(cash), &cash);
//    _dbOdbc.BindColInt(sizeof(curCharaterType), &curCharaterType);
//    _dbOdbc.BindColInt(sizeof(curWeaponType), &curWeaponType);
//    _dbOdbc.BindColInt(sizeof(weaponOne), &weaponOne);
//    _dbOdbc.BindColInt(sizeof(weaponTwo), &weaponTwo);
//    _dbOdbc.BindColInt(sizeof(weaponThr), &weaponThr);
//    result = conn->Fetch();
//
//    conn->CloseCursor();
//    _dbOdbc.ReSetIdx();
//    return result;
//}

//bool SessionDB::UpdateAccount(int32 accountCode, int32 curCharaterType, int32 curWeaponType, int32 cash, int32 weaponOne, int32 weaponTwo, int32 weaponThr)
//{
//    _dbOdbc.SetDBConn(conn);
//    bool result = conn->Prepare(updateSessionAccountQuery);
//    _dbOdbc.BindParamInt(&cash);
//    _dbOdbc.BindParamInt(&curCharaterType);
//    _dbOdbc.BindParamInt(&curWeaponType);
//    _dbOdbc.BindParamInt(&weaponOne);
//    _dbOdbc.BindParamInt(&weaponTwo);
//    _dbOdbc.BindParamInt(&weaponThr);
//    _dbOdbc.BindParamInt(&accountCode);
//    result = conn->Execute();
//
//    _dbOdbc.ReSetIdx();
//    return result;
//}

//bool SessionDB::PlayerDB(int32 accountCode)
//{
//    _dbOdbc.SetDBConn(conn);
//    bool result = conn->Prepare(selectSessionPlayerQuery);
//    if (_accountCode > 0)
//    {
//        _dbOdbc.BindParamInt(&_accountCode);
//    }
//    else
//    {
//        _dbOdbc.BindParamInt(&accountCode);
//    }
//    result = conn->Execute();
//
//    _dbOdbc.BindColInt(sizeof(_playerCode), &_playerCode);
//    _dbOdbc.BindColWchar(sizeof(_name), &_name);
//    _dbOdbc.BindColInt(sizeof(_jobCode), &_jobCode);
//    _dbOdbc.BindColInt(sizeof(_mapCode), &_mapCode);
//    _dbOdbc.BindColInt(sizeof(_gold), &_gold);
//    _dbOdbc.BindColInt(sizeof(_lv), &_lv);
//    _dbOdbc.BindColInt(sizeof(_exp), &_exp);
//
//    return result;
//}

//bool SessionDB::PlayerDB(int32 accountCode, int32 type)
//{
//    _dbOdbc.SetDBConn(conn);
//    bool result = conn->Prepare(selectSessionPlayerOneQuery);
//    _dbOdbc.BindParamInt(&accountCode);
//    _dbOdbc.BindParamInt(&type);
//    result = conn->Execute();
//
//    _dbOdbc.BindColInt(sizeof(_playerCode), &_playerCode);
//    _dbOdbc.BindColWchar(sizeof(_name), &_name);
//    _dbOdbc.BindColInt(sizeof(_jobCode), &_jobCode);
//    _dbOdbc.BindColInt(sizeof(_mapCode), &_mapCode);
//    _dbOdbc.BindColInt(sizeof(_gold), &_gold);
//    _dbOdbc.BindColInt(sizeof(_lv), &_lv);
//    _dbOdbc.BindColInt(sizeof(_exp), &_exp);
//
//    return result;
//}

//bool SessionDB::GetPlayerDBInfo(int32& playerCode, wchar_t* name, int32& jobCode, int32& mapCode, int32& gold, int32& lv, int32& exp)
//{
//    if (conn->Fetch())
//    {
//        playerCode = _playerCode;
//        jobCode = _jobCode;
//        mapCode = _mapCode;
//        gold = _gold;
//        lv = _lv;
//        exp = _exp;
//        wcscpy_s(name, 10, _name);
//        return true;
//    }
//    else
//    {
//        conn->CloseCursor();
//        _dbOdbc.ReSetIdx();
//        return false;
//    }
//}

void SessionDB::SavePlayerDB(int32 playerCode, int32 gold)
{
    _dbOdbc.SetDBConn(conn);
    bool result = conn->Prepare(updateSessionPlayerGoldQuery);
    _dbOdbc.BindParamInt(&gold);
    _dbOdbc.BindParamInt(&playerCode);
    result = conn->Execute();

    _dbOdbc.ReSetIdx();
}

//bool SessionDB::InsertCharater(int32& playerCode, int32 accountCode, int32 type, const wchar_t* name)
//{
//    _dbOdbc.SetDBConn(conn);
//    bool result = conn->Prepare(insertSessionPlayerQuery);
//    _dbOdbc.BindParamWchar(sizeof(name), (SQLPOINTER)name);
//    _dbOdbc.BindParamInt(&type);
//    _dbOdbc.BindParamInt(&accountCode);
//    result = conn->Execute();
//
//    _dbOdbc.BindColInt(sizeof(playerCode), &playerCode);
//    conn->Fetch();
//
//    conn->CloseCursor();
//    _dbOdbc.ReSetIdx();
//    return result;
//}

bool SessionDB::UpdateExp(int32 playerCode, int32 exp, int32 lv)
{
    _dbOdbc.SetDBConn(conn);
    bool result = conn->Prepare(updateSessionPlayerExpquery);
    _dbOdbc.BindParamInt(&lv);
    _dbOdbc.BindParamInt(&exp);
    _dbOdbc.BindParamInt(&playerCode);
    result = conn->Execute();

    _dbOdbc.ReSetIdx();
    return result;
}

void SessionDB::ResetDBOrm()
{
    conn->CloseCursor();
    _dbOdbc.ReSetIdx();
}
