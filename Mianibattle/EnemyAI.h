#pragma once

#include "BattleAction.h"
#include "BattleContext.h"
#include <vector>

struct ActionScore
{
	BattleAction action;
	int score;
};

class EnemyAI
{
public:
	BattleAction ChooseAction(const BattleContext& context);

private:

	int EvaluateAttack(const BattleContext& context) const;
	int EvaluatePowerAttack(const BattleContext& context) const;
	int EvaluateHeal(const BattleContext& context) const;
	int EvaluatePoisonAttack(const BattleContext& context) const;
	int EvaluateGuard(const BattleContext& context) const;
	int EvaluateStunAttack(const BattleContext& context) const;
	int EvaluateFireAttack(const BattleContext& context) const;
	int EvaluateSleepAttack(const BattleContext& context) const;
	int EvaluateFreezeAttack(const BattleContext& context) const;

	bool HasActionControlStatus(const StatusSnapshot& status) const;
	float GetHpRatio(const CharacterSnapshot& character) const;

	const char* ToString(BattleAction action) const;
	void PrintDecisionLog(const std::vector<ActionScore>& scores, BattleAction selectedAction) const;

};