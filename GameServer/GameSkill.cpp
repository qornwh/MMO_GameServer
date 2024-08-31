#include "GameSkill.h"

Skill::Skill(int32 type, float value, float duration, int32 code, float coolTime, int32 weaponCode): _type(static_cast<SkillType>(type)), _value(value), _duration(duration),
                                                                                                     _code(code), _coolTime(coolTime), _weaponCode(weaponCode)
{
}

Skill::Skill(const Skill& other)
{
    _type = static_cast<SkillType>(other._type);
    _value = other._value;
    _duration = other._duration;
    _code = other._code;
    _coolTime = other._coolTime;
    _weaponCode = other._weaponCode;
}

GameSkill::GameSkill()
{
}

void GameSkill::ClearSkill()
{
    _skill.clear();
}
