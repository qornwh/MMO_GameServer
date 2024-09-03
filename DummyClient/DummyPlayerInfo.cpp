#include "pch.h"

#include "DummyPlayerInfo.h"

DummyPlayerInfo::DummyPlayerInfo()
{
    _speed = 400.f;
    _map.StartX = -2500;
    _map.StartY = -2000;
    _map.EndX = 2500;
    _map.EndY = 2000;
    
    _name.reserve(20);
}

DummyPlayerInfo::~DummyPlayerInfo()
{
}

Vector3d& DummyPlayerInfo::GetPostion()
{
    return _position;
}

void DummyPlayerInfo::SetPostion(Vector3d vec)
{
    _position = vec;
}

void DummyPlayerInfo::Start()
{
    _position.X = dstX(rng);
    _position.Y = dstY(rng);
}

void DummyPlayerInfo::UpdateMapRange()
{
}

bool DummyPlayerInfo::IsMapRange(int32 mapNum)
{
    MapRange* map = &_map;
    
    if (map->StartX >= _position.X || map->EndX <= _position.X)
        return false;
    if (map->StartY >= _position.Y || map->EndY <= _position.Y)
        return false;
    return true;
}

void DummyPlayerInfo::updatePosition()
{
    _position.Yaw = rot(rng);
    _position.X += sinf(_position.Yaw * 3.14f / 180.f) * _speed;
    _position.Y += cosf(_position.Yaw * 3.14f / 180.f) * _speed;
    
    MapRange* map = &_map;
    
    if (map->StartX >= _position.X)
        _position.X = map->StartX;
    if (map->EndX <= _position.X)
        _position.X = map->EndX;
    if (map->StartY >= _position.Y)
        _position.Y = map->StartY;
    if (map->EndY <= _position.Y)
        _position.Y = map->EndY;
}

void DummyPlayerInfo::SetName(const std::wstring& name)
{
    _name = name;
}
