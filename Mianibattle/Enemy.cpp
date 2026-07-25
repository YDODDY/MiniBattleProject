#include "Enemy.h"

Enemy::Enemy(const std::string& name, int hp, int maxHp)
	: Character(name, hp, maxHp)
{

}

BattleAction Enemy::ChooseAction()
{
	return BattleAction::Attack;
}
