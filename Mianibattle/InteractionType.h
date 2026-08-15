#pragma once
#include <vector>

struct RoundAction;

enum class InteractionType
{
	None,
	Guard,
	Counter,
	Parry
};

enum class InteractionResult
{
	None,
	Success,
	Failed
};

struct InteractionPlan
{
	InteractionType interaction = InteractionType::None;
	InteractionResult result = InteractionResult::None;

	RoundAction* attacker = nullptr;
	RoundAction* reactor = nullptr;
};

struct RoundResolutionPlan
{
	bool hasInteraction = false;
	std::vector<InteractionPlan> interactions;
};