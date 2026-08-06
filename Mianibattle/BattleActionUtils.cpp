#include "BattleActionUtils.h"

bool IsDirectAttackAction(BattleAction action)
{
    switch (action)
    {
    case BattleAction::Attack:
    case BattleAction::PowerAttack:
    case BattleAction::PoisonAttack:
    case BattleAction::StunAttack:
        return true;

    default:
        return false;
    }
}
