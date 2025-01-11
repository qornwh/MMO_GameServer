#include "GameInit.h"
#include "GameGlobal.h"
#include "GameMapInfo.h"
#include "GameSkill.h"
#include "IRoom.h"
#include <fstream>

#include "GameCharater.h"
#include "GameDrop.h"
#include "GameExpLv.h"
#include "GameItem.h"
#include "GameWeapon.h"

GameInit::GameInit()
{
	String configFilePath = "./config.json";
	String jsonStr = "";
	nlohmann::json json;
	
	std::ifstream openFile(configFilePath.data());
	
	CrashFunc(openFile.is_open());
	
	String line = "";
	while (getline(openFile, line))
		jsonStr.append(line);
	openFile.close();
	json = GameUtils::JsonParser::GetStrParser(jsonStr);
	
	nlohmann::json unitJson = GameUtils::JsonParser::Parser("units", json);
	nlohmann::json playerJson = GameUtils::JsonParser::Parser("players", unitJson);
	nlohmann::json monsterJson = GameUtils::JsonParser::Parser("monsters", unitJson);
	nlohmann::json weaponJson = GameUtils::JsonParser::Parser("weapons", unitJson);
	nlohmann::json skillJson = GameUtils::JsonParser::Parser("Skills", unitJson);
	nlohmann::json LvJson = GameUtils::JsonParser::Parser("lvs", json);
	nlohmann::json mapJson = GameUtils::JsonParser::Parser("maps", json);
	nlohmann::json mapUnitJson = GameUtils::JsonParser::Parser("mapsUnits", json);
	nlohmann::json itemEqiupJson = GameUtils::JsonParser::Parser("itemEquip", json);
	nlohmann::json itemEtcJson = GameUtils::JsonParser::Parser("itemEtc", json);
	nlohmann::json dropJsonJson = GameUtils::JsonParser::Parser("dropJson", json);
	
	SetPlayer(playerJson);
	SetMonster(monsterJson);
	SetWeapon(weaponJson);
	SetSkill(skillJson);
	SetLv(LvJson);
	SetItemEquip(itemEqiupJson);
	SetItemEtc(itemEtcJson);
	SetDropItem(dropJsonJson);
	SetMap(mapJson, mapUnitJson);
}

void GameInit::SetMap(nlohmann::json& mapJson, nlohmann::json& mapUnitJson)
{
	CrashFunc(mapJson.is_array());

	for (int i = 0; i < mapJson.size(); i++)
	{
		auto& map = mapJson[i];
		int32 type = static_cast<int32>(GameUtils::JsonParser::Parser("type", map).get<int32>());
		int32 code = static_cast<int32>(GameUtils::JsonParser::Parser("mapCode", map).get<int32>());
		int32 x = static_cast<int32>(GameUtils::JsonParser::Parser("x", map).get<int32>());
		int32 y = static_cast<int32>(GameUtils::JsonParser::Parser("y", map).get<int32>());
		int32 centerX = static_cast<int32>(GameUtils::JsonParser::Parser("centerX", map).get<int32>());
		int32 centerY = static_cast<int32>(GameUtils::JsonParser::Parser("centerY", map).get<int32>());

		GameRoomRef room = GRoomManger->CreateRoom(code);
		CrashFunc(room != nullptr);
		room->CreateMapInfo(type);
		room->GetGameMap()->SetMapCode(code);
		room->GetGameMap()->GetMapInfo().GetRect().X = x;
		room->GetGameMap()->GetMapInfo().GetRect().Y = y;
		room->GetGameMap()->GetMapInfo().GetRect().CenterX = centerX;
		room->GetGameMap()->GetMapInfo().GetRect().CenterY = centerY;

		if (type == 0)
		{
			nlohmann::json monsterMapJson = GameUtils::JsonParser::Parser("monsterMap", map);

			x = static_cast<int32>(GameUtils::JsonParser::Parser("x", monsterMapJson).get<int32>());
			y = static_cast<int32>(GameUtils::JsonParser::Parser("y", monsterMapJson).get<int32>());
			centerX = static_cast<int32>(GameUtils::JsonParser::Parser("centerX", monsterMapJson).get<int32>());
			centerY = static_cast<int32>(GameUtils::JsonParser::Parser("centerY", monsterMapJson).get<int32>());

			room->GetGameMap()->GetMonsterMapInfo()->GetRect().X = x;
			room->GetGameMap()->GetMonsterMapInfo()->GetRect().Y = y;
			room->GetGameMap()->GetMonsterMapInfo()->GetRect().CenterX = centerX;
			room->GetGameMap()->GetMonsterMapInfo()->GetRect().CenterY = centerY;

			
			auto& mapsUnits = mapUnitJson[i];
			auto mapMonsterList = GameUtils::JsonParser::Parser("monsterList", mapsUnits);
			for(int32 j = 0; j < mapMonsterList.size(); j++)
			{
				auto& unitsInfo = mapMonsterList[j];
				int32 monsterType = static_cast<int32>(GameUtils::JsonParser::Parser("code", unitsInfo).get<int32>());
				int32 monsterCount = static_cast<int32>(GameUtils::JsonParser::Parser("count", unitsInfo).get<int32>());
				room->CreateMonster(monsterType, monsterCount);
			}
		}
	}
}

void GameInit::SetPlayer(nlohmann::json& unitJson)
{
	for (auto& unit : unitJson)
	{
		int32 code = static_cast<int32>(GameUtils::JsonParser::Parser("code", unit).get<int32>());
		float attack = static_cast<float>(GameUtils::JsonParser::Parser("attack", unit).get<float>());
		float moveSpeed = static_cast<float>(GameUtils::JsonParser::Parser("moveSpeed", unit).get<float>());
		int32 hp = static_cast<int32>(GameUtils::JsonParser::Parser("hp", unit).get<int32>());

		GPlayer->GetCharater().emplace(code, Charater(code, attack, moveSpeed, hp));
	}
}

