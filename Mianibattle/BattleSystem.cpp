#include "BattleSystem.h"
#include "Character.h"
#include "Event.h"
#include <iostream>

void BattleSystem::Attack(Character& attacker, Character& target, int damage)
{
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

	default:
		break;
	}
	
}
