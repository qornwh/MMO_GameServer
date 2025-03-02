#include "RequestDTO.h"

std::string LoginRequest::requestStr(const std::string& id, const std::string& pwd)
{
	nlohmann::json json;
	json["id"] = id.c_str();
	json["pwd"] = pwd.c_str();
	return json.dump();
}

std::string AccountRequest::requestStr(const int accountCode)
{
	nlohmann::json json;
	json["accountCode"] = accountCode;
	return json.dump();
}

std::string PlayerRequest::requestStr(const int accountCode, const int characterType)
{
	nlohmann::json json;
	json["accountCode"] = accountCode;
	json["characterType"] = characterType;
	return json.dump();
}

std::string UpdateAccountRequest::requestStr(const int accountCode, const int characterType, const int weaponType)
{
	nlohmann::json json;
	json["accountCode"] = accountCode;
	json["characterType"] = characterType;
	json["weaponType"] = weaponType;
	return json.dump();
}

std::string BuyCharaterRequest::requestStr(const int accountCode, const int useCash, const int characterType, const std::string& characterName)
{
	nlohmann::json json;
	json["accountCode"] = accountCode;
	json["useCash"] = useCash;
	json["characterType"] = characterType;
	json["characterName"] = characterName;
	return json.dump();
}

std::string BuyWeaponRequest::requestStr(const int accountCode, const int useCash, const int weaponType)
{
	nlohmann::json json;
	json["accountCode"] = accountCode;
	json["useCash"] = useCash;
	json["weaponType"] = weaponType;
	return json.dump();
}
