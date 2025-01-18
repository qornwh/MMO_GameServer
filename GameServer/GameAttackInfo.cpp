#include "GameAttackInfo.h"
#include "GameMapInfo.h"
#include "IRoom.h"

GameAttackInfo::GameAttackInfo(GameRoomRef room, int32 number) : GameObjectInfo(room, number, -1), _isStart(false), _presentPosition{0,0}
{
	_collider.SetRadius(10.f);
	_collider.SetHeight(10.f);
}

GameAttackInfo::~GameAttackInfo()
{
	std::cout << "GameAttackInfo remove !!!" << '\n';
}

void GameAttackInfo::StartAttack()
{
	_isStart = true;
}

void GameAttackInfo::EndAttack()
{
	_isStart = false;
}

void GameAttackInfo::Update()
{

}

void GameAttackInfo::SetPresentPosition(Vector2& position)
{
	_presentPosition = position;
}
