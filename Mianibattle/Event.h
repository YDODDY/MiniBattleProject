#pragma once
#include "Character.h"

enum class DamageType
{
	Normal,
	Poison
};

struct DamagedEvent
{
	Character& attacker;
	Character& target;

	int damage;

	DamageType damageType;

	bool isCritical;
	bool isPowerAttack;
};

struct HealedEvent
{
	Character& healer;
	Character& target;

	int healAmount;
};

struct DeadEvent
{
	Character& deadCharacter;
};

struct GuardEvent
{
	Character& attacker;
	Character& defender;
};

struct MissedEvent
{
	Character& attacker;
	Character& target;

	bool isPowerAttack;
};

struct AttackData
{
	int damage;
	DamageType damageType;

	bool isPowerAttack;
	bool hasStatusEffect;

	StatusEffect statusEffect;
};