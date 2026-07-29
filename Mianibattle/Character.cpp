#include "Character.h"
#include <iostream>

TurnStartResult Character::BeginTurn()
{
	TurnStartResult result;

	const StatusEffect* poison = status.Find(StatusType::Poison);

	if (poison != nullptr)
	{
		result.damage = ReceiveDamage(poison->value);
		result.damageType = DamageType::Poison;
	}

	status.DecreaseTurns();
	status.RemoveExpired();

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

int Character::GetDefualtDamage() const
{
	return defaultDamage;
}

bool Character::IsGuarding() const
{
	return isGuarding;
}


void Character::ApplyStatus(const StatusEffect& effect)
{
	status.Add(effect);
}

bool Character::HasStatus(StatusType type) const
{
	return status.Has(type);
}

void Character::RemoveStatus(StatusType type)
{
	status.Remove(type);
}


