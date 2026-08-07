#include "EnemyAI.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

BattleAction EnemyAI::ChooseAction(const BattleContext& context)
{
    std::vector<ActionScore> scores;
    
    if (!context.self.status.directAttackLocked)
    {
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

        // parry failure penlaty : directAttacks 불가, parry 는 어차피 이번 턴에 못쓰기 때문에 제외
        if (context.actionControl.canParry)
        {
            scores.push_back({ BattleAction::Parry, EvaluateParry(context) });
        }
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

    if (scores.empty())
    {
        // 실제로는 발생하지 않도록 안전 행동 하나 남겨두기
        return BattleAction::Guard;
    }

    // Utility 평가식 결과값 내림차순 정렬 및 출력으로 수정
    std::sort(scores.begin(), scores.end(), []
    (const ActionScore& left, const ActionScore& right)
        {
            return left.score > right.score;
        });

    const BattleAction selectedAction = scores.front().action;

    PrintDecisionLog(scores, selectedAction);

    PrintMemoryDebug(context);
    
    return selectedAction;
}

void EnemyAI::ResetMemory()
{
    memory = AIMemory{};
}

void EnemyAI::ObservePlayerAction(BattleAction action)
{
    memory.lastPlayerAction = action;
    ++memory.totalPlayerTurns;

    memory.recentPlayerActions.push_back(action);

    if (memory.recentPlayerActions.size() > 5)
    {
        memory.recentPlayerActions.pop_front();
    }

    if (IsDirectAttackAction(action))
    {
        ++memory.totalDirectAttacks;
    }

    if (action == BattleAction::PowerAttack)
    {
        memory.turnsSincePowerAttack = 0;
    }
    else if (memory.turnsSincePowerAttack < 999)
    {
        ++memory.turnsSincePowerAttack;
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

    // 피해를 받아야 하는 Counter 특성
    if (selfHpRatio <= 0.20f) score -= 40;
    else if (selfHpRatio <= 0.40f) score -= 15;
    else if (selfHpRatio >= 0.60f) score += 10;

    // "공격이 올 것 같다" 정도부터 Counter 가치 상승
    if (threat >= 40) score += 25;

    if (threat >= 60) score += 15;

    // 공격 확신이 매우 높으면 Parry에게 일부 양보
    if (threat >= 75)
        score -= 15;

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

    if (threat >= 60) score += 25;

    if (threat >= 75) score += 45;

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

    if (context.target.status.attackUp) threat += 25;

    const float recentAttackRatio = GetRecentDirectAttackRatio();
    const float overallAttackRatio = GetOverallDirectAttackRatio();

    // 최근 성향을 더 중요하게 봄
    if (recentAttackRatio >= 0.8f) threat += 30;
    else if (recentAttackRatio >= 0.6f) threat += 20;
    else if (recentAttackRatio >= 0.4f) threat += 10;

    // 전체적인 성향은 약하게 반영
    if (overallAttackRatio >= 0.7f) threat += 10;
    else if (overallAttackRatio <= 0.3f) threat -= 10;

    return std::clamp(threat, 0, 100);
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

bool EnemyAI::IsDirectAttackAction(BattleAction action) const
{
    switch (action)
    {
    case BattleAction::Attack:
    case BattleAction::PowerAttack:
    case BattleAction::PoisonAttack:
    case BattleAction::StunAttack:
        return true;
    }

    return false;
}

float EnemyAI::GetRecentDirectAttackRatio() const
{
    if (memory.recentPlayerActions.empty())
        return 0.0f;

    int directAttackCount = 0;

    for (BattleAction action : memory.recentPlayerActions)
    {
        if (IsDirectAttackAction(action))
        {
            directAttackCount++;
        }
    }

    return static_cast<float>(directAttackCount)
        / memory.recentPlayerActions.size();
}

float EnemyAI::GetOverallDirectAttackRatio() const
{
    if (memory.totalPlayerTurns == 0)
        return 0.0f;

    return static_cast<float>(memory.totalDirectAttacks)
        / memory.totalPlayerTurns;
}

bool EnemyAI::WasLastPlayerAction(BattleAction action) const
{
    return memory.lastPlayerAction == action;
}

bool EnemyAI::HasPlayerRecentlyUsed(BattleAction action)
{
    for (BattleAction act : memory.recentPlayerActions)
    {
        if (act == action)
        {
            return true;
        }
    }

    return false;
}

void EnemyAI::PrintMemoryDebug(const BattleContext& context) const
{

    std::cout
        << "\n========== AI MEMORY ==========\n";

    std::cout
        << "Player Turns         : "
        << memory.totalPlayerTurns
        << '\n';

    std::cout
        << "Last Player Action   : "
        << ToString(memory.lastPlayerAction)
        << '\n';

    std::cout
        << "Recent Attack Ratio  : "
        << GetRecentDirectAttackRatio()
        << '\n';

    std::cout
        << "Overall Attack Ratio : "
        << GetOverallDirectAttackRatio()
        << '\n';

    std::cout
        << "Turns Since PowerAtk : "
        << memory.turnsSincePowerAttack
        << '\n';

    std::cout
        << "Direct Attack Threat : "
        << EstimateDirectAttackThreat(context)
        << '\n';

    std::cout
        << "Recent Actions       : ";

    for (BattleAction action :
    memory.recentPlayerActions)
    {
        std::cout
            << ToString(action)
            << " ";
    }

    std::cout
        << "\n================================\n";
}

