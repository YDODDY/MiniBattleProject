#include "ConsoleLogSystem.h"
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

	eventBus.Subscribe<GuardEvent>(
		[this](const GuardEvent& event)
		{	
			OnGuardAttack(event);
		}
	);
	
	eventBus.Subscribe<MissedEvent>(
		[this](const MissedEvent& event)
		{
			OnMissedAttack(event);
		}
	);

}

void ConsoleLogSystem::OnDamaged(const DamagedEvent& event)
{
	if (event.isPowerAttack)
	{
		std::cout << "[PowerAttack!]\n";

		if (event.target.IsGuarding())
			std::cout << event.target.GetName()
			<< "'s Guarding is failed!\n";
	}

	if (event.isCritical)
	{
		std::cout << "[Critical Attack!]\n";
	}

	if (event.damageType == DamageType::Poison)
	{
		std::cout << event.target.GetName()
			<< " takes "
			<< event.damage
			<< " poison damage.\n";
	}
	else 
	{
		std::cout
			<< event.attacker.GetName()
			<< " attacked "
			<< event.target.GetName()
			<< " for "
			<< event.damage
			<< " damage.\n";
	}

	std::cout
		<< event.target.GetName()
		<< " HP: "
		<< event.target.GetHp()
		<< '\n';
}

void ConsoleLogSystem::OnHealed(const HealedEvent & event)
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

void ConsoleLogSystem::OnDead(const DeadEvent & event)
{
	std::cout
		<< event.deadCharacter.GetName()
		<< " is dead!\n";
}

void ConsoleLogSystem::OnGuardAttack(const GuardEvent& event)
{
	std::cout
		<< event.defender.GetName()
		<< "Guarded " << event.attacker.GetName() << "'s Attack! \n";
}

void ConsoleLogSystem::OnMissedAttack(const MissedEvent& event)
{
	std::cout << event.attacker.GetName() << "'s ";
	if (event.isPowerAttack)
	{
		std::cout << "Power "; 
	}
	std::cout << "Attack to " << event.target.GetName() << " is Missed! \n";
}
