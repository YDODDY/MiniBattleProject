#include "BattleSystem.h"
#include "Character.h"
#include "Event.h"
#include <iostream>

void BattleSystem::StartTurn(Character& character)
{
	if (character.IsGuarding())
		character.StopGuarding();
}

void BattleSystem::Attack(Character& attacker, Character& target, int damage)
{
	if (target.IsDead())
		return;

	if (target.IsGuarding())
	{	
		HandleGuardedAttack(attacker, target);
		return;
	}

	const bool wasAlive = !target.IsDead();
	const int appliedDamage = target.ReceiveDamage(damage);

	if (appliedDamage <= 0) { return; }

	eventBus.Publish(DamagedEvent{ attacker , target, appliedDamage });

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
		Attack(actor, target, 20);
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
		character.StartGurding();
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
