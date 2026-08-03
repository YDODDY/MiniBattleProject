#pragma once
#include "DamageType.h"
#include "Status.h"		

struct AttackData
{

	BattleAction action;

	float damageMultiplier = 0.0f;
	float hitChanceModifier = 0.0f;

	int cooldownTurns = 0;

	StatusType appliedStatus = StatusType::None;
	int statusTurns = 0;
	float statusValue = 0.0f;

};