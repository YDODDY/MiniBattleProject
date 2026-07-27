#pragma once
#include "Character.h"


struct DamagedEvent
{
	Character& attacker;
	Character& target;

	int damage;
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

struct CriticalDamagedEvent
{
	Character& attacker;
	Character& target;

	int criticalDamage;
	bool isPowerAttack;
};