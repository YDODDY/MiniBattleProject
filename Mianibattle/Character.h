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
	bool IsDead() const;
	std::string GetName() const;
	int GetHp() const;


private: 
	std::string name;
	int hp;
	int maxHp;
};