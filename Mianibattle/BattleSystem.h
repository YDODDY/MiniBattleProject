#pragma once

#include "Character.h"
#include "Event.h"
#include "EventBus.h"
#include "AttackData.h"
#include "BattleContextBuilder.h"
#include "StatusActionData.h"
#include "RoundAction.h"
#include <cstring>

// 객체를 실제로 만들려면 (참조X) 크기, 맴버, 생성자 모두 알아야 하기 때문에 전방 선언 만으로는 모른다. 그래서 include 해줘야 함


// Forward Declaration (전방 선언) : 컴파일러에게 '이런 클래스가 어딘가에 존재한다.' 까지만 일단 알려주는 것 
// -> 참조해서 사용해야 할 때는 이거만 있어도 컴파일러가 객체가 존재함 까지는 알기 때문에 전방 선언만 하면 됨
class Character;
class EventBus;

class BattleSystem
{
public:
	BattleSystem(EventBus& inputEventBus) : eventBus(inputEventBus)
	{

	}

	TurnStartResult StartTurn(Character& character);

	void Attack(Character& attacker, Character& target, const AttackData& attackData);

	void Heal(Character& character, int healAmount);

	void ExecuteAction(BattleAction action, Character& actor, Character& target);

	void HandleGuardedAttack(Character& attacker, Character& actor, const AttackData& attackData, bool isHit);

	bool CheckItWasHit(const Character& attacker, const Character& target, const AttackData& data) const;

	bool CheckIsCritical(const Character& attacker) const;

	AttackData MakeAttackData(Character& character, BattleAction action);

	void ApplyAttackResult(Character& attacker, Character& target, int damage, const AttackData& attackData, bool isCritical);

	BattleAction RequestAction(Character& actor, Character& target);

	int CalculateRawDamage(const Character& attacker, const AttackData& data) const;
	int CalculateFinalDamage(int rawDamage, const Character& target) const;
	int ApplyCriticalDamage(int damage, const Character& attacker) const;

	StatusActionData MakeStatusActionData(BattleAction action) const;
	void ApplyStatusAction(Character& actor, Character& opponent, const StatusActionData& data);
	void ApplyStatusEffect(Character& target, StatusType type, int turns, float value);

	bool IsDirectAttack(BattleAction action) const;

	bool ApplyReaction(Character& attacker, Character& target, const AttackData& attackData, bool isHit);
	void ExecuteCounterAttack(Character& counterAttacker, Character& target, float damageMultiplier, BattleAction sourceAction);
	void HandleCounter(Character& attacker, Character& defender, const AttackData& attackData, bool isHit);
	void HandleParry(Character& attacker, Character& defender, const AttackData& attackData, bool isHit);

	void ResolveUnusedReaction(Character& waitingCharacter, BattleAction performedAction);

	void ApplyAttackStatus(Character& attacker, Character& target, const AttackData& attackData);

	void RevealActions(const RoundContext& context);
	ActionPhaseStartResult StartActionPhase(Character& character);
	void EndActionPhase(Character& character);

private:

	EventBus& eventBus;

	BattleContextBuilder builder;

	std::string ToString(const BattleAction& action);

};