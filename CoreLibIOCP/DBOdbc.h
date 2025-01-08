#pragma once
#include "pch.h"
#include "DBConnectPool.h"

class DBOdbc
{
public:
	DBOdbc(int32 size);
	~DBOdbc();

	void SetDBConn(DBConnRef conn);
	void BindColInt(SQLULEN len, SQLPOINTER value);
	void BindColChar(SQLULEN len, SQLPOINTER value);
	void BindColWchar(SQLULEN len, SQLPOINTER value);

	void BindParamInt(SQLPOINTER value);
	void BindParamChar(SQLULEN len, SQLPOINTER value);
	void BindParamWchar(SQLULEN len, SQLPOINTER value);
	void ParamInit();

	void ReSetIdx()
	{
		_colIdx = 1;
		_paramIdx = 1;
	}

private:
	DBConnRef _conn;
	int32 _colIdx = 1;
	int32 _paramIdx = 1;
	Vector<SQLLEN> _sqlLenList;
};
