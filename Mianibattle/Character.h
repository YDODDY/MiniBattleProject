#pragma once
#include <iostream>
#include <string>
#include "BattleAction.h"

class Character
{

public : 

	Character(std::string inputName, int inputHp, int inputMaxHP)
		: name(inputName), hp(inputHp), maxHp(inputMaxHP)
	{
	
	}

	virtual ~Character() = default;

	virtual BattleAction ChooseAction() = 0;


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
	bool IsGuarding() const;
	


private: 
	std::string name;
	int hp;
	int maxHp;
	bool isGuarding = false;
	int cooldownPowerAttack = false;
};