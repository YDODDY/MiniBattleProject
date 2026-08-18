#include "BattleSystem.h"
#include "Character.h"
#include "Event.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "RoundAction.h"
#include <iomanip>

void BattleSystem::Attack(Character& attacker, Character& target, const AttackData& attackData)
{
	if (!attacker.CanUseAction(attackData.action))
	{
		return;
	}

	attacker.StartCooldown(attackData.action, attackData.cooldownTurns);

	const bool isHit = CheckItWasHit(attacker, target, attackData);

	if (!isHit)
	{
		eventBus.Publish(MissedEvent{attacker, target, attackData.action});
		return;
	}

	int damage = CalculateRawDamage(attacker, attackData);
	const bool isCritical =	CheckIsCritical(attacker);

	if (isCritical)
	{
		damage = ApplyCriticalDamage(damage, attacker);
	}

	damage = CalculateFinalDamage(damage, target);

	ApplyAttackResult(attacker, target, damage, attackData, isCritical);
}

void BattleSystem::Heal(Character& character, int healAmount)
{
	const int appliedHealAmount = character.Heal(healAmount);
	if (appliedHealAmount <= 0) { return; }

	eventBus.Publish(HealedEvent{ character, character, appliedHealAmount });
}

void BattleSystem::ExecuteAction(BattleAction action, Character& actor, Character& target)
{
	switch (action)
	{
	case BattleAction::Heal:
		if (!actor.CanUseAction(BattleAction::Heal)) return;

		Heal(actor, 20);
		actor.StartCooldown(BattleAction::Heal, 4);
		break;

	case BattleAction::AttackBuff:
	case BattleAction::DefenseBuff:
	{
		const StatusActionData data = MakeStatusActionData(action);
		ApplyStatusAction(actor, target, data);
		break;
	}

	default:
		const AttackData data = MakeAttackData(actor, action);
		Attack(actor, target, data);
		break;
	}	
}

void BattleSystem::ResolveInteraction(RoundContext& context, RoundResolutionPlan& plan)
{
	for (auto& interact : plan.interactions)
	{
		AttackData reactorActionData = MakeAttackData
		(*interact.reactor->actor, interact.reactor->action);

		interact.reactor->actor->StartCooldown
		(reactorActionData.action, reactorActionData.cooldownTurns);

		switch (interact.result)
		{
		case InteractionResult::Success:
			ResolveSuccessfullInteraction(interact);
			break;
			
		case InteractionResult::Failed:
			ResolveFailedInteraction(interact);
			break;

		case InteractionResult::None:
		default:
			break;
		}
	}
}

void BattleSystem::ResolveSuccessfullInteraction(InteractionPlan & interaction)
{
	switch (interaction.interaction)
	{
	case InteractionType::Guard:
		ResolveGuardSuccess(interaction);
		break;

	case InteractionType::Counter:
		ResolveCounterSuccess(interaction);
		break;

	case InteractionType::Parry:
		ResolveParrySuccess(interaction);
		break;

	case InteractionType::None:
	default:
		break;
	}
}

void BattleSystem::ResolveFailedInteraction(InteractionPlan & interaction)
{
	switch (interaction.interaction)
	{
	case InteractionType::Guard:
		ResolveGuardFailed(interaction);
		break;

	case InteractionType::Counter:
		ResolveCounterFailed(interaction);
		break;

	case InteractionType::Parry:
		ResolveParryFailed(interaction);
		break;


	case InteractionType::None:
	default:
		break;
	}
}

bool BattleSystem::CheckItWasHit(const Character& attacker, const Character& target, const AttackData& data) const
{
	float hitChance = attacker.GetAccuracy() +
		data.hitChanceModifier - target.GetEvasion();

	hitChance = std::clamp(hitChance, 0.05f, 0.95f);

	const float roll = static_cast<float>(rand()) /
		static_cast<float>(RAND_MAX);

	return roll < hitChance;
}

bool BattleSystem::CheckIsCritical(const Character& attacker) const
{
	const float roll =
		static_cast<float>(rand()) /
		static_cast<float>(RAND_MAX);

	return roll < attacker.GetCriticalChance();
}

