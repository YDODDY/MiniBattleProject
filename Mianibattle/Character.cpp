#include "Character.h"
#include <iostream>

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

bool Character::IsGuarding() const
{
	return isGuarding;
}


