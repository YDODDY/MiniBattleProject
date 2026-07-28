#pragma once

#include <vector>

enum class StatusType
{
	None,
	Poison,
	Stun,
	Sleep
};

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

	const StatusEffect* Find(StatusType type) const;

	void DecreaseTurns();
	void RemoveExpired();
};