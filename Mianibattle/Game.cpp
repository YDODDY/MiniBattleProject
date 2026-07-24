#include "Game.h"
#include "Character.h"
#include <iostream>

Game::Game() 
	: battleSystem(eventBus), consoleLogSystem(eventBus)
{
	
}

void Game::Run()
{
	Character player("Player", 100, 100);
	Character enemy("Enemy", 50, 50);

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
	// 둘 다 살아있는 동안 반복
	while (!player.IsDead() && !enemy.IsDead())
	{
		// player 행동
		if (CheckBattleEnd(player, enemy))
			break;

		// enemy 행동
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
