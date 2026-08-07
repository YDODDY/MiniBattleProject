#include "Enemy.h"

BattleAction Enemy::ChooseAction(const BattleContext& context)
{
	return ai.ChooseAction(context);
}

void Enemy::RememberPlayerAction(BattleAction action)
{
	ai.ObservePlayerAction(action);
}
