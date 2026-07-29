#include "Status.h"
#include <algorithm>

void Status::Add(StatusEffect effect)
{
	for (auto& current : effects)
	{
		if (current.type == effect.type)
		{
			current.remainingTurns = effect.remainingTurns;
			current.value = effect.value;
			return;
		}
	}

	effects.push_back(effect);
}

bool Status::Has(StatusType type) const
{
	for (const auto& effect : effects)
	{
		if (effect.type == type)
		{
			return true;
		}
	}

	return false;
}

void Status::Remove(StatusType type)
{
	for (auto it = effects.begin(); it != effects.end(); ++it)
	{
		if (it->type == type)
		{
			effects.erase(it);
			return;
		}
	}
}

void Status::TickTurn()
{
	for (StatusEffect& effect : effects)
	{
		effect.remainingTurns--;
	}

	effects.erase(
		std::remove_if(
			effects.begin(),
			effects.end(),
			[](const StatusEffect& effect)
			{
				return effect.remainingTurns <= 0;
			}),
		effects.end());

}

const StatusEffect* Status::Find(StatusType type) const
{
	for (auto& current : effects)
	{
		if (current.type == type)
		{
			return &current;
		}
	}

	return nullptr;
}

const std::vector<StatusEffect>& Status::GetEffects() const
{
	return effects;
}

std::string ToString(StatusType type)
{
	switch (type)
	{
	case StatusType::None:
		return "None";

	case StatusType::Poison:
		return "Poisoned";

	case StatusType::Stun:
		return "Stuned";

	case StatusType::Sleep:
		return "asleep";

	default:
		return "None";
	}

	return "None";
}
