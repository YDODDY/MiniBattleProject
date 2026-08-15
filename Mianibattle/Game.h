#pragma once

#include "BattleSystem.h"
#include "EventBus.h"
#include "BattleResult.h"
#include "ConsoleLogSystem.h"
#include "RoundAction.h"

struct TurnOrder
{
	Character* firstActor = nullptr;
	Character* secondActor = nullptr;
};

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

	TurnOrder order;
	TurnOrder SetTurnOrder(Character& player, Character& enemy);
	void PrintOrder(TurnOrder& order);

	RoundContext CreateRoundContext(const TurnOrder& order);

	bool IsAnyCharacterDead(const RoundContext& context);
	bool ResolveBasicRound(RoundContext& context);
	bool ResolveInteractionRound(RoundContext& context, RoundResolutionPlan& plan);
};