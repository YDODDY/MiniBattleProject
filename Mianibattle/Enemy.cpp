#include "Enemy.h"

BattleAction Enemy::ChooseAction(const BattleContext& context)
{
	return ai.ChooseAction(context);
}
