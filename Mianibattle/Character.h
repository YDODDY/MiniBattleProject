#pragma once
#include <iostream>
#include <string>

class Character
{

public : 
	Character(std::string inputName, int inputHp, int inputMaxHP)
		: name(inputName), hp(inputHp), maxHp(inputMaxHP)
	{
	
	}

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