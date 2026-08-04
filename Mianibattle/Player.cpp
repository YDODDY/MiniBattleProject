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

		switch (choice)
		{
		case 1:
			return BattleAction::Attack;

		case 2:
			if (!CanUseAction(BattleAction::PowerAttack))
			{
				std::cout << "PowerAttack is on cooldown.\n";
				break;
			}
			return BattleAction::PowerAttack;

		case 3:
			if (!CanUseAction(BattleAction::PoisonAttack))
			{
				std::cout << "PoisonAttack is on cooldown.\n";
				break;
			}
			return BattleAction::PoisonAttack;

		case 4:
			if (!CanUseAction(BattleAction::Heal))
			{
				std::cout << "Heal is on cooldown.\n";
				break;
			}
			return BattleAction::Heal;

		case 5:
			if (!CanUseAction(BattleAction::Guard))
			{
				std::cout << "Guard is on cooldown.\n";
				break;
			}
			return BattleAction::Guard;

		case 6:
			if (!CanUseAction(BattleAction::StunAttack))
			{
				std::cout << "StunAttack is on cooldown.\n";
				break;
			}
			return BattleAction::StunAttack;
		case 7:
			if (!CanUseAction(BattleAction::AttackBuff))
			{
				std::cout << "AttackBuff is on cooldown.\n";
				break;
			}
			return BattleAction::AttackBuff;
		case 8:
			if (!CanUseAction(BattleAction::DefenseBuff))
			{
				std::cout << "DefenseBuff is on cooldown.\n";
				break;
			}
			return BattleAction::DefenseBuff;
				
		default:
			std::cout << "Only Mentioned Command is available. \n";
			break;
		}
	}

	return BattleAction::Attack;
}
