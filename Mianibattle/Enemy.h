#pragma once

#include "Character.h"
#include "EnemyAI.h"
#include "AIMemoryUpdateData.h"


class Enemy : public Character
{

public : 

	Enemy(const std::string& name, const CharacterStats& stats)
		: Character(name, stats)
	{
	}

	BattleAction ChooseAction(const BattleContext& context) override;
	void RememberPlayerAction(BattleAction action);

	void RememberRound(const AIMemoryUpdateData& data);

private:

	EnemyAI ai;

};