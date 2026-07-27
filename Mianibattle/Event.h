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
