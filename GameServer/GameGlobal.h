#pragma once
#include "pch.h"

extern std::shared_ptr<class GameRoomManager> GRoomManger;
extern std::shared_ptr<class GameSkill> GSkill;
extern std::shared_ptr<class GameCharater> GPlayer;
extern std::shared_ptr<class GameCharater> GMonster;
extern std::shared_ptr<class GameWeapon> GWeapon;
extern std::shared_ptr<class GameExpLv> GExpLv;
extern std::shared_ptr<class GameEquipItem> GEquipItem;
extern std::shared_ptr<class GameEtcItem> GEtcItem;
extern std::shared_ptr<class GameDrop> GDropItem;
extern std::shared_ptr<class GameEvent> GEvent;

// 일단 유저접속정보도 여기서 관리한다.
extern std::shared_ptr<class GameUserAccess> GUserAccess;