AttackData BattleSystem::MakeAttackData(Character& character, BattleAction action)
{
	AttackData data;
	data.action = action;

	switch (action)
	{
	case BattleAction::Attack:
		data.damageMultiplier = 1.0f;
		data.hitChanceModifier = 0.0f;
		data.cooldownTurns = 0;
		break;

	case BattleAction::PowerAttack:
		data.damageMultiplier = 2.2f;
		data.hitChanceModifier = -0.2f;
		data.cooldownTurns = 5;
		break;

	case BattleAction::PoisonAttack:
		data.damageMultiplier = 0.7f;
		data.hitChanceModifier = -0.05f;
		data.cooldownTurns = 6;
		data.appliedStatus = StatusType::Poison;
		data.statusTurns = 3;
		data.statusValue = 0.2f;
		break;

	case BattleAction::StunAttack:
		data.damageMultiplier = 0.4f;
		data.hitChanceModifier = -0.15f;
		data.cooldownTurns = 6;

		data.appliedStatus = StatusType::Stun;
		data.statusTurns = 1;
		break;

	case BattleAction::Guard:
		data.cooldownTurns = 3;
		break;

	case BattleAction::Heal:
		data.cooldownTurns = 3;
		break;

	case BattleAction::Counter:
		data.cooldownTurns = 6;
		break;

	case BattleAction::Parry:
		data.cooldownTurns = 7;
		break;

	default:
		break;
	}

	return data;
}

void BattleSystem::ApplyAttackResult(Character& attacker, Character& target, int damage, const AttackData& attackData, bool isCritical)
{
    const int appliedDamage = target.ReceiveDamage(damage);
	
	eventBus.Publish(DamagedEvent{ attacker, target, 
		appliedDamage, DamageType::Direct, 
		isCritical, attackData.action });

	if (attackData.appliedStatus != StatusType::None)
	{
		ApplyAttackStatus(attacker, target, attackData);
	}

	if (target.IsDead())
	{
		eventBus.Publish(DeadEvent{ target });
	}
}

BattleAction BattleSystem::RequestAction(Character& actor, Character& target)
{
	BattleContext context = builder.Build(actor, target);

	return 	actor.ChooseAction(context);
}

int BattleSystem::CalculateRawDamage(const Character& attacker, const AttackData& data) const
{
	return static_cast<int>(attacker.GetAttack() * data.damageMultiplier);
}

int BattleSystem::CalculateFinalDamage(int rawDamage, const Character& target) const
{
	return std::max(1, rawDamage - target.GetDefense());
}

int BattleSystem::ApplyCriticalDamage(int damage, const Character& attacker) const
{
	return static_cast<int>(
		damage * attacker.GetCriticalDamageMultiplier());
}

StatusActionData BattleSystem::MakeStatusActionData(BattleAction action) const
{
	StatusActionData data;
	data.action = action;

	switch (action)
	{
	case BattleAction::AttackBuff:
		data.target = ActionTarget::Self;
		data.statusType = StatusType::AttackUp;
		data.statusTurns = 3;
		data.statusValue = 0.40f;
		data.cooldownTurns = 5;
		break;

	case BattleAction::DefenseBuff:
		data.target = ActionTarget::Self;
		data.statusType = StatusType::DefenseUp;
		data.statusTurns = 3;
		data.statusValue = 0.40f;
		data.cooldownTurns = 5;
		break;

	default:
		break;
	}

	return data;
}

void BattleSystem::ApplyStatusAction(Character& actor, Character& opponent, const StatusActionData& data)
{
	if (!actor.CanUseAction(data.action))
		return;

	Character& receiver = data.target == ActionTarget::Self ? actor : opponent;
	actor.StartCooldown(data.action, data.cooldownTurns);

	StatusEffect effect;
	effect.type = data.statusType;
	effect.remainingTurns = data.statusTurns;
	effect.value = data.statusValue;

	const StatusApplyResult result = receiver.ApplyStatus(effect);

	eventBus.Publish(AppliedStatusEvent{receiver, effect, result});

}

void BattleSystem::ApplyStatusEffect(Character& target, StatusType type, int turns, float value)
{
	StatusEffect effect;
	effect.type = type;
	effect.remainingTurns = turns;
	effect.value = value;

	const StatusApplyResult result = target.ApplyStatus(effect);

	eventBus.Publish( AppliedStatusEvent{target,effect,	result});
}

bool BattleSystem::IsDirectAttack(BattleAction action) const
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

bool BattleSystem::IsInteractionAction(BattleAction action) const
{
	return action == BattleAction::Guard
		|| action == BattleAction::Counter
		|| action == BattleAction::Parry;
}

