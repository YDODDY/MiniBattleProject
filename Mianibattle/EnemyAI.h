#pragma once

#include "BattleAction.h"
#include "BattleContext.h"

class EnemyAI
{
public:
	BattleAction ChooseAction(const BattleContext& context);

};