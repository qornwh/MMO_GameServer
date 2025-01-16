#pragma once
#include "Collider.h"
#include "RectangleCollider.h"

class CapsuleCollider : public Collider
{
public:
    CapsuleCollider(float radius, float height);
    CapsuleCollider(float  x, float y, float radius, float height);
    ~CapsuleCollider();

    void SetRadius(float radius);
    float GetRadius() { return _radius; }

    void SetHeight(float height);
    float GetHeight() { return _height; }

    bool Trigger(CapsuleCollider& targetCollider) override;
    bool Trigger(CapsuleCollider& targetCollider, Vector2& targetPosition1, Vector2& targetPosition2) override;
    bool Trigger(RectangleCollider& targetCollider, Vector2& targetPosition, Vector2& position) override;

private:
    float _radius = 0.f;
    float _height = 0.f;
};

