#include "EnemyAI.h"
#include <vector>
#include <iostream>
#include <iomanip>

BattleAction EnemyAI::ChooseAction(const BattleContext& context)
{
    std::vector<ActionScore> scores;
    
    scores.push_back({ BattleAction::Attack, EvaluateAttack(context) });
    scores.push_back({ BattleAction::Heal, EvaluateHeal(context) });
    scores.push_back({ BattleAction::PowerAttack, EvaluatePowerAttack(context) });
    scores.push_back({ BattleAction::PoisonAttack, EvaluatePoisonAttack(context) });
    scores.push_back({ BattleAction::FireAttack, EvaluateFireAttack(context) });
    scores.push_back({ BattleAction::FreezeAttack, EvaluateFreezeAttack(context) });
    scores.push_back({ BattleAction::StunAttack, EvaluateStunAttack(context) });
    scores.push_back({ BattleAction::SleepAttack, EvaluateSleepAttack(context) });
    scores.push_back({ BattleAction::Guard, EvaluateGuard(context) });

    ActionScore currentMax = scores[0];

    for (auto& score : scores)
    {
        if (score.score > currentMax.score)
            currentMax = score;
    }

    PrintDecisionLog(scores, currentMax.action);
    UpdateMemory(currentMax.action);

    return currentMax.action;
}

void EnemyAI::UpdateMemory(BattleAction selectedAction)
{
    if (memory.hasPreviousAction &&
        memory.lastAction == selectedAction)
    {
        ++memory.consecutiveUseCount;
    }
    else
    {
        memory.lastAction = selectedAction;
        memory.consecutiveUseCount 1;
        memory.hasPreviousAction = true;
    }

}

int EnemyAI::EvaluateAttack(const BattleContext& context) const
{
    int score = 40;

    return score;
}

int EnemyAI::EvaluatePowerAttack(const BattleContext& context) const
{
    if (!context.actionControl.canPowerAttack)
        return 0;

    int score = 35;

    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);
    if (targetHpRatio <= 0.35f) score += 40;
    if (selfHpRatio <= 0.2f) score -= 20;

    return score;
}

int EnemyAI::EvaluateHeal(const BattleContext& context) const
{
    int score = 0;

    const float selfHpRatio = GetHpRatio(context.self);
    if (selfHpRatio < 0.5f) score = 50;
    if (selfHpRatio <= 0.3f) score = 90;

    return score;
}

int EnemyAI::EvaluatePoisonAttack(const BattleContext& context) const
{
    if (context.target.status.poisoned) return 0;

    int score = 25;

    const float targetHpRatio = GetHpRatio(context.target);
    if (targetHpRatio >= 0.8f) score += 35;
    if (targetHpRatio <= 0.6f) score -= 25;

    return score;
}

int EnemyAI::EvaluateGuard(const BattleContext& context) const
{
    if (!context.actionControl.canGuard) return 0;

    int score = 10;

    const float selfHpRatio = GetHpRatio(context.self);
    if (selfHpRatio <= 0.5f) score += 25;
    if (selfHpRatio <= 0.3f) score += 35;
    // Heal 이 더 급한 상황이면 guard 밀리도록
    if (selfHpRatio <= 0.2f) score -= 20;
    if (selfHpRatio > 0.3f && selfHpRatio <= 0.5f) score += 50;

    return score;
}

int EnemyAI::EvaluateStunAttack(const BattleContext& context) const
{
    if (HasActionControlStatus(context.target.status))
    {
        return 0;
    }

    int score = 30;

    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);

    // 상대가 아직 오래 살아남을 것 같으면 행동 차단 가치 증가
    if (targetHpRatio >= 0.5f) score += 20;
    // 본인이 위험할수록 상대 행동 차단 가치 증가
    if (selfHpRatio <= 0.4f) score += 25;
    // 상대가 거의 죽을 상황이면 stun 우선순위 낮추고 attack 
    if (targetHpRatio <= 0.25f) score -= 30;

    return score;
}

int EnemyAI::EvaluateFireAttack(const BattleContext& context) const
{
    if (context.target.status.burned) return 0;

    int score = 30;

    const float targetHpRatio = GetHpRatio(context.target);

    // 중반 즈음 압박
    if (targetHpRatio <= 0.7f &&
        targetHpRatio >= 0.35f)
    {
        score += 35;
    }

    // 상대가 거의 죽어가면 도트뎀 보다 직접 타격이 좋음
    if (targetHpRatio < 0.25f) score -= 30;

    return score;
}

int EnemyAI::EvaluateSleepAttack(const BattleContext& context) const
{
    if (HasActionControlStatus(context.target.status))
    {
        return 0;
    }

    int score = 20;

    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);

    // 중간 정도로 다쳤을 때 숨고르기 용
    if (selfHpRatio <= 0.6f &&
        selfHpRatio > 0.3f)
    {
        score += 35;
    }

    // 상대가 아직 오래 살아남을 상황이면 수면 가치 증가
    if (targetHpRatio >= 0.6f) score += 15;
    // 본인이 거의 죽기 직전이면 sleep 보다 heal, stun 우선
    if (selfHpRatio <= 0.25f) score -= 20;

    return score;
}

int EnemyAI::EvaluateFreezeAttack(const BattleContext& context) const
{
    if (HasActionControlStatus(context.target.status))
    {
        return 0;
    }

    int score = 30;

    const float selfHpRatio =
        GetHpRatio(context.self);

    const float targetHpRatio =
        GetHpRatio(context.target);

    // 본인이 위험하면 상대 행동 차단 가치 증가
    if (selfHpRatio <= 0.4f)
        score += 25;

    // 상대 체력이 높으면 제어기 가치 있음
    if (targetHpRatio >= 0.5f)
        score += 20;

    // 상대가 Burn 상태면 Freeze로 덮어쓰건 손해
    if (context.target.status.burned)
        score -= 40;

    return score;
}

bool EnemyAI::HasActionControlStatus(const StatusSnapshot& status) const
{
    return status.stunned
        || status.sleeping
        || status.frozen;
}

float EnemyAI::GetHpRatio(const CharacterSnapshot& character) const
{
    if (character.maxHp <= 0)
        return 0.0f;

    return static_cast<float>(character.hp) 
        / static_cast<float>(character.maxHp);
}

const char* EnemyAI::ToString(BattleAction action) const
{
    switch (action)
    {
    case BattleAction::Attack:
        return "Attack";

    case BattleAction::PowerAttack:
        return "PowerAttack";

    case BattleAction::PoisonAttack:
        return "Poison";

    case BattleAction::StunAttack:
        return "Stun";

    case BattleAction::SleepAttack:
        return "Sleep";

    case BattleAction::FireAttack:
        return "Fire";

    case BattleAction::FreezeAttack:
        return "Freeze";

    case BattleAction::Heal:
        return "Heal";

    case BattleAction::Guard:
        return "Guard";

    default:
        return "Unknown";
    }
}

void EnemyAI::PrintDecisionLog(const std::vector<ActionScore>& scores, BattleAction selectedAction) const
{
    std::cout << "\n===== Enemy AI =====\n\n";

    for (const ActionScore& actionScore : scores)
    {
        std::cout
            << std::left
            << std::setw(13)
            << ToString(actionScore.action)
            << ": "
            << actionScore.score
            << '\n';
    }

    std::cout
        << "\nChoose -> "
        << ToString(selectedAction)
        << "\n\n";

}
