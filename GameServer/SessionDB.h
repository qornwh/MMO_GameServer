#pragma once
#include "DBOrm.h"

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
	bool InsertCharater(int32 accountCode, int32 type, const wchar_t* name);
	bool UpdateExp(int32 playerCode, int32 exp, int32 lv);
	void ResetDBOrm();

private:
	DBOrm _dbOrm;
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

