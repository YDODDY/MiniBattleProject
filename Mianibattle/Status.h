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
	ElementalState
};

enum class StatusType
{
	None,
	Poison,
	Freeze,
	Burn,
	Stun,
	Sleep
};

std::string ToString(StatusType type);

struct StatusEffect
{
	StatusType type;
	int remainingTurns;
	int value;
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