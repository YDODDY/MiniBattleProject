#pragma once

#include "EventBus.h"
#include "Event.h"

class ConsoleLogSystem
{
public:
	ConsoleLogSystem(EventBus& eventBus);

private:

	void OnDamaged(const DamagedEvent& event);
	void OnHealed(const HealedEvent& event);
	void OnDead(const DeadEvent& event);
	void OnGuardAttack(const GuardEvent& event);
	void OnMissedAttack(const MissedEvent& event);
	void OnAppliedStatus(const AppliedStatusEvent& event);
	void OnPreventedAction(const ActionPreventedEvent& event);
	void OnDamageOverTime(const DamageOverTimeEvent& event);
	void OnReactedAttack(const ReactionEvent& event);

};