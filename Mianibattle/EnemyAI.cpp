#include "EnemyAI.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "InteractionType.h"

EnemyAI::EnemyAI()
{
    ResetMemory();
}

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

    if (memory.totalPlayerActions <= 1)
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

        if (openingCandidates.empty())
        {
            return scores.front().action;
        }

        const int randomIndex = rand() % openingCandidates.size();

        const BattleAction openingAction = openingCandidates[randomIndex].action;

        PrintDecisionLog(scores, context, openingAction);
        return openingAction;
    }

    PrintDecisionLog(scores, context, selectedAction);
    return selectedAction;
}

void EnemyAI::ResetMemory()
{
    memory = AIMemory{};

    memory.roundsSincePlayerUsedAction.insert({BattleAction::Attack, -1});
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::PowerAttack, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::PoisonAttack, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::StunAttack, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::Heal, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::Guard, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::AttackBuff, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::DefenseBuff, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::Counter, -1 });
    memory.roundsSincePlayerUsedAction.insert({ BattleAction::Parry, -1 });

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

    if (PlayerLikelyCanUse(BattleAction::PowerAttack))
    {
        // PowerAttack은 Guard를 뚫으니까
        score -= 15;
    }

    if (!PlayerLikelyCanUse(BattleAction::PowerAttack))
    {
        score += 10;
    }

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

    int score = 15;

    const float directAttackThreat =
        EstimateDirectAttackThreat(context);

    score += static_cast<int>(
        directAttackThreat * 40.0f);

    return score;
}

int EnemyAI::EvaluateParry(const BattleContext& context) const
{
    int score = 5;

    const float directAttackThreat =
        EstimateDirectAttackThreat(context);

    score += static_cast<int>(
        directAttackThreat * 45.0f);

    return score;
}

int EnemyAI::GetRoundsSincePlayerUsed(BattleAction action) const
{
    auto it =
        memory.roundsSincePlayerUsedAction.find(action);

    if (it == memory.roundsSincePlayerUsedAction.end())
        return -1;

    return it->second;
}

bool EnemyAI::PlayerLikelyCanUse(BattleAction action) const
{
    const int roundsSinceUsed =
        GetRoundsSincePlayerUsed(action);

    if (roundsSinceUsed == -1)
        return true;

    const int cooldown = GetActionBaseCoolDown(action);

    return roundsSinceUsed >= cooldown;
}

