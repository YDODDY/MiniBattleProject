#pragma once
#include "Status.h"

struct ActionPhaseStartResult
{
	bool canAct = true;
	StatusType preventedBy = StatusType::None;
};