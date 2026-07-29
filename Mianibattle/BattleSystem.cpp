#include "BattleSystem.h"
#include "Character.h"
#include "Event.h"
#include <iostream>
#include <random>

void BattleSystem::StartTurn(Character& character)
{
	const bool wasAlive = !character.IsDead();
	TurnStartResult result = character.BeginTurn();

	if (result.damage > 0)
	{
		eventBus.Publish(DamagedEvent{ character, character, 
			result.damage, result.damageType, false, false });
	}

	if (wasAlive && character.IsDead())
	{
		eventBus.Publish(DeadEvent{ character });
	}

	if (!result.canAct)
	{

	}
}

void BattleSystem::Attack(Character& attacker, Character& target, const AttackData& attackData)
{
	if (target.IsDead())
		return;

	if (!CheckItWasHit(attacker, target, attackData.isPowerAttack))
	{
		eventBus.Publish(MissedEvent{ attacker, target, attackData.isPowerAttack });
		return;
	}

	if (CheckIsCritical(attacker, target, attackData.isPowerAttack))
	{
		ApplyAttackResult(attacker, target, attackData.damage+10, attackData, true);
		return;
	}

	if (target.IsGuarding() && !attackData.isPowerAttack)
	{	
		HandleGuardedAttack(attacker, target);
		return;
	}

	ApplyAttackResult(attacker, target, attackData.damage + 10, attackData, false);
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
		/*
	case BattleAction::Attack:
		Attack(actor, target, data);
		break;

	case BattleAction::PowerAttack:
		Attack(actor, target, data);
		break;

	case BattleAction::PoisonAttack:
		Attack(actor, target, data);
		break;
		*/

	case BattleAction::Heal:
		Heal(actor, 10);
		break;

	case BattleAction::Guard:
		Guard(actor);
		break;

	default:
		AttackData data = MakeAttackData(actor, action);
		Attack(actor, target, data);
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
	num = rand() % 100;
	if (num < accuracyPercent)
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
	num = rand() % 100;
	if (num < criticalPercent)
	{
		return true;
	}

	return false;
}

AttackData BattleSystem::MakeAttackData(Character& character, BattleAction action)
{
	AttackData data;
	data.damage = character.GetDefualtDamage();
	data.damageType = DamageType::Normal;
	data.isPowerAttack = false;
	
	switch (action)
	{
	case BattleAction::Attack:
		break;

	case BattleAction::PowerAttack:
		data.isPowerAttack = true;
		break;

	case BattleAction::PoisonAttack:

		StatusEffect effect;
		effect.type = StatusType::Poison;
		effect.remainingTurns = 3;
		effect.value = 1;

		data.statusEffect = effect;
		break;

		/*
	case BattleAction::Heal:
		data.damageType = DamageType::None;
		data.isPowerAttack = false;
		break;

	case BattleAction::Guard:
		data.damageType = DamageType::None;
		data.isPowerAttack = false;
		break;
		*/

	default:
		break;
	}

	return data;
}

void BattleSystem::ApplyAttackResult(Character& attacker, Character& target, int damage, const AttackData& attackData, bool isCritical)
{
	const bool wasAlive = !target.IsDead();
	const int appliedDamage = target.ReceiveDamage(damage);

	if (appliedDamage <= 0) { return; }

	if (attackData.isPowerAttack)
	{
		attacker.SetUsedPowerAttackLastTurn(2);
	}

	eventBus.Publish(DamagedEvent{ attacker , target, appliedDamage, DamageType::Normal, true, attackData.isPowerAttack });

	if (attackData.statusEffect.has_value())
	{
		target.ApplyStatus(*attackData.statusEffect);
		eventBus.Publish(AppliedStatusEvent{ target, *attackData.statusEffect });
	}

	if (wasAlive && target.IsDead())
	{
		eventBus.Publish(DeadEvent{ target });
	}
}


