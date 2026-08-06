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
    snapshot.status.directAttackLocked = character.HasStatus(StatusType::DirectAttackLocked);
    return snapshot;
}

StatusSnapshot BattleContextBuilder::MakeStatusSnapshot(const Character& character)
{
    StatusSnapshot snapshot;
    const Status& status = character.GetStatus();

    snapshot.poisoned = status.Find(StatusType::Poison) != nullptr;
    snapshot.stunned = status.Find(StatusType::Stun) != nullptr;
    snapshot.attackUp = status.Find(StatusType::AttackUp) != nullptr;
    snapshot.defenseUp = status.Find(StatusType::DefenseUp) != nullptr;

    return snapshot;
}

ActionControl BattleContextBuilder::MakeActionControl(const Character& character)
{
    ActionControl control;

    control.canPowerAttack = character.CanUseAction(BattleAction::PowerAttack);
    control.canPoisonAttack = character.CanUseAction(BattleAction::PoisonAttack);
    control.canStunAttack = character.CanUseAction(BattleAction::StunAttack);   
    control.canHeal = character.CanUseAction(BattleAction::Heal);
    control.canGuard = character.CanUseAction(BattleAction::Guard);
    control.canAttackBuff = character.CanUseAction(BattleAction::AttackBuff);
    control.canDefenseBuff = character.CanUseAction(BattleAction::DefenseBuff);
    control.canCounter = character.CanUseAction(BattleAction::Counter);
    control.canParry = character.CanUseAction(BattleAction::Parry);

    return control;
}
