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

StatusGroup Status::GetStatusGroup(StatusType type) const
{
	switch (type)
	{
	case StatusType::Stun:
		return StatusGroup::ActionControl;

	case StatusType::Poison:
		return StatusGroup::ElementalState;

	case StatusType::AttackUp:
	case StatusType::DefenseUp:
		return StatusGroup::StatModifier;

	default:
		return StatusGroup::Independent;
	}

	return StatusGroup::None;
}


bool Status::CheckConflict(StatusType existingType, StatusType newType) const
{
	const StatusGroup existingGroup = GetStatusGroup(existingType);
	const StatusGroup newGroup = GetStatusGroup(newType);

	if (existingGroup == StatusGroup::Independent ||
		newGroup == StatusGroup::Independent)
	{
		return false;
	}

	return existingGroup == newGroup;
}

StatusApplyResult Status::TryAddEffect(StatusEffect newEffect)
{
	for (StatusEffect& existingEffect : effects)
	{
		if (existingEffect.type == newEffect.type)
		{
			existingEffect.remainingTurns =
				newEffect.remainingTurns;

			existingEffect.value =
				newEffect.value;

			return StatusApplyResult::Refreshed;
		}
	}

	for (const StatusEffect& existingEffect : effects)
	{
		if (CheckConflict(existingEffect.type, newEffect.type))
		{
			return StatusApplyResult::Refected;
		}
	}

	effects.push_back(newEffect);
	return StatusApplyResult::Success;
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
	
	case StatusType::AttackUp:
		return "AttackUped";

	case StatusType::DefenseUp:
		return "DefenseUped";

	default:
		return "None";
	}

	return "None";
}
