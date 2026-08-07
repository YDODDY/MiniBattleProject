#pragma once

#include "BattleAction.h"
#include <deque>


struct AIMemory
{
	std::deque<BattleAction> recentPlayerActions;

	int totalPlayerTurns = 0;
	int totalDirectAttacks = 0;

	BattleAction lastPlayerAction = BattleAction::None;

	int turnsSincePowerAttack = 999;
};