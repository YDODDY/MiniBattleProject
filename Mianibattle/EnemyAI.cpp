#include "EnemyAI.h"
#include <vector>
#include <iostream>
#include <iomanip>

BattleAction EnemyAI::ChooseAction(const BattleContext& context)
{
    std::vector<ActionScore> scores;
    
    scores.push_back({ BattleAction::Attack, EvaluateAttack(context) });
   
    if (context.actionControl.canPowerAttack)
    {
        scores.push_back({ BattleAction::PowerAttack, EvaluatePowerAttack(context) });
    }

    if (context.actionControl.canPoisonAttack)
    {
        scores.push_back({ BattleAction::PoisonAttack, EvaluatePoisonAttack(context) });
    }

    if (context.actionControl.canStunAttack)
    {
        scores.push_back({ BattleAction::StunAttack, EvaluateStunAttack(context) });
    }

    if (context.actionControl.canHeal)
    {
        scores.push_back({ BattleAction::Heal, EvaluateHeal(context) });
    }

    if (context.actionControl.canGuard)
    {
        scores.push_back({ BattleAction::Guard, EvaluateGuard(context) });
    }

    if (context.actionControl.canAttackBuff)
    {
        scores.push_back({ BattleAction::AttackBuff, EvaluateAttackBuff(context) });
    }

    if(context.actionControl.canDefenseBuff)
    {
        scores.push_back({ BattleAction::DefenseBuff, EvaluateDefenseBuff(context) });
    }

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
        memory.consecutiveUseCount = 1;
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
    if (context.target.status.poisoned ||
        !context.actionControl.canPoisonAttack) return 0;

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
    if (HasActionControlStatus(context.target.status) ||
        !context.actionControl.canStunAttack) return 0;

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

int EnemyAI::EvaluateAttackBuff(const BattleContext& context) const
{
    if (!context.actionControl.canAttackBuff) return 0;
    if (context.self.status.attackUp) return 0;

    int score = 20;

    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);

    // 체력이 충분하면 준비 행동을 할 여유가 있음
    if (selfHpRatio >= 0.75f) score += 30;
    else if (selfHpRatio >= 0.50f) score += 15;

    // 자신이 위험하면 버프보다 생존 행동 우선
    if (selfHpRatio <= 0.35f) score -= 40;

    // 상대 체력이 너무 낮으면 준비보다 즉시 마무리
    if (targetHpRatio <= 0.25f) score -= 35;

    // 상대 체력이 충분히 많으면 강화된 2턴을 활용할 가치가 큼
    if (targetHpRatio >= 0.60f) score += 15;

    // 강화 후 사용할 공격들이 쿨다운에서 곧 사용 가능하면 가치 상승
    if (context.actionControl.canPowerAttack) score += 15;

    return std::max(0, score);
}

int EnemyAI::EvaluateDefenseBuff(const BattleContext& context) const
{
    if (!context.actionControl.canDefenseBuff) return 0;
    if (context.self.status.defenseUp) return 0;

    int score = 20;

    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);

    // 당장 죽을 정도는 아니지만 방어 준비가 필요한 구간
    if (selfHpRatio <= 0.70f) score += 20;
    if (selfHpRatio <= 0.50f) score += 25;

    // 너무 위험하면 지속 버프보다 즉시 생존 행동
    if (selfHpRatio <= 0.25f) score -= 45;

    // 상대가 건강할수록 앞으로 받을 공격 횟수가 많음
    if (targetHpRatio >= 0.60f) score += 15;

    // 상대가 곧 죽으면 방어 투자보다 마무리가 낫다
    if (targetHpRatio <= 0.25f) score -= 30;

    // 이미 독에 걸렸다면 방어력으로 막지 못하는 피해가 있으므로 가치 감소
    if (context.self.status.poisoned) score -= 10;

    return std::max(0, score);
}

bool EnemyAI::HasActionControlStatus(const StatusSnapshot& status) const
{
    return status.stunned;
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

    case BattleAction::Heal:
        return "Heal";

    case BattleAction::Guard:
        return "Guard";

    case BattleAction::AttackBuff:
        return "AttackBuff";

    case BattleAction::DefenseBuff:
        return "DefenseBuff";
            
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
