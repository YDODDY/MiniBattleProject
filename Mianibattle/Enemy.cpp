#include "Enemy.h"

Enemy::Enemy(const std::string& name, int hp, int maxHp, int defaultDamage)
	: Character(name, hp, maxHp, defaultDamage)
{

}

BattleAction Enemy::ChooseAction()
{
	return BattleAction::Attack;
}
