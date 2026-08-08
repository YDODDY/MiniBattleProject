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

    if (memory.totalPlayerTurns <= 1)
    {
        const int bestScore = scores.front().score;
        const int openingThreshold = 30;
        std::vector<ActionScore> openingCandidates;
        for (const ActionScore& actionScore : scores)
        {
            if (actionScore.action == BattleAction::Heal)
                continue;

            if (actionScore.score >= bestScore - openingThreshold)
            {
                openingCandidates.push_back(actionScore);
            }
        }

        const int randomIndex = rand() % openingCandidates.size();

        const BattleAction openingAction = openingCandidates[randomIndex].action;

        PrintDecisionLog(scores, openingAction);
        PrintMemoryDebug(context);

        return openingAction;
    }


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

    // 기본 선호도 점수 (너무 안전추구형은 X)
    int score = 10;

    const float selfHpRatio = GetHpRatio(context.self);
    // 체력이 낮아질 수록 방어적으로 바뀜
    if (selfHpRatio <= 0.5f) score += 25;
    if (selfHpRatio <= 0.3f) score += 35;
    
    // 거의 치명상이다 (PowerAttack 같은거 크리티컬 맞으면 죽을 것 같다) 싶으면 Heal 이 더 급하도록 
    if (selfHpRatio <= 0.2f) score -= 20;

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

    // 기본 선호도 점수, Guard 보다는 높게 -> 체력이 너무 낮아 위험한 상황 아니면 조금 공격적인 방어 느낌을 선호
    // 근데 체력이 너무 낮아지면 쿨타임도 더 짧고 더 안전형인 Gaurd 선호하게 되는 느낌
    int score = 15;

    const float selfHpRatio = GetHpRatio(context.self);

    // 체력이 깎이고 있긴 한데 아직 버틸만 한 것 같으면 (한방에는 안죽을 것 같은 체력상황 즈음?) 방어적인 행동을 고르되, 아직 상대에게 데미지를 입히려는 성향으로
    if (selfHpRatio <= 0.20f) score -= 40;
    else if (selfHpRatio <= 0.40f) score -= 15;
    else if (selfHpRatio >= 0.60f) score += 10;

    // 플레이어 행동 성향이 공격 위주 같으면 Counter 도 자주 섞어 쓰기 (체력이 엄청 낮은거 아닌 이상) 
    if (threat >= 40) score += 25;
    if (threat >= 60) score += 15;

    // 플레이어가 계속 공격만 한다? 일단 Parry 도 좀 섞어주면서, 조금 더 위협적인 역공격형 방어를 써도 되겠다는 판정 들어갈 수 있음 
    if (threat >= 75)
        score -= 15;

    score += GetRiskVariation(3);

    return std::max(0, score);
}

int EnemyAI::EvaluateParry(const BattleContext& context) const
{
    const int threat = EstimateDirectAttackThreat(context);
    const float selfHpRatio = GetHpRatio(context.self);
    const float targetHpRatio = GetHpRatio(context.target);

    // 기본 선호도 점수. 실패 패널티가 현재 제일 크기 때문에 guard/counter 보다는 좀 더 고심해서 쓰는 느낌
    int score = 5;

    // 상대가 슬슬 공격적으로 압박할 가능성이 있어지는 체력상태면, 그걸 역이용해서 Parry 로 조금 판 뒤집기 해보려는 느낌
    if (selfHpRatio <= 0.4) score += 20;

    // 낮은 체력에서는 피해 무효의 가치가 매우 큼 (Guard 할지 Parry 할지가 여기서 판가름 날 듯, Guard 해보고 Guard 못하면 Parry 도 하고 하는 느낌으로. ) 
    // 꽤 위험한 상황부터는 Counter 도 데미지+상태이상 받는게 부담스러워 질 수 있기 때문에 Parry 를 더 선호하게 되는 느낌
    if (selfHpRatio <= 0.3) score += 30;

    // 근데 진짜 한방 제대로 맞으면 바로 죽을 정도로 너무 체력이 낮다? -> 일단 heal 도 해야할거고 (1순위일듯), 도박을 걸 여유는 아님 
    if (selfHpRatio <= 0.15f) score -= 15;

    // 상대도 낮은 HP에서 피니시를 노리고 직접 공격할 가능성이 있다면 역이용 가능
    if (targetHpRatio <= 0.25f) score += 10;

    // 상대가 DefenseUp 상황이면 상대가 현재 조금 불리한 상황일 수도 있음, 방어형 행동 보다는 압박하는게 더 유리할 수 있기에 parry 후순위
    if (context.target.status.defenseUp) score -= 15;

    // 상대가 꽤 공격적이면 슬슬 선호도 올라감 
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

    // 최근 행동 선택 성향을 더 비중있게 두고 판단 
    if (recentAttackRatio >= 0.8f) threat += 30;
    else if (recentAttackRatio >= 0.6f) threat += 20;
    else if (recentAttackRatio >= 0.4f) threat += 10;

    // 전체적으로 어떤 느낌인지 틀 잡는 정도로 판정
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

