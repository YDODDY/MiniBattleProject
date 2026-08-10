#include <iostream>
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "BattleSystem.h"	
#include "EventBus.h"
#include "ConsoleLogSystem.h"
#include "Game.h"
#include <cstdlib>
#include <ctime>

int main()
{
	srand(static_cast<unsigned int>(time(nullptr)));

	Game game;
	game.Run();

	return 0;
}

