#include "Game.h"
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include <iostream>
#include "BattleAction.h"
#include "ConsoleLogSystem.h"

Game::Game() 
	: battleSystem(eventBus), consoleLogSystem(eventBus)
{
	
}

void Game::Run()
{
	CharacterStats defaultStats;
	defaultStats.maxHp = 100;
	defaultStats.attack = 10;
	defaultStats.defense = 3;
	defaultStats.accuracy = 0.90f;
	defaultStats.evasion = 0.05f;
	defaultStats.criticalChance = 0.10f;
	defaultStats.criticalDamageMultiplier = 1.50f;

	Player player("Player", defaultStats);
	Enemy enemy("Enemy", defaultStats);

	order = SetTurnOrder(player, enemy);
	PrintOrder(order);
	
	StartBattle(player, enemy);
	RunBattleLoop(player, enemy);
	ShowBattleResult(player, enemy);
	
}

bool Game::CheckBattleEnd(Character& player, Character& enemy)
{
	if (player.IsDead())
	{
		battleResult = BattleResult::EnemyWin;
		return true;
	}

	if (enemy.IsDead())
	{
		battleResult = BattleResult::PlayerWin;
		return true;
	}
	return false;
}

void Game::StartBattle(Character& player, Character& enemy)
{
	battleResult = BattleResult::None;

	// 전투 시작 출력
	std::cout << "Battle Started !\n";
	std::cout << player.GetName() << " vs "
		<< enemy.GetName() << '\n';	
}

void Game::RunBattleLoop(Character& player, Character& enemy)
{
	Enemy& actualEnemy = static_cast<Enemy&>(enemy);
	int roundNum = 1;

	while (!player.IsDead() && !enemy.IsDead())
	{
		consoleLogSystem.PrintRoundHeader(roundNum);

		RoundContext context = CreateRoundContext(order);
		context.roundNumber = roundNum;

		consoleLogSystem.PrintRoundStatus(player, enemy);

		context.first.action = battleSystem.RequestAction
		(*context.first.actor, *context.first.target);
		
		context.second.action = battleSystem.RequestAction
		(*context.second.actor, *context.second.target);

		RoundResolutionPlan plan = battleSystem.ChooseActionComplete(context);

		battleSystem.RevealActions(context);

		if (!plan.hasInteraction)
		{
			if (ResolveBasicRound(context))
			{
				CheckBattleEnd(player, enemy);
				break;
			}
		}
		else
		{
			if (ResolveInteractionRound(context, plan))
			{
				CheckBattleEnd(player, enemy);
				break;
			}
		}

		BattleAction playerAction =
			battleSystem.GetActionByActor(context, player);

		BattleAction enemyAction =
			battleSystem.GetActionByActor(context, enemy);


		consoleLogSystem.PrintRoundEndStatus(player, enemy);

		consoleLogSystem.PrintHpBar(player);
		consoleLogSystem.PrintStatusSummary(player);

		std::cout << '\n';

		consoleLogSystem.PrintHpBar(enemy);
		consoleLogSystem.PrintStatusSummary(enemy);


		AIMemoryUpdateData data = battleSystem.MakeAIMemoryUpdateData
		(playerAction, enemyAction, plan, actualEnemy);
		actualEnemy.RememberRound(data);

		++roundNum;
	}
}

void Game::ShowBattleResult(Character& player, Character& enemy)
{
	std::cout << "Battle End!\n\n";

	switch (battleResult)
	{
	case BattleResult::None:
		std::cout << "Battle result is not decided.\n";
		break;

	case BattleResult::PlayerWin:
		std::cout << player.GetName() << " Win!\n";
		std::cout << enemy.GetName() << " Lose!\n";
		break;

	case BattleResult::EnemyWin:
		std::cout << enemy.GetName() << " Win!\n";
		std::cout << player.GetName() << " Lose!\n";
		break;

	default:
		break;
	}
}

TurnOrder Game::SetTurnOrder(Character& player, Character& enemy)
{
	TurnOrder order;

	if (rand() % 2 == 0)
	{
		order.firstActor = &player;
		order.secondActor = &enemy;
	}
	else
	{
		order.firstActor = &enemy;
		order.secondActor = &player;
	}

	return order;
}

void Game::PrintOrder(TurnOrder& order)
{
	std::cout << "=== TURN ORDER ===\n"
		<< "First : " << order.firstActor->GetName() << "\n"
		<< "Second : " << order.secondActor->GetName() << "\n";
}

RoundContext Game::CreateRoundContext(const TurnOrder& order)
{
	RoundContext context;

	context.first.actor = order.firstActor;
	context.first.target = order.secondActor;

	context.second.actor = order.secondActor;
	context.second.target = order.firstActor;

	return context;
}

bool Game::IsAnyCharacterDead(const RoundContext& context)
{
	return context.first.actor->IsDead()
		|| context.second.actor->IsDead();
}

bool Game::ResolveBasicRound(RoundContext& context)
{
	// first
	ActionPhaseStartResult firstStart =
		battleSystem.StartActionPhase(*context.first.actor);

	if (firstStart.canAct)
	{
		battleSystem.ExecuteAction(
			context.first.action,
			*context.first.actor,
			*context.first.target);
	}

	battleSystem.EndActionPhase(*context.first.actor);

	if (IsAnyCharacterDead(context))
		return true;

	// second
	ActionPhaseStartResult secondStart =
		battleSystem.StartActionPhase(*context.second.actor);

	if (secondStart.canAct)
	{
		battleSystem.ExecuteAction(
			context.second.action,
			*context.second.actor,
			*context.second.target);
	}

	battleSystem.EndActionPhase(*context.second.actor);


	return IsAnyCharacterDead(context);
}

bool Game::ResolveInteractionRound(RoundContext& context, RoundResolutionPlan& plan)
{
	ActionPhaseStartResult firstStart =
		battleSystem.StartActionPhase(*context.first.actor);

	ActionPhaseStartResult secondStart =
		battleSystem.StartActionPhase(*context.second.actor);

	battleSystem.ValidateInteractionsForActionPhase(context, plan, firstStart, secondStart);

	battleSystem.ResolveInteraction(context, plan);

	if (firstStart.canAct &&
		!battleSystem.WasActionResolvedByInteraction(
			context.first, plan))
	{
		battleSystem.ExecuteAction(
			context.first.action,
			*context.first.actor,
			*context.first.target);
	}

	if (secondStart.canAct &&
		!battleSystem.WasActionResolvedByInteraction(
			context.second, plan))
	{
		battleSystem.ExecuteAction(
			context.second.action,
			*context.second.actor,
			*context.second.target);
	}


	battleSystem.EndActionPhase(*context.first.actor);
	battleSystem.EndActionPhase(*context.second.actor);

	return IsAnyCharacterDead(context);
}


