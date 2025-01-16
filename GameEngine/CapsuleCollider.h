#pragma once
#include "Collider.h"
#include "RectangleCollider.h"

class CapsuleCollider : public Collider
{
public:
    CapsuleCollider(float radius, float height);
    CapsuleCollider(float  x, float y, float radius, float height);
    ~CapsuleCollider();

    float GetRadius() { return _radius; }
    bool Trigger(CapsuleCollider& targetCollider);
    bool Trigger(CapsuleCollider& targetCollider, Vector2& targetPosition1, Vector2& targetPosition2);
    bool Trigger(RectangleCollider& targetCollider, Vector2& targetPosition, Vector2& position);

private:
    float _radius = 0.f;
    float _height = 0.f;
};

