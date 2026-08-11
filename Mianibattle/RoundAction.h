#pragma once
#include "Character.h"


struct RoundAction
{
	Character* actor = nullptr;
	Character* target = nullptr;
	BattleAction action = BattleAction::None;
};

struct RoundContext
{
	int roundNumber = 0;

	RoundAction first;
	RoundAction second;
};

