#pragma once

#include "BattleAction.h"
#include <deque>
#include <unordered_map>


struct AIMemory
{
    // Recent
    std::deque<BattleAction> recentPlayerActions;
    BattleAction lastPlayerAction = BattleAction::None;

    // Aggregate
    int totalPlayerActions = 0;
    int playerDirectAttackCount = 0;

    int playerPowerAttackCount = 0;
    int playerGuardCount = 0;
    int playerCounterCount = 0;
    int playerParryCount = 0;

    // Tactical
    std::unordered_map<BattleAction, int> roundsSincePlayerUsedAction;

    int enemyGuardBrokenCount = 0;

    int enemyCounterSuccessCount = 0;
    int enemyCounterFailedCount = 0;

    int enemyParrySuccessCount = 0;
    int enemyParryFailedCount = 0;
};