#include "ResponsDTO.h"

void AccountDTO::Dto(nlohmann::json data)
{
	accountCode = data["accountCode"].get<int32>();
	cash = data["cash"].get<int32>();
	weaponOne = data["weaponOne"].get<int32>();
	weaponTwo = data["weaponTwo"].get<int32>();
	weaponThr = data["weaponThr"].get<int32>();
	curPlayerType = data["curPlayerType"].get<int32>();
	curWeaponType = data["curWeaponType"].get<int32>();
}

AccountDTO& AccountDTO::operator=(const AccountDTO& dto)
{
	accountCode = dto.accountCode;
	cash = dto.cash;
	weaponOne = dto.weaponOne;
	weaponTwo = dto.weaponTwo;
	weaponThr = dto.weaponThr;
	curPlayerType = dto.curPlayerType;
	curWeaponType = dto.curWeaponType;
	return *this;
}

void PlayerDTO::Dto(nlohmann::json data)
{
	playerCode = data["playerCode"].get<int32>();
	jobCode = data["jobCode"].get<int32>();
	mapCode = data["mapCode"].get<int32>();
	gold = data["gold"].get<int32>();
	lv = data["lv"].get<int32>();
	exp = data["exp"].get<int32>();
	name = data["name"].get<std::string>();
}

PlayerDTO& PlayerDTO::operator=(const PlayerDTO& dto)
{
	playerCode = dto.playerCode;
	jobCode = dto.jobCode;
	mapCode = dto.mapCode;
	gold = dto.gold;
	lv = dto.lv;
	exp = dto.exp;
	name = dto.name;
	return *this;
}

bool AccountResponse::response(cpr::Response& res)
{
	nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
	_ret = bodyData["ret"].get<int32>();
	if (_ret == 1)
	{
		nlohmann::json accountJson = GameUtils::JsonParser::Parser("account", bodyData);
		_account.Dto(accountJson);
		return true;
	}
	return false;
}

bool PlayersResponse::response(cpr::Response& res)
{
	nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
	_ret = bodyData["ret"].get<int32>();
	if (_ret == 1)
	{
		nlohmann::json playersJson = GameUtils::JsonParser::Parser("players", bodyData);
		for (auto& playerJson : playersJson)
		{
			PlayerDTO dto;
			dto.Dto(playerJson);
			_players.push_back(dto);
		}
		return true;
	}
	return false;
}

bool PlayerResponse::response(cpr::Response& res)
{
	nlohmann::json bodyData = GameUtils::JsonParser::GetStrParser(res.text);
	_ret = bodyData["ret"].get<int32>();
	if (_ret == 1)
	{
		nlohmann::json playerJson = GameUtils::JsonParser::Parser("player", bodyData);
		_player.Dto(playerJson);
		return true;
	}
	return false;
}
