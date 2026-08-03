#pragma once
#include <iostream>
#include <string>
#include "BattleAction.h"
#include "Status.h"
#include "TurnStartResult.h"
#include "BattleContext.h"

struct ActionCooldown
{
	BattleAction action;
	int remainingTurns;
};

struct CharacterStats
{
	int maxHp = 100;

	int attack = 10;
	int defense = 0;

	float accuracy = 0.90f;
	float evasion = 0.05f;

	float criticalChance = 0.10f;
	float criticalDamageMultiplier = 1.50f;

	int speed = 0;
	int luck = 0;
};

class Character
{

public : 

	Character(
		const std::string& name,
		const CharacterStats& stats)
		: selfHp(stats.maxHp),
		isGuarding(false),
		name(name),
		selfStats(stats)
	{
	
	}

	virtual ~Character() = default;

	virtual BattleAction ChooseAction(const BattleContext& context) = 0;

	const CharacterStats& GetStats() const
	{
		return selfStats;
	}

	TurnStartResult BeginTurn();

	int ReceiveDamage(int damage);
	int Heal(int healAmount);
	void StartGuarding();
	void StopGuarding();

	bool CanUseAction(BattleAction action) const;
	void StartCooldown(BattleAction action, int turns);
	void TickCooldowns();
	int GetRemainingCooldown(BattleAction action) const;

	int GetAttack() const;
	int GetDefense() const;
	float GetAccuracy() const;
	float GetEvasion() const;
	float GetCriticalChance() const;
	float GetCriticalDamageMultiplier() const;


	bool IsDead() const;
	std::string GetName() const;
	int GetHp() const;
	int GetMaxHp() const;
	bool IsGuarding() const;

	StatusApplyResult ApplyStatus(const StatusEffect& effect);
	bool HasStatus(StatusType type) const;
	void RemoveStatus(StatusType type);

	TurnStartResult ProcessTurnStartStatuses();

	const Status& GetStatus() const;

private: 

	int selfHp;
	bool isGuarding = false;

	std::string name;

	Status status;

	std::vector<ActionCooldown> cooldowns;

	CharacterStats selfStats;
};