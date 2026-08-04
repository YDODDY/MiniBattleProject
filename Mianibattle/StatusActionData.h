#pragma once

#include "BattleAction.h"
#include "ActionTarget.h"
#include "Status.h"

struct StatusActionData
{
	BattleAction action;
	ActionTarget target = ActionTarget::Self;

	StatusType statusType = StatusType::None;
	int statusTurns = 0;
	float statusValue = 0.0f;

	int cooldownTurns = 0;
};