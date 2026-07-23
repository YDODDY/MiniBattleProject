#include <iostream>
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "BattleSystem.h"	
#include "EventBus.h"
#include "ConsoleLogSystem.h"

int main()
{
	EventBus eventBus;
	BattleSystem battleSystem(eventBus);
	ConsoleLogSystem consoleLogSystem(eventBus);

	Character player("Player", 100, 100);
	Character enemy("Enemy", 30, 30);

	return 0;
}

