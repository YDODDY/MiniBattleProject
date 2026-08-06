#pragma once
#include "DamageType.h"
#include "Status.h"

struct TurnStartResult
{
    int damage = 0;
    DamageType damageType = DamageType::None;
    bool canAct = true;
    StatusType preventedBy = StatusType::None;
    std::vector<StatusType> expiredStatuses;
};