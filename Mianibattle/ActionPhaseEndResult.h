#pragma once

#include "DamageType.h"
#include "Status.h"

struct StatusDamageResult
{
    StatusType source = StatusType::None;
    int damage = 0;
};

struct ActionPhaseEndResult
{
    int damage = 0;
    StatusType damageSource = StatusType::None;
    std::vector<StatusType> expiredStatuses;
};