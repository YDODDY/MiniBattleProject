#include "BattleSystem.h"
#include "Character.h"
#include "Event.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

TurnStartResult BattleSystem::StartTurn(Character& character)
{
	const bool wasAlive = !character.IsDead();
	TurnStartResult result = character.BeginTurn();

	if (result.damage > 0)
	{
		eventBus.Publish(DamageOverTimeEvent{ character, 
			result.damage, result.preventedBy });
	}

	if (wasAlive && character.IsDead())
	{
		eventBus.Publish(DeadEvent{ character });
	}

	if (!result.canAct)
	{
		eventBus.Publish(ActionPreventedEvent{character, result.preventedBy});
	}
	
		return result;
}

void BattleSystem::Attack(Character& attacker, Character& target, const AttackData& attackData)
{
	if (!attacker.CanUseAction(attackData.action))
	{
		return;
	}

	attacker.StartCooldown(attackData.action, attackData.cooldownTurns);

	if (!CheckItWasHit(attacker, target, attackData))
	{
		eventBus.Publish(MissedEvent{attacker, target, attackData.action});
		return;
	}

	if (target.IsGuarding())
	{
		HandleGuardedAttack(attacker, target);
		return;
	}

	int damage = CalculateRawDamage(attacker, attackData);
	const bool isCritical =	CheckIsCritical(attacker);

	if (isCritical)
	{
		damage = ApplyCriticalDamage(damage, attacker);
	}

	damage = CalculateFinalDamage(damage, target);

	ApplyAttackResult(attacker, target, damage, attackData, isCritical);
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
	case BattleAction::Heal:
		if (!actor.CanUseAction(BattleAction::Heal)) return;

		Heal(actor, 20);
		actor.StartCooldown(BattleAction::Heal, 4);
		break;

	case BattleAction::Guard:
		if (!actor.CanUseAction(BattleAction::Guard)) return;

		Guard(actor);
		actor.StartCooldown(BattleAction::Guard, 2);
		break;


	default:
		const AttackData data = MakeAttackData(actor, action);
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

bool BattleSystem::CheckItWasHit(const Character& attacker, const Character& target, const AttackData& data) const
{
	float hitChance = attacker.GetAccuracy() +
		data.hitChanceModifier - target.GetEvasion();

	hitChance = std::clamp(hitChance, 0.05f, 0.95f);

	const float roll = static_cast<float>(rand()) /
		static_cast<float>(RAND_MAX);

	return roll < hitChance;
}

bool BattleSystem::CheckIsCritical(const Character& attacker) const
{
	const float roll =
		static_cast<float>(rand()) /
		static_cast<float>(RAND_MAX);

	return roll < attacker.GetCriticalChance();
}

AttackData BattleSystem::MakeAttackData(Character& character, BattleAction action)
{
	AttackData data;
	data.action = action;

	switch (action)
	{
	case BattleAction::Attack:
		data.damageMultiplier = 1.0f;
		data.hitChanceModifier = 0.0f;
		data.cooldownTurns = 0;
		break;

	case BattleAction::PowerAttack:
		data.damageMultiplier = 2.2f;
		data.hitChanceModifier = -0.2f;
		data.cooldownTurns = 4;
		break;

	case BattleAction::PoisonAttack:
		data.damageMultiplier = 0.7f;
		data.hitChanceModifier = -0.05f;
		data.cooldownTurns = 5;
		data.appliedStatus = StatusType::Poison;
		data.statusTurns = 3;
		data.statusValue = 0.2f;
		break;

	case BattleAction::StunAttack:
		data.damageMultiplier = 0.4f;
		data.hitChanceModifier = -0.15f;
		data.cooldownTurns = 5;

		data.appliedStatus = StatusType::Stun;
		data.statusTurns = 1;
		break;

	default:
		break;
	}

	return data;
}

void BattleSystem::ApplyAttackResult(Character& attacker, Character& target, int damage, const AttackData& attackData, bool isCritical)
{
    const int appliedDamage = target.ReceiveDamage(damage);
	
	eventBus.Publish(DamagedEvent{ attacker, target, 
		appliedDamage, DamageType::Direct, 
		isCritical, attackData.action });

	if (attackData.appliedStatus != StatusType::None)
	{
		StatusEffect effect;
		effect.type = attackData.appliedStatus;
		effect.remainingTurns = attackData.statusTurns;

		effect.value = std::max(1, static_cast<int>(
			attacker.GetAttack() * attackData.statusValue));

		StatusApplyResult result = target.ApplyStatus(effect);

		eventBus.Publish(AppliedStatusEvent{target, effect, result});
	}

	if (target.IsDead())
	{
		eventBus.Publish(DeadEvent{ target });
	}
}

BattleAction BattleSystem::RequestAction(Character& actor, Character& target)
{
	BattleContext context = builder.Build(actor, target);

	return 	actor.ChooseAction(context);
}

int BattleSystem::CalculateRawDamage(const Character& attacker, const AttackData& data) const
{
	return static_cast<int>(
		attacker.GetAttack() * data.damageMultiplier);
}

int BattleSystem::CalculateFinalDamage(int rawDamage, const Character& target) const
{
	return std::max(1, rawDamage - target.GetDefense());
}

int BattleSystem::ApplyCriticalDamage(int damage, const Character& attacker) const
{
	return static_cast<int>(
		damage * attacker.GetCriticalDamageMultiplier());
}


