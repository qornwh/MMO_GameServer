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
	boost::json::value json;
	
	std::ifstream openFile(configFilePath.data());
	
	CrashFunc(openFile.is_open());
	
	String line = "";
	while (getline(openFile, line))
		jsonStr.append(line);
	openFile.close();
	json = GameUtils::JsonParser::GetStrParser(jsonStr);
	
	boost::json::value unitJson = GameUtils::JsonParser::Parser("units", json);
	boost::json::value playerJson = GameUtils::JsonParser::Parser("players", unitJson);
	boost::json::value monsterJson = GameUtils::JsonParser::Parser("monsters", unitJson);
	boost::json::value weaponJson = GameUtils::JsonParser::Parser("weapons", unitJson);
	boost::json::value skillJson = GameUtils::JsonParser::Parser("Skills", unitJson);
	boost::json::value LvJson = GameUtils::JsonParser::Parser("lvs", json);
	boost::json::value mapJson = GameUtils::JsonParser::Parser("maps", json);
	boost::json::value itemEqiupJson = GameUtils::JsonParser::Parser("itemEquip", json);
	boost::json::value itemEtcJson = GameUtils::JsonParser::Parser("itemEtc", json);
	boost::json::value dropJsonJson = GameUtils::JsonParser::Parser("dropJson", json);
	
	// DB item, dropItem 설정
	// SetItem();
	// SetDropItem();
	
	SetPlayer(playerJson);
	SetMonster(monsterJson);
	SetWeapon(weaponJson);
	SetSkill(skillJson);
	SetLv(LvJson);
	SetItemEquip(itemEqiupJson);
	SetItemEtc(itemEtcJson);
	SetDropItem(dropJsonJson);
	
	SetMap(mapJson);
}

void GameInit::SetMap(boost::json::value& mapJson)
{
	for (int i = 0; i < mapJson.get_array().size(); i++)
	{
		auto& map = mapJson.get_array()[i];
		int32 type = GameUtils::JsonParser::Parser("type", map).get_int64();
		int32 code = GameUtils::JsonParser::Parser("mapCode", map).get_int64();
		int32 x = GameUtils::JsonParser::Parser("x", map).get_int64();
		int32 y = GameUtils::JsonParser::Parser("y", map).get_int64();
		int32 centerX = GameUtils::JsonParser::Parser("centerX", map).get_int64();
		int32 centerY = GameUtils::JsonParser::Parser("centerY", map).get_int64();

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
			boost::json::value monsterMapJson = GameUtils::JsonParser::Parser("monsterMap", map);

			x = GameUtils::JsonParser::Parser("x", monsterMapJson).get_int64();
			y = GameUtils::JsonParser::Parser("y", monsterMapJson).get_int64();
			centerX = GameUtils::JsonParser::Parser("centerX", monsterMapJson).get_int64();
			centerY = GameUtils::JsonParser::Parser("centerY", monsterMapJson).get_int64();

			room->GetGameMap()->GetMonsterMapInfo()->GetRect().X = x;
			room->GetGameMap()->GetMonsterMapInfo()->GetRect().Y = y;
			room->GetGameMap()->GetMonsterMapInfo()->GetRect().CenterX = centerX;
			room->GetGameMap()->GetMonsterMapInfo()->GetRect().CenterY = centerY;
			room->InitMonsters();
		}
	}
}

void GameInit::SetPlayer(boost::json::value& unitJson)
{
	for (auto& unit : unitJson.as_array())
	{
		int32 code = GameUtils::JsonParser::Parser("code", unit).get_int64();
		float attack = GameUtils::JsonParser::Parser("attack", unit).get_double();
		float moveSpeed = GameUtils::JsonParser::Parser("moveSpeed", unit).get_double();
		int32 hp = GameUtils::JsonParser::Parser("hp", unit).get_int64();

		GPlayer->GetCharater().emplace(code, Charater(code, attack, moveSpeed, hp));
	}
}

void GameInit::SetMonster(boost::json::value& unitJson)
{
	for (auto& unit : unitJson.as_array())
	{
		int32 code = GameUtils::JsonParser::Parser("code", unit).get_int64();
		float attack = GameUtils::JsonParser::Parser("attack", unit).get_double();
		float moveSpeed = GameUtils::JsonParser::Parser("moveSpeed", unit).get_double();
		int32 hp = GameUtils::JsonParser::Parser("hp", unit).get_int64();
		int32 exp = GameUtils::JsonParser::Parser("exp", unit).get_int64();

		GMonster->GetCharater().emplace(code, Charater(code, attack, moveSpeed, hp, exp));
	}
}

