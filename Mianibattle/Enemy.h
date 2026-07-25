#pragma once

#include "Character.h"

class Enemy : public Character
{

public : 

	Enemy(const std::string& name, int hp, int maxHp);

	BattleAction ChooseAction() override;
};