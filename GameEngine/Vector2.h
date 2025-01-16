﻿#pragma once
#include <complex>
#include <iostream>

#include "GameEngine.h"

struct Vector2
{
    Vector2(float X, float Y) : X(X), Y(Y)
    {
    }

    float X;
    float Y;

    Vector2& operator=(const Vector2& ref)
    {
        X = ref.X;
        Y = ref.Y;
        return *this;
    }

    Vector2 operator+(const Vector2& ref)
    {
        return Vector2(X + ref.X, Y + ref.Y);
    }

    Vector2 operator-(const Vector2& ref)
    {
        return Vector2(X - ref.X, Y - ref.Y);
    }

    static float Hypot(Vector2 a)
    {
        return hypot(a.X, a.Y);
    }

    static float DotProduct(Vector2 a, Vector2 b)
    {
        return a.X * b.X + a.Y * b.Y;
    }

    static float AbsDotProduct(Vector2 a, Vector2 b)
    {
        return abs(a.X * b.X + a.Y * b.Y);
    }

    // 벡터 외적 공식 사용
    static float crossProductAngle(Vector2 a, Vector2 b)
    {
        return a.X * b.Y - b.X * a.Y;
    }

    // 벡터 크기
    static float Magnitude(Vector2 vector)
    {
        return static_cast<float>(std::sqrt(pow(vector.X, 2) + pow(vector.Y, 2)));
    }

    static Vector2 NormalizeVector(Vector2 vector)
    {
        float mag = Magnitude(vector);

        if (mag != 0.0)
            return Vector2(vector.X / mag, vector.Y / mag);
        else
            return Vector2(0, 0);
    }
    
    // 2점 사이각 구한는 함수
    static float CalculateAngle(Vector2& a, Vector2& b)
    {
        Vector2 sub = b - a;
        //정면벡터를 쓴 이유는 기준으로 잡기 위해 마지막에 나온 각도를 더하는게 아닌 set을 하기때문
        Vector2 forward(1, 0);
        float dot = DotProduct(NormalizeVector(sub), forward);
        float radian = acos(dot);
        float angleRad = GameEngine::MathUtils::RadianToDegree(radian);

        if (crossProductAngle(forward, sub) > 0)
            return angleRad;
        
        return angleRad * (-1);
    }

    // 2차원 Counter Clock Wise
    static float CCW(Vector2& a, Vector2& b, Vector2& c)
    {
        return (b.X - a.X) * (c.Y - a.Y) - (c.X - a.X) * (b.Y - a.Y);
    }
};
