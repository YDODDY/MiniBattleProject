#include "Player.h"
#include <iostream>
#include <limits>


BattleAction Player::ChooseAction(const BattleContext& context)
{
	while (true)
	{
		int choice = 0;

		std::cout << "1. Attack\n";
		std::cout << "2. PowerAttack\n";
		std::cout << "3. PoisonAttack\n";
		std::cout << "4. Heal\n";
		std::cout << "5. Guard\n";
		std::cout << "6. StunAttack\n";
		std::cout << "7. AttackBuff\n";
		std::cout << "8. DefenseBuff\n";
		std::cout << "9. Counter\n";
		std::cout << "0. Parry\n";
		std::cout << ">> ";

		if (!(std::cin >> choice))
		{
			std::cout << "Only Mentioned Command is available. \n";

			// cin fail 상태 해제
			std::cin.clear();

			// 입력줄 남아 있는 잘못된 내용 제거
			std::cin.ignore(
				std::numeric_limits<std::streamsize>::max(),
				'\n');

			continue;
		}

		// 숫자 뒤 남은 입력도 제거
		std::cin.ignore(
			std::numeric_limits<std::streamsize>::max(),
			'\n');

		BattleAction selectedAction = ConvertChoiceToBattleAction(choice);

		if (!CanUseAction(selectedAction))
		{
			std::cout
				<< "This action is currently unavailable.\n";

			continue;
		}
	
		return selectedAction;
	}
}

BattleAction Player::ConvertChoiceToBattleAction(int choice)
{
	switch (choice)
	{
	case 1:
		return BattleAction::Attack;

	case 2:
		return BattleAction::PowerAttack;

	case 3:
		return BattleAction::PoisonAttack;

	case 4:
		return BattleAction::Heal;

	case 5:
		return BattleAction::Guard;

	case 6:
		return BattleAction::StunAttack;

	case 7:
		return BattleAction::AttackBuff;

	case 8:
		return BattleAction::DefenseBuff;

	case 9:
		return BattleAction::Counter;

	case 0:
		return BattleAction::Parry;

	default:
		return BattleAction::None;
	}
}
