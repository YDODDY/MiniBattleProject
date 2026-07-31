#include "Character.h"
#include <iostream>

TurnStartResult Character::BeginTurn()
{
	TurnStartResult result = ProcessTurnStartStatuses();
	status.TickTurn();
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

	int previousHp = hp;
	hp -= damage;

	if (hp <= 0)
	{
		hp = 0;
	}

	return previousHp - hp;

}

int Character::Heal(int healAmount)
{
	if (IsDead() || hp == maxHp)
	{
		return 0;
	}

	if (healAmount <= 0)
	{
		return 0;
	}

	int previousHp = hp;
	hp += healAmount;
	
	if (hp >= maxHp)
	{
		hp = maxHp;
	}

	return hp - previousHp;
}

void Character::StartGuarding()
{
	if (isGuarding)
		return;

	isGuarding = true;
}

void Character::StopGuarding()
{
	if (!isGuarding)
		return;

	isGuarding = false;
}

void Character::SetUsedPowerAttackLastTurn(int coolDown)
{
	cooldownPowerAttack = coolDown;
}

bool Character::CanUsePowerAttackThisTurn() const
{
	return cooldownPowerAttack == 0;
}

int Character::GetCoolDownCount() const
{
	return cooldownPowerAttack;
}

bool Character::IsDead() const
{
	return hp <= 0;
}

std::string Character::GetName() const
{
	return name;
}

int Character::GetHp() const
{
	return hp;
}

int Character::GetMaxHp() const
{
	return maxHp;
}

int Character::GetDefualtDamage() const
{
	return defaultDamage;
}

bool Character::IsGuarding() const
{
	return isGuarding;
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

		case StatusType::Sleep:
			result.canAct = false;
			result.preventedBy = StatusType::Sleep;
			break;

		case StatusType::Freeze:
			result.canAct = false;
			result.preventedBy = StatusType::Freeze;
			break;

		case StatusType::Burn:
			ReceiveDamage(effect.value);
			result.damage += effect.value;
			result.damageType = DamageType::DamageOverTime;
			result.preventedBy = StatusType::Burn;
			break;
		}
	}

	return result;
}

const Status& Character::GetStatus() const
{
	return status;
}