bool BattleSystem::WasActionResolvedByInteraction(const RoundAction& roundAction, const RoundResolutionPlan& plan) const
{
	for (const InteractionPlan& interaction : plan.interactions)
	{
		// 성공한 Interaction은
		// attacker + reactor 행동을 둘 다 처리한 것으로 봄
		if (interaction.result == InteractionResult::Success)
		{
			if (interaction.attacker == &roundAction ||
				interaction.reactor == &roundAction)
			{
				return true;
			}
		}
		// 실패한 Counter / Parry는 reactor 행동 자체는
		// 실패 처리된 것으로 봄
		if (interaction.result == InteractionResult::Failed)
		{
			if (interaction.reactor == &roundAction)
			{
				return true;
			}
		}
	}

	return false;
}

void BattleSystem::ExecuteCounterAttack(Character& counterAttacker, Character& target, float damageMultiplier, BattleAction sourceAction)
{
	int damage = static_cast<int>(counterAttacker.GetAttack() * damageMultiplier);
	damage = CalculateFinalDamage(damage, target);
	const int appliedDamage = target.ReceiveDamage(damage);

	eventBus.Publish(DamagedEvent{counterAttacker, target, 
		appliedDamage, DamageType::Counter, false, sourceAction});
	
	if (target.IsDead())
	{
		eventBus.Publish(DeadEvent{target});
	}

}

void BattleSystem::ApplyAttackStatus(Character& attacker, Character& target, const AttackData& attackData)
{
	if (attackData.appliedStatus == StatusType::None)
		return;

	StatusEffect effect;
	effect.type = attackData.appliedStatus;
	effect.remainingTurns = attackData.statusTurns;

	effect.value = std::max( 1.0f,attacker.GetBaseAttack()
		* attackData.statusValue);

	const StatusApplyResult result = target.ApplyStatus(effect);

	eventBus.Publish( AppliedStatusEvent{ target,effect,result });
}

void BattleSystem::RevealActions(const RoundContext& context)
{
	std::cout
		<< "------------- ACTION REVEAL ------------\n\n";

	std::cout
		<< std::left << std::setw(12)
		<< context.first.actor->GetName()
		<< " : "
		<< ToString(context.first.action)
		<< '\n';

	std::cout
		<< std::left << std::setw(12)
		<< context.second.actor->GetName()
		<< " : "
		<< ToString(context.second.action)
		<< '\n';

	std::cout
		<< "\n----------------------------------------\n";
}

ActionPhaseStartResult BattleSystem::StartActionPhase(Character& character)
{
	ActionPhaseStartResult result =
		character.ProcessActionPhaseStart();

	if (!result.canAct)
	{
		eventBus.Publish(
			ActionPreventedEvent{
				character,
				result.preventedBy
			});
	}

	if (result.preventedBy == StatusType::Stun)
	{
		eventBus.Publish(
			StatusExpiredEvent{
				character, StatusType::Stun});
	}

	return result;

}

void BattleSystem::EndActionPhase(Character& character)
{
	ActionPhaseEndResult result =
		character.ProcessActionPhaseEnd();

	if (result.damage > 0)
	{
		eventBus.Publish(
			DamageOverTimeEvent{
				character,
				result.damage,
				result.damageSource
			});
	}

	for (StatusType expired : result.expiredStatuses)
	{
		eventBus.Publish(
			StatusExpiredEvent{
				character,
				expired
			});
	}

	if (character.IsDead())
	{
		eventBus.Publish(DeadEvent{ character });
	}
}

InteractionType BattleSystem::GetInteractionType(BattleAction reactorAction) const
{
	switch (reactorAction)
	{
	case BattleAction::Guard:
		return InteractionType::Guard;

	case BattleAction::Counter:
		return InteractionType::Counter;

	case BattleAction::Parry:
		return InteractionType::Parry;

	default:
		return InteractionType::None;
	}
}

RoundResolutionPlan BattleSystem::ChooseActionComplete(RoundContext& context)
{
	RoundResolutionPlan plan;

	AnalyzeInteraction(context.first,context.second,plan);
	AnalyzeInteraction(context.second,context.first,plan);

	if (!plan.interactions.empty())
		plan.hasInteraction = true;

	return plan;
}

