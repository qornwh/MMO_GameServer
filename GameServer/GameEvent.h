#pragma once
#include "Inventory.h"
#include "Types.h"

class GameEvent : public std::enable_shared_from_this<GameEvent>
{
public:
    void CreateCharacterMail(int32 playerCode);
    void DropEquipMail(int32 playerCode, EquipItem& item);
    void DropEtcMail(int32 playerCode, EtcItem& item);
};
