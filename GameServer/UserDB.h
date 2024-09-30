#pragma once
#include "DBOrm.h"
#include "Types.h"

static const wchar_t* selectAccountQuery = L"SELECT accountCode, id FROM Account";
static const wchar_t* selectPlayerQuery = L"SELECT playerCode, name, jobCode, accountCode FROM Player";

struct User
{
    User() : accountCode(-1), id{0, }
    {
    }
    
    User(int32 accountCode, wchar_t* str) : accountCode(accountCode)
    {
        wcscpy_s(id, 10, str);
    }

    User(const User& other)
    {
        accountCode = other.accountCode;
        wcscpy_s(id, 10, other.id);
    }

    User& operator=(const User& other)
    {
        accountCode = other.accountCode;
        wcscpy_s(id, 10, other.id);
        return *this;
    }
    int32 accountCode = 1;
    wchar_t id[10];
};

struct Player
{
    Player() : playerCode(-1), jobCode(0), name{0, }, accountCode(-1)
    {
    }
    
    Player(int32 playerCode, int32 jobCode, int32 accountCode, wchar_t* str) : playerCode(playerCode), jobCode(jobCode), accountCode(accountCode)
    {
        wcscpy_s(name, 10, str);
    }

    Player(const Player& other)
    {
        playerCode = other.playerCode;
        jobCode = other.jobCode;
        accountCode = other.accountCode;
        wcscpy_s(name, 10, other.name);
    }

    Player& operator=(const Player& other)
    {
        playerCode = other.playerCode;
        jobCode = other.jobCode;
        accountCode = other.accountCode;
        wcscpy_s(name, 10, other.name);
        return *this;
    }
    int32 playerCode = 1;
    wchar_t name[10];
    int32 jobCode = 1;
    int32 accountCode = 1;
};

class UserDB
{
public:
    UserDB();
    ~UserDB();
    
    void LoadAccountDB();
    void LoadPlayerDB();
    bool GetAccount(User& user);
    bool GetPlayer(Player& player);

private:
    DBOrm _dbOrm;
	DBConnRef conn;

    User _user;
    Player _player;
};
