#pragma once
#include "Vector2.h"

// 충돌은 2d로만 구현함

class CapsuleCollider;
class RectangleCollider;

class Collider
{
public:
    Collider(float x = 0.f, float y = 0.f, float rotate = 0.f);
    virtual ~Collider();

    void SetPosition(float x, float y);
    Vector2& GetPosition() { return _position; }

    void SetRotate(float rot);
    float GetRotate() { return _rotate; }

    virtual bool Trigger(CapsuleCollider& targetCollider);
    virtual bool Trigger(CapsuleCollider& targetCollider, Vector2& targetPosition1, Vector2& targetPosition2);
    virtual bool Trigger(RectangleCollider& targetCollider, Vector2& targetPosition, Vector2& position);

    static float CircleToCircleDistance(Vector2& a, Vector2& b)
    {
        return hypotf(b.X - a.X, b.Y - a.Y);
    }

    static float CapsuleToCircleDistance(Vector2& a, Vector2& b, Vector2& p)
    {
        Vector2 ab = b - a;
        Vector2 pa = p - a;

        float dot = Vector2::DotProduct(pa, ab);
        float len2sqrt = Vector2::Hypot(ab);

        float t = dot / len2sqrt;

        if (t < 0.f)
        {
            // 점 a와 가까워짐
            return Vector2::Hypot(p - a);
        }
        else if (t > 1.f)
        {
            // 점 b와 가까워짐
            return Vector2::Hypot(p - b);
        }
        else
        {
            // 점 p는 선분ab사이를 벗어나지 않음
            // 비율 t만큼 a를 이동시킨 수선의 발 h이다
            Vector2 h{a.X + t * ab.X, a.Y + t * ab.Y};

            return Vector2::Hypot(p - h);
        }
    }

    static float CapsuleToCapsuleDistance(Vector2& a, Vector2& b, Vector2& p, Vector2& q)
    {
        float cross_ab = Vector2::CCW(a, b, p) * Vector2::CCW(a, b, q);
        float cross_pq = Vector2::CCW(p, q, a) * Vector2::CCW(p, q, b);

        // 두 선분이 교차됨
        if (cross_pq * cross_ab <= 0 && cross_ab == 0 && cross_pq == 0)
            return 0.f;

        // 두 선분이 교차하지 않음 + 두 선분이 같은방향으로 감 , 선분과 점사이의 거리 필요(CapsuleToCircleDistance)
        float d = CapsuleToCircleDistance(a, b, p);
        d = min(d, CapsuleToCircleDistance(a, b, q));
        d = min(d, CapsuleToCircleDistance(p, q, a));
        d = min(d, CapsuleToCircleDistance(p, q, b));
        return d;
    }

private:
    Vector2 _position{0.f, 0.f};
    float _rotate = 0.f;
};
