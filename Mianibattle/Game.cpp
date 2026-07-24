#include "Game.h"
#include "Character.h"
#include <iostream>

Game::Game() 
	: battleSystem(eventBus), consoleLogSystem(eventBus)
{
	
}

void Game::Run()
{
	// player, enemy 생성
	Character player("Player", 100, 100);
	Character enemy("Enemy", 50, 50);

	Character* winner = nullptr;
	Character* loser = nullptr;

	// 전투 시작 출력
	std::cout << "Battle Started !\n";

	// 둘 다 살아있는 동안 반복
	while (!player.IsDead() && !enemy.IsDead())
	{
		// player 행동
		if (CheckBattleEnd(player, enemy, winner, loser))
			break;

		// enemy 행동
		if (CheckBattleEnd(player, enemy, winner, loser))
			break;
	}
	
	// 승패 확인 및 출력
	if (winner != nullptr && loser != nullptr)
	{
		std::cout << winner->GetName() << " Win!\n";
		std::cout << loser->GetName() << " Lose!\n";
	}

}

bool Game::CheckBattleEnd(Character& _player, Character& _enemy, Character*& _winner, Character*& _loser)
{
	if (_player.IsDead())
	{
		_winner = &_enemy;
		_loser = &_player;

		std::cout << "Battle End\n";
		return true;
	}

	if (_enemy.IsDead())
	{
		_winner = &_player;
		_loser = &_enemy;

		std::cout << "Battle End\n";
		return true;
	}

	return false;
}
