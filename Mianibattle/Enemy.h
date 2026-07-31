#pragma once

#include "Character.h"
#include "EnemyAI.h"

class Enemy : public Character
{

public : 

	Enemy(const std::string& name, int hp, int maxHp, int defaultDamage);

	BattleAction ChooseAction(const BattleContext& context) override;

private:

	EnemyAI ai;

};