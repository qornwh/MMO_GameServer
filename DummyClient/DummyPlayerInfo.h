#pragma once

struct Vector3d
{
    Vector3d(float X, float Y, float Z, float Yaw) : X(X), Y(Y), Z(Z), Yaw(Yaw)
    {
    }

    float X;
    float Y;
    float Z;
    float Yaw;

    Vector3d& operator=(const Vector3d& ref)
    {
        X = ref.X;
        Y = ref.Y;
        Z = ref.Z;
        Yaw = ref.Yaw;
        return *this;
    }
};

struct MapRange
{
    int32 StartX;
    int32 StartY;
    int32 EndX;
    int32 EndY;
};

class DummyPlayerInfo : public std::enable_shared_from_this<DummyPlayerInfo>
{
public:
    DummyPlayerInfo();
    ~DummyPlayerInfo();

    Vector3d& GetPostion();
    void SetPostion(Vector3d vec);
    void Start();
    void UpdateMapRange();
    bool IsMapRange(int32 mapNum);
    void updatePosition();
    void SetName(const std::wstring& name);
    const std::wstring& GetName() { return _name; }

protected:
    Vector3d _position{0, 0, 0, 0};

private:
    MapRange _map{0, 0, 0, 0};
    float _speed = 5.f;
    std::wstring _name;
};
