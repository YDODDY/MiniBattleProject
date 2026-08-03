#pragma once

struct StatusSnapshot
{
	bool poisoned;
	bool stunned;
};

struct CharacterSnapshot
{
	int hp;
	int maxHp;

	StatusSnapshot status;
};

struct ActionControl
{
	bool canAttack = true;
	bool canPowerAttack = true;
	bool canPoisonAttack = true;
	bool canStunAttack = true;

	bool canHeal = true;
	bool canGuard = true;
};


struct BattleContext
{
	CharacterSnapshot self;
	CharacterSnapshot target;

	ActionControl actionControl;
};

