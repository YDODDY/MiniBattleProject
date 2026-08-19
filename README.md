# MiniBattle

C++로 제작한 콘솔 기반 턴제 전투 프로젝트

단순한 선공/후공 전투에서 시작하여
Round 기반 전투 구조, Interaction System,
Status Effect, Cooldown, Utility 기반 Enemy AI 까지
단계적으로 확장하며 제작 


## Features

- Round-based Battle System
- Player / Enemy turn order
- Basic Attack / Power Attack
- Poison / Stun Attack
- Heal
- Attack / Defense Buff
- Guard / Counter / Parry Interaction
- Status Effect Lifecycle
- Action Cooldown System
- Utility-based Enemy AI
- Player behavior memory (AIMemory)
- Console battle log / HP display

## Battle Actions

### Direct Attack
- Attack
- PowerAttack
- PoisonAttack
- StunAttack

### Defense / Recovery
- Heal
- Guard

### Buff
- AttackBuff
- DefenseBuff

### Interaction
- Counter
- Parry

## Battle Flow

Each battle consists of repeated Rounds.

1. Create RoundContext
2. Player and Enemy choose their actions
3. Analyze the selected action combination
4. Create RoundResolutionPlan
5. Reveal actions
6. Resolve Basic or Interaction Round
7. Process Status Effects / Cooldowns
8. Check Battle End
9. Update Enemy AIMemory
10. Start the next Round

## Interaction System

Guard, Counter, and Parry are resolved through an
Interaction system rather than normal first/second action order.

The result depends on the combination of the two selected actions.

Examples:

- Guard vs Attack -> Guard Success
- Guard vs PowerAttack -> Guard Break
- Counter vs DirectAttack -> Counter Success
- Counter vs non-attack action -> Counter Failed
- Parry vs DirectAttack -> Parry Success

## Enemy AI

The Enemy uses a Utility-based decision system.

Instead of following a fixed action sequence,
each available action receives a score based on the current
battle state and previous Player behavior.

The AI considers information such as:

- Current HP
- Target HP
- Available actions
- Cooldowns
- Recent Player actions
- Overall Player attack ratio
- Recent Player attack ratio
- Estimated DirectAttack threat
- Previous Counter / Parry results

### AIMemory

Enemy AI stores information from previous Rounds.

Examples:

- Recent Player actions
- Last Player action
- Total Player actions
- DirectAttack count
- Action usage count
- Rounds since an action was last used
- Counter / Parry success and failure history

AIMemory is updated after a Round is resolved
and is used when evaluating actions in the next Round.

## Project Structure

The project separates responsibilities between systems such as:

- Game
  - Battle lifecycle and Round progression

- BattleSystem
  - Action execution
  - Interaction analysis and resolution
  - Action phase processing

- Character / Player / Enemy
  - Character state and actions

- EnemyAI
  - Utility evaluation and action selection

- AIMemory
  - Player behavior history

- ConsoleLogSystem
  - Battle information and event output

## What I Learned

This project was created as a learning project while studying C++.

During development, I practiced:

- C++ classes and object-oriented design
- Header / source file separation
- References and pointers
- STL containers
- `enum` / `struct`
- `unique_ptr`
- Responsibility separation between classes
- Event-based logging
- State and lifecycle management
- Debugging with Visual Studio
- Refactoring legacy code
- Designing and testing edge cases
- Git / GitHub workflow

The battle system was repeatedly redesigned during development,
including a migration from an earlier Turn-based structure
to the current Round-based architecture.

## Known Limitations

- Console-based interface
- Only one Enemy AI type
- No difficulty selection
- Balance is experimental
- AI Utility values are manually tuned
- No save/load system

This project focuses primarily on battle-system architecture
and learning rather than content volume or polished presentation.

## Future

MiniBattle is considered feature-complete for its current learning goal.

Future updates will primarily focus on bug fixes discovered
through external playtesting rather than major feature additions.