void BattleSystem::AnalyzeInteraction(RoundAction& reactor, RoundAction& opponent, RoundResolutionPlan& plan)
{
	switch (reactor.action) 
	{
	case BattleAction::Guard:
		if (IsDirectAttack(opponent.action))
		{
			if (opponent.action == BattleAction::PowerAttack)
			{
				plan.interactions.push_back({
				InteractionType::Guard,
				InteractionResult::Failed,
				&opponent,
				&reactor
					});
			}
			else
			{
				plan.interactions.push_back({
				InteractionType::Guard,
				InteractionResult::Success,
				&opponent,
				&reactor
				});
			}
		}
		break;

	case BattleAction::Counter:
		        plan.interactions.push_back({
            InteractionType::Counter,
            IsDirectAttack(opponent.action)
                ? InteractionResult::Success
                : InteractionResult::Failed,
            IsDirectAttack(opponent.action)
                ? &opponent
                : nullptr,
				& reactor,
			    });

        break;

	case BattleAction::Parry:
				plan.interactions.push_back({
			InteractionType::Parry,
			IsDirectAttack(opponent.action)
				? InteractionResult::Success
				: InteractionResult::Failed,
			IsDirectAttack(opponent.action)
				? &opponent
				: nullptr,
				& reactor,
					});

		break;

	default:
		break;
	}
}

void BattleSystem::ResolveGuardSuccess(InteractionPlan& interaction)
{
	Character& attacker = *interaction.attacker->actor;
	Character& guarder = *interaction.reactor->actor;

	const BattleAction attackAction = interaction.attacker->action;
	AttackData attackData = MakeAttackData(attacker, attackAction);

	const bool isHit = CheckItWasHit(attacker, guarder, attackData);

	if (!isHit)
	{
		eventBus.Publish(MissedEvent{ attacker, guarder, attackAction});
		return;
	}

	ResolveGuardInteraction(attacker, guarder, attackData);
}

void BattleSystem::ResolveCounterSuccess(InteractionPlan & interaction)
{
	Character& attacker = *interaction.attacker->actor;
	Character& counter = *interaction.reactor->actor;

	const BattleAction attackAction = interaction.attacker->action;
	AttackData attackData = MakeAttackData(attacker, attackAction);

	const bool isHit = CheckItWasHit(attacker, counter, attackData);

	if (!isHit)
	{
		eventBus.Publish(MissedEvent{ attacker, counter, attackAction });
	}

	ResolveCounterInteraction(attacker, counter, attackData, isHit);
}

void BattleSystem::ResolveParrySuccess(InteractionPlan & interaction)
{
	Character& attacker = *interaction.attacker->actor;
	Character& counter = *interaction.reactor->actor;

	const BattleAction attackAction = interaction.attacker->action;
	AttackData attackData = MakeAttackData(attacker, attackAction);

	const bool isHit = CheckItWasHit(attacker, counter, attackData);

	if (!isHit)
	{
		eventBus.Publish(MissedEvent{ attacker, counter, attackAction });
	}

	ResolveParryInteraction(attacker, counter, attackData);
}

void BattleSystem::ResolveGuardFailed(InteractionPlan& interaction)
{
	Character& attacker = *interaction.attacker->actor;
	Character& guarder = *interaction.reactor->actor;
	const BattleAction attackAction = interaction.attacker->action;

	if (attackAction == BattleAction::PowerAttack)
	{
		eventBus.Publish(GuardFailedEvent{attacker, guarder});

		AttackData attackData = MakeAttackData(attacker, attackAction);
		int damage = CalculateRawDamage(attacker, attackData);
		const bool isCritical = CheckIsCritical(attacker);

		if (isCritical)
		{
			damage = ApplyCriticalDamage(damage, attacker);
		}

		damage = CalculateFinalDamage(damage, guarder);

		ApplyAttackResult(attacker, guarder, damage, attackData, isCritical);

		return;
	}
}

void BattleSystem::ResolveCounterFailed(InteractionPlan & interaction)
{
	Character& counter = *interaction.reactor->actor;
	ApplyStatusEffect(counter, StatusType::DefenseDown, 3, 0.5f);
}

void BattleSystem::ResolveParryFailed(InteractionPlan & interaction)
{
	Character& counter = *interaction.reactor->actor;
	ApplyStatusEffect(counter, StatusType::DirectAttackLocked, 2, 0.0f);
}

void BattleSystem::ResolveGuardInteraction(Character& attacker, Character& guarder, const AttackData& attackData)
{
	eventBus.Publish(InteractEvent{ attacker, guarder, InteractType::Guard });

	if (attackData.appliedStatus != StatusType::None)
	{
		ApplyAttackStatus(attacker, guarder, attackData);
	}
}

