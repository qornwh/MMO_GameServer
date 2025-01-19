#pragma once
#include "GameObjectInfo.h"

class GameAttackInfo : public GameObjectInfo
{
public:
	GameAttackInfo(GameRoomRef room, int32 number);
	~GameAttackInfo();

	void StartAttack();
	void EndAttack();
	void Update();
	void SetPresentPosition(Vector2& position);
	Vector2& GetPresentPosition() { return _presentPosition; };

private:
	bool _isStart;
	Vector2 _presentPosition;
};