void GameInit::SetWeapon(boost::json::value& unitJson)
{
	for (auto& unit : unitJson.as_array())
	{
		int32 code = GameUtils::JsonParser::Parser("code", unit).get_int64();
		float attackSpeed = GameUtils::JsonParser::Parser("attackSpeed", unit).get_double();

		GWeapon->GetWeapon().emplace(code, Weapon(code, attackSpeed));
	}
}

void GameInit::SetLv(boost::json::value& lvJson)
{
	for (auto expLv : lvJson.get_array())
	{
		int32 lv = GameUtils::JsonParser::Parser("lv", expLv).get_int64(); // 레벨
		int32 exp = GameUtils::JsonParser::Parser("exp", expLv).get_int64(); // 레벨업에 필요한 경험치량

		GExpLv->GetExpLv().emplace(lv, ExpLv(lv, exp));
	}
}

void GameInit::SetSkill(boost::json::value& unitJson)
{
	for (auto& unit : unitJson.as_array())
	{
		int32 type = GameUtils::JsonParser::Parser("type", unit).get_int64();
		float value = GameUtils::JsonParser::Parser("value", unit).get_double();
		float duration = GameUtils::JsonParser::Parser("duration", unit).get_double();
		int32 code = GameUtils::JsonParser::Parser("code", unit).get_int64();
		float coolTime = GameUtils::JsonParser::Parser("coolTime", unit).get_double();
		int32 weaponCode = GameUtils::JsonParser::Parser("weaponCode", unit).get_int64();

		GSkill->GetSkill().emplace(code, Skill(type, value, duration, code, coolTime, weaponCode));
	}
}

void GameInit::SetItemEquip(boost::json::value& itemJson)
{
	for (auto& equip : itemJson.as_array())
	{
		int32 code = GameUtils::JsonParser::Parser("code", equip).get_int64();
		int32 type = GameUtils::JsonParser::Parser("type", equip).get_int64();
		int32 gold = GameUtils::JsonParser::Parser("gold", equip).get_int64();
		int32 attack = GameUtils::JsonParser::Parser("attack", equip).get_int64();
		int32 speed = GameUtils::JsonParser::Parser("speed", equip).get_int64();

		GEquipItem->AddItem(code, type, gold, attack, speed);
	}
}

void GameInit::SetItemEtc(boost::json::value& itemJson)
{
	for (auto& etc : itemJson.as_array())
	{
		int32 code = GameUtils::JsonParser::Parser("code", etc).get_int64();
		int32 type = GameUtils::JsonParser::Parser("type", etc).get_int64();
		int32 gold = GameUtils::JsonParser::Parser("gold", etc).get_int64();
		int32 maxCnt = GameUtils::JsonParser::Parser("maxCnt", etc).get_int64();

		GEtcItem->AddItem(code, type, gold, maxCnt);
	}
}

void GameInit::SetDropItem(boost::json::value& dropJson)
{
	for (auto& drop : dropJson.as_array())
	{
		int32 monsterCode = GameUtils::JsonParser::Parser("monsterCode", drop).get_int64();
		boost::json::value dropEquipList = GameUtils::JsonParser::Parser("dropEquipList", drop);
		for (auto& item : dropEquipList.as_array())
		{
			int32 itemCode = GameUtils::JsonParser::Parser("itemCode", item).get_int64();
			GDropItem->AddDropEquipItem(monsterCode, itemCode);
		}
		int32 gold = GameUtils::JsonParser::Parser("gold", drop).get_int64();
		boost::json::value dropList = GameUtils::JsonParser::Parser("dropList", drop);
		for (auto& item : dropList.as_array())
		{
			int32 itemCode = GameUtils::JsonParser::Parser("itemCode", item).get_int64();
			int32 cnt = GameUtils::JsonParser::Parser("cnt", item).get_int64();
			GDropItem->AddDropItem(monsterCode, itemCode, cnt);
		}
		GDropItem->AddGold(monsterCode, gold);
	}
}