int EnemyAI::EstimateDirectAttackThreat(const BattleContext& context) const
{
    // 아직 정보 없음
    if (memory.totalPlayerActions == 0)
        return 0.5f;
    
    const float recentRatio = GetRecentDirectAttackRatio();
    const float overallRatio = GetOverallDirectAttackRatio(); 

    float threat = recentRatio * 0.6f + overallRatio * 0.4f;

    const float confidence =
        std::min(memory.totalPlayerActions / 5.0f, 1.0f);

    threat = 0.5f * (1.0f - confidence) + threat * confidence;

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

void EnemyAI::PrintDecisionLog(const std::vector<ActionScore>& scores, const BattleContext& context, BattleAction selectedAction) const
{
    const float recentAttackRatio =
        GetRecentDirectAttackRatio();

    const float overallAttackRatio =
        GetOverallDirectAttackRatio();

    const float directAttackThreat =
        EstimateDirectAttackThreat(context);

    std::cout
        << "\n"
        << "========================================\n"
        << "              AI DECISION\n"
        << "========================================\n";

    std::cout
        << std::left << std::setw(18)
        << "Selected" << " : "
        << ToString(selectedAction) << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Attack Threat" << " : "
        << std::fixed << std::setprecision(2)
        << directAttackThreat << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Recent Atk Ratio" << " : "
        << recentAttackRatio << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Overall Atk Ratio" << " : "
        << overallAttackRatio << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Last Player Action" << " : "
        << ToString(memory.lastPlayerAction) << '\n';

   // PrintActionScores(scores, selectedAction);
   // PrintMemoryDebug(context);

    std::cout
        << "========================================\n";
}

void EnemyAI::PrintActionScores(const std::vector<ActionScore>& scores, BattleAction selectedAction) const
{
    std::cout << "\n---------- ACTION SCORES ----------\n";

    for (const ActionScore& actionScore : scores)
    {
        std::cout
            << std::left
            << std::setw(16)
            << ToString(actionScore.action)
            << " : "
            << std::right
            << std::setw(3)
            << actionScore.score;

        if (actionScore.action == selectedAction)
        {
            std::cout << "  < SELECTED";
        }

        std::cout << '\n';
    }
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

    int count = 0;

    for (BattleAction action : memory.recentPlayerActions)
    {
        if (IsDirectAttackAction(action))
            ++count;
    }

    return static_cast<float>(count)
        / memory.recentPlayerActions.size();
}

float EnemyAI::GetOverallDirectAttackRatio() const
{
    if (memory.totalPlayerActions == 0)
        return 0.0f;

    return static_cast<float>(memory.playerDirectAttackCount)
        / memory.totalPlayerActions;
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
    std::cout << "\n---------- AI MEMORY ----------\n";

    const float recentAttackRatio =
        GetRecentDirectAttackRatio();

    const float overallAttackRatio =
        GetOverallDirectAttackRatio();

    std::cout
        << std::left << std::setw(18)
        << "Player Actions" << " : "
        << memory.totalPlayerActions << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Direct Attacks" << " : "
        << memory.playerDirectAttackCount << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Recent Atk Ratio" << " : "
        << std::fixed << std::setprecision(2)
        << recentAttackRatio << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Overall Atk Ratio" << " : "
        << std::fixed << std::setprecision(2)
        << overallAttackRatio << '\n';

    std::cout
        << std::left << std::setw(18)
        << "Last Action" << " : "
        << ToString(memory.lastPlayerAction) << '\n';


    // Recent Actions
    std::cout << "\n[ Recent Actions ]\n";

    if (memory.recentPlayerActions.empty())
    {
        std::cout << "(empty)\n";
    }
    else
    {
        bool first = true;

        for (BattleAction action : memory.recentPlayerActions)
        {
            if (!first)
            {
                std::cout << " > ";
            }

            std::cout << ToString(action);
            first = false;
        }

        std::cout << '\n';
    }


    // Action History
    std::cout << "\n[ Rounds Since Player Used Action ]\n";

    for (const auto& [action, rounds]
        : memory.roundsSincePlayerUsedAction)
    {
        std::cout
            << std::left
            << std::setw(16)
            << ToString(action)
            << " : ";

        if (rounds == -1)
        {
            std::cout << "NEVER";
        }
        else
        {
            std::cout << rounds;
        }

        std::cout << '\n';
    }


    // Player Action Counts
    std::cout << "\n[ Player Action Counts ]\n";

    std::cout
        << std::left << std::setw(16)
        << "PowerAttack" << " : "
        << memory.playerPowerAttackCount << '\n';

    std::cout
        << std::left << std::setw(16)
        << "Guard" << " : "
        << memory.playerGuardCount << '\n';

    std::cout
        << std::left << std::setw(16)
        << "Counter" << " : "
        << memory.playerCounterCount << '\n';

    std::cout
        << std::left << std::setw(16)
        << "Parry" << " : "
        << memory.playerParryCount << '\n';


    // Enemy Interaction Records
    std::cout << "\n[ Enemy Interaction Record ]\n";

    std::cout
        << "Counter : "
        << memory.enemyCounterSuccessCount
        << " Success / "
        << memory.enemyCounterFailedCount
        << " Failed\n";

    std::cout
        << "Parry   : "
        << memory.enemyParrySuccessCount
        << " Success / "
        << memory.enemyParryFailedCount
        << " Failed\n";

    std::cout
        << "Guard Broken : "
        << memory.enemyGuardBrokenCount
        << '\n';
}

void EnemyAI::UpdateRecentMemory(const AIMemoryUpdateData& data)
{
    memory.lastPlayerAction = data.playerAction;

    memory.recentPlayerActions.push_back(data.playerAction);

    if (memory.recentPlayerActions.size() > 5)
    {
        memory.recentPlayerActions.pop_front();
    }
}

void EnemyAI::UpdateAggregateMemory(const AIMemoryUpdateData & data)
{
    memory.totalPlayerActions++;

    if (IsDirectAttackAction(data.playerAction))
        memory.playerDirectAttackCount++;

    switch (data.playerAction)
    {
    case BattleAction::PowerAttack:
        memory.playerPowerAttackCount++;
        break;

    case BattleAction::Guard:
        memory.playerGuardCount++;
        break;

    case BattleAction::Counter:
        memory.playerCounterCount++;
        break;

    case BattleAction::Parry:
        memory.playerParryCount++;
        break;

    default:
        break;
    }

    for (auto& [action, rounds] :
        memory.roundsSincePlayerUsedAction)
    {
        if (rounds >= 0)
            ++rounds;
    }

    memory.roundsSincePlayerUsedAction[data.playerAction] = 0;
}

void EnemyAI::UpdateTacticalMemory(const AIMemoryUpdateData & data)
{
    for (const auto& it : data.interactions)
    {
        if (!it.enemyWasReactor)
            continue;

        switch (it.type)
        {
        case InteractionType::Guard:
            if (it.result == InteractionResult::Failed)
                memory.enemyGuardBrokenCount++;
            break;

        case InteractionType::Counter:
            if (it.result == InteractionResult::Success)
                memory.enemyCounterSuccessCount++;
            else if (it.result == InteractionResult::Failed)
                memory.enemyCounterFailedCount++;
            break;

        case InteractionType::Parry:
            if (it.result == InteractionResult::Success)
                memory.enemyParrySuccessCount++;
            else if (it.result == InteractionResult::Failed)
                memory.enemyParryFailedCount++;
            break;

        default:
            break;
        }
    }
}

int EnemyAI::GetActionBaseCoolDown(BattleAction action) const
{
    switch (action)
    {
    case BattleAction::PowerAttack:
        return 4;

    case BattleAction::PoisonAttack:
        return 5;

    case BattleAction::StunAttack:
        return 5;

    case BattleAction::Heal:
        return 4;

    default:
        return 0;
    }
}

void EnemyAI::RememberRound(const AIMemoryUpdateData& data)
{
    UpdateRecentMemory(data);
    UpdateAggregateMemory(data);
    UpdateTacticalMemory(data);
}

