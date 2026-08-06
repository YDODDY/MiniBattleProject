#pragma once

enum class ReactionType
{
	None,
	Guard,
	Counter,
	Parry
};

const char* ToString(ReactionType reaction);