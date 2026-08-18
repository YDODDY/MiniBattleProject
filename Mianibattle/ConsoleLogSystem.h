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
	void OnMissedAttack(const MissedEvent& event);
	void OnAppliedStatus(const AppliedStatusEvent& event);
	void OnPreventedAction(const ActionPreventedEvent& event);
	void OnDamageOverTime(const DamageOverTimeEvent& event);
	void OnInteractionSuccess(const InteractEvent& event);
	void OnInteractionFailed(const InteractFailedEvent& event);
	void OnStatusExpired(const StatusExpiredEvent& event);
	void OnGuardFailed(const GuardFailedEvent& event);

public:
	void PrintRoundHeader(int roundNum);
	void PrintHpBar(const Character& character);
	void PrintRoundStatus(const Character& player, const Character& enemy);
	void PrintRoundEndStatus(const Character& player, const Character& enemy);
	void PrintStatusSummary(const Character& character);
};