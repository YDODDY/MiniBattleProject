# Sprint 6

## Added

- AttackData introduced
- Poison Attack implemented
- AppliedStatusEvent added
- BeginTurn poison damage added

## Refactoring

- ReceiveDamage only handles HP
- BattleSystem now publishes events
- Removed duplicated status flag

## Manual Test

- [x] Normal Attack
- [x] Power Attack
- [x] Poison Attack
- [x] Guard
- [x] Heal
- [x] Poison damage at turn start
- [x] Enemy death by poison
- [x] Battle ends correctly