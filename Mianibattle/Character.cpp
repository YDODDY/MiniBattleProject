#include "Character.h"
#include <iostream>
#include <algorithm>
#include "BattleActionUtils.h"

TurnStartResult Character::BeginTurn()
{
	TurnStartResult result = ProcessTurnStartStatuses();
	result.expiredStatuses = status.TickTurn();
	TickCooldowns();

	return result;
}

int Character::ReceiveDamage(int damage)
{
	if (IsDead())
	{
		return 0;
	}

	if (damage <= 0)
	{
		return 0;
	}

	int previousHp = selfHp;
	selfHp -= damage;

	if (selfHp <= 0)
	{
		selfHp = 0;
	}

	return previousHp - selfHp;

}

int Character::Heal(int healAmount)
{
	if (IsDead() || selfHp == selfStats.maxHp)
		return 0;

	if (healAmount <= 0)
		return 0;

	int previousHp = selfHp;
	selfHp += healAmount;

	if (selfHp >= selfStats.maxHp)
		selfHp = selfStats.maxHp;

	return selfHp - previousHp;
}


bool Character::CanUseAction(BattleAction action) const
{
	const bool isDirectAttackLocked =
		status.Find(StatusType::DirectAttackLocked)	!= nullptr;

	if (isDirectAttackLocked && IsDirectAttackAction(action))
	{
		return false;
	}

	for (const ActionCooldown& cooldown : cooldowns)
	{
		if (cooldown.action == action)
		{
			return cooldown.remainingTurns <= 0;
		}
	}

	return true;
}

void Character::StartCooldown(BattleAction action, int turns)
{
	if (turns <= 0)
		return;

	for (ActionCooldown& cooldown : cooldowns)
	{
		if (cooldown.action == action)
		{
			cooldown.remainingTurns = turns;
			return;
		}
	}

	cooldowns.push_back({action, turns});
}

void Character::TickCooldowns()
{
	for (ActionCooldown& cooldown : cooldowns)
	{
		cooldown.remainingTurns--;
	}

	cooldowns.erase(
		std::remove_if(
			cooldowns.begin(),
			cooldowns.end(),
			[](const ActionCooldown& cooldown)
			{
				return cooldown.remainingTurns <= 0;
			}),
		cooldowns.end());

}

int Character::GetRemainingCooldown(BattleAction action) const
{
	for (auto& cooldown : cooldowns)
	{
		if (cooldown.action == action)
		{
			return cooldown.remainingTurns;
		}
	}

	return 0;
}


int Character::GetAttack() const
{
	float multiplier = 1.0f;

	if (const StatusEffect* attackUp = status.Find(StatusType::AttackUp))
		multiplier += attackUp->value;

	return static_cast<int>(selfStats.attack * multiplier);
}

int Character::GetDefense() const
{
	float multiplier = 1.0f;

	if (const StatusEffect* defenseUp = status.Find(StatusType::DefenseUp))
		multiplier += defenseUp->value;

	if (const StatusEffect* defenseDown = status.Find(StatusType::DefenseDown))
		multiplier -= defenseDown->value;

	return static_cast<int>(selfStats.defense * multiplier);
}

int Character::GetBaseAttack() const
{
	return selfStats.attack;
}

float Character::GetAccuracy() const
{
	return selfStats.accuracy;
}

float Character::GetEvasion() const
{
	return selfStats.evasion;
}

float Character::GetCriticalChance() const
{
	return selfStats.criticalChance;
}

float Character::GetCriticalDamageMultiplier() const
{
	return selfStats.criticalDamageMultiplier;
}

bool Character::IsDead() const
{
	return selfHp <= 0;
}

std::string Character::GetName() const
{
	return name;
}

int Character::GetHp() const
{
	return selfHp;
}

int Character::GetMaxHp() const
{
	return selfStats.maxHp;
}

StatusApplyResult Character::ApplyStatus(const StatusEffect& effect)
{
	return status.TryAddEffect(effect);
}

bool Character::HasStatus(StatusType type) const
{
	return status.Has(type);
}

void Character::RemoveStatus(StatusType type)
{
	status.Remove(type);
}

TurnStartResult Character::ProcessTurnStartStatuses()
{
	TurnStartResult result;

	for (const StatusEffect& effect : status.GetEffects())
	{
		switch (effect.type)	
		{
		case StatusType::Poison:
			ReceiveDamage(effect.value);
			result.damage += effect.value;
			result.damageType = DamageType::DamageOverTime;
			result.preventedBy = StatusType::Poison;
			break;
		
		case StatusType::Stun:
			result.canAct = false;
			result.preventedBy = StatusType::Stun;
			break;
		}
	}

	return result;
}

const Status& Character::GetStatus() const
{
	return status;
}

ActionPhaseEndResult Character::ProcessActionPhaseEnd()
{
	ActionPhaseEndResult result;

	for (const StatusEffect& effect : status.GetEffects())
	{
		switch (effect.type)
		{
		case StatusType::Poison:
			ReceiveDamage(effect.value);

			result.damage += effect.value;
			result.damageSource = StatusType::Poison;
			break;

		default:
			break;
		}
	}

	result.expiredStatuses = status.TickTurn();

	TickCooldowns();

	return result;
}

ActionPhaseStartResult Character::ProcessActionPhaseStart()
{
	ActionPhaseStartResult result;

	if(status.Has(StatusType::Stun))
	{
		result.canAct = false;
		result.preventedBy = StatusType::Stun;
		
		status.Consume(StatusType::Stun);	
	}

	return result;
}


