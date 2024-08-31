#pragma once
#include "Types.h"

class Skill
{
public:
    enum SkillType
    {
        ATTACK = 0,
        HEAL = 1,
        ATTACKBUF = 2,
        SPEEDBUF = 3
    };

    Skill(int32 type, float value, float duration, int32 code, float coolTime, int32 weaponCode);
    Skill(const Skill& other);

    SkillType _type;
    float _value;
    float _duration;
    int32 _code;
    float _coolTime;
    int32 _weaponCode;
};

class GameSkill : public std::enable_shared_from_this<GameSkill>
{
public:
    GameSkill();

    void ClearSkill();
    Map<int32, Skill>& GetSkill() { return _skill; }

private:
    Map<int32, Skill> _skill;
};
