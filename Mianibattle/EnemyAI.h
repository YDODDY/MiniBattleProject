#pragma once

#include "BattleAction.h"
#include "BattleContext.h"
#include <vector>
#include <unordered_map>
#include "AIMemory.h"


struct ActionScore
{
	BattleAction action;
	int score;
};

class EnemyAI
{
public:
	BattleAction ChooseAction(const BattleContext& context);

	void ResetMemory();
	void ObservePlayerAction(BattleAction action);

private:

	AIMemory memory;

	int EvaluateAttack(const BattleContext& context) const;
	int EvaluatePowerAttack(const BattleContext& context) const;
	int EvaluateHeal(const BattleContext& context) const;
	int EvaluatePoisonAttack(const BattleContext& context) const;
	int EvaluateGuard(const BattleContext& context) const;
	int EvaluateStunAttack(const BattleContext& context) const;
	int EvaluateAttackBuff(const BattleContext& context) const;
	int EvaluateDefenseBuff(const BattleContext& context) const;
	int EvaluateCounter(const BattleContext& context) const;
	int EvaluateParry(const BattleContext& context) const;

	int GetRiskVariation(int range) const;
	int EstimateDirectAttackThreat(const BattleContext& context) const;

	bool HasActionControlStatus(const StatusSnapshot& status) const;
	float GetHpRatio(const CharacterSnapshot& character) const;

	const char* ToString(BattleAction action) const;
	void PrintDecisionLog(const std::vector<ActionScore>& scores, BattleAction selectedAction) const;
	void PrintActionScores(const std::vector<ActionScore>& scores, BattleAction selectedAction) const;
		
	bool IsDirectAttackAction(BattleAction action) const;

	float GetRecentDirectAttackRatio() const;
	float GetOverallDirectAttackRatio() const;

	bool WasLastPlayerAction(BattleAction action) const;
	bool HasPlayerRecentlyUsed(BattleAction action);

	void PrintMemoryDebug(const BattleContext& context) const;
 };