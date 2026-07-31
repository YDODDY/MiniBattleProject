#pragma once

struct StatusSnapshot
{
	bool poisoned;
	bool burned;
	bool stunned;
	bool sleeping;
	bool frozen;

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

	bool canHeal = true;
	bool canGuard = true;

	bool canPoisonAttack = true;
	bool canStunAttack = true;
	bool canSleepAttack = true;
	bool canFireAttack = true;
	bool canFreezeAttack = true;
};


struct BattleContext
{
	CharacterSnapshot self;
	CharacterSnapshot target;

	ActionControl actionControl;
};

