#pragma once
#include "pch.h"

struct LoginRequest
{
	std::string requestStr(const std::string& id, const std::string& pwd);
};

struct AccountRequest
{
	std::string requestStr(const int accountCode);
};

struct PlayerRequest
{
	std::string requestStr(const int accountCode, const int characterType);
};

struct UpdateAccountRequest
{
	std::string requestStr(const int accountCode, const int characterType, const int weaponType);
};

struct BuyCharaterRequest
{
	std::string requestStr(const int accountCode, const int useCash, const int characterType, const std::string& characterName);
};

struct BuyWeaponRequest
{
	std::string requestStr(const int accountCode, const int useCash, const int weaponType);
};
