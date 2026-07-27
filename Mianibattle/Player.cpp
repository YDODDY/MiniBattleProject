#include "Player.h"
#include <iostream>
#include <limits>

Player::Player(const std::string& name, int hp, int maxHp)
	: Character(name, hp, maxHp)
{

}

BattleAction Player::ChooseAction()
{
	while (true)
	{
		int choice = 0;

		std::cout << "1. Attack\n";
		std::cout << "2. PowerAttack\n";
		std::cout << "3. Heal\n";
		std::cout << "4. Guard\n";
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

		if (choice == 2 && !CanUsePowerAttackThisTurn())
		{
			std::cout << "Can not use PowerAttack now! \n";

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

		switch (choice)
		{
		case 1:
			return BattleAction::Attack;
		case 2:
			return BattleAction::PowerAttack;
		case 3:
			return BattleAction::Heal;
		case 4 : 
			return BattleAction::Guard;
		default:
			std::cout << "Only Mentioned Command is available. \n";
			break;
		}
	}

	return BattleAction::Attack;
}
