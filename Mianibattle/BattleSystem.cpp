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

	const bool isHit = CheckItWasHit(attacker, target, attackData);

	if (ApplyReaction(attacker, target, attackData, isHit))
	{
		return;
	}

	if (!isHit)
	{
		eventBus.Publish(MissedEvent{attacker, target, attackData.action});
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

	case BattleAction::AttackBuff:
	case BattleAction::DefenseBuff:
	{
		const StatusActionData data = MakeStatusActionData(action);
		ApplyStatusAction(actor, target, data);
		break;
	}

	case BattleAction::Guard:
		if (!actor.CanUseAction(BattleAction::Guard)) return;

		actor.PrepareReaction(ReactionType::Guard);
		actor.StartCooldown(BattleAction::Guard, 3);
		break;

	case BattleAction::Counter:
		if (!actor.CanUseAction(BattleAction::Counter)) return;

		actor.PrepareReaction(ReactionType::Counter);
		actor.StartCooldown(BattleAction::Counter, 3);
		break;


	case BattleAction::Parry:
		if (!actor.CanUseAction(BattleAction::Parry)) return;

		actor.PrepareReaction(ReactionType::Parry);
		actor.StartCooldown(BattleAction::Parry, 5);
		break;

	default:
		const AttackData data = MakeAttackData(actor, action);
		Attack(actor, target, data);
		break;
	}

	ResolveUnusedReaction(target, action);
	
}

