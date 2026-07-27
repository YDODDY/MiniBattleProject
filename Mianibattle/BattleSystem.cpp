#include "BattleSystem.h"
#include "Character.h"
#include "Event.h"
#include <iostream>
#include <random>

void BattleSystem::StartTurn(Character& character)
{
	if (character.IsGuarding())
		character.StopGuarding();

	if (!character.CanUsePowerAttackThisTurn())
	{
		int coolDown = character.GetCoolDownCount();
		character.SetUsedPowerAttackLastTurn(coolDown-1);
	}
}

void BattleSystem::Attack(Character& attacker, Character& target, int damage, bool isPowerAttack)
{
	if (target.IsDead())
		return;

	if (!CheckItWasHit(attacker, target, isPowerAttack))
	{
		eventBus.Publish(MissedEvent{ attacker, target, isPowerAttack });
		return;
	}

	if (CheckIsCritical(attacker, target, isPowerAttack))
	{
		HandleCriticalAttack(attacker, target, damage, isPowerAttack);
		return;
	}

	if (target.IsGuarding() && !isPowerAttack)
	{	
		HandleGuardedAttack(attacker, target);
		return;
	}

	const bool wasAlive = !target.IsDead();
	const int appliedDamage = target.ReceiveDamage(damage);

	if (appliedDamage <= 0) { return; }

	if (isPowerAttack)
	{
		attacker.SetUsedPowerAttackLastTurn(2);
	}

	eventBus.Publish(DamagedEvent{ attacker , target, appliedDamage, isPowerAttack});

	if (wasAlive && target.IsDead())
	{
		eventBus.Publish(DeadEvent{ target });
	}
}

void BattleSystem::Heal(Character& character, int healAmount)
{
	const int appliedHealAmount = character.Heal(healAmount);
	if (appliedHealAmount <= 0) { return; }

	eventBus.Publish(HealedEvent{ character, character, appliedHealAmount });
}

void BattleSystem::ExecuteAction(BattleAction action, Character& actor, Character& target)
{
	switch (action)
	{
	case BattleAction::Attack:
		Attack(actor, target, 5, false);
		break;

	case BattleAction::PowerAttack:
		Attack(actor, target, 10, true);
		break;

	case BattleAction::Heal:
		Heal(actor, 10);
		break;

	case BattleAction::Guard:
		Guard(actor);
		break;

	default:
		break;
	}
	
}

void BattleSystem::Guard(Character& character)
{
	if (!character.IsGuarding())
	{
		character.StartGuarding();
	}
}

void BattleSystem::UnGuard(Character& character)
{
	if (character.IsGuarding())
	{
		character.StopGuarding();
	}
}

void BattleSystem::HandleGuardedAttack(Character& attacker, Character& defender)
{
	eventBus.Publish(GuardEvent{ attacker, defender });
	defender.StopGuarding();
}

bool BattleSystem::CheckItWasHit(Character& attacker, Character& target, bool isPowerAttack)
{
	int accuracyPercent = 90;
	if (isPowerAttack)
		accuracyPercent = 70;
	
	int num; 
	num = rand() & 100;
	if (num <= accuracyPercent)
	{
		return true;
	}

	return false;
}

bool BattleSystem::CheckIsCritical(Character& attacker, Character& target, bool isPowerAttack)
{
	int criticalPercent = 40;
	if (isPowerAttack)
		criticalPercent = 60;

	int num;
	num = rand() & 100;
	if (num <= criticalPercent)
	{
		return true;
	}

	return false;
}

void BattleSystem::HandleCriticalAttack(Character& attacker, Character& target, int damage, bool isPowerAttack)
{
	int criticalDamage = damage + 10;
	const bool wasAlive = !target.IsDead();
	const int appliedDamage = target.ReceiveDamage(criticalDamage);

	if (appliedDamage <= 0) { return; }

	if (isPowerAttack)
	{
		attacker.SetUsedPowerAttackLastTurn(2);
	}

	eventBus.Publish(CriticalDamagedEvent{ attacker , target, appliedDamage, isPowerAttack });

	if (wasAlive && target.IsDead())
	{
		eventBus.Publish(DeadEvent{ target });
	}
}