void GameInit::SetMonster(nlohmann::json& unitJson)
{
	for (auto& unit : unitJson)
	{
		int32 code = static_cast<int32>(GameUtils::JsonParser::Parser("code", unit).get<int32>());
		float attack = static_cast<float>(GameUtils::JsonParser::Parser("attack", unit).get<float>());
		float moveSpeed = static_cast<float>(GameUtils::JsonParser::Parser("moveSpeed", unit).get<float>());
		int32 hp = static_cast<int32>(GameUtils::JsonParser::Parser("hp", unit).get<int32>());
		int32 exp = static_cast<int32>(GameUtils::JsonParser::Parser("exp", unit).get<int32>());

		GMonster->GetCharater().emplace(code, Charater(code, attack, moveSpeed, hp, exp));
	}
}

void GameInit::SetWeapon(nlohmann::json& unitJson)
{
	for (auto& unit : unitJson)
	{
		int32 code = static_cast<int32>(GameUtils::JsonParser::Parser("code", unit).get<int32>());
		float attackSpeed = static_cast<float>(GameUtils::JsonParser::Parser("attackSpeed", unit).get<float>());

		GWeapon->GetWeapon().emplace(code, Weapon(code, attackSpeed));
	}
}

void GameInit::SetLv(nlohmann::json& lvJson)
{
	for (auto expLv : lvJson)
	{
		int32 lv = static_cast<int32>(GameUtils::JsonParser::Parser("lv", expLv).get<int32>()); // 레벨
		int32 exp = static_cast<int32>(GameUtils::JsonParser::Parser("exp", expLv).get<int32>()); // 레벨업에 필요한 경험치량

		GExpLv->GetExpLv().emplace(lv, ExpLv(lv, exp));
	}
}

void GameInit::SetSkill(nlohmann::json& unitJson)
{
	for (auto& unit : unitJson)
	{
		int32 type = static_cast<int32>(GameUtils::JsonParser::Parser("type", unit).get<int32>());
		float value = static_cast<float>(GameUtils::JsonParser::Parser("value", unit).get<float>());
		float duration = static_cast<float>(GameUtils::JsonParser::Parser("duration", unit).get<float>());
		int32 code = static_cast<int32>(GameUtils::JsonParser::Parser("code", unit).get<int32>());
		float coolTime = static_cast<float>(GameUtils::JsonParser::Parser("coolTime", unit).get<float>());
		int32 weaponCode = static_cast<int32>(GameUtils::JsonParser::Parser("weaponCode", unit).get<int32>());

		GSkill->GetSkill().emplace(code, Skill(type, value, duration, code, coolTime, weaponCode));
	}
}

void GameInit::SetItemEquip(nlohmann::json& itemJson)
{
	for (auto& equip : itemJson)
	{
		int32 code = static_cast<int32>(GameUtils::JsonParser::Parser("code", equip).get<int32>());
		int32 type = static_cast<int32>(GameUtils::JsonParser::Parser("type", equip).get<int32>());
		int32 gold = static_cast<int32>(GameUtils::JsonParser::Parser("gold", equip).get<int32>());
		int32 attack = static_cast<int32>(GameUtils::JsonParser::Parser("attack", equip).get<int32>());
		int32 speed = static_cast<int32>(GameUtils::JsonParser::Parser("speed", equip).get<int32>());

		GEquipItem->AddItem(code, type, gold, attack, speed);
	}
}

void GameInit::SetItemEtc(nlohmann::json& itemJson)
{
	for (auto& etc : itemJson)
	{
		int32 code = static_cast<int32>(GameUtils::JsonParser::Parser("code", etc).get<int32>());
		int32 type = static_cast<int32>(GameUtils::JsonParser::Parser("type", etc).get<int32>());
		int32 gold = static_cast<int32>(GameUtils::JsonParser::Parser("gold", etc).get<int32>());
		int32 maxCnt = static_cast<int32>(GameUtils::JsonParser::Parser("maxCnt", etc).get<int32>());

		GEtcItem->AddItem(code, type, gold, maxCnt);
	}
}

void GameInit::SetDropItem(nlohmann::json& dropJson)
{
	for (auto& drop : dropJson)
	{
		int32 monsterCode = static_cast<int32>(GameUtils::JsonParser::Parser("monsterCode", drop).get<int32>());
		nlohmann::json dropEquipList = GameUtils::JsonParser::Parser("dropEquipList", drop);
		for (auto& item : dropEquipList)
		{
			int32 itemCode = static_cast<int32>(GameUtils::JsonParser::Parser("itemCode", item).get<int32>());
			GDropItem->AddDropEquipItem(monsterCode, itemCode);
		}
		int32 gold = static_cast<int32>(GameUtils::JsonParser::Parser("gold", drop).get<int32>());
		nlohmann::json dropList = GameUtils::JsonParser::Parser("dropList", drop);
		for (auto& item : dropList)
		{
			int32 itemCode = static_cast<int32>(GameUtils::JsonParser::Parser("itemCode", item).get<int32>());
			int32 cnt = static_cast<int32>(GameUtils::JsonParser::Parser("cnt", item).get<int32>());
			GDropItem->AddDropItem(monsterCode, itemCode, cnt);
		}
		GDropItem->AddGold(monsterCode, gold);
	}
}
