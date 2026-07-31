#pragma once
#include <iostream>
#include <string>
#include "BattleAction.h"
#include "Status.h"
#include "TurnStartResult.h"
#include "BattleContext.h"

class Character
{

public : 

	Character(std::string inputName, int inputHp, int inputMaxHP, int inputDefaultDamage)
		: name(inputName), hp(inputHp), maxHp(inputMaxHP), defaultDamage(inputDefaultDamage)
	{
	
	}

	virtual ~Character() = default;

	virtual BattleAction ChooseAction(const BattleContext& context) = 0;

	TurnStartResult BeginTurn();

	int ReceiveDamage(int damage);
	int Heal(int healAmount);
	void StartGuarding();
	void StopGuarding();
	void SetUsedPowerAttackLastTurn(int coolDown);
	bool CanUsePowerAttackThisTurn() const;
	int GetCoolDownCount() const;
	bool IsDead() const;
	std::string GetName() const;
	int GetHp() const;
	int GetMaxHp() const;
	int GetDefualtDamage() const;
	bool IsGuarding() const;

	StatusApplyResult ApplyStatus(const StatusEffect& effect);
	bool HasStatus(StatusType type) const;
	void RemoveStatus(StatusType type);

	TurnStartResult ProcessTurnStartStatuses();

	const Status& GetStatus() const;

private: 
	std::string name;
	int hp;
	int maxHp;
	int defaultDamage;
	bool isGuarding = false;
	int cooldownPowerAttack = false;

	Status status;
};