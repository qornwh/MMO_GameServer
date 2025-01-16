#pragma once
#include "Collider.h"

class RectangleCollider : public Collider
{
public:
    RectangleCollider(float width, float height);
    RectangleCollider(float centerX, float centerY, float width, float height);
    ~RectangleCollider();

    float GetWidth() { return _width; }
    float GetHeight() { return _height; }

private:
    float _width = 0.f;
    float _height = 0.f;
};
