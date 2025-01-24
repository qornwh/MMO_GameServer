#include "GameSkill.h"

Skill::Skill(int32 type, float value, float duration, int32 code, float coolTime, int32 weaponCode, int32 targetCount, bool isTargetting, bool moveProjectile, int32 rangeRadius, int32 rangetHeight)
    :_type(static_cast<SkillType>(type)), _value(value), _duration(duration), _code(code), _coolTime(coolTime), _weaponCode(weaponCode), _targetCount(targetCount), _isTargetting(isTargetting), 
    _moveProjectile(moveProjectile), _rangeRadius(rangeRadius), _rangeHeight(rangetHeight)
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
    _targetCount = other._targetCount;
    _isTargetting = other._isTargetting;
    _moveProjectile = other._moveProjectile;
    _rangeRadius = other._rangeRadius;
    _rangeHeight = other._rangeHeight;
}

GameSkill::GameSkill()
{
}

void GameSkill::ClearSkill()
{
    _skill.clear();
}
