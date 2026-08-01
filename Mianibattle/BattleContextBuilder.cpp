#include "BattleContextBuilder.h"

BattleContext BattleContextBuilder::Build(const Character& self, const Character& target)
{
    BattleContext context;

    context.self = MakeCharacterSnapshot(self);
    context.target = MakeCharacterSnapshot(target);
    context.actionControl = MakeActionControl(self);

    return context;
}

CharacterSnapshot BattleContextBuilder::MakeCharacterSnapshot(const Character& character)
{
    CharacterSnapshot snapshot;
    snapshot.hp = character.GetHp();
    snapshot.maxHp = character.GetMaxHp();
    snapshot.status = MakeStatusSnapshot(character);

    return snapshot;
}

StatusSnapshot BattleContextBuilder::MakeStatusSnapshot(const Character& character)
{
    StatusSnapshot snapshot;
    const Status& status = character.GetStatus();

    snapshot.poisoned = status.Find(StatusType::Poison) != nullptr;
    snapshot.stunned = status.Find(StatusType::Stun) != nullptr;
    snapshot.burned = status.Find(StatusType::Burn) != nullptr;
    snapshot.frozen = status.Find(StatusType::Freeze) != nullptr;
    snapshot.sleeping = status.Find(StatusType::Sleep) != nullptr;

    return snapshot;
}

ActionControl BattleContextBuilder::MakeActionControl(const Character& character)
{
    ActionControl control;

    control.canPowerAttack = character.CanUsePowerAttackThisTurn();
    control.canGuard = !character.IsGuarding();

    return control;
}
