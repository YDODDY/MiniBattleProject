#include "ConsoleLogSystem.h"
#include "ReactionType.h"
#include <iostream>

ConsoleLogSystem::ConsoleLogSystem(EventBus& eventBus)
{
	eventBus.Subscribe<DamagedEvent>(
		[this](const DamagedEvent& event)
		{
			OnDamaged(event);
		}
	);

	eventBus.Subscribe<HealedEvent>(
		[this](const HealedEvent& event)
		{
			OnHealed(event);
		}
	);

	eventBus.Subscribe<DeadEvent>(
		[this](const DeadEvent& event)
		{
			OnDead(event);
		}
	);

	eventBus.Subscribe<MissedEvent>(
		[this](const MissedEvent& event)
		{
			OnMissedAttack(event);
		}
	);

	eventBus.Subscribe<AppliedStatusEvent>(
		[this](const AppliedStatusEvent& event)
		{
			OnAppliedStatus(event);
		}
	);

	eventBus.Subscribe<ActionPreventedEvent>(
		[this](const ActionPreventedEvent& event)
		{
			OnPreventedAction(event);
		}
	);

	eventBus.Subscribe<DamageOverTimeEvent>(
		[this](const DamageOverTimeEvent& event)
		{
			OnDamageOverTime(event);
		}
	);

	eventBus.Subscribe<ReactionEvent>(
		[this](const ReactionEvent& event)
		{
			OnReactedAttack(event);
		});

	eventBus.Subscribe<ReactionFailedEvent>(
		[this](const ReactionFailedEvent& event)
		{
			OnReactionFailed(event);
		}
	);

	eventBus.Subscribe<StatusExpiredEvent>(
		[this](const StatusExpiredEvent& event)
		{
			OnStatusExpired(event);
		}
	);
}

void ConsoleLogSystem::OnDamaged(const DamagedEvent& event)
{
	if (event.sourceAction ==
		BattleAction::PowerAttack)
	{
		std::cout << "[PowerAttack!]\n";

		if (event.target.GetPreparedReaction() == ReactionType::Guard)
			std::cout << event.target.GetName()
			<< "'s Guarding is failed!\n";
	}

	if (event.isCritical)
	{
		std::cout << "[Critical Attack!]\n";
	}

	std::cout
		<< event.attacker.GetName()
		<< " attacked "
		<< event.target.GetName()
		<< " for "
		<< event.damage
		<< " damage.\n";

	std::cout
		<< event.target.GetName()
		<< " HP: "
		<< event.target.GetHp()
		<< '\n';
}

void ConsoleLogSystem::OnHealed(const HealedEvent& event)
{
	std::cout
		<< event.healer.GetName()
		<< " healed "
		<< event.target.GetName()
		<< " for "
		<< event.healAmount
		<< " HP.\n";

	std::cout
		<< event.target.GetName()
		<< " HP: "
		<< event.target.GetHp()
		<< '\n';
}

void ConsoleLogSystem::OnDead(const DeadEvent& event)
{
	std::cout
		<< event.deadCharacter.GetName()
		<< " is dead!\n";
}

void ConsoleLogSystem::OnMissedAttack(const MissedEvent& event)
{
	std::cout << event.attacker.GetName() << "'s ";
	if (event.sourceAction ==
		BattleAction::PowerAttack)
	{
		std::cout << "Power "; 
	}
	std::cout << "Attack to " << event.target.GetName() << " is Missed! \n";
}

void ConsoleLogSystem::OnAppliedStatus(const AppliedStatusEvent& event)
{
	switch (event.result)
	{
	case StatusApplyResult::Success:
		std::cout << event.character.GetName() << " got "
			<< ToString(event.statusEffect.type) << "! \n";
		break;

	case StatusApplyResult::Refreshed:
		std::cout << event.character.GetName() << " got  "
			<< ToString(event.statusEffect.type) << " again! \n";
		break;

	case StatusApplyResult::Refected:
		std::cout << event.character.GetName() << " didn't get "
			<< ToString(event.statusEffect.type) << ". \n";

		break;
	default:
		break;
	}

}

void ConsoleLogSystem::OnPreventedAction(const ActionPreventedEvent& event)
{
	std::cout << event.character.GetName() << " cannot act because of "
		<< ToString(event.reason) << " !\n";
}

void ConsoleLogSystem::OnDamageOverTime(const DamageOverTimeEvent& event)
{
	switch (event.statusType)
	{
	case StatusType::Poison:
		std::cout << event.target.GetName()
			<< " takes "
			<< event.damage
			<< " poison damage.\n";
		break;

	default:
		std::cout << event.target.GetName()
			<< " takes "
			<< event.damage
			<< " damage over time.\n";
		break;
	}

	std::cout << event.target.GetName()
		<< " HP: "
		<< event.target.GetHp()
		<< '\n';
}

void ConsoleLogSystem::OnReactedAttack(const ReactionEvent& event)
{
	switch (event.reaction)
	{
	case ReactionType::Guard:
		std::cout
			<< event.reactor.GetName()
			<< " guarded "
			<< event.attacker.GetName()
			<< "'s attack!\n";
		break;

	case ReactionType::Counter:
		std::cout
			<< event.reactor.GetName()
			<< " countered "
			<< event.attacker.GetName()
			<< "'s attack!\n";
		break;

	case ReactionType::Parry:
		std::cout
			<< event.reactor.GetName()
			<< " parried "
			<< event.attacker.GetName()
			<< "'s attack!\n";
		break;

	case ReactionType::None:
	default:
		break;
	}
}

void ConsoleLogSystem::OnReactionFailed(const ReactionFailedEvent& event)
{
	std::cout
		<< event.character.GetName()
		<< "'s "
		<< ToString(event.reaction)
		<< " failed!\n";
}

void ConsoleLogSystem::OnStatusExpired(const StatusExpiredEvent& event)
{
	switch (event.statusType)
	{
	case StatusType::DefenseDown:
		std::cout
			<< event.character.GetName()
			<< "'s Defense returned to normal.\n";
		break;

	case StatusType::DirectAttackLocked:
		std::cout
			<< event.character.GetName()
			<< " can use direct attacks again.\n";
		break;

	case StatusType::AttackUp:
		std::cout
			<< event.character.GetName()
			<< "'s AttackUp expired.\n";
		break;

	default:
		std::cout
			<< ToString(event.statusType)
			<< " expired on "
			<< event.character.GetName()
			<< ".\n";
		break;
	}
}
