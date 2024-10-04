#include "pch.h"
#include "DBOdbc.h"
#include <sqlext.h>

DBOdbc::DBOdbc(int32 size)
{
	_sqlLenList.resize(size);
}

DBOdbc::~DBOdbc()
{
}

void DBOdbc::SetDBConn(DBConnRef conn)
{
	_conn = conn;
}

void DBOdbc::BindColInt(SQLULEN len, SQLPOINTER value)
{
	_conn->BindCol(_colIdx, SQL_C_DEFAULT, len, value, &_sqlLenList[_colIdx - 1]);
	_colIdx++;
}

void DBOdbc::BindColWchar(SQLULEN len, SQLPOINTER value)
{
	_conn->BindCol(_colIdx, SQL_C_WCHAR, len, value, &_sqlLenList[_colIdx - 1]);
	_colIdx++;
}

void DBOdbc::BindParamInt(SQLPOINTER value)
{
	_conn->BindParameter(_paramIdx, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, value, 0, nullptr);
	_paramIdx++;
}

void DBOdbc::BindParamWchar(SQLULEN len, SQLPOINTER value)
{
	_conn->BindParameter(_paramIdx, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, len, 0, value, 0, nullptr);
	_paramIdx++;
}

void DBOdbc::ParamInit()
{
	_paramIdx = 1;
}
