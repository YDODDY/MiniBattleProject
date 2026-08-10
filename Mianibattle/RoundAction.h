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
	RoundAction first;
	RoundAction second;
};

