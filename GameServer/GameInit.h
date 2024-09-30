#pragma once
#include <boost/json.hpp>

class GameInit
{
public:
    GameInit();
    void SetMap(boost::json::value& mapJson, boost::json::value& mapUnitJson);
    void SetPlayer(boost::json::value& unitJson);
    void SetMonster(boost::json::value& unitJson);
    void SetWeapon(boost::json::value& unitJson);
    void SetLv(boost::json::value& lvJson);
    void SetSkill(boost::json::value& unitJson);
    void SetItemEquip(boost::json::value& itemJson);
    void SetItemEtc(boost::json::value& itemJson);
    void SetDropItem(boost::json::value& dropJson);
};
