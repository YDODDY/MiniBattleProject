#include "EnemyAI.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

BattleAction EnemyAI::ChooseAction(const BattleContext& context)
{
    /*
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

    if (context.actionControl.canCounter)
    {
        scores.push_back({ BattleAction::Counter, EvaluateCounter(context) });
    }

    if (context.actionControl.canParry)
    {
        scores.push_back({ BattleAction::Parry, EvaluateParry(context) });
    }

    // Utility 평가식 결과값 내림차순 정렬 및 출력으로 수정
    std::sort(scores.begin(), scores.end(), []
    (const ActionScore& left, const ActionScore& right)
        {
            return left.score > right.score;
        });

    const BattleAction selectexAction = scores.front().action;

    PrintDecisionLog(scores, selectexAction);
    UpdateMemory(selectexAction);

    
    return selectexAction;
    */

    return BattleAction::PowerAttack;
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

int EnemyAI::EvaluateCounter(const BattleContext& context) const
{
    const int threat = EstimateDirectAttackThreat(context);
    int score = 15;
    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);

    // Counter 피해를 감당하기 어려움
    if (selfHpRatio <= 0.20f) score -= 50;
    else if (selfHpRatio <= 0.40f) score -= 20;

    // 적당한 체력 여유가 있어 일부 피해를 감수할 수 있음
    if (selfHpRatio >= 0.60f) score += 15;

    // 상대가 공격력 강화 상태라 공격할 가능성이 높음
    if (context.target.status.attackUp) score += 35;

    // Enemy 체력이 낮으면 Player가 마무리를 시도할 가능성이 있음
    if (selfHpRatio <= 0.35f) score += 20;

    // Player 체력이 낮으면 Player도 공격적으로 마무리하려 할 수 있음
    if (targetHpRatio <= 0.30f) score += 15;

    // 상대가 방어 강화 중이면 공격 대신 다른 행동을 할 가능성도 있음
    if (context.target.status.defenseUp) score -= 10;

    if (threat >= 40) score += 30;
    if (threat >= 75) score -= 10;

    score += GetRiskVariation(3);
    return std::max(0, score);
}

int EnemyAI::EvaluateParry(const BattleContext& context) const
{
    const int threat = EstimateDirectAttackThreat(context);
    int score = 5;
    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);

    // 상대가 AttackUp 상태면 직접 공격 가능성이 크게 상승
    if (context.target.status.attackUp) score += 45;

    // 낮은 체력에서는 피해 무효의 가치가 매우 큼
    if (selfHpRatio <= 0.30f) score += 30;

    // 하지만 너무 낮은 체력에서 실패하면 치명적
    if (selfHpRatio <= 0.15f) score -= 15;

    // 상대가 마무리를 노릴 만한 상황
    if (selfHpRatio <= 0.35f) score += 20;

    // 상대 체력이 낮으면 공격으로 승부를 보려 할 가능성
    if (targetHpRatio <= 0.25f) score += 10;

    // 상대가 비공격 준비 행동을 할 여지가 있다고 추정
    if (context.target.status.defenseUp) score -= 15;

    if (threat >= 70) score += 70;

    score += GetRiskVariation(3);

    return std::max(0, score);
}

int EnemyAI::GetRiskVariation(int range) const
{
    return rand() % (range * 2 + 1) - range;
}

int EnemyAI::EstimateDirectAttackThreat(const BattleContext& context) const
{
    int threat = 20;

    if (context.target.status.attackUp) threat += 40;
    if (GetHpRatio(context.self) <= 0.35f) threat += 20;
    if (GetHpRatio(context.target) <= 0.25f) threat += 10;

    return threat;
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
           
    case BattleAction::Counter:
        return "Counter";

    case BattleAction::Parry:
        return "Parry";


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

void EnemyAI::PrintActionScores(const std::vector<ActionScore>& scores, BattleAction selectedAction) const
{
    std::cout << "\n===== Enemy AI =====\n\n";

    for (const ActionScore& actionScore : scores)
    {
        std::cout
            << std::left
            << std::setw(15)
            << ToString(actionScore.action)
            << ": "
            << actionScore.score
            << '\n';
    }

    std::cout << "\n-------------------------\n";

    if (!scores.empty())
    {
        std::cout
            << "Highest Score : "
            << scores.front().score
            << '\n';
    }

    std::cout
        << "Choose        : "
        << ToString(selectedAction)
        << "\n\n";
}