void BattleSystem::HandleGuardedAttack(Character& attacker, Character& defender, const AttackData& attackData, bool isHit)
{

	if ((attackData.action == BattleAction::PowerAttack))
	{
		if (!isHit)
		{
			eventBus.Publish(MissedEvent{attacker, defender, attackData.action});
			return;
		}

		int damage = CalculateRawDamage(attacker, attackData);
		const bool isCritical = CheckIsCritical(attacker);

		if (isCritical)
		{
			damage = ApplyCriticalDamage(damage, attacker);
		}

		damage = CalculateFinalDamage(damage, defender);

		ApplyAttackResult(attacker, defender, damage, attackData, isCritical);
		defender.ClearPreparedReaction();
		return;
	}

	defender.ClearPreparedReaction();
	eventBus.Publish(ReactionEvent{ attacker, defender, ReactionType::Guard });
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
		ApplyAttackStatus(attacker, target, attackData);
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
	return static_cast<int>(attacker.GetAttack() * data.damageMultiplier);
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

StatusActionData BattleSystem::MakeStatusActionData(BattleAction action) const
{
	StatusActionData data;
	data.action = action;

	switch (action)
	{
	case BattleAction::AttackBuff:
		data.target = ActionTarget::Self;
		data.statusType = StatusType::AttackUp;
		data.statusTurns = 3;
		data.statusValue = 0.40f;
		data.cooldownTurns = 5;
		break;

	case BattleAction::DefenseBuff:
		data.target = ActionTarget::Self;
		data.statusType = StatusType::DefenseUp;
		data.statusTurns = 3;
		data.statusValue = 0.40f;
		data.cooldownTurns = 5;
		break;

	default:
		break;
	}

	return data;
}

void BattleSystem::ApplyStatusAction(Character& actor, Character& opponent, const StatusActionData& data)
{
	if (!actor.CanUseAction(data.action))
		return;

	Character& receiver = data.target == ActionTarget::Self ? actor : opponent;
	actor.StartCooldown(data.action, data.cooldownTurns);

	StatusEffect effect;
	effect.type = data.statusType;
	effect.remainingTurns = data.statusTurns;
	effect.value = data.statusValue;

	const StatusApplyResult result = receiver.ApplyStatus(effect);

	eventBus.Publish(AppliedStatusEvent{receiver, effect, result});

}

bool BattleSystem::IsDirectAttack(BattleAction action) const
{
	switch (action)
	{
	case BattleAction::Attack:
	case BattleAction::PowerAttack:
	case BattleAction::PoisonAttack:
	case BattleAction::StunAttack:
		return true;

	default:
		return false;
	}
}

bool BattleSystem::ApplyReaction(Character& attacker, Character& target, const AttackData& attackData, bool isHit)
{
	const ReactionType reaction = target.GetPreparedReaction();

	if (reaction == ReactionType::None)
	{
		return false;
	}

	switch (reaction)
	{
	case ReactionType::Guard:
		HandleGuardedAttack(attacker, target, attackData, isHit);
		return true;

	case ReactionType::Counter:
		target.ClearPreparedReaction();
		HandleCounter(attacker,target,attackData,isHit);
		eventBus.Publish(ReactionEvent{ attacker, target, reaction });
		return true;

	case ReactionType::Parry:
		target.ClearPreparedReaction();
		HandleParry(attacker,target,attackData,isHit);
		eventBus.Publish(ReactionEvent{ attacker, target, reaction });
		return true;

	case ReactionType::None:
	default:
		return false;
	}
}

void BattleSystem::ExecuteCounterAttack(Character& counterAttacker, Character& target, float damageMultiplier, BattleAction sourceAction)
{
	int damage = static_cast<int>(counterAttacker.GetAttack() * damageMultiplier);
	damage = CalculateFinalDamage(damage, target);
	const int appliedDamage = target.ReceiveDamage(damage);

	eventBus.Publish(DamagedEvent{counterAttacker, target, 
		appliedDamage, DamageType::Counter, false, sourceAction});
	
	if (target.IsDead())
	{
		eventBus.Publish(DeadEvent{target});
	}

}

void BattleSystem::HandleCounter(Character& attacker, Character& defender, const AttackData& attackData, bool isHit)
{
	if (isHit)
	{
		int damage = CalculateRawDamage(attacker, attackData);

		const bool isCritical = CheckIsCritical(attacker);
		if (isCritical)
		{
			damage = ApplyCriticalDamage(damage, attacker);
		}

		damage = CalculateFinalDamage(damage, defender);
		damage = static_cast<int>(damage * 0.5f);
		const int appliedDamage = defender.ReceiveDamage(damage);

		eventBus.Publish(DamagedEvent{attacker, defender, 
			appliedDamage, DamageType::Direct, isCritical, attackData.action});

		if (defender.IsDead())
		{
			eventBus.Publish(DeadEvent { defender});
			return;
		}

		ApplyAttackStatus(attacker, defender, attackData);
	}
	else
	{
		eventBus.Publish(MissedEvent{ attacker,defender,attackData.action });
	}


	if (!defender.IsDead())
	{
		ExecuteCounterAttack( defender,attacker, 0.8f, BattleAction::Counter);
	}
}

void BattleSystem::HandleParry(Character& attacker, Character& defender, const AttackData& attackData, bool isHit)
{
	if (!isHit)
	{
		eventBus.Publish( MissedEvent{ attacker, defender, attackData.action });
	}

	StatusEffect effect;
	effect.type = StatusType::AttackUp;
	effect.remainingTurns = 2;
	effect.value = 0.4f;

	const StatusApplyResult result = defender.ApplyStatus(effect);

	eventBus.Publish(AppliedStatusEvent{ defender, effect, result });
	ExecuteCounterAttack( defender, attacker, 1.2f, BattleAction::Parry);
}

void BattleSystem::ResolveUnusedReaction(Character& waitingCharacter, BattleAction performedAction)
{
	if (!waitingCharacter.HasPreparedReaction()) return;
	if (IsDirectAttack(performedAction)) return;

	waitingCharacter.ClearPreparedReaction();

}

void BattleSystem::ApplyAttackStatus(Character& attacker, Character& target, const AttackData& attackData)
{
	if (attackData.appliedStatus == StatusType::None)
		return;

	StatusEffect effect;
	effect.type = attackData.appliedStatus;
	effect.remainingTurns = attackData.statusTurns;

	effect.value = std::max( 1.0f,attacker.GetBaseAttack()
		* attackData.statusValue);

	const StatusApplyResult result = target.ApplyStatus(effect);

	eventBus.Publish( AppliedStatusEvent{ target,effect,result });
}


