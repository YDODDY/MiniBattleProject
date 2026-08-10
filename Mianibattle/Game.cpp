#include "Game.h"
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include <iostream>
#include "BattleAction.h"

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

	int playerFirst = 0;
	int enemyFirst = 0;

	for (int i = 0; i < 100; ++i)
	{
		TurnOrder order = SetTurnOrder(player, enemy);

		if (order.firstActor == &player)
			++playerFirst;
		else
			++enemyFirst;
	}

	std::cout
		<< "Player First : " << playerFirst << '\n'
		<< "Enemy First  : " << enemyFirst << '\n';
	/*
	StartBattle(player, enemy);
	RunBattleLoop(player, enemy);
	ShowBattleResult(player, enemy);
	*/
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

	// 둘 다 살아있는 동안 반복
	while (true)
	{
		TurnStartResult playerTurn = battleSystem.StartTurn(player);

		if (CheckBattleEnd(player, enemy))
			break;

		if (playerTurn.canAct)
		{
			BattleAction playerAction = battleSystem.RequestAction(player, enemy);

			battleSystem.ExecuteAction(playerAction, player, enemy);

			actualEnemy.RememberPlayerAction(playerAction);
		}

		if (CheckBattleEnd(player, enemy))
			break;

		TurnStartResult enemyTurn = battleSystem.StartTurn(enemy);

		if (CheckBattleEnd(player, enemy))
			break;

		if (enemyTurn.canAct)
		{
			BattleAction enemyAction = battleSystem.RequestAction(enemy,player);
			battleSystem.ExecuteAction(enemyAction, enemy, player);
		}

		if (CheckBattleEnd(player, enemy))
			break;
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
