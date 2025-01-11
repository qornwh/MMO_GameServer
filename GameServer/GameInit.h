#pragma once
#include "pch.h"

class GameInit
{
public:
    GameInit();
    void SetMap(nlohmann::json& mapJson, nlohmann::json& mapUnitJson);
    void SetPlayer(nlohmann::json& unitJson);
    void SetMonster(nlohmann::json& unitJson);
    void SetWeapon(nlohmann::json& unitJson);
    void SetLv(nlohmann::json& lvJson);
    void SetSkill(nlohmann::json& unitJson);
    void SetItemEquip(nlohmann::json& itemJson);
    void SetItemEtc(nlohmann::json& itemJson);
    void SetDropItem(nlohmann::json& dropJson);
};
