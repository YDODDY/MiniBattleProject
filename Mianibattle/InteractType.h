#pragma once

enum class InteractType
{
	None,
	Guard,
	Counter,
	Parry
};

const char* ToString(InteractType reaction);