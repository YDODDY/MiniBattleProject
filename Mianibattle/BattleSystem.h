#pragma once

#include "Character.h"
#include "Event.h"
#include "EventBus.h"

class BattleSystem
{
public : 
	BattleSystem(EventBus& inputEventBus) : eventBus(inputEventBus)
	{

	}

	void Attack(Character& attacker, Character& target, int damage);

	void Heal(Character& character, int healAmount);


private:
	
	EventBus& eventBus;

};