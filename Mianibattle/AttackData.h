#pragma once
#include "DamageType.h"
#include "Status.h"		
#include <optional>

struct AttackData
{
	int damage;
	DamageType damageType;

	bool isPowerAttack;

	std::optional<StatusEffect> statusEffect;
};