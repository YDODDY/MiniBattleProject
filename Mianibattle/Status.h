#pragma once

#include <vector>
#include <string>

enum class StatusApplyResult
{
	Success,
	Refreshed,
	Refected
};

enum class StatusGroup
{
	None,
	Independent,
	ActionControl,
	ElementalState,
	StatModifier
};

enum class StatusType
{
	None,
	Poison,
	Stun,
	AttackUp,
	DefenseUp
};

std::string ToString(StatusType type);

struct StatusEffect
{
	StatusType type = StatusType::None;
	int remainingTurns = 0;
	float value = 0.0f;
};

class Status
{
private:
	std::vector<StatusEffect> effects;

public:
	void Add(StatusEffect effect);
	bool Has(StatusType type) const;
	void Remove(StatusType type);
	void TickTurn();

	const StatusEffect* Find(StatusType type) const;

	const std::vector<StatusEffect>& GetEffects() const;

	StatusGroup GetStatusGroup(StatusType type) const;
	bool CheckConflict(StatusType existingType, StatusType newType) const;
	StatusApplyResult TryAddEffect(StatusEffect effect);

};