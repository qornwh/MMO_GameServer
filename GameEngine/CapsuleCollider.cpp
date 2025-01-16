#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider(float radius, float height) : Collider(), _radius(radius), _height(height)
{
}

CapsuleCollider::CapsuleCollider(float x, float y, float radius, float height) : Collider(x, y), _radius(radius), _height(height)
{
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::SetRadius(float radius)
{
	_radius = radius;
}

void CapsuleCollider::SetHeight(float height)
{
	_height = height;
}

bool CapsuleCollider::Trigger(CapsuleCollider& targetCollider)
{
	Vector2& position = GetPosition();
	Vector2& targetPosition = targetCollider.GetPosition();
	float d = Collider::CircleToCircleDistance(position, targetPosition);

	if (d <= _radius + targetCollider._radius)
		return true;
	return false;
}

bool CapsuleCollider::Trigger(CapsuleCollider& targetCollider, Vector2& targetPosition1, Vector2& targetPosition2)
{
	Vector2& position = GetPosition();
	bool isCircle = (_height - _radius) == 0 ? true : false;

	float d = 0;
	if (isCircle)
	{
		// ¿ø Ä¸½¶
		d = CapsuleToCircleDistance(targetPosition1, targetPosition2, position);
	}
	else
	{
		// Ä¸½¶ Ä¸½¶
		float dx = GameEngine::MathUtils::GetCos(_radius, (_height - _radius) / 2);
		float dy = GameEngine::MathUtils::GetSin(_radius, (_height - _radius) / 2);
		Vector2 position1{ position.X - dx, position.Y - dx};
		Vector2 position2{ position.X + dx, position.Y + dy };

		d = CapsuleToCapsuleDistance(targetPosition1, targetPosition2, position1, position2);
	}

	if (d <= _radius + targetCollider._radius)
		return true;
	return false;
}

bool CapsuleCollider::Trigger(RectangleCollider& targetCollider, Vector2& targetPosition, Vector2& position)
{
	return false;
}
