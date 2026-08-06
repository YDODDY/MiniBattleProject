#pragma once

#include "Character.h"


class Player : public Character
{
public:
	Player(const std::string& name, const CharacterStats& stats)
		: Character(name, stats)
	{
	}

	BattleAction ChooseAction(const BattleContext& context) override;
	BattleAction ConvertChoiceToBattleAction(int choice);
};