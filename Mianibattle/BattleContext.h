#pragma once

struct StatusSnapshot
{
	bool poisoned = false;
	bool stunned = false;

	bool attackUp = false;
	bool defenseUp = false;

	bool directAttackLocked = false;
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

	bool canAttackBuff = true;
	bool canDefenseBuff = true;

	bool canCounter = true;
	bool canParry = true;
};


struct BattleContext
{
	CharacterSnapshot self;
	CharacterSnapshot target;

	ActionControl actionControl;
};

