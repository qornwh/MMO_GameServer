#pragma once
#include "DBOdbc.h"

static const wchar_t* selectSessionAccountQuery = L"SELECT accountCode, id, pwd, cash, curPlayerType, curWeaponType FROM Account WHERE id = ?";
static const wchar_t* insertSessionAccountquery = L"INSERT INTO Account (id, pwd, cash) VALUES (?, ?, ?)";
static const wchar_t* selectSessionAccountCodeQuery = L"SELECT cash, curPlayerType, curWeaponType, weaponOne, weaponTwo, weaponThr FROM Account WHERE accountCode = ?";
static const wchar_t* updateSessionAccountQuery = L"UPDATE Account SET cash = ?, curPlayerType = ?, curWeaponType = ?, weaponOne = ?, weaponTwo = ?, weaponThr = ? WHERE accountCode = ?";
static const wchar_t* selectSessionPlayerQuery = L"SELECT playerCode, name, jobCode, mapCode, gold, lv, exp FROM Player WHERE accountCode = ?";
static const wchar_t* selectSessionPlayerOneQuery = L"SELECT playerCode, name, jobCode, mapCode, gold, lv, exp FROM Player WHERE accountCode = ? AND jobCode = ?";
static const wchar_t* updateSessionPlayerGoldQuery = L"UPDATE Player SET gold = ? WHERE playerCode = ?";
static const wchar_t* insertSessionPlayerQuery = L"INSERT INTO Player (name, jobCode, mapCode, accountCode, gold, lv) OUTPUT inserted.playerCode VALUES (?, ?, 1, ?, 0, 1)";
static const wchar_t* updateSessionPlayerExpquery = L"UPDATE Player SET lv = ?, exp = ? WHERE playerCode = ?";

class SessionDB
{
public:
	SessionDB();
	~SessionDB();

	bool LoginDB(const wchar_t* id, int32& accountCode, int32& curCharaterType, int32& curWeaponType, int32& cash);
	bool LoginCheck(const wchar_t* pwd);
	bool CreateAccount(const wchar_t* id, const wchar_t* pwd, int32 cash);
	bool GetAccount(int32 accountCode, int32& cash, int32& curCharaterType, int32& curWeaponType, int32& weaponOne, int32& weaponTwo, int32& weaponThr);
	bool UpdateAccount(int32 accountCode, int32 curCharaterType, int32 curWeaponType, int32 cash, int32 weaponOne, int32 weaponTwo, int32 weaponThr);
	
	bool PlayerDB(int32 accountCode);
	bool PlayerDB(int32 accountCode, int32 type);
	bool GetPlayerDBInfo(int32& playerCode, wchar_t* name, int32& jobCode, int32& mapCode, int32& gold, int32& lv, int32& exp);
	void SavePlayerDB(int32 playerCode, int32 gold);
	bool InsertCharater(int32& playerCode, int32 accountCode, int32 type, const wchar_t* name);
	bool UpdateExp(int32 playerCode, int32 exp, int32 lv);
	void ResetDBOrm();

private:
	DBOdbc _dbOdbc;
	int32 _accountCode = -1;
	int32 _curCharaterType = -1;
	int32 _curWeaponType = -1;
	int32 _cash = -1;
	wchar_t _id[10] = { 0, };
	wchar_t _pwd[10] = { 0, };

	// player
	int32 _playerCode = -1;
	wchar_t _name[10] = { 0, };
	int32 _jobCode = -1;
	int32 _mapCode = -1;
	int32 _gold = 0;
	int32 _lv = 0;
	int32 _exp = 0;

	DBConnRef conn;
};

