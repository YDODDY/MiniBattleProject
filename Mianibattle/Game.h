#pragma once

#include "BattleSystem.h"
#include "EventBus.h"
#include "BattleResult.h"
#include "ConsoleLogSystem.h"

class Game
{
public:
	Game();
	void Run();

private:

	EventBus eventBus; // 먼저 생성
	BattleSystem battleSystem; // eventbus 참조 
	ConsoleLogSystem consoleLogSystem; // eventbus 참조
	BattleResult battleResult;

	bool CheckBattleEnd(Character& player,	Character& enemy);

	void StartBattle(Character& player, Character& enemy);
	void RunBattleLoop(Character& player, Character& enemy);
	void ShowBattleResult(Character& player, Character& enemy);

};