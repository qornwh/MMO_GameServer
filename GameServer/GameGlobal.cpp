#include "GameGlobal.h"

#include "GameCharater.h"
#include "GameDrop.h"
#include "GameExpLv.h"
#include "GameRoomManager.h"
#include "GameSkill.h"
#include "GameItem.h"
#include "GameUserAccess.h"
#include "GameWeapon.h"

std::shared_ptr<GameRoomManager> GRoomManger = std::make_shared<GameRoomManager>();
std::shared_ptr<GameSkill> GSkill = std::make_shared<GameSkill>();
std::shared_ptr<GameCharater> GPlayer = std::make_shared<GameCharater>();
std::shared_ptr<GameCharater> GMonster = std::make_shared<GameCharater>();
std::shared_ptr<GameWeapon> GWeapon = std::make_shared<GameWeapon>();
std::shared_ptr<GameExpLv> GExpLv = std::make_shared<GameExpLv>();
std::shared_ptr<GameEquipItem> GEquipItem = std::make_shared<GameEquipItem>();
std::shared_ptr<GameEtcItem> GEtcItem = std::make_shared<GameEtcItem>();
std::shared_ptr<GameDrop> GDropItem = std::make_shared<GameDrop>();

std::shared_ptr<GameUserAccess> GUserAccess = std::make_shared<GameUserAccess>();

class GameGlobal
{
public:
    GameGlobal()
    {
    }
    ~GameGlobal()
    {
        GRoomManger->ClearRoom();
        GSkill->ClearSkill();
        GPlayer->ClearCharater();
        GMonster->ClearCharater();
        GWeapon->ClearWeapon();
        GExpLv->ClearExpLv();
        GEquipItem->ClearItem();
        GEtcItem->ClearItem();
        GDropItem->ClearItem();
        GUserAccess->ClearUser();
    }
    
}GGameGlobal;