void BattleSystem::ResolveCounterInteraction(Character& attacker, Character& counter, const AttackData& attackData, bool isHit)
{
	if (isHit)
	{
		int damage = CalculateRawDamage(attacker, attackData);
		const bool isCritical = CheckIsCritical(attacker);
		if (isCritical)
		{
			damage = ApplyCriticalDamage(damage, attacker);
		}

		CalculateFinalDamage(damage, counter);
		damage = static_cast<int>(damage * 0.5f);
		const int appliedDamage = counter.ReceiveDamage(damage);

		eventBus.Publish(DamagedEvent{ attacker, counter,
			appliedDamage, DamageType::Direct, isCritical, attackData.action });


		if (counter.IsDead())
		{
			eventBus.Publish(DeadEvent{ counter });
			return;
		}

		ApplyAttackStatus(attacker, counter, attackData);
	}
	

	if (!counter.IsDead())
	{
		ExecuteCounterAttack(counter, attacker, 0.8f, BattleAction::Counter);
	}
}

void BattleSystem::ResolveParryInteraction(Character & attacker, Character & counter, const AttackData & attackData)
{
	ExecuteCounterAttack(counter, attacker, 1.2f, BattleAction::Parry);

	StatusEffect effect;
	effect.type = StatusType::AttackUp;
	effect.remainingTurns = 2;
	effect.value = 0.4f;

	const StatusApplyResult result = counter.ApplyStatus(effect);
	eventBus.Publish(AppliedStatusEvent{ counter, effect, result });
}

AIMemoryUpdateData BattleSystem::MakeAIMemoryUpdateData(BattleAction playerAction, BattleAction enemyAction, const RoundResolutionPlan& plan, Character& enemy)
{
	AIMemoryUpdateData data;

	data.playerAction = playerAction;
	data.enemyAction = enemyAction;

	for (const auto& interaction : plan.interactions)
	{
		InteractionMemoryData memoryInteraction;

		memoryInteraction.type = interaction.interaction;
		memoryInteraction.result = interaction.result;

		memoryInteraction.enemyWasReactor =
			interaction.reactor != nullptr &&
			interaction.reactor->actor == &enemy;

		data.interactions.push_back(memoryInteraction);
	}

	return data;
}

BattleAction BattleSystem::GetActionByActor(const RoundContext& context, const Character& actor)
{
	if (context.first.actor == &actor)
		return context.first.action;

	if (context.second.actor == &actor)
		return context.second.action;

	return BattleAction::None;
}

bool BattleSystem::CanRoundActionAct(const RoundAction* action, const RoundContext& context, const ActionPhaseStartResult& firstStart, const ActionPhaseStartResult& secondStart)
{
	if (action == nullptr)
		return false;

	if (action == &context.first)
		return firstStart.canAct;

	if (action == &context.second)
		return secondStart.canAct;

	return false;
}

void BattleSystem::ValidateInteractionsForActionPhase(RoundContext& context, RoundResolutionPlan& plan, const ActionPhaseStartResult& firstStart, const ActionPhaseStartResult& secondStart)
{
	for (InteractionPlan& interaction : plan.interactions)
	{
		const bool reactorCanAct =
			CanRoundActionAct(
				interaction.reactor,
				context,
				firstStart,
				secondStart);

		if (!reactorCanAct)
		{
			interaction.result = InteractionResult::None;
			continue;
		}

		if (interaction.attacker != nullptr)
		{
			const bool attackerCanAct =
				CanRoundActionAct(
					interaction.attacker,
					context,
					firstStart,
					secondStart);

			if (!attackerCanAct)
			{
				interaction.result = InteractionResult::Failed;
				interaction.attacker = nullptr;
			}
		}
	}
}


std::string BattleSystem::ToString(const BattleAction& action)
{
	switch (action)
	{
	case::BattleAction::Attack:
		return "Attack";
	case::BattleAction::PowerAttack:
		return "PowerAttack";
	case::BattleAction::PoisonAttack:
		return "PoisonAttack";
	case::BattleAction::StunAttack:
		return "StunAttack";
	case::BattleAction::Heal:
		return "Heal";
	case::BattleAction::Guard:
		return "Guard";
	case::BattleAction::AttackBuff:
		return "AttackBuff";
	case::BattleAction::DefenseBuff:
		return "DefenseBuff";
	case::BattleAction::Counter:
		return "Counter";
	case::BattleAction::Parry:
		return "Parry";


	default:
		return "Nothing.";
	}
}
