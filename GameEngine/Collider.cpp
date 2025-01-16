#include "Collider.h"

Collider::Collider(float x, float y, float rotate) : _position{x, y}, _rotate(rotate)
{
}

Collider::~Collider()
{

}

void Collider::SetPosition(float x, float y)
{
    _position.X = x;
    _position.Y = y;
}

void Collider::SetRotate(float rot)
{
    _rotate = rot;
}

bool Collider::Trigger(CapsuleCollider& targetCollider)
{
    return false;
}

bool Collider::Trigger(CapsuleCollider& targetCollider, Vector2& targetPosition1, Vector2& targetPosition2)
{
    return false;
}

bool Collider::Trigger(RectangleCollider& targetCollider, Vector2& targetPosition, Vector2& position)
{
    return false;
}
