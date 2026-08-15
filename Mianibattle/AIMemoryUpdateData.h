#pragma once
#include "BattleAction.h"
#include "InteractionType.h"


struct InteractionMemoryData
{
    InteractionType type = InteractionType::None;
    InteractionResult result = InteractionResult::None;

    bool enemyWasReactor = false;
};

struct AIMemoryUpdateData
{
    BattleAction playerAction = BattleAction::None;
    BattleAction enemyAction = BattleAction::None;

    std::vector<InteractionMemoryData> interactions;
};