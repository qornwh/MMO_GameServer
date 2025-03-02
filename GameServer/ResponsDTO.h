#pragma once
#include "pch.h"
#include "GameUtils.h"

struct AccountDTO
{
	int accountCode = 0;
	int cash = 0;
	int weaponOne = 0;
	int weaponTwo = 0;
	int weaponThr = 0;
	int curPlayerType = 0;
	int curWeaponType = 0;

	void Dto(nlohmann::json data);

	AccountDTO& operator=(const AccountDTO& dto);
};

struct PlayerDTO
{
	int playerCode = 0;
	int jobCode = 0;
	int mapCode = 0;
	int gold = 0;
	int lv = 0;
	int exp = 0;
	std::string name;

	void Dto(nlohmann::json data);

	PlayerDTO& operator=(const PlayerDTO& dto);
};

struct AccountResponse
{
	int32 _ret = 0;
	AccountDTO _account;

	bool response(cpr::Response& res);
};

struct PlayersResponse
{
	int32 _ret = 0;
	Vector<PlayerDTO> _players;
	bool response(cpr::Response& res);
};

struct PlayerResponse
{
	int32 _ret = 0;
	PlayerDTO _player;
	bool response(cpr::Response& res);
};

