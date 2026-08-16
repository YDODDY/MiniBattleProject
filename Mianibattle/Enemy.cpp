#include "Enemy.h"

BattleAction Enemy::ChooseAction(const BattleContext& context)
{
	return ai.ChooseAction(context);
}

void Enemy::RememberRound(const AIMemoryUpdateData& data)
{
	ai.RememberRound(data);
}
