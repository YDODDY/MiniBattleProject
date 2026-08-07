#pragma once
#include "Character.h"

struct DamagedEvent
{
	Character& attacker;
	Character& target;

	int damage;

	DamageType damageType;

	bool isCritical;
	BattleAction sourceAction;
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

struct MissedEvent
{
	Character& attacker;
	Character& target;

	BattleAction sourceAction;
};

struct AppliedStatusEvent
{
	Character& character;
	StatusEffect statusEffect;
	StatusApplyResult result;
};

struct ActionPreventedEvent
{
	Character& character;
	StatusType reason;
};

struct DamageOverTimeEvent
{
	Character& target;
	int damage;
	StatusType statusType;
};

struct ReactionEvent
{
	Character& attacker;
	Character& reactor;

	ReactionType reaction;
};

struct ReactionFailedEvent
{
	Character& character;

	ReactionType reaction;
	StatusType statusType;
};

struct StatusExpiredEvent
{
	Character& character;
	StatusType statusType;
};
