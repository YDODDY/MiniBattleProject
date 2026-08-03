#pragma once

#include "Character.h"
#include "EnemyAI.h"

class Enemy : public Character
{

public : 

	Enemy(const std::string& name, const CharacterStats& stats)
		: Character(name, stats)
	{
	}

	BattleAction ChooseAction(const BattleContext& context) override;

private:

	EnemyAI ai;